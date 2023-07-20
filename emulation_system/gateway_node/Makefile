TARGETS	= emulator/e4k64f external/libexternal_gateway.so external/libglue_gateway.so

all: $(TARGETS)

emulator/e4k64f:
	env LANG=C $(MAKE) -C emulator e4k64f

external/libexternal_gateway.so:
	env LANG=C $(MAKE) -C external libexternal_gateway.so

external/libglue_gateway.so:
	env LANG=C $(MAKE) -C external libglue_gateway.so

clean:
	env LANG=C $(MAKE) -C emulator clean
	env LANG=C $(MAKE) -C external clean
	rm -f $(TARGETS)

run: $(TARGETS)
	env LD_LIBRARY_PATH=external emulator/e4k64f -d 3 -e external/libglue_gateway.so target/e4k64f_gateway_node.hex

symbolize:
	env OBJDUMP=/usr/bin/arm-none-eabi-objdump python tools/lookup_symbol.py target/e4k64f_gateway_node.axf
