# C/H 文件与依赖索引

按 Keil 文件引用标记编译参与情况；头文件通过 include 间接参与。函数/调用为静态词法索引，不代表运行时完整调用图。库文件保留原始授权头。

## `firmware/F103_Project/APP/app_types.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/APP/auto_mode.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`auto_mode.h`。
定义：`AutoMode_Init`, `AutoMode_Enter`, `AutoMode_Service`, `AutoMode_Exit`, `AutoMode_IsActive`, `AutoMode_GetShape`。

## `firmware/F103_Project/APP/auto_mode.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`app_types.h`, `signal_capture.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/APP/manual_control.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`manual_control.h`, `AD9910.h`, `app_config.h`。
定义：`Control_Abs32`, `Control_Clamp32`, `Control_WrapPhase`, `Control_DpllCorrectionLimit`, `Control_FtwToMilliHz`, `Control_FtwQ16ToMilliHz`, `Control_ErrorToFtw`, `Control_UpdateDisplayedCorrection`, `Control_WriteToneFtw`, `Control_ResetDpll`, `Control_CorrectionMilliHzToFtwQ16`, `Control_ApplyDpllOutput`, `Control_ResetFeedbackFilter`, `Control_ResetPhaseWindow`, `Control_FeedbackMedian`, `Control_UpdateFeedbackFilter`, `Control_CommitFtwDelta`, `Control_SetTarget`, `Control_UpdatePhaseSlope`, `Control_UpdateAcquire`, `Control_UpdateFine`, `Control_UpdateDpll`, `Control_DefaultTarget`, `ManualControl_Init`, `ManualControl_Start`, `ManualControl_Stop`, `ManualControl_Update`, `ManualControl_GetStatus`, `ManualControl_IsActive`。

## `firmware/F103_Project/APP/manual_control.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`app_types.h`, `signal_capture.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/APP/menu.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`board.h`, `signal_capture.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/APP/menu_chinese.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`menu_chinese.h`, `lcd.h`。
定义：`MenuChinese_DrawGlyph`, `MenuChinese_TextWidth`, `MenuChinese_DrawText`。

## `firmware/F103_Project/APP/menu_chinese.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/APP/menu_v2.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`menu.h`, `lcd.h`, `menu_chinese.h`, `manual_control.h`, `auto_mode.h`, `stdio.h`。
定义：`Menu_ShowAscii`, `Menu_ShowTextId`, `Menu_DrawHeader`, `Menu_DrawFooter`, `Menu_OptionCount`, `Menu_OptionText`, `Menu_DrawOptionRow`, `Menu_DrawOptions`, `Menu_UpdateSelection`, `Menu_DrawHome`, `Menu_DrawShape`, `Menu_DrawVoltage`, `Menu_DrawManualRunFrame`, `Menu_ShapeText`, `Menu_DrawAutoPlaceholder`, `Menu_DrawBypassActive`, `Menu_Redraw`, `Menu_Init`, `Menu_HandleKey`, `Menu_FormatFrequency`, `Menu_FormatPhase`, `Menu_DrawRuntimeLabel`, `Menu_DiagnosticDisplayCount`, `Menu_RuntimeInitField`, `Menu_RuntimeInitLabelField`, `Menu_RuntimePrepareLayout`, `Menu_RuntimeSetText`, `Menu_RuntimeRenderOneCharacter`, `Menu_RuntimeUpdateModel`, `Menu_RefreshRuntime`, `Menu_RefreshRuntime`。

## `firmware/F103_Project/CMSIS/CM3/CORE/core_cm3.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stdint.h`。
定义：`__get_PSP`, `__set_PSP`, `__get_MSP`, `__set_MSP`, `__REV16`, `__RBIT`, `__LDREXB`, `__LDREXH`, `__LDREXW`, `__STREXB`, `__STREXH`, `__STREXW`, `__get_PSP`, `__set_PSP`, `__get_MSP`, `__set_MSP`, `__get_BASEPRI`, `__set_BASEPRI`, `__get_PRIMASK`, `__set_PRIMASK`, `__get_FAULTMASK`, `__set_FAULTMASK`, `__get_CONTROL`, `__set_CONTROL`, `__REV`, `__REV16`, `__REVSH`, `__RBIT`, `__LDREXB`, `__LDREXH`, `__LDREXW`, `__STREXB`, `__STREXH`, `__STREXW`。

## `firmware/F103_Project/CMSIS/CM3/CORE/core_cm3.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stdint.h`, `intrinsics.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/CM3/stm32f10x.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`core_cm3.h`, `system_stm32f10x.h`, `stdint.h`, `stm32f10x_conf.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/CM3/stm32f10x_conf.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x_adc.h`, `stm32f10x_bkp.h`, `stm32f10x_can.h`, `stm32f10x_cec.h`, `stm32f10x_crc.h`, `stm32f10x_dac.h`, `stm32f10x_dbgmcu.h`, `stm32f10x_dma.h`, `stm32f10x_exti.h`, `stm32f10x_flash.h`, `stm32f10x_gpio.h`, `stm32f10x_iwdg.h`, `stm32f10x_pwr.h`, `stm32f10x_rcc.h`, `stm32f10x_rtc.h`, `stm32f10x_spi.h`, `stm32f10x_tim.h`, `stm32f10x_usart.h`, `stm32f10x_wwdg.h`, `misc.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/CM3/stm32f10x_it.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_it.h`, `delay.h`, `sys.h`, `stm32f10x_tim.h`。
定义：`NMI_Handler`, `HardFault_Handler`, `MemManage_Handler`, `BusFault_Handler`, `UsageFault_Handler`, `SVC_Handler`, `DebugMon_Handler`, `PendSV_Handler`, `SysTick_Handler`, `TIM3_IRQHandler`。

## `firmware/F103_Project/CMSIS/CM3/stm32f10x_it.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/CM3/system_stm32f10x.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x.h`。
定义：`SystemInit`, `SystemCoreClockUpdate`, `SetSysClock`, `SystemInit_ExtMemCtl`, `SetSysClockToHSE`, `SetSysClockTo24`, `SetSysClockTo36`, `SetSysClockTo48`, `SetSysClockTo56`, `SetSysClockTo72`。

