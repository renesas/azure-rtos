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
            <component display="r_bsp" id="r_bsp7.30" version="7.30">
                <gridItem id="BSP_CFG_ISTACK_BYTES" selectedIndex="0x800"/>
                <gridItem id="BSP_CFG_HEAP_BYTES" selectedIndex="0x800"/>
                <gridItem id="BSP_CFG_USER_CHARPUT_ENABLED" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_cmt_rx">
            <component display="r_cmt_rx" id="r_cmt_rx5.50" version="5.50">
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
                <gridItem id="SAMPLE_DEVICE_MANUFACTURER" selectedIndex="&quot;RENESAS&quot;"/>
                <gridItem id="SAMPLE_DEVICE_MODEL" selectedIndex="&quot;RX65N-RSK&quot;"/>
                <gridItem id="SAMPLE_LEAF_DEVICE_MANUFACTURER" selectedIndex="&quot;RENESAS&quot;"/>
                <gridItem id="SAMPLE_LEAF_DEVICE_MODEL" selectedIndex="&quot;RX65N-RSK-Leaf&quot;"/>
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="filex">
            <component display="Azure RTOS FileX" id="filex${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration inuse="true" name="r_ether_rx">
            <component description="Dependency : r_bsp version(s) 5.50&#10;The Ethernet fit module provides a method to send and receive Ethernet / IEEE802.3 frame using Ethernet controller (ETHERC), Ethernet DMA controller (EDMAC)." detailDescription="Ethernet Driver." display="r_ether_rx" id="r_ether_rx1.23" version="1.23">
                <gridItem id="ET0_TX_CLK" selectedIndex="1"/>
                <gridItem id="ET0_RX_CLK" selectedIndex="1"/>
                <gridItem id="ET0_TX_EN" selectedIndex="1"/>
                <gridItem id="ET0_ETXD3" selectedIndex="1"/>
                <gridItem id="ET0_ETXD2" selectedIndex="1"/>
                <gridItem id="ET0_ETXD1" selectedIndex="1"/>
                <gridItem id="ET0_ETXD0" selectedIndex="1"/>
                <gridItem id="ET0_RX_DV" selectedIndex="1"/>
                <gridItem id="ET0_ERXD3" selectedIndex="1"/>
                <gridItem id="ET0_ERXD2" selectedIndex="1"/>
                <gridItem id="ET0_ERXD1" selectedIndex="1"/>
                <gridItem id="ET0_ERXD0" selectedIndex="1"/>
                <gridItem id="ET0_RX_ER" selectedIndex="1"/>
                <gridItem id="ET0_CRS" selectedIndex="1"/>
                <gridItem id="ET0_COL" selectedIndex="1"/>
                <gridItem id="ET0_MDC" selectedIndex="1"/>
                <gridItem id="ET0_MDIO" selectedIndex="1"/>
                <gridItem id="ET0_LINKSTA" selectedIndex="1"/>
                <gridItem id="ETHERC0_MII" selectedIndex="1"/>
                <gridItem id="ETHER_CFG_CH0_PHY_ADDRESS" selectedIndex="30"/>
                <gridItem id="ETHER_CFG_EMAC_RX_DESCRIPTORS" selectedIndex="8"/>
                <gridItem id="ETHER_CFG_EMAC_TX_DESCRIPTORS" selectedIndex="16"/>
                <gridItem id="ETHER_CFG_CH0_PHY_ACCESS" selectedIndex="0"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_sci_rx">
            <component detailDescription="Multi-Channel Multi-Mode SCI Driver." display="r_sci_rx" id="r_sci_rx4.80" version="4.80">
                <gridItem id="RXD8/SMISO8/SSCL8" selectedIndex="1"/>
                <gridItem id="TXD8/SMOSI8/SSDA8" selectedIndex="1"/>
                <gridItem id="SCI8" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="0"/>
                <gridItem id="SCI_CFG_CH8_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_TEI_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH8_TX_BUFSIZ" selectedIndex="2180"/>
                <gridItem id="SCI_CFG_CH8_RX_BUFSIZ" selectedIndex="4096"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_flash_rx">
            <component display="r_flash_rx" id="r_flash_rx5.00" version="5.00">
                <gridItem id="FLASH_CFG_CODE_FLASH_ENABLE" selectedIndex="1"/>
                <gridItem id="FLASH_CFG_DATA_FLASH_BGO" selectedIndex="1"/>
                <gridItem id="FLASH_CFG_CODE_FLASH_BGO" selectedIndex="1"/>
                <gridItem id="FLASH_CFG_CODE_FLASH_RUN_FROM_ROM" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_byteq">
            <component display="r_byteq" id="r_byteq2.10" version="2.10">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_fwup">
            <component display="r_fwup" id="r_fwup1.06" version="1.06">
                <gridItem id="FWUP_CFG_IMPLEMENTATION_ENVIRONMENT" selectedIndex="4"/>
                <gridItem id="FWUP_CFG_COMMUNICATION_FUNCTION" selectedIndex="1"/>
                <gridItem id="FWUP_CFG_SERIAL_TERM_SCI" selectedIndex="6"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_sys_time_rx">
            <component display="r_sys_time_rx" id="r_sys_time_rx1.01" version="1.01"/>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
    </tool>
</smc>
