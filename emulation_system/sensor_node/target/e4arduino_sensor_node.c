/*
 * The Clear BSD License
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 *  modification, are permitted (subject to the limitations in the disclaimer
 *  below) provided *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#undef  SDK_DEBUGCONSOLE
#define SDK_DEBUGCONSOLE 0U

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"

#include "fsl_lpi2c.h"
#include "fsl_lpi2c_freertos.h"

#include "fsl_lpuart.h"
#include "fsl_lpuart_freertos.h"

#include "pin_mux.h"
#include "clock_config.h"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define SENSORNODE_UART LPUART0
#define SENSORNODE_UART_CLKSRC kCLOCK_ScgSircAsyncDiv3Clk
#define SENSORNODE_UART_CLOCK_FREQ CLOCK_GetFreq(kCLOCK_ScgSircAsyncDiv3Clk)
#define SENSORNODE_UART_RX_TX_IRQn LPUART0_IRQn
#define EXAMPLE_I2C_MASTER_BASE (LPI2C1_BASE)
#define EXAMPLE_I2C_MASTER_IRQN (LPI2C1_IRQn)
#define EXAMPLE_I2C_MASTER   ((LPI2C_Type *)EXAMPLE_I2C_MASTER_BASE)
#define SENSORNODE_I2C_MASTER_BASE (LPI2C1_BASE)
#define SENSORNODE_I2C_MASTER ((LPI2C_Type *)SENSORNODE_I2C_MASTER_BASE)
#define SENSORNODE_I2C_MASTER_CLOCK_FREQ 48000000U
#define SENSORNODE_I2C_MASTER_BAUDRATE (100000) /* 100K */
#define BME280_I2C_ADDR (0x76U)
#define TSL2561_I2C_ADDR (0x39U)
#define PILOT_LED_GPIO BOARD_INITPINS_PILOT_LED_GPIO
#define PILOT_LED_PORT BOARD_INITPINS_PILOT_LED_PORT
#define PILOT_LED_PIN BOARD_INITPINS_PILOT_LED_PIN
/* Task priorities. */
#define led_task_PRIORITY (configMAX_PRIORITIES - 2)
#define sensor_node_task_PRIORITY (configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void led_task(void *pvParameters);
static void sensor_node_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
uint32_t node_id;
uint8_t background_buffer[256];
uint8_t recv_buffer[32];

gpio_pin_config_t pilot_led_config = {
    kGPIO_DigitalOutput, 0,
};

lpuart_rtos_config_t lpuart_config = {
    .baudrate = 38400,
    .parity = kLPUART_ParityDisabled,
    .stopbits = kLPUART_OneStopBit,
    .buffer = background_buffer,
    .buffer_size = sizeof(background_buffer),
};

uint8_t flashing_pattern, flashing_mask;

typedef struct {
    uint32_t node_id;
    uint32_t illuminance;
    uint32_t temperature;
    uint32_t checksum;
} sensor_node_data_t;

void set_id(uint32_t *id) {
	*id = (uint32_t)0x4055ba0d;
}

static void led_task(void *pvParameters)
{
    int led_state;
    TickType_t last_tick;

    flashing_pattern = 0x0c;
    flashing_mask = 0x0c;
    GPIO_PinInit(PILOT_LED_GPIO, PILOT_LED_PIN, &pilot_led_config);
    last_tick = xTaskGetTickCount();
    for(led_state = 0;; led_state++) {
        if((led_state & flashing_mask) == flashing_pattern) {
            GPIO_PortClear(PILOT_LED_GPIO, 1u << PILOT_LED_PIN);
        } else {
            GPIO_PortSet(PILOT_LED_GPIO, 1u << PILOT_LED_PIN);
        }
        vTaskDelayUntil(&last_tick, pdMS_TO_TICKS(40));
    }
}

