# azure-rtos
This Azure RTOS repository that includes some modules (ThreadX/NetX/FileX/etc) as git submodule are referred from e2 studio/SmartConfigurator(SC).

# Quick Start Steps
1) Buy MCU board like CK-RX65N.
<br/>https://www.renesas.com/products/microcontrollers-microprocessors/rx-32-bit-performance-efficiency-mcus/ck-rx65n-cloud-kit-based-rx65n-mcu-group
2) Install e2 studio.
<br/>https://www.renesas.com/software-tool/e-studio
3) Create new project on e2 studio, and user can select "Azure RTOS" on project generator, and specify version like v6.2.1_rel-rx-1.0.0. e2 studio can download the code from this repository. You can find guidebook on this URL:
<br/>https://www.renesas.com/search?keywords=R01AN6455
4) Input your credential info for your Azure account.
5) Build.
6) Download built firmware to MCU.
7) Run, MCU can connect to Azure servers.

# For RA Family user
* Please visit Flexible Software Package page.
  * https://www.renesas.com/software-tool/flexible-software-package-fsp
  
# Tutorial Video
## Just demos
### playlist on YouTube
* https://www.youtube.com/playlist?list=PLgUXqPkOStPsB9yh-XZ7BNv0J6UtAERO1

### CK-RX65N (new: RX65N Cloud Kit)
* Azure RTOS Tutorial (n/3) RX65N Cloud Kit: ~ Development Environment Setup
* You can download, compile, execution for 1st step: cpu board = RX65N Cloud Kit (new: ether(available)/cellular(now preparing))
* same contents on both of Renesas website and YouTube/bilibili
  * en
    * https://www.renesas.com/us/en/video/azure-rtos-tutorial-13-ck-rx65n-setup?rxsw-e
    * https://www.youtube.com/playlist?list=PLgUXqPkOStPuSFEGrxJi1uDbo1Hv4X2lP
  * ja
    * https://www.renesas.com/jp/ja/video/azure-rtos-tutorial-13-ck-rx65n-setup?rxsw-j
    * https://www.youtube.com/playlist?list=PLgUXqPkOStPvSOWrIECsFJV5Ci3X8sVJ1
    
<details>
<summary>old boards</summary>
 
### RX65N Cloud Kit (old)
* Azure RTOS Tutorial (n/3) RX65N Cloud Kit: ~ Development Environment Setup
* You can download, compile, execution for 1st step: cpu board = RX65N Cloud Kit (old: wifi)
* same contents on both of Renesas website and YouTube/bilibili
  * en
    * https://www.renesas.com/jp/en/video/azure-rtos-tutorial-13-rx65n-cloud-kit-development-environment-setup
  * ja
    * https://www.renesas.com/jp/ja/video/azure-rtos-tutorial-13-rx65n-cloud-kit-development-environment-setup
 
</details>

## For development phase
* Getting started for Azure RTOS using Smart Configurator in e2 studio
* You can configure any settings: cpu board = any
* same contents on both of Renesas website and YouTube/bilibili
  * en 
    * https://www.renesas.com/jp/en/video/getting-started-azure-rtos-using-smart-configurator-e2-studio
    * https://www.youtube.com/watch?v=97C1stpNFig
  * ja
    * https://www.renesas.com/jp/ja/video/getting-started-azure-rtos-using-smart-configurator-e2-studio
    * https://www.youtube.com/watch?v=TKUFraKUSpQ
  * zh
    * https://www.renesas.com/jp/zh/video/getting-started-azure-rtos-using-smart-configurator-e2-studio
    * https://www.bilibili.com/video/BV1K34y1E76k
* How to create Azure RTOS sample project for IAR EW
  * en: https://en-support.renesas.com/knowledgeBase/20533128
  * ja: https://ja-support.renesas.com/knowledgeBase/20533124

# versioning
* https://github.com/renesas/amazon-freertos/issues/8

