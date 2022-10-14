#! /bin/sh
killall openocd
openocd -f /usr/share/openocd/scripts/interface/stlink.cfg -f /usr/share/openocd/scripts/target/stm32l0.cfg -c 'program ./build/pumpkin.bin 0x08000000 reset exit' 
