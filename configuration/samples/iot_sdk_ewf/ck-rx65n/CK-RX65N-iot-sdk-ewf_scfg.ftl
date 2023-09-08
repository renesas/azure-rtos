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
                <gridItem id="BSP_CFG_USER_CHARPUT_ENABLED" selectedIndex="1"/>
                <gridItem id="BSP_CFG_ROM_CACHE_ENABLE" selectedIndex="1"/>
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
            <component display="Azure RTOS NetX Duo Addons" id="netxduo_addons${packageVersion}" version="${packageVersion}"/>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="netxduo">
            <component display="Azure RTOS NetX Duo" id="netxduo${packageVersion}" version="${packageVersion}">
                <gridItem id="USE_NX_ENABLE_TCPIP_OFFLOAD" selectedIndex="1"/>
                <gridItem id="USE_NX_ENABLE_INTERFACE_CAPABILITY" selectedIndex="1"/>
                <gridItem id="USE_NX_DNS_CLIENT_USER_CREATE_PACKET_POOL" selectedIndex="1"/>
                <gridItem id="USE_NX_DNS_CLIENT_CLEAR_QUEUE" selectedIndex="1"/>
                <gridItem id="USE_NX_SNTP_CLIENT_MAX_ROOT_DISPERSION" selectedIndex="1"/>
                <gridItem id="USE_NX_SNTP_CURRENT_YEAR" selectedIndex="1"/>
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="filex">
            <component display="Azure RTOS FileX" id="filex${packageVersion}" version="${packageVersion}">
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="ewf">
            <component display="Azure Embedded Wireless Framework" id="ewf${packageVersion}" version="${packageVersion}">
                <gridItem id="EWF_CONFIG_AZURE_RTOS_NETX" selectedIndex="1"/>
            </component>
            <source description="Code generator for Real-time OS" display="RTOS Configurator" id="com.renesas.smc.tools.swcomponent.rtosconfigurator.source"/>
        </configuration>
        <configuration enable="true" exclude="false" inuse="true" name="azurertos_object">
            <allocatable id="" name="" type="">
                <allocatable id="THREAD" name="THREAD" type="TABALLOCATABLE">
                    <allocatable id="THREAD1" name="THREAD1" type="ROWALLOCATABLE">
                        <option defaultValue="new_thread[X]" id="threadpointer" value="application_thread"/>
                        <option defaultValue="New Thread" id="threadname" value="Application Thread"/>
                        <option defaultValue="new_thread[X]_entry" id="entryfunction" value="application_thread_entry"/>
                        <option defaultValue="0" id="entryinput" value="0"/>
                        <option defaultValue="1024" id="stacksize" value="4096"/>
                        <option defaultValue="1" id="priority" value="5"/>
                        <option defaultValue="1" id="preemptionthreshold" value="5"/>
                        <option defaultValue="0" id="timeslice" value="0"/>
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
        <configuration inuse="true" name="r_byteq">
            <component detailDescription="Byte-based circular buffer library." display="r_byteq" id="r_byteq2.10" version="2.10">
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
        <configuration inuse="true" name="r_sci_rx">
            <component detailDescription="Multi-Channel Multi-Mode SCI Driver." display="r_sci_rx" id="r_sci_rx4.90" version="4.90">
                <gridItem id="RXD5/SMISO5/SSCL5" selectedIndex="1"/>
                <gridItem id="TXD5/SMOSI5/SSDA5" selectedIndex="1"/>
                <gridItem id="SCI5" selectedIndex="1"/>
                <gridItem id="RXD6/SMISO6/SSCL6" selectedIndex="1"/>
                <gridItem id="TXD6/SMOSI6/SSDA6" selectedIndex="1"/>
                <gridItem id="CTS6#/RTS6#/SS6#" selectedIndex="1"/>
                <gridItem id="SCI6" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH0_INCLUDED" selectedIndex="0"/>
                <gridItem id="SCI_CFG_CH1_INCLUDED" selectedIndex="0"/>
                <gridItem id="SCI_CFG_CH5_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH6_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_EN_TXI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH6_EN_TXI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_EN_RXI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH6_EN_RXI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_EN_TEI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH6_EN_TEI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_EN_ERI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH6_EN_ERI_NESTED_INT" selectedIndex="1"/>
                <gridItem id="SCI_CFG_TEI_INCLUDED" selectedIndex="1"/>
                <gridItem id="SCI_CFG_CH5_TX_BUFSIZ" selectedIndex="2048"/>
                <gridItem id="SCI_CFG_CH6_TX_BUFSIZ" selectedIndex="4096"/>
                <gridItem id="SCI_CFG_CH6_RX_BUFSIZ" selectedIndex="8192"/>
            </component>
            <source description="Components supporting Firmware Integration Technology" display="Firmware Integration Technology" id="com.renesas.smc.tools.swcomponent.fit.source"/>
        </configuration>
    </tool>
</smc>
