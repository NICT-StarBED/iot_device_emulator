TARGETS	= emulator/e4arduino external/libexternal_sensor.so external/libglue_sensor.so

all: $(TARGETS)

emulator/e4arduino:
	env LANG=C $(MAKE) -C emulator e4arduino

external/libexternal_sensor.so:
	env LANG=C $(MAKE) -C external libexternal_sensor.so

external/libglue_sensor.so:
	env LANG=C $(MAKE) -C external libglue_sensor.so

clean:
	env LANG=C $(MAKE) -C emulator clean
	env LANG=C $(MAKE) -C external clean
	rm -f $(TARGETS)

run: $(TARGETS)
	env LD_LIBRARY_PATH=external emulator/e4arduino -d 3 -e external/libglue_sensor.so target/e4arduino_sensor_node.hex

symbolize:
	env OBJDUMP=/usr/bin/arm-none-eabi-objdump python tools/lookup_symbol.py target/e4arduino_sensor_node.axf
