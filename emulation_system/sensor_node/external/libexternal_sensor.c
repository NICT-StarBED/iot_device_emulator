#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include "xft.h"

#define MAKE_STATUS(group, code) ((((group)*100) + (code)))

typedef u_int8_t uint8_t;
typedef u_int16_t uint16_t;
typedef u_int32_t uint32_t;
typedef int32_t status_t;

enum _status_groups {
    kStatusGroup_Generic = 0,                 /*!< Group number for generic status codes. */
    kStatusGroup_FLASH = 1,                   /*!< Group number for FLASH status codes. */
    kStatusGroup_LPSPI = 4,                   /*!< Group number for LPSPI status codes. */
    kStatusGroup_FLEXIO_SPI = 5,              /*!< Group number for FLEXIO SPI status codes. */
    kStatusGroup_DSPI = 6,                    /*!< Group number for DSPI status codes. */
    kStatusGroup_FLEXIO_UART = 7,             /*!< Group number for FLEXIO UART status codes. */
    kStatusGroup_FLEXIO_I2C = 8,              /*!< Group number for FLEXIO I2C status codes. */
    kStatusGroup_LPI2C = 9,                   /*!< Group number for LPI2C status codes. */
    kStatusGroup_UART = 10,                   /*!< Group number for UART status codes. */
    kStatusGroup_I2C = 11,                    /*!< Group number for UART status codes. */
    kStatusGroup_LPSCI = 12,                  /*!< Group number for LPSCI status codes. */
    kStatusGroup_LPUART = 13,                 /*!< Group number for LPUART status codes. */
    kStatusGroup_SPI = 14,                    /*!< Group number for SPI status code.*/
    kStatusGroup_XRDC = 15,                   /*!< Group number for XRDC status code.*/
    kStatusGroup_SEMA42 = 16,                 /*!< Group number for SEMA42 status code.*/
    kStatusGroup_SDHC = 17,                   /*!< Group number for SDHC status code */
    kStatusGroup_SDMMC = 18,                  /*!< Group number for SDMMC status code */
    kStatusGroup_SAI = 19,                    /*!< Group number for SAI status code */
    kStatusGroup_MCG = 20,                    /*!< Group number for MCG status codes. */
    kStatusGroup_SCG = 21,                    /*!< Group number for SCG status codes. */
    kStatusGroup_SDSPI = 22,                  /*!< Group number for SDSPI status codes. */
    kStatusGroup_FLEXIO_I2S = 23,             /*!< Group number for FLEXIO I2S status codes */
    kStatusGroup_FLEXIO_MCULCD = 24,          /*!< Group number for FLEXIO LCD status codes */
    kStatusGroup_FLASHIAP = 25,               /*!< Group number for FLASHIAP status codes */
    kStatusGroup_FLEXCOMM_I2C = 26,           /*!< Group number for FLEXCOMM I2C status codes */
    kStatusGroup_I2S = 27,                    /*!< Group number for I2S status codes */
    kStatusGroup_IUART = 28,                  /*!< Group number for IUART status codes */
    kStatusGroup_CSI = 29,                    /*!< Group number for CSI status codes */
    kStatusGroup_MIPI_DSI = 30,               /*!< Group number for MIPI DSI status codes */
    kStatusGroup_SDRAMC = 35,                 /*!< Group number for SDRAMC status codes. */
    kStatusGroup_POWER = 39,                  /*!< Group number for POWER status codes. */
    kStatusGroup_ENET = 40,                   /*!< Group number for ENET status codes. */
    kStatusGroup_PHY = 41,                    /*!< Group number for PHY status codes. */
    kStatusGroup_TRGMUX = 42,                 /*!< Group number for TRGMUX status codes. */
    kStatusGroup_SMARTCARD = 43,              /*!< Group number for SMARTCARD status codes. */
    kStatusGroup_LMEM = 44,                   /*!< Group number for LMEM status codes. */
    kStatusGroup_QSPI = 45,                   /*!< Group number for QSPI status codes. */
    kStatusGroup_DMA = 50,                    /*!< Group number for DMA status codes. */
    kStatusGroup_EDMA = 51,                   /*!< Group number for EDMA status codes. */
    kStatusGroup_DMAMGR = 52,                 /*!< Group number for DMAMGR status codes. */
    kStatusGroup_FLEXCAN = 53,                /*!< Group number for FlexCAN status codes. */
    kStatusGroup_LTC = 54,                    /*!< Group number for LTC status codes. */
    kStatusGroup_FLEXIO_CAMERA = 55,          /*!< Group number for FLEXIO CAMERA status codes. */
    kStatusGroup_LPC_SPI = 56,                /*!< Group number for LPC_SPI status codes. */
    kStatusGroup_LPC_USART = 57,              /*!< Group number for LPC_USART status codes. */
    kStatusGroup_DMIC = 58,                   /*!< Group number for DMIC status codes. */
    kStatusGroup_SDIF = 59,                   /*!< Group number for SDIF status codes.*/
    kStatusGroup_SPIFI = 60,                  /*!< Group number for SPIFI status codes. */
    kStatusGroup_OTP = 61,                    /*!< Group number for OTP status codes. */
    kStatusGroup_MCAN = 62,                   /*!< Group number for MCAN status codes. */
    kStatusGroup_CAAM = 63,                   /*!< Group number for CAAM status codes. */
    kStatusGroup_ECSPI = 64,                  /*!< Group number for ECSPI status codes. */
    kStatusGroup_USDHC = 65,                  /*!< Group number for USDHC status codes.*/
    kStatusGroup_LPC_I2C = 66,                /*!< Group number for LPC_I2C status codes.*/
    kStatusGroup_DCP = 67,                    /*!< Group number for DCP status codes.*/
    kStatusGroup_MSCAN = 68,                  /*!< Group number for MSCAN status codes.*/
    kStatusGroup_ESAI = 69,                   /*!< Group number for ESAI status codes. */
    kStatusGroup_FLEXSPI = 70,                /*!< Group number for FLEXSPI status codes. */
    kStatusGroup_MMDC = 71,                   /*!< Group number for MMDC status codes. */
    kStatusGroup_MICFIL = 72,                 /*!< Group number for MIC status codes. */
    kStatusGroup_SDMA = 73,                   /*!< Group number for SDMA status codes. */
    kStatusGroup_ICS = 74,                    /*!< Group number for ICS status codes. */
    kStatusGroup_SPDIF = 75,                  /*!< Group number for SPDIF status codes. */
    kStatusGroup_LPC_MINISPI = 76,            /*!< Group number for LPC_MINISPI status codes. */
    kStatusGroup_NOTIFIER = 98,               /*!< Group number for NOTIFIER status codes. */
    kStatusGroup_DebugConsole = 99,           /*!< Group number for debug console status codes. */
    kStatusGroup_SEMC = 100,                   /*!< Group number for SEMC status codes. */
    kStatusGroup_ApplicationRangeStart = 101, /*!< Starting number for application groups. */
};

