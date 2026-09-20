
/*****************************************************************************
* File Name        : main.c
*
* Description      : This source file contains the main routine for secure
*                    application in the CM33 CPU
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

#include "cy_pdl.h"
#include "cybsp.h"
#include "mtb_pmbus.h"
#include "mtb_hal.h"
#include "cy_retarget_io.h"
#include <stdio.h>

/*******************************************************************************
* Macros
********************************************************************************/
/* These are the addresses where the core0 and core1 images are located. */
#define CORE0_IMAGE_ADDRESS    CYMEM_CM33_0_S_m33s_ppca0_nvm_C_S_START
#define CORE1_IMAGE_ADDRESS    CYMEM_CM33_0_S_m33s_ppca1_nvm_C_S_START
#define PPCA0_IMAGE_SIZE       CYMEM_CM33_0_S_ppca0_code_SIZE
#define PPCA1_IMAGE_SIZE       CYMEM_CM33_0_S_ppca1_code_SIZE

/** [CTRL_PROJECT_DEFINES] */
/* PMBus controller configuration defines */
#define PMBUS_TARGET_ADDRESS        (0x18U)    /* Default PMBus slave address */

/* PMBus command codes for testing */
#define PMBUS_PAGE_CMD_CODE         (0x00U)    /* PAGE command for zone selection */
#define PMBUS_TEST_CMD_1_CODE       (0xD6U)    /* Test command 1 - 4-byte read */
#define PMBUS_TEST_CMD_1_SIZE       (0x4U)
#define PMBUS_TEST_CMD_2_CODE       (0xD7U)    /* Test command 2 - 2-byte read/write */
#define PMBUS_TEST_CMD_2_SIZE       (0x2U)
#define PMBUS_TEST_CMD_3_CODE       (0xC7U)    /* Test command 3 - zone command */
#define PMBUS_TEST_CMD_3_SIZE       (0x4U)

#define PMBUS_TEST_PAGE_NUM         (0x01U)    /* Test page number for multi-page support */
/* 1 second timeout in microseconds */
#define PMBUS_CTRL_TIMEOUT_US       (1000000UL)
/* [CTRL_PROJECT_DEFINES] */


/*******************************************************************************
* Global Variables
********************************************************************************/
/** [PMBUS_UART_GLOBALS] */
/* Debug UART variables */
static cy_stc_scb_uart_context_t    DEBUG_UART_context; /* UART context */
static mtb_hal_uart_t               DEBUG_UART_hal_obj; /* Debug UART HAL object */
/* [PMBUS_UART_GLOBALS] */

/** [PMBUS_HW_CFG_GLOBALS] */
extern mtb_pmbus_ctrl_stc_t pmbus_ctrl_inst;
extern mtb_pmbus_ctrl_cfg_t pmbus_ctrl_cfg;
/* [PMBUS_HW_CFG_GLOBALS] */

/*******************************************************************************
* Function Name: retarget_io_config
*******************************************************************************
* Summary:
*  Configures the debug UART peripheral and retarget-io for serial communication.
*  Initializes the UART with the predefined configuration and enables it.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void retarget_io_config(void)
{
    cy_rslt_t result;
    cy_en_scb_uart_status_t init_status;

    init_status = Cy_SCB_UART_Init(DEBUG_UART_HW, &DEBUG_UART_config, &DEBUG_UART_context);
    if (init_status!=CY_SCB_UART_SUCCESS)
    {
        CY_ASSERT(0);
    }

    Cy_SCB_UART_Enable(DEBUG_UART_HW);

    result = mtb_hal_uart_setup(&DEBUG_UART_hal_obj, &DEBUG_UART_hal_config, &DEBUG_UART_context, NULL);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    result = cy_retarget_io_init(&DEBUG_UART_hal_obj);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("Retarget-io is configured\n\r");
}


/*******************************************************************************
* Function Name: ctrl_pmbus_init_enable
*******************************************************************************
* Summary:
*  Initializes and enables the PMBus controller instance. Sets up the PMBus
*  master with the predefined configuration and enables it for operation.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void ctrl_pmbus_init_enable(void)
{
    /** [CTRL_INIT_ENABLE] */
    mtb_pmbus_ctrl_status_t status;

    /* Initialize PMBus controller */
    status = mtb_pmbus_ctrl_init(&pmbus_ctrl_inst, &PMBUS_CONTROLLER_ctrl_config);

    if (MTB_PMBUS_CTRL_STATUS_SUCCESS != status)
    {
        /* Handle the error status */
        printf("PMBus Controller initialization failed! Status: %d\n\r", status);
        CY_ASSERT(0);
    }
    else
    {
        printf("PMBus Controller initialized successfully\n\r");

        /* Enable the controller */
        mtb_pmbus_ctrl_enable(&pmbus_ctrl_inst);
        printf("PMBus Controller enabled\n\r");
    }
    /* [CTRL_INIT_ENABLE] */
}

