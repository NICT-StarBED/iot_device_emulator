#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>
#include <strings.h>
#include "xft.h"

#define BME280_DEVICE_ADDRESS                       (0x77)
/* 0x88:0xa1 */
#define BME280_REGISTER_CALIB00                     (0x88)
#define BME280_REGISTER_CALIB01                     (0x89)
#define BME280_REGISTER_CALIB02                     (0x8a)
#define BME280_REGISTER_CALIB03                     (0x8b)
#define BME280_REGISTER_CALIB04                     (0x8c)
#define BME280_REGISTER_CALIB05                     (0x8d)
#define BME280_REGISTER_CALIB06                     (0x8e)
#define BME280_REGISTER_CALIB07                     (0x8f)
#define BME280_REGISTER_CALIB08                     (0x90)
#define BME280_REGISTER_CALIB09                     (0x91)
#define BME280_REGISTER_CALIB10                     (0x92)
#define BME280_REGISTER_CALIB11                     (0x93)
#define BME280_REGISTER_CALIB12                     (0x94)
#define BME280_REGISTER_CALIB13                     (0x95)
#define BME280_REGISTER_CALIB14                     (0x96)
#define BME280_REGISTER_CALIB15                     (0x97)
#define BME280_REGISTER_CALIB16                     (0x98)
#define BME280_REGISTER_CALIB17                     (0x99)
#define BME280_REGISTER_CALIB18                     (0x9a)
#define BME280_REGISTER_CALIB19                     (0x9b)
#define BME280_REGISTER_CALIB20                     (0x9c)
#define BME280_REGISTER_CALIB21                     (0x9d)
#define BME280_REGISTER_CALIB22                     (0x9e)
#define BME280_REGISTER_CALIB23                     (0x9f)
#define BME280_REGISTER_CALIB24                     (0xa0)
#define BME280_REGISTER_CALIB25                     (0xa1)
/* 0xd0:0xd0 */
#define BME280_REGISTER_ID                          (0xd0)
/* 0xe0:0xf0 */
#define BME280_REGISTER_RESET                       (0xe0)
#define BME280_REGISTER_CALIB26                     (0xe1)
#define BME280_REGISTER_CALIB27                     (0xe2)
#define BME280_REGISTER_CALIB28                     (0xe3)
#define BME280_REGISTER_CALIB29                     (0xe4)
#define BME280_REGISTER_CALIB30                     (0xe5)
#define BME280_REGISTER_CALIB31                     (0xe6)
#define BME280_REGISTER_CALIB32                     (0xe7)
#define BME280_REGISTER_CALIB33                     (0xe8)
#define BME280_REGISTER_CALIB34                     (0xe9)
#define BME280_REGISTER_CALIB35                     (0xea)
#define BME280_REGISTER_CALIB36                     (0xeb)
#define BME280_REGISTER_CALIB37                     (0xec)
#define BME280_REGISTER_CALIB38                     (0xed)
#define BME280_REGISTER_CALIB39                     (0xee)
#define BME280_REGISTER_CALIB40                     (0xef)
#define BME280_REGISTER_CALIB41                     (0xf0)
/* 0xf2:0xfe */
#define BME280_REGISTER_CTRL_HUM                    (0xf2)
#define BME280_REGISTER_STATUS                      (0xf3)
#define BME280_REGISTER_CTRL_MEAS                   (0xf4)
#define BME280_REGISTER_CONFIG                      (0xf5)
#define BME280_REGISTER_PRESS_MSB                   (0xf7)
#define BME280_REGISTER_PRESS_LSB                   (0xf8)
#define BME280_REGISTER_PRESS_XLSB                  (0xf9)
#define BME280_REGISTER_TEMP_MSB                    (0xfa)
#define BME280_REGISTER_TEMP_LSB                    (0xfb)
#define BME280_REGISTER_TEMP_XLSB                   (0xfc)
#define BME280_REGISTER_HUM_MSB                     (0xfd)
#define BME280_REGISTER_HUM_LSB                     (0xfe)

typedef struct {
    u_int8_t initialized;
    u_int8_t registers[0x100];
} bme280_t;