enum _generic_status {
    kStatus_Success = MAKE_STATUS(kStatusGroup_Generic, 0),
    kStatus_Fail = MAKE_STATUS(kStatusGroup_Generic, 1),
    kStatus_ReadOnly = MAKE_STATUS(kStatusGroup_Generic, 2),
    kStatus_OutOfRange = MAKE_STATUS(kStatusGroup_Generic, 3),
    kStatus_InvalidArgument = MAKE_STATUS(kStatusGroup_Generic, 4),
    kStatus_Timeout = MAKE_STATUS(kStatusGroup_Generic, 5),
    kStatus_NoTransferInProgress = MAKE_STATUS(kStatusGroup_Generic, 6),
};

XFT_EXPOSED_FUNCTION
void BOARD_BootClockRUN(void) {
    printf("[XFT] Trap void BOARD_BootClockRUN(void)\n");
}

u_int32_t uuid;

XFT_EXPOSED_FUNCTION
void BOARD_InitPins(void) {
    FILE *fp;
    char buf[BUFSIZ];
    int i, num;

    printf("[XFT] Trap void BOARD_InitPins(void)\n");

    if ((fp = fopen("/var/tmp/.node_id", "r")) == NULL) {
        num = 0;
    } else {
        while (fgets(buf, sizeof(buf), fp) != NULL) ;
        num = atoi(buf) + 1;
        if (num >= 64)  num = 0;
        fclose(fp);
    }

    if ((fp = fopen("/var/tmp/.node_id", "w")) != NULL) {
        fprintf(fp, "%d", num);
        fclose(fp);
    } else {
        fprintf(stderr, "[XFT] node_id setting error: fopen\n");
        return;
    }

    if ((fp = fopen("external/uuid.dat", "r")) == NULL) {
        fprintf(stderr, "[XFT] node_id setting error: uuid file not found\n");
        return;
    }
    for (i = 0; i < num + 1; i++) {
        fscanf(fp, "%08x\n", &uuid);
    }
    printf("[XFT] sensor node number: %02d\n", num);
    printf("[XFT] sensor node id: %08x\n", uuid);
    fclose(fp);
}