# Supported boards/devices and applications
## The latest software package
<table>
  <tr>
    <th align="left">Device</th>
    <th colspan="4">RX65N</th>
    <th>RX651</th>
    <th>RX130</th>
    <th>RX140</th>
    <th>RX660</th>
    <th>RX66T</th>
    <th>RX72N</th>
    <th>RX671</th>
  </tr>
  <tr>
    <th align="left">Board</th>
    <th>RSK</th>
    <th>Cloud Kit</th>
    <th>New CK</th>
    <th>New CK</th>
    <th>RSK RX65N</th>
    <th>Target Board</th>
    <th>RSK</th>
    <th>RSK</th>
    <th>RSK</th>
    <th>Envision Kit</th>
    <th>RSK</th>
  </tr>
  <tr>
    <th align="left">Connectivity</th>
    <th>Ethernet</th>
    <th>Wi-Fi</th>
    <th>Ethernet</th>
    <th>Cat M1</th>
    <th>-</th>
    <th>-</th>
    <th>-</th>
    <th>-</th>
    <th>-</th>
    <th>Ethernet</th>
    <th>Wi-Fi</th>
  </tr>
  <tr align="center">
    <td align="left">ThreadX sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
  </tr>
  <tr align="center">
    <td align="left">FileX sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
  </tr>
  <tr align="center">
    <td align="left">Ping sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>O</td>
  </tr>
  <tr align="center">
    <td align="left">Iperf sample</td>
    <td>O</td>
    <td>-</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>-</td>
  </tr>
  <tr align="center">
    <td align="left">IoT Embedded SDK sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>O</td>
  </tr>
  <tr align="center">
    <td align="left">IoT Embedded SDK sample using EWF</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
  </tr>
  <tr align="center">
    <td align="left">IoT Embedded SDK PnP sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>O</td>
  </tr>
  <tr align="center">
    <td align="left">IoT Embedded SDK PnP sample using EWF</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
  </tr>
  <tr align="center">
    <td align="left">IoT Embedded SDK with IoT Plug and Play sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>O</td>
  </tr>
  <tr align="center">
    <td align="left">IoT Embedded SDK with IoT Plug and Play sample using EWF</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
  </tr>
  <tr align="center">
    <td align="left">GUIX (8bpp, 16bpp, draw 2d) sample</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>-</td>
  </tr>
  <tr align="center">
    <td align="left">USBX PCDC sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
  </tr>
  <tr align="center">
    <td align="left">USBX HMSC sample</td>
    <td>O</td>
    <td>-</td>
    <td>O</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>O</td>
  </tr>
  <tr align="center">
    <td align="left">Threadx Low Power sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>-</td>
    <td>O</td>
    <td>O</td>
  </tr>
  <tr align="center">
    <td align="left">ADU sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>O</td>
  </tr>
  <tr align="center">
    <td align="left">Bootloader sample</td>
    <td>O</td>
    <td>O</td>
    <td>O</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>-</td>
    <td>O</td>
    <td>O</td>
  </tr>
</table>

## History
* 6.2.1_rel-rx-1.0.1
  * Update EWF library to the latest version
  * Update SDK samples with EWF to use the latest codes (fixed debug off and iothub connection issue)
* 6.2.1_rel-rx-1.0.0
  * Update Azure RTOS libraries to 6.2.1_rel
  * Support CC-RX for IoT Embedded SDK samples using EWF
  * Update SDK samples with EWF to support SCI channel configuration
  * Migrate Root CA to "DigiCert Global G2 Root"
  * Enable optimizaton and "Motorola S format type" for CC-RX projects
  * Update bsp version from 7.20 to 7.21
  * Change SCI driver code style from CG to FIT
  * Improve codes related to ethernet and Wifi for SDK samples
  * Improve codes for ADU sample
* 6.2.0_rel-rx-1.0.0
  * Update Azure RTOS libraries to 6.2.0_rel
  * Update RX Driver Package to 1.37
  * Support USBX HMSC sample
  * Support IoT Embedded SDK sample using EWF (Preliminary version).
    Note: only support GCC RX for this preliminary version
  * Support ADU and bootloader sample.
    Note: only support CC-RX and GCC RX
  * Set "-m64bit-doubles" as default build option for GCC RX project
  * Update IoT samples to use the latest azure_iot cert from NetXDuo
* 6.1.12_rel-rx-1.0.1
  * Update CC-RX section setting for RX65N sample projects using ethernet
* 6.1.12_rel-rx-1.0.0
  * Update Azure RTOS libraries to 6.1.12_rel
  * Improve sample linker script file
  * Set "-dbl_size=8" as default build option for CC-RX project
  * Set "64 bits" for size of type 'double' as default build option for IAR project
  * Rename PnP Temperature Control sample project to IoT Embedded SDK with IoT Plug and Play sample project
* 6.1.11_rel-rx-1.0.0
  * Update Azure RTOS libraries to 6.1.11_rel
  * Update RX Driver Package to 1.36
  * Support RX65N New Cloud Kit, RX140 RSK, RX660 RSK
  * Support ThreadX Low Power sample
  * Update USBX sample to use Renesas USB driver
* 6.1.10_rel-rx-1.0.0
  * Update Azure RTOS libraries to 6.1.10_rel
  * Support IAR
  * Update for new board name of Envision Kit and Cloud Kit
  * Update RSKRX671 to use PMOD2
  * Update to use TX_TIMER_TICKS_PER_SECOND macro in sample code
  * Update to unify default time server to 0.pool.ntp.org in sample code
* 6.1.9_rel-rx-1.0.0
  * Update Azure RTOS libraries to 6.1.9_rel
  * Update RX Driver Package to 1.32-with-wifi
  * Support RX651, RX130, RX66T, RX671, RX72N
  * Support GUIX sample
* 6.1.6_rel-rx-1.0.*
  * Azure RTOS libraries to 6.1.6_rel
  * RX Driver Package to 1.32
  * Support RX65N
  * Support ThreadX, FileX, Ping, Iperf, SDK, SDK PnP, PnP Temperature, USBX sample

# Reference Video
* Azure ADU (Firmware Update on the air) 
  * https://www.youtube.com/watch?v=gxODyzvWSYs

# How to use Trusted Secure IP with Azure RTOS
* under constructing...

