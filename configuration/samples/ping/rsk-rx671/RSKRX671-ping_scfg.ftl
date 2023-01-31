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
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_cmt_rx">
            <component display="r_cmt_rx" id="r_cmt_rx5.40" version="5.40">
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
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="filex">
            <component display="Azure RTOS FileX" id="filex${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration inuse="true" name="Config_SCI10">
            <allocatable id="SCI10">
                <option enabled="true" id="Bitrate" selection="FreeValue">
                    <item id="FreeValue" input="115200" vlaue="115200"/>
                </option>
            </allocatable>
            <component display="SCI/SCIF Asynchronous Mode" id="com.renesas.smc.tools.swcomponent.codegenerator.sciasyncmode" version="1.11.0"/>
            <allocator channelLevel0="10" channelLevel1="" channelLevel2="" channelLevel3="" channelLevel4="" channelLevel5="" display="SCI10" id="com.renesas.smc.swc.cg.rx.sciasync.rx671.sci10" type="">
                <context>
                    <option enabled="true" id="Mode" selection="BothMode">
                    </option>
                </context>
            </allocator>
            <source description="Code generator components provide peripheral drivers with customized generated source geared towards small code size" display="Code Generator" id="com.renesas.smc.tools.swcomponent.codegenerator.source"/>
        </configuration>
        <configuration inuse="true" name="r_byteq">
            <component detailDescription="Byte-based circular buffer library." display="r_byteq" id="r_byteq2.10" version="2.10">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_sci_rx">
            <component detailDescription="Multi-Channel Multi-Mode SCI Driver." display="r_sci_rx" id="r_sci_rx4.60" version="4.60">
            <gridItem id="RXD0/SMISO0/SSCL0" selectedIndex="1"/>
            <gridItem id="TXD0/SMOSI0/SSDA0" selectedIndex="1"/>
            <gridItem id="SCI0" selectedIndex="1"/>
            <gridItem id="RXD2/SMISO2/SSCL2" selectedIndex="1"/>
            <gridItem id="TXD2/SMOSI2/SSDA2" selectedIndex="1"/>
            <gridItem id="CTS2#/RTS2#/SS2#" selectedIndex="1"/>
            <gridItem id="SCI2" selectedIndex="1"/>
            <gridItem id="SCI_CFG_CH0_INCLUDED" selectedIndex="1"/>
            <gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="0"/>
            <gridItem id="SCI_CFG_CH2_INCLUDED" selectedIndex="1"/>
            <gridItem id="SCI_CFG_TEI_INCLUDED" selectedIndex="1"/>
            <gridItem id="SCI_CFG_CH0_RX_BUFSIZ" selectedIndex="3000"/>
            <gridItem id="SCI_CFG_CH2_RX_BUFSIZ" selectedIndex="3000"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_wifi_sx_ulpgn">
            <component detailDescription="WI-FI Module control functions for Renesas MCUs." display="r_wifi_sx_ulpgn" id="r_wifi_sx_ulpgn1.16" version="1.16">
            <gridItem id="WIFI_CFG_SCI_CHANNEL" selectedIndex="2"/>
            <gridItem id="WIFI_CFG_SCI_SECOND_CHANNEL" selectedIndex="0"/>
            <gridItem id="WIFI_CFG_RESET_PORT" selectedIndex="9"/>
            <gridItem id="WIFI_CFG_RESET_PIN" selectedIndex="0"/>
            <gridItem id="WIFI_CFG_RTS_PORT" selectedIndex="5"/>
            <gridItem id="WIFI_CFG_RTS_PIN" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
    </tool>
</smc>
