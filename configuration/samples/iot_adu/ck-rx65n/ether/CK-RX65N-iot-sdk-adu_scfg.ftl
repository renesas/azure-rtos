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
            <component display="r_bsp" id="r_bsp7.41" version="7.41">
                <gridItem id="BSP_CFG_ISTACK_BYTES" selectedIndex="0x800"/>
                <gridItem id="BSP_CFG_HEAP_BYTES" selectedIndex="0x800"/>
                <gridItem id="BSP_CFG_USER_CHARPUT_ENABLED" selectedIndex="1"/>
                <gridItem id="BSP_CFG_SCI_UART_TERMINAL_CHANNEL" selectedIndex="5"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_cmt_rx">
            <component display="r_cmt_rx" id="r_cmt_rx5.60" version="5.60">
                <gridItem id="CMT_CFG_CH0_EN_NESTED_INT" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="threadx">
            <component display="Azure RTOS ThreadX" id="threadx${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="netxduo_addons">
            <component display="Azure RTOS NetX Duo Addons" id="netxduo_addons${packageVersion}" version="${packageVersion}">
                <gridItem id="HTTP" selectedIndex="0"/>
                <gridItem id="NAT" selectedIndex="0"/>
                <gridItem id="SMTP" selectedIndex="0"/>
                <gridItem id="SNMP" selectedIndex="0"/>
            </component>
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
                <gridItem id="NX_AZURE_IOT_HUB_CLIENT_USER_AGENT_DEVICE_TYPE" selectedIndex="&quot;CK-RX65N&quot;"/>
                <gridItem id="SAMPLE_DEVICE_MANUFACTURER" selectedIndex="&quot;RENESAS&quot;"/>
                <gridItem id="SAMPLE_DEVICE_MODEL" selectedIndex="&quot;CK-RX65N&quot;"/>
                <gridItem id="SAMPLE_LEAF_DEVICE_MANUFACTURER" selectedIndex="&quot;RENESAS&quot;"/>
                <gridItem id="SAMPLE_LEAF_DEVICE_MODEL" selectedIndex="&quot;CK-RX65N-Leaf&quot;"/>
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration inuse="true" name="r_ether_rx">
            <component display="r_ether_rx" id="r_ether_rx1.23" version="1.23">
                <gridItem id="REF50CK0" selectedIndex="1"/>
                <gridItem id="RMII0_TXD_EN" selectedIndex="1"/>
                <gridItem id="RMII0_TXD1" selectedIndex="1"/>
                <gridItem id="RMII0_TXD0" selectedIndex="1"/>
                <gridItem id="RMII0_RXD1" selectedIndex="1"/>
                <gridItem id="RMII0_RXD0" selectedIndex="1"/>
                <gridItem id="RMII0_RX_ER" selectedIndex="1"/>
                <gridItem id="RMII0_CRS_DV" selectedIndex="1"/>
                <gridItem id="ET0_MDC" selectedIndex="1"/>
                <gridItem id="ET0_MDIO" selectedIndex="1"/>
                <gridItem id="ET0_LINKSTA" selectedIndex="1"/>
                <gridItem id="ETHERC0_RMII" selectedIndex="1"/>
                <gridItem id="ETHER_CFG_MODE_SEL" selectedIndex="1"/>
                <gridItem id="ETHER_CFG_CH0_PHY_ADDRESS" selectedIndex="5"/>
                <gridItem id="ETHER_CFG_EMAC_RX_DESCRIPTORS" selectedIndex="8"/>
                <gridItem id="ETHER_CFG_EMAC_TX_DESCRIPTORS" selectedIndex="16"/>
                <gridItem id="ETHER_CFG_CH0_PHY_ACCESS" selectedIndex="0"/>
                <gridItem id="ETHER_CFG_LINK_PRESENT" selectedIndex="1"/>
                <gridItem id="ETHER_CFG_USE_PHY_ICS1894_32" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_sci_rx">
            <component detailDescription="Multi-Channel Multi-Mode SCI Driver." display="r_sci_rx" id="r_sci_rx4.90" version="4.90">
                <gridItem id="RXD5/SMISO5/SSCL5" selectedIndex="1"/>
                <gridItem id="TXD5/SMOSI5/SSDA5" selectedIndex="1"/>
                <gridItem id="SCI5" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="0"/>
                <gridItem id="SCI_CFG_CH5_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_EN_TXI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_EN_RXI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_EN_TEI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_EN_ERI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_TEI_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_TX_BUFSIZ" selectedIndex="2180"/>
                <gridItem id="SCI_CFG_CH5_RX_BUFSIZ" selectedIndex="4096"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_flash_rx">
            <component display="r_flash_rx" id="r_flash_rx5.10" version="5.10">
                <gridItem id="FLASH_CFG_CODE_FLASH_ENABLE" selectedIndex="1"/>
                <gridItem id="FLASH_CFG_DATA_FLASH_BGO" selectedIndex="0"/>
                <gridItem id="FLASH_CFG_CODE_FLASH_BGO" selectedIndex="0"/>
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
            <component display="r_fwup" id="r_fwup2.01" version="2.01">
                <gridItem id="FWUP_CFG_UPDATE_MODE" selectedIndex="0"/>
                <gridItem id="FWUP_CFG_FUNCTION_MODE" selectedIndex="1"/>
                <gridItem id="FWUP_CFG_MAIN_AREA_ADDR_L" selectedIndex="0xFFF00000"/>
                <gridItem id="FWUP_CFG_BUF_AREA_ADDR_L" selectedIndex="0xFFE00000"/>
                <gridItem id="FWUP_CFG_AREA_SIZE" selectedIndex="0xF0000"/>
                <gridItem id="FWUP_CFG_CF_BLK_SIZE" selectedIndex="0x8000"/>
                <gridItem id="FWUP_CFG_EXT_BUF_AREA_ADDR_L" selectedIndex="0x00000000"/>
                <gridItem id="FWUP_CFG_EXT_BUF_AREA_BLK_SIZE" selectedIndex="4096"/>
                <gridItem id="FWUP_CFG_DF_ADDR_L" selectedIndex="0x00100000"/>
                <gridItem id="FWUP_CFG_DF_BLK_SIZE" selectedIndex="64"/>
                <gridItem id="FWUP_CFG_DF_NUM_BLKS" selectedIndex="512"/>
                <gridItem id="FWUP_CFG_SIGNATURE_VERIFICATION" selectedIndex="0"/>
                <gridItem id="FWUP_CFG_PRINTF_DISABLE" selectedIndex="0"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
    </tool>
</smc>
