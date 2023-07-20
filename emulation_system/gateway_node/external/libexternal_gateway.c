#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

typedef enum _mcg_oscsel {
    kMCG_OscselOsc, /*!< Selects System Oscillator (OSCCLK) */
    kMCG_OscselRtc, /*!< Selects 32 kHz RTC Oscillator      */
    kMCG_OscselIrc  /*!< Selects 48 MHz IRC Oscillator      */
} mcg_oscsel_t;

typedef enum _mcg_pll_clk_select {
    kMCG_PllClkSelPll0, /*!< PLL0 output clock is selected  */
    kMCG_PllClkSelPll1  /* PLL1 output clock is selected    */
} mcg_pll_clk_select_t;

typedef struct _mcg_pll_config {
    uint8_t enableMode; /*!< Enable mode. OR'ed value of @ref _mcg_pll_enable_mode. */
    uint8_t prdiv;      /*!< Reference divider PRDIV.    */
    uint8_t vdiv;       /*!< VCO divider VDIV.           */
} mcg_pll_config_t;

XFT_EXPOSED_FUNCTION
status_t CLOCK_BootToPeeMode(mcg_oscsel_t oscsel,
                         mcg_pll_clk_select_t pllcs,
                         cortex_m_ptr_t config) {
    return kStatus_Success;
}

XFT_EXPOSED_FUNCTION
void set_ucMaxPriorityValue(cortex_m_ptr_t *ucMaxPriorityValue, cortex_m_ptr_t const reg) {
  uint8_t *ucMaxPriorityValue_ptr = (uint8_t *)XFT_CONVERT_ADDR_T2H(ucMaxPriorityValue);
  *ucMaxPriorityValue_ptr = 0xf0;
}

typedef u_int8_t u8_t;
typedef u_int16_t u16_t;
typedef u_int32_t u32_t;
typedef int8_t s8_t;
typedef int16_t s16_t;
typedef int32_t s32_t;

typedef s8_t err_t;

typedef enum {
  ERR_OK         = 0,
  ERR_MEM        = -1,
  ERR_BUF        = -2,
  ERR_TIMEOUT    = -3,
  ERR_RTE        = -4,
  ERR_INPROGRESS = -5,
  ERR_VAL        = -6,
  ERR_WOULDBLOCK = -7,
  ERR_USE        = -8,
  ERR_ALREADY    = -9,
  ERR_ISCONN     = -10,
  ERR_CONN       = -11,
  ERR_IF         = -12,

  ERR_ABRT       = -13,
  ERR_RST        = -14,
  ERR_CLSD       = -15,
  ERR_ARG        = -16
} err_enum_t;

typedef struct ip4_addr {
  u32_t addr;
} ip4_addr_t;

struct netif {
  cortex_m_ptr_t next;
  ip4_addr_t ip_addr;
  ip4_addr_t netmask;
  ip4_addr_t gw;
  cortex_m_ptr_t input;
  cortex_m_ptr_t output;
  cortex_m_ptr_t linkoutput;
  cortex_m_ptr_t state;
  cortex_m_ptr_t client_data[1];
  u8_t rs_count;
  u16_t mtu;
  u8_t hwaddr_len;
  u8_t hwaddr[6];
  u8_t flags;
  char name[2];
  u8_t num;
};

enum netconn_type {
  NETCONN_INVALID     = 0,
  NETCONN_TCP         = 0x10,
  NETCONN_UDP         = 0x20,
  NETCONN_UDPLITE     = 0x21,
  NETCONN_UDPNOCHKSUM = 0x22,
  NETCONN_RAW         = 0x40
};

enum netconn_state {
  NETCONN_NONE,
  NETCONN_WRITE,
  NETCONN_LISTEN,
  NETCONN_CONNECT,
  NETCONN_CLOSE
};

struct netconn {
  enum netconn_type type;
  enum netconn_state state;
  union {
    cortex_m_ptr_t ip;
    cortex_m_ptr_t tcp;
    cortex_m_ptr_t udp;
    cortex_m_ptr_t raw;
  } pcb;
  err_t last_err;
  cortex_m_ptr_t op_completed;
  cortex_m_ptr_t recvmbox;
  cortex_m_ptr_t acceptmbox;
  int32_t socket;
  int32_t recv_timeout;
  u8_t flags;
  size_t write_offset;
  cortex_m_ptr_t current_msg;
  cortex_m_ptr_t callback;
};

int fd_uart;
struct sockaddr_un sun_uart;
char uart_path[BUFSIZ];