## `firmware/F103_Project/CMSIS/CM3/system_stm32f10x.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：无显式 include。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/misc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_adc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_bkp.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_can.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_cec.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_crc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_dac.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_dbgmcu.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_dma.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_exti.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_flash.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_fsmc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_gpio.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_i2c.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_iwdg.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_pwr.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_rcc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_rtc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_sdio.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_spi.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_tim.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_usart.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/inc/stm32f10x_wwdg.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/CMSIS/STM32LIB/src/misc.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`misc.h`。
定义：`NVIC_PriorityGroupConfig`, `NVIC_Init`, `NVIC_SetVectorTable`, `NVIC_SystemLPConfig`, `SysTick_CLKSourceConfig`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_adc.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_adc.h`, `stm32f10x_rcc.h`。
定义：`ADC_DeInit`, `ADC_Init`, `ADC_StructInit`, `ADC_Cmd`, `ADC_DMACmd`, `ADC_ITConfig`, `ADC_ResetCalibration`, `ADC_GetResetCalibrationStatus`, `ADC_StartCalibration`, `ADC_GetCalibrationStatus`, `ADC_SoftwareStartConvCmd`, `ADC_GetSoftwareStartConvStatus`, `ADC_DiscModeChannelCountConfig`, `ADC_DiscModeCmd`, `ADC_RegularChannelConfig`, `ADC_ExternalTrigConvCmd`, `ADC_GetConversionValue`, `ADC_GetDualModeConversionValue`, `ADC_AutoInjectedConvCmd`, `ADC_InjectedDiscModeCmd`, `ADC_ExternalTrigInjectedConvConfig`, `ADC_ExternalTrigInjectedConvCmd`, `ADC_SoftwareStartInjectedConvCmd`, `ADC_GetSoftwareStartInjectedConvCmdStatus`, `ADC_InjectedChannelConfig`, `ADC_InjectedSequencerLengthConfig`, `ADC_SetInjectedOffset`, `ADC_GetInjectedConversionValue`, `ADC_AnalogWatchdogCmd`, `ADC_AnalogWatchdogThresholdsConfig`, `ADC_AnalogWatchdogSingleChannelConfig`, `ADC_TempSensorVrefintCmd`, `ADC_GetFlagStatus`, `ADC_ClearFlag`, `ADC_GetITStatus`, `ADC_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_bkp.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_bkp.h`, `stm32f10x_rcc.h`。
定义：`BKP_DeInit`, `BKP_TamperPinLevelConfig`, `BKP_TamperPinCmd`, `BKP_ITConfig`, `BKP_RTCOutputConfig`, `BKP_SetRTCCalibrationValue`, `BKP_WriteBackupRegister`, `BKP_ReadBackupRegister`, `BKP_GetFlagStatus`, `BKP_ClearFlag`, `BKP_GetITStatus`, `BKP_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_can.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_can.h`, `stm32f10x_rcc.h`。
定义：`CAN_DeInit`, `CAN_Init`, `CAN_FilterInit`, `CAN_StructInit`, `CAN_SlaveStartBank`, `CAN_DBGFreeze`, `CAN_TTComModeCmd`, `CAN_Transmit`, `CAN_TransmitStatus`, `CAN_CancelTransmit`, `CAN_Receive`, `CAN_FIFORelease`, `CAN_MessagePending`, `CAN_OperatingModeRequest`, `CAN_Sleep`, `CAN_WakeUp`, `CAN_GetLastErrorCode`, `CAN_GetReceiveErrorCounter`, `CAN_GetLSBTransmitErrorCounter`, `CAN_ITConfig`, `CAN_GetFlagStatus`, `CAN_ClearFlag`, `CAN_GetITStatus`, `CAN_ClearITPendingBit`, `CheckITStatus`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_cec.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_cec.h`, `stm32f10x_rcc.h`。
定义：`CEC_DeInit`, `CEC_Init`, `CEC_Cmd`, `CEC_ITConfig`, `CEC_OwnAddressConfig`, `CEC_SetPrescaler`, `CEC_SendDataByte`, `CEC_ReceiveDataByte`, `CEC_StartOfMessage`, `CEC_EndOfMessageCmd`, `CEC_GetFlagStatus`, `CEC_ClearFlag`, `CEC_GetITStatus`, `CEC_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_crc.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_crc.h`。
定义：`CRC_ResetDR`, `CRC_CalcCRC`, `CRC_CalcBlockCRC`, `CRC_GetCRC`, `CRC_SetIDRegister`, `CRC_GetIDRegister`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_dac.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_dac.h`, `stm32f10x_rcc.h`。
定义：`DAC_DeInit`, `DAC_Init`, `DAC_StructInit`, `DAC_Cmd`, `DAC_ITConfig`, `DAC_DMACmd`, `DAC_SoftwareTriggerCmd`, `DAC_DualSoftwareTriggerCmd`, `DAC_WaveGenerationCmd`, `DAC_SetChannel1Data`, `DAC_SetChannel2Data`, `DAC_SetDualChannelData`, `DAC_GetDataOutputValue`, `DAC_GetFlagStatus`, `DAC_ClearFlag`, `DAC_GetITStatus`, `DAC_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_dbgmcu.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_dbgmcu.h`。
定义：`DBGMCU_GetREVID`, `DBGMCU_GetDEVID`, `DBGMCU_Config`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_dma.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_dma.h`, `stm32f10x_rcc.h`。
定义：`DMA_DeInit`, `DMA_Init`, `DMA_StructInit`, `DMA_Cmd`, `DMA_ITConfig`, `DMA_SetCurrDataCounter`, `DMA_GetCurrDataCounter`, `DMA_GetFlagStatus`, `DMA_ClearFlag`, `DMA_GetITStatus`, `DMA_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_exti.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_exti.h`。
定义：`EXTI_DeInit`, `EXTI_Init`, `EXTI_StructInit`, `EXTI_GenerateSWInterrupt`, `EXTI_GetFlagStatus`, `EXTI_ClearFlag`, `EXTI_GetITStatus`, `EXTI_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_flash.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_flash.h`。
定义：`FLASH_SetLatency`, `FLASH_HalfCycleAccessCmd`, `FLASH_PrefetchBufferCmd`, `FLASH_Unlock`, `FLASH_UnlockBank1`, `FLASH_UnlockBank2`, `FLASH_Lock`, `FLASH_LockBank1`, `FLASH_LockBank2`, `FLASH_ErasePage`, `FLASH_EraseAllPages`, `FLASH_EraseAllBank1Pages`, `FLASH_EraseAllBank2Pages`, `FLASH_EraseOptionBytes`, `FLASH_ProgramWord`, `FLASH_ProgramHalfWord`, `FLASH_ProgramOptionByteData`, `FLASH_EnableWriteProtection`, `FLASH_ReadOutProtection`, `FLASH_UserOptionByteConfig`, `FLASH_BootConfig`, `FLASH_GetUserOptionByte`, `FLASH_GetWriteProtectionOptionByte`, `FLASH_GetReadOutProtectionStatus`, `FLASH_GetPrefetchBufferStatus`, `FLASH_ITConfig`, `FLASH_GetFlagStatus`, `FLASH_ClearFlag`, `FLASH_GetStatus`, `FLASH_GetBank1Status`, `FLASH_GetBank2Status`, `FLASH_WaitForLastOperation`, `FLASH_WaitForLastBank1Operation`, `FLASH_WaitForLastBank2Operation`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_fsmc.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_fsmc.h`, `stm32f10x_rcc.h`。
定义：`FSMC_NORSRAMDeInit`, `FSMC_NANDDeInit`, `FSMC_PCCARDDeInit`, `FSMC_NORSRAMInit`, `FSMC_NANDInit`, `FSMC_PCCARDInit`, `FSMC_NORSRAMStructInit`, `FSMC_NANDStructInit`, `FSMC_PCCARDStructInit`, `FSMC_NORSRAMCmd`, `FSMC_NANDCmd`, `FSMC_PCCARDCmd`, `FSMC_NANDECCCmd`, `FSMC_GetECC`, `FSMC_ITConfig`, `FSMC_GetFlagStatus`, `FSMC_ClearFlag`, `FSMC_GetITStatus`, `FSMC_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_gpio.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_gpio.h`, `stm32f10x_rcc.h`。
定义：`GPIO_DeInit`, `GPIO_AFIODeInit`, `GPIO_Init`, `GPIO_StructInit`, `GPIO_ReadInputDataBit`, `GPIO_ReadInputData`, `GPIO_ReadOutputDataBit`, `GPIO_ReadOutputData`, `GPIO_SetBits`, `GPIO_ResetBits`, `GPIO_WriteBit`, `GPIO_Write`, `GPIO_PinLockConfig`, `GPIO_EventOutputConfig`, `GPIO_EventOutputCmd`, `GPIO_PinRemapConfig`, `GPIO_EXTILineConfig`, `GPIO_ETH_MediaInterfaceConfig`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_i2c.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_i2c.h`, `stm32f10x_rcc.h`。
定义：`I2C_DeInit`, `I2C_Init`, `I2C_StructInit`, `I2C_Cmd`, `I2C_DMACmd`, `I2C_DMALastTransferCmd`, `I2C_GenerateSTART`, `I2C_GenerateSTOP`, `I2C_AcknowledgeConfig`, `I2C_OwnAddress2Config`, `I2C_DualAddressCmd`, `I2C_GeneralCallCmd`, `I2C_ITConfig`, `I2C_SendData`, `I2C_ReceiveData`, `I2C_Send7bitAddress`, `I2C_ReadRegister`, `I2C_SoftwareResetCmd`, `I2C_NACKPositionConfig`, `I2C_SMBusAlertConfig`, `I2C_TransmitPEC`, `I2C_PECPositionConfig`, `I2C_CalculatePEC`, `I2C_GetPEC`, `I2C_ARPCmd`, `I2C_StretchClockCmd`, `I2C_FastModeDutyCycleConfig`, `I2C_CheckEvent`, `I2C_GetLastEvent`, `I2C_GetFlagStatus`, `I2C_ClearFlag`, `I2C_GetITStatus`, `I2C_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_iwdg.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_iwdg.h`。
定义：`IWDG_WriteAccessCmd`, `IWDG_SetPrescaler`, `IWDG_SetReload`, `IWDG_ReloadCounter`, `IWDG_Enable`, `IWDG_GetFlagStatus`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_pwr.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_pwr.h`, `stm32f10x_rcc.h`。
定义：`PWR_DeInit`, `PWR_BackupAccessCmd`, `PWR_PVDCmd`, `PWR_PVDLevelConfig`, `PWR_WakeUpPinCmd`, `PWR_EnterSTOPMode`, `PWR_EnterSTANDBYMode`, `PWR_GetFlagStatus`, `PWR_ClearFlag`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_rcc.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_rcc.h`。
定义：`RCC_DeInit`, `RCC_HSEConfig`, `RCC_WaitForHSEStartUp`, `RCC_AdjustHSICalibrationValue`, `RCC_HSICmd`, `RCC_PLLConfig`, `RCC_PLLCmd`, `RCC_PREDIV1Config`, `RCC_PREDIV2Config`, `RCC_PLL2Config`, `RCC_PLL2Cmd`, `RCC_PLL3Config`, `RCC_PLL3Cmd`, `RCC_SYSCLKConfig`, `RCC_GetSYSCLKSource`, `RCC_HCLKConfig`, `RCC_PCLK1Config`, `RCC_PCLK2Config`, `RCC_ITConfig`, `RCC_USBCLKConfig`, `RCC_OTGFSCLKConfig`, `RCC_ADCCLKConfig`, `RCC_I2S2CLKConfig`, `RCC_I2S3CLKConfig`, `RCC_LSEConfig`, `RCC_LSICmd`, `RCC_RTCCLKConfig`, `RCC_RTCCLKCmd`, `RCC_GetClocksFreq`, `RCC_AHBPeriphClockCmd`, `RCC_APB2PeriphClockCmd`, `RCC_APB1PeriphClockCmd`, `RCC_AHBPeriphResetCmd`, `RCC_APB2PeriphResetCmd`, `RCC_APB1PeriphResetCmd`, `RCC_BackupResetCmd`, `RCC_ClockSecuritySystemCmd`, `RCC_MCOConfig`, `RCC_GetFlagStatus`, `RCC_ClearFlag`, `RCC_GetITStatus`, `RCC_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_rtc.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_rtc.h`。
定义：`RTC_ITConfig`, `RTC_EnterConfigMode`, `RTC_ExitConfigMode`, `RTC_GetCounter`, `RTC_SetCounter`, `RTC_SetPrescaler`, `RTC_SetAlarm`, `RTC_GetDivider`, `RTC_WaitForLastTask`, `RTC_WaitForSynchro`, `RTC_GetFlagStatus`, `RTC_ClearFlag`, `RTC_GetITStatus`, `RTC_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_sdio.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_sdio.h`, `stm32f10x_rcc.h`。
定义：`SDIO_DeInit`, `SDIO_Init`, `SDIO_StructInit`, `SDIO_ClockCmd`, `SDIO_SetPowerState`, `SDIO_GetPowerState`, `SDIO_ITConfig`, `SDIO_DMACmd`, `SDIO_SendCommand`, `SDIO_CmdStructInit`, `SDIO_GetCommandResponse`, `SDIO_GetResponse`, `SDIO_DataConfig`, `SDIO_DataStructInit`, `SDIO_GetDataCounter`, `SDIO_ReadData`, `SDIO_WriteData`, `SDIO_GetFIFOCount`, `SDIO_StartSDIOReadWait`, `SDIO_StopSDIOReadWait`, `SDIO_SetSDIOReadWaitMode`, `SDIO_SetSDIOOperation`, `SDIO_SendSDIOSuspendCmd`, `SDIO_CommandCompletionCmd`, `SDIO_CEATAITCmd`, `SDIO_SendCEATACmd`, `SDIO_GetFlagStatus`, `SDIO_ClearFlag`, `SDIO_GetITStatus`, `SDIO_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_spi.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_spi.h`, `stm32f10x_rcc.h`。
定义：`SPI_I2S_DeInit`, `SPI_Init`, `I2S_Init`, `SPI_StructInit`, `I2S_StructInit`, `SPI_Cmd`, `I2S_Cmd`, `SPI_I2S_ITConfig`, `SPI_I2S_DMACmd`, `SPI_I2S_SendData`, `SPI_I2S_ReceiveData`, `SPI_NSSInternalSoftwareConfig`, `SPI_SSOutputCmd`, `SPI_DataSizeConfig`, `SPI_TransmitCRC`, `SPI_CalculateCRC`, `SPI_GetCRC`, `SPI_GetCRCPolynomial`, `SPI_BiDirectionalLineConfig`, `SPI_I2S_GetFlagStatus`, `SPI_I2S_ClearFlag`, `SPI_I2S_GetITStatus`, `SPI_I2S_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_tim.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_tim.h`, `stm32f10x_rcc.h`。
定义：`TIM_DeInit`, `TIM_TimeBaseInit`, `TIM_OC1Init`, `TIM_OC2Init`, `TIM_OC3Init`, `TIM_OC4Init`, `TIM_ICInit`, `TIM_PWMIConfig`, `TIM_BDTRConfig`, `TIM_TimeBaseStructInit`, `TIM_OCStructInit`, `TIM_ICStructInit`, `TIM_BDTRStructInit`, `TIM_Cmd`, `TIM_CtrlPWMOutputs`, `TIM_ITConfig`, `TIM_GenerateEvent`, `TIM_DMAConfig`, `TIM_DMACmd`, `TIM_InternalClockConfig`, `TIM_ITRxExternalClockConfig`, `TIM_TIxExternalClockConfig`, `TIM_ETRClockMode1Config`, `TIM_ETRClockMode2Config`, `TIM_ETRConfig`, `TIM_PrescalerConfig`, `TIM_CounterModeConfig`, `TIM_SelectInputTrigger`, `TIM_EncoderInterfaceConfig`, `TIM_ForcedOC1Config`, `TIM_ForcedOC2Config`, `TIM_ForcedOC3Config`, `TIM_ForcedOC4Config`, `TIM_ARRPreloadConfig`, `TIM_SelectCOM`, `TIM_SelectCCDMA`, `TIM_CCPreloadControl`, `TIM_OC1PreloadConfig`, `TIM_OC2PreloadConfig`, `TIM_OC3PreloadConfig`, `TIM_OC4PreloadConfig`, `TIM_OC1FastConfig`, `TIM_OC2FastConfig`, `TIM_OC3FastConfig`, `TIM_OC4FastConfig`, `TIM_ClearOC1Ref`, `TIM_ClearOC2Ref`, `TIM_ClearOC3Ref`, `TIM_ClearOC4Ref`, `TIM_OC1PolarityConfig`, `TIM_OC1NPolarityConfig`, `TIM_OC2PolarityConfig`, `TIM_OC2NPolarityConfig`, `TIM_OC3PolarityConfig`, `TIM_OC3NPolarityConfig`, `TIM_OC4PolarityConfig`, `TIM_CCxCmd`, `TIM_CCxNCmd`, `TIM_SelectOCxM`, `TIM_UpdateDisableConfig`, `TIM_UpdateRequestConfig`, `TIM_SelectHallSensor`, `TIM_SelectOnePulseMode`, `TIM_SelectOutputTrigger`, `TIM_SelectSlaveMode`, `TIM_SelectMasterSlaveMode`, `TIM_SetCounter`, `TIM_SetAutoreload`, `TIM_SetCompare1`, `TIM_SetCompare2`, `TIM_SetCompare3`, `TIM_SetCompare4`, `TIM_SetIC1Prescaler`, `TIM_SetIC2Prescaler`, `TIM_SetIC3Prescaler`, `TIM_SetIC4Prescaler`, `TIM_SetClockDivision`, `TIM_GetCapture1`, `TIM_GetCapture2`, `TIM_GetCapture3`, `TIM_GetCapture4`, `TIM_GetCounter`, `TIM_GetPrescaler`, `TIM_GetFlagStatus`, `TIM_ClearFlag`, `TIM_GetITStatus`, `TIM_ClearITPendingBit`, `TI1_Config`, `TI2_Config`, `TI3_Config`, `TI4_Config`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_usart.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_usart.h`, `stm32f10x_rcc.h`。
定义：`USART_DeInit`, `USART_Init`, `USART_StructInit`, `USART_ClockInit`, `USART_ClockStructInit`, `USART_Cmd`, `USART_ITConfig`, `USART_DMACmd`, `USART_SetAddress`, `USART_WakeUpConfig`, `USART_ReceiverWakeUpCmd`, `USART_LINBreakDetectLengthConfig`, `USART_LINCmd`, `USART_SendData`, `USART_ReceiveData`, `USART_SendBreak`, `USART_SetGuardTime`, `USART_SetPrescaler`, `USART_SmartCardCmd`, `USART_SmartCardNACKCmd`, `USART_HalfDuplexCmd`, `USART_OverSampling8Cmd`, `USART_OneBitMethodCmd`, `USART_IrDAConfig`, `USART_IrDACmd`, `USART_GetFlagStatus`, `USART_ClearFlag`, `USART_GetITStatus`, `USART_ClearITPendingBit`。

## `firmware/F103_Project/CMSIS/STM32LIB/src/stm32f10x_wwdg.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f10x_wwdg.h`, `stm32f10x_rcc.h`。
定义：`WWDG_DeInit`, `WWDG_SetPrescaler`, `WWDG_SetWindowValue`, `WWDG_EnableIT`, `WWDG_SetCounter`, `WWDG_Enable`, `WWDG_GetFlagStatus`, `WWDG_ClearFlag`。

## `firmware/F103_Project/HARDWARE/ad9910/AD9910.H`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`, `sys.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/ad9910/AD9910V1.C`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`stm32f10x.h`, `AD9910.h`, `delay.h`, `sys.h`。
定义：`AD9110_IOInit`, `txd_8bit`, `Write_32bit`, `Init_AD9910`, `Txfrc`, `AD9910_FreWrite`, `AD9910_AmpWrite`, `AD9910_PhaseWrite`, `AD9910_FtwFromMilliHz`, `AD9910_PowFromMilliDegrees`, `AD9910_ProfileWriteRaw`, `AD9910_SetToneMilliHz`, `AD9910_IsPllLocked`, `AD9910_RAM_WAVE_Set`, `AD9910_DRG_FreInit_AutoSet`, `AD9910_DRG_FrePara_Set`。