typedef enum _clock_ip_name {
    kCLOCK_IpInvalid = 0U,
    /* PCC 0 */
    kCLOCK_Dma0 = 0x4007A020U,
    kCLOCK_Flash0 = 0x4007A080U,
    kCLOCK_Dmamux0 = 0x4007A084U,
    kCLOCK_Intmux0 = 0x4007A090U,
    kCLOCK_Tpm2 = 0x4007A0B8U,
    kCLOCK_Lpit0 = 0x4007A0C0U,
    kCLOCK_Lptmr0 = 0x4007A0D0U,
    kCLOCK_Rtc0 = 0x4007A0E0U,
    kCLOCK_Lpspi2 = 0x4007A0F8U,
    kCLOCK_Lpi2c2 = 0x4007A108U,
    kCLOCK_Lpuart2 = 0x4007A118U,
    kCLOCK_Sai0 = 0x4007A130U,
    kCLOCK_Emvsim0 = 0x4007A138U,
    kCLOCK_Usbfs0 = 0x4007A154U,
    kCLOCK_PortA = 0x4007A168U,
    kCLOCK_PortB = 0x4007A16CU,
    kCLOCK_PortC = 0x4007A170U,
    kCLOCK_PortD = 0x4007A174U,
    kCLOCK_PortE = 0x4007A178U,
    kCLOCK_Tsi0 = 0x4007A188U,
    kCLOCK_Adc0 = 0x4007A198U,
    kCLOCK_Dac0 = 0x4007A1A8U,
    kCLOCK_Cmp0 = 0x4007A1B8U,
    kCLOCK_Vref0 = 0x4007A1C8U,
    kCLOCK_Crc0 = 0x4007A1E0U,
    /* PCC 1 */
    kCLOCK_Trng0 = 0x400FA094U,
    kCLOCK_Tpm0 = 0x400FA0B0U,
    kCLOCK_Tpm1 = 0x400FA0B4U,
    kCLOCK_Lptmr1 = 0x400FA0D4U,
    kCLOCK_Lpspi0 = 0x400FA0F0U,
    kCLOCK_Lpspi1 = 0x400FA0F4U,
    kCLOCK_Lpi2c0 = 0x400FA100U,
    kCLOCK_Lpi2c1 = 0x400FA104U,
    kCLOCK_Lpuart0 = 0x400FA110U,
    kCLOCK_Lpuart1 = 0x400FA114U,
    kCLOCK_Flexio0 = 0x400FA128U,
    kCLOCK_Cmp1 = 0x400FA1BCU,
} clock_ip_name_t;

typedef int32_t status_t;

typedef enum _clock_ip_src
{
    kCLOCK_IpSrcNoneOrExt = 0U,   /*!< Clock is off or external clock is used. */
    kCLOCK_IpSrcSysOscAsync = 1U, /*!< System Oscillator async clock.          */
    kCLOCK_IpSrcSircAsync = 2U,   /*!< Slow IRC async clock.                   */
    kCLOCK_IpSrcFircAsync = 3U,   /*!< Fast IRC async clock.                   */
    kCLOCK_IpSrcSysPllAsync = 6U  /*!< System PLL async clock.                 */
} clock_ip_src_t;

XFT_EXPOSED_FUNCTION
void CLOCK_SetIpSrc(clock_ip_name_t name, clock_ip_src_t src)
{
    printf("[XFT] Trap void CLOCK_SetIpSrc(clock_ip_name_t name, clock_ip_src_t src)\n");
    printf("[XFT] clock_ip_name_t is enum type. value: 0x%08x\n", name);
    printf("[XFT] clock_ip_src_t is enum type. value: 0x%d\n", src);
}