XFT_EXPOSED_FUNCTION
void tcpip_init(cortex_m_ptr_t initfunc, cortex_m_ptr_t arg) {
    printf("[XFT] void tcpip_init(tcpip_init_done_fn initfunc, void *arg)\n");
    printf("[XFT] tcpip_init_done_fn is function pointer. address: 0x%08x\n", initfunc);
    printf("[XFT] void* is pointer. address: 0x%08x\n", arg);
}

XFT_EXPOSED_FUNCTION
cortex_m_ptr_t netif_add(cortex_m_ptr_t netif,
          cortex_m_ptr_t ipaddr, cortex_m_ptr_t netmask, cortex_m_ptr_t gw,
          cortex_m_ptr_t state, cortex_m_ptr_t init, cortex_m_ptr_t input) {
  return 0;
}

XFT_EXPOSED_FUNCTION
void netif_set_default(cortex_m_ptr_t netif) {
}

XFT_EXPOSED_FUNCTION
void netif_set_up(cortex_m_ptr_t netif) {
}

XFT_EXPOSED_FUNCTION
int _printf(cortex_m_ptr_t format, ...) {
    return 1;
}

XFT_EXPOSED_FUNCTION
int UART_RTOS_Init(cortex_m_ptr_t handle, cortex_m_ptr_t t_handle,
    const cortex_m_ptr_t cfg) {
    printf("[XFT] int UART_RTOS_Init(uart_rtos_handle_t *handle, uart_handle_t *t_handle, const uart_rtos_config_t *cfg)\n");
    printf("[XFT] uart_rtos_handle_t* is struct pointer. address: 0x%08x\n", handle);
    printf("[XFT] uart_handle_t* is struct pointer. address: 0x%08x\n", t_handle);
    printf("[XFT] uart_rtos_config_t* is struct pointer. address: 0x%08x\n", t_handle);

    strcpy(uart_path, "/var/tmp/.uart");

    memset(&sun_uart, 0, sizeof(sun_uart));
    sun_uart.sun_family = AF_UNIX;
    strncpy(sun_uart.sun_path, uart_path, sizeof(sun_uart.sun_path) - 1);

    if((fd_uart = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        fprintf(stderr, "socket\n");
        return kStatus_Fail;
    }
    unlink(uart_path);
    if(bind(fd_uart, (struct sockaddr *)&sun_uart, sizeof(struct sockaddr_un)) < 0) {
        fprintf(stderr, "bind\n");
        return kStatus_Fail;
    }
    if(listen(fd_uart, SOMAXCONN) < 0) {
        fprintf(stderr, "listen\n");
        return kStatus_Fail;
    }

    return kStatus_Success;
}

XFT_EXPOSED_FUNCTION
int UART_RTOS_Receive(cortex_m_ptr_t handle,
          cortex_m_ptr_t buffer, u_int32_t length, cortex_m_ptr_t received) {
    int cs, i;

    u_int8_t *recvbuf_ptr = (u_int8_t *)XFT_CONVERT_ADDR_T2H(buffer);
    size_t *num_ptr = (size_t *)XFT_CONVERT_ADDR_T2H(received);
    // u_int8_t buf[16];

    printf("[XFT] Trap int UART_RTOS_Receive(uart_rtos_handle_t *handle, uint8_t *buffer, uint32_t length, size_t *received)\n");
    printf("[XFT] uart_rtos_handle_t* is struct pointer. address: 0x%08x\n", handle);
    printf("[XFT] uint8_t* is pointer. address: 0x%08x\n", buffer);
    printf("[XFT] uint32_t is value type. value: 0x%02x\n", length);
    printf("[XFT] size_t* is pointer. address: 0x%08x\n", received);

    if((cs = accept(fd_uart, NULL, NULL)) == -1) {
        fprintf(stderr, "[XFT] UART_RTOS_Receive()\tsensor_node not found\n");
        printf("[XFT] return value is value type. value: 0x%08x\n", kStatus_Fail);
        return kStatus_Fail;
    }
    printf("[XFT] UART_RTOS_Receive()\tconnection established\n");
    printf("[XFT] read:");
    for(i = 0; i < 16; i++) {
        if(read(cs, recvbuf_ptr, 1) > 0) {
            printf(" %02x", *recvbuf_ptr);
            recvbuf_ptr++;
        } else {
            fprintf(stderr, "connection closed\n");
            break;
        }
    }
    *num_ptr = 1;
    printf("\n");
    close(cs);

    printf("[XFT] return value is value type. value: 0x%08x\n", kStatus_Success);
    return kStatus_Success;
}

typedef struct {
    uint32_t node_id;
    uint32_t illuminance;
    uint32_t temperature;
    uint32_t checksum;
    uint32_t illuminance2;
} gateway_node_data_t;

int append_string(char *buf, size_t *len, char *str)
{
    int i;

    for(i = 0; str[i] != '\0'; i++) {
        buf[i + *len] = str[i];
    }
    *len += i;
    return 0;
}

int append_hex(char *buf, size_t *len, uint8_t num)
{
    uint8_t h, l;

    h = (num & 0xf0) >> 4;
    l = (num & 0x0f) >> 0;
    buf[*len + 0] = (h < 10) ? '0' + h : 'a' + h - 10;
    buf[*len + 1] = (l < 10) ? '0' + l : 'a' + l - 10;
    *len += 2;
    return 0;
}

int append_digit(char *buf, size_t *len, uint32_t num)
{
    int digit;
    size_t append;
    uint32_t cur, mod;

    append = 0;
    for(mod = 1000000000; (num / mod == 0) && (mod > 1); mod /= 10);
    if(mod == 1) {
        buf[*len + append] = '0';
        append++;
    } else {
        for(cur = num; mod > 0; mod /= 10) {
            digit = cur / mod;
            buf[*len + append] = '0' + digit;
            append++;
            cur -= digit * mod;
        }
    }
    *len += append;
    return 0;
}

size_t create_request(char *buf, uint32_t node_id, uint32_t temperature,
                             uint32_t illuminance, uint32_t checksum)
{
    char header[128], body[128];
    int i;
    size_t header_len, body_len;
    uint8_t num;

    body_len = 0;
    if(append_string(body, &body_len, "{\"sender\": \"") < 0) {
        return -1;
    }
    for(i = 0; i < 4; i++) {
        num = (node_id & ((uint64_t)0xff << (i  * 8))) >> (i * 8);
        if(append_hex(body, &body_len, num) < 0) {
            return -1;
        }
        if(i == 3) {
            if(append_string(body, &body_len, "\", ") < 0) {
                return -1;
            }
        } else {
            if(append_string(body, &body_len, ":") < 0) {
                return -1;
            }
        }
    }
    if(append_string(body, &body_len, "\"temperature\": ") < 0) {
        return -1;
    }
    if(append_digit(body, &body_len, temperature) < 0) {
        return -1;
    }
    if(append_string(body, &body_len, ", ") < 0) {
        return -1;
    }
    if(append_string(body, &body_len, "\"illuminance\": ") < 0) {
        return -1;
    }
    if(append_digit(body, &body_len, illuminance) < 0) {
        return -1;
    }
    if(append_string(body, &body_len, ", ") < 0) {
        return -1;
    }
    if(append_string(body, &body_len, "\"checksum\": ") < 0) {
        return -1;
    }
    if(append_digit(body, &body_len, checksum) < 0) {
        return -1;
    }
    if(append_string(body, &body_len, "}") < 0) {
        return -1;
    }

    header_len = 0;
    if(append_string(header, &header_len, "POST / HTTP/1.1\r\n") < 0) {
        return -1;
    }
    if(append_string(header, &header_len,
                     "Content-Type: application/json\r\n") < 0) {
        return -1;
    }
    if(append_string(header, &header_len, "Content-Length: ") < 0) {
        return -1;
    }
    if(append_digit(header, &header_len, body_len) < 0) {
        return -1;
    }
    if(append_string(header, &header_len, "\r\n\r\n") < 0) {
        return -1;
    }

    for(i = 0; i < header_len; i++) {
        buf[i] = header[i];
    }
    for(i = 0; i < body_len; i++) {
        buf[i + header_len] = body[i];
    }
    buf[header_len + body_len] = '\0';
    return header_len + body_len;
}

XFT_EXPOSED_FUNCTION
void jsondata_send(cortex_m_ptr_t data) {
    int s;
    struct sockaddr_in addr;
    gateway_node_data_t *data_ptr = (gateway_node_data_t *)XFT_CONVERT_ADDR_T2H(data);
    char request[256];
    uint16_t len;

    printf("[XFT] Trap void jsondata_send(gateway_node_data_t *data)\n");
    printf("[XFT] gateway_node_data_t* is struct pointer. address: 0x%08x\n", data);

    len = create_request(request, data_ptr->node_id, data_ptr->temperature, data_ptr->illuminance, data_ptr->checksum);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
        fprintf(stderr, "[XFT] create socket error\n");
        return;
    }
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(5000);
    // addr.sin_addr.s_addr = inet_addr("192.168.0.100");
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        fprintf(stderr, "[XFT] http server not found\n");
        return;
    }
    printf("[XFT] http connection established\n");
    if ((send(s, request, len, 0)) > 0) {
        printf("[XFT] post: %s\n", request);
    } else {
        fprintf(stderr, "[XFT] http post failed\n");
        return;
    }
    close(s);

    printf("[XFT] return value is void type.\n");
}