## `firmware/F103_Project/HARDWARE/BOARD/board.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`board.h`。
定义：`Board_KeyStateInit`, `Board_KeyPressed`, `Board_Init`, `Board_CalibrateConfirmIdle`, `Board_KeysPoll1ms`, `Board_GetConfirmIdleLevel`, `Board_MarkerSet`, `Board_MarkerToggle`, `Board_RelaySet`, `Board_RelayIsOn`。

## `firmware/F103_Project/HARDWARE/BOARD/board.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/CAPTURE/signal_capture.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`signal_capture.h`, `usart.h`。
定义：`Link_IncrementSaturated`, `Link_QueueSnapshot`, `Link_GetU16`, `Link_GetU32`, `Link_Crc16`, `Link_AcceptFrame`, `Link_ProcessByte`, `SignalCapture_Init`, `SignalCapture_Service`, `SignalCapture_Tick1ms`, `SignalCapture_TickMs`, `SignalCapture_GetSnapshot`, `SignalCapture_TakeSnapshot`, `SignalCapture_GetDiagnostics`, `SignalCapture_ClearDiagnostics`, `SignalCapture_CopyReferenceWaveform`。

## `firmware/F103_Project/HARDWARE/CAPTURE/signal_capture.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/LCD/Dis_Picture.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`Dis_Picture.h`。
定义：`Show_Picture`。

## `firmware/F103_Project/HARDWARE/LCD/Dis_Picture.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`lcd.h`, `Text.h`, `string.h`, `Picture.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/LCD/FONT.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`font.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/LCD/FONT.H`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：无显式 include。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/LCD/lcd.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`lcd.h`, `stdlib.h`, `font.h`, `usart.h`, `Text.h`, `Dis_Picture.h`。
定义：`SPI2_Init`, `LCD_GPIO_Init`, `SPI2_ReadWriteByte`, `LCD_WR_REG`, `LCD_WR_DATA16`, `LCD_WriteReg`, `LCD_WriteRAM_Prepare`, `LCD_WriteRAM`, `LCD_DisplayOn`, `LCD_DisplayOff`, `LCD_SoftRest`, `LCD_HardwareRest`, `LCD_SetCursor`, `LCD_DrawPoint`, `LCD_Fast_DrawPoint`, `LCD_Scan_Dir`, `LCD_Display_Dir`, `LCD_Set_Window`, `LCD_Init`, `LCD_Clear`, `LCD_Fill`, `LCD_Color_Fill`, `LCD_DrawLine`, `LCD_DrawRectangle`, `LCD_Draw_Circle`, `LCD_ShowChar`, `LCD_Pow`, `LCD_ShowNum`, `LCD_ShowxNum`, `LCD_ShowString`, `DisplayButtonDown`, `DisplayButtonUp`, `Draw_Test`, `Color_Test`, `Font_Test`, `Demo_Menu`。

## `firmware/F103_Project/HARDWARE/LCD/lcd.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`sys.h`, `stdlib.h`, `delay.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/LCD/lcd_stubs.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：无显式 include。
定义：`GBK_LibFont_Test`。

## `firmware/F103_Project/HARDWARE/LCD/Picture.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`Picture.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/LCD/Picture.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`lcd.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/LCD/Text.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`sys.h`, `lcd.h`, `Text.h`, `string.h`, `usart.h`。
定义：`Copy_Mem`, `Copy_HZK16`, `Copy_HZK24`, `Get_HzMat`, `Show_Font`, `Show_Str`, `Draw_Font16B`, `Draw_Font24B`。

## `firmware/F103_Project/HARDWARE/LCD/Text.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`sys.h`, `font.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/HARDWARE/STM32_config.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`sys.h`, `stm32f10x.h`, `string.h`, `math.h`, `delay.h`, `usart.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/SYSTEM/debug_uart/debug_uart.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`debug_uart.h`, `sys.h`。
定义：`DebugUart_StartDma`, `DebugUart_Init`, `DebugUart_Send`, `DebugUart_GetDroppedMessages`, `DMA1_Channel7_IRQHandler`。