bme280_t bme280 = {1,
    { 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x87, 0x70, 0x89, 0x6c, 0x5e, 0x44, 0xa9, 0x06, 0xc6, 0x6f, 0x7e, 0x69, 0x32, 0x00, 0x1d, 0x95,
        0x7c, 0xd6, 0xd0, 0x0b, 0x92, 0x1d, 0x9f, 0xff, 0xf9, 0xff, 0x0c, 0x30, 0x20, 0xd1, 0x88, 0x13,
        0x00, 0x00, 0x84, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0x00, 0x00, 0xc0,
        0x00, 0x54, 0x00, 0x00, 0x00, 0x00, 0x60, 0x02, 0x00, 0x01, 0xf3, 0xff, 0x1f, 0x71, 0x03, 0x00,
        0x00, 0x00, 0x23, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x6a, 0x01, 0x00, 0x13, 0x2f, 0x03, 0x1e, 0x06, 0x41, 0x00, 0xff, 0xf3, 0xff, 0xf3, 0xff,
        0xf3, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00, 0x80, 0x00, 0x00,
    }
};

static void bme280_reset(void)
{
    /* Do something */
}

static void bme280_update_registers(void)
{
    /* Do something */
}

XFT_EXPOSED_FUNCTION
void ShowResult(int32_t temperature)
{
    printf("temp: %d\n", temperature);
}

XFT_EXPOSED_FUNCTION
u_int8_t Chip_Clock_IsSystemPLLLocked(void)
{
    return 1;
}

XFT_EXPOSED_FUNCTION
void SetupXferRecAndExecute(u_int8_t devAddr, cortex_m_ptr_t txBuffPtr,
                            u_int16_t txSize, cortex_m_ptr_t rxBuffPtr,
                            u_int16_t rxSize)
{
    u_int8_t *m2s_buff = XFT_CONVERT_ADDR_T2H(txBuffPtr);
    u_int8_t *s2m_buff = XFT_CONVERT_ADDR_T2H(rxBuffPtr);

    switch(devAddr) {
    case BME280_DEVICE_ADDRESS:
        switch(m2s_buff[0]) {
        case BME280_REGISTER_ID:
            s2m_buff[0] = 0x60;
            printf("SetupXferRecAndExecute()\tBME280_REGISTER_ID\n");
            break;
        case BME280_REGISTER_RESET:
            if(m2s_buff[1] == 0xb6) {
                bme280_reset();
            }
            printf("SetupXferRecAndExecute()\tBME280_REGISTER_RESET\n");
            break;
        case BME280_REGISTER_CTRL_HUM:
            bme280.registers[BME280_REGISTER_CTRL_HUM] = m2s_buff[1];
            printf("SetupXferRecAndExecute()\tBME280_REGISTER_CTRL_HUM\n");
            break;
        case BME280_REGISTER_CTRL_MEAS:
            bme280.registers[BME280_REGISTER_CTRL_MEAS] = m2s_buff[1];
            printf("SetupXferRecAndExecute()\tBME280_REGISTER_CTRL_MEAS\n");
            break;
        case BME280_REGISTER_CONFIG:
            bme280.registers[BME280_REGISTER_CONFIG] = m2s_buff[1];
            printf("SetupXferRecAndExecute()\tBME280_REGISTER_CONFIG\n");
            break;
        case BME280_REGISTER_CALIB00:
            s2m_buff[0] = bme280.registers[BME280_REGISTER_CALIB00];
            s2m_buff[1] = bme280.registers[BME280_REGISTER_CALIB01];
            s2m_buff[2] = bme280.registers[BME280_REGISTER_CALIB02];
            s2m_buff[3] = bme280.registers[BME280_REGISTER_CALIB03];
            s2m_buff[4] = bme280.registers[BME280_REGISTER_CALIB04];
            s2m_buff[5] = bme280.registers[BME280_REGISTER_CALIB05];
            printf("SetupXferRecAndExecute()\tBME280_REGISTER_CALIB00\n");
            break;
        case BME280_REGISTER_TEMP_MSB:
            s2m_buff[0] = bme280.registers[BME280_REGISTER_TEMP_MSB];
            s2m_buff[1] = bme280.registers[BME280_REGISTER_TEMP_LSB];
            s2m_buff[2] = bme280.registers[BME280_REGISTER_TEMP_XLSB];
            printf("SetupXferRecAndExecute()\tBME280_REGISTER_TEMP_MSB\n");
            break;
        default:
            break;
        }
    default:
        break;
    }
}