XFT_EXPOSED_FUNCTION
void CLOCK_EnableClock(clock_ip_name_t name)
{
    printf("[XFT] Trap void CLOCK_EnableClock(clock_ip_name_t name)\n");
    printf("[XFT] clock_ip_name_t is enum type. value: 0x%08x\n", name);
}

int fd_i2c, fd_uart;
char i2c_path[BUFSIZ], uart_path[BUFSIZ];
struct sockaddr_un sun_i2c, sun_uart;

XFT_EXPOSED_FUNCTION
int LPUART_RTOS_Init(cortex_m_ptr_t handle, cortex_m_ptr_t t_handle, const cortex_m_ptr_t cfg)
{
    // char buf[BUFSIZ] = {'\0'};
    printf("[XFT] Trap int LPUART_RTOS_Init(lpuart_rtos_handle_t *handle, lpuart_handle_t *t_handle, const lpuart_rtos_config_t *cfg)\n");
    printf("[XFT] lpuart_rtos_handle_t* is struct pointer. address: 0x%08x\n", handle);
    printf("[XFT] lpuart_handle_t* is struct pointer. address: 0x%08x\n", t_handle);
    printf("[XFT] lpuart_rtos_config_t* is struct pointer. address: 0x%08x\n", cfg);

    return kStatus_Success;
}

XFT_EXPOSED_FUNCTION
status_t LPI2C_RTOS_Init(cortex_m_ptr_t handle, cortex_m_ptr_t base,
                    cortex_m_ptr_t masterConfig, uint32_t srcClock_Hz)
{
    printf("[XFT] Trap status_t LPI2C_RTOS_Init(lpi2c_rtos_handle_t *handle, LPI2C_Type *base, const lpi2c_master_config_t *masterConfig, uint32_t srcClock_Hz)\n");
    printf("[XFT] lpi2c_rtos_handle_t* is struct pointer. address: 0x%08x\n", handle);
    printf("[XFT] LPI2C_Type* is struct pointer. address: 0x%08x\n", base);
    printf("[XFT] lpi2c_master_config_t* is struct pointer. address: 0x%08x\n", masterConfig);
    printf("[XFT] uint32_t is value type. value: %d\n", srcClock_Hz);

    return kStatus_Success;
}

typedef enum _lpi2c_direction
{
    kLPI2C_Write = 0U, /*!< Master transmit. */
    kLPI2C_Read = 1U   /*!< Master receive. */
} lpi2c_direction_t;

#define     __I     volatile const       /*!< Defines 'read only' permissions */
#define     __O     volatile             /*!< Defines 'write only' permissions */
#define     __IO    volatile             /*!< Defines 'read / write' permissions */

