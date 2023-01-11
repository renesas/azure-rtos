<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<smc>
    <general version="2.0.0.0">
        <configuration active="true" id="${configurationTypeReleaseID}">
            <property id="com.renesas.smc.service.project.buildArtefactType" values="com.renesas.smc.service.project.buildArtefactType.exe"/>
            <toolchain id="${toolChainID}">
                <option id="com.renesas.smc.toolchain.option.buildArtefactType" key="com.renesas.smc.toolchain.option.buildArtefactType.exe"/>
                <option id="com.renesas.smc.toolchain.option.rtos" key="com.renesas.smc.toolchain.option.rtos.azurertos">
                    <item id="com.renesas.smc.toolchain.option.rtos.azurertos" value="${packageVersion}"/>
                </option>
            </toolchain>
        </configuration>
        <platform id="${targetDevice}"/>
    </general>
    <tool id="SWComponent" version="1.0.0.0">
        <configuration inuse="true" name="r_bsp">
            <component display="r_bsp" id="r_bsp7.20" version="7.20">
                <gridItem id="BSP_CFG_USER_CHARPUT_ENABLED" selectedIndex="1"/>
                <gridItem id="BSP_CFG_SWINT_UNIT2_ENABLE" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_cmt_rx">
            <component display="r_cmt_rx" id="r_cmt_rx5.20" version="5.20">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="threadx">
            <component display="Azure RTOS ThreadX" id="threadx${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="netxduo_addons">
            <component display="Azure RTOS NetX Duo Addons" id="netxduo_addons${packageVersion}" version="${packageVersion}"/>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="netxduo">
            <component display="Azure RTOS NetX Duo" id="netxduo${packageVersion}" version="${packageVersion}">
                <gridItem id="USE_NX_SECURE_TLS_DISABLE_TLS_1_1" selectedIndex="1"/>
                <gridItem id="USE_NX_DNS_CLIENT_USER_CREATE_PACKET_POOL" selectedIndex="1"/>
                <gridItem id="USE_NX_DNS_CLIENT_CLEAR_QUEUE" selectedIndex="1"/>
                <gridItem id="USE_NXD_MQTT_PING_TIMEOUT_DELAY" selectedIndex="1"/>
                <gridItem id="USE_NXD_MQTT_SOCKET_TIMEOUT" selectedIndex="1"/>
                <gridItem id="USE_NX_SNTP_CLIENT_MIN_SERVER_STRATUM" selectedIndex="1"/>
                <gridItem id="USE_NX_AZURE_DISABLE_IOT_SECURITY_MODULE" selectedIndex="1"/>
                <gridItem id="USE_NX_AZURE_IOT_HUB_CLIENT_USER_AGENT_DEVICE_VENDOR" selectedIndex="1"/>
                <gridItem id="USE_NX_AZURE_IOT_HUB_CLIENT_USER_AGENT_DEVICE_TYPE" selectedIndex="1"/>
                <gridItem id="NX_AZURE_IOT_HUB_CLIENT_USER_AGENT_DEVICE_TYPE" selectedIndex="&quot;RX65N-RSK&quot;"/>
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="filex">
            <component display="Azure RTOS FileX" id="filex${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration inuse="true" name="Config_SCI5">
            <allocatable id="SCI5">
                <option enabled="true" id="Bitrate" selection="FreeValue">
                    <item id="FreeValue" input="115200" vlaue="115200"/>
                </option>
            </allocatable>
            <component display="SCI/SCIF Asynchronous Mode" id="com.renesas.smc.tools.swcomponent.codegenerator.sciasyncmode" version="1.11.0"/>
            <allocator channelLevel0="5" channelLevel1="" channelLevel2="" channelLevel3="" channelLevel4="" channelLevel5="" description="SCI channel 5" display="SCI5" id="com.renesas.smc.tools.swcomponent.codegenerator.sciasync.rx651.sci5" type="SCI">
                <context>
                    <option enabled="true" id="Mode" selection="BothMode">
                    </option>
                </context>
            </allocator>
            <source description="Code generator components provide peripheral drivers with customized generated source geared towards small code size" display="Code Generator" id="com.renesas.smc.tools.swcomponent.codegenerator.source"/>
        </configuration>
        <configuration inuse="true" name="r_flash_rx">
            <component display="r_flash_rx" id="r_flash_rx4.90" version="4.90">
                <gridItem id="FLASH_CFG_CODE_FLASH_ENABLE" selectedIndex="1"/>
                <gridItem id="FLASH_CFG_DATA_FLASH_BGO" selectedIndex="1"/>
                <gridItem id="FLASH_CFG_CODE_FLASH_BGO" selectedIndex="1"/>
                <gridItem id="FLASH_CFG_CODE_FLASH_RUN_FROM_ROM" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_byteq">
            <component display="r_byteq" id="r_byteq2.00" version="2.00">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_sci_rx">
            <component display="r_sci_rx" id="r_sci_rx4.40" version="4.40">
                <gridItem id="RXD0/SMISO0/SSCL0" selectedIndex="1"/>
                <gridItem id="TXD0/SMOSI0/SSDA0" selectedIndex="1"/>
                <gridItem id="CTS0#/RTS0#/SS0#" selectedIndex="1"/>
                <gridItem id="SCI0" selectedIndex="1"/>
                <gridItem id="RXD1/SMISO1/SSCL1" selectedIndex="1"/>
                <gridItem id="TXD1/SMOSI1/SSDA1" selectedIndex="1"/>
                <gridItem id="SCI1" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH0_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH0_TX_BUFSIZ" selectedIndex="2180"/>
                <gridItem id="SCI_CFG_CH1_TX_BUFSIZ" selectedIndex="2180"/>
                <gridItem id="SCI_CFG_CH0_RX_BUFSIZ" selectedIndex="4096"/>
                <gridItem id="SCI_CFG_CH1_RX_BUFSIZ" selectedIndex="4096"/>
                <gridItem id="SCI_CFG_TEI_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_ERI_TEI_PRIORITY" selectedIndex="3"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_wifi_sx_ulpgn">
            <component display="r_wifi_sx_ulpgn" id="r_wifi_sx_ulpgn1.16" version="1.16">
                <gridItem id="WIFI_CFG_SCI_CHANNEL" selectedIndex="0"/>
                <gridItem id="WIFI_CFG_SCI_SECOND_CHANNEL" selectedIndex="1"/>
                <gridItem id="WIFI_CFG_SCI_INTERRUPT_LEVEL" selectedIndex="2"/>
                <gridItem id="WIFI_CFG_RESET_PORT" selectedIndex="13"/>
                <gridItem id="WIFI_CFG_RTS_PORT" selectedIndex="2"/>
                <gridItem id="WIFI_CFG_RTS_PIN" selectedIndex="2"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_fwup">
            <component display="r_fwup" id="r_fwup1.06" version="1.06">
                <gridItem id="FWUP_CFG_IMPLEMENTATION_ENVIRONMENT" selectedIndex="4"/>
                <gridItem id="FWUP_CFG_COMMUNICATION_FUNCTION" selectedIndex="1"/>
                <gridItem id="FWUP_CFG_SERIAL_TERM_SCI" selectedIndex="5"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_sys_time_rx">
            <component display="r_sys_time_rx" id="r_sys_time_rx1.01" version="1.01"/>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
    </tool>
</smc>
