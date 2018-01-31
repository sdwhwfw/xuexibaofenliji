@if exist .\..\TrueSTUDIO\STM322xG_EVAL\Debug\STM322xG_EVAL.elf ("arm-elf-objcopy.exe" -O binary ".\..\TrueSTUDIO\STM322xG_EVAL\Debug\STM322xG_EVAL.elf" "STM322xG_EVAL_SysTick.bin")

pause