static status_t tsl2561_powerup(lpi2c_rtos_handle_t *i2c_handle)
{
    uint8_t powerup_command = 0x03;
    lpi2c_master_transfer_t i2c_tsl2561;
    status_t status;

    memset(&i2c_tsl2561, 0, sizeof(i2c_tsl2561));
    i2c_tsl2561.slaveAddress = TSL2561_I2C_ADDR;
    i2c_tsl2561.direction = kLPI2C_Write;
    i2c_tsl2561.subaddress = 0x80;
    i2c_tsl2561.subaddressSize = 1;
    i2c_tsl2561.data = &powerup_command;
    i2c_tsl2561.dataSize = 1;
    i2c_tsl2561.flags = kLPI2C_TransferDefaultFlag;

    status = LPI2C_RTOS_Transfer(i2c_handle, &i2c_tsl2561);
    return status;
}

static uint8_t tsl2561_read_adc_data(lpi2c_rtos_handle_t *i2c_handle,
                                     uint8_t *output)
{
    lpi2c_master_transfer_t i2c_tsl2561;
    status_t status;

    memset(&i2c_tsl2561, 0, sizeof(i2c_tsl2561));
    i2c_tsl2561.slaveAddress = TSL2561_I2C_ADDR;
    i2c_tsl2561.direction = kLPI2C_Read;
    i2c_tsl2561.subaddress = 0x9c;
    i2c_tsl2561.subaddressSize = 1;
    i2c_tsl2561.data = output;
    i2c_tsl2561.dataSize = 4;
    i2c_tsl2561.flags = kLPI2C_TransferDefaultFlag;

    status = LPI2C_RTOS_Transfer(i2c_handle, &i2c_tsl2561);

    return status;
}

float tsl2561_calib_lux(uint8_t *data)
{
    float ch0, ch1, p, lux;

    ch0 = (float)(data[0] | data[1] << 8);
    ch1 = (float)(data[2] | data[3] << 8);

    p = ch1 / ch0;
    if(p <= 0.125) {
        lux = (0.0304 - 0.0272 * p) * ch0;
    } else if(p <= 0.25) {
        lux = (0.03250 - 0.0440 * p) * ch0;
    } else if(p <= 0.375) {
        lux = (0.03510 - 0.0544 * p) * ch0;
    } else if(p <= 0.5) {
        lux = (0.03810 - 0.0624 * p) * ch0;
    } else if(p <= 0.65) {
        lux = (0.022400 - 0.0310 * p) * ch0;
    } else if(p <= 0.8) {
        lux = (0.012800 - 0.0153 * p) * ch0;
    } else if(p <= 1.3) {
        lux = (0.001460 - 0.00112 * p) * ch0;
    } else {
        lux = .0;
    }
    return lux;
}

status_t bme280_set_ctrlmeas(lpi2c_rtos_handle_t *i2c_handle, uint8_t osrs_t,
                             uint8_t osrs_p, uint8_t mode)
{
    uint8_t byte;
    lpi2c_master_transfer_t i2c_bme280;
    status_t status;

    byte = ((osrs_t & 0x07) << 5) | ((osrs_p & 0x07) << 2) | (mode & 0x03);
    memset(&i2c_bme280, 0, sizeof(i2c_bme280));
    i2c_bme280.slaveAddress = BME280_I2C_ADDR;
    i2c_bme280.direction = kLPI2C_Write;
    i2c_bme280.subaddress = 0xf4;
    i2c_bme280.subaddressSize = 1;
    i2c_bme280.data = &byte;
    i2c_bme280.dataSize = 1;
    i2c_bme280.flags = kLPI2C_TransferDefaultFlag;

    status = LPI2C_RTOS_Transfer(i2c_handle, &i2c_bme280);

    return status;
}

status_t bme280_set_config(lpi2c_rtos_handle_t *i2c_handle,
                           uint8_t t_sb, uint8_t filter)
{
    uint8_t byte;
    lpi2c_master_transfer_t i2c_bme280;
    status_t status;

    byte = ((t_sb & 0x07) << 5) | ((filter & 0x07) << 2);
    memset(&i2c_bme280, 0, sizeof(i2c_bme280));
    i2c_bme280.slaveAddress = BME280_I2C_ADDR;
    i2c_bme280.direction = kLPI2C_Write;
    i2c_bme280.subaddress = 0xf5;
    i2c_bme280.subaddressSize = 1;
    i2c_bme280.data = &byte;
    i2c_bme280.dataSize = 1;
    i2c_bme280.flags = kLPI2C_TransferDefaultFlag;

    status = LPI2C_RTOS_Transfer(i2c_handle, &i2c_bme280);

    return status;
}

