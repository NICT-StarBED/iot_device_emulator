TARGETS	= libexternal_sensor.so libglue_sensor.c libglue_sensor.so
CFLAGS	= -g3 -I../include

all: $(TARGETS)

libexternal_sensor.so: libexternal_sensor.c
	$(CC) $(CFLAGS) -fvisibility=hidden -fPIC -shared -o $@ $?

libglue_sensor.c:
	../tools/stage1.py libexternal_sensor.so ../target/e4arduino_sensor_node.axf | tee libexternal_sensor.yml | ../tools/stage2.py - libexternal_sensor.so > libglue_sensor.c

libglue_sensor.so: libglue_sensor.c
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $?

clean:
	rm -f *.yml $(TARGETS)