/*******************************************************************************
* Function Name: controller_protocol_transfer
*******************************************************************************
* Summary:
*  Demonstrates various PMBus controller command examples including:
*  - Quick Command: minimal command for device control
*  - Read 32 Protocol: reading 4-byte data from slave devices
*  - Generic Transfer API: PAGE command, Write/Read Word operations
*  Each example includes proper status handling and timeouts.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void controller_protocol_transfer(void)
{
    /** [CTRL_QUICK_CMD_EXAMPLE] */
    mtb_pmbus_ctrl_status_t status;

    printf("\n\r");
    printf("====================================\n\r");
    printf("Starting PMBus Controller Examples\n\r");
    printf("====================================\n\r");

    /* Wait a bit before starting transfers */
    Cy_SysLib_Delay(1000U);

    /* Example 1: Send Quick Command to toggle LED on target device */
    printf("\n\r--- Example 1: Quick Command ---\n\r");
    printf("Sending Quick Command to address 0x%02X (toggles target LED)\n\r", PMBUS_TARGET_ADDRESS);

    status = mtb_pmbus_ctrl_ex_quick_cmd(&pmbus_ctrl_inst, PMBUS_TARGET_ADDRESS);

    if (status == MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Wait for transfer completion (1 second timeout) */
        status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);

        if (status == MTB_PMBUS_CTRL_STATUS_IS_READY)
        {
            printf("Quick Command sent successfully\n\r");
        }
        else if (status == MTB_PMBUS_CTRL_STATUS_TIMEOUT)
        {
            printf("Quick Command timeout\n\r");
        }
        else
        {
            printf("Quick Command failed with status: %d\n\r", status);
        }
    }
    else
    {
        printf("Quick Command API failed with status: %d\n\r", status);
    }

    /* A short delay before next example */
    Cy_SysLib_Delay(500U);
    /* [CTRL_QUICK_CMD_EXAMPLE] */


    /** [CTRL_READ_32_EXAMPLE] */
    /* Example 2: Read 32-bit data using Read 32 protocol */
    printf("\n\r--- Example 2: Read 32 Protocol ---\n\r");
    printf("Reading 4 bytes from CMD1 (0x%02X) - LED toggle counter\n\r", PMBUS_TEST_CMD_1_CODE);
    uint8_t read32_buffer[4U] = {0U};
    status = mtb_pmbus_ctrl_ex_read_32(&pmbus_ctrl_inst, PMBUS_TARGET_ADDRESS, 
                                        PMBUS_TEST_CMD_1_CODE, read32_buffer, false);

    if (status == MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Wait for transfer completion (1 second timeout) */
        status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);

        if (status == MTB_PMBUS_CTRL_STATUS_IS_READY)
        {
            uint32_t counter_value = read32_buffer[0U] | (read32_buffer[1U] << 8) |
                                     (read32_buffer[2U] << 16) | (read32_buffer[3U] << 24);
            printf("Read 32 completed successfully\n\r");
            printf("Counter value: %lu\n\r", (unsigned long)counter_value);
        }
        else if (status == MTB_PMBUS_CTRL_STATUS_TIMEOUT)
        {
            printf("Read 32 timeout\n\r");
        }
        else
        {
            printf("Read 32 failed with status: %d\n\r", status);
        }
    }
    else
    {
        printf("Read 32 API failed with status: %d\n\r", status);
    }

    /* A short delay before next example */
    Cy_SysLib_Delay(500U);
    /* [CTRL_READ_32_EXAMPLE] */

    /** [CTRL_GENERIC_TRANSFER_PAGE] */
    /* Example 3: Use generic transfer API for PAGE command and Write/Read Word */
    printf("\n\r--- Example 3: Generic Transfer API ---\n\r");

    /* Step 3a: Send PAGE command using Write Byte protocol */
    printf("Setting PAGE to %d using generic transfer\n\r", PMBUS_TEST_PAGE_NUM);

    mtb_pmbus_ctrl_stc_transfer_cfg_t transfer_cfg;
    uint8_t page_data[2U];
    page_data[0U] = PMBUS_PAGE_CMD_CODE;  /* PAGE command */
    page_data[1U] = PMBUS_TEST_PAGE_NUM;  /* Page number */

    transfer_cfg.addr = PMBUS_TARGET_ADDRESS;
    transfer_cfg.data = page_data;
    transfer_cfg.wr_size = 2U;  /* Command code + data byte */
    transfer_cfg.rd_size = 0U;  /* No read */
    transfer_cfg.execute_stop = true;

    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &transfer_cfg);

    if (status == MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Wait for transfer completion (1 second timeout) */
        status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);

        if (status == MTB_PMBUS_CTRL_STATUS_IS_READY)
        {
            printf("PAGE command sent successfully\n\r");
        }
        else if (status == MTB_PMBUS_CTRL_STATUS_TIMEOUT)
        {
            printf("PAGE command timeout\n\r");
        }
        else
        {
            printf("PAGE command failed with status: %d\n\r", status);
        }
    }
    else
    {
        printf("PAGE command API failed with status: %d\n\r", status);
    }

    /* A short delay before next step */
    Cy_SysLib_Delay(500U);
    /* [CTRL_GENERIC_TRANSFER_PAGE] */

    /** [CTRL_GENERIC_TRANSFER_WRITE] */
    /* Step 3b: Write Word to CMD2 on selected page */
    printf("\n\rWriting 0xABCD to CMD2 (0x%02X) using generic transfer\n\r", PMBUS_TEST_CMD_2_CODE);

    uint8_t write_data[3U];
    write_data[0U] = PMBUS_TEST_CMD_2_CODE;  /* CMD2 command */
    write_data[1U] = 0xABU;
    write_data[2U] = 0xCDU;

    transfer_cfg.data = write_data;
    transfer_cfg.wr_size = 3U;  /* Command code + 2 data bytes */
    transfer_cfg.rd_size = 0U;

    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &transfer_cfg);

    if (status == MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Wait for transfer completion (1 second timeout) */
        status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);

        if (status == MTB_PMBUS_CTRL_STATUS_IS_READY)
        {
            printf("Write Word completed successfully\n\r");
        }
        else if (status == MTB_PMBUS_CTRL_STATUS_TIMEOUT)
        {
            printf("Write Word timeout\n\r");
        }
        else
        {
            printf("Write Word failed with status: %d\n\r", status);
        }
    }
    else
    {
        printf("Write Word API failed with status: %d\n\r", status);
    }

    /* A short delay before next step */
    Cy_SysLib_Delay(500U);
    /* [CTRL_GENERIC_TRANSFER_WRITE] */


    /** [CTRL_GENERIC_TRANSFER_READ] */
    /* Step 3c: Read Word from CMD2 on selected page */
    printf("\n\rReading from CMD2 (0x%02X) using generic transfer\n\r", PMBUS_TEST_CMD_2_CODE);

    uint8_t read_data[3U];
    read_data[0U] = PMBUS_TEST_CMD_2_CODE;  /* CMD2 command */

    transfer_cfg.data = read_data;
    transfer_cfg.wr_size = 1U;  /* Command code only */
    transfer_cfg.rd_size = 2U;  /* Read 2 bytes */

    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &transfer_cfg);

    if (status == MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Wait for transfer completion (1 second timeout) */
        status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);

        if (status == MTB_PMBUS_CTRL_STATUS_IS_READY)
        {
            printf("Read Word completed successfully\n\r");
            uint16_t word_value = read_data[1U] | (read_data[0U] << 8);
            printf("Word value: 0x%04X\n\r", word_value);
        }
        else if (status == MTB_PMBUS_CTRL_STATUS_TIMEOUT)
        {
            printf("Read Word timeout\n\r");
        }
        else
        {
            printf("Read Word failed with status: %d\n\r", status);
        }
    }
    else
    {
        printf("Read Word API failed with status: %d\n\r", status);
    }

    printf("\n\r====================================\n\r");
    printf("Examples completed\n\r");
    printf("====================================\n\r");
    /* [CTRL_GENERIC_TRANSFER_READ] */
}