typedef struct {
  __I  uint32_t VERID;                             /**< Version ID Register, offset: 0x0 */
  __I  uint32_t PARAM;                             /**< Parameter Register, offset: 0x4 */
       uint8_t RESERVED_0[8];
  __IO uint32_t MCR;                               /**< Master Control Register, offset: 0x10 */
  __IO uint32_t MSR;                               /**< Master Status Register, offset: 0x14 */
  __IO uint32_t MIER;                              /**< Master Interrupt Enable Register, offset: 0x18 */
  __IO uint32_t MDER;                              /**< Master DMA Enable Register, offset: 0x1C */
  __IO uint32_t MCFGR0;                            /**< Master Configuration Register 0, offset: 0x20 */
  __IO uint32_t MCFGR1;                            /**< Master Configuration Register 1, offset: 0x24 */
  __IO uint32_t MCFGR2;                            /**< Master Configuration Register 2, offset: 0x28 */
  __IO uint32_t MCFGR3;                            /**< Master Configuration Register 3, offset: 0x2C */
       uint8_t RESERVED_1[16];
  __IO uint32_t MDMR;                              /**< Master Data Match Register, offset: 0x40 */
       uint8_t RESERVED_2[4];
  __IO uint32_t MCCR0;                             /**< Master Clock Configuration Register 0, offset: 0x48 */
       uint8_t RESERVED_3[4];
  __IO uint32_t MCCR1;                             /**< Master Clock Configuration Register 1, offset: 0x50 */
       uint8_t RESERVED_4[4];
  __IO uint32_t MFCR;                              /**< Master FIFO Control Register, offset: 0x58 */
  __I  uint32_t MFSR;                              /**< Master FIFO Status Register, offset: 0x5C */
  __O  uint32_t MTDR;                              /**< Master Transmit Data Register, offset: 0x60 */
       uint8_t RESERVED_5[12];
  __I  uint32_t MRDR;                              /**< Master Receive Data Register, offset: 0x70 */
       uint8_t RESERVED_6[156];
  __IO uint32_t SCR;                               /**< Slave Control Register, offset: 0x110 */
  __IO uint32_t SSR;                               /**< Slave Status Register, offset: 0x114 */
  __IO uint32_t SIER;                              /**< Slave Interrupt Enable Register, offset: 0x118 */
  __IO uint32_t SDER;                              /**< Slave DMA Enable Register, offset: 0x11C */
       uint8_t RESERVED_7[4];
  __IO uint32_t SCFGR1;                            /**< Slave Configuration Register 1, offset: 0x124 */
  __IO uint32_t SCFGR2;                            /**< Slave Configuration Register 2, offset: 0x128 */
       uint8_t RESERVED_8[20];
  __IO uint32_t SAMR;                              /**< Slave Address Match Register, offset: 0x140 */
       uint8_t RESERVED_9[12];
  __I  uint32_t SASR;                              /**< Slave Address Status Register, offset: 0x150 */
  __IO uint32_t STAR;                              /**< Slave Transmit ACK Register, offset: 0x154 */
       uint8_t RESERVED_10[8];
  __O  uint32_t STDR;                              /**< Slave Transmit Data Register, offset: 0x160 */
       uint8_t RESERVED_11[12];
  __I  uint32_t SRDR;                              /**< Slave Receive Data Register, offset: 0x170 */
} LPI2C_Type;

typedef struct _lpi2c_master_handle lpi2c_master_handle_t;
typedef void (*lpi2c_master_transfer_callback_t)(LPI2C_Type *base,
                                                 lpi2c_master_handle_t *handle,
                                                 status_t completionStatus,
                                                 void *userData);

enum _lpi2c_master_transfer_flags
{
    kLPI2C_TransferDefaultFlag = 0x00U,       /*!< Transfer starts with a start signal, stops with a stop signal. */
    kLPI2C_TransferNoStartFlag = 0x01U,       /*!< Don't send a start condition, address, and sub address */
    kLPI2C_TransferRepeatedStartFlag = 0x02U, /*!< Send a repeated start condition */
    kLPI2C_TransferNoStopFlag = 0x04U,        /*!< Don't send a stop condition. */
};

typedef struct _lpi2c_master_transfer lpi2c_master_transfer_t;
struct _lpi2c_master_transfer
{
    uint32_t
        flags; /*!< Bit mask of options for the transfer. See enumeration #_lpi2c_master_transfer_flags for available
                  options. Set to 0 or #kLPI2C_TransferDefaultFlag for normal transfers. */
    uint16_t slaveAddress;        /*!< The 7-bit slave address. */
    uint16_t direction;           /*!< Either #kLPI2C_Read or #kLPI2C_Write. */
    uint32_t subaddress;          /*!< Sub address. Transferred MSB first. */
    uint32_t subaddressSize;      /*!< Length of sub address to send in bytes. Maximum size is 4 bytes. */
    cortex_m_ptr_t data;                /*!< Pointer to data to transfer. */
    uint32_t dataSize;            /*!< Number of bytes to transfer. */
};

#define TSL2561_I2C_ADDR      (0x39U)
#define BME280_I2C_ADDR       (0x76U)

#define TSL2561_READBIT           (0x01)

#define TSL2561_COMMAND_BIT       (0x80)    // Must be 1
#define TSL2561_CLEAR_BIT         (0x40)    // Clears any pending interrupt (write 1 to clear)
#define TSL2561_WORD_BIT          (0x20)    // 1 = read/write word (rather than byte)
#define TSL2561_BLOCK_BIT         (0x10)    // 1 = using block read/write

#define TSL2561_CONTROL_POWERON   (0x03)
#define TSL2561_CONTROL_POWEROFF  (0x00)