## `firmware/F103_Project/SYSTEM/debug_uart/debug_uart.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/SYSTEM/delay/delay.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`delay.h`。
定义：`delay_init`, `delay_us`, `delay_ms`, `delay_init`, `delay_us`。

## `firmware/F103_Project/SYSTEM/delay/delay.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`sys.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/SYSTEM/sys/sys.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`sys.h`。
定义：`STM32_Flash_Capacity`, `STM32_CPUID`, `MY_NVIC_SetVectorTable`, `MY_NVIC_PriorityGroup_Config`, `MY_NVIC_Init`, `MY_RCC_DeInit`, `SYS_Standby`, `SYS_SoftReset`, `STM_JTAG_Set`, `STM_Clock_Init`, `BCD_to_HEX`, `HEX_to_BCD`, `DX_to_HX`, `HX_to_DX`。

## `firmware/F103_Project/SYSTEM/sys/sys.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f10x.h`, `string.h`, `stdio.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/SYSTEM/usart/usart.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`usart.h`。
定义：`_sys_exit`, `fputc`, `USARTx_Init`, `USART1_GetWriteTotal`, `USART1_ReadByte`, `USART1_GetOverflowCount`, `USART1_ClearOverflowCount`, `DMA1_Channel5_IRQHandler`, `USART1_IRQHandler`。

## `firmware/F103_Project/SYSTEM/usart/usart.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stdio.h`, `sys.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/USER/app_config.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：无显式 include。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F103_Project/USER/main.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`stm32_config.h`, `stdio.h`, `usart.h`, `debug_uart.h`, `board.h`, `AD9910.h`, `signal_capture.h`, `manual_control.h`, `auto_mode.h`, `menu.h`, `app_config.h`。
定义：`App_TimebaseInit`, `TIM6_IRQHandler`, `App_TimeReached`, `App_SendCsvHeader`, `App_SendDiagnosticCsv`, `main`。

## `firmware/F407_Project/Drive/Include/delay.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/Drive/Include/led.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/Drive/Source/delay.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`delay.h`。
定义：`Delay_Init`, `TimingDelay_Decrement`, `Delay_ms`。

## `firmware/F407_Project/Drive/Source/led.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`led.h`。
定义：`LED_Init`。

## `firmware/F407_Project/MY/app_config.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：无显式 include。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/MY/main.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`stm32f4xx.h`, `led.h`, `measurement.h`, `protocol.h`。
定义：`main`。

## `firmware/F407_Project/MY/measurement.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`measurement.h`, `app_config.h`, `math.h`, `string.h`。
定义：`Measurement_GetSample`, `Measurement_AnalyzeChannel`, `Measurement_AbsDiff`, `Measurement_WrapPhase`, `Measurement_CalculateIqPhase`, `Measurement_CalculateRelativePhase`, `Measurement_Init`, `Measurement_TakeReadyFrame`, `Measurement_Analyze`, `Measurement_MarkReadyFrame`, `Measurement_DmaIrqHandler`。

## `firmware/F407_Project/MY/measurement.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/MY/protocol.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`protocol.h`, `app_config.h`。
定义：`Protocol_PutU16`, `Protocol_PutU32`, `Protocol_Crc16`, `Protocol_StartDma`, `Protocol_BuildFrame`, `Protocol_Init`, `Protocol_SendMeasurement`, `Protocol_DmaIrqHandler`。

## `firmware/F407_Project/MY/protocol.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`measurement.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/MY/stm32f4xx_conf.h`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`stm32f4xx_adc.h`, `stm32f4xx_crc.h`, `stm32f4xx_dbgmcu.h`, `stm32f4xx_dma.h`, `stm32f4xx_exti.h`, `stm32f4xx_flash.h`, `stm32f4xx_gpio.h`, `stm32f4xx_i2c.h`, `stm32f4xx_iwdg.h`, `stm32f4xx_pwr.h`, `stm32f4xx_rcc.h`, `stm32f4xx_rtc.h`, `stm32f4xx_sdio.h`, `stm32f4xx_spi.h`, `stm32f4xx_syscfg.h`, `stm32f4xx_tim.h`, `stm32f4xx_usart.h`, `stm32f4xx_wwdg.h`, `misc.h`, `stm32f4xx_cryp.h`, `stm32f4xx_hash.h`, `stm32f4xx_rng.h`, `stm32f4xx_can.h`, `stm32f4xx_dac.h`, `stm32f4xx_dcmi.h`, `stm32f4xx_dma2d.h`, `stm32f4xx_fmc.h`, `stm32f4xx_ltdc.h`, `stm32f4xx_sai.h`, `stm32f4xx_cryp.h`, `stm32f4xx_hash.h`, `stm32f4xx_rng.h`, `stm32f4xx_can.h`, `stm32f4xx_dac.h`, `stm32f4xx_dcmi.h`, `stm32f4xx_dma2d.h`, `stm32f4xx_fmc.h`, `stm32f4xx_sai.h`, `stm32f4xx_cryp.h`, `stm32f4xx_hash.h`, `stm32f4xx_rng.h`, `stm32f4xx_can.h`, `stm32f4xx_dac.h`, `stm32f4xx_dcmi.h`, `stm32f4xx_fsmc.h`, `stm32f4xx_rng.h`, `stm32f4xx_dac.h`, `stm32f4xx_flash_ramfunc.h`, `stm32f4xx_qspi.h`, `stm32f4xx_fmpi2c.h`, `stm32f4xx_spdifrx.h`, `stm32f4xx_cec.h`, `stm32f4xx_dsi.h`, `stm32f4xx_lptim.h`, `stm32f4xx_rng.h`, `stm32f4xx_can.h`, `stm32f4xx_qspi.h`, `stm32f4xx_rng.h`, `stm32f4xx_fsmc.h`, `stm32f4xx_dfsdm.h`, `stm32f4xx_cryp.h`, `stm32f4xx_fmpi2c.h`, `stm32f4xx_rng.h`, `stm32f4xx_can.h`, `stm32f4xx_qspi.h`, `stm32f4xx_rng.h`, `stm32f4xx_fsmc.h`, `stm32f4xx_dfsdm.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/MY/stm32f4xx_it.c`
分类：应用/板级/显示/配置支持；Keil 直接引用。
依赖：`stm32f4xx_it.h`, `measurement.h`, `protocol.h`。
定义：`NMI_Handler`, `HardFault_Handler`, `MemManage_Handler`, `BusFault_Handler`, `UsageFault_Handler`, `SVC_Handler`, `DebugMon_Handler`, `PendSV_Handler`, `SysTick_Handler`, `DMA2_Stream0_IRQHandler`, `DMA2_Stream7_IRQHandler`。

## `firmware/F407_Project/MY/stm32f4xx_it.h`
分类：应用/板级/显示/配置支持；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Device/ST/STM32F4xx/Include/stm32f4xx.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`core_cm4.h`, `system_stm32f4xx.h`, `stdint.h`, `stm32f4xx_conf.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Device/ST/STM32F4xx/Include/system_stm32f4xx.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：无显式 include。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Device/ST/STM32F4xx/Source/Templates/system_stm32f4xx.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f4xx.h`。
定义：`SystemInit`, `SystemCoreClockUpdate`, `SetSysClock`, `SystemInit_ExtMemCtl`, `SystemInit_ExtMemCtl`, `SystemInit_ExtMemCtl`。

