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
    <tool id="Interrupt">
		<Item currentVect="185" defaultVect="185" id="USB0_USBI0" priority="3" usedState="Not Use"/>
    </tool>
    <tool id="SWComponent" version="1.0.0.0">
        <configuration inuse="true" name="r_bsp">
            <component display="r_bsp" id="r_bsp7.20" version="7.20">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_cmt_rx">
            <component display="r_cmt_rx" id="r_cmt_rx5.20" version="5.20">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
		<configuration inuse="true" name="r_usb_basic">
            <component display="r_usb_basic" id="r_usb_basic1.41" version="1.41">
				<gridItem id="USB0_DP" selectedIndex="1"/>
                <gridItem id="USB0_DM" selectedIndex="1"/>
                <gridItem id="USB0_VBUSEN" selectedIndex="1"/>
                <gridItem id="USB0_OVRCURA" selectedIndex="1"/>
                <gridItem id="USB0_HOST" selectedIndex="1"/>
				<gridItem id="USB_CFG_MODE" selectedIndex="0"/>
                <gridItem id="USB_CFG_DEVICE_CLASS" selectedIndex="2"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="threadx">
            <component display="Azure RTOS ThreadX" id="threadx${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="filex">
            <component display="Azure RTOS FileX" id="filex${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="usbx">
            <component display="Azure RTOS UsbX" id="usbx${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
		<configuration enable="true" exclude="false" inuse="true" name="usbx_hmsc">
            <component display="Azure RTOS UsbX HMSC" id="usbx_hmsc${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
		<configuration enable="true" exclude="false" inuse="true" name="azurertos_object">
            <allocatable id="" name="" type="">
                <allocatable id="THREAD" name="THREAD" type="TABALLOCATABLE">
                    <allocatable id="THREAD1" name="THREAD1" type="ROWALLOCATABLE">
                        <option defaultValue="new_thread[X]" id="threadpointer" value="usb_thread0"/>
                        <option defaultValue="New Thread" id="threadname" value="New Thread"/>
                        <option defaultValue="new_thread[X]_entry" id="entryfunction" value="usb_thread0_func"/>
                        <option defaultValue="0" id="entryinput" value="0"/>
                        <option defaultValue="1024" id="stacksize" value="2048"/>
                        <option defaultValue="1" id="priority" value="21"/>
                        <option defaultValue="1" id="preemptionthreshold" value="21"/>
                        <option defaultValue="0" id="timeslice" value="1"/>
                        <option defaultValue="TX_AUTO_START" id="autostart" value="TX_AUTO_START"/>
                    </allocatable>
                </allocatable>
                <allocatable id="QUEUE" name="QUEUE" type="TABALLOCATABLE"/>
                <allocatable id="SEMAPHORE" name="SEMAPHORE" type="TABALLOCATABLE"/>
                <allocatable id="MUTEX" name="MUTEX" type="TABALLOCATABLE"/>
                <allocatable id="EVENT" name="EVENT" type="TABALLOCATABLE"/>
                <allocatable id="TIMER" name="TIMER" type="TABALLOCATABLE"/>
            </allocatable>
            <component description="" detailDescription="" display="AzureRTOS Object" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.azurertos.object" version="1.0.111"/>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
    </tool>
</smc>