enum
{
    TSL2561_REGISTER_CONTROL          = 0x00,
    TSL2561_REGISTER_TIMING           = 0x01,
    TSL2561_REGISTER_THRESHHOLDL_LOW  = 0x02,
    TSL2561_REGISTER_THRESHHOLDL_HIGH = 0x03,
    TSL2561_REGISTER_THRESHHOLDH_LOW  = 0x04,
    TSL2561_REGISTER_THRESHHOLDH_HIGH = 0x05,
    TSL2561_REGISTER_INTERRUPT        = 0x06,
    TSL2561_REGISTER_CRC              = 0x08,
    TSL2561_REGISTER_ID               = 0x0A,
    TSL2561_REGISTER_CHAN0_LOW        = 0x0C,
    TSL2561_REGISTER_CHAN0_HIGH       = 0x0D,
    TSL2561_REGISTER_CHAN1_LOW        = 0x0E,
    TSL2561_REGISTER_CHAN1_HIGH       = 0x0F
};

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

typedef struct {
    u_int8_t initialized;
    u_int8_t registers[0x10];
} tsl2561_t;

tsl2561_t tsl2561 = {1,
    {
        0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x09, 0x50, 0x04, 0xA2, 0x02, 0x53, 0x00,
    }
};

static void tsl2561_poweron(void)
{
    /* Do something */
}

static void tsl2561_poweroff(void)
{
    /* Do something */
}

static void bme280_reset(void)
{
    /* Do something */
}

float lux;