/*******************************************************************************
* Function Name: controller_zone_transfer
*******************************************************************************
* Summary:
*  Demonstrates PMBus Zone Read protocol implementation. Configures multiple
*  zones, assigns pages to zones, performs zone write and zone read operations.
*  This example shows the complex multi-transfer zone protocol with proper
*  START/RESTART/STOP condition handling.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void controller_zone_transfer(void)
{
    /** [CTRL_ZONE_READ_TRANSFER] */
    mtb_pmbus_ctrl_status_t status;

    /* Addresses to be used in the zone read example */
    uint8_t target_address = 0x18U;
    uint8_t zone_wr_address = 0x37U;
    uint8_t zone_rd_address = 0x28U;

    /* Buffers to commands used in zone read example */
    uint8_t write_buffer_page[2U];
    uint8_t write_buffer_zone_config[3U];
    uint8_t write_buffer_zone_active[3U];
    uint8_t write_buffer_zone_write[5U];
    uint8_t write_buffer_zone_read[2U];
    uint8_t read_buffer_zone_read[4U];
    
    /* Controller configuration structure to read/write transfer */
    mtb_pmbus_ctrl_stc_transfer_cfg_t write_cfg;
    mtb_pmbus_ctrl_stc_transfer_cfg_t read_cfg;

    printf("\n\r");
    printf("====================================\n\r");
    printf("Starting PMBus Controller Zone Examples\n\r");
    printf("====================================\n\r");
    printf("\n\r");

    /* Step 1: Set pages and assigne them to zone */
    write_buffer_page[0] = 0x00U;  /* Page Command code */
    write_buffer_page[1] = 0x00U;  /* Page number */
    
    write_cfg.data = write_buffer_page;
    write_cfg.addr = target_address;
    write_cfg.wr_size = 2U;           /* Write 2 bytes: PAGE cmd and PAGE number */
    write_cfg.rd_size = 0U;           /* No read in this phase */
    write_cfg.execute_stop = true;    /* Execute STOP condition */

    /* Set active page number 0 */
    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &write_cfg);
    
    if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Handle error - unable to start write phase */
        return;
    }

    status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);
    
    if (status != MTB_PMBUS_CTRL_STATUS_IS_READY)
    {
        /* Handle error - write phase failed */
        return;
    }

    Cy_SysLib_Delay(500);

    write_buffer_zone_config[0] = 0x07U;  /* Zone Command code */
    write_buffer_zone_config[1] = 0x01U;  /* Zone Config: Write zone number */
    write_buffer_zone_config[2] = 0x02U;  /* Zone Config: Read zone number */
    

    write_cfg.data = write_buffer_zone_config;
    write_cfg.addr = target_address;
    write_cfg.wr_size = 3U;            /* Write 3 bytes: Zone Config cmd and data */
    write_cfg.rd_size = 0U;            /* No read in this phase */
    write_cfg.execute_stop = true;     /* Execute STOP condition */

    /* Assign page 0 to write zone 1 and read zone 2 */
    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &write_cfg);
    
    if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Handle error - unable to start write phase */
        return;
    }

    status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);
    
    if (status != MTB_PMBUS_CTRL_STATUS_IS_READY)
    {
        /* Handle error - write phase failed */
        return;
    }

    printf("Assigned page 0 to Write zone 1 and Read zone 2\r\n");
    Cy_SysLib_Delay(500);

    write_buffer_page[0] = 0x00U;  /* Page Command code */
    write_buffer_page[1] = 0x01U;  /* Page number */
    
    write_cfg.data = write_buffer_page;
    write_cfg.addr = target_address;
    write_cfg.wr_size = 2U;           /* Write 2 bytes: PAGE cmd and PAGE number */
    write_cfg.rd_size = 0U;           /* No read in this phase */
    write_cfg.execute_stop = true;    /* Execute STOP condition */

    /* Set active page number 1 */
    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &write_cfg);
    
    if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Handle error - unable to start write phase */
        return;
    }

    status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);
    
    if (status != MTB_PMBUS_CTRL_STATUS_IS_READY)
    {
        /* Handle error - write phase failed */
        return;
    }

    Cy_SysLib_Delay(500);

    write_buffer_zone_config[0] = 0x07U;  /* Zone Config cmd */
    write_buffer_zone_config[1] = 0x01U;  /* Zone Config: Write zone number */
    write_buffer_zone_config[2] = 0x02U;  /* Zone Config: Read zone number */
    
    write_cfg.data = write_buffer_zone_config;
    write_cfg.addr = target_address;
    write_cfg.wr_size = 3U;           /* Write 3 bytes: Zone Config cmd and data */
    write_cfg.rd_size = 0U;           /* No read in this phase */
    write_cfg.execute_stop = true;    /* Execute STOP condition */

    /* Assign page 1 to write zone 1 and read zone 2 */
    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &write_cfg);
    
    if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Handle error - unable to start write phase */
        return;
    }

    status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);
    
    if (status != MTB_PMBUS_CTRL_STATUS_IS_READY)
    {
        /* Handle error - write phase failed */
        return;
    }

    printf("Assigned page 1 to write zone 1 and read zone 2\r\n");
    Cy_SysLib_Delay(500);

    /* Step 2: Set active zone */
    write_buffer_zone_active[0] = 0x08U;  /* Zone Active cmd */
    write_buffer_zone_active[1] = 0x01U;  /* Zone Active: Write zone number */
    write_buffer_zone_active[2] = 0x02U;  /* Zone Active: Read zone number */

    write_cfg.data = write_buffer_zone_active;
    write_cfg.addr = zone_wr_address;
    write_cfg.wr_size = 3U;           /* Write 3 bytes: Zone Active cmd and data */
    write_cfg.rd_size = 0U;           /* No read in this phase */
    write_cfg.execute_stop = true;    /* Execute STOP condition */

    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &write_cfg);
    
    if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Handle error - unable to start write phase */
        return;
    }

    status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);
    
    if (status != MTB_PMBUS_CTRL_STATUS_IS_READY)
    {
        /* Handle error - write phase failed */
        return;
    }

    printf("====================================\n\r");
    printf("Zone configuration complete\n\r");
    printf("====================================\n\r");
    printf("\r\n");
    Cy_SysLib_Delay(500);

    /* Step 3: Send Zone Write */
    write_buffer_zone_write[0] = 0xC7U;  /* MRF specific cmd C7 */
    write_buffer_zone_write[1] = 0xBBU;  /* Data byte 1 */
    write_buffer_zone_write[2] = 0xCCU;  /* Data byte 2 */
    write_buffer_zone_write[3] = 0xDDU;  /* Data byte 3 */
    write_buffer_zone_write[4] = 0xEEU;  /* Data byte 4 */

    write_cfg.data = write_buffer_zone_write;
    write_cfg.addr = zone_wr_address;
    write_cfg.wr_size = 5U;           /* Write 3 bytes: MRF specific cmd C7 and data */
    write_cfg.rd_size = 0U;           /* No read in this phase */
    write_cfg.execute_stop = true;    /* Execute STOP condition */

    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &write_cfg);
    
    if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Handle error - unable to start write phase */
        return;
    }

    status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);
    
    if (status != MTB_PMBUS_CTRL_STATUS_IS_READY) 
    {
        /* Handle error - write phase failed */
        return;
    }

    printf("====================================\n\r");
    printf("Zone write complete\n\r");
    printf("====================================\n\r");
    printf("\r\n");
    Cy_SysLib_Delay(500);

    /* At this point of execution of snippet there were load into target command buffer
       0xBB, 0xCC as data. This data available to read through Zone Read command. */

    /* Step 4: Send Zone Read */
    write_buffer_zone_read[0] = 0x00U;  /* Zone Read cmd byte mask */
    write_buffer_zone_read[1] = 0xC7U;  /* MRF specific cmd C7 */

    write_cfg.data = write_buffer_zone_read;
    write_cfg.addr = zone_rd_address;
    write_cfg.wr_size = 2U;           /* Write 2 bytes: Zone Read cmd mask and cmd code */
    write_cfg.rd_size = 0U;           /* No read in this phase */
    write_cfg.execute_stop = false;   /* Not Execute STOP condition. This transfer starts
                                         without a STOP condition because ZONE_READ is a
                                         complex protocol that consists of several transfers
                                         separated by Restart events. */

    /* Start Zone Read transfer: Write part */
    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &write_cfg);
    
    if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Handle error - unable to start write phase */
        return;
    }

    status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);
    
    if (status != MTB_PMBUS_CTRL_STATUS_IS_READY)
    {
        /* Handle error - write phase failed */
        return;
    }

    read_cfg.data = read_buffer_zone_read;
    read_cfg.addr = zone_rd_address;
    read_cfg.wr_size = 0U;           /* Write 4 bytes: Zone Read cmd mask and data */
    read_cfg.rd_size = 4U;           /* No read in this phase */
    read_cfg.execute_stop = false;   /* Not Execute STOP condition */

    /* Read Page 0 from read zone 2 */
    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &read_cfg);
    
    if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Handle error - unable to start write phase */
        return;
    }

    status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);
    
    if (status != MTB_PMBUS_CTRL_STATUS_IS_READY)
    {
        /* Handle error - write phase failed */
        return;
    }
    printf("Zone Read: 0x%02X 0x%02X 0x%02X 0x%02X\n\r", read_buffer_zone_read[0], read_buffer_zone_read[1], read_buffer_zone_read[2], read_buffer_zone_read[3]);

    read_cfg.data = read_buffer_zone_read;
    read_cfg.addr = zone_rd_address;
    read_cfg.wr_size = 0U;           /* Write 2 bytes: Zone Read cmd mask and data */
    read_cfg.rd_size = 4U;           /* No read in this phase */
    read_cfg.execute_stop = true;    /* Execute STOP condition. There is no more page
                                        to read in zone read 2. */

    /* Read Page 1 from read zone 2 */
    status = mtb_pmbus_ctrl_execute_transfer(&pmbus_ctrl_inst, &read_cfg);
    
    if (status != MTB_PMBUS_CTRL_STATUS_SUCCESS)
    {
        /* Handle error - unable to start write phase */
        return;
    }

    status = mtb_pmbus_ctrl_wait_cmpl(&pmbus_ctrl_inst, PMBUS_CTRL_TIMEOUT_US);
    
    if (status != MTB_PMBUS_CTRL_STATUS_IS_READY)
    {
        /* Handle error - write phase failed */
        return;
    }
    printf("Zone Read: 0x%02X 0x%02X 0x%02X 0x%02X\n\r", read_buffer_zone_read[0], read_buffer_zone_read[1], read_buffer_zone_read[2], read_buffer_zone_read[3]);
    /* [CTRL_ZONE_READ_TRANSFER] */

    printf("====================================\n\r");
    printf("Zone Read complete\n\r");
    printf("====================================\n\r");
    printf("\r\n");
}

