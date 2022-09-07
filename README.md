# azure-rtos
This Azure RTOS repository that includes some modules (ThreadX/NetX/FileX/etc) as git submodule are referred from e2 studio/SmartConfigurator(SC).

# Tutorial Video
## Just demos
* Azure RTOS Tutorial (n/3) RX65N Cloud Kit: ~ Development Environment Setup
* You can download, compile, execution for 1st step: cpu board = RX65N Cloud Kit
  * en
    * https://www.renesas.com/jp/en/video/azure-rtos-tutorial-13-rx65n-cloud-kit-development-environment-setup
  * ja
    * https://www.renesas.com/jp/ja/video/azure-rtos-tutorial-13-rx65n-cloud-kit-development-environment-setup

## For development phase
* Getting started for Azure RTOS using Smart Configurator in e2 studio
* You can configure any settings: cpu board = any
  * en (same contents on both of Renesas website and YouTube)
    * https://www.renesas.com/jp/en/video/getting-started-azure-rtos-using-smart-configurator-e2-studio
    * https://www.youtube.com/watch?v=97C1stpNFig
  * ja (same contents on both of Renesas website and YouTube)
    * https://www.renesas.com/jp/ja/video/getting-started-azure-rtos-using-smart-configurator-e2-studio
    * https://www.youtube.com/watch?v=TKUFraKUSpQ
  * zh (same contents on both of Renesas website and bilibili)
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
    <th colspan="3">RX65N</th>
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
    <td>O</td>
    <td>O</td>
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
    <td>O</td>
    <td>O</td>
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
    <td>O</td>
    <td>O</td>
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
    <td>O</td>
    <td>-</td>
  </tr>
  <tr align="center">
    <td align="left">USBX sample</td>
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
    <td align="left">Threadx Low Power sample</td>
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
</table>

## History
* 6.1.12_rel-rx-1.0.0
  * Update Azure RTOS libraries to 6.1.12_rel
  * Improve sample linker script file
  * Set "-dbl_size=8" as default build option for CC-RX project
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
