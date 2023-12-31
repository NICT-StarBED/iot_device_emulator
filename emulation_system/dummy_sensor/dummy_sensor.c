#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

/* tsl2561_register_addr */
enum {
    TSL2561_REGISTER_CHAN0_LOW        = 0x0C,
    TSL2561_REGISTER_CHAN0_HIGH       = 0x0D,
    TSL2561_REGISTER_CHAN1_LOW        = 0x0E,
    TSL2561_REGISTER_CHAN1_HIGH       = 0x0F
};

/* bme280_register_addr */
#define BME280_REGISTER_TEMP_MSB       (0xfa)
#define BME280_REGISTER_TEMP_LSB       (0xfb)
#define BME280_REGISTER_TEMP_XLSB      (0xfc)

#define TSL2561_DATA_REGISTER_SIZE     4
#define BME280_DATA_REGISTER_SIZE      3

#define TSL2561_DUMMY_SAMPLES_NUM      150
#define BME280_DUMMY_SAMPLES_NUM       150

typedef struct {
    u_int8_t err[TSL2561_DATA_REGISTER_SIZE];
    u_int8_t samples[TSL2561_DUMMY_SAMPLES_NUM][TSL2561_DATA_REGISTER_SIZE];
} tsl2561_dummy_output_t;

tsl2561_dummy_output_t tsl2561 = { {0x00, 0x00, 0x00, 0x00},
    {
        {0x88, 0x01, 0x3a, 0x00}, {0x89, 0x01, 0x3a, 0x00},
        {0x8a, 0x01, 0x3a, 0x00}, {0x8a, 0x01, 0x3a, 0x00},
        {0x8a, 0x01, 0x3a, 0x00}, {0x8b, 0x01, 0x3a, 0x00},
        {0x8b, 0x01, 0x3a, 0x00}, {0x8c, 0x01, 0x3a, 0x00},
        {0x8b, 0x01, 0x3a, 0x00}, {0x8a, 0x01, 0x3a, 0x00},
        {0x88, 0x01, 0x3a, 0x00}, {0x88, 0x01, 0x3a, 0x00},
        {0x89, 0x01, 0x3a, 0x00}, {0x89, 0x01, 0x3a, 0x00},
        {0x89, 0x01, 0x3a, 0x00}, {0x8a, 0x01, 0x3a, 0x00},
        {0x8a, 0x01, 0x3a, 0x00}, {0x8a, 0x01, 0x3a, 0x00},
        {0x8a, 0x01, 0x3a, 0x00}, {0x89, 0x01, 0x3a, 0x00},
        {0x8a, 0x01, 0x3b, 0x00}, {0x8b, 0x01, 0x3b, 0x00},
        {0x8b, 0x01, 0x3b, 0x00}, {0x8c, 0x01, 0x3b, 0x00},
        {0x91, 0x01, 0x3c, 0x00}, {0x93, 0x01, 0x3c, 0x00},
        {0x94, 0x01, 0x3c, 0x00}, {0x96, 0x01, 0x3d, 0x00},
        {0x7b, 0x01, 0x39, 0x00}, {0x87, 0x01, 0x3a, 0x00},
        {0x8a, 0x01, 0x3b, 0x00}, {0x8c, 0x01, 0x3b, 0x00},
        {0x8e, 0x01, 0x3c, 0x00}, {0x91, 0x01, 0x3c, 0x00},
        {0x82, 0x01, 0x3b, 0x00}, {0x87, 0x01, 0x3b, 0x00},
        {0x85, 0x01, 0x3b, 0x00}, {0x85, 0x01, 0x3b, 0x00},
        {0x84, 0x01, 0x3b, 0x00}, {0x8a, 0x01, 0x3c, 0x00},
        {0x89, 0x01, 0x3c, 0x00}, {0x8a, 0x01, 0x3c, 0x00},
        {0x8b, 0x01, 0x3c, 0x00}, {0x8d, 0x01, 0x3d, 0x00},
        {0x8f, 0x01, 0x3d, 0x00}, {0x8f, 0x01, 0x3d, 0x00},
        {0x8f, 0x01, 0x3d, 0x00}, {0x8f, 0x01, 0x3d, 0x00},
        {0x90, 0x01, 0x3d, 0x00}, {0x90, 0x01, 0x3e, 0x00},
        {0x93, 0x01, 0x3f, 0x00}, {0x97, 0x01, 0x40, 0x00},
        {0x84, 0x01, 0x3f, 0x00}, {0xca, 0x00, 0x2e, 0x00},
        {0xc0, 0x00, 0x2d, 0x00}, {0xca, 0x00, 0x2f, 0x00},
        {0x2b, 0x00, 0x0e, 0x00}, {0x27, 0x00, 0x0d, 0x00},
        {0x1c, 0x00, 0x09, 0x00}, {0x11, 0x00, 0x05, 0x00},
        {0x12, 0x00, 0x05, 0x00}, {0x95, 0x00, 0x21, 0x00},
        {0xa5, 0x01, 0x44, 0x00}, {0xa4, 0x01, 0x44, 0x00},
        {0xa9, 0x01, 0x46, 0x00}, {0xad, 0x01, 0x47, 0x00},
        {0xad, 0x01, 0x47, 0x00}, {0xa8, 0x01, 0x45, 0x00},
        {0xa4, 0x01, 0x44, 0x00}, {0xa0, 0x01, 0x42, 0x00},
        {0x9d, 0x01, 0x40, 0x00}, {0x9b, 0x01, 0x40, 0x00},
        {0x9a, 0x01, 0x40, 0x00}, {0x9a, 0x01, 0x40, 0x00},
        {0x9a, 0x01, 0x40, 0x00}, {0x99, 0x01, 0x40, 0x00},
        {0x99, 0x01, 0x3f, 0x00}, {0x98, 0x01, 0x3f, 0x00},
        {0x99, 0x01, 0x3f, 0x00}, {0x9b, 0x01, 0x3f, 0x00},
        {0xa0, 0x01, 0x40, 0x00}, {0xa0, 0x01, 0x40, 0x00},
        {0xa0, 0x01, 0x40, 0x00}, {0xa2, 0x01, 0x41, 0x00},
        {0xa4, 0x01, 0x41, 0x00}, {0xa4, 0x01, 0x41, 0x00},
        {0xa4, 0x01, 0x41, 0x00}, {0xa3, 0x01, 0x41, 0x00},
        {0xa2, 0x01, 0x40, 0x00}, {0x9e, 0x01, 0x40, 0x00},
        {0x9a, 0x01, 0x3f, 0x00}, {0x99, 0x01, 0x3f, 0x00},
        {0x92, 0x01, 0x3e, 0x00}, {0x91, 0x01, 0x3e, 0x00},
        {0x92, 0x01, 0x3e, 0x00}, {0x93, 0x01, 0x3e, 0x00},
        {0x94, 0x01, 0x3e, 0x00}, {0x94, 0x01, 0x3e, 0x00},
        {0x94, 0x01, 0x3e, 0x00}, {0x96, 0x01, 0x3e, 0x00},
        {0x96, 0x01, 0x3e, 0x00}, {0x97, 0x01, 0x3e, 0x00},
        {0x97, 0x01, 0x3e, 0x00}, {0x97, 0x01, 0x3e, 0x00},
        {0x98, 0x01, 0x3f, 0x00}, {0x98, 0x01, 0x3e, 0x00},
        {0x98, 0x01, 0x3f, 0x00}, {0x98, 0x01, 0x3f, 0x00},
        {0x98, 0x01, 0x3f, 0x00}, {0x97, 0x01, 0x3f, 0x00},
        {0x97, 0x01, 0x3f, 0x00}, {0x97, 0x01, 0x3f, 0x00},
        {0x78, 0x01, 0x3b, 0x00}, {0xa2, 0x01, 0x41, 0x00},
        {0xa3, 0x01, 0x41, 0x00}, {0xa2, 0x01, 0x41, 0x00},
        {0xa2, 0x01, 0x41, 0x00}, {0xa1, 0x01, 0x41, 0x00},
        {0xa0, 0x01, 0x40, 0x00}, {0x9d, 0x01, 0x40, 0x00},
        {0x9d, 0x01, 0x40, 0x00}, {0x9d, 0x01, 0x40, 0x00},
        {0x9e, 0x01, 0x40, 0x00}, {0x9e, 0x01, 0x40, 0x00},
        {0x9f, 0x01, 0x40, 0x00}, {0xa0, 0x01, 0x40, 0x00},
        {0xa0, 0x01, 0x41, 0x00}, {0xa1, 0x01, 0x41, 0x00},
        {0xa4, 0x01, 0x42, 0x00}, {0xa4, 0x01, 0x42, 0x00},
        {0xa6, 0x01, 0x42, 0x00}, {0xa5, 0x01, 0x42, 0x00},
        {0xa7, 0x01, 0x42, 0x00}, {0xa3, 0x01, 0x42, 0x00},
        {0xa1, 0x01, 0x41, 0x00}, {0x9f, 0x01, 0x41, 0x00},
        {0x9e, 0x01, 0x41, 0x00}, {0x9e, 0x01, 0x41, 0x00},
        {0x9f, 0x01, 0x41, 0x00}, {0x9f, 0x01, 0x41, 0x00},
        {0xa0, 0x01, 0x41, 0x00}, {0x9f, 0x01, 0x41, 0x00},
        {0x9f, 0x01, 0x41, 0x00}, {0x9f, 0x01, 0x41, 0x00},
        {0xa3, 0x01, 0x42, 0x00}, {0xa4, 0x01, 0x42, 0x00},
        {0xa6, 0x01, 0x42, 0x00}, {0x88, 0x01, 0x3a, 0x00},
        {0x89, 0x01, 0x3a, 0x00}, {0x8a, 0x01, 0x3a, 0x00},
    }
};

