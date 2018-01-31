@echo off

if exist C:\Keil\ARM\BIN40\fromelf.exe (
if exist .\..\MDK-ARM\STM322xG_EVAL\STM322xG_EVAL.axf (C:\Keil\ARM\BIN40\fromelf.exe ".\..\MDK-ARM\STM322xG_EVAL\STM322xG_EVAL.axf" --bin --output ".\STM322xG_EVAL_SysTick.bin")

 )

pause