XFT_EXPOSED_FUNCTION
float tsl2561_calib_lux(cortex_m_ptr_t data) {
    uint8_t *data_ptr = (uint8_t *)XFT_CONVERT_ADDR_T2H(data);

    float ch0, ch1, p;

    ch0 = (float)(data_ptr[0] | data_ptr[1] << 8);
    ch1 = (float)(data_ptr[2] | data_ptr[3] << 8);

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

XFT_EXPOSED_FUNCTION
status_t LPI2C_RTOS_Transfer(cortex_m_ptr_t handle, cortex_m_ptr_t transfer)
{
    lpi2c_master_transfer_t *xfer_ptr = (lpi2c_master_transfer_t *)XFT_CONVERT_ADDR_T2H(transfer);
    u_int8_t out;
    int i, length;
    char buf[BUFSIZ];

    printf("[XFT] Trap status_t LPI2C_RTOS_Transfer(lpi2c_rtos_handle_t *handle, lpi2c_master_transfer_t *transfer)\n");
    printf("[XFT] lpi2c_rtos_handle_t* is struct pointer. address: 0x%08x\n", handle);
    printf("[XFT] lpi2c_master_transfer_t* is struct pointer. address: 0x%08x\n", transfer);

    fd_i2c = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_i2c < 0) {
        perror("socket");
    return kStatus_Fail;
    }
    memset(&sun_i2c, 0, sizeof(struct sockaddr_un));
    sun_i2c.sun_family = AF_UNIX;
    sprintf(buf, "/var/tmp/.%08x.i2c", uuid);
    // strcpy(sun_i2c.sun_path, "/var/tmp/.i2c");
    strcpy(sun_i2c.sun_path, buf);
    if (connect(fd_i2c, (struct sockaddr *)&sun_i2c, sizeof(struct sockaddr_un)) < 0) {
        perror("connect");
        return kStatus_Success;
        // return kStatus_Fail;
    }

    switch (xfer_ptr->slaveAddress) {
    case TSL2561_I2C_ADDR:
        if (xfer_ptr->direction == kLPI2C_Write) {
            if (xfer_ptr->subaddress & TSL2561_COMMAND_BIT) {
                tsl2561.registers[xfer_ptr->subaddress & 0x0f] |= (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data));
                switch (xfer_ptr->subaddress & 0x0f) {
                case TSL2561_REGISTER_CONTROL:
                    switch (*(uint8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data)) {
                    case TSL2561_CONTROL_POWEROFF:
                        printf("[XFT] LPI2C_RTOS_Transfer()\tTSL2561_CONTROL_POWEROFF\n");
                        tsl2561_poweroff();
                        break;
                    case TSL2561_CONTROL_POWERON:
                        printf("[XFT] LPI2C_RTOS_Transfer()\tTSL2561_CONTROL_POWERON\n");
                        tsl2561_poweron();
                        break;
                    default:
                        fprintf(stderr, "tsl2561: unknown command\n");
                        break;
                    }
                    break;
                default:
                    fprintf(stderr, "tsl2561: unknown command\n");
                    break;
                }
            }
        } else {
            if (xfer_ptr->subaddress & TSL2561_COMMAND_BIT) {
                switch (xfer_ptr->subaddress & 0x0f) {
                case TSL2561_REGISTER_CHAN0_LOW:
                    out = TSL2561_REGISTER_CHAN0_LOW;
                    if (write(fd_i2c, &out, 1) > 0) {
                        u_int8_t in;
                        printf("[XFT] LPI2C_RTOS_Transfer()\tTSL2561_REGISTER_CHAN0_LOW\n");
                        for (i = 0; i < xfer_ptr->dataSize;) {
                            if (read(fd_i2c, &in, sizeof(in)) > 0) {
                                tsl2561.registers[TSL2561_REGISTER_CHAN0_LOW + i] = in;
                                (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data + i)) = in;
                                printf("[XFT] read: %02x\n", in);
                                i++;
                            } else {
                                perror("read");
                                for (i = 0; i < xfer_ptr->dataSize; i++) {
                                    (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data + i)) = 0x0;
                                }
                                return kStatus_Fail;
                            }
                        }
                    } else {
                        printf("[XFT] illuminance sensor not found\n");
                        for (i = 0; i < xfer_ptr->dataSize; i++) {
                            (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data + i)) = 0x0;
                        }
                    }
                    break;
                default:
                    fprintf(stderr, "tsl2561: unknown command\n");
                    break;
                }
            }
        }
        break;
    case BME280_I2C_ADDR:
        switch(xfer_ptr->subaddress) {
        case BME280_REGISTER_ID:
            (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data)) = 0x60;
            printf("[XFT] LPI2C_RTOS_Transfer()\tBME280_REGISTER_ID\n");
            break;
        case BME280_REGISTER_RESET:
            if((*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data)) == 0xb6) {
                bme280_reset();
            }
            printf("[XFT] LPI2C_RTOS_Transfer()\tBME280_REGISTER_RESET\n");
            break;
        case BME280_REGISTER_CTRL_HUM:
            bme280.registers[BME280_REGISTER_CTRL_HUM] =
                (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data));
            printf("[XFT] LPI2C_RTOS_Transfer()\tBME280_REGISTER_CTRL_HUM\n");
            break;
        case BME280_REGISTER_CTRL_MEAS:
            bme280.registers[BME280_REGISTER_CTRL_MEAS] =
                (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data));
            printf("[XFT] LPI2C_RTOS_Transfer()\tBME280_REGISTER_CTRL_MEAS\n");
            break;
        case BME280_REGISTER_CONFIG:
            bme280.registers[BME280_REGISTER_CONFIG] =
                (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data));
            printf("[XFT] LPI2C_RTOS_Transfer()\tBME280_REGISTER_CONFIG\n");
            break;
        case BME280_REGISTER_CALIB00:
            for (int i = 0; i < xfer_ptr->dataSize; i++) {
                (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data + i))
                    = bme280.registers[BME280_REGISTER_CALIB00 + i];
            }
            printf("[XFT] LPI2C_RTOS_Transfer()\tBME280_REGISTER_CALIB00\n");
            break;
        case BME280_REGISTER_TEMP_MSB:
            out = BME280_REGISTER_TEMP_MSB;
            if (write(fd_i2c, &out, 1) > 0) {
                u_int8_t in;
                printf("[XFT] LPI2C_RTOS_Transfer()\tBME280_REGISTER_TEMP_MSB\n");
                for (i = 0; i < xfer_ptr->dataSize;) {
                    if (read(fd_i2c, &in, sizeof(in)) > 0) {
                        bme280.registers[BME280_REGISTER_TEMP_MSB + i] = in;
                        (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data + i)) = in;
                        printf("[XFT] read: %02x\n", in);
                        i++;
                    } else {
                        fprintf(stderr, "[XFT] sensor not found\n");
                        for (i = 0; i < xfer_ptr->dataSize; i++) {
                            (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data + i)) = 0x0;
                        }
                    }
                }
            } else {
                for (i = 0; i < xfer_ptr->dataSize; i++) {
                    printf("[XFT] temperature sensor not found\n");
                    (*(u_int8_t *)XFT_CONVERT_ADDR_T2H(xfer_ptr->data + i)) = 0x0;
                }
            }
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    close(fd_i2c);
    printf("[XFT] return value is value type. value: 0x%08x\n", kStatus_Success);
    return kStatus_Success;
}

