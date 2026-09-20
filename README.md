# PSOC&trade; Control C3M/P8: PMBus&reg; Controller

This code example demonstrates how to get started with the PMBus&reg; controller/master component. The project implements a PMBus&reg; (Power Management Bus) controller/master that communicates with PMBus&reg; slave devices using the I2C protocol. It showcases how to use the ModusToolbox&trade; PMBus&reg; middleware to send various PMBus&reg; commands and receive responses.

[View this README on GitHub.](https://github.com/Infineon/mtb-example-ce242798-pmbus-controller)

[Provide feedback on this code example.](https://yourvoice.infineon.com/jfe/form/SV_1NTns53sK2yiljn?Q_EED=eyJVbmlxdWUgRG9jIElkIjoiQ0UyNDI3OTgiLCJTcGVjIE51bWJlciI6IjAwMi00Mjc5OCIsIkRvYyBUaXRsZSI6IlBTT0MmdHJhZGU7IENvbnRyb2wgQzNNL1A4OiBQTUJ1cyZyZWc7IENvbnRyb2xsZXIiLCJyaWQiOiJtYWhhZGV2YS5ob3Nha290ZW5hZ2FyYWphQGluZmluZW9uLmNvbSIsIkRvYyB2ZXJzaW9uIjoiMS4wLjAiLCJEb2MgTGFuZ3VhZ2UiOiJFbmdsaXNoIiwiRG9jIERpdmlzaW9uIjoiTUNEIiwiRG9jIEJVIjoiSUNXIiwiRG9jIEZhbWlseSI6IlBTT0MifQ==)

## Requirements

- [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) v3.9.0 or later (tested with v3.9.0)
- Board support package (BSP) minimum required version for:
   - KIT_PSC3M8_EVK: v2.2.0
- Programming language: C
- Associated parts: All [PSOC&trade; Control C3M/P8 MCU](https://www.infineon.com/products/microcontroller/32-bit-psoc-arm-cortex/32-bit-psoc-control-arm-cortex-m33-mcu/psoc-control-c3-performance-line) parts


## Supported toolchains (make variable 'TOOLCHAIN')

- GNU Arm&reg; Embedded Compiler v14.2.1 (`GCC_ARM`) – Default value of `TOOLCHAIN`
- Arm&reg; Compiler v6.22 (`ARM`)
- IAR C/C++ Compiler v9.70.4 (`IAR`)


## Supported kits (make variable 'TARGET')

- [PSOC&trade; Control C3M8 Evaluation Kit](https://www.infineon.com/KIT_PSC3M8_EVK) (`KIT_PSC3M8_EVK`) – Default value of `TARGET`


## Hardware setup

Connect the PMBus&reg; Target SDA signal to P2 [4] and SCL signal to P2 [3]. Any compliant PMBus&reg; Target may be used. PMBUS_Slave code example "[mtb-example-ce242218-pmbus-slave](https://github.com/Infineon/mtb-example-ce242218-pmbus-slave)" can also be used to emulate a PMBus&reg; Slave (Build and program the PMBus&reg; slave code example to other EVK to emulate the slave/target).

- **2x PSOC&trade; Control C3M8 Evaluation Kits (KIT_PSC3M8_EVK):**
  - One kit acts as the PMBus&reg; Controller (Master)
  - One kit acts as the PMBus&reg; Slave/Target (or use the PMBus&reg; Slave code example)
  
Setup your board by interconnecting the SDA controller to the SDA target and the SCL controller to the SCL Target. See the kit user guide to ensure that the board is configured correctly. (MiniProg4)

**Table 1. I2C_SCL/I2C_SDA pin assignment**

Board name     | Master/Controller SCL   | Master/Controller SDA   | Slave/Target SCL | Slave/Target SDA
---------      | ----------      | ----------      | --------- | ---------
KIT_PSC3M8_EVK | 2.3            |  2.4           | 8.0       | 8.1 

<br>

   **Figure 1. Controller-Target hardware connection**

   ![](images/hardware-setup.png)


## Software setup

See the [ModusToolbox&trade; tools package installation guide](https://www.infineon.com/ModusToolboxInstallguide) for information about installing and configuring the tools package.

Install the ModusToolbox&trade; PMBus Configurator to access the PMBus Configurator tool.

Install a terminal emulator if you do not have one. Instructions in this document use [Tera Term](https://teratermproject.github.io/index-en.html).


## Using the code example


### Create the project

The ModusToolbox&trade; tools package provides the Project Creator as both a GUI tool and a command line tool.

<details><summary><b>Use Project Creator GUI</b></summary>

1. Open the Project Creator GUI tool

   There are several ways to do this, including launching it from the dashboard or from inside the Eclipse IDE. For more details, see the [Project Creator user guide](https://www.infineon.com/ModusToolboxProjectCreator) (locally available at *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/docs/project-creator.pdf*)

2. On the **Choose Board Support Package (BSP)** page, select a kit supported by this code example. See [Supported kits](#supported-kits-make-variable-target)

   > **Note:** To use this code example for a kit not listed here, you may need to update the source files. If the kit does not have the required resources, the application may not work

3. On the **Select Application** page:

   a. Select the **Applications(s) Root Path** and the **Target IDE**

      > **Note:** Depending on how you open the Project Creator tool, these fields may be pre-selected for you

   b. Select this code example from the list by enabling its check box

      > **Note:** You can narrow the list of displayed examples by typing in the filter box

   c. (Optional) Change the suggested **New Application Name** and **New BSP Name**

   d. Click **Create** to complete the application creation process

</details>


<details><summary><b>Use Project Creator CLI</b></summary>

The 'project-creator-cli' tool can be used to create applications from a CLI terminal or from within batch files or shell scripts. This tool is available in the *{ModusToolbox&trade; install directory}/tools_{version}/project-creator/* directory.

Use a CLI terminal to invoke the 'project-creator-cli' tool. On Windows, use the command-line 'modus-shell' program provided in the ModusToolbox&trade; installation instead of a standard Windows command-line application. This shell provides access to all ModusToolbox&trade; tools. You can access it by typing "modus-shell" in the search box in the Windows menu. In Linux and macOS, you can use any terminal application.

The following example clones the "[mtb-example-ce242798-pmbus-controller](https://github.com/Infineon/mtb-example-ce242798-pmbus-controller)" application with the desired name "PMBusCtrl" configured for the *KIT_PSC3M8_EVK* BSP into the specified working directory, *C:/mtb_projects*:

   ```
   project-creator-cli --board-id KIT_PSC3M8_EVK --app-id mtb-example-ce242798-pmbus-controller --user-app-name PMBusCtrl --target-dir "C:/mtb_projects"
   ```

The 'project-creator-cli' tool has the following arguments:

Argument | Description | Required/optional
---------|-------------|-----------
`--board-id` | Defined in the <id> field of the [BSP](https://github.com/Infineon?q=bsp-manifest&type=&language=&sort=) manifest | Required
`--app-id`   | Defined in the <id> field of the [CE](https://github.com/Infineon?q=ce-manifest&type=&language=&sort=) manifest | Required
`--target-dir`| Specify the directory in which the application is to be created if you prefer not to use the default current working directory | Optional
`--user-app-name`| Specify the name of the application if you prefer to have a name other than the example's default name | Optional

<br>

> **Note:** The project-creator-cli tool uses the `git clone` and `make getlibs` commands to fetch the repository and import the required libraries. For details, see the "Project creator tools" section of the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at {ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf).

</details>


### Open the project

After the project has been created, you can open it in your preferred development environment.


<details><summary><b>Eclipse IDE</b></summary>

If you opened the Project Creator tool from the included Eclipse IDE, the project will open in Eclipse automatically.

For more details, see the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_ide_user_guide.pdf*).

</details>


<details><summary><b>Visual Studio (VS) Code</b></summary>

Launch VS Code manually, and then open the generated *{project-name}.code-workspace* file located in the project directory.

For more details, see the [Visual Studio Code for ModusToolbox&trade; user guide](https://www.infineon.com/MTBVSCodeUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_vscode_user_guide.pdf*).

</details>



<details><summary><b>IAR Embedded Workbench</b></summary>

Open IAR Embedded Workbench manually, and create a new project. Then select the generated *{project-name}.ipcf* file located in the project directory.

For more details, see the [IAR Embedded Workbench for ModusToolbox&trade; user guide](https://www.infineon.com/MTBIARUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mt_iar_user_guide.pdf*).

</details>


<details><summary><b>Command line</b></summary>

If you prefer to use the CLI, open the appropriate terminal, and navigate to the project directory. On Windows, use the command-line 'modus-shell' program; on Linux and macOS, you can use any terminal application. From there, you can run various `make` commands.

For more details, see the [ModusToolbox&trade; tools package user guide](https://www.infineon.com/ModusToolboxUserGuide) (locally available at *{ModusToolbox&trade; install directory}/docs_{version}/mtb_user_guide.pdf*).

</details>


## Operation

1. Connect the board to your PC using the provided USB cable through the KitProg3 USB connector

2. Open a terminal program and select the KitProg3 COM port. Set the serial port parameters to 8N1 and 115200 baud

3. Program the board using one of the following:

   <details><summary><b>Using Eclipse IDE</b></summary>

      1. Select the application project in the Project Explorer

      2. In the **Quick Panel**, scroll down, and click **\<Application Name> Program (KitProg3_MiniProg4)**
   </details>


   <details><summary><b>In other IDEs</b></summary>

   Follow the instructions in your preferred IDE
   </details>


   <details><summary><b>Using CLI</b></summary>

     From the terminal, execute the `make program` command to build and program the application using the default toolchain to the default target. The default toolchain is specified in the application's Makefile but you can override this value manually:
      ```
      make program TOOLCHAIN=<toolchain>
      ```

      Example:
      ```
      make program TOOLCHAIN=GCC_ARM
      ```
   </details>

4. After programming, the application starts automatically. Confirm that the UART terminal displays the following:

   **Figure 2. Terminal output on program startup**

   ![](images/terminal-pmbus-controller.png)



## Debugging

You can debug the example to step through the code.


<details><summary><b>In Eclipse IDE</b></summary>

Use the **\<Application Name> Debug (KitProg3_MiniProg4)** configuration in the **Quick Panel**. For details, see the "Program and debug" section in the [Eclipse IDE for ModusToolbox&trade; user guide](https://www.infineon.com/MTBEclipseIDEUserGuide).


</details>


<details><summary><b>In other IDEs</b></summary>

Follow the instructions in your preferred IDE.

</details>


## Design and implementation

This PMBus&reg; Controller example demonstrates how to implement a PMBus&reg; master/controller using the Infineon ModusToolbox&trade; PMBus&reg; middleware. The example showcases multiple PMBus&reg; command protocols and communication patterns over I2C.

### Architecture Overview

The example implements a multi-core application where:

- **Secure CM33 (Main Processor):** Runs the PMBus&reg; Controller application, handles all I2C communication, and executes PMBus&reg; commands
- **PPCA0 and PPCA1 (Secondary Processors):** Minimal initialization and idle loop; can be extended for additional application-specific tasks

The PMBus&reg; Controller communicates with PMBus&reg; Slave devices using the I2C protocol at 100 kHz clock speed.

### PMBus&reg; Protocol Stack

The example implements the following PMBus&reg; command protocols:

#### 1. Quick Command
- **Purpose:** Minimal command for device control without data payload
- **Use Case:** Toggling a device feature (e.g., LED on/off)
- **Protocol:** Address + R/W bit only; no data exchange
- **Example in Code:** Toggles the target device LED

#### 2. Send/Receive Byte
- **Purpose:** Send or receive a single byte of data
- **Use Case:** Simple status queries or commands
- **Data Format:** 1 byte of data
- **Reliability:** CRC/PEC (Packet Error Code) optional

#### 3. Read 32 Protocol
- **Purpose:** Read 32-bit (4-byte) data from a specific command
- **Use Case:** Reading status registers, counters, or measurements
- **Data Format:** 4 bytes of data from slave
- **Protocol Sequence:**
  1. Master sends command code 
  2. Master reads 4 data bytes from slave
  3. Optional PEC verification
- **Example in Code:** Reads LED toggle counter (4 bytes) from target

#### 4. Generic Transfer API
- **Purpose:** Custom I2C transfers with arbitrary read/write sizes
- **Use Case:** PAGE command, Write/Read Word, multi-byte transfers
- **Flexibility:** Supports various PMBus&reg; command types
- **Protocol Sequences:**
  - **Write Only:** Master sends command code + N bytes
  - **Read Only:** Master reads N bytes from slave
  - **Write-Read:** Master writes command, then reads response

**PAGE Command Example:**
```
Sequence:
1. Send PAGE command (0x00) with page number
2. Subsequent operations apply to selected page
3. Different memory banks/pages on slave selected
```

**Zone Protocol (Advanced):**
- Supports multi-page, multi-zone communication
- Allows parallel operation on different zones
- Used for multi-channel power management systems

#### 5. Host-Notify Protocol (Optional)
- **Purpose:** Slave-initiated notifications to master
- **Use Case:** Alert master about critical conditions
- **Trigger:** Slave asserts SMAlert pin
- **Response:** Master queries slave for alert status

### Control Flow

The application executes in the following sequence:

```
┌─────────────────────────────────────────────────┐
│ 1. Initialization                               │
│    - Initialize MCU and peripherals             │
│    - Configure UART for debug output            │
│    - Initialize PMBus controller                │
│    - Enable global interrupts                   │
└─────────────────────────────────────────────────┘
                      ↓
┌─────────────────────────────────────────────────┐
│ 2. Main Loop (Infinite)                         │
│    - Run PMBus Examples                         │
│      * Quick Command                            │
│      * Read 32 Protocol                         │
│      * Generic Transfer (PAGE/Write/Read)       │
│    - Run Zone Read Example                      │
│    - Wait before repeating                      │
└─────────────────────────────────────────────────┘
```

**Timing:**
- Each example includes a timeout (10 seconds) to prevent indefinite blocking
- Delays between commands ensure slave has time to process
- Zone read example includes multi-transfer sequences

### Interrupt Handling

The controller uses interrupt-driven operation:

1. **I2C Interrupt Handler (`ctrl_i2c_isr`):**
   - Triggered by I2C events (data ready, NACK, error, completion)
   - Calls PMBus&reg; middleware ISR
   - Signals transfer completion to main application

2. **Optional Timeout Timer Interrupt:**
   - Monitors I2C transfer duration
   - Prevents infinite blocking on unresponsive slaves
   - Triggers timeout error if transfer exceeds limit

3. **Callback Mechanism:**
   - `pmbus_ctrl_callback()`: Receives PMBus&reg; events (transfer done, errors)
   - `pmbus_hw_callback()`: Receives hardware resource actions (init, enable, disable)
   - Application can log, handle errors, or trigger additional actions

### Error Handling

The example demonstrates proper error handling for:

- **Communication Errors:**
  - Target NACK on address (slave not responding)
  - Target NACK on command code (command not supported)
  - Target NACK on data bytes (data rejected)
  
- **Bus Errors:**
  - Arbitration lost (multi-master conflict)
  - Start condition error
  - Stop condition error

- **Timeout Errors:**
  - Transfer timeout (slave too slow or unresponsive)
  - Retry mechanism not implemented (slave must be responsive)

- **Data Integrity:**
  - Packet Error Code (PEC) validation optional
  - Corrupted data detection
  - Logging of all errors for debugging


### Resources and settings

**Table 2. Application resources**

 Resource  |  Alias/object     |    Purpose
 :-------- | :-------------    | :------------
 UART (HAL) |cy_retarget_io_uart_obj | UART HAL object used by Retarget-IO for the debug UART port
 I2C (PDL) |      PMBUS_CTRL_I2C       | Master-Slave  I2C

<br>


## Related resources

Resources  | Links
-----------|----------------------------------
Code examples  | [Using ModusToolbox&trade;](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software) on GitHub
Device documentation | [PSOC&trade; Control C3M/P8 MCU documents](https://www.infineon.com/products/microcontroller/32-bit-psoc-arm-cortex/32-bit-psoc-control-arm-cortex-m33-mcu/psoc-control-c3-performance-line?ftab=01#Documents)
Development kits | Select your kits from the [Evaluation board finder](https://www.infineon.com/cms/en/design-support/finder-selection-tools/product-finder/evaluation-board)
Libraries on GitHub  | [mtb-dsl-psc3m8](https://github.com/Infineon/mtb-dsl-psc3m8) – Device Support Library (DSL) <br> [retarget-io](https://github.com/Infineon/retarget-io) – Utility library to retarget STDIO messages to a UART port
Tools  | [ModusToolbox&trade;](https://www.infineon.com/modustoolbox) – ModusToolbox&trade; software is a collection of easy-to-use libraries and tools enabling rapid development with Infineon MCUs for applications ranging from wireless and cloud-connected systems, edge AI/ML, embedded sense and control, to wired USB connectivity using PSOC&trade; Industrial/IoT MCUs, AIROC&trade; Wi-Fi and Bluetooth&reg; connectivity devices, XMC&trade; Industrial MCUs, and EZ-USB&trade;/EZ-PD&trade; wired connectivity controllers. ModusToolbox&trade; incorporates a comprehensive set of BSPs, HAL, libraries, configuration tools, and provides support for industry-standard IDEs to fast-track your embedded application development

<br>


## Other resources

Infineon provides a wealth of data at [www.infineon.com](https://www.infineon.com) to help you select the right device, and quickly and effectively integrate it into your design.


## Document history

Document title: *CE242798* – *PSOC&trade; Control C3M/P8: PMBus&reg; Controller*

 Version | Description of change
 ------- | ---------------------
 1.0.0   | New code example

<br>


All referenced product or service names and trademarks are the property of their respective owners.

The Bluetooth&reg; word mark and logos are registered trademarks owned by Bluetooth SIG, Inc., and any use of such marks by Infineon is under license.

PSOC&trade;, formerly known as PSoC&trade;, is a trademark of Infineon Technologies. Any references to PSoC&trade; in this document or others shall be deemed to refer to PSOC&trade;.

---------------------------------------------------------

(c) 2024-2026, Infineon Technologies AG, or an affiliate of Infineon Technologies AG. All rights reserved.
This software, associated documentation and materials ("Software") is owned by Infineon Technologies AG or one of its affiliates ("Infineon") and is protected by and subject to worldwide patent protection, worldwide copyright laws, and international treaty provisions. Therefore, you may use this Software only as provided in the license agreement accompanying the software package from which you obtained this Software. If no license agreement applies, then any use, reproduction, modification, translation, or compilation of this Software is prohibited without the express written permission of Infineon.
<br>
Disclaimer: UNLESS OTHERWISE EXPRESSLY AGREED WITH INFINEON, THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, ALL WARRANTIES OF NON-INFRINGEMENT OF THIRD-PARTY RIGHTS AND IMPLIED WARRANTIES SUCH AS WARRANTIES OF FITNESS FOR A SPECIFIC USE/PURPOSE OR MERCHANTABILITY. Infineon reserves the right to make changes to the Software without notice. You are responsible for properly designing, programming, and testing the functionality and safety of your intended application of the Software, as well as complying with any legal requirements related to its use. Infineon does not guarantee that the Software will be free from intrusion, data theft or loss, or other breaches (“Security Breaches”), and Infineon shall have no liability arising out of any Security Breaches. Unless otherwise explicitly approved by Infineon, the Software may not be used in any application where a failure of the Product or any consequences of the use thereof can reasonably be expected to result in personal injury.
