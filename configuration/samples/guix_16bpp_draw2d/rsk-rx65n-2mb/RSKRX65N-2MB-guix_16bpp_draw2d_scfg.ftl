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
                <gridItem id="BSP_CFG_HEAP_BYTES" selectedIndex="0x4000"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_cmt_rx">
            <component display="r_cmt_rx" id="r_cmt_rx5.50" version="5.50">
                <gridItem id="CMT_CFG_CH0_EN_NESTED_INT" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="threadx">
            <component display="Azure RTOS ThreadX" id="threadx${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="guix">
            <component display="Azure RTOS GUIX" id="guix${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration inuse="true" name="r_glcdc_rx">
            <component display="r_glcdc_rx" id="r_glcdc_rx1.50" version="1.50">
                <gridItem id="LCD_CLK" selectedIndex="1"/>
                <gridItem id="LCD_TCON0" selectedIndex="1"/>
                <gridItem id="LCD_TCON2" selectedIndex="1"/>
                <gridItem id="LCD_TCON3" selectedIndex="1"/>
                <gridItem id="LCD_DATA0" selectedIndex="1"/>
                <gridItem id="LCD_DATA1" selectedIndex="1"/>
                <gridItem id="LCD_DATA2" selectedIndex="1"/>
                <gridItem id="LCD_DATA3" selectedIndex="1"/>
                <gridItem id="LCD_DATA4" selectedIndex="1"/>
                <gridItem id="LCD_DATA5" selectedIndex="1"/>
                <gridItem id="LCD_DATA6" selectedIndex="1"/>
                <gridItem id="LCD_DATA7" selectedIndex="1"/>
                <gridItem id="LCD_DATA8" selectedIndex="1"/>
                <gridItem id="LCD_DATA9" selectedIndex="1"/>
                <gridItem id="LCD_DATA10" selectedIndex="1"/>
                <gridItem id="LCD_DATA11" selectedIndex="1"/>
                <gridItem id="LCD_DATA12" selectedIndex="1"/>
                <gridItem id="LCD_DATA13" selectedIndex="1"/>
                <gridItem id="LCD_DATA14" selectedIndex="1"/>
                <gridItem id="LCD_DATA15" selectedIndex="1"/>
                <gridItem id="LCD_CH0_IN_GR2_FRAME_EDGE" selectedIndex="1"/>
                <gridItem id="LCD_CH0_BLEND_GR2_VISIBLE" selectedIndex="1"/>
                <gridItem id="LCD_CH0_BLEND_GR1_VISIBLE" selectedIndex="0"/>
                <gridItem id="LCD_CH0_IN_GR2_PBASE" selectedIndex="0x00860000"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_sci_iic_rx">
            <component display="r_sci_iic_rx" id="r_sci_iic_rx2.60" version="2.60">
                <gridItem id="SSCL7" selectedIndex="1"/>
                <gridItem id="SSDA7" selectedIndex="1"/>
                <gridItem id="SCI7" selectedIndex="1"/>
                <gridItem id="SCI_IIC_CFG_CH7_INCLUDED" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_drw2d_rx">
            <component display="r_drw2d_rx" id="r_drw2d_rx1.12" version="1.12"/>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
    </tool>
</smc>