typedef struct {
  __I  uint32_t VERID;                             /**< Version ID Register, offset: 0x0 */
  __I  uint32_t PARAM;                             /**< Parameter Register, offset: 0x4 */
  __IO uint32_t GLOBAL;                            /**< LPUART Global Register, offset: 0x8 */
  __IO uint32_t PINCFG;                            /**< LPUART Pin Configuration Register, offset: 0xC */
  __IO uint32_t BAUD;                              /**< LPUART Baud Rate Register, offset: 0x10 */
  __IO uint32_t STAT;                              /**< LPUART Status Register, offset: 0x14 */
  __IO uint32_t CTRL;                              /**< LPUART Control Register, offset: 0x18 */
  __IO uint32_t DATA;                              /**< LPUART Data Register, offset: 0x1C */
  __IO uint32_t MATCH;                             /**< LPUART Match Address Register, offset: 0x20 */
  __IO uint32_t MODIR;                             /**< LPUART Modem IrDA Register, offset: 0x24 */
  __IO uint32_t FIFO;                              /**< LPUART FIFO Register, offset: 0x28 */
  __IO uint32_t WATER;                             /**< LPUART Watermark Register, offset: 0x2C */
} LPUART_Type;

typedef struct {
    uint32_t node_id;
    uint32_t illuminance;
    uint32_t temperature;
    uint32_t checksum;
} sensor_node_data_t;

XFT_EXPOSED_FUNCTION
int LPUART_RTOS_Send(cortex_m_ptr_t handle, const cortex_m_ptr_t buffer, uint32_t length)
{
    int i;
    sensor_node_data_t *sensor_node_ptr = (sensor_node_data_t *)XFT_CONVERT_ADDR_T2H(buffer);

    printf("[XFT] Trap int LPUART_RTOS_Send(lpuart_rtos_handle_t *handle, const uint8_t *buffer, uint32_t length)\n");
    printf("[XFT] lpuart_rtos_handle_t* is struct pointer. address: 0x%08x\n", handle);
    printf("[XFT] uint8_t* is pointer. address: 0x%08x\n", buffer);
    printf("[XFT] uint32_t is value type. value: %02x\n", length);

    sensor_node_ptr->illuminance = (uint32_t)(lux * 1000);

    uint8_t* buffer_ptr = (uint8_t *)XFT_CONVERT_ADDR_T2H(buffer);

    fd_uart = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd_uart < 0) {
        printf("socket error\n");
        return kStatus_Fail;
    }
    memset(&sun_uart, 0, sizeof(struct sockaddr_un));
    sun_uart.sun_family = AF_UNIX;
    // sprintf(buf, "/var/tmp/.%08d.uart", uuid);
    strcpy(sun_uart.sun_path, "/var/tmp/.uart");

    if (connect(fd_uart, (struct sockaddr *)&sun_uart, sizeof(struct sockaddr_un)) < 0) {
        fprintf(stderr, "[XFT] LPUART_RTOS_Send()\tgateway not found\n");

        return kStatus_Success;
        // return kStatus_Fail;
    }
    printf("[XFT] LPUART_RTOS_Send()\tgateway found and connected\n");

    printf("[XFT] write:");
    for(i = 0; i < 16; i++) {
        write(fd_uart, &buffer_ptr[i], 1);
        printf(" %02x", buffer_ptr[i]);
    }
    printf("\n");

    close(fd_uart);

    printf("[XFT] return value is value type. value: 0x%08x\n", kStatus_Success);
    return kStatus_Success;
}

XFT_EXPOSED_FUNCTION
void set_id(cortex_m_ptr_t node_id) {
    u_int32_t *node_id_ptr = (u_int32_t *)XFT_CONVERT_ADDR_T2H(node_id);
    *node_id_ptr = uuid;
}