/*******************************************************************************
* Function Name: main
*******************************************************************************
* Summary:
*  Main entry point for the PMBus Controller example application.
*  Performs the following operations:
*    1. Initializes the MCU and board peripherals
*    2. Initializes secondary processor cores (PPCA0 and PPCA1)
*    3. Configures debug UART for serial communication
*    4. Initializes and enables the PMBus controller
*    5. Runs PMBus controller examples in an infinite loop
*
* Parameters:
*  void
*
* Return:
*  int (does not return)
*
*******************************************************************************/
int main(void)
{
    cy_rslt_t result;
    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* enable interrupts */
    __enable_irq();

    Cy_System_Init_CPU0((void*)CORE0_IMAGE_ADDRESS, PPCA0_IMAGE_SIZE);
    Cy_System_Init_CPU1((void*)CORE1_IMAGE_ADDRESS, PPCA1_IMAGE_SIZE);

    /** [PMBUS_UART_SETUP] */
    retarget_io_config();

    /* Transmit header to the terminal */
    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

    printf("************************************************************\r\n");
    printf("PSOC Control C3M/P8: PMBus Controller\r\n");
    printf("************************************************************\r\n\n");
    /* [PMBUS_UART_SETUP] */

    /** [PMBUS_ENABLE_IRQ] */
    /* Enable global interrupts */
    __enable_irq();
    /* [PMBUS_ENABLE_IRQ] */

     ctrl_pmbus_init_enable();

    for (;;)
    {
        controller_protocol_transfer();
        controller_zone_transfer();
        Cy_SysLib_Delay(1000);
    }
    /* [PMBUS_INIT_ENABLE] */
}