typedef struct {
    u_int8_t err[BME280_DATA_REGISTER_SIZE];
    u_int8_t samples[BME280_DUMMY_SAMPLES_NUM][BME280_DATA_REGISTER_SIZE];
} bme280_dummy_output_t;

bme280_dummy_output_t bme280 = { {0x00, 0x00, 0x00},
    {
        {0x83, 0xa8, 0x10}, {0x83, 0xa7, 0x60}, {0x83, 0xa6, 0x40},
        {0x83, 0xa7, 0x00}, {0x83, 0xa6, 0xa0}, {0x83, 0xa7, 0xe0},
        {0x83, 0xa9, 0x30}, {0x83, 0xa7, 0xf0}, {0x83, 0xa7, 0x60},
        {0x83, 0xa8, 0x00}, {0x83, 0xa7, 0xc0}, {0x83, 0xa8, 0x90},
        {0x83, 0xa6, 0x90}, {0x83, 0xa6, 0xc0}, {0x83, 0xa5, 0xf0},
        {0x83, 0xa7, 0x90}, {0x83, 0xa7, 0xe0}, {0x83, 0xaa, 0x60},
        {0x83, 0xab, 0x10}, {0x83, 0xac, 0x10}, {0x83, 0xab, 0xd0},
        {0x83, 0xab, 0x30}, {0x83, 0xac, 0x80}, {0x83, 0xae, 0xa0},
        {0x83, 0xad, 0x80}, {0x83, 0xab, 0x00}, {0x83, 0xaa, 0x10},
        {0x83, 0xac, 0x70}, {0x83, 0xad, 0xb0}, {0x83, 0xad, 0x30},
        {0x83, 0xad, 0x70}, {0x83, 0xae, 0x20}, {0x83, 0xaf, 0xa0},
        {0x83, 0xb0, 0x30}, {0x83, 0xaf, 0x70}, {0x83, 0xac, 0xf0},
        {0x83, 0xac, 0x30}, {0x83, 0xae, 0x40}, {0x83, 0xaf, 0x90},
        {0x83, 0xb1, 0x10}, {0x83, 0xae, 0x10}, {0x83, 0xae, 0xc0},
        {0x83, 0xae, 0xb0}, {0x83, 0xad, 0x70}, {0x83, 0xae, 0xb0},
        {0x83, 0xae, 0x30}, {0x83, 0xae, 0x40}, {0x83, 0xac, 0xf0},
        {0x83, 0xac, 0x70}, {0x83, 0xaa, 0xd0}, {0x83, 0xab, 0x20},
        {0x83, 0xaa, 0x00}, {0x83, 0xac, 0x20}, {0x83, 0xa9, 0x20},
        {0x83, 0xa8, 0x70}, {0x83, 0xad, 0x70}, {0x83, 0xac, 0x20},
        {0x83, 0xae, 0x30}, {0x83, 0xb0, 0x10}, {0x83, 0xb3, 0x50},
        {0x83, 0xb6, 0x10}, {0x83, 0xba, 0x20}, {0x83, 0xb2, 0x40},
        {0x83, 0xb1, 0x30}, {0x83, 0xb1, 0x00}, {0x83, 0xaf, 0xd0},
        {0x83, 0xb0, 0x30}, {0x83, 0xae, 0xf0}, {0x83, 0xaf, 0x30},
        {0x83, 0xb0, 0x20}, {0x83, 0xb0, 0xd0}, {0x83, 0xb0, 0x70},
        {0x83, 0xb1, 0x00}, {0x83, 0xb0, 0x00}, {0x83, 0xad, 0x90},
        {0x83, 0xab, 0xc0}, {0x83, 0xab, 0xf0}, {0x83, 0xac, 0xa0},
        {0x83, 0xae, 0x70}, {0x83, 0xb0, 0x70}, {0x83, 0xae, 0x60},
        {0x83, 0xad, 0xd0}, {0x83, 0xac, 0x60}, {0x83, 0xad, 0x50},
        {0x83, 0xac, 0x40}, {0x83, 0xac, 0xa0}, {0x83, 0xab, 0xc0},
        {0x83, 0xac, 0x20}, {0x83, 0xac, 0x50}, {0x83, 0xac, 0x60},
        {0x83, 0xac, 0xb0}, {0x83, 0xac, 0x20}, {0x83, 0xac, 0x20},
        {0x83, 0xa9, 0x90}, {0x83, 0xa5, 0xd0}, {0x83, 0xa9, 0x30},
        {0x83, 0xa8, 0x00}, {0x83, 0xa6, 0x70}, {0x83, 0xa6, 0xd0},
        {0x83, 0xa8, 0x70}, {0x83, 0xa6, 0xd0}, {0x83, 0xa9, 0x00},
        {0x83, 0xa6, 0x10}, {0x83, 0xa7, 0x50}, {0x83, 0xa7, 0xd0},
        {0x83, 0xa6, 0x10}, {0x83, 0xa6, 0xe0}, {0x83, 0xa5, 0x80},
        {0x83, 0xa5, 0x00}, {0x83, 0xa4, 0xa0}, {0x83, 0xa2, 0x50},
        {0x83, 0xa3, 0xe0}, {0x83, 0xa2, 0x30}, {0x83, 0xa2, 0x40},
        {0x83, 0x9f, 0x00}, {0x83, 0xa0, 0xc0}, {0x83, 0xa0, 0xe0},
        {0x83, 0x9f, 0xf0}, {0x83, 0x9e, 0xf0}, {0x83, 0x9c, 0xe0},
        {0x83, 0xa0, 0xa0}, {0x83, 0x9f, 0x50}, {0x83, 0xa1, 0x10},
        {0x83, 0xa2, 0x70}, {0x83, 0xa3, 0x10}, {0x83, 0xa3, 0xd0},
        {0x83, 0xa3, 0xa0}, {0x83, 0xa3, 0xe0}, {0x83, 0xa2, 0x90},
        {0x83, 0xa4, 0xc0}, {0x83, 0xa6, 0x20}, {0x83, 0xa7, 0xe0},
        {0x83, 0xa7, 0x80}, {0x83, 0xa9, 0x20}, {0x83, 0xa9, 0x90},
        {0x83, 0xaa, 0x60}, {0x83, 0xac, 0x50}, {0x83, 0xab, 0x30},
        {0x83, 0xaa, 0x50}, {0x83, 0xaa, 0x40}, {0x83, 0xa9, 0x20},
        {0x83, 0xa8, 0x90}, {0x83, 0xa8, 0x90}, {0x83, 0xa7, 0xc0},
        {0x83, 0xa9, 0x40}, {0x83, 0xa9, 0x10}, {0x83, 0xa6, 0x00},
        {0x83, 0xa8, 0x10}, {0x83, 0xa7, 0x60}, {0x83, 0xa6, 0x40},
    }
};

