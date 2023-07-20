TARGETS	= libexternal.so libglue.c libglue.so
CFLAGS	= -g3 -I../include

all: $(TARGETS)

libexternal.so: libexternal.c
	$(CC) $(CFLAGS) -fvisibility=hidden -fPIC -shared -o $@ $?

libglue.c:
	../tools/stage1.py libexternal.so ../target/target.axf | tee libexternal.yml | ../tools/stage2.py - libexternal.so > libglue.c

libglue.so: libglue.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $?

clean:
	rm -f *.yml $(TARGETS)
