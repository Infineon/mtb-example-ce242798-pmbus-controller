
/*****************************************************************************
* File Name        : pmbus_hw_cfg.c
*
* Description      : This source file contains the hardware configuration for
*                    the PMBus Middleware.
*
* Related Document : See README.md
*
********************************************************************************
* (c) 2026, Infineon Technologies AG, or an affiliate of Infineon
* Technologies AG. All rights reserved.
* This software, associated documentation and materials ("Software") is
* owned by Infineon Technologies AG or one of its affiliates ("Infineon")
* and is protected by and subject to worldwide patent protection, worldwide
* copyright laws, and international treaty provisions. Therefore, you may use
* this Software only as provided in the license agreement accompanying the
* software package from which you obtained this Software. If no license
* agreement applies, then any use, reproduction, modification, translation, or
* compilation of this Software is prohibited without the express written
* permission of Infineon.
*
* Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE
* IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
* INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF
* THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A
* SPECIFIC USE/PURPOSE OR MERCHANTABILITY.
* Infineon reserves the right to make changes to the Software without notice.
* You are responsible for properly designing, programming, and testing the
* functionality and safety of your intended application of the Software, as
* well as complying with any legal requirements related to its use. Infineon
* does not guarantee that the Software will be free from intrusion, data theft
* or loss, or other breaches ("Security Breaches"), and Infineon shall have
* no liability arising out of any Security Breaches. Unless otherwise
* explicitly approved by Infineon, the Software may not be used in any
* application where a failure of the Product or any consequences of the use
* thereof can reasonably be expected to result in personal injury.
*******************************************************************************/
#include "mtb_pmbus.h"
#include "cy_pdl.h"
#include "cybsp.h"


/******************************************************************************
 * I2C Hardware Configuration
 ******************************************************************************/

mtb_pmbus_ctrl_events_t pmbus_controller_callback_events = MTB_PMBUS_CTRL_TRANSFER_DONE;

/** [PMBUS_HW_CFG] */
static cy_stc_scb_i2c_context_t i2c_ctrl_pdl_context;

mtb_pmbus_ctrl_stc_t pmbus_ctrl_inst;

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))

/** [PMBUS_TIMEOUT_TIMER_ISR_CFG_CTRL] */
void ctrl_timer_isr(void)
{
    mtb_pmbus_ctrl_timer_isr(&pmbus_inst);
}
/** [PMBUS_TIMEOUT_TIMER_ISR_CFG_CTRL] */

void init_tcpwm(void)
{
/** [PMBUS_TIMEOUT_TIMER_CFG_CTRL] */
    cy_en_tcpwm_status_t pdl_tcpwm_status = Cy_TCPWM_Counter_Init(PMBUS_TIMEOUT_HW, PMBUS_TIMEOUT_NUM, &PMBUS_TIMEOUT_config);
    if (CY_TCPWM_SUCCESS != pdl_tcpwm_status)
    {
        MTB_PMBUS_LOG_ERR("Error during TCPWM PDL initialization. Status: %X", pdl_tcpwm_status);
    }
    else
    {
        cy_stc_sysint_t ctrl_timer_isr_cfg =
        {
            .intrSrc = PMBUS_TIMEOUT_IRQ,
            .intrPriority = 3U
        };

        cy_en_sysint_status_t pld_interrupt_status = Cy_SysInt_Init(&ctrl_timer_isr_cfg, ctrl_timer_isr);
        if (CY_SYSINT_SUCCESS != pld_interrupt_status)
        {
            MTB_PMBUS_LOG_ERR("Error during Timer Interrupt initialization. Status: %X", pld_interrupt_status);
        }
        else
        {
            MTB_PMBUS_LOG_INF("Timer is initialized");
        }
    }
/** [PMBUS_TIMEOUT_TIMER_CFG_CTRL] */
}
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */

#if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U))
void ctrl_init_tgs(void)
{
/** [PMBUS_TIMEOUT_TGS_CFG] */
    cy_en_scb_tgs_status_t pdl_tgs_status;

    pdl_tgs_status = Cy_SCB_TGSx_Init(PMBUS_CONTROLLER_I2C_HW, MTB_PMBUS_TGS_TIMER_NUM, &PMBUS_CONTROLLER_I2C_tgs_config);
    if (CY_SCB_TGS_SUCCESS != pdl_tgs_status)
    {
        MTB_PMBUS_LOG_ERR("Error during TGS initialization. Status: %X", pdl_tgs_status);
    }
    else
    {
        Cy_SCB_TGSx_Enable(PMBUS_CONTROLLER_I2C_HW, MTB_PMBUS_TGS_TIMER_NUM);
        MTB_PMBUS_LOG_INF("TGS is initialized");
    }
/** [PMBUS_TIMEOUT_TGS_CFG] */
}
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */

/*******************************************************************************
* Function Name: ctrl_i2c_isr
*******************************************************************************
* Summary:
*  Interrupt service routine for PMBus I2C controller. Handles all I2C-related
*  events and delegates to the PMBus controller ISR for processing.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void ctrl_i2c_isr(void)
{
    mtb_pmbus_ctrl_isr(&pmbus_ctrl_inst);
}

/*******************************************************************************
* Function Name: PMBUS_CONTROLLER_ctrl_hw_resource_ctrl
*******************************************************************************
* Summary:
*  Hardware resource callback for PMBus controller. Handles initialization,
*  enabling, and disabling of hardware resources (I2C, interrupts, and timers)
*  based on the requested action.
*
* Parameters:
*  event - Hardware action requested (INIT, ENABLE, or DISABLE)
*
* Return:
*  void
*
*******************************************************************************/
void PMBUS_CONTROLLER_ctrl_hw_resource_ctrl(mtb_pmbus_ctrl_hw_resources_ctrl_action_t event)
{
    switch (event)
    {
        case MTB_PMBUS_CTRL_HW_RESOURCES_INIT:
        {
            cy_en_scb_i2c_status_t pdl_i2c_status;
            cy_en_sysint_status_t  pld_interrupt_status;

            pdl_i2c_status = Cy_SCB_I2C_Init(PMBUS_CONTROLLER_I2C_HW, &PMBUS_CONTROLLER_I2C_config, &i2c_ctrl_pdl_context);
            if (CY_SCB_I2C_SUCCESS != pdl_i2c_status)
            {
                MTB_PMBUS_LOG_ERR("Error during I2C PDL initialization. Status: %X", pdl_i2c_status);
            }
            else
            {
                cy_stc_sysint_t ctrl_i2c_isr_cfg =
                {
                    .intrSrc = PMBUS_CONTROLLER_I2C_IRQ,
                    .intrPriority = 3U
                };

                pld_interrupt_status = Cy_SysInt_Init(&ctrl_i2c_isr_cfg, ctrl_i2c_isr);
                if (CY_SYSINT_SUCCESS != pld_interrupt_status)
                {
                    MTB_PMBUS_LOG_ERR("Error during I2C Interrupt initialization. Status: %X", pld_interrupt_status);
                }
                else
                {
                    MTB_PMBUS_LOG_INF("I2C transport is initialized");
                }
            }

#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
            init_tcpwm();
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U))
            ctrl_init_tgs();
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TGS) && (MTB_PMBUS_HAL_USE_TGS == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
            break;
        }
        case MTB_PMBUS_CTRL_HW_RESOURCES_ENABLE:
        {
            Cy_SCB_I2C_Enable(PMBUS_CONTROLLER_I2C_HW);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
MTB_PMBUS_LOG_INF("Enable Timer");
/** [PMBUS_TIMEOUT_TIMER_EN_CALLBACK_CTRL] */
            Cy_TCPWM_Counter_Enable(PMBUS_TIMEOUT_HW, PMBUS_TIMEOUT_NUM);
/** [PMBUS_TIMEOUT_TIMER_EN_CALLBACK_CTRL] */
            #endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
            break;
        }
        case MTB_PMBUS_CTRL_HW_RESOURCES_DISABLE:
        {
            Cy_SCB_I2C_Disable(PMBUS_CONTROLLER_I2C_HW, &i2c_ctrl_pdl_context);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
/** [PMBUS_TIMEOUT_TIMER_DIS_CALLBACK_CTRL] */
            Cy_TCPWM_Counter_Disable(PMBUS_TIMEOUT_HW, PMBUS_TIMEOUT_NUM);
/** [PMBUS_TIMEOUT_TIMER_DIS_CALLBACK_CTRL] */
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
            break;
        }
        default:
            break;
    }
}