int i2c_dummy(u_int8_t input, u_int8_t output[4])
{
    int length = 0, r = 0;

    bzero(output, (sizeof(u_int8_t) * 4));

    switch(input) {
    case TSL2561_REGISTER_CHAN0_LOW:
        printf("TSL2561 register output\n");
        r = rand() % TSL2561_DUMMY_SAMPLES_NUM;
        output[0] = tsl2561.samples[r][0];
        output[1] = tsl2561.samples[r][1];
        output[2] = tsl2561.samples[r][2];
        output[3] = tsl2561.samples[r][3];
        length = TSL2561_DATA_REGISTER_SIZE;
        break;
    case BME280_REGISTER_TEMP_MSB:
        printf("BME280 register output\n");
        r = rand() % BME280_DUMMY_SAMPLES_NUM;
        output[0] = bme280.samples[r][0];
        output[1] = bme280.samples[r][1];
        output[2] = bme280.samples[r][2];
        length = BME280_DATA_REGISTER_SIZE;
        break;
    default:
        output = bme280.err;
        length = BME280_DATA_REGISTER_SIZE;
    }
    return length;
}

int get_server_socket(const char *const path)
{
    int s;
    struct sockaddr_un sun;

    memset(&sun, 0, sizeof(sun));
    sun.sun_family = AF_UNIX;
    strncpy(sun.sun_path, path, sizeof(sun.sun_path) - 1);

    if((s = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        return -1;
    }
    unlink(path);
    if(bind(s, (struct sockaddr *)&sun, sizeof(struct sockaddr_un)) < 0) {
        perror("bind");
        return -1;
    }
    if(listen(s, SOMAXCONN) < 0) {
        perror("listen");
        return -1;
    }

    return s;
}

int main(int argc, char *argv[])
{
    int s, cs, out_len, buffer_ptr = 0;
    u_int8_t in, out[4], buffer[BUFSIZ];

    srand((unsigned)time(NULL));

    if(argc == 1) {
        if((s = get_server_socket("/var/tmp/.i2c")) == -1) {
            exit(EXIT_FAILURE);
        }
    } else if(argc == 2) {
        if((s = get_server_socket(argv[1])) == -1) {
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "usage: %s [/path/to/socket]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    for(;;) {
        if((cs = accept(s, NULL, NULL)) == -1) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "connection established\n");
        for(;;) {
            if(read(cs, &in, 1) > 0) {
                printf(" read: %02hhx\n", in);
                if((out_len = i2c_dummy(in, out)) < 0) {
                    break;
                }
                for (int i = 0; i < out_len;) {
                    if(write(cs, &out[i], 1) > 0) {
                        printf("write: %02hhx\n", out[i]);
                        i++;
                    } else {
                        fprintf(stderr, "connection closed\n");
                        break;
                    }
                }
            } else {
                fprintf(stderr, "connection closed\n");
                break;
            }
        }
        close(cs);
    }
    return 0;
}
