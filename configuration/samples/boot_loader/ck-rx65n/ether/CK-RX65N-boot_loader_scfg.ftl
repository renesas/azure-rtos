<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<smc>
    <general version="2.0.0.0">
        <configuration active="true" id="${configurationTypeReleaseID}">
            <property id="com.renesas.smc.service.project.buildArtefactType" values="com.renesas.smc.service.project.buildArtefactType.exe"/>
            <toolchain id="${toolChainID}">
                <option id="com.renesas.smc.toolchain.option.buildArtefactType" key="com.renesas.smc.toolchain.option.buildArtefactType.exe"/>
                <option id="com.renesas.smc.toolchain.option.rtos" key="com.renesas.smc.toolchain.option.rtos.none"/>
            </toolchain>
        </configuration>
        <platform id="${targetDevice}"/>
    </general>
    <tool id="SWComponent" version="1.0.0.0">
        <configuration inuse="true" name="r_bsp">
            <component display="r_bsp" id="r_bsp7.41" version="7.41">
                <gridItem id="BSP_CFG_USER_CHARPUT_ENABLED" selectedIndex="1"/>
                <gridItem id="BSP_CFG_SWINT_UNIT1_ENABLE" selectedIndex="1"/>
                <gridItem id="BSP_CFG_SWINT_UNIT2_ENABLE" selectedIndex="1"/>
                <gridItem id="BSP_CFG_SCI_UART_TERMINAL_ENABLE" selectedIndex="1"/>
                <gridItem id="BSP_CFG_SCI_UART_TERMINAL_CHANNEL" selectedIndex="5"/>
                <gridItem id="BSP_CFG_SCI_UART_TERMINAL_INTERRUPT_PRIORITY" selectedIndex="3"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_flash_rx">
            <component display="r_flash_rx" id="r_flash_rx5.10" version="5.10">
                <gridItem id="FLASH_CFG_CODE_FLASH_ENABLE" selectedIndex="1"/>
                <gridItem id="FLASH_CFG_CODE_FLASH_RUN_FROM_ROM" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_fwup">
            <component display="r_fwup" id="r_fwup2.01" version="2.01">
                <gridItem id="FWUP_CFG_UPDATE_MODE" selectedIndex="0"/>
                <gridItem id="FWUP_CFG_FUNCTION_MODE" selectedIndex="0"/>
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
        <configuration inuse="true" name="r_sci_rx">
            <component display="r_sci_rx" id="r_sci_rx4.90" version="4.90">
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
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_byteq">
            <component display="r_byteq" id="r_byteq2.10" version="2.10">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
    </tool>
</smc>