/*******************************************************************************
* Function Name: PMBUS_CONTROLLER_ctrl_events_ctrl
*******************************************************************************
* Summary:
*  Event callback for PMBus controller. Handles all PMBus-related events including
*  successful transfers, errors (data corruption, NACK, timeouts, arbitration loss),
*  and other status conditions. Provides error logging and visual feedback.
*
* Parameters:
*  event - PMBus event type to be processed
*
* Return:
*  void
*
*******************************************************************************/
void PMBUS_CONTROLLER_ctrl_events_ctrl(mtb_pmbus_ctrl_events_t event)
{
    if (event == MTB_PMBUS_CTRL_TRANSFER_DONE)
    {
        MTB_PMBUS_LOG_DBG("TRANSFER DONE event");
    }
    else if (event == MTB_PMBUS_CTRL_CORRUPTED_DATA)
    {
        MTB_PMBUS_LOG_ERR("CORRUPTED DATA event - incorrect PEC received");
    }
    else if (event == MTB_PMBUS_CTRL_TARGET_NACK_ADDR)
    {
        MTB_PMBUS_LOG_ERR("TARGET NACK ADDR event - target sends NACK on address");
    }
    else if (event == MTB_PMBUS_CTRL_BUS_ERR)
    {
        MTB_PMBUS_LOG_ERR("BUS ERROR event - bus error on I2C bus");
    }
    else if (event == MTB_PMBUS_CTRL_TIMEOUT)
    {
        Cy_GPIO_Inv(CYBSP_USER_LED_PORT, CYBSP_USER_LED_PIN);
        Cy_GPIO_Inv(TIMEOUT_INT_PORT, TIMEOUT_INT_PIN);
        MTB_PMBUS_LOG_ERR("TIMEOUT event - timeout detected");
    }
    else if (event == MTB_PMBUS_CTRL_TARGET_NACK_BYTE)
    {
        MTB_PMBUS_LOG_ERR("TARGET NACK BYTE event - target sends NACK on data byte");
    }
    else if (event == MTB_PMBUS_CTRL_TARGET_NACK_CMD)
    {
        MTB_PMBUS_LOG_ERR("TARGET NACK CMD event - target sends NACK on command code");
    }
    else if (event == MTB_PMBUS_CTRL_ARB_LOST)
    {
        MTB_PMBUS_LOG_ERR("ARB LOST event - controller arbitration lost");
    }
    else if (event == MTB_PMBUS_CTRL_ABORT_START)
    {
        MTB_PMBUS_LOG_ERR("ABORT START event - controller abort start");
    }
    else if (event == MTB_PMBUS_CTRL_BLOCK_COUNT_TOO_BIG)
    {
        MTB_PMBUS_LOG_ERR("BLOCK COUNT ERROR event - received block count is too big");
    }
    else
    {
        MTB_PMBUS_LOG_ERR("Unknown event: %d", event);
    }
    pmbus_controller_callback_events = event;
    MTB_PMBUS_LOG_DBG("Event: %d", pmbus_controller_callback_events);
}