status_t bme280_read_trim(lpi2c_rtos_handle_t *i2c_handle, uint16_t *trims)
{
    lpi2c_master_transfer_t i2c_bme280;
    status_t status;

    memset(&i2c_bme280, 0, sizeof(i2c_bme280));
    i2c_bme280.slaveAddress = BME280_I2C_ADDR;
    i2c_bme280.direction = kLPI2C_Read;
    i2c_bme280.subaddress = 0x88;
    i2c_bme280.subaddressSize = 1;
    i2c_bme280.data = trims;
    i2c_bme280.dataSize = 6;
    i2c_bme280.flags = kLPI2C_TransferDefaultFlag;

    status = LPI2C_RTOS_Transfer(i2c_handle, &i2c_bme280);
    return status;
}

status_t bme280_read_temp(lpi2c_rtos_handle_t *i2c_handle, uint8_t *data)
{
    lpi2c_master_transfer_t i2c_bme280;
    status_t status;

    memset(&i2c_bme280, 0, sizeof(i2c_bme280));
    i2c_bme280.slaveAddress = BME280_I2C_ADDR;
    i2c_bme280.direction = kLPI2C_Read;
    i2c_bme280.subaddress = 0xfa;
    i2c_bme280.subaddressSize = 1;
    i2c_bme280.data = data;
    i2c_bme280.dataSize = 3;
    i2c_bme280.flags = kLPI2C_TransferDefaultFlag;

    status = LPI2C_RTOS_Transfer(i2c_handle, &i2c_bme280);
    return status;
}

int32_t bme280_calib_temp(uint8_t *values, uint16_t *trims)
{
    int32_t v0, v1, t1, t2, t3, result;
    uint32_t raw_t;

    raw_t = (values[0] << 12) | (values[1] << 4) | (values[2] >> 4);
    t1 = (int32_t)trims[0];
    t2 = (int32_t)trims[1];
    t3 = (int32_t)trims[2];

    v0 = (((raw_t >> 3) - (t1 << 1)) * t2) >> 11;
    v1 = (((((raw_t >> 4) - t1) * ((raw_t >> 4) - t1)) >> 12) * t3) >> 14;
    result = (((v0 + v1) * 5 + 128) >> 8) * 10;
    return result;
}

