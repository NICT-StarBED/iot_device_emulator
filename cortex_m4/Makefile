TARGETS	= emulator/e4k64f external/libexternal.so external/libglue.so

all: $(TARGETS)

emulator/e4k64f:
	env LANG=C $(MAKE) -C emulator e4k64f

external/libexternal.so:
	env LANG=C $(MAKE) -C external libexternal.so

external/libglue.so:
	env LANG=C $(MAKE) -C external libglue.so

clean:
	env LANG=C $(MAKE) -C emulator clean
	env LANG=C $(MAKE) -C external clean
	rm -f $(TARGETS)

run: $(TARGETS)
	env LD_LIBRARY_PATH=external emulator/e4k64f -d 5 -e external/libglue.so target/target.hex

symbolize:
	env OBJDUMP=/usr/bin/arm-none-eabi-objdump python tools/lookup_symbol.py target/target.axf