## `firmware/F407_Project/STM32/CMSIS/Include/arm_common_tables.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`arm_math.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/arm_const_structs.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`arm_math.h`, `arm_common_tables.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/arm_math.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`core_cm7.h`, `core_cm4.h`, `core_cm3.h`, `core_cm0.h`, `core_cm0plus.h`, `string.h`, `math.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_cm0.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stdint.h`, `core_cmInstr.h`, `core_cmFunc.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_cm0plus.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stdint.h`, `core_cmInstr.h`, `core_cmFunc.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_cm3.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stdint.h`, `core_cmInstr.h`, `core_cmFunc.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_cm4.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stdint.h`, `core_cmInstr.h`, `core_cmFunc.h`, `core_cmSimd.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_cm7.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stdint.h`, `core_cmInstr.h`, `core_cmFunc.h`, `core_cmSimd.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_cmFunc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`cmsis_iar.h`, `cmsis_ccs.h`, `cmsis_csm.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_cmInstr.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`cmsis_iar.h`, `cmsis_ccs.h`, `cmsis_csm.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_cmSimd.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`cmsis_iar.h`, `cmsis_ccs.h`, `cmsis_csm.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_sc000.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stdint.h`, `core_cmInstr.h`, `core_cmFunc.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/CMSIS/Include/core_sc300.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stdint.h`, `core_cmInstr.h`, `core_cmFunc.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/misc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_adc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_can.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_cec.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_crc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_cryp.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dac.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dbgmcu.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dcmi.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dfsdm.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dma.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dma2d.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_dsi.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_exti.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_flash.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_flash_ramfunc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_fmc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_fmpi2c.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_fsmc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_gpio.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_hash.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_i2c.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_iwdg.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_lptim.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_ltdc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_pwr.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_qspi.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rcc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rng.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_rtc.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_sai.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_sdio.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_spdifrx.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_spi.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_syscfg.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_tim.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_usart.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/inc/stm32f4xx_wwdg.h`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/misc.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`misc.h`。
定义：`NVIC_PriorityGroupConfig`, `NVIC_Init`, `NVIC_SetVectorTable`, `NVIC_SystemLPConfig`, `SysTick_CLKSourceConfig`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_adc.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f4xx_adc.h`, `stm32f4xx_rcc.h`。
定义：`ADC_DeInit`, `ADC_Init`, `ADC_StructInit`, `ADC_CommonInit`, `ADC_CommonStructInit`, `ADC_Cmd`, `ADC_AnalogWatchdogCmd`, `ADC_AnalogWatchdogThresholdsConfig`, `ADC_AnalogWatchdogSingleChannelConfig`, `ADC_TempSensorVrefintCmd`, `ADC_VBATCmd`, `ADC_RegularChannelConfig`, `ADC_SoftwareStartConv`, `ADC_GetSoftwareStartConvStatus`, `ADC_EOCOnEachRegularChannelCmd`, `ADC_ContinuousModeCmd`, `ADC_DiscModeChannelCountConfig`, `ADC_DiscModeCmd`, `ADC_GetConversionValue`, `ADC_GetMultiModeConversionValue`, `ADC_DMACmd`, `ADC_DMARequestAfterLastTransferCmd`, `ADC_MultiModeDMARequestAfterLastTransferCmd`, `ADC_InjectedChannelConfig`, `ADC_InjectedSequencerLengthConfig`, `ADC_SetInjectedOffset`, `ADC_ExternalTrigInjectedConvConfig`, `ADC_ExternalTrigInjectedConvEdgeConfig`, `ADC_SoftwareStartInjectedConv`, `ADC_GetSoftwareStartInjectedConvCmdStatus`, `ADC_AutoInjectedConvCmd`, `ADC_InjectedDiscModeCmd`, `ADC_GetInjectedConversionValue`, `ADC_ITConfig`, `ADC_GetFlagStatus`, `ADC_ClearFlag`, `ADC_GetITStatus`, `ADC_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_can.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_can.h`, `stm32f4xx_rcc.h`。
定义：`CAN_DeInit`, `CAN_Init`, `CAN_FilterInit`, `CAN_FilterInit`, `CAN_StructInit`, `CAN_SlaveStartBank`, `CAN_SlaveStartBank`, `CAN_DBGFreeze`, `CAN_TTComModeCmd`, `CAN_Transmit`, `CAN_TransmitStatus`, `CAN_CancelTransmit`, `CAN_Receive`, `CAN_FIFORelease`, `CAN_MessagePending`, `CAN_OperatingModeRequest`, `CAN_Sleep`, `CAN_WakeUp`, `CAN_GetLastErrorCode`, `CAN_GetReceiveErrorCounter`, `CAN_GetLSBTransmitErrorCounter`, `CAN_ITConfig`, `CAN_GetFlagStatus`, `CAN_ClearFlag`, `CAN_GetITStatus`, `CAN_ClearITPendingBit`, `CheckITStatus`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cec.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_cec.h`, `stm32f4xx_rcc.h`。
定义：`CEC_DeInit`, `CEC_Init`, `CEC_StructInit`, `CEC_Cmd`, `CEC_ListenModeCmd`, `CEC_OwnAddressConfig`, `CEC_OwnAddressClear`, `CEC_SendData`, `CEC_ReceiveData`, `CEC_StartOfMessage`, `CEC_EndOfMessage`, `CEC_ITConfig`, `CEC_GetFlagStatus`, `CEC_ClearFlag`, `CEC_GetITStatus`, `CEC_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_crc.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_crc.h`。
定义：`CRC_ResetDR`, `CRC_CalcCRC`, `CRC_CalcBlockCRC`, `CRC_GetCRC`, `CRC_SetIDRegister`, `CRC_GetIDRegister`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_cryp.h`, `stm32f4xx_rcc.h`。
定义：`CRYP_DeInit`, `CRYP_Init`, `CRYP_StructInit`, `CRYP_KeyInit`, `CRYP_KeyStructInit`, `CRYP_IVInit`, `CRYP_IVStructInit`, `CRYP_PhaseConfig`, `CRYP_FIFOFlush`, `CRYP_Cmd`, `CRYP_DataIn`, `CRYP_DataOut`, `CRYP_SaveContext`, `CRYP_RestoreContext`, `CRYP_DMACmd`, `CRYP_ITConfig`, `CRYP_GetITStatus`, `CRYP_GetCmdStatus`, `CRYP_GetFlagStatus`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_aes.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_cryp.h`。
定义：`CRYP_AES_ECB`, `CRYP_AES_CBC`, `CRYP_AES_CTR`, `CRYP_AES_GCM`, `CRYP_AES_CCM`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_des.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_cryp.h`。
定义：`CRYP_DES_ECB`, `CRYP_DES_CBC`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_cryp_tdes.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_cryp.h`。
定义：`CRYP_TDES_ECB`, `CRYP_TDES_CBC`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dac.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_dac.h`, `stm32f4xx_rcc.h`。
定义：`DAC_DeInit`, `DAC_Init`, `DAC_StructInit`, `DAC_Cmd`, `DAC_SoftwareTriggerCmd`, `DAC_DualSoftwareTriggerCmd`, `DAC_WaveGenerationCmd`, `DAC_SetChannel1Data`, `DAC_SetChannel2Data`, `DAC_SetDualChannelData`, `DAC_GetDataOutputValue`, `DAC_DMACmd`, `DAC_ITConfig`, `DAC_GetFlagStatus`, `DAC_ClearFlag`, `DAC_GetITStatus`, `DAC_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dbgmcu.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_dbgmcu.h`。
定义：`DBGMCU_GetREVID`, `DBGMCU_GetDEVID`, `DBGMCU_Config`, `DBGMCU_APB1PeriphConfig`, `DBGMCU_APB2PeriphConfig`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dcmi.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_dcmi.h`, `stm32f4xx_rcc.h`。
定义：`DCMI_DeInit`, `DCMI_Init`, `DCMI_StructInit`, `DCMI_CROPConfig`, `DCMI_CROPCmd`, `DCMI_SetEmbeddedSynchroCodes`, `DCMI_JPEGCmd`, `DCMI_Cmd`, `DCMI_CaptureCmd`, `DCMI_ReadData`, `DCMI_ITConfig`, `DCMI_GetFlagStatus`, `DCMI_ClearFlag`, `DCMI_GetITStatus`, `DCMI_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dfsdm.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_dfsdm.h`, `stm32f4xx_rcc.h`。
定义：`DFSDM_DeInit`, `DFSDM_TransceiverInit`, `DFSDM_TransceiverStructInit`, `DFSDM_FilterInit`, `DFSDM_FilterStructInit`, `DFSDM_Command`, `DFSDM_Cmd`, `DFSDM_ChannelCmd`, `DFSDM_FilterCmd`, `DFSDM_ConfigClkOutputDivider`, `DFSDM_ConfigClkOutputSource`, `DFSDM_ConfigClkOutputDivider`, `DFSDM_ConfigClkOutputSource`, `DFSDM_ConfigBRKAnalogWatchDog`, `DFSDM_ConfigBRKShortCircuitDetector`, `DFSDM_ConfigShortCircuitThreshold`, `DFSDM_ConfigAnalogWatchdog`, `DFSDM_SelectExtremesDetectorChannel`, `DFSDM_GetRegularConversionData`, `DFSDM_GetInjectedConversionData`, `DFSDM_GetMaxValue`, `DFSDM_GetMinValue`, `DFSDM_GetMaxValueChannel`, `DFSDM_GetMinValueChannel`, `DFSDM_GetConversionTime`, `DFSDM_ConfigAWDFilter`, `DFSDM_GetAWDConversionValue`, `DFSDM_SetAWDThreshold`, `DFSDM_SelectInjectedChannel`, `DFSDM_SelectRegularChannel`, `DFSDM_StartSoftwareInjectedConversion`, `DFSDM_StartSoftwareRegularConversion`, `DFSDM_ConfigInjectedTrigger`, `DFSDM_SynchronousFilter0InjectedStart`, `DFSDM_SynchronousFilter0RegularStart`, `DFSDM_RegularContinuousModeCmd`, `DFSDM_FastModeCmd`, `DFSDM_SelectInjectedConversionMode`, `DFSDM_DMATransferConfig`, `DFSDM_ITConfig`, `DFSDM_ITClockAbsenceCmd`, `DFSDM_ITShortCircuitDetectorCmd`, `DFSDM_ITClockAbsenceCmd`, `DFSDM_ITShortCircuitDetectorCmd`, `DFSDM_GetFlagStatus`, `DFSDM_GetClockAbsenceFlagStatus`, `DFSDM_GetShortCircuitFlagStatus`, `DFSDM_GetClockAbsenceFlagStatus`, `DFSDM_GetShortCircuitFlagStatus`, `DFSDM_GetWatchdogFlagStatus`, `DFSDM_ClearFlag`, `DFSDM_ClearClockAbsenceFlag`, `DFSDM_ClearShortCircuitFlag`, `DFSDM_ClearClockAbsenceFlag`, `DFSDM_ClearShortCircuitFlag`, `DFSDM_ClearAnalogWatchdogFlag`, `DFSDM_GetITStatus`, `DFSDM_GetClockAbsenceITStatus`, `DFSDM_GetShortCircuitITStatus`, `DFSDM_GetClockAbsenceITStatus`, `DFSDM_GetShortCircuitITStatus`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f4xx_dma.h`, `stm32f4xx_rcc.h`。
定义：`DMA_DeInit`, `DMA_Init`, `DMA_StructInit`, `DMA_Cmd`, `DMA_PeriphIncOffsetSizeConfig`, `DMA_FlowControllerConfig`, `DMA_SetCurrDataCounter`, `DMA_GetCurrDataCounter`, `DMA_DoubleBufferModeConfig`, `DMA_DoubleBufferModeCmd`, `DMA_MemoryTargetConfig`, `DMA_GetCurrentMemoryTarget`, `DMA_GetCmdStatus`, `DMA_GetFIFOStatus`, `DMA_GetFlagStatus`, `DMA_ClearFlag`, `DMA_ITConfig`, `DMA_GetITStatus`, `DMA_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_dma2d.h`, `stm32f4xx_rcc.h`。
定义：`DMA2D_DeInit`, `DMA2D_Init`, `DMA2D_StructInit`, `DMA2D_StartTransfer`, `DMA2D_AbortTransfer`, `DMA2D_Suspend`, `DMA2D_FGConfig`, `DMA2D_FG_StructInit`, `DMA2D_BGConfig`, `DMA2D_BG_StructInit`, `DMA2D_FGStart`, `DMA2D_BGStart`, `DMA2D_DeadTimeConfig`, `DMA2D_LineWatermarkConfig`, `DMA2D_ITConfig`, `DMA2D_GetFlagStatus`, `DMA2D_ClearFlag`, `DMA2D_GetITStatus`, `DMA2D_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dsi.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_dsi.h`。
定义：`DSI_DeInit`, `DSI_Init`, `DSI_StructInit`, `DSI_SetGenericVCID`, `DSI_ConfigVideoMode`, `DSI_ConfigAdaptedCommandMode`, `DSI_ConfigCommand`, `DSI_ConfigFlowControl`, `DSI_ConfigPhyTimer`, `DSI_ConfigHostTimeouts`, `DSI_Start`, `DSI_Stop`, `DSI_Refresh`, `DSI_ColorMode`, `DSI_Shutdown`, `DSI_ShortWrite`, `DSI_LongWrite`, `DSI_Read`, `DSI_ConfigPacketHeader`, `DSI_EnterULPMData`, `DSI_ExitULPMData`, `DSI_EnterULPM`, `DSI_ExitULPM`, `DSI_PatternGeneratorStart`, `DSI_PatternGeneratorStop`, `DSI_SetSlewRateAndDelayTuning`, `DSI_SetLowPowerRXFilter`, `DSI_SetSDD`, `DSI_SetLanePinsConfiguration`, `DSI_SetPHYTimings`, `DSI_ForceTXStopMode`, `DSI_ForceRXLowPower`, `DSI_ForceDataLanesInRX`, `DSI_SetPullDown`, `DSI_SetContentionDetectionOff`, `DSI_ITConfig`, `DSI_GetFlagStatus`, `DSI_ClearFlag`, `DSI_GetITStatus`, `DSI_ClearITPendingBit`, `DSI_ConfigErrorMonitor`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_exti.h`。
定义：`EXTI_DeInit`, `EXTI_Init`, `EXTI_StructInit`, `EXTI_GenerateSWInterrupt`, `EXTI_GetFlagStatus`, `EXTI_ClearFlag`, `EXTI_GetITStatus`, `EXTI_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_flash.h`。
定义：`FLASH_SetLatency`, `FLASH_PrefetchBufferCmd`, `FLASH_InstructionCacheCmd`, `FLASH_DataCacheCmd`, `FLASH_InstructionCacheReset`, `FLASH_DataCacheReset`, `FLASH_Unlock`, `FLASH_Lock`, `FLASH_EraseSector`, `FLASH_EraseAllSectors`, `FLASH_EraseAllBank1Sectors`, `FLASH_EraseAllBank2Sectors`, `FLASH_ProgramDoubleWord`, `FLASH_ProgramWord`, `FLASH_ProgramHalfWord`, `FLASH_ProgramByte`, `FLASH_OB_Unlock`, `FLASH_OB_Lock`, `FLASH_OB_WRPConfig`, `FLASH_OB_WRP1Config`, `FLASH_OB_PCROPSelectionConfig`, `FLASH_OB_PCROPConfig`, `FLASH_OB_PCROP1Config`, `FLASH_OB_RDPConfig`, `FLASH_OB_UserConfig`, `FLASH_OB_BootConfig`, `FLASH_OB_BORConfig`, `FLASH_OB_Launch`, `FLASH_OB_GetUser`, `FLASH_OB_GetWRP`, `FLASH_OB_GetWRP1`, `FLASH_OB_GetPCROP`, `FLASH_OB_GetPCROP1`, `FLASH_OB_GetRDP`, `FLASH_OB_GetBOR`, `FLASH_ITConfig`, `FLASH_GetFlagStatus`, `FLASH_ClearFlag`, `FLASH_GetStatus`, `FLASH_WaitForLastOperation`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_flash_ramfunc.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_flash_ramfunc.h`。
定义：宏、类型、数据或声明；无匹配函数定义。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_fmc.h`, `stm32f4xx_rcc.h`。
定义：`FMC_NORSRAMDeInit`, `FMC_NORSRAMInit`, `FMC_NORSRAMStructInit`, `FMC_NORSRAMCmd`, `FMC_NANDDeInit`, `FMC_NANDInit`, `FMC_NANDStructInit`, `FMC_NANDCmd`, `FMC_NANDECCCmd`, `FMC_GetECC`, `FMC_PCCARDDeInit`, `FMC_PCCARDInit`, `FMC_PCCARDStructInit`, `FMC_PCCARDCmd`, `FMC_SDRAMDeInit`, `FMC_SDRAMInit`, `FMC_SDRAMStructInit`, `FMC_SDRAMCmdConfig`, `FMC_GetModeStatus`, `FMC_SetRefreshCount`, `FMC_SetAutoRefresh_Number`, `FMC_SDRAMWriteProtectionConfig`, `FMC_ITConfig`, `FMC_GetFlagStatus`, `FMC_ClearFlag`, `FMC_GetITStatus`, `FMC_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmpi2c.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_fmpi2c.h`, `stm32f4xx_rcc.h`。
定义：`FMPI2C_DeInit`, `FMPI2C_Init`, `FMPI2C_StructInit`, `FMPI2C_Cmd`, `FMPI2C_SoftwareResetCmd`, `FMPI2C_ITConfig`, `FMPI2C_StretchClockCmd`, `FMPI2C_DualAddressCmd`, `FMPI2C_OwnAddress2Config`, `FMPI2C_GeneralCallCmd`, `FMPI2C_SlaveByteControlCmd`, `FMPI2C_SlaveAddressConfig`, `FMPI2C_10BitAddressingModeCmd`, `FMPI2C_AutoEndCmd`, `FMPI2C_ReloadCmd`, `FMPI2C_NumberOfBytesConfig`, `FMPI2C_MasterRequestConfig`, `FMPI2C_GenerateSTART`, `FMPI2C_GenerateSTOP`, `FMPI2C_10BitAddressHeaderCmd`, `FMPI2C_AcknowledgeConfig`, `FMPI2C_GetAddressMatched`, `FMPI2C_GetTransferDirection`, `FMPI2C_TransferHandling`, `FMPI2C_SMBusAlertCmd`, `FMPI2C_ClockTimeoutCmd`, `FMPI2C_ExtendedClockTimeoutCmd`, `FMPI2C_IdleClockTimeoutCmd`, `FMPI2C_TimeoutAConfig`, `FMPI2C_TimeoutBConfig`, `FMPI2C_CalculatePEC`, `FMPI2C_PECRequestCmd`, `FMPI2C_GetPEC`, `FMPI2C_ReadRegister`, `FMPI2C_SendData`, `FMPI2C_ReceiveData`, `FMPI2C_DMACmd`, `FMPI2C_GetFlagStatus`, `FMPI2C_ClearFlag`, `FMPI2C_GetITStatus`, `FMPI2C_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fsmc.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_fsmc.h`, `stm32f4xx_rcc.h`。
定义：`FSMC_NORSRAMDeInit`, `FSMC_NORSRAMInit`, `FSMC_NORSRAMStructInit`, `FSMC_NORSRAMCmd`, `FSMC_NANDDeInit`, `FSMC_NANDInit`, `FSMC_NANDStructInit`, `FSMC_NANDCmd`, `FSMC_NANDECCCmd`, `FSMC_GetECC`, `FSMC_PCCARDDeInit`, `FSMC_PCCARDInit`, `FSMC_PCCARDStructInit`, `FSMC_PCCARDCmd`, `FSMC_ITConfig`, `FSMC_GetFlagStatus`, `FSMC_ClearFlag`, `FSMC_GetITStatus`, `FSMC_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f4xx_gpio.h`, `stm32f4xx_rcc.h`。
定义：`GPIO_DeInit`, `GPIO_Init`, `GPIO_StructInit`, `GPIO_PinLockConfig`, `GPIO_ReadInputDataBit`, `GPIO_ReadInputData`, `GPIO_ReadOutputDataBit`, `GPIO_ReadOutputData`, `GPIO_SetBits`, `GPIO_ResetBits`, `GPIO_WriteBit`, `GPIO_Write`, `GPIO_ToggleBits`, `GPIO_PinAFConfig`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_hash.h`, `stm32f4xx_rcc.h`。
定义：`HASH_DeInit`, `HASH_Init`, `HASH_StructInit`, `HASH_Reset`, `HASH_SetLastWordValidBitsNbr`, `HASH_DataIn`, `HASH_GetInFIFOWordsNbr`, `HASH_GetDigest`, `HASH_StartDigest`, `HASH_SaveContext`, `HASH_RestoreContext`, `HASH_AutoStartDigest`, `HASH_DMACmd`, `HASH_ITConfig`, `HASH_GetFlagStatus`, `HASH_ClearFlag`, `HASH_GetITStatus`, `HASH_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_md5.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_hash.h`。
定义：`HASH_MD5`, `HMAC_MD5`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_hash_sha1.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_hash.h`。
定义：`HASH_SHA1`, `HMAC_SHA1`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_i2c.h`, `stm32f4xx_rcc.h`。
定义：`I2C_DeInit`, `I2C_Init`, `I2C_StructInit`, `I2C_Cmd`, `I2C_AnalogFilterCmd`, `I2C_DigitalFilterConfig`, `I2C_GenerateSTART`, `I2C_GenerateSTOP`, `I2C_Send7bitAddress`, `I2C_AcknowledgeConfig`, `I2C_OwnAddress2Config`, `I2C_DualAddressCmd`, `I2C_GeneralCallCmd`, `I2C_SoftwareResetCmd`, `I2C_StretchClockCmd`, `I2C_FastModeDutyCycleConfig`, `I2C_NACKPositionConfig`, `I2C_SMBusAlertConfig`, `I2C_ARPCmd`, `I2C_SendData`, `I2C_ReceiveData`, `I2C_TransmitPEC`, `I2C_PECPositionConfig`, `I2C_CalculatePEC`, `I2C_GetPEC`, `I2C_DMACmd`, `I2C_DMALastTransferCmd`, `I2C_ReadRegister`, `I2C_ITConfig`, `I2C_CheckEvent`, `I2C_GetLastEvent`, `I2C_GetFlagStatus`, `I2C_ClearFlag`, `I2C_GetITStatus`, `I2C_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_iwdg.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_iwdg.h`。
定义：`IWDG_WriteAccessCmd`, `IWDG_SetPrescaler`, `IWDG_SetReload`, `IWDG_ReloadCounter`, `IWDG_Enable`, `IWDG_GetFlagStatus`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_lptim.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_lptim.h`。
定义：`LPTIM_DeInit`, `LPTIM_Init`, `LPTIM_StructInit`, `LPTIM_Cmd`, `LPTIM_SelectClockSource`, `LPTIM_SelectULPTIMClockPolarity`, `LPTIM_ConfigPrescaler`, `LPTIM_ConfigExternalTrigger`, `LPTIM_SelectSoftwareStart`, `LPTIM_ConfigTriggerGlitchFilter`, `LPTIM_ConfigClockGlitchFilter`, `LPTIM_SelectOperatingMode`, `LPTIM_TimoutCmd`, `LPTIM_ConfigWaveform`, `LPTIM_ConfigUpdate`, `LPTIM_SetAutoreloadValue`, `LPTIM_SetCompareValue`, `LPTIM_SelectCounterMode`, `LPTIM_SelectEncoderMode`, `LPTIM_GetCounterValue`, `LPTIM_GetAutoreloadValue`, `LPTIM_GetCompareValue`, `LPTIM_RemapConfig`, `LPTIM_ITConfig`, `LPTIM_GetFlagStatus`, `LPTIM_ClearFlag`, `LPTIM_GetITStatus`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_ltdc.h`, `stm32f4xx_rcc.h`。
定义：`LTDC_DeInit`, `LTDC_Init`, `LTDC_StructInit`, `LTDC_Cmd`, `LTDC_DitherCmd`, `LTDC_GetRGBWidth`, `LTDC_RGBStructInit`, `LTDC_LIPConfig`, `LTDC_ReloadConfig`, `LTDC_LayerInit`, `LTDC_LayerStructInit`, `LTDC_LayerCmd`, `LTDC_GetPosStatus`, `LTDC_PosStructInit`, `LTDC_GetCDStatus`, `LTDC_ColorKeyingConfig`, `LTDC_ColorKeyingStructInit`, `LTDC_CLUTCmd`, `LTDC_CLUTInit`, `LTDC_CLUTStructInit`, `LTDC_LayerPosition`, `LTDC_LayerAlpha`, `LTDC_LayerAddress`, `LTDC_LayerSize`, `LTDC_LayerPixelFormat`, `LTDC_ITConfig`, `LTDC_GetFlagStatus`, `LTDC_ClearFlag`, `LTDC_GetITStatus`, `LTDC_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_pwr.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_pwr.h`, `stm32f4xx_rcc.h`。
定义：`PWR_DeInit`, `PWR_BackupAccessCmd`, `PWR_PVDLevelConfig`, `PWR_PVDCmd`, `PWR_WakeUpPinCmd`, `PWR_WakeUpPinCmd`, `PWR_BackupRegulatorCmd`, `PWR_MainRegulatorModeConfig`, `PWR_OverDriveCmd`, `PWR_OverDriveSWCmd`, `PWR_UnderDriveCmd`, `PWR_MainRegulatorUnderDriveCmd`, `PWR_LowRegulatorUnderDriveCmd`, `PWR_MainRegulatorLowVoltageCmd`, `PWR_LowRegulatorLowVoltageCmd`, `PWR_FlashPowerDownCmd`, `PWR_EnterSTOPMode`, `PWR_EnterUnderDriveSTOPMode`, `PWR_EnterSTANDBYMode`, `PWR_GetFlagStatus`, `PWR_ClearFlag`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_qspi.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_qspi.h`。
定义：`QSPI_DeInit`, `QSPI_StructInit`, `QSPI_ComConfig_StructInit`, `QSPI_Init`, `QSPI_ComConfig_Init`, `QSPI_Cmd`, `QSPI_AutoPollingMode_Config`, `QSPI_AutoPollingMode_SetInterval`, `QSPI_MemoryMappedMode_SetTimeout`, `QSPI_SetAddress`, `QSPI_SetAlternateByte`, `QSPI_SetFIFOThreshold`, `QSPI_SetDataLength`, `QSPI_TimeoutCounterCmd`, `QSPI_AutoPollingModeStopCmd`, `QSPI_AbortRequest`, `QSPI_SendData8`, `QSPI_SendData16`, `QSPI_SendData32`, `QSPI_ReceiveData8`, `QSPI_ReceiveData16`, `QSPI_ReceiveData32`, `QSPI_DMACmd`, `QSPI_ITConfig`, `QSPI_GetFIFOLevel`, `QSPI_GetFMode`, `QSPI_GetFlagStatus`, `QSPI_ClearFlag`, `QSPI_GetITStatus`, `QSPI_ClearITPendingBit`, `QSPI_DualFlashMode_Cmd`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f4xx_rcc.h`。
定义：`RCC_DeInit`, `RCC_HSEConfig`, `RCC_WaitForHSEStartUp`, `RCC_AdjustHSICalibrationValue`, `RCC_HSICmd`, `RCC_LSEConfig`, `RCC_LSICmd`, `RCC_PLLConfig`, `RCC_PLLConfig`, `RCC_PLLCmd`, `RCC_PLLI2SConfig`, `RCC_PLLI2SConfig`, `RCC_PLLI2SConfig`, `RCC_PLLI2SConfig`, `RCC_PLLI2SCmd`, `RCC_PLLSAIConfig`, `RCC_PLLSAIConfig`, `RCC_PLLSAIConfig`, `RCC_PLLSAICmd`, `RCC_ClockSecuritySystemCmd`, `RCC_MCO1Config`, `RCC_MCO2Config`, `RCC_SYSCLKConfig`, `RCC_GetSYSCLKSource`, `RCC_HCLKConfig`, `RCC_PCLK1Config`, `RCC_PCLK2Config`, `RCC_GetClocksFreq`, `RCC_RTCCLKConfig`, `RCC_RTCCLKCmd`, `RCC_BackupResetCmd`, `RCC_I2SCLKConfig`, `RCC_SAICLKConfig`, `RCC_SAIBlockACLKConfig`, `RCC_SAIBlockBCLKConfig`, `RCC_I2SCLKConfig`, `RCC_I2SCLKConfig`, `RCC_SAIBlockACLKConfig`, `RCC_SAIBlockBCLKConfig`, `RCC_SAIPLLI2SClkDivConfig`, `RCC_SAIPLLSAIClkDivConfig`, `RCC_SAIPLLI2SRClkDivConfig`, `RCC_SAIPLLRClkDivConfig`, `RCC_LTDCCLKDivConfig`, `RCC_DFSDM1CLKConfig`, `RCC_DFSDM1ACLKConfig`, `RCC_DFSDM2ACLKConfig`, `RCC_TIMCLKPresConfig`, `RCC_AHB1PeriphClockCmd`, `RCC_AHB2PeriphClockCmd`, `RCC_AHB3PeriphClockCmd`, `RCC_APB1PeriphClockCmd`, `RCC_APB2PeriphClockCmd`, `RCC_AHB1PeriphResetCmd`, `RCC_AHB2PeriphResetCmd`, `RCC_AHB3PeriphResetCmd`, `RCC_APB1PeriphResetCmd`, `RCC_APB2PeriphResetCmd`, `RCC_AHB1PeriphClockLPModeCmd`, `RCC_AHB2PeriphClockLPModeCmd`, `RCC_AHB3PeriphClockLPModeCmd`, `RCC_APB1PeriphClockLPModeCmd`, `RCC_APB2PeriphClockLPModeCmd`, `RCC_LSEModeConfig`, `RCC_LPTIM1ClockSourceConfig`, `RCC_DSIClockSourceConfig`, `RCC_48MHzClockSourceConfig`, `RCC_SDIOClockSourceConfig`, `RCC_AHB1ClockGatingCmd`, `RCC_SPDIFRXClockSourceConfig`, `RCC_CECClockSourceConfig`, `RCC_FMPI2C1ClockSourceConfig`, `RCC_MCO1Cmd`, `RCC_MCO2Cmd`, `RCC_ITConfig`, `RCC_GetFlagStatus`, `RCC_ClearFlag`, `RCC_GetITStatus`, `RCC_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rng.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_rng.h`, `stm32f4xx_rcc.h`。
定义：`RNG_DeInit`, `RNG_Cmd`, `RNG_GetRandomNumber`, `RNG_ITConfig`, `RNG_GetFlagStatus`, `RNG_ClearFlag`, `RNG_GetITStatus`, `RNG_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rtc.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_rtc.h`。
定义：`RTC_DeInit`, `RTC_Init`, `RTC_StructInit`, `RTC_WriteProtectionCmd`, `RTC_EnterInitMode`, `RTC_ExitInitMode`, `RTC_WaitForSynchro`, `RTC_RefClockCmd`, `RTC_BypassShadowCmd`, `RTC_SetTime`, `RTC_TimeStructInit`, `RTC_GetTime`, `RTC_GetSubSecond`, `RTC_SetDate`, `RTC_DateStructInit`, `RTC_GetDate`, `RTC_SetAlarm`, `RTC_AlarmStructInit`, `RTC_GetAlarm`, `RTC_AlarmCmd`, `RTC_AlarmSubSecondConfig`, `RTC_GetAlarmSubSecond`, `RTC_WakeUpClockConfig`, `RTC_SetWakeUpCounter`, `RTC_GetWakeUpCounter`, `RTC_WakeUpCmd`, `RTC_DayLightSavingConfig`, `RTC_GetStoreOperation`, `RTC_OutputConfig`, `RTC_CoarseCalibConfig`, `RTC_CoarseCalibCmd`, `RTC_CalibOutputCmd`, `RTC_CalibOutputConfig`, `RTC_SmoothCalibConfig`, `RTC_TimeStampCmd`, `RTC_GetTimeStamp`, `RTC_GetTimeStampSubSecond`, `RTC_TamperTriggerConfig`, `RTC_TamperCmd`, `RTC_TamperFilterConfig`, `RTC_TamperSamplingFreqConfig`, `RTC_TamperPinsPrechargeDuration`, `RTC_TimeStampOnTamperDetectionCmd`, `RTC_TamperPullUpCmd`, `RTC_WriteBackupRegister`, `RTC_ReadBackupRegister`, `RTC_TamperPinSelection`, `RTC_TimeStampPinSelection`, `RTC_OutputTypeConfig`, `RTC_SynchroShiftConfig`, `RTC_ITConfig`, `RTC_GetFlagStatus`, `RTC_ClearFlag`, `RTC_GetITStatus`, `RTC_ClearITPendingBit`, `RTC_ByteToBcd2`, `RTC_Bcd2ToByte`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sai.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_sai.h`, `stm32f4xx_rcc.h`。
定义：`SAI_DeInit`, `SAI_Init`, `SAI_FrameInit`, `SAI_SlotInit`, `SAI_StructInit`, `SAI_FrameStructInit`, `SAI_SlotStructInit`, `SAI_Cmd`, `SAI_MonoModeConfig`, `SAI_TRIStateConfig`, `SAI_CompandingModeConfig`, `SAI_MuteModeCmd`, `SAI_MuteValueConfig`, `SAI_MuteFrameCounterConfig`, `SAI_BlockSynchroConfig`, `SAI_FlushFIFO`, `SAI_ReceiveData`, `SAI_SendData`, `SAI_DMACmd`, `SAI_ITConfig`, `SAI_GetFlagStatus`, `SAI_ClearFlag`, `SAI_GetITStatus`, `SAI_ClearITPendingBit`, `SAI_GetCmdStatus`, `SAI_GetFIFOStatus`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_sdio.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_sdio.h`, `stm32f4xx_rcc.h`。
定义：`SDIO_DeInit`, `SDIO_Init`, `SDIO_StructInit`, `SDIO_ClockCmd`, `SDIO_SetPowerState`, `SDIO_GetPowerState`, `SDIO_SendCommand`, `SDIO_CmdStructInit`, `SDIO_GetCommandResponse`, `SDIO_GetResponse`, `SDIO_DataConfig`, `SDIO_DataStructInit`, `SDIO_GetDataCounter`, `SDIO_ReadData`, `SDIO_WriteData`, `SDIO_GetFIFOCount`, `SDIO_StartSDIOReadWait`, `SDIO_StopSDIOReadWait`, `SDIO_SetSDIOReadWaitMode`, `SDIO_SetSDIOOperation`, `SDIO_SendSDIOSuspendCmd`, `SDIO_CommandCompletionCmd`, `SDIO_CEATAITCmd`, `SDIO_SendCEATACmd`, `SDIO_DMACmd`, `SDIO_ITConfig`, `SDIO_GetFlagStatus`, `SDIO_ClearFlag`, `SDIO_GetITStatus`, `SDIO_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spdifrx.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_spdifrx.h`, `stm32f4xx_rcc.h`。
定义：`SPDIFRX_DeInit`, `SPDIFRX_Init`, `SPDIFRX_StructInit`, `SPDIFRX_SetPreambleTypeBit`, `SPDIFRX_SetUserDataChannelStatusBits`, `SPDIFRX_SetValidityBit`, `SPDIFRX_SetParityBit`, `SPDIFRX_RxDMACmd`, `SPDIFRX_CbDMACmd`, `SPDIFRX_Cmd`, `SPDIFRX_ITConfig`, `SPDIFRX_GetFlagStatus`, `SPDIFRX_ClearFlag`, `SPDIFRX_GetITStatus`, `SPDIFRX_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_spi.h`, `stm32f4xx_rcc.h`。
定义：`SPI_I2S_DeInit`, `SPI_Init`, `I2S_Init`, `SPI_StructInit`, `I2S_StructInit`, `SPI_Cmd`, `I2S_Cmd`, `SPI_DataSizeConfig`, `SPI_BiDirectionalLineConfig`, `SPI_NSSInternalSoftwareConfig`, `SPI_SSOutputCmd`, `SPI_TIModeCmd`, `I2S_FullDuplexConfig`, `SPI_I2S_ReceiveData`, `SPI_I2S_SendData`, `SPI_CalculateCRC`, `SPI_TransmitCRC`, `SPI_GetCRC`, `SPI_GetCRCPolynomial`, `SPI_I2S_DMACmd`, `SPI_I2S_ITConfig`, `SPI_I2S_GetFlagStatus`, `SPI_I2S_ClearFlag`, `SPI_I2S_GetITStatus`, `SPI_I2S_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_syscfg.h`, `stm32f4xx_rcc.h`。
定义：`SYSCFG_DeInit`, `SYSCFG_MemoryRemapConfig`, `SYSCFG_MemorySwappingBank`, `SYSCFG_EXTILineConfig`, `SYSCFG_ETH_MediaInterfaceConfig`, `SYSCFG_CompensationCellCmd`, `SYSCFG_GetCompensationCellStatus`, `SYSCFG_BreakConfig`, `DFSDM_BitstreamClock_SourceSelection`, `DFSDM_DisableDelayClock`, `DFSDM_EnableDelayClock`, `DFSDM_ClockIn_SourceSelection`, `DFSDM_ClockOut_SourceSelection`, `DFSDM_DataIn0_SourceSelection`, `DFSDM_DataIn2_SourceSelection`, `DFSDM_DataIn4_SourceSelection`, `DFSDM_DataIn6_SourceSelection`, `DFSDM1_BitStreamClk_Config`, `DFSDM2_BitStreamClk_Config`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f4xx_tim.h`, `stm32f4xx_rcc.h`。
定义：`TIM_DeInit`, `TIM_TimeBaseInit`, `TIM_TimeBaseStructInit`, `TIM_PrescalerConfig`, `TIM_CounterModeConfig`, `TIM_SetCounter`, `TIM_SetAutoreload`, `TIM_GetCounter`, `TIM_GetPrescaler`, `TIM_UpdateDisableConfig`, `TIM_UpdateRequestConfig`, `TIM_ARRPreloadConfig`, `TIM_SelectOnePulseMode`, `TIM_SetClockDivision`, `TIM_Cmd`, `TIM_OC1Init`, `TIM_OC2Init`, `TIM_OC3Init`, `TIM_OC4Init`, `TIM_OCStructInit`, `TIM_SelectOCxM`, `TIM_SetCompare1`, `TIM_SetCompare2`, `TIM_SetCompare3`, `TIM_SetCompare4`, `TIM_ForcedOC1Config`, `TIM_ForcedOC2Config`, `TIM_ForcedOC3Config`, `TIM_ForcedOC4Config`, `TIM_OC1PreloadConfig`, `TIM_OC2PreloadConfig`, `TIM_OC3PreloadConfig`, `TIM_OC4PreloadConfig`, `TIM_OC1FastConfig`, `TIM_OC2FastConfig`, `TIM_OC3FastConfig`, `TIM_OC4FastConfig`, `TIM_ClearOC1Ref`, `TIM_ClearOC2Ref`, `TIM_ClearOC3Ref`, `TIM_ClearOC4Ref`, `TIM_OC1PolarityConfig`, `TIM_OC1NPolarityConfig`, `TIM_OC2PolarityConfig`, `TIM_OC2NPolarityConfig`, `TIM_OC3PolarityConfig`, `TIM_OC3NPolarityConfig`, `TIM_OC4PolarityConfig`, `TIM_CCxCmd`, `TIM_CCxNCmd`, `TIM_ICInit`, `TIM_ICStructInit`, `TIM_PWMIConfig`, `TIM_GetCapture1`, `TIM_GetCapture2`, `TIM_GetCapture3`, `TIM_GetCapture4`, `TIM_SetIC1Prescaler`, `TIM_SetIC2Prescaler`, `TIM_SetIC3Prescaler`, `TIM_SetIC4Prescaler`, `TIM_BDTRConfig`, `TIM_BDTRStructInit`, `TIM_CtrlPWMOutputs`, `TIM_SelectCOM`, `TIM_CCPreloadControl`, `TIM_ITConfig`, `TIM_GenerateEvent`, `TIM_GetFlagStatus`, `TIM_ClearFlag`, `TIM_GetITStatus`, `TIM_ClearITPendingBit`, `TIM_DMAConfig`, `TIM_DMACmd`, `TIM_SelectCCDMA`, `TIM_InternalClockConfig`, `TIM_ITRxExternalClockConfig`, `TIM_TIxExternalClockConfig`, `TIM_ETRClockMode1Config`, `TIM_ETRClockMode2Config`, `TIM_SelectInputTrigger`, `TIM_SelectOutputTrigger`, `TIM_SelectSlaveMode`, `TIM_SelectMasterSlaveMode`, `TIM_ETRConfig`, `TIM_EncoderInterfaceConfig`, `TIM_SelectHallSensor`, `TIM_RemapConfig`, `TI1_Config`, `TI2_Config`, `TI3_Config`, `TI4_Config`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.c`
分类：CMSIS/芯片标准库；Keil 直接引用。
依赖：`stm32f4xx_usart.h`, `stm32f4xx_rcc.h`。
定义：`USART_DeInit`, `USART_Init`, `USART_StructInit`, `USART_ClockInit`, `USART_ClockStructInit`, `USART_Cmd`, `USART_SetPrescaler`, `USART_OverSampling8Cmd`, `USART_OneBitMethodCmd`, `USART_SendData`, `USART_ReceiveData`, `USART_SetAddress`, `USART_ReceiverWakeUpCmd`, `USART_WakeUpConfig`, `USART_LINBreakDetectLengthConfig`, `USART_LINCmd`, `USART_SendBreak`, `USART_HalfDuplexCmd`, `USART_SetGuardTime`, `USART_SmartCardCmd`, `USART_SmartCardNACKCmd`, `USART_IrDAConfig`, `USART_IrDACmd`, `USART_DMACmd`, `USART_ITConfig`, `USART_GetFlagStatus`, `USART_ClearFlag`, `USART_GetITStatus`, `USART_ClearITPendingBit`。

## `firmware/F407_Project/STM32/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_wwdg.c`
分类：CMSIS/芯片标准库；头文件/未直接列入工程的保留文件。
依赖：`stm32f4xx_wwdg.h`, `stm32f4xx_rcc.h`。
定义：`WWDG_DeInit`, `WWDG_SetPrescaler`, `WWDG_SetWindowValue`, `WWDG_EnableIT`, `WWDG_SetCounter`, `WWDG_Enable`, `WWDG_GetFlagStatus`, `WWDG_ClearFlag`。