#if (defined(MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U))
/*******************************************************************************
* Function Name: pmbus_ctrl_hnp_callback
*******************************************************************************
* Summary:
*  Host Notification Protocol (HNP) callback for PMBus controller. Handles
*  host notification events including completion, request reception, and errors.
*  Provides logging of received addresses and data.
*
* Parameters:
*  event - Host notification event type
*
* Return:
*  void
*
*******************************************************************************/
void pmbus_ctrl_hnp_callback(mtb_pmbus_ctrl_host_notify_events_t event)
{
    if (event == MTB_PMBUS_CTRL_HOST_NOTIFY_COMPLETE)
    {
        MTB_PMBUS_LOG_DBG("HOST NOTIFY COMPLETE: Addr: 0x%02X Data: 0x%04X",
                          mtb_pmbus_ctrl_hnp_get_trgt_addr(&pmbus_ctrl_inst),
                          mtb_pmbus_ctrl_hnp_get_trgt_data(&pmbus_ctrl_inst));
    }
    else if (event == MTB_PMBUS_CTRL_HOST_NOTIFY_REQUEST_RECEIVED)
    {
        MTB_PMBUS_LOG_DBG("HOST NOTIFY REQUEST RECEIVED event");
    }
    else if (event == MTB_PMBUS_CTRL_HOST_NOTIFY_ERROR)
    {
        MTB_PMBUS_LOG_ERR("HOST NOTIFY ERROR event");
    }
    else
    {
        MTB_PMBUS_LOG_ERR("Unknown HNP event: %d", event);
    }
}
#endif /* #if (defined(MTB_PMBUS_SUPPORT_HOST_NOTIFY) && (MTB_PMBUS_SUPPORT_HOST_NOTIFY != 0U)) */

/** [PMBUS_TIMEOUT_TIMER_INT_CTRL_CTRL] */
/*******************************************************************************
* Function Name: PMBUS_CONTROLLER_ctrl_hw_irq_enable
*******************************************************************************
* Summary:
*  Enables the I2C controller interrupt and timeout timer interrupt (if enabled).
*  Called by the PMBus middleware when resources are initialized.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void PMBUS_CONTROLLER_ctrl_hw_irq_enable(void)
{
    NVIC_EnableIRQ((IRQn_Type) PMBUS_CONTROLLER_I2C_IRQ);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    NVIC_EnableIRQ((IRQn_Type) PMBUS_TIMEOUT_IRQ);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
}
 
/*******************************************************************************
* Function Name: PMBUS_CONTROLLER_ctrl_hw_irq_disable
*******************************************************************************
* Summary:
*  Disables the I2C controller interrupt and timeout timer interrupt (if enabled).
*  Called by the PMBus middleware when resources are being disabled.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void PMBUS_CONTROLLER_ctrl_hw_irq_disable(void)
{
    NVIC_DisableIRQ((IRQn_Type) PMBUS_CONTROLLER_I2C_IRQ);
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    NVIC_DisableIRQ((IRQn_Type) PMBUS_TIMEOUT_IRQ);
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
}
/** [PMBUS_TIMEOUT_TIMER_INT_CTRL_CTRL] */

/** [PMBUS_TIMEOUT_TIMER_HAL_OBJ_CTRL] */
mtb_pmbus_ctrl_stc_config_hal_t PMBUS_CONTROLLER_ctrl_hal_config =
{
    .hw_ptr = PMBUS_CONTROLLER_I2C_HW,
    .pdl_i2c_context = &i2c_ctrl_pdl_context,
#if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U))
#if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U))
    .timeout_tcpwm_base = PMBUS_TIMEOUT_HW,
    .timeout_tcpwm_cntnum = PMBUS_TIMEOUT_NUM,
#endif /* #if (defined (MTB_PMBUS_HAL_USE_TCPWM) && (MTB_PMBUS_HAL_USE_TCPWM == 1U)) */
#endif /* #if (defined (MTB_PMBUS_ENABLE_TIMEOUT) && (MTB_PMBUS_ENABLE_TIMEOUT == 1U)) */
};
/** [PMBUS_TIMEOUT_TIMER_HAL_OBJ_CTRL] */