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
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_cmt_rx">
            <component display="r_cmt_rx" id="r_cmt_rx5.50" version="5.50">
                <gridItem id="CMT_CFG_CH0_EN_NESTED_INT" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_sci_rx">
            <component detailDescription="Multi-Channel Multi-Mode SCI Driver." display="r_sci_rx" id="r_sci_rx4.80" version="4.80">
                <gridItem id="RXD10/SMISO10/SSCL10" selectedIndex="1"/>
                <gridItem id="TXD10/SMOSI10/SSDA10" selectedIndex="1"/>
                <gridItem id="SCI10" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="0"/>
                <gridItem id="SCI_CFG_CH10_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH10_EN_TXI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH10_EN_RXI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH10_EN_TEI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH10_EN_ERI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_TEI_INCLUDED" selectedIndex="1"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_byteq">
            <component detailDescription="Byte-based circular buffer library." display="r_byteq" id="r_byteq2.10" version="2.10">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_gpio_rx">
            <component display="r_gpio_rx" id="r_gpio_rx4.90" version="4.90">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="threadx">
            <component display="Azure RTOS ThreadX" id="threadx${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="azurertos_object">
            <allocatable id="" name="" type="">
                <allocatable id="THREAD" name="THREAD" type="TABALLOCATABLE">
                    <allocatable id="THREAD1" name="THREAD1" type="ROWALLOCATABLE">
                        <option defaultValue="new_thread[X]" id="threadpointer" value="sample"/>
                        <option defaultValue="New Thread" id="threadname" value="sample_thread"/>
                        <option defaultValue="new_thread[X]_entry" id="entryfunction" value="sample_entry"/>
                        <option defaultValue="0" id="entryinput" value="0"/>
                        <option defaultValue="1024" id="stacksize" value="1024"/>
                        <option defaultValue="1" id="priority" value="16"/>
                        <option defaultValue="1" id="preemptionthreshold" value="16"/>
                        <option defaultValue="0" id="timeslice" value="0"/>
                        <option defaultValue="TX_AUTO_START" id="autostart" value="TX_AUTO_START"/>
                    </allocatable>
                </allocatable>
            </allocatable>
            <component display="AzureRTOS Object" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.azurertos.object" version="1.0.113"/>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
    </tool>
</smc>
