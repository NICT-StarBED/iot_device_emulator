#ifndef INCLUDE_XFT_H
#define INCLUDE_XFT_H

#define XFT_EXPOSED_FUNCTION         	__attribute__((visibility("default")))
#define XFT_CONVERT_ADDR_T2H(x)     	((cortex_m_ptr_t)(x) + xft_memory_offset)
#define XFT_CONVERT_ADDR_H2T(x)     	((cortex_m_ptr_t)(x) - xft_memory_offset)

typedef u_int32_t cortex_m_ptr_t;
XFT_EXPOSED_FUNCTION u_int8_t *xft_memory_offset;

#endif /* INCLUDE_XFT_H */