static void sensor_node_task(void *pvParameters)
{
    uint16_t trims[3];
    status_t status;
    sensor_node_data_t data;
    TickType_t last_tick;

    lpuart_rtos_handle_t handle;
    struct _lpuart_handle t_handle;

    lpuart_config.srcclk = SENSORNODE_UART_CLOCK_FREQ;
    lpuart_config.base = SENSORNODE_UART;

    lpi2c_master_config_t masterConfig;
    lpi2c_rtos_handle_t i2c_handle;

    status = LPUART_RTOS_Init(&handle, &t_handle, &lpuart_config);
    if(status != kStatus_Success) {
        PRINTF("LPUART: Error initializing LPUART!\r\n");
        for(;;) {
            flashing_pattern = 0x00;
            flashing_mask = 0x00;
            taskYIELD();
        }
    }

    LPI2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.debugEnable = false;
    masterConfig.ignoreAck = false;
    masterConfig.pinConfig = kLPI2C_2PinOpenDrain;
    masterConfig.baudRate_Hz = SENSORNODE_I2C_MASTER_BAUDRATE;
    masterConfig.busIdleTimeout_ns = 0;
    masterConfig.pinLowTimeout_ns = 0;
    masterConfig.sdaGlitchFilterWidth_ns = 0;
    masterConfig.sclGlitchFilterWidth_ns = 0;

    status = LPI2C_RTOS_Init(&i2c_handle, SENSORNODE_I2C_MASTER, &masterConfig,
                             SENSORNODE_I2C_MASTER_CLOCK_FREQ);
    if(status != kStatus_Success) {
        PRINTF("LPI2C master: Error initializing LPI2C!\r\n");
        for(;;) {
            flashing_pattern = 0x00;
            flashing_mask = 0x00;
            taskYIELD();
        }
    }

    status = tsl2561_powerup(&i2c_handle);
    if(status != kStatus_Success) {
        PRINTF("TSL2561 communication failed.\r\n");
        for(;;) {
            flashing_pattern = 0x00;
            flashing_mask = 0x00;
            taskYIELD();
        }
    }

    status = bme280_set_ctrlmeas(&i2c_handle, 1, 0, 3);
    if(status != kStatus_Success) {
        PRINTF("BME280 communication failed.\r\n");
        for(;;) {
            flashing_pattern = 0x00;
            flashing_mask = 0x00;
            taskYIELD();
        }
    }
    status = bme280_set_config(&i2c_handle, 3, 0);
    if(status != kStatus_Success) {
        PRINTF("BME280 communication failed.\r\n");
        for(;;) {
            flashing_pattern = 0x00;
            flashing_mask = 0x00;
            taskYIELD();
        }
    }
    status = bme280_read_trim(&i2c_handle, trims);
    if(status != kStatus_Success) {
        PRINTF("BME280 communication failed.\r\n");
        for(;;) {
            flashing_pattern = 0x00;
            flashing_mask = 0x00;
            taskYIELD();
        }
    }

    data.node_id = node_id;

    last_tick = xTaskGetTickCount();
    for(;;)
    {
        int i;
        uint8_t values[4], *p = &data;

        status = tsl2561_read_adc_data(&i2c_handle, values);
        if(status != kStatus_Success)
        {
            PRINTF("TSL2561 communication failed.\r\n");
            for(;;) {
                flashing_pattern = 0x00;
                flashing_mask = 0x00;
                taskYIELD();
            }
        } else {
            data.illuminance = (uint32_t)(tsl2561_calib_lux(values) * 1000);
        }

        status = bme280_read_temp(&i2c_handle, values);
        if(status != kStatus_Success) {
            PRINTF("BME280 communication failed.\r\n");
            for(;;) {
                flashing_pattern = 0x00;
                flashing_mask = 0x00;
                taskYIELD();
            }
        } else {
            data.temperature = (uint32_t)(bme280_calib_temp(values, trims));
        }

        data.checksum = 0;
        for(i = 0; i < 12; i++) {
            data.checksum += p[i];
        }

        status = LPUART_RTOS_Send(&handle, (uint8_t *)&data, sizeof(data));
        if(status != kStatus_Success) {
            PRINTF("LPUART communication failed.\r\n");
            for(;;) {
                flashing_pattern = 0x00;
                flashing_mask = 0x00;
                taskYIELD();
            }
        }
        vTaskDelayUntil(&last_tick, pdMS_TO_TICKS(8000));
    }

    for(;;) {
        flashing_pattern = 0x00;
        flashing_mask = 0x00;
        taskYIELD();
    }

    LPI2C_RTOS_Deinit(&i2c_handle);
    LPUART_RTOS_Deinit(&handle);
}

/*!
 * @brief Application entry point.
 */
int main(void)
{
    /* Init board hardware. */
    BOARD_InitPins();
    BOARD_BootClockRUN();
    CLOCK_SetIpSrc(kCLOCK_Lpuart0, kCLOCK_IpSrcSircAsync);
    CLOCK_SetIpSrc(kCLOCK_Lpi2c1, kCLOCK_IpSrcFircAsync);
    set_id(&node_id);
    if(xTaskCreate(sensor_node_task, "SensorNode_task",
                   configMINIMAL_STACK_SIZE + 70, NULL,
                   sensor_node_task_PRIORITY, NULL) != pdPASS) {
        PRINTF("Task creation failed!.\r\n");
        for(;;) {
            flashing_pattern = 0x00;
            flashing_mask = 0x00;
            taskYIELD();
        }
    }
    if(xTaskCreate(led_task, "LED_task", configMINIMAL_STACK_SIZE + 70, NULL,
                   led_task_PRIORITY, NULL) != pdPASS) {
        PRINTF("Task creation failed!.\r\n");
        for(;;) {
            flashing_pattern = 0x00;
            flashing_mask = 0x00;
            taskYIELD();
        }
    }
    vTaskStartScheduler();
    for (;;);
}
