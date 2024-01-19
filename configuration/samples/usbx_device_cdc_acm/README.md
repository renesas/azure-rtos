USBX device CDC-ACM Class sample project
========================================
Usage Notes
-----------
This demonstration illustrates the setup and use of USBX device CDC-ACM Class to communicate with the host as a serial device.

For more information about how to use this sample project, 
please refer to section 2.9 of r01an6455ej0200-rx-azure-rtos.pdf

To keep 8 bytes size for double type
 "-dbl_size=8" compiler build option is set as default for CC-RX project
 Size of type 'double' is set to "64 bits" as default for IAR project
 "-m64bit-doubles" build option is set as default for GCC project

Please also take note that the sample project is verified with C project.
If you create new project with C++ option, please confirm its behavior by yourself.
For example, if you create project with C++ option and CC-RX compiler, you will need to add abort() function manually.

Caution / Known Issue
---------------------
When using GCC compiler, in case you set Optimization level as Optimize size(-Os), please set the linker option not to remove unused sections as following
- in Project Explorer view, right-click on the project and select Properties
- on Properties dialog select C/C++ Build -> Settings -> Tool Settings tab -> Linker -> Other
- add "-Wl,--no-gc-sections" on User defined options
- click Apply and Close" button

When using GCC compiler, the "_end" section in src/linker_script.ld should be at the end. However, the default linker script may not meet this order, so please check linker_script.ld and move the section below to the end if needed, and build project again
.bss :
{
	_bss = .;
	*(.bss)
	*(.bss.**)
	*(COMMON)
	*(B)
	*(B_1)
	*(B_2)
	_ebss = .;
	_end = .;
} > RAM

When you connect RX65N Cloud Kit to your PC and get USB device not recognized message from Windows, please go to Clock page of Smart Configurator, enable SCKCR2 and set USB clock to 48MHz.

When using RX microcontrollers with DPFPU
Target projects (RX72N, RX671):
- Projects with the double precision floating point instruction generation option (CC-RX: -dpfpu, GCC: --mdfpu=64, IAR: --fpu=64) enabled on RXv3 core devices
Workaround:
- Call tx_thread_fpu_enable() at the beginning of every task.


Changes of sample project
=========================
The previous sample project up to USBX v6.1.10 used the USB HAL driver developed by Microsoft, however, this sample project using USBX v6.1.11 or later uses the USB HAL driver developed by Renesas.

If you would like to migrate previous application to the latest USBX, at least the following changes are needed.

src/hardware_setup.c
--------------------
- Remove usb0_setup function.
```
void usb0_setup(void)
{
    ...
}
```
- Remove the calling of usb0_setup function in platform_setup function.
```
    usb0_setup();
```

src/sample_usbx_device_cdc_acm.c
--------------------------------
- Remove including of "ux_dcd_rx.h"
```
#include "ux_dcd_rx.h"
```

- Add including of "r_usb_basic_if.h" and "r_usb_basic_config.h"
```
#include "r_usb_basic_if.h"
#include "r_usb_basic_config.h"
```

- Add prototype definition for R_USB_PinSet_USB0_PERI().
```
void R_USB_PinSet_USB0_PERI();
```

- Remove the calling of _ux_dcd_rx_initialize function and error check in sample_thread_entry function.
```
    status = _ux_dcd_rx_initialize(0);
    if (status != UX_SUCCESS)
        return;
```

- Add the calling of R_USB_PinSet_USB0_PERI function and R_USB_Open function in sample_thread_entry function.
```
usb_ctrl_t ctrl;
usb_cfg_t  cfg;

    R_USB_PinSet_USB0_PERI();

    ctrl.module     = USB_IP0; 
    ctrl.type       = USB_PCDC; 
    cfg.usb_speed   = USB_FS; /* USB_HS/USB_FS */ 
    cfg.usb_mode    = USB_PERI; 
    R_USB_Open(&ctrl, &cfg); 
```

In addition to above information, please refer to the current source code and r_usb_basic module's document in src\smc_gen\r_usb_basic\doc.
