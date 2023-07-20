TARGETS	= emulator/e4arduino external/libexternal.so external/libglue.so

all: $(TARGETS)

emulator/e4arduino:
	env LANG=C $(MAKE) -C emulator e4arduino

external/libexternal.so:
	env LANG=C $(MAKE) -C external libexternal.so

external/libglue.so:
	env LANG=C $(MAKE) -C external libglue.so

clean:
	env LANG=C $(MAKE) -C emulator clean
	env LANG=C $(MAKE) -C external clean
	rm -f $(TARGETS)

run: $(TARGETS)
	env LD_LIBRARY_PATH=external emulator/e4arduino -d 5 -e external/libglue.so target/target.hex

symbolize:
	env OBJDUMP=/usr/bin/arm-none-eabi-objdump python tools/lookup.py target/target.axf
