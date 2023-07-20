TARGETS	= libexternal_gateway.so libglue_gateway.c libglue_gateway.so
CFLAGS	= -g3 -I../include

all: $(TARGETS)

libexternal_gateway.so: libexternal_gateway.c
	$(CC) $(CFLAGS) -fvisibility=hidden -fPIC -shared -o $@ $?

libglue_gateway.c:
	../tools/stage1.py libexternal_gateway.so ../target/e4k64f_gateway_node.axf | tee libexternal_gateway.yml | ../tools/stage2.py - libexternal_gateway.so > libglue_gateway.c

libglue_gateway.so: libglue_gateway.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $?

clean:
	rm -f *.yml $(TARGETS)
