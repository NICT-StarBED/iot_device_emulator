#ifndef INCLUDE_E4ARDUINO_DEFS_H
#define INCLUDE_E4ARDUINO_DEFS_H

#include <sys/types.h>

#define OP32(x) (((*((halfword *)x)) << 16) \
                 | (*(((halfword *)x) + 1)))
#define OP16(x) (*((halfword *)x))

typedef u_int8_t    byte;
typedef int8_t    signed_byte;
typedef u_int16_t   halfword;
typedef int16_t   signed_halfword;
typedef u_int32_t   word;
typedef int32_t   signed_word;
typedef u_int64_t   doubleword;
typedef int64_t   signed_doubleword;
typedef union {
    byte        op8[4];
    halfword    op16[2];
    word        op32[1];
} e4arduino_opcode_t;

/*
 * Copyright (C) 2016-2017 Junya NAKATA and SMK Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

/*
 *   A5.2.1
 *   Shift (immediate), add, subtract, move, and compare
 */

extern void mov_reg_16_et2(void *ctx, halfword rm, halfword rd);
#define E4ARDUINO_MOV_REG_16_ET2_MASK          0xffc0
#define E4ARDUINO_MOV_REG_16_ET2_PATTERN       0x0000
#define PROC_E4ARDUINO_MOV_REG_16_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_MOV_REG_16_ET2_MASK)                  \
                    == E4ARDUINO_MOV_REG_16_ET2_PATTERN)) {                 \
                mov_reg_16_et2(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void lsl_imm_16_et1(void *ctx, halfword imm, halfword rm, halfword rd);
#define E4ARDUINO_LSL_IMM_16_ET1_MASK          0xf800
#define E4ARDUINO_LSL_IMM_16_ET1_PATTERN       0x0000
#define PROC_E4ARDUINO_LSL_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LSL_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_LSL_IMM_16_ET1_PATTERN)) {                 \
                lsl_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x07c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void lsr_imm_16_et1(void *ctx, halfword imm, halfword rm, halfword rd);
#define E4ARDUINO_LSR_IMM_16_ET1_MASK          0xf800
#define E4ARDUINO_LSR_IMM_16_ET1_PATTERN       0x0800
#define PROC_E4ARDUINO_LSR_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LSR_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_LSR_IMM_16_ET1_PATTERN)) {                 \
                lsr_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x07c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void asr_imm_16_et1(void *ctx, halfword imm, halfword rm, halfword rd);
#define E4ARDUINO_ASR_IMM_16_ET1_MASK          0xf800
#define E4ARDUINO_ASR_IMM_16_ET1_PATTERN       0x1000
#define PROC_E4ARDUINO_ASR_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ASR_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_ASR_IMM_16_ET1_PATTERN)) {                 \
                asr_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x07c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void add_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rd);
#define E4ARDUINO_ADD_REG_16_ET1_MASK          0xfe00
#define E4ARDUINO_ADD_REG_16_ET1_PATTERN       0x1800
#define PROC_E4ARDUINO_ADD_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADD_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_ADD_REG_16_ET1_PATTERN)) {                 \
                add_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void sub_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rd);
#define E4ARDUINO_SUB_REG_16_ET1_MASK          0xfe00
#define E4ARDUINO_SUB_REG_16_ET1_PATTERN       0x1a00
#define PROC_E4ARDUINO_SUB_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_SUB_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_SUB_REG_16_ET1_PATTERN)) {                 \
                sub_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void add_imm_16_et1(void *ctx, halfword imm3, halfword rn, halfword rd);
#define E4ARDUINO_ADD_IMM_16_ET1_MASK          0xfe00
#define E4ARDUINO_ADD_IMM_16_ET1_PATTERN       0x1c00
#define PROC_E4ARDUINO_ADD_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADD_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_ADD_IMM_16_ET1_PATTERN)) {                 \
                add_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void sub_imm_16_et1(void *ctx, halfword imm3, halfword rn, halfword rd);
#define E4ARDUINO_SUB_IMM_16_ET1_MASK          0xfe00
#define E4ARDUINO_SUB_IMM_16_ET1_PATTERN       0x1e00
#define PROC_E4ARDUINO_SUB_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_SUB_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_SUB_IMM_16_ET1_PATTERN)) {                 \
                sub_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void mov_imm_16_et1(void *ctx, halfword rd, halfword imm);
#define E4ARDUINO_MOV_IMM_16_ET1_MASK          0xf800
#define E4ARDUINO_MOV_IMM_16_ET1_PATTERN       0x2000
#define PROC_E4ARDUINO_MOV_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_MOV_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_MOV_IMM_16_ET1_PATTERN)) {                 \
                mov_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void cmp_imm_16_et1(void *ctx, halfword rn, halfword imm);
#define E4ARDUINO_CMP_IMM_16_ET1_MASK          0xf800
#define E4ARDUINO_CMP_IMM_16_ET1_PATTERN       0x2800
#define PROC_E4ARDUINO_CMP_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_CMP_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_CMP_IMM_16_ET1_PATTERN)) {                 \
                cmp_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void add_imm_16_et2(void *ctx, halfword rdn, halfword imm);
#define E4ARDUINO_ADD_IMM_16_ET2_MASK          0xf800
#define E4ARDUINO_ADD_IMM_16_ET2_PATTERN       0x3000
#define PROC_E4ARDUINO_ADD_IMM_16_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADD_IMM_16_ET2_MASK)                  \
                    == E4ARDUINO_ADD_IMM_16_ET2_PATTERN)) {                 \
                add_imm_16_et2(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void sub_imm_16_et2(void *ctx, halfword rdn, halfword imm);
#define E4ARDUINO_SUB_IMM_16_ET2_MASK          0xf800
#define E4ARDUINO_SUB_IMM_16_ET2_PATTERN       0x3800
#define PROC_E4ARDUINO_SUB_IMM_16_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_SUB_IMM_16_ET2_MASK)                  \
                    == E4ARDUINO_SUB_IMM_16_ET2_PATTERN)) {                 \
                sub_imm_16_et2(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

/*
 *   A5.2.2
 *   Data processing
 */

extern void and_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_AND_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_AND_REG_16_ET1_PATTERN       0x4000
#define PROC_E4ARDUINO_AND_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_AND_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_AND_REG_16_ET1_PATTERN)) {                 \
                and_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void eor_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_EOR_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_EOR_REG_16_ET1_PATTERN       0x4040
#define PROC_E4ARDUINO_EOR_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_EOR_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_EOR_REG_16_ET1_PATTERN)) {                 \
                eor_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void lsl_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_LSL_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_LSL_REG_16_ET1_PATTERN       0x4080
#define PROC_E4ARDUINO_LSL_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LSL_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_LSL_REG_16_ET1_PATTERN)) {                 \
                lsl_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void lsr_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_LSR_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_LSR_REG_16_ET1_PATTERN       0x40c0
#define PROC_E4ARDUINO_LSR_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LSR_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_LSR_REG_16_ET1_PATTERN)) {                 \
                lsr_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void asr_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_ASR_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_ASR_REG_16_ET1_PATTERN       0x4100
#define PROC_E4ARDUINO_ASR_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ASR_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_ASR_REG_16_ET1_PATTERN)) {                 \
                asr_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void adc_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_ADC_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_ADC_REG_16_ET1_PATTERN       0x4140
#define PROC_E4ARDUINO_ADC_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADC_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_ADC_REG_16_ET1_PATTERN)) {                 \
                adc_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void sbc_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_SBC_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_SBC_REG_16_ET1_PATTERN       0x4180
#define PROC_E4ARDUINO_SBC_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_SBC_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_SBC_REG_16_ET1_PATTERN)) {                 \
                sbc_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void ror_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_ROR_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_ROR_REG_16_ET1_PATTERN       0x41c0
#define PROC_E4ARDUINO_ROR_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ROR_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_ROR_REG_16_ET1_PATTERN)) {                 \
                ror_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void tst_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_TST_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_TST_REG_16_ET1_PATTERN       0x4200
#define PROC_E4ARDUINO_TST_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_TST_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_TST_REG_16_ET1_PATTERN)) {                 \
                tst_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void rsb_imm_16_et1(void *ctx, halfword rn, halfword rd);
#define E4ARDUINO_RSB_IMM_16_ET1_MASK          0xffc0
#define E4ARDUINO_RSB_IMM_16_ET1_PATTERN       0x4240
#define PROC_E4ARDUINO_RSB_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_RSB_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_RSB_IMM_16_ET1_PATTERN)) {                 \
                rsb_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void cmp_reg_16_et1(void *ctx, halfword rm, halfword rn);
#define E4ARDUINO_CMP_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_CMP_REG_16_ET1_PATTERN       0x4280
#define PROC_E4ARDUINO_CMP_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_CMP_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_CMP_REG_16_ET1_PATTERN)) {                 \
                cmp_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void cmn_reg_16_et1(void *ctx, halfword rm, halfword rn);
#define E4ARDUINO_CMN_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_CMN_REG_16_ET1_PATTERN       0x42c0
#define PROC_E4ARDUINO_CMN_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_CMN_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_CMN_REG_16_ET1_PATTERN)) {                 \
                cmn_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void orr_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_ORR_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_ORR_REG_16_ET1_PATTERN       0x4300
#define PROC_E4ARDUINO_ORR_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ORR_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_ORR_REG_16_ET1_PATTERN)) {                 \
                orr_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void mul_16_et1(void *ctx, halfword rn, halfword rdm);
#define E4ARDUINO_MUL_16_ET1_MASK              0xffc0
#define E4ARDUINO_MUL_16_ET1_PATTERN           0x4340
#define PROC_E4ARDUINO_MUL_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_MUL_16_ET1_MASK)                      \
                    == E4ARDUINO_MUL_16_ET1_PATTERN)) {                     \
                mul_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void bic_reg_16_et1(void *ctx, halfword rm, halfword rdn);
#define E4ARDUINO_BIC_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_BIC_REG_16_ET1_PATTERN       0x4380
#define PROC_E4ARDUINO_BIC_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_BIC_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_BIC_REG_16_ET1_PATTERN)) {                 \
                bic_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void mvn_reg_16_et1(void *ctx, halfword rm, halfword rd);
#define E4ARDUINO_MVN_REG_16_ET1_MASK          0xffc0
#define E4ARDUINO_MVN_REG_16_ET1_PATTERN       0x43c0
#define PROC_E4ARDUINO_MVN_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_MVN_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_MVN_REG_16_ET1_PATTERN)) {                 \
                mvn_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

/*
 *   A5.2.3
 *   Special data instructions and branch and exchange
 */

extern void add_reg_16_et2(void *ctx, halfword dn, halfword rm, halfword rdn);
#define E4ARDUINO_ADD_REG_16_ET2_MASK          0xff00
#define E4ARDUINO_ADD_REG_16_ET2_PATTERN       0x4400
#define PROC_E4ARDUINO_ADD_REG_16_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADD_REG_16_ET2_MASK)                  \
                    == E4ARDUINO_ADD_REG_16_ET2_PATTERN)) {                 \
                add_reg_16_et2(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0080) >> 7),                             \
                    ((OP16(op) & 0x0078) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void add_sp_reg_16_et1(void *ctx, halfword dm, halfword rdm);
#define E4ARDUINO_ADD_SP_REG_16_ET1_MASK       0xff78
#define E4ARDUINO_ADD_SP_REG_16_ET1_PATTERN    0x4468
#define PROC_E4ARDUINO_ADD_SP_REG_16_ET1(ctx, op)                           \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADD_SP_REG_16_ET1_MASK)               \
                    == E4ARDUINO_ADD_SP_REG_16_ET1_PATTERN)) {              \
                add_sp_reg_16_et1(                                          \
                    ctx,                                                    \
                    ((OP16(op) & 0x0080) >> 7),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void add_sp_reg_16_et2(void *ctx, halfword rm);
#define E4ARDUINO_ADD_SP_REG_16_ET2_MASK       0xff87
#define E4ARDUINO_ADD_SP_REG_16_ET2_PATTERN    0x4485
#define PROC_E4ARDUINO_ADD_SP_REG_16_ET2(ctx, op)                           \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADD_SP_REG_16_ET2_MASK)               \
                    == E4ARDUINO_ADD_SP_REG_16_ET2_PATTERN)) {              \
                add_sp_reg_16_et2(                                          \
                    ctx,                                                    \
                    ((OP16(op) & 0x0078) >> 3));                            \
            }                                                               \
        } while(0)

extern void cmp_reg_16_et2(void *ctx, halfword n, halfword rm, halfword rn);
#define E4ARDUINO_CMP_REG_16_ET2_MASK          0xff00
#define E4ARDUINO_CMP_REG_16_ET2_PATTERN       0x4500
#define PROC_E4ARDUINO_CMP_REG_16_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_CMP_REG_16_ET2_MASK)                  \
                    == E4ARDUINO_CMP_REG_16_ET2_PATTERN)) {                 \
                cmp_reg_16_et2(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0080) >> 7),                             \
                    ((OP16(op) & 0x0078) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void mov_reg_16_et1(void *ctx, halfword d, halfword rm, halfword rd);
#define E4ARDUINO_MOV_REG_16_ET1_MASK          0xff00
#define E4ARDUINO_MOV_REG_16_ET1_PATTERN       0x4600
#define PROC_E4ARDUINO_MOV_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_MOV_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_MOV_REG_16_ET1_PATTERN)) {                 \
                mov_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0080) >> 7),                             \
                    ((OP16(op) & 0x0078) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void bx_16_et1(void *ctx, halfword rm);
#define E4ARDUINO_BX_16_ET1_MASK               0xff87
#define E4ARDUINO_BX_16_ET1_PATTERN            0x4700
#define PROC_E4ARDUINO_BX_16_ET1(ctx, op)                                   \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_BX_16_ET1_MASK)                       \
                    == E4ARDUINO_BX_16_ET1_PATTERN)) {                      \
                bx_16_et1(                                                  \
                    ctx,                                                    \
                    ((OP16(op) & 0x0078) >> 3));                            \
            }                                                               \
        } while(0)

extern void blx_reg_16_et1(void *ctx, halfword rm);
#define E4ARDUINO_BLX_REG_16_ET1_MASK          0xff87
#define E4ARDUINO_BLX_REG_16_ET1_PATTERN       0x4780
#define PROC_E4ARDUINO_BLX_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_BLX_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_BLX_REG_16_ET1_PATTERN)) {                 \
                blx_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0078) >> 3));                            \
            }                                                               \
        } while(0)

/*
 *   A5.2.4
 *   Load/store single data item
 */

extern void ldr_lit_16_et1(void *ctx, halfword rt, halfword imm);
#define E4ARDUINO_LDR_LIT_16_ET1_MASK          0xf800
#define E4ARDUINO_LDR_LIT_16_ET1_PATTERN       0x4800
#define PROC_E4ARDUINO_LDR_LIT_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDR_LIT_16_ET1_MASK)                  \
                    == E4ARDUINO_LDR_LIT_16_ET1_PATTERN)) {                 \
                ldr_lit_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void str_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
#define E4ARDUINO_STR_REG_16_ET1_MASK          0xfe00
#define E4ARDUINO_STR_REG_16_ET1_PATTERN       0x5000
#define PROC_E4ARDUINO_STR_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_STR_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_STR_REG_16_ET1_PATTERN)) {                 \
                str_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void strh_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
#define E4ARDUINO_STRH_REG_16_ET1_MASK         0xfe00
#define E4ARDUINO_STRH_REG_16_ET1_PATTERN      0x5200
#define PROC_E4ARDUINO_STRH_REG_16_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_STRH_REG_16_ET1_MASK)                 \
                    == E4ARDUINO_STRH_REG_16_ET1_PATTERN)) {                \
                strh_reg_16_et1(                                            \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void strb_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
#define E4ARDUINO_STRB_REG_16_ET1_MASK         0xfe00
#define E4ARDUINO_STRB_REG_16_ET1_PATTERN      0x5400
#define PROC_E4ARDUINO_STRB_REG_16_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_STRB_REG_16_ET1_MASK)                 \
                    == E4ARDUINO_STRB_REG_16_ET1_PATTERN)) {                \
                strb_reg_16_et1(                                            \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldrsb_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
#define E4ARDUINO_LDRSB_REG_16_ET1_MASK        0xfe00
#define E4ARDUINO_LDRSB_REG_16_ET1_PATTERN     0x5600
#define PROC_E4ARDUINO_LDRSB_REG_16_ET1(ctx, op)                            \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDRSB_REG_16_ET1_MASK)                \
                    == E4ARDUINO_LDRSB_REG_16_ET1_PATTERN)) {               \
                ldrsb_reg_16_et1(                                           \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldr_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
#define E4ARDUINO_LDR_REG_16_ET1_MASK          0xfe00
#define E4ARDUINO_LDR_REG_16_ET1_PATTERN       0x5800
#define PROC_E4ARDUINO_LDR_REG_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDR_REG_16_ET1_MASK)                  \
                    == E4ARDUINO_LDR_REG_16_ET1_PATTERN)) {                 \
                ldr_reg_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldrh_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
#define E4ARDUINO_LDRH_REG_16_ET1_MASK         0xfe00
#define E4ARDUINO_LDRH_REG_16_ET1_PATTERN      0x5a00
#define PROC_E4ARDUINO_LDRH_REG_16_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDRH_REG_16_ET1_MASK)                 \
                    == E4ARDUINO_LDRH_REG_16_ET1_PATTERN)) {                \
                ldrh_reg_16_et1(                                            \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldrb_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
#define E4ARDUINO_LDRB_REG_16_ET1_MASK         0xfe00
#define E4ARDUINO_LDRB_REG_16_ET1_PATTERN      0x5c00
#define PROC_E4ARDUINO_LDRB_REG_16_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDRB_REG_16_ET1_MASK)                 \
                    == E4ARDUINO_LDRB_REG_16_ET1_PATTERN)) {                \
                ldrb_reg_16_et1(                                            \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldrsh_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
#define E4ARDUINO_LDRSH_REG_16_ET1_MASK        0xfe00
#define E4ARDUINO_LDRSH_REG_16_ET1_PATTERN     0x5e00
#define PROC_E4ARDUINO_LDRSH_REG_16_ET1(ctx, op)                            \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDRSH_REG_16_ET1_MASK)                \
                    == E4ARDUINO_LDRSH_REG_16_ET1_PATTERN)) {               \
                ldrsh_reg_16_et1(                                           \
                    ctx,                                                    \
                    ((OP16(op) & 0x01c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void str_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
#define E4ARDUINO_STR_IMM_16_ET1_MASK          0xf800
#define E4ARDUINO_STR_IMM_16_ET1_PATTERN       0x6000
#define PROC_E4ARDUINO_STR_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_STR_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_STR_IMM_16_ET1_PATTERN)) {                 \
                str_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x07c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldr_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
#define E4ARDUINO_LDR_IMM_16_ET1_MASK          0xf800
#define E4ARDUINO_LDR_IMM_16_ET1_PATTERN       0x6800
#define PROC_E4ARDUINO_LDR_IMM_16_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDR_IMM_16_ET1_MASK)                  \
                    == E4ARDUINO_LDR_IMM_16_ET1_PATTERN)) {                 \
                ldr_imm_16_et1(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x07c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void strb_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
#define E4ARDUINO_STRB_IMM_16_ET1_MASK         0xf800
#define E4ARDUINO_STRB_IMM_16_ET1_PATTERN      0x7000
#define PROC_E4ARDUINO_STRB_IMM_16_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_STRB_IMM_16_ET1_MASK)                 \
                    == E4ARDUINO_STRB_IMM_16_ET1_PATTERN)) {                \
                strb_imm_16_et1(                                            \
                    ctx,                                                    \
                    ((OP16(op) & 0x07c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldrb_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
#define E4ARDUINO_LDRB_IMM_16_ET1_MASK         0xf800
#define E4ARDUINO_LDRB_IMM_16_ET1_PATTERN      0x7800
#define PROC_E4ARDUINO_LDRB_IMM_16_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDRB_IMM_16_ET1_MASK)                 \
                    == E4ARDUINO_LDRB_IMM_16_ET1_PATTERN)) {                \
                ldrb_imm_16_et1(                                            \
                    ctx,                                                    \
                    ((OP16(op) & 0x07c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void strh_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
#define E4ARDUINO_STRH_IMM_16_ET1_MASK         0xf800
#define E4ARDUINO_STRH_IMM_16_ET1_PATTERN      0x8000
#define PROC_E4ARDUINO_STRH_IMM_16_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_STRH_IMM_16_ET1_MASK)                 \
                    == E4ARDUINO_STRH_IMM_16_ET1_PATTERN)) {                \
                strh_imm_16_et1(                                            \
                    ctx,                                                    \
                    ((OP16(op) & 0x07c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldrh_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
#define E4ARDUINO_LDRH_IMM_16_ET1_MASK         0xf800
#define E4ARDUINO_LDRH_IMM_16_ET1_PATTERN      0x8800
#define PROC_E4ARDUINO_LDRH_IMM_16_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDRH_IMM_16_ET1_MASK)                 \
                    == E4ARDUINO_LDRH_IMM_16_ET1_PATTERN)) {                \
                ldrh_imm_16_et1(                                            \
                    ctx,                                                    \
                    ((OP16(op) & 0x07c0) >> 6),                             \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void str_imm_16_et2(void *ctx, halfword rt, halfword imm);
#define E4ARDUINO_STR_IMM_16_ET2_MASK          0xf800
#define E4ARDUINO_STR_IMM_16_ET2_PATTERN       0x9000
#define PROC_E4ARDUINO_STR_IMM_16_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_STR_IMM_16_ET2_MASK)                  \
                    == E4ARDUINO_STR_IMM_16_ET2_PATTERN)) {                 \
                str_imm_16_et2(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldr_imm_16_et2(void *ctx, halfword rt, halfword imm);
#define E4ARDUINO_LDR_IMM_16_ET2_MASK          0xf800
#define E4ARDUINO_LDR_IMM_16_ET2_PATTERN       0x9800
#define PROC_E4ARDUINO_LDR_IMM_16_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDR_IMM_16_ET2_MASK)                  \
                    == E4ARDUINO_LDR_IMM_16_ET2_PATTERN)) {                 \
                ldr_imm_16_et2(                                             \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

/*
 *   A5.2.5
 *   Miscellaneous 16-bit instructions
 */

extern void adr_16_et1(void *ctx, halfword rd, halfword imm);
#define E4ARDUINO_ADR_16_ET1_MASK              0xf800
#define E4ARDUINO_ADR_16_ET1_PATTERN           0xa000
#define PROC_E4ARDUINO_ADR_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADR_16_ET1_MASK)                      \
                    == E4ARDUINO_ADR_16_ET1_PATTERN)) {                     \
                adr_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void add_sp_imm_16_et1(void *ctx, halfword rd, halfword imm);
#define E4ARDUINO_ADD_SP_IMM_16_ET1_MASK       0xf800
#define E4ARDUINO_ADD_SP_IMM_16_ET1_PATTERN    0xa800
#define PROC_E4ARDUINO_ADD_SP_IMM_16_ET1(ctx, op)                           \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADD_SP_IMM_16_ET1_MASK)               \
                    == E4ARDUINO_ADD_SP_IMM_16_ET1_PATTERN)) {              \
                add_sp_imm_16_et1(                                          \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void cps_16_et1(void *ctx, halfword im, halfword i, halfword f);
#define E4ARDUINO_CPS_16_ET1_MASK              0xffec
#define E4ARDUINO_CPS_16_ET1_PATTERN           0xb660
#define PROC_E4ARDUINO_CPS_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_CPS_16_ET1_MASK)                      \
                    == E4ARDUINO_CPS_16_ET1_PATTERN)) {                     \
                cps_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x0010) >> 4),                             \
                    ((OP16(op) & 0x0002) >> 1),                             \
                    ((OP16(op) & 0x0001) >> 0));                            \
            }                                                               \
        } while(0)

extern void add_sp_imm_16_et2(void *ctx, halfword imm);
#define E4ARDUINO_ADD_SP_IMM_16_ET2_MASK       0xff80
#define E4ARDUINO_ADD_SP_IMM_16_ET2_PATTERN    0xb000
#define PROC_E4ARDUINO_ADD_SP_IMM_16_ET2(ctx, op)                           \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_ADD_SP_IMM_16_ET2_MASK)               \
                    == E4ARDUINO_ADD_SP_IMM_16_ET2_PATTERN)) {              \
                add_sp_imm_16_et2(                                          \
                    ctx,                                                    \
                    ((OP16(op) & 0x007f) >> 0));                            \
            }                                                               \
        } while(0)

extern void sub_sp_imm_16_et1(void *ctx, halfword imm);
#define E4ARDUINO_SUB_SP_IMM_16_ET1_MASK       0xff80
#define E4ARDUINO_SUB_SP_IMM_16_ET1_PATTERN    0xb080
#define PROC_E4ARDUINO_SUB_SP_IMM_16_ET1(ctx, op)                           \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_SUB_SP_IMM_16_ET1_MASK)               \
                    == E4ARDUINO_SUB_SP_IMM_16_ET1_PATTERN)) {              \
                sub_sp_imm_16_et1(                                          \
                    ctx,                                                    \
                    ((OP16(op) & 0x007f) >> 0));                            \
            }                                                               \
        } while(0)

extern void cbz_16_et1(void *ctx, halfword i, halfword imm, halfword rn);
#define E4ARDUINO_CBZ_16_ET1_MASK              0xfd00
#define E4ARDUINO_CBZ_16_ET1_PATTERN           0xb100
#define PROC_E4ARDUINO_CBZ_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_CBZ_16_ET1_MASK)                      \
                    == E4ARDUINO_CBZ_16_ET1_PATTERN)) {                     \
                cbz_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x0200) >> 9),                             \
                    ((OP16(op) & 0x00f8) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void cbnz_16_et1(void *ctx, halfword i, halfword imm, halfword rn);
#define E4ARDUINO_CBNZ_16_ET1_MASK             0xfd00
#define E4ARDUINO_CBNZ_16_ET1_PATTERN          0xb900
#define PROC_E4ARDUINO_CBNZ_16_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_CBNZ_16_ET1_MASK)                     \
                    == E4ARDUINO_CBNZ_16_ET1_PATTERN)) {                    \
                cbnz_16_et1(                                                \
                    ctx,                                                    \
                    ((OP16(op) & 0x0200) >> 9),                             \
                    ((OP16(op) & 0x00f8) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void sxth_16_et1(void *ctx, halfword rm, halfword rd);
#define E4ARDUINO_SXTH_16_ET1_MASK             0xffc0
#define E4ARDUINO_SXTH_16_ET1_PATTERN          0xb200
#define PROC_E4ARDUINO_SXTH_16_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_SXTH_16_ET1_MASK)                     \
                    == E4ARDUINO_SXTH_16_ET1_PATTERN)) {                    \
                sxth_16_et1(                                                \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void sxtb_16_et1(void *ctx, halfword rm, halfword rd);
#define E4ARDUINO_SXTB_16_ET1_MASK             0xffc0
#define E4ARDUINO_SXTB_16_ET1_PATTERN          0xb240
#define PROC_E4ARDUINO_SXTB_16_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_SXTB_16_ET1_MASK)                     \
                    == E4ARDUINO_SXTB_16_ET1_PATTERN)) {                    \
                sxtb_16_et1(                                                \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void uxth_16_et1(void *ctx, halfword rm, halfword rd);
#define E4ARDUINO_UXTH_16_ET1_MASK             0xffc0
#define E4ARDUINO_UXTH_16_ET1_PATTERN          0xb280
#define PROC_E4ARDUINO_UXTH_16_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_UXTH_16_ET1_MASK)                     \
                    == E4ARDUINO_UXTH_16_ET1_PATTERN)) {                    \
                uxth_16_et1(                                                \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void uxtb_16_et1(void *ctx, halfword rm, halfword rd);
#define E4ARDUINO_UXTB_16_ET1_MASK             0xffc0
#define E4ARDUINO_UXTB_16_ET1_PATTERN          0xb2c0
#define PROC_E4ARDUINO_UXTB_16_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_UXTB_16_ET1_MASK)                     \
                    == E4ARDUINO_UXTB_16_ET1_PATTERN)) {                    \
                uxtb_16_et1(                                                \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void push_16_et1(void *ctx, halfword m, halfword rl);
#define E4ARDUINO_PUSH_16_ET1_MASK             0xfe00
#define E4ARDUINO_PUSH_16_ET1_PATTERN          0xb400
#define PROC_E4ARDUINO_PUSH_16_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_PUSH_16_ET1_MASK)                     \
                    == E4ARDUINO_PUSH_16_ET1_PATTERN)) {                    \
                push_16_et1(                                                \
                    ctx,                                                    \
                    ((OP16(op) & 0x0100) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void rev_16_et1(void *ctx, halfword rm, halfword rd);
#define E4ARDUINO_REV_16_ET1_MASK              0xffc0
#define E4ARDUINO_REV_16_ET1_PATTERN           0xba00
#define PROC_E4ARDUINO_REV_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_REV_16_ET1_MASK)                      \
                    == E4ARDUINO_REV_16_ET1_PATTERN)) {                     \
                rev_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void rev16_16_et1(void *ctx, halfword rm, halfword rd);
#define E4ARDUINO_REV16_16_ET1_MASK            0xffc0
#define E4ARDUINO_REV16_16_ET1_PATTERN         0xba40
#define PROC_E4ARDUINO_REV16_16_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_REV16_16_ET1_MASK)                    \
                    == E4ARDUINO_REV16_16_ET1_PATTERN)) {                   \
                rev16_16_et1(                                               \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void revsh_16_et1(void *ctx, halfword rm, halfword rd);
#define E4ARDUINO_REVSH_16_ET1_MASK            0xffc0
#define E4ARDUINO_REVSH_16_ET1_PATTERN         0xbac0
#define PROC_E4ARDUINO_REVSH_16_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_REVSH_16_ET1_MASK)                    \
                    == E4ARDUINO_REVSH_16_ET1_PATTERN)) {                   \
                revsh_16_et1(                                               \
                    ctx,                                                    \
                    ((OP16(op) & 0x0038) >> 3),                             \
                    ((OP16(op) & 0x0007) >> 0));                            \
            }                                                               \
        } while(0)

extern void pop_16_et1(void *ctx, halfword p, halfword rl);
#define E4ARDUINO_POP_16_ET1_MASK              0xfe00
#define E4ARDUINO_POP_16_ET1_PATTERN           0xbc00
#define PROC_E4ARDUINO_POP_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_POP_16_ET1_MASK)                      \
                    == E4ARDUINO_POP_16_ET1_PATTERN)) {                     \
                pop_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x0100) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void bkpt_16_et1(void *ctx, halfword imm);
#define E4ARDUINO_BKPT_16_ET1_MASK             0xff00
#define E4ARDUINO_BKPT_16_ET1_PATTERN          0xbe00
#define PROC_E4ARDUINO_BKPT_16_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_BKPT_16_ET1_MASK)                     \
                    == E4ARDUINO_BKPT_16_ET1_PATTERN)) {                    \
                bkpt_16_et1(                                                \
                    ctx,                                                    \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void it_16_et1(void *ctx, halfword fc, halfword mask);
#define E4ARDUINO_IT_16_ET1_MASK               0xff00
#define E4ARDUINO_IT_16_ET1_PATTERN            0xbf00
#define PROC_E4ARDUINO_IT_16_ET1(ctx, op)                                   \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_IT_16_ET1_MASK)                       \
                    == E4ARDUINO_IT_16_ET1_PATTERN)) {                      \
                it_16_et1(                                                  \
                    ctx,                                                    \
                    ((OP16(op) & 0x00f0) >> 4),                             \
                    ((OP16(op) & 0x000f) >> 0));                            \
            }                                                               \
        } while(0)

extern void nop_16_et1(void *ctx);
#define E4ARDUINO_NOP_16_ET1_MASK              0xffff
#define E4ARDUINO_NOP_16_ET1_PATTERN           0xbf00
#define PROC_E4ARDUINO_NOP_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_NOP_16_ET1_MASK)                      \
                    == E4ARDUINO_NOP_16_ET1_PATTERN)) {                     \
                nop_16_et1(                                                 \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void yield_16_et1(void *ctx);
#define E4ARDUINO_YIELD_16_ET1_MASK            0xffff
#define E4ARDUINO_YIELD_16_ET1_PATTERN         0xbf10
#define PROC_E4ARDUINO_YIELD_16_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_YIELD_16_ET1_MASK)                    \
                    == E4ARDUINO_YIELD_16_ET1_PATTERN)) {                   \
                yield_16_et1(                                               \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void wfe_16_et1(void *ctx);
#define E4ARDUINO_WFE_16_ET1_MASK              0xffff
#define E4ARDUINO_WFE_16_ET1_PATTERN           0xbf20
#define PROC_E4ARDUINO_WFE_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_WFE_16_ET1_MASK)                      \
                    == E4ARDUINO_WFE_16_ET1_PATTERN)) {                     \
                wfe_16_et1(                                                 \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void wfi_16_et1(void *ctx);
#define E4ARDUINO_WFI_16_ET1_MASK              0xffff
#define E4ARDUINO_WFI_16_ET1_PATTERN           0xbf30
#define PROC_E4ARDUINO_WFI_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_WFI_16_ET1_MASK)                      \
                    == E4ARDUINO_WFI_16_ET1_PATTERN)) {                     \
                wfi_16_et1(                                                 \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void sev_16_et1(void *ctx);
#define E4ARDUINO_SEV_16_ET1_MASK              0xffff
#define E4ARDUINO_SEV_16_ET1_PATTERN           0xbf40
#define PROC_E4ARDUINO_SEV_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_SEV_16_ET1_MASK)                      \
                    == E4ARDUINO_SEV_16_ET1_PATTERN)) {                     \
                sev_16_et1(                                                 \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void stm_16_et1(void *ctx, halfword rn, halfword rl);
#define E4ARDUINO_STM_16_ET1_MASK              0xf800
#define E4ARDUINO_STM_16_ET1_PATTERN           0xc000
#define PROC_E4ARDUINO_STM_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_STM_16_ET1_MASK)                      \
                    == E4ARDUINO_STM_16_ET1_PATTERN)) {                     \
                stm_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void ldm_16_et1(void *ctx, halfword rn, halfword rl);
#define E4ARDUINO_LDM_16_ET1_MASK              0xf800
#define E4ARDUINO_LDM_16_ET1_PATTERN           0xc800
#define PROC_E4ARDUINO_LDM_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_LDM_16_ET1_MASK)                      \
                    == E4ARDUINO_LDM_16_ET1_PATTERN)) {                     \
                ldm_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x0700) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

/*
 *   A5.2.6
 *   Conditional branch, and supervisor call
 */

extern void b_16_et1(void *ctx, halfword cond, halfword imm);
#define E4ARDUINO_B_16_ET1_MASK                0xf000
#define E4ARDUINO_B_16_ET1_PATTERN             0xd000
#define PROC_E4ARDUINO_B_16_ET1(ctx, op)                                    \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_B_16_ET1_MASK)                        \
                    == E4ARDUINO_B_16_ET1_PATTERN)) {                       \
                b_16_et1(                                                   \
                    ctx,                                                    \
                    ((OP16(op) & 0x0f00) >> 8),                             \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void udf_16_et1(void *ctx, halfword imm);
#define E4ARDUINO_UDF_16_ET1_MASK              0xff00
#define E4ARDUINO_UDF_16_ET1_PATTERN           0xde00
#define PROC_E4ARDUINO_UDF_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_UDF_16_ET1_MASK)                      \
                    == E4ARDUINO_UDF_16_ET1_PATTERN)) {                     \
                udf_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void svc_16_et1(void *ctx, halfword imm);
#define E4ARDUINO_SVC_16_ET1_MASK              0xff00
#define E4ARDUINO_SVC_16_ET1_PATTERN           0xdf00
#define PROC_E4ARDUINO_SVC_16_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_SVC_16_ET1_MASK)                      \
                    == E4ARDUINO_SVC_16_ET1_PATTERN)) {                     \
                svc_16_et1(                                                 \
                    ctx,                                                    \
                    ((OP16(op) & 0x00ff) >> 0));                            \
            }                                                               \
        } while(0)

extern void b_16_et2(void *ctx, halfword imm);
#define E4ARDUINO_B_16_ET2_MASK                0xf800
#define E4ARDUINO_B_16_ET2_PATTERN             0xe000
#define PROC_E4ARDUINO_B_16_ET2(ctx, op)                                    \
        do {                                                                \
            if(((OP16(op) & E4ARDUINO_B_16_ET2_MASK)                        \
                    == E4ARDUINO_B_16_ET2_PATTERN)) {                       \
                b_16_et2(                                                   \
                    ctx,                                                    \
                    ((OP16(op) & 0x07ff) >> 0));                            \
            }                                                               \
        } while(0)

/*
 *   A5.3.5
 *   Load Multiple and Store Multiple
 */

extern void stm_32_et2(void *ctx, word w, word rn, word m, word rl);
#define E4ARDUINO_STM_32_ET2_MASK              0xffd0a000
#define E4ARDUINO_STM_32_ET2_PATTERN           0xe8800000
#define PROC_E4ARDUINO_STM_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STM_32_ET2_MASK)                      \
                    == E4ARDUINO_STM_32_ET2_PATTERN)) {                     \
                stm_32_et2(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00004000) >> 14),                        \
                    ((OP32(op) & 0x00001fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldm_32_et2(void *ctx, word w, word rn, word p, word m, word rl);
#define E4ARDUINO_LDM_32_ET2_MASK              0xffd02000
#define E4ARDUINO_LDM_32_ET2_PATTERN           0xe8900000
#define PROC_E4ARDUINO_LDM_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDM_32_ET2_MASK)                      \
                    == E4ARDUINO_LDM_32_ET2_PATTERN)) {                     \
                ldm_32_et2(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00008000) >> 15),                        \
                    ((OP32(op) & 0x00004000) >> 14),                        \
                    ((OP32(op) & 0x00001fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void pop_32_et2(void *ctx, word p, word m, word rl);
#define E4ARDUINO_POP_32_ET2_MASK              0xffff2000
#define E4ARDUINO_POP_32_ET2_PATTERN           0xe8bd0000
#define PROC_E4ARDUINO_POP_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_POP_32_ET2_MASK)                      \
                    == E4ARDUINO_POP_32_ET2_PATTERN)) {                     \
                pop_32_et2(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x00008000) >> 15),                        \
                    ((OP32(op) & 0x00004000) >> 14),                        \
                    ((OP32(op) & 0x00001fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void stmdb_32_et1(void *ctx, word w, word rn, word m, word rl);
#define E4ARDUINO_STMDB_32_ET1_MASK            0xffd0a000
#define E4ARDUINO_STMDB_32_ET1_PATTERN         0xe9000000
#define PROC_E4ARDUINO_STMDB_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STMDB_32_ET1_MASK)                    \
                    == E4ARDUINO_STMDB_32_ET1_PATTERN)) {                   \
                stmdb_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00004000) >> 14),                        \
                    ((OP32(op) & 0x00001fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void push_32_et2(void *ctx, word m, word rl);
#define E4ARDUINO_PUSH_32_ET2_MASK             0xffffa000
#define E4ARDUINO_PUSH_32_ET2_PATTERN          0xe92d0000
#define PROC_E4ARDUINO_PUSH_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PUSH_32_ET2_MASK)                     \
                    == E4ARDUINO_PUSH_32_ET2_PATTERN)) {                    \
                push_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00004000) >> 14),                        \
                    ((OP32(op) & 0x00001fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldmdb_32_et1(void *ctx, word w, word rn, word p, word m, word rl);
#define E4ARDUINO_LDMDB_32_ET1_MASK            0xffd02000
#define E4ARDUINO_LDMDB_32_ET1_PATTERN         0xe9100000
#define PROC_E4ARDUINO_LDMDB_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDMDB_32_ET1_MASK)                    \
                    == E4ARDUINO_LDMDB_32_ET1_PATTERN)) {                   \
                ldmdb_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00008000) >> 15),                        \
                    ((OP32(op) & 0x00004000) >> 14),                        \
                    ((OP32(op) & 0x00001fff) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.6
 *   Load/store dual or exclusive, table branch
 */

extern void strex_32_et1(void *ctx, word rn, word rt, word rd, word imm);
#define E4ARDUINO_STREX_32_ET1_MASK            0xfff00000
#define E4ARDUINO_STREX_32_ET1_PATTERN         0xe8400000
#define PROC_E4ARDUINO_STREX_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STREX_32_ET1_MASK)                    \
                    == E4ARDUINO_STREX_32_ET1_PATTERN)) {                   \
                strex_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrex_32_et1(void *ctx, word rn, word rt, word imm);
#define E4ARDUINO_LDREX_32_ET1_MASK            0xfff00f00
#define E4ARDUINO_LDREX_32_ET1_PATTERN         0xe8500f00
#define PROC_E4ARDUINO_LDREX_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDREX_32_ET1_MASK)                    \
                    == E4ARDUINO_LDREX_32_ET1_PATTERN)) {                   \
                ldrex_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void strd_imm_32_et1(void *ctx, word p, word u, word w, word rn, word rt, word rt2, word imm);
#define E4ARDUINO_STRD_IMM_32_ET1_MASK         0xfe500000
#define E4ARDUINO_STRD_IMM_32_ET1_PATTERN      0xe8400000
#define PROC_E4ARDUINO_STRD_IMM_32_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRD_IMM_32_ET1_MASK)                 \
                    == E4ARDUINO_STRD_IMM_32_ET1_PATTERN)) {                \
                strd_imm_32_et1(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrd_imm_32_et1(void *ctx, word p, word u, word w, word rn, word rt, word rt2, word imm);
#define E4ARDUINO_LDRD_IMM_32_ET1_MASK         0xfe500000
#define E4ARDUINO_LDRD_IMM_32_ET1_PATTERN      0xe8500000
#define PROC_E4ARDUINO_LDRD_IMM_32_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRD_IMM_32_ET1_MASK)                 \
                    == E4ARDUINO_LDRD_IMM_32_ET1_PATTERN)) {                \
                ldrd_imm_32_et1(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrd_lit_32_et1(void *ctx, word p, word u, word w, word rt, word rt2, word imm);
#define E4ARDUINO_LDRD_LIT_32_ET1_MASK         0xfe5f0000
#define E4ARDUINO_LDRD_LIT_32_ET1_PATTERN      0xe85f0000
#define PROC_E4ARDUINO_LDRD_LIT_32_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRD_LIT_32_ET1_MASK)                 \
                    == E4ARDUINO_LDRD_LIT_32_ET1_PATTERN)) {                \
                ldrd_lit_32_et1(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void strexb_32_et1(void *ctx, word rn, word rt, word rd);
#define E4ARDUINO_STREXB_32_ET1_MASK           0xfff00ff0
#define E4ARDUINO_STREXB_32_ET1_PATTERN        0xe8c00f40
#define PROC_E4ARDUINO_STREXB_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STREXB_32_ET1_MASK)                   \
                    == E4ARDUINO_STREXB_32_ET1_PATTERN)) {                  \
                strexb_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void strexh_32_et1(void *ctx, word rn, word rt, word rd);
#define E4ARDUINO_STREXH_32_ET1_MASK           0xfff00ff0
#define E4ARDUINO_STREXH_32_ET1_PATTERN        0xe8c00f50
#define PROC_E4ARDUINO_STREXH_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STREXH_32_ET1_MASK)                   \
                    == E4ARDUINO_STREXH_32_ET1_PATTERN)) {                  \
                strexh_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void tbb_32_et1(void *ctx, word rn, word rm);
#define E4ARDUINO_TBB_32_ET1_MASK              0xfff0fff0
#define E4ARDUINO_TBB_32_ET1_PATTERN           0xe8d0f000
#define PROC_E4ARDUINO_TBB_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_TBB_32_ET1_MASK)                      \
                    == E4ARDUINO_TBB_32_ET1_PATTERN)) {                     \
                tbb_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void tbh_32_et1(void *ctx, word rn, word rm);
#define E4ARDUINO_TBH_32_ET1_MASK              0xfff0fff0
#define E4ARDUINO_TBH_32_ET1_PATTERN           0xe8d0f010
#define PROC_E4ARDUINO_TBH_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_TBH_32_ET1_MASK)                      \
                    == E4ARDUINO_TBH_32_ET1_PATTERN)) {                     \
                tbh_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrexb_32_et1(void *ctx, word rn, word rt);
#define E4ARDUINO_LDREXB_32_ET1_MASK           0xfff00fff
#define E4ARDUINO_LDREXB_32_ET1_PATTERN        0xe8d00f4f
#define PROC_E4ARDUINO_LDREXB_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDREXB_32_ET1_MASK)                   \
                    == E4ARDUINO_LDREXB_32_ET1_PATTERN)) {                  \
                ldrexb_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12));                       \
            }                                                               \
        } while(0)

extern void ldrexh_32_et1(void *ctx, word rn, word rt);
#define E4ARDUINO_LDREXH_32_ET1_MASK           0xfff00fff
#define E4ARDUINO_LDREXH_32_ET1_PATTERN        0xe8d00f5f
#define PROC_E4ARDUINO_LDREXH_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDREXH_32_ET1_MASK)                   \
                    == E4ARDUINO_LDREXH_32_ET1_PATTERN)) {                  \
                ldrexh_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12));                       \
            }                                                               \
        } while(0)

/*
 *   A5.3.11
 *   Data processing (shifted register)
 */

extern void and_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_AND_REG_32_ET2_MASK          0xffe08000
#define E4ARDUINO_AND_REG_32_ET2_PATTERN       0xea000000
#define PROC_E4ARDUINO_AND_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_AND_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_AND_REG_32_ET2_PATTERN)) {                 \
                and_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void tst_reg_32_et2(void *ctx, word rn, word imm3, word imm2, word type, word rm);
#define E4ARDUINO_TST_REG_32_ET2_MASK          0xfff08f00
#define E4ARDUINO_TST_REG_32_ET2_PATTERN       0xea100f00
#define PROC_E4ARDUINO_TST_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_TST_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_TST_REG_32_ET2_PATTERN)) {                 \
                tst_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void bic_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_BIC_REG_32_ET2_MASK          0xffe08000
#define E4ARDUINO_BIC_REG_32_ET2_PATTERN       0xea200000
#define PROC_E4ARDUINO_BIC_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_BIC_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_BIC_REG_32_ET2_PATTERN)) {                 \
                bic_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void orr_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_ORR_REG_32_ET2_MASK          0xffe08000
#define E4ARDUINO_ORR_REG_32_ET2_PATTERN       0xea400000
#define PROC_E4ARDUINO_ORR_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ORR_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_ORR_REG_32_ET2_PATTERN)) {                 \
                orr_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mov_reg_32_et3(void *ctx, word s, word rd, word rm);
#define E4ARDUINO_MOV_REG_32_ET3_MASK          0xffeff0f0
#define E4ARDUINO_MOV_REG_32_ET3_PATTERN       0xea4f0000
#define PROC_E4ARDUINO_MOV_REG_32_ET3(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MOV_REG_32_ET3_MASK)                  \
                    == E4ARDUINO_MOV_REG_32_ET3_PATTERN)) {                 \
                mov_reg_32_et3(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void lsl_imm_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word rm);
#define E4ARDUINO_LSL_IMM_32_ET2_MASK          0xffef8030
#define E4ARDUINO_LSL_IMM_32_ET2_PATTERN       0xea4f0000
#define PROC_E4ARDUINO_LSL_IMM_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LSL_IMM_32_ET2_MASK)                  \
                    == E4ARDUINO_LSL_IMM_32_ET2_PATTERN)) {                 \
                lsl_imm_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void lsr_imm_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word rm);
#define E4ARDUINO_LSR_IMM_32_ET2_MASK          0xffef8030
#define E4ARDUINO_LSR_IMM_32_ET2_PATTERN       0xea4f0010
#define PROC_E4ARDUINO_LSR_IMM_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LSR_IMM_32_ET2_MASK)                  \
                    == E4ARDUINO_LSR_IMM_32_ET2_PATTERN)) {                 \
                lsr_imm_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void asr_imm_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word rm);
#define E4ARDUINO_ASR_IMM_32_ET2_MASK          0xffef8030
#define E4ARDUINO_ASR_IMM_32_ET2_PATTERN       0xea4f0020
#define PROC_E4ARDUINO_ASR_IMM_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ASR_IMM_32_ET2_MASK)                  \
                    == E4ARDUINO_ASR_IMM_32_ET2_PATTERN)) {                 \
                asr_imm_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void rrx_32_et1(void *ctx, word s, word rd, word rm);
#define E4ARDUINO_RRX_32_ET1_MASK              0xffeff0f0
#define E4ARDUINO_RRX_32_ET1_PATTERN           0xea4f0030
#define PROC_E4ARDUINO_RRX_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_RRX_32_ET1_MASK)                      \
                    == E4ARDUINO_RRX_32_ET1_PATTERN)) {                     \
                rrx_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ror_imm_32_et1(void *ctx, word s, word imm3, word rd, word imm2, word rm);
#define E4ARDUINO_ROR_IMM_32_ET1_MASK          0xffef8030
#define E4ARDUINO_ROR_IMM_32_ET1_PATTERN       0xea4f0030
#define PROC_E4ARDUINO_ROR_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ROR_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_ROR_IMM_32_ET1_PATTERN)) {                 \
                ror_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void orn_reg_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_ORN_REG_32_ET1_MASK          0xffe08000
#define E4ARDUINO_ORN_REG_32_ET1_PATTERN       0xea600000
#define PROC_E4ARDUINO_ORN_REG_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ORN_REG_32_ET1_MASK)                  \
                    == E4ARDUINO_ORN_REG_32_ET1_PATTERN)) {                 \
                orn_reg_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mvn_reg_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_MVN_REG_32_ET2_MASK          0xffef8000
#define E4ARDUINO_MVN_REG_32_ET2_PATTERN       0xea6f0000
#define PROC_E4ARDUINO_MVN_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MVN_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_MVN_REG_32_ET2_PATTERN)) {                 \
                mvn_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void eor_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_EOR_REG_32_ET2_MASK          0xffe08000
#define E4ARDUINO_EOR_REG_32_ET2_PATTERN       0xea800000
#define PROC_E4ARDUINO_EOR_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_EOR_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_EOR_REG_32_ET2_PATTERN)) {                 \
                eor_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void teq_reg_32_et1(void *ctx, word rn, word imm3, word imm2, word type, word rm);
#define E4ARDUINO_TEQ_REG_32_ET1_MASK          0xfff08f00
#define E4ARDUINO_TEQ_REG_32_ET1_PATTERN       0xea900f00
#define PROC_E4ARDUINO_TEQ_REG_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_TEQ_REG_32_ET1_MASK)                  \
                    == E4ARDUINO_TEQ_REG_32_ET1_PATTERN)) {                 \
                teq_reg_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void pkhbt_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word t, word rm);
#define E4ARDUINO_PKHBT_32_ET1_MASK            0xffe08020
#define E4ARDUINO_PKHBT_32_ET1_PATTERN         0xeac00000
#define PROC_E4ARDUINO_PKHBT_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PKHBT_32_ET1_MASK)                    \
                    == E4ARDUINO_PKHBT_32_ET1_PATTERN)) {                   \
                pkhbt_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000010) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void pkhtb_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word t, word rm);
#define E4ARDUINO_PKHTB_32_ET1_MASK            0xffe08020
#define E4ARDUINO_PKHTB_32_ET1_PATTERN         0xeac00020
#define PROC_E4ARDUINO_PKHTB_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PKHTB_32_ET1_MASK)                    \
                    == E4ARDUINO_PKHTB_32_ET1_PATTERN)) {                   \
                pkhtb_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000010) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void add_reg_32_et3(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_ADD_REG_32_ET3_MASK          0xffe08000
#define E4ARDUINO_ADD_REG_32_ET3_PATTERN       0xeb000000
#define PROC_E4ARDUINO_ADD_REG_32_ET3(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADD_REG_32_ET3_MASK)                  \
                    == E4ARDUINO_ADD_REG_32_ET3_PATTERN)) {                 \
                add_reg_32_et3(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void add_sp_reg_32_et3(void *ctx, word s, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_ADD_SP_REG_32_ET3_MASK       0xffef8000
#define E4ARDUINO_ADD_SP_REG_32_ET3_PATTERN    0xeb0d0000
#define PROC_E4ARDUINO_ADD_SP_REG_32_ET3(ctx, op)                           \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADD_SP_REG_32_ET3_MASK)               \
                    == E4ARDUINO_ADD_SP_REG_32_ET3_PATTERN)) {              \
                add_sp_reg_32_et3(                                          \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void cmn_reg_32_et2(void *ctx, word rn, word imm3, word imm2, word type, word rm);
#define E4ARDUINO_CMN_REG_32_ET2_MASK          0xfff08f00
#define E4ARDUINO_CMN_REG_32_ET2_PATTERN       0xeb100f00
#define PROC_E4ARDUINO_CMN_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_CMN_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_CMN_REG_32_ET2_PATTERN)) {                 \
                cmn_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void adc_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_ADC_REG_32_ET2_MASK          0xffe08000
#define E4ARDUINO_ADC_REG_32_ET2_PATTERN       0xeb400000
#define PROC_E4ARDUINO_ADC_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADC_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_ADC_REG_32_ET2_PATTERN)) {                 \
                adc_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sbc_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_SBC_REG_32_ET2_MASK          0xffe08000
#define E4ARDUINO_SBC_REG_32_ET2_PATTERN       0xeb600000
#define PROC_E4ARDUINO_SBC_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SBC_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_SBC_REG_32_ET2_PATTERN)) {                 \
                sbc_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sub_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_SUB_REG_32_ET2_MASK          0xffe08000
#define E4ARDUINO_SUB_REG_32_ET2_PATTERN       0xeba00000
#define PROC_E4ARDUINO_SUB_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SUB_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_SUB_REG_32_ET2_PATTERN)) {                 \
                sub_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sub_sp_reg_32_et1(void *ctx, word s, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_SUB_SP_REG_32_ET1_MASK       0xffef8000
#define E4ARDUINO_SUB_SP_REG_32_ET1_PATTERN    0xebad0000
#define PROC_E4ARDUINO_SUB_SP_REG_32_ET1(ctx, op)                           \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SUB_SP_REG_32_ET1_MASK)               \
                    == E4ARDUINO_SUB_SP_REG_32_ET1_PATTERN)) {              \
                sub_sp_reg_32_et1(                                          \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void cmp_reg_32_et3(void *ctx, word rn, word imm3, word imm2, word type, word rm);
#define E4ARDUINO_CMP_REG_32_ET3_MASK          0xfff08f00
#define E4ARDUINO_CMP_REG_32_ET3_PATTERN       0xebb00f00
#define PROC_E4ARDUINO_CMP_REG_32_ET3(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_CMP_REG_32_ET3_MASK)                  \
                    == E4ARDUINO_CMP_REG_32_ET3_PATTERN)) {                 \
                cmp_reg_32_et3(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void rsb_reg_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
#define E4ARDUINO_RSB_REG_32_ET1_MASK          0xffe08000
#define E4ARDUINO_RSB_REG_32_ET1_PATTERN       0xebc00000
#define PROC_E4ARDUINO_RSB_REG_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_RSB_REG_32_ET1_MASK)                  \
                    == E4ARDUINO_RSB_REG_32_ET1_PATTERN)) {                 \
                rsb_reg_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.18
 *   Coprocessor instructions
 */

extern void stc_32_et1(void *ctx, word p, word u, word n, word w, word rn, word crd, word cprc, word imm8);
#define E4ARDUINO_STC_32_ET1_MASK              0xfe100000
#define E4ARDUINO_STC_32_ET1_PATTERN           0xec000000
#define PROC_E4ARDUINO_STC_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STC_32_ET1_MASK)                      \
                    == E4ARDUINO_STC_32_ET1_PATTERN)) {                     \
                stc_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void stc2_32_et2(void *ctx, word p, word u, word n, word w, word rn, word crd, word cprc, word imm8);
#define E4ARDUINO_STC2_32_ET2_MASK             0xfe100000
#define E4ARDUINO_STC2_32_ET2_PATTERN          0xfc000000
#define PROC_E4ARDUINO_STC2_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STC2_32_ET2_MASK)                     \
                    == E4ARDUINO_STC2_32_ET2_PATTERN)) {                    \
                stc2_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldc_imm_32_et1(void *ctx, word p, word u, word d, word w, word rn, word crd, word cprc, word imm8);
#define E4ARDUINO_LDC_IMM_32_ET1_MASK          0xfe100000
#define E4ARDUINO_LDC_IMM_32_ET1_PATTERN       0xec100000
#define PROC_E4ARDUINO_LDC_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDC_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_LDC_IMM_32_ET1_PATTERN)) {                 \
                ldc_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldc2_imm_32_et2(void *ctx, word p, word u, word d, word w, word rn, word crd, word cprc, word imm8);
#define E4ARDUINO_LDC2_IMM_32_ET2_MASK         0xfe100000
#define E4ARDUINO_LDC2_IMM_32_ET2_PATTERN      0xfc100000
#define PROC_E4ARDUINO_LDC2_IMM_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDC2_IMM_32_ET2_MASK)                 \
                    == E4ARDUINO_LDC2_IMM_32_ET2_PATTERN)) {                \
                ldc2_imm_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldc_lit_32_et1(void *ctx, word p, word u, word d, word w, word crd, word cprc, word imm8);
#define E4ARDUINO_LDC_LIT_32_ET1_MASK          0xfe1f0000
#define E4ARDUINO_LDC_LIT_32_ET1_PATTERN       0xec1f0000
#define PROC_E4ARDUINO_LDC_LIT_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDC_LIT_32_ET1_MASK)                  \
                    == E4ARDUINO_LDC_LIT_32_ET1_PATTERN)) {                 \
                ldc_lit_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldc2_lit_32_et2(void *ctx, word p, word u, word d, word w, word crd, word cprc, word imm8);
#define E4ARDUINO_LDC2_LIT_32_ET2_MASK         0xfe1f0000
#define E4ARDUINO_LDC2_LIT_32_ET2_PATTERN      0xfc1f0000
#define PROC_E4ARDUINO_LDC2_LIT_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDC2_LIT_32_ET2_MASK)                 \
                    == E4ARDUINO_LDC2_LIT_32_ET2_PATTERN)) {                \
                ldc2_lit_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void mcrr_32_et1(void *ctx, word rt2, word rt, word cprc, word opc1, word crm);
#define E4ARDUINO_MCRR_32_ET1_MASK             0xfff00000
#define E4ARDUINO_MCRR_32_ET1_PATTERN          0xec400000
#define PROC_E4ARDUINO_MCRR_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MCRR_32_ET1_MASK)                     \
                    == E4ARDUINO_MCRR_32_ET1_PATTERN)) {                    \
                mcrr_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000f0) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mcrr2_32_et2(void *ctx, word rt2, word rt, word cprc, word opc1, word crm);
#define E4ARDUINO_MCRR2_32_ET2_MASK            0xfff00000
#define E4ARDUINO_MCRR2_32_ET2_PATTERN         0xfc400000
#define PROC_E4ARDUINO_MCRR2_32_ET2(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MCRR2_32_ET2_MASK)                    \
                    == E4ARDUINO_MCRR2_32_ET2_PATTERN)) {                   \
                mcrr2_32_et2(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000f0) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mrrc_32_et1(void *ctx, word rt2, word rt, word cprc, word opc1, word crm);
#define E4ARDUINO_MRRC_32_ET1_MASK             0xfff00000
#define E4ARDUINO_MRRC_32_ET1_PATTERN          0xec500000
#define PROC_E4ARDUINO_MRRC_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MRRC_32_ET1_MASK)                     \
                    == E4ARDUINO_MRRC_32_ET1_PATTERN)) {                    \
                mrrc_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000f0) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mrrc2_32_et2(void *ctx, word rt2, word rt, word cprc, word opc1, word crm);
#define E4ARDUINO_MRRC2_32_ET2_MASK            0xfff00000
#define E4ARDUINO_MRRC2_32_ET2_PATTERN         0xfc500000
#define PROC_E4ARDUINO_MRRC2_32_ET2(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MRRC2_32_ET2_MASK)                    \
                    == E4ARDUINO_MRRC2_32_ET2_PATTERN)) {                   \
                mrrc2_32_et2(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000f0) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void cdp_32_et1(void *ctx, word opc1, word crn, word crd, word cprc, word opc2, word crm);
#define E4ARDUINO_CDP_32_ET1_MASK              0xff000e10
#define E4ARDUINO_CDP_32_ET1_PATTERN           0xee000a00
#define PROC_E4ARDUINO_CDP_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_CDP_32_ET1_MASK)                      \
                    == E4ARDUINO_CDP_32_ET1_PATTERN)) {                     \
                cdp_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x00f00000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000e0) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void cdp2_32_et2(void *ctx, word opc1, word crn, word crd, word cprc, word opc2, word crm);
#define E4ARDUINO_CDP2_32_ET2_MASK             0xff000e10
#define E4ARDUINO_CDP2_32_ET2_PATTERN          0xfe000a00
#define PROC_E4ARDUINO_CDP2_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_CDP2_32_ET2_MASK)                     \
                    == E4ARDUINO_CDP2_32_ET2_PATTERN)) {                    \
                cdp2_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00f00000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000e0) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mcr_32_et1(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm);
#define E4ARDUINO_MCR_32_ET1_MASK              0xff100010
#define E4ARDUINO_MCR_32_ET1_PATTERN           0xee000010
#define PROC_E4ARDUINO_MCR_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MCR_32_ET1_MASK)                      \
                    == E4ARDUINO_MCR_32_ET1_PATTERN)) {                     \
                mcr_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x00e00000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000e0) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mcr2_32_et2(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm);
#define E4ARDUINO_MCR2_32_ET2_MASK             0xff100010
#define E4ARDUINO_MCR2_32_ET2_PATTERN          0xfe000010
#define PROC_E4ARDUINO_MCR2_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MCR2_32_ET2_MASK)                     \
                    == E4ARDUINO_MCR2_32_ET2_PATTERN)) {                    \
                mcr2_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00e00000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000e0) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mrc_32_et1(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm);
#define E4ARDUINO_MRC_32_ET1_MASK              0xff100010
#define E4ARDUINO_MRC_32_ET1_PATTERN           0xee100010
#define PROC_E4ARDUINO_MRC_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MRC_32_ET1_MASK)                      \
                    == E4ARDUINO_MRC_32_ET1_PATTERN)) {                     \
                mrc_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x00e00000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000e0) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mrc2_32_et2(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm);
#define E4ARDUINO_MRC2_32_ET2_MASK             0xff100010
#define E4ARDUINO_MRC2_32_ET2_PATTERN          0xfe100010
#define PROC_E4ARDUINO_MRC2_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MRC2_32_ET2_MASK)                     \
                    == E4ARDUINO_MRC2_32_ET2_PATTERN)) {                    \
                mrc2_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00e00000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000e0) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.1
 *   Data processing (modified immediate)
 */

extern void and_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_AND_IMM_32_ET1_MASK          0xfbe08000
#define E4ARDUINO_AND_IMM_32_ET1_PATTERN       0xf0000000
#define PROC_E4ARDUINO_AND_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_AND_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_AND_IMM_32_ET1_PATTERN)) {                 \
                and_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void tst_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word imm8);
#define E4ARDUINO_TST_IMM_32_ET1_MASK          0xfbe08f00
#define E4ARDUINO_TST_IMM_32_ET1_PATTERN       0xf0000f00
#define PROC_E4ARDUINO_TST_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_TST_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_TST_IMM_32_ET1_PATTERN)) {                 \
                tst_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void bic_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_BIC_IMM_32_ET1_MASK          0xfbe08000
#define E4ARDUINO_BIC_IMM_32_ET1_PATTERN       0xf0200000
#define PROC_E4ARDUINO_BIC_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_BIC_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_BIC_IMM_32_ET1_PATTERN)) {                 \
                bic_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void orr_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_ORR_IMM_32_ET1_MASK          0xfbe08000
#define E4ARDUINO_ORR_IMM_32_ET1_PATTERN       0xf0400000
#define PROC_E4ARDUINO_ORR_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ORR_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_ORR_IMM_32_ET1_PATTERN)) {                 \
                orr_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void mov_imm_32_et2(void *ctx, word i, word s, word imm3, word rd, word imm8);
#define E4ARDUINO_MOV_IMM_32_ET2_MASK          0xfbef8000
#define E4ARDUINO_MOV_IMM_32_ET2_PATTERN       0xf04f0000
#define PROC_E4ARDUINO_MOV_IMM_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MOV_IMM_32_ET2_MASK)                  \
                    == E4ARDUINO_MOV_IMM_32_ET2_PATTERN)) {                 \
                mov_imm_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void orn_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_ORN_IMM_32_ET1_MASK          0xfbe08000
#define E4ARDUINO_ORN_IMM_32_ET1_PATTERN       0xf0600000
#define PROC_E4ARDUINO_ORN_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ORN_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_ORN_IMM_32_ET1_PATTERN)) {                 \
                orn_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void mvn_imm_32_et1(void *ctx, word i, word s, word imm3, word rd, word imm8);
#define E4ARDUINO_MVN_IMM_32_ET1_MASK          0xfbef8000
#define E4ARDUINO_MVN_IMM_32_ET1_PATTERN       0xf06f0000
#define PROC_E4ARDUINO_MVN_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MVN_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_MVN_IMM_32_ET1_PATTERN)) {                 \
                mvn_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void eor_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_EOR_IMM_32_ET1_MASK          0xfbe08000
#define E4ARDUINO_EOR_IMM_32_ET1_PATTERN       0xf0800000
#define PROC_E4ARDUINO_EOR_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_EOR_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_EOR_IMM_32_ET1_PATTERN)) {                 \
                eor_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void teq_imm_32_et1(void *ctx, word i, word rn, word imm3, word imm8);
#define E4ARDUINO_TEQ_IMM_32_ET1_MASK          0xfbf08f00
#define E4ARDUINO_TEQ_IMM_32_ET1_PATTERN       0xf0900f00
#define PROC_E4ARDUINO_TEQ_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_TEQ_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_TEQ_IMM_32_ET1_PATTERN)) {                 \
                teq_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void add_imm_32_et3(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_ADD_IMM_32_ET3_MASK          0xfbe08000
#define E4ARDUINO_ADD_IMM_32_ET3_PATTERN       0xf1000000
#define PROC_E4ARDUINO_ADD_IMM_32_ET3(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADD_IMM_32_ET3_MASK)                  \
                    == E4ARDUINO_ADD_IMM_32_ET3_PATTERN)) {                 \
                add_imm_32_et3(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void add_sp_imm_32_et3(void *ctx, word i, word s, word imm3, word rd, word imm8);
#define E4ARDUINO_ADD_SP_IMM_32_ET3_MASK       0xfbef8000
#define E4ARDUINO_ADD_SP_IMM_32_ET3_PATTERN    0xf10d0000
#define PROC_E4ARDUINO_ADD_SP_IMM_32_ET3(ctx, op)                           \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADD_SP_IMM_32_ET3_MASK)               \
                    == E4ARDUINO_ADD_SP_IMM_32_ET3_PATTERN)) {              \
                add_sp_imm_32_et3(                                          \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void cmn_imm_32_et1(void *ctx, word i, word rn, word imm3, word imm8);
#define E4ARDUINO_CMN_IMM_32_ET1_MASK          0xfbf08f00
#define E4ARDUINO_CMN_IMM_32_ET1_PATTERN       0xf1100f00
#define PROC_E4ARDUINO_CMN_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_CMN_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_CMN_IMM_32_ET1_PATTERN)) {                 \
                cmn_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void adc_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_ADC_IMM_32_ET1_MASK          0xfbe08000
#define E4ARDUINO_ADC_IMM_32_ET1_PATTERN       0xf1400000
#define PROC_E4ARDUINO_ADC_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADC_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_ADC_IMM_32_ET1_PATTERN)) {                 \
                adc_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void sbc_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_SBC_IMM_32_ET1_MASK          0xfbe08000
#define E4ARDUINO_SBC_IMM_32_ET1_PATTERN       0xf1600000
#define PROC_E4ARDUINO_SBC_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SBC_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_SBC_IMM_32_ET1_PATTERN)) {                 \
                sbc_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void sub_imm_32_et3(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_SUB_IMM_32_ET3_MASK          0xfbe08000
#define E4ARDUINO_SUB_IMM_32_ET3_PATTERN       0xf1a00000
#define PROC_E4ARDUINO_SUB_IMM_32_ET3(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SUB_IMM_32_ET3_MASK)                  \
                    == E4ARDUINO_SUB_IMM_32_ET3_PATTERN)) {                 \
                sub_imm_32_et3(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void sub_sp_imm_32_et2(void *ctx, word i, word s, word imm3, word rd, word imm8);
#define E4ARDUINO_SUB_SP_IMM_32_ET2_MASK       0xfbef8000
#define E4ARDUINO_SUB_SP_IMM_32_ET2_PATTERN    0xf1ad0000
#define PROC_E4ARDUINO_SUB_SP_IMM_32_ET2(ctx, op)                           \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SUB_SP_IMM_32_ET2_MASK)               \
                    == E4ARDUINO_SUB_SP_IMM_32_ET2_PATTERN)) {              \
                sub_sp_imm_32_et2(                                          \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void cmp_imm_32_et2(void *ctx, word i, word rn, word imm3, word imm8);
#define E4ARDUINO_CMP_IMM_32_ET2_MASK          0xfbf08f00
#define E4ARDUINO_CMP_IMM_32_ET2_PATTERN       0xf1b00f00
#define PROC_E4ARDUINO_CMP_IMM_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_CMP_IMM_32_ET2_MASK)                  \
                    == E4ARDUINO_CMP_IMM_32_ET2_PATTERN)) {                 \
                cmp_imm_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void rsb_imm_32_et2(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_RSB_IMM_32_ET2_MASK          0xfbe08000
#define E4ARDUINO_RSB_IMM_32_ET2_PATTERN       0xf1c00000
#define PROC_E4ARDUINO_RSB_IMM_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_RSB_IMM_32_ET2_MASK)                  \
                    == E4ARDUINO_RSB_IMM_32_ET2_PATTERN)) {                 \
                rsb_imm_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.3
 *   Data processing (plain binary immediate)
 */

extern void add_imm_32_et4(void *ctx, word i, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_ADD_IMM_32_ET4_MASK          0xfbf08000
#define E4ARDUINO_ADD_IMM_32_ET4_PATTERN       0xf2000000
#define PROC_E4ARDUINO_ADD_IMM_32_ET4(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADD_IMM_32_ET4_MASK)                  \
                    == E4ARDUINO_ADD_IMM_32_ET4_PATTERN)) {                 \
                add_imm_32_et4(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void add_sp_imm_32_et4(void *ctx, word i, word imm3, word rd, word imm8);
#define E4ARDUINO_ADD_SP_IMM_32_ET4_MASK       0xfbff8000
#define E4ARDUINO_ADD_SP_IMM_32_ET4_PATTERN    0xf20d0000
#define PROC_E4ARDUINO_ADD_SP_IMM_32_ET4(ctx, op)                           \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADD_SP_IMM_32_ET4_MASK)               \
                    == E4ARDUINO_ADD_SP_IMM_32_ET4_PATTERN)) {              \
                add_sp_imm_32_et4(                                          \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void adr_32_et2(void *ctx, word i, word imm3, word rd, word imm8);
#define E4ARDUINO_ADR_32_ET2_MASK              0xfbff8000
#define E4ARDUINO_ADR_32_ET2_PATTERN           0xf2af0000
#define PROC_E4ARDUINO_ADR_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADR_32_ET2_MASK)                      \
                    == E4ARDUINO_ADR_32_ET2_PATTERN)) {                     \
                adr_32_et2(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void mov_imm_32_et3(void *ctx, word i, word imm4, word imm3, word rd, word imm8);
#define E4ARDUINO_MOV_IMM_32_ET3_MASK          0xfbf08000
#define E4ARDUINO_MOV_IMM_32_ET3_PATTERN       0xf2400000
#define PROC_E4ARDUINO_MOV_IMM_32_ET3(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MOV_IMM_32_ET3_MASK)                  \
                    == E4ARDUINO_MOV_IMM_32_ET3_PATTERN)) {                 \
                mov_imm_32_et3(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void sub_imm_32_et4(void *ctx, word i, word rn, word imm3, word rd, word imm8);
#define E4ARDUINO_SUB_IMM_32_ET4_MASK          0xfbf08000
#define E4ARDUINO_SUB_IMM_32_ET4_PATTERN       0xf2a00000
#define PROC_E4ARDUINO_SUB_IMM_32_ET4(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SUB_IMM_32_ET4_MASK)                  \
                    == E4ARDUINO_SUB_IMM_32_ET4_PATTERN)) {                 \
                sub_imm_32_et4(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void sub_sp_imm_32_et3(void *ctx, word i, word imm3, word rd, word imm8);
#define E4ARDUINO_SUB_SP_IMM_32_ET3_MASK       0xfbff8000
#define E4ARDUINO_SUB_SP_IMM_32_ET3_PATTERN    0xf2ad0000
#define PROC_E4ARDUINO_SUB_SP_IMM_32_ET3(ctx, op)                           \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SUB_SP_IMM_32_ET3_MASK)               \
                    == E4ARDUINO_SUB_SP_IMM_32_ET3_PATTERN)) {              \
                sub_sp_imm_32_et3(                                          \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void adr_32_et3(void *ctx, word i, word imm3, word rd, word imm8);
#define E4ARDUINO_ADR_32_ET3_MASK              0xfbff8000
#define E4ARDUINO_ADR_32_ET3_PATTERN           0xf20f0000
#define PROC_E4ARDUINO_ADR_32_ET3(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ADR_32_ET3_MASK)                      \
                    == E4ARDUINO_ADR_32_ET3_PATTERN)) {                     \
                adr_32_et3(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void movt_32_et1(void *ctx, word i, word imm4, word imm3, word rd, word imm8);
#define E4ARDUINO_MOVT_32_ET1_MASK             0xfbf08000
#define E4ARDUINO_MOVT_32_ET1_PATTERN          0xf2c00000
#define PROC_E4ARDUINO_MOVT_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MOVT_32_ET1_MASK)                     \
                    == E4ARDUINO_MOVT_32_ET1_PATTERN)) {                    \
                movt_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ssat_32_et1(void *ctx, word sh, word rn, word imm3, word rd, word imm2, word si);
#define E4ARDUINO_SSAT_32_ET1_MASK             0xffd08020
#define E4ARDUINO_SSAT_32_ET1_PATTERN          0xf3000000
#define PROC_E4ARDUINO_SSAT_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SSAT_32_ET1_MASK)                     \
                    == E4ARDUINO_SSAT_32_ET1_PATTERN)) {                    \
                ssat_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000001f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ssat16_32_et1(void *ctx, word rn, word rd, word si);
#define E4ARDUINO_SSAT16_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SSAT16_32_ET1_PATTERN        0xf3200000
#define PROC_E4ARDUINO_SSAT16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SSAT16_32_ET1_MASK)                   \
                    == E4ARDUINO_SSAT16_32_ET1_PATTERN)) {                  \
                ssat16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sbfx_32_et1(void *ctx, word rn, word imm3, word rd, word imm2, word wm);
#define E4ARDUINO_SBFX_32_ET1_MASK             0xfff08020
#define E4ARDUINO_SBFX_32_ET1_PATTERN          0xf3400000
#define PROC_E4ARDUINO_SBFX_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SBFX_32_ET1_MASK)                     \
                    == E4ARDUINO_SBFX_32_ET1_PATTERN)) {                    \
                sbfx_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000001f) >> 0));                        \
            }                                                               \
        } while(0)

extern void bfi_32_et1(void *ctx, word rn, word imm3, word rd, word imm2, word msb);
#define E4ARDUINO_BFI_32_ET1_MASK              0xfff08020
#define E4ARDUINO_BFI_32_ET1_PATTERN           0xf3600000
#define PROC_E4ARDUINO_BFI_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_BFI_32_ET1_MASK)                      \
                    == E4ARDUINO_BFI_32_ET1_PATTERN)) {                     \
                bfi_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000001f) >> 0));                        \
            }                                                               \
        } while(0)

extern void bfc_32_et1(void *ctx, word imm3, word rd, word imm2, word msb);
#define E4ARDUINO_BFC_32_ET1_MASK              0xffff8020
#define E4ARDUINO_BFC_32_ET1_PATTERN           0xf36f0000
#define PROC_E4ARDUINO_BFC_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_BFC_32_ET1_MASK)                      \
                    == E4ARDUINO_BFC_32_ET1_PATTERN)) {                     \
                bfc_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000001f) >> 0));                        \
            }                                                               \
        } while(0)

extern void usat_32_et1(void *ctx, word sh, word rn, word imm3, word rd, word imm2, word si);
#define E4ARDUINO_USAT_32_ET1_MASK             0xffd08020
#define E4ARDUINO_USAT_32_ET1_PATTERN          0xf3800000
#define PROC_E4ARDUINO_USAT_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_USAT_32_ET1_MASK)                     \
                    == E4ARDUINO_USAT_32_ET1_PATTERN)) {                    \
                usat_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000001f) >> 0));                        \
            }                                                               \
        } while(0)

extern void usat16_32_et1(void *ctx, word rn, word rd, word si);
#define E4ARDUINO_USAT16_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_USAT16_32_ET1_PATTERN        0xf3a00000
#define PROC_E4ARDUINO_USAT16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_USAT16_32_ET1_MASK)                   \
                    == E4ARDUINO_USAT16_32_ET1_PATTERN)) {                  \
                usat16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ubfx_32_et1(void *ctx, word rn, word imm3, word rd, word imm2, word wm);
#define E4ARDUINO_UBFX_32_ET1_MASK             0xfff08020
#define E4ARDUINO_UBFX_32_ET1_PATTERN          0xf3c00000
#define PROC_E4ARDUINO_UBFX_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UBFX_32_ET1_MASK)                     \
                    == E4ARDUINO_UBFX_32_ET1_PATTERN)) {                    \
                ubfx_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00007000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000c0) >> 6),                         \
                    ((OP32(op) & 0x0000001f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.4
 *   Branches and miscellaneous control
 */

extern void b_32_et3(void *ctx, word s, word cond, word imm6, word j1, word j2, word imm11);
#define E4ARDUINO_B_32_ET3_MASK                0xf800d000
#define E4ARDUINO_B_32_ET3_PATTERN             0xf0008000
#define PROC_E4ARDUINO_B_32_ET3(ctx, op)                                    \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_B_32_ET3_MASK)                        \
                    == E4ARDUINO_B_32_ET3_PATTERN)) {                       \
                b_32_et3(                                                   \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x03c00000) >> 22),                        \
                    ((OP32(op) & 0x003f0000) >> 16),                        \
                    ((OP32(op) & 0x00002000) >> 13),                        \
                    ((OP32(op) & 0x00000800) >> 11),                        \
                    ((OP32(op) & 0x000007ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void msr_32_et1(void *ctx, word rn, word mask, word sysm);
#define E4ARDUINO_MSR_32_ET1_MASK              0xfff0f300
#define E4ARDUINO_MSR_32_ET1_PATTERN           0xf3808000
#define PROC_E4ARDUINO_MSR_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MSR_32_ET1_MASK)                      \
                    == E4ARDUINO_MSR_32_ET1_PATTERN)) {                     \
                msr_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000c00) >> 10),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void nop_32_et2(void *ctx);
#define E4ARDUINO_NOP_32_ET2_MASK              0xffffffff
#define E4ARDUINO_NOP_32_ET2_PATTERN           0xf3af8000
#define PROC_E4ARDUINO_NOP_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_NOP_32_ET2_MASK)                      \
                    == E4ARDUINO_NOP_32_ET2_PATTERN)) {                     \
                nop_32_et2(                                                 \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void yield_32_et2(void *ctx);
#define E4ARDUINO_YIELD_32_ET2_MASK            0xffffffff
#define E4ARDUINO_YIELD_32_ET2_PATTERN         0xf3af8001
#define PROC_E4ARDUINO_YIELD_32_ET2(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_YIELD_32_ET2_MASK)                    \
                    == E4ARDUINO_YIELD_32_ET2_PATTERN)) {                   \
                yield_32_et2(                                               \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void wfe_32_et2(void *ctx);
#define E4ARDUINO_WFE_32_ET2_MASK              0xffffffff
#define E4ARDUINO_WFE_32_ET2_PATTERN           0xf3af8002
#define PROC_E4ARDUINO_WFE_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_WFE_32_ET2_MASK)                      \
                    == E4ARDUINO_WFE_32_ET2_PATTERN)) {                     \
                wfe_32_et2(                                                 \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void wfi_32_et2(void *ctx);
#define E4ARDUINO_WFI_32_ET2_MASK              0xffffffff
#define E4ARDUINO_WFI_32_ET2_PATTERN           0xf3af8003
#define PROC_E4ARDUINO_WFI_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_WFI_32_ET2_MASK)                      \
                    == E4ARDUINO_WFI_32_ET2_PATTERN)) {                     \
                wfi_32_et2(                                                 \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void sev_32_et2(void *ctx);
#define E4ARDUINO_SEV_32_ET2_MASK              0xffffffff
#define E4ARDUINO_SEV_32_ET2_PATTERN           0xf3af8004
#define PROC_E4ARDUINO_SEV_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SEV_32_ET2_MASK)                      \
                    == E4ARDUINO_SEV_32_ET2_PATTERN)) {                     \
                sev_32_et2(                                                 \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void dbg_32_et1(void *ctx, word opt);
#define E4ARDUINO_DBG_32_ET1_MASK              0xfffffff0
#define E4ARDUINO_DBG_32_ET1_PATTERN           0xf3af80f0
#define PROC_E4ARDUINO_DBG_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_DBG_32_ET1_MASK)                      \
                    == E4ARDUINO_DBG_32_ET1_PATTERN)) {                     \
                dbg_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void clrex_32_et1(void *ctx);
#define E4ARDUINO_CLREX_32_ET1_MASK            0xffffffff
#define E4ARDUINO_CLREX_32_ET1_PATTERN         0xf3bf8f2f
#define PROC_E4ARDUINO_CLREX_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_CLREX_32_ET1_MASK)                    \
                    == E4ARDUINO_CLREX_32_ET1_PATTERN)) {                   \
                clrex_32_et1(                                               \
                    ctx);                                                   \
            }                                                               \
        } while(0)

extern void dsb_32_et1(void *ctx, word opt);
#define E4ARDUINO_DSB_32_ET1_MASK              0xfffffff0
#define E4ARDUINO_DSB_32_ET1_PATTERN           0xf3bf8f40
#define PROC_E4ARDUINO_DSB_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_DSB_32_ET1_MASK)                      \
                    == E4ARDUINO_DSB_32_ET1_PATTERN)) {                     \
                dsb_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void dmb_32_et1(void *ctx, word opt);
#define E4ARDUINO_DMB_32_ET1_MASK              0xfffffff0
#define E4ARDUINO_DMB_32_ET1_PATTERN           0xf3bf8f50
#define PROC_E4ARDUINO_DMB_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_DMB_32_ET1_MASK)                      \
                    == E4ARDUINO_DMB_32_ET1_PATTERN)) {                     \
                dmb_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void isb_32_et1(void *ctx, word opt);
#define E4ARDUINO_ISB_32_ET1_MASK              0xfffffff0
#define E4ARDUINO_ISB_32_ET1_PATTERN           0xf3bf8f60
#define PROC_E4ARDUINO_ISB_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ISB_32_ET1_MASK)                      \
                    == E4ARDUINO_ISB_32_ET1_PATTERN)) {                     \
                isb_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mrs_32_et1(void *ctx, word rd, word sysm);
#define E4ARDUINO_MRS_32_ET1_MASK              0xfffff000
#define E4ARDUINO_MRS_32_ET1_PATTERN           0xf3ef8000
#define PROC_E4ARDUINO_MRS_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MRS_32_ET1_MASK)                      \
                    == E4ARDUINO_MRS_32_ET1_PATTERN)) {                     \
                mrs_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void udf_32_et2(void *ctx, word imm4, word imm12);
#define E4ARDUINO_UDF_32_ET2_MASK              0xfff0f000
#define E4ARDUINO_UDF_32_ET2_PATTERN           0xf7f0a000
#define PROC_E4ARDUINO_UDF_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UDF_32_ET2_MASK)                      \
                    == E4ARDUINO_UDF_32_ET2_PATTERN)) {                     \
                udf_32_et2(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void b_32_et4(void *ctx, word s, word imm10, word j1, word j2, word imm11);
#define E4ARDUINO_B_32_ET4_MASK                0xf800d000
#define E4ARDUINO_B_32_ET4_PATTERN             0xf0009000
#define PROC_E4ARDUINO_B_32_ET4(ctx, op)                                    \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_B_32_ET4_MASK)                        \
                    == E4ARDUINO_B_32_ET4_PATTERN)) {                       \
                b_32_et4(                                                   \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x03ff0000) >> 16),                        \
                    ((OP32(op) & 0x00002000) >> 13),                        \
                    ((OP32(op) & 0x00000800) >> 11),                        \
                    ((OP32(op) & 0x000007ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void bl_32_et1(void *ctx, word s, word imm10, word j1, word j2, word imm11);
#define E4ARDUINO_BL_32_ET1_MASK               0xf800d000
#define E4ARDUINO_BL_32_ET1_PATTERN            0xf000d000
#define PROC_E4ARDUINO_BL_32_ET1(ctx, op)                                   \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_BL_32_ET1_MASK)                       \
                    == E4ARDUINO_BL_32_ET1_PATTERN)) {                      \
                bl_32_et1(                                                  \
                    ctx,                                                    \
                    ((OP32(op) & 0x04000000) >> 26),                        \
                    ((OP32(op) & 0x03ff0000) >> 16),                        \
                    ((OP32(op) & 0x00002000) >> 13),                        \
                    ((OP32(op) & 0x00000800) >> 11),                        \
                    ((OP32(op) & 0x000007ff) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.10
 *   Store single data item
 */

extern void strb_imm_32_et2(void *ctx, word rn, word rt, word imm12);
#define E4ARDUINO_STRB_IMM_32_ET2_MASK         0xfff00000
#define E4ARDUINO_STRB_IMM_32_ET2_PATTERN      0xf8800000
#define PROC_E4ARDUINO_STRB_IMM_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRB_IMM_32_ET2_MASK)                 \
                    == E4ARDUINO_STRB_IMM_32_ET2_PATTERN)) {                \
                strb_imm_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void strb_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
#define E4ARDUINO_STRB_IMM_32_ET3_MASK         0xfff00800
#define E4ARDUINO_STRB_IMM_32_ET3_PATTERN      0xf8000800
#define PROC_E4ARDUINO_STRB_IMM_32_ET3(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRB_IMM_32_ET3_MASK)                 \
                    == E4ARDUINO_STRB_IMM_32_ET3_PATTERN)) {                \
                strb_imm_32_et3(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000400) >> 10),                        \
                    ((OP32(op) & 0x00000200) >> 9),                         \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void strbt_32_et1(void *ctx, word rn, word rt, word imm8);
#define E4ARDUINO_STRBT_32_ET1_MASK            0xfff00f00
#define E4ARDUINO_STRBT_32_ET1_PATTERN         0xf8000e00
#define PROC_E4ARDUINO_STRBT_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRBT_32_ET1_MASK)                    \
                    == E4ARDUINO_STRBT_32_ET1_PATTERN)) {                   \
                strbt_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void strb_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
#define E4ARDUINO_STRB_REG_32_ET2_MASK         0xfff00fc0
#define E4ARDUINO_STRB_REG_32_ET2_PATTERN      0xf8000000
#define PROC_E4ARDUINO_STRB_REG_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRB_REG_32_ET2_MASK)                 \
                    == E4ARDUINO_STRB_REG_32_ET2_PATTERN)) {                \
                strb_reg_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void strh_imm_32_et2(void *ctx, word rn, word rt, word imm12);
#define E4ARDUINO_STRH_IMM_32_ET2_MASK         0xfff00000
#define E4ARDUINO_STRH_IMM_32_ET2_PATTERN      0xf8a00000
#define PROC_E4ARDUINO_STRH_IMM_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRH_IMM_32_ET2_MASK)                 \
                    == E4ARDUINO_STRH_IMM_32_ET2_PATTERN)) {                \
                strh_imm_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void strh_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
#define E4ARDUINO_STRH_IMM_32_ET3_MASK         0xfff00800
#define E4ARDUINO_STRH_IMM_32_ET3_PATTERN      0xf8200800
#define PROC_E4ARDUINO_STRH_IMM_32_ET3(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRH_IMM_32_ET3_MASK)                 \
                    == E4ARDUINO_STRH_IMM_32_ET3_PATTERN)) {                \
                strh_imm_32_et3(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000400) >> 10),                        \
                    ((OP32(op) & 0x00000200) >> 9),                         \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void strht_32_et1(void *ctx, word rn, word rt, word imm8);
#define E4ARDUINO_STRHT_32_ET1_MASK            0xfff00f00
#define E4ARDUINO_STRHT_32_ET1_PATTERN         0xf8200e00
#define PROC_E4ARDUINO_STRHT_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRHT_32_ET1_MASK)                    \
                    == E4ARDUINO_STRHT_32_ET1_PATTERN)) {                   \
                strht_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void strh_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
#define E4ARDUINO_STRH_REG_32_ET2_MASK         0xfff00fc0
#define E4ARDUINO_STRH_REG_32_ET2_PATTERN      0xf8200000
#define PROC_E4ARDUINO_STRH_REG_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRH_REG_32_ET2_MASK)                 \
                    == E4ARDUINO_STRH_REG_32_ET2_PATTERN)) {                \
                strh_reg_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void str_imm_32_et3(void *ctx, word rn, word rt, word imm12);
#define E4ARDUINO_STR_IMM_32_ET3_MASK          0xfff00000
#define E4ARDUINO_STR_IMM_32_ET3_PATTERN       0xf8c00000
#define PROC_E4ARDUINO_STR_IMM_32_ET3(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STR_IMM_32_ET3_MASK)                  \
                    == E4ARDUINO_STR_IMM_32_ET3_PATTERN)) {                 \
                str_imm_32_et3(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void str_imm_32_et4(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
#define E4ARDUINO_STR_IMM_32_ET4_MASK          0xfff00800
#define E4ARDUINO_STR_IMM_32_ET4_PATTERN       0xf8400800
#define PROC_E4ARDUINO_STR_IMM_32_ET4(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STR_IMM_32_ET4_MASK)                  \
                    == E4ARDUINO_STR_IMM_32_ET4_PATTERN)) {                 \
                str_imm_32_et4(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000400) >> 10),                        \
                    ((OP32(op) & 0x00000200) >> 9),                         \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void strt_32_et1(void *ctx, word rn, word rt, word imm8);
#define E4ARDUINO_STRT_32_ET1_MASK             0xfff00f00
#define E4ARDUINO_STRT_32_ET1_PATTERN          0xf8400e00
#define PROC_E4ARDUINO_STRT_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STRT_32_ET1_MASK)                     \
                    == E4ARDUINO_STRT_32_ET1_PATTERN)) {                    \
                strt_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void push_32_et3(void *ctx, word rt);
#define E4ARDUINO_PUSH_32_ET3_MASK             0xffff0fff
#define E4ARDUINO_PUSH_32_ET3_PATTERN          0xf84d0d04
#define PROC_E4ARDUINO_PUSH_32_ET3(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PUSH_32_ET3_MASK)                     \
                    == E4ARDUINO_PUSH_32_ET3_PATTERN)) {                    \
                push_32_et3(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x0000f000) >> 12));                       \
            }                                                               \
        } while(0)

extern void str_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
#define E4ARDUINO_STR_REG_32_ET2_MASK          0xfff00fc0
#define E4ARDUINO_STR_REG_32_ET2_PATTERN       0xf8400000
#define PROC_E4ARDUINO_STR_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_STR_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_STR_REG_32_ET2_PATTERN)) {                 \
                str_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.9
 *   Load byte, memory hints
 */

extern void ldrb_lit_32_et1(void *ctx, word u, word rt, word rmm12);
#define E4ARDUINO_LDRB_LIT_32_ET1_MASK         0xff7f0000
#define E4ARDUINO_LDRB_LIT_32_ET1_PATTERN      0xf81f0000
#define PROC_E4ARDUINO_LDRB_LIT_32_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRB_LIT_32_ET1_MASK)                 \
                    == E4ARDUINO_LDRB_LIT_32_ET1_PATTERN)) {                \
                ldrb_lit_32_et1(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrb_imm_32_et2(void *ctx, word rn, word rt, word imm12);
#define E4ARDUINO_LDRB_IMM_32_ET2_MASK         0xfff00000
#define E4ARDUINO_LDRB_IMM_32_ET2_PATTERN      0xf8900000
#define PROC_E4ARDUINO_LDRB_IMM_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRB_IMM_32_ET2_MASK)                 \
                    == E4ARDUINO_LDRB_IMM_32_ET2_PATTERN)) {                \
                ldrb_imm_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrb_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
#define E4ARDUINO_LDRB_IMM_32_ET3_MASK         0xfff00800
#define E4ARDUINO_LDRB_IMM_32_ET3_PATTERN      0xf8100800
#define PROC_E4ARDUINO_LDRB_IMM_32_ET3(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRB_IMM_32_ET3_MASK)                 \
                    == E4ARDUINO_LDRB_IMM_32_ET3_PATTERN)) {                \
                ldrb_imm_32_et3(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000400) >> 10),                        \
                    ((OP32(op) & 0x00000200) >> 9),                         \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrbt_32_et1(void *ctx, word rn, word rt, word imm8);
#define E4ARDUINO_LDRBT_32_ET1_MASK            0xfff00f00
#define E4ARDUINO_LDRBT_32_ET1_PATTERN         0xf8100e00
#define PROC_E4ARDUINO_LDRBT_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRBT_32_ET1_MASK)                    \
                    == E4ARDUINO_LDRBT_32_ET1_PATTERN)) {                   \
                ldrbt_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrb_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
#define E4ARDUINO_LDRB_REG_32_ET2_MASK         0xfff00fc0
#define E4ARDUINO_LDRB_REG_32_ET2_PATTERN      0xf8100000
#define PROC_E4ARDUINO_LDRB_REG_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRB_REG_32_ET2_MASK)                 \
                    == E4ARDUINO_LDRB_REG_32_ET2_PATTERN)) {                \
                ldrb_reg_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsb_lit_32_et1(void *ctx, word u, word rt, word imm12);
#define E4ARDUINO_LDRSB_LIT_32_ET1_MASK        0xff7f0000
#define E4ARDUINO_LDRSB_LIT_32_ET1_PATTERN     0xf91f0000
#define PROC_E4ARDUINO_LDRSB_LIT_32_ET1(ctx, op)                            \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSB_LIT_32_ET1_MASK)                \
                    == E4ARDUINO_LDRSB_LIT_32_ET1_PATTERN)) {               \
                ldrsb_lit_32_et1(                                           \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsb_imm_32_et1(void *ctx, word rn, word rt, word imm12);
#define E4ARDUINO_LDRSB_IMM_32_ET1_MASK        0xfff00000
#define E4ARDUINO_LDRSB_IMM_32_ET1_PATTERN     0xf9900000
#define PROC_E4ARDUINO_LDRSB_IMM_32_ET1(ctx, op)                            \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSB_IMM_32_ET1_MASK)                \
                    == E4ARDUINO_LDRSB_IMM_32_ET1_PATTERN)) {               \
                ldrsb_imm_32_et1(                                           \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsb_imm_32_et2(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
#define E4ARDUINO_LDRSB_IMM_32_ET2_MASK        0xfff00800
#define E4ARDUINO_LDRSB_IMM_32_ET2_PATTERN     0xf9100800
#define PROC_E4ARDUINO_LDRSB_IMM_32_ET2(ctx, op)                            \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSB_IMM_32_ET2_MASK)                \
                    == E4ARDUINO_LDRSB_IMM_32_ET2_PATTERN)) {               \
                ldrsb_imm_32_et2(                                           \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000400) >> 10),                        \
                    ((OP32(op) & 0x00000200) >> 9),                         \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsbt_32_et1(void *ctx, word rn, word rt, word imm8);
#define E4ARDUINO_LDRSBT_32_ET1_MASK           0xfff00f00
#define E4ARDUINO_LDRSBT_32_ET1_PATTERN        0xf9100e00
#define PROC_E4ARDUINO_LDRSBT_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSBT_32_ET1_MASK)                   \
                    == E4ARDUINO_LDRSBT_32_ET1_PATTERN)) {                  \
                ldrsbt_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsb_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
#define E4ARDUINO_LDRSB_REG_32_ET2_MASK        0xfff00fc0
#define E4ARDUINO_LDRSB_REG_32_ET2_PATTERN     0xf9100000
#define PROC_E4ARDUINO_LDRSB_REG_32_ET2(ctx, op)                            \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSB_REG_32_ET2_MASK)                \
                    == E4ARDUINO_LDRSB_REG_32_ET2_PATTERN)) {               \
                ldrsb_reg_32_et2(                                           \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void pld_lit_32_et1(void *ctx, word u, word imm12);
#define E4ARDUINO_PLD_LIT_32_ET1_MASK          0xff7ff000
#define E4ARDUINO_PLD_LIT_32_ET1_PATTERN       0xf91ff000
#define PROC_E4ARDUINO_PLD_LIT_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PLD_LIT_32_ET1_MASK)                  \
                    == E4ARDUINO_PLD_LIT_32_ET1_PATTERN)) {                 \
                pld_lit_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void pld_imm_32_et1(void *ctx, word rn, word imm12);
#define E4ARDUINO_PLD_IMM_32_ET1_MASK          0xfff0f000
#define E4ARDUINO_PLD_IMM_32_ET1_PATTERN       0xf990f000
#define PROC_E4ARDUINO_PLD_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PLD_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_PLD_IMM_32_ET1_PATTERN)) {                 \
                pld_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void pld_imm_32_et2(void *ctx, word rn, word imm8);
#define E4ARDUINO_PLD_IMM_32_ET2_MASK          0xfff0ff00
#define E4ARDUINO_PLD_IMM_32_ET2_PATTERN       0xf910fc00
#define PROC_E4ARDUINO_PLD_IMM_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PLD_IMM_32_ET2_MASK)                  \
                    == E4ARDUINO_PLD_IMM_32_ET2_PATTERN)) {                 \
                pld_imm_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void pld_reg_32_et1(void *ctx, word rn, word sft, word rm);
#define E4ARDUINO_PLD_REG_32_ET1_MASK          0xfff0ffc0
#define E4ARDUINO_PLD_REG_32_ET1_PATTERN       0xf910f000
#define PROC_E4ARDUINO_PLD_REG_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PLD_REG_32_ET1_MASK)                  \
                    == E4ARDUINO_PLD_REG_32_ET1_PATTERN)) {                 \
                pld_reg_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void pli_imm_32_et1(void *ctx, word rn, word imm12);
#define E4ARDUINO_PLI_IMM_32_ET1_MASK          0xfff0f000
#define E4ARDUINO_PLI_IMM_32_ET1_PATTERN       0xf990f000
#define PROC_E4ARDUINO_PLI_IMM_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PLI_IMM_32_ET1_MASK)                  \
                    == E4ARDUINO_PLI_IMM_32_ET1_PATTERN)) {                 \
                pli_imm_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void pli_imm_32_et2(void *ctx, word rn, word imm8);
#define E4ARDUINO_PLI_IMM_32_ET2_MASK          0xfff0ff00
#define E4ARDUINO_PLI_IMM_32_ET2_PATTERN       0xf910fc00
#define PROC_E4ARDUINO_PLI_IMM_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PLI_IMM_32_ET2_MASK)                  \
                    == E4ARDUINO_PLI_IMM_32_ET2_PATTERN)) {                 \
                pli_imm_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void pli_lit_32_et3(void *ctx, word u, word imm12);
#define E4ARDUINO_PLI_LIT_32_ET3_MASK          0xff7ff000
#define E4ARDUINO_PLI_LIT_32_ET3_PATTERN       0xf91ff000
#define PROC_E4ARDUINO_PLI_LIT_32_ET3(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PLI_LIT_32_ET3_MASK)                  \
                    == E4ARDUINO_PLI_LIT_32_ET3_PATTERN)) {                 \
                pli_lit_32_et3(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void pli_reg_32_et1(void *ctx, word rn, word imm2, word rm);
#define E4ARDUINO_PLI_REG_32_ET1_MASK          0xfff0ffc0
#define E4ARDUINO_PLI_REG_32_ET1_PATTERN       0xf910f000
#define PROC_E4ARDUINO_PLI_REG_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_PLI_REG_32_ET1_MASK)                  \
                    == E4ARDUINO_PLI_REG_32_ET1_PATTERN)) {                 \
                pli_reg_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.8
 *   Load halfword, memory hints
 */

extern void ldrh_lit_32_et1(void *ctx, word u, word rt, word imm12);
#define E4ARDUINO_LDRH_LIT_32_ET1_MASK         0xff7f0000
#define E4ARDUINO_LDRH_LIT_32_ET1_PATTERN      0xf83f0000
#define PROC_E4ARDUINO_LDRH_LIT_32_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRH_LIT_32_ET1_MASK)                 \
                    == E4ARDUINO_LDRH_LIT_32_ET1_PATTERN)) {                \
                ldrh_lit_32_et1(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrh_imm_32_et2(void *ctx, word rn, word rt, word imm12);
#define E4ARDUINO_LDRH_IMM_32_ET2_MASK         0xfff00000
#define E4ARDUINO_LDRH_IMM_32_ET2_PATTERN      0xf8b00000
#define PROC_E4ARDUINO_LDRH_IMM_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRH_IMM_32_ET2_MASK)                 \
                    == E4ARDUINO_LDRH_IMM_32_ET2_PATTERN)) {                \
                ldrh_imm_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrh_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
#define E4ARDUINO_LDRH_IMM_32_ET3_MASK         0xfff00800
#define E4ARDUINO_LDRH_IMM_32_ET3_PATTERN      0xf8300800
#define PROC_E4ARDUINO_LDRH_IMM_32_ET3(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRH_IMM_32_ET3_MASK)                 \
                    == E4ARDUINO_LDRH_IMM_32_ET3_PATTERN)) {                \
                ldrh_imm_32_et3(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000400) >> 10),                        \
                    ((OP32(op) & 0x00000200) >> 9),                         \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrh_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
#define E4ARDUINO_LDRH_REG_32_ET2_MASK         0xfff00fc0
#define E4ARDUINO_LDRH_REG_32_ET2_PATTERN      0xf8300000
#define PROC_E4ARDUINO_LDRH_REG_32_ET2(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRH_REG_32_ET2_MASK)                 \
                    == E4ARDUINO_LDRH_REG_32_ET2_PATTERN)) {                \
                ldrh_reg_32_et2(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrht_32_et1(void *ctx, word rn, word rt, word imm8);
#define E4ARDUINO_LDRHT_32_ET1_MASK            0xfff00f00
#define E4ARDUINO_LDRHT_32_ET1_PATTERN         0xf8300e00
#define PROC_E4ARDUINO_LDRHT_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRHT_32_ET1_MASK)                    \
                    == E4ARDUINO_LDRHT_32_ET1_PATTERN)) {                   \
                ldrht_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsh_imm_32_et1(void *ctx, word rn, word rt, word imm12);
#define E4ARDUINO_LDRSH_IMM_32_ET1_MASK        0xfff00000
#define E4ARDUINO_LDRSH_IMM_32_ET1_PATTERN     0xf9b00000
#define PROC_E4ARDUINO_LDRSH_IMM_32_ET1(ctx, op)                            \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSH_IMM_32_ET1_MASK)                \
                    == E4ARDUINO_LDRSH_IMM_32_ET1_PATTERN)) {               \
                ldrsh_imm_32_et1(                                           \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsh_imm_32_et2(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
#define E4ARDUINO_LDRSH_IMM_32_ET2_MASK        0xfff00800
#define E4ARDUINO_LDRSH_IMM_32_ET2_PATTERN     0xf9300800
#define PROC_E4ARDUINO_LDRSH_IMM_32_ET2(ctx, op)                            \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSH_IMM_32_ET2_MASK)                \
                    == E4ARDUINO_LDRSH_IMM_32_ET2_PATTERN)) {               \
                ldrsh_imm_32_et2(                                           \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000400) >> 10),                        \
                    ((OP32(op) & 0x00000200) >> 9),                         \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsh_lit_32_et1(void *ctx, word u, word rt, word imm12);
#define E4ARDUINO_LDRSH_LIT_32_ET1_MASK        0xff7f0000
#define E4ARDUINO_LDRSH_LIT_32_ET1_PATTERN     0xf93f0000
#define PROC_E4ARDUINO_LDRSH_LIT_32_ET1(ctx, op)                            \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSH_LIT_32_ET1_MASK)                \
                    == E4ARDUINO_LDRSH_LIT_32_ET1_PATTERN)) {               \
                ldrsh_lit_32_et1(                                           \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsh_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
#define E4ARDUINO_LDRSH_REG_32_ET2_MASK        0xfff00fc0
#define E4ARDUINO_LDRSH_REG_32_ET2_PATTERN     0xf9300000
#define PROC_E4ARDUINO_LDRSH_REG_32_ET2(ctx, op)                            \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSH_REG_32_ET2_MASK)                \
                    == E4ARDUINO_LDRSH_REG_32_ET2_PATTERN)) {               \
                ldrsh_reg_32_et2(                                           \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldrsht_32_et1(void *ctx, word rn, word rt, word imm8);
#define E4ARDUINO_LDRSHT_32_ET1_MASK           0xfff00f00
#define E4ARDUINO_LDRSHT_32_ET1_PATTERN        0xf9300e00
#define PROC_E4ARDUINO_LDRSHT_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRSHT_32_ET1_MASK)                   \
                    == E4ARDUINO_LDRSHT_32_ET1_PATTERN)) {                  \
                ldrsht_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.7
 *   Load word
 */

extern void ldr_imm_32_et3(void *ctx, word rn, word rt, word imm12);
#define E4ARDUINO_LDR_IMM_32_ET3_MASK          0xfff00000
#define E4ARDUINO_LDR_IMM_32_ET3_PATTERN       0xf8d00000
#define PROC_E4ARDUINO_LDR_IMM_32_ET3(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDR_IMM_32_ET3_MASK)                  \
                    == E4ARDUINO_LDR_IMM_32_ET3_PATTERN)) {                 \
                ldr_imm_32_et3(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldr_imm_32_et4(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
#define E4ARDUINO_LDR_IMM_32_ET4_MASK          0xfff00800
#define E4ARDUINO_LDR_IMM_32_ET4_PATTERN       0xf8500800
#define PROC_E4ARDUINO_LDR_IMM_32_ET4(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDR_IMM_32_ET4_MASK)                  \
                    == E4ARDUINO_LDR_IMM_32_ET4_PATTERN)) {                 \
                ldr_imm_32_et4(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000400) >> 10),                        \
                    ((OP32(op) & 0x00000200) >> 9),                         \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void pop_32_et3(void *ctx, word rt);
#define E4ARDUINO_POP_32_ET3_MASK              0xffff0fff
#define E4ARDUINO_POP_32_ET3_PATTERN           0xf85d0b04
#define PROC_E4ARDUINO_POP_32_ET3(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_POP_32_ET3_MASK)                      \
                    == E4ARDUINO_POP_32_ET3_PATTERN)) {                     \
                pop_32_et3(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x0000f000) >> 12));                       \
            }                                                               \
        } while(0)

extern void ldrt_32_et1(void *ctx, word rn, word rt, word imm8);
#define E4ARDUINO_LDRT_32_ET1_MASK             0xfff00f00
#define E4ARDUINO_LDRT_32_ET1_PATTERN          0xf8500e00
#define PROC_E4ARDUINO_LDRT_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDRT_32_ET1_MASK)                     \
                    == E4ARDUINO_LDRT_32_ET1_PATTERN)) {                    \
                ldrt_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldr_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
#define E4ARDUINO_LDR_REG_32_ET2_MASK          0xfff00fc0
#define E4ARDUINO_LDR_REG_32_ET2_PATTERN       0xf8500000
#define PROC_E4ARDUINO_LDR_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDR_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_LDR_REG_32_ET2_PATTERN)) {                 \
                ldr_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ldr_lit_32_et2(void *ctx, word u, word rt, word imm12);
#define E4ARDUINO_LDR_LIT_32_ET2_MASK          0xff7f0000
#define E4ARDUINO_LDR_LIT_32_ET2_PATTERN       0xf85f0000
#define PROC_E4ARDUINO_LDR_LIT_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LDR_LIT_32_ET2_MASK)                  \
                    == E4ARDUINO_LDR_LIT_32_ET2_PATTERN)) {                 \
                ldr_lit_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000fff) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.12
 *   Data processing (register)
 */

extern void lsl_reg_32_et2(void *ctx, word s, word rn, word rd, word rm);
#define E4ARDUINO_LSL_REG_32_ET2_MASK          0xffe0f0f0
#define E4ARDUINO_LSL_REG_32_ET2_PATTERN       0xfa00f000
#define PROC_E4ARDUINO_LSL_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LSL_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_LSL_REG_32_ET2_PATTERN)) {                 \
                lsl_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void lsr_reg_32_et2(void *ctx, word s, word rn, word rd, word rm);
#define E4ARDUINO_LSR_REG_32_ET2_MASK          0xffe0f0f0
#define E4ARDUINO_LSR_REG_32_ET2_PATTERN       0xfa20f000
#define PROC_E4ARDUINO_LSR_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_LSR_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_LSR_REG_32_ET2_PATTERN)) {                 \
                lsr_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void asr_reg_32_et2(void *ctx, word s, word rn, word rd, word rm);
#define E4ARDUINO_ASR_REG_32_ET2_MASK          0xffe0f0f0
#define E4ARDUINO_ASR_REG_32_ET2_PATTERN       0xfa40f000
#define PROC_E4ARDUINO_ASR_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ASR_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_ASR_REG_32_ET2_PATTERN)) {                 \
                asr_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ror_reg_32_et2(void *ctx, word s, word rn, word rd, word rm);
#define E4ARDUINO_ROR_REG_32_ET2_MASK          0xffe0f0f0
#define E4ARDUINO_ROR_REG_32_ET2_PATTERN       0xfa60f000
#define PROC_E4ARDUINO_ROR_REG_32_ET2(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_ROR_REG_32_ET2_MASK)                  \
                    == E4ARDUINO_ROR_REG_32_ET2_PATTERN)) {                 \
                ror_reg_32_et2(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sxtah_32_et1(void *ctx, word rn, word rd, word rot, word rm);
#define E4ARDUINO_SXTAH_32_ET1_MASK            0xfff0f0c0
#define E4ARDUINO_SXTAH_32_ET1_PATTERN         0xfa00f080
#define PROC_E4ARDUINO_SXTAH_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SXTAH_32_ET1_MASK)                    \
                    == E4ARDUINO_SXTAH_32_ET1_PATTERN)) {                   \
                sxtah_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sxth_32_et2(void *ctx, word rd, word rot, word rm);
#define E4ARDUINO_SXTH_32_ET2_MASK             0xfffff0c0
#define E4ARDUINO_SXTH_32_ET2_PATTERN          0xfa0ff080
#define PROC_E4ARDUINO_SXTH_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SXTH_32_ET2_MASK)                     \
                    == E4ARDUINO_SXTH_32_ET2_PATTERN)) {                    \
                sxth_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uxtah_32_et1(void *ctx, word rn, word rd, word rot, word rm);
#define E4ARDUINO_UXTAH_32_ET1_MASK            0xfff0f0c0
#define E4ARDUINO_UXTAH_32_ET1_PATTERN         0xfa10f080
#define PROC_E4ARDUINO_UXTAH_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UXTAH_32_ET1_MASK)                    \
                    == E4ARDUINO_UXTAH_32_ET1_PATTERN)) {                   \
                uxtah_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uxth_32_et2(void *ctx, word rd, word rot, word rm);
#define E4ARDUINO_UXTH_32_ET2_MASK             0xfffff0c0
#define E4ARDUINO_UXTH_32_ET2_PATTERN          0xfa1ff080
#define PROC_E4ARDUINO_UXTH_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UXTH_32_ET2_MASK)                     \
                    == E4ARDUINO_UXTH_32_ET2_PATTERN)) {                    \
                uxth_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sxtab16_32_et1(void *ctx, word rn, word rd, word rot, word rm);
#define E4ARDUINO_SXTAB16_32_ET1_MASK          0xfff0f0c0
#define E4ARDUINO_SXTAB16_32_ET1_PATTERN       0xfa20f080
#define PROC_E4ARDUINO_SXTAB16_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SXTAB16_32_ET1_MASK)                  \
                    == E4ARDUINO_SXTAB16_32_ET1_PATTERN)) {                 \
                sxtab16_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sxtb16_32_et1(void *ctx, word rd, word rot, word rm);
#define E4ARDUINO_SXTB16_32_ET1_MASK           0xfffff0c0
#define E4ARDUINO_SXTB16_32_ET1_PATTERN        0xfa2ff080
#define PROC_E4ARDUINO_SXTB16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SXTB16_32_ET1_MASK)                   \
                    == E4ARDUINO_SXTB16_32_ET1_PATTERN)) {                  \
                sxtb16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uxtab16_32_et1(void *ctx, word rn, word rd, word rot, word rm);
#define E4ARDUINO_UXTAB16_32_ET1_MASK          0xfff0f0c0
#define E4ARDUINO_UXTAB16_32_ET1_PATTERN       0xfa30f080
#define PROC_E4ARDUINO_UXTAB16_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UXTAB16_32_ET1_MASK)                  \
                    == E4ARDUINO_UXTAB16_32_ET1_PATTERN)) {                 \
                uxtab16_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uxtb16_32_et1(void *ctx, word rd, word rot, word rm);
#define E4ARDUINO_UXTB16_32_ET1_MASK           0xfffff0c0
#define E4ARDUINO_UXTB16_32_ET1_PATTERN        0xfa3ff080
#define PROC_E4ARDUINO_UXTB16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UXTB16_32_ET1_MASK)                   \
                    == E4ARDUINO_UXTB16_32_ET1_PATTERN)) {                  \
                uxtb16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sxtab_32_et1(void *ctx, word rn, word rd, word rot, word rm);
#define E4ARDUINO_SXTAB_32_ET1_MASK            0xfff0f0c0
#define E4ARDUINO_SXTAB_32_ET1_PATTERN         0xfa40f080
#define PROC_E4ARDUINO_SXTAB_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SXTAB_32_ET1_MASK)                    \
                    == E4ARDUINO_SXTAB_32_ET1_PATTERN)) {                   \
                sxtab_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sxtb_32_et2(void *ctx, word rd, word rot, word rm);
#define E4ARDUINO_SXTB_32_ET2_MASK             0xfffff0c0
#define E4ARDUINO_SXTB_32_ET2_PATTERN          0xfa4ff080
#define PROC_E4ARDUINO_SXTB_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SXTB_32_ET2_MASK)                     \
                    == E4ARDUINO_SXTB_32_ET2_PATTERN)) {                    \
                sxtb_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uxtab_32_et1(void *ctx, word rn, word rd, word rot, word rm);
#define E4ARDUINO_UXTAB_32_ET1_MASK            0xfff0f0c0
#define E4ARDUINO_UXTAB_32_ET1_PATTERN         0xfa50f080
#define PROC_E4ARDUINO_UXTAB_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UXTAB_32_ET1_MASK)                    \
                    == E4ARDUINO_UXTAB_32_ET1_PATTERN)) {                   \
                uxtab_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uxtb_32_et2(void *ctx, word rd, word rot, word rm);
#define E4ARDUINO_UXTB_32_ET2_MASK             0xfffff0c0
#define E4ARDUINO_UXTB_32_ET2_PATTERN          0xfa5ff080
#define PROC_E4ARDUINO_UXTB_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UXTB_32_ET2_MASK)                     \
                    == E4ARDUINO_UXTB_32_ET2_PATTERN)) {                    \
                uxtb_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x00000030) >> 4),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.13
 *   Parallel addition and subtraction, signed
 */

extern void sadd16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SADD16_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SADD16_32_ET1_PATTERN        0xfa90f000
#define PROC_E4ARDUINO_SADD16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SADD16_32_ET1_MASK)                   \
                    == E4ARDUINO_SADD16_32_ET1_PATTERN)) {                  \
                sadd16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sasx_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SASX_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_SASX_32_ET1_PATTERN          0xfaa0f000
#define PROC_E4ARDUINO_SASX_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SASX_32_ET1_MASK)                     \
                    == E4ARDUINO_SASX_32_ET1_PATTERN)) {                    \
                sasx_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ssax_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SSAX_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_SSAX_32_ET1_PATTERN          0xfae0f000
#define PROC_E4ARDUINO_SSAX_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SSAX_32_ET1_MASK)                     \
                    == E4ARDUINO_SSAX_32_ET1_PATTERN)) {                    \
                ssax_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ssub16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SSUB16_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SSUB16_32_ET1_PATTERN        0xfad0f000
#define PROC_E4ARDUINO_SSUB16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SSUB16_32_ET1_MASK)                   \
                    == E4ARDUINO_SSUB16_32_ET1_PATTERN)) {                  \
                ssub16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sadd8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SADD8_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_SADD8_32_ET1_PATTERN         0xfa80f000
#define PROC_E4ARDUINO_SADD8_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SADD8_32_ET1_MASK)                    \
                    == E4ARDUINO_SADD8_32_ET1_PATTERN)) {                   \
                sadd8_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void ssub8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SSUB8_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_SSUB8_32_ET1_PATTERN         0xfac0f000
#define PROC_E4ARDUINO_SSUB8_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SSUB8_32_ET1_MASK)                    \
                    == E4ARDUINO_SSUB8_32_ET1_PATTERN)) {                   \
                ssub8_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qadd16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QADD16_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_QADD16_32_ET1_PATTERN        0xfa90f010
#define PROC_E4ARDUINO_QADD16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QADD16_32_ET1_MASK)                   \
                    == E4ARDUINO_QADD16_32_ET1_PATTERN)) {                  \
                qadd16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qasx_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QASX_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_QASX_32_ET1_PATTERN          0xfaa0f010
#define PROC_E4ARDUINO_QASX_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QASX_32_ET1_MASK)                     \
                    == E4ARDUINO_QASX_32_ET1_PATTERN)) {                    \
                qasx_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qsax_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QSAX_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_QSAX_32_ET1_PATTERN          0xfae0f010
#define PROC_E4ARDUINO_QSAX_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QSAX_32_ET1_MASK)                     \
                    == E4ARDUINO_QSAX_32_ET1_PATTERN)) {                    \
                qsax_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qsub16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QSUB16_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_QSUB16_32_ET1_PATTERN        0xfad0f010
#define PROC_E4ARDUINO_QSUB16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QSUB16_32_ET1_MASK)                   \
                    == E4ARDUINO_QSUB16_32_ET1_PATTERN)) {                  \
                qsub16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qadd8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QADD8_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_QADD8_32_ET1_PATTERN         0xfa80f010
#define PROC_E4ARDUINO_QADD8_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QADD8_32_ET1_MASK)                    \
                    == E4ARDUINO_QADD8_32_ET1_PATTERN)) {                   \
                qadd8_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qsub8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QSUB8_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_QSUB8_32_ET1_PATTERN         0xfac0f010
#define PROC_E4ARDUINO_QSUB8_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QSUB8_32_ET1_MASK)                    \
                    == E4ARDUINO_QSUB8_32_ET1_PATTERN)) {                   \
                qsub8_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void shadd16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SHADD16_32_ET1_MASK          0xfff0f0f0
#define E4ARDUINO_SHADD16_32_ET1_PATTERN       0xfa90f020
#define PROC_E4ARDUINO_SHADD16_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SHADD16_32_ET1_MASK)                  \
                    == E4ARDUINO_SHADD16_32_ET1_PATTERN)) {                 \
                shadd16_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uadd16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UADD16_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_UADD16_32_ET1_PATTERN        0xfa90f040
#define PROC_E4ARDUINO_UADD16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UADD16_32_ET1_MASK)                   \
                    == E4ARDUINO_UADD16_32_ET1_PATTERN)) {                  \
                uadd16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uqadd16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UQADD16_32_ET1_MASK          0xfff0f0f0
#define E4ARDUINO_UQADD16_32_ET1_PATTERN       0xfa90f050
#define PROC_E4ARDUINO_UQADD16_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UQADD16_32_ET1_MASK)                  \
                    == E4ARDUINO_UQADD16_32_ET1_PATTERN)) {                 \
                uqadd16_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uhadd16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UHADD16_32_ET1_MASK          0xfff0f0f0
#define E4ARDUINO_UHADD16_32_ET1_PATTERN       0xfa90f060
#define PROC_E4ARDUINO_UHADD16_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UHADD16_32_ET1_MASK)                  \
                    == E4ARDUINO_UHADD16_32_ET1_PATTERN)) {                 \
                uhadd16_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void rev_32_et2(void *ctx, word rm, word rd, word rm2);
#define E4ARDUINO_REV_32_ET2_MASK              0xfff0f0f0
#define E4ARDUINO_REV_32_ET2_PATTERN           0xfa90f080
#define PROC_E4ARDUINO_REV_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_REV_32_ET2_MASK)                      \
                    == E4ARDUINO_REV_32_ET2_PATTERN)) {                     \
                rev_32_et2(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void rev16_32_et2(void *ctx, word rm, word rd, word rm2);
#define E4ARDUINO_REV16_32_ET2_MASK            0xfff0f0f0
#define E4ARDUINO_REV16_32_ET2_PATTERN         0xfa90f090
#define PROC_E4ARDUINO_REV16_32_ET2(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_REV16_32_ET2_MASK)                    \
                    == E4ARDUINO_REV16_32_ET2_PATTERN)) {                   \
                rev16_32_et2(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void rbit_32_et1(void *ctx, word rm, word rd, word rm2);
#define E4ARDUINO_RBIT_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_RBIT_32_ET1_PATTERN          0xfa90f0a0
#define PROC_E4ARDUINO_RBIT_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_RBIT_32_ET1_MASK)                     \
                    == E4ARDUINO_RBIT_32_ET1_PATTERN)) {                    \
                rbit_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void revsh_32_et2(void *ctx, word rm, word rd, word rm2);
#define E4ARDUINO_REVSH_32_ET2_MASK            0xfff0f0f0
#define E4ARDUINO_REVSH_32_ET2_PATTERN         0xfa90f0b0
#define PROC_E4ARDUINO_REVSH_32_ET2(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_REVSH_32_ET2_MASK)                    \
                    == E4ARDUINO_REVSH_32_ET2_PATTERN)) {                   \
                revsh_32_et2(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void shasx_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SHASX_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_SHASX_32_ET1_PATTERN         0xfaa0f020
#define PROC_E4ARDUINO_SHASX_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SHASX_32_ET1_MASK)                    \
                    == E4ARDUINO_SHASX_32_ET1_PATTERN)) {                   \
                shasx_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uasx_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UASX_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_UASX_32_ET1_PATTERN          0xfaa0f040
#define PROC_E4ARDUINO_UASX_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UASX_32_ET1_MASK)                     \
                    == E4ARDUINO_UASX_32_ET1_PATTERN)) {                    \
                uasx_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uqasx_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UQASX_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_UQASX_32_ET1_PATTERN         0xfaa0f050
#define PROC_E4ARDUINO_UQASX_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UQASX_32_ET1_MASK)                    \
                    == E4ARDUINO_UQASX_32_ET1_PATTERN)) {                   \
                uqasx_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uhasx_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UHASX_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_UHASX_32_ET1_PATTERN         0xfaa0f060
#define PROC_E4ARDUINO_UHASX_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UHASX_32_ET1_MASK)                    \
                    == E4ARDUINO_UHASX_32_ET1_PATTERN)) {                   \
                uhasx_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sel_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SEL_32_ET1_MASK              0xfff0f0f0
#define E4ARDUINO_SEL_32_ET1_PATTERN           0xfaa0f080
#define PROC_E4ARDUINO_SEL_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SEL_32_ET1_MASK)                      \
                    == E4ARDUINO_SEL_32_ET1_PATTERN)) {                     \
                sel_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void clz_32_et1(void *ctx, word rm, word rd, word rm2);
#define E4ARDUINO_CLZ_32_ET1_MASK              0xfff0f0f0
#define E4ARDUINO_CLZ_32_ET1_PATTERN           0xfab0f080
#define PROC_E4ARDUINO_CLZ_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_CLZ_32_ET1_MASK)                      \
                    == E4ARDUINO_CLZ_32_ET1_PATTERN)) {                     \
                clz_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void shsax_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SHSAX_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_SHSAX_32_ET1_PATTERN         0xfae0f020
#define PROC_E4ARDUINO_SHSAX_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SHSAX_32_ET1_MASK)                    \
                    == E4ARDUINO_SHSAX_32_ET1_PATTERN)) {                   \
                shsax_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void usax_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_USAX_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_USAX_32_ET1_PATTERN          0xfae0f040
#define PROC_E4ARDUINO_USAX_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_USAX_32_ET1_MASK)                     \
                    == E4ARDUINO_USAX_32_ET1_PATTERN)) {                    \
                usax_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uqsax_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UQSAX_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_UQSAX_32_ET1_PATTERN         0xfae0f050
#define PROC_E4ARDUINO_UQSAX_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UQSAX_32_ET1_MASK)                    \
                    == E4ARDUINO_UQSAX_32_ET1_PATTERN)) {                   \
                uqsax_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uhsax_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UHSAX_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_UHSAX_32_ET1_PATTERN         0xfae0f060
#define PROC_E4ARDUINO_UHSAX_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UHSAX_32_ET1_MASK)                    \
                    == E4ARDUINO_UHSAX_32_ET1_PATTERN)) {                   \
                uhsax_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void shsub16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SHSUB16_32_ET1_MASK          0xfff0f0f0
#define E4ARDUINO_SHSUB16_32_ET1_PATTERN       0xfad0f020
#define PROC_E4ARDUINO_SHSUB16_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SHSUB16_32_ET1_MASK)                  \
                    == E4ARDUINO_SHSUB16_32_ET1_PATTERN)) {                 \
                shsub16_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void usub16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_USUB16_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_USUB16_32_ET1_PATTERN        0xfad0f040
#define PROC_E4ARDUINO_USUB16_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_USUB16_32_ET1_MASK)                   \
                    == E4ARDUINO_USUB16_32_ET1_PATTERN)) {                  \
                usub16_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uqsub16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UQSUB16_32_ET1_MASK          0xfff0f0f0
#define E4ARDUINO_UQSUB16_32_ET1_PATTERN       0xfad0f050
#define PROC_E4ARDUINO_UQSUB16_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UQSUB16_32_ET1_MASK)                  \
                    == E4ARDUINO_UQSUB16_32_ET1_PATTERN)) {                 \
                uqsub16_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uhsub16_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UHSUB16_32_ET1_MASK          0xfff0f0f0
#define E4ARDUINO_UHSUB16_32_ET1_PATTERN       0xfad0f060
#define PROC_E4ARDUINO_UHSUB16_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UHSUB16_32_ET1_MASK)                  \
                    == E4ARDUINO_UHSUB16_32_ET1_PATTERN)) {                 \
                uhsub16_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void shadd8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SHADD8_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SHADD8_32_ET1_PATTERN        0xfa80f020
#define PROC_E4ARDUINO_SHADD8_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SHADD8_32_ET1_MASK)                   \
                    == E4ARDUINO_SHADD8_32_ET1_PATTERN)) {                  \
                shadd8_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uadd8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UADD8_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_UADD8_32_ET1_PATTERN         0xfa80f040
#define PROC_E4ARDUINO_UADD8_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UADD8_32_ET1_MASK)                    \
                    == E4ARDUINO_UADD8_32_ET1_PATTERN)) {                   \
                uadd8_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uqadd8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UQADD8_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_UQADD8_32_ET1_PATTERN        0xfa80f050
#define PROC_E4ARDUINO_UQADD8_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UQADD8_32_ET1_MASK)                   \
                    == E4ARDUINO_UQADD8_32_ET1_PATTERN)) {                  \
                uqadd8_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uhadd8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UHADD8_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_UHADD8_32_ET1_PATTERN        0xfa80f060
#define PROC_E4ARDUINO_UHADD8_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UHADD8_32_ET1_MASK)                   \
                    == E4ARDUINO_UHADD8_32_ET1_PATTERN)) {                  \
                uhadd8_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qadd_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QADD_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_QADD_32_ET1_PATTERN          0xfa80f080
#define PROC_E4ARDUINO_QADD_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QADD_32_ET1_MASK)                     \
                    == E4ARDUINO_QADD_32_ET1_PATTERN)) {                    \
                qadd_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qdadd_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QDADD_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_QDADD_32_ET1_PATTERN         0xfa80f090
#define PROC_E4ARDUINO_QDADD_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QDADD_32_ET1_MASK)                    \
                    == E4ARDUINO_QDADD_32_ET1_PATTERN)) {                   \
                qdadd_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qsub_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QSUB_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_QSUB_32_ET1_PATTERN          0xfa80f0a0
#define PROC_E4ARDUINO_QSUB_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QSUB_32_ET1_MASK)                     \
                    == E4ARDUINO_QSUB_32_ET1_PATTERN)) {                    \
                qsub_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void qdsub_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_QDSUB_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_QDSUB_32_ET1_PATTERN         0xfa80f0b0
#define PROC_E4ARDUINO_QDSUB_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_QDSUB_32_ET1_MASK)                    \
                    == E4ARDUINO_QDSUB_32_ET1_PATTERN)) {                   \
                qdsub_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void shsub8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SHSUB8_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SHSUB8_32_ET1_PATTERN        0xfac0f020
#define PROC_E4ARDUINO_SHSUB8_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SHSUB8_32_ET1_MASK)                   \
                    == E4ARDUINO_SHSUB8_32_ET1_PATTERN)) {                  \
                shsub8_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void usub8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_USUB8_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_USUB8_32_ET1_PATTERN         0xfac0f040
#define PROC_E4ARDUINO_USUB8_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_USUB8_32_ET1_MASK)                    \
                    == E4ARDUINO_USUB8_32_ET1_PATTERN)) {                   \
                usub8_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uqsub8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UQSUB8_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_UQSUB8_32_ET1_PATTERN        0xfac0f050
#define PROC_E4ARDUINO_UQSUB8_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UQSUB8_32_ET1_MASK)                   \
                    == E4ARDUINO_UQSUB8_32_ET1_PATTERN)) {                  \
                uqsub8_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void uhsub8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UHSUB8_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_UHSUB8_32_ET1_PATTERN        0xfac0f060
#define PROC_E4ARDUINO_UHSUB8_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UHSUB8_32_ET1_MASK)                   \
                    == E4ARDUINO_UHSUB8_32_ET1_PATTERN)) {                  \
                uhsub8_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.16
 *   Multiply, multiply accumulate, and absolute difference
 */

extern void mla_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_MLA_32_ET1_MASK              0xfff000f0
#define E4ARDUINO_MLA_32_ET1_PATTERN           0xfb000000
#define PROC_E4ARDUINO_MLA_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MLA_32_ET1_MASK)                      \
                    == E4ARDUINO_MLA_32_ET1_PATTERN)) {                     \
                mla_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mul_32_et2(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_MUL_32_ET2_MASK              0xfff0f0f0
#define E4ARDUINO_MUL_32_ET2_PATTERN           0xfb00f000
#define PROC_E4ARDUINO_MUL_32_ET2(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MUL_32_ET2_MASK)                      \
                    == E4ARDUINO_MUL_32_ET2_PATTERN)) {                     \
                mul_32_et2(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void mls_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_MLS_32_ET1_MASK              0xfff000f0
#define E4ARDUINO_MLS_32_ET1_PATTERN           0xfb000010
#define PROC_E4ARDUINO_MLS_32_ET1(ctx, op)                                  \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_MLS_32_ET1_MASK)                      \
                    == E4ARDUINO_MLS_32_ET1_PATTERN)) {                     \
                mls_32_et1(                                                 \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlabb_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLABB_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLABB_32_ET1_PATTERN        0xfb100000
#define PROC_E4ARDUINO_SMLABB_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLABB_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLABB_32_ET1_PATTERN)) {                  \
                smlabb_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlabt_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLABT_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLABT_32_ET1_PATTERN        0xfb100010
#define PROC_E4ARDUINO_SMLABT_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLABT_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLABT_32_ET1_PATTERN)) {                  \
                smlabt_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlatb_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLATB_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLATB_32_ET1_PATTERN        0xfb100020
#define PROC_E4ARDUINO_SMLATB_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLATB_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLATB_32_ET1_PATTERN)) {                  \
                smlatb_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlatt_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLATT_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLATT_32_ET1_PATTERN        0xfb100030
#define PROC_E4ARDUINO_SMLATT_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLATT_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLATT_32_ET1_PATTERN)) {                  \
                smlatt_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smulbb_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMULBB_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SMULBB_32_ET1_PATTERN        0xfb10f000
#define PROC_E4ARDUINO_SMULBB_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMULBB_32_ET1_MASK)                   \
                    == E4ARDUINO_SMULBB_32_ET1_PATTERN)) {                  \
                smulbb_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smulbt_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMULBT_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SMULBT_32_ET1_PATTERN        0xfb10f010
#define PROC_E4ARDUINO_SMULBT_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMULBT_32_ET1_MASK)                   \
                    == E4ARDUINO_SMULBT_32_ET1_PATTERN)) {                  \
                smulbt_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smultb_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMULTB_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SMULTB_32_ET1_PATTERN        0xfb10f020
#define PROC_E4ARDUINO_SMULTB_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMULTB_32_ET1_MASK)                   \
                    == E4ARDUINO_SMULTB_32_ET1_PATTERN)) {                  \
                smultb_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smultt_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMULTT_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SMULTT_32_ET1_PATTERN        0xfb10f030
#define PROC_E4ARDUINO_SMULTT_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMULTT_32_ET1_MASK)                   \
                    == E4ARDUINO_SMULTT_32_ET1_PATTERN)) {                  \
                smultt_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlad_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLAD_32_ET1_MASK            0xfff000f0
#define E4ARDUINO_SMLAD_32_ET1_PATTERN         0xfb200000
#define PROC_E4ARDUINO_SMLAD_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLAD_32_ET1_MASK)                    \
                    == E4ARDUINO_SMLAD_32_ET1_PATTERN)) {                   \
                smlad_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smladx_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLADX_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLADX_32_ET1_PATTERN        0xfb200010
#define PROC_E4ARDUINO_SMLADX_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLADX_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLADX_32_ET1_PATTERN)) {                  \
                smladx_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smuad_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMUAD_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_SMUAD_32_ET1_PATTERN         0xfb20f000
#define PROC_E4ARDUINO_SMUAD_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMUAD_32_ET1_MASK)                    \
                    == E4ARDUINO_SMUAD_32_ET1_PATTERN)) {                   \
                smuad_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smuadx_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMUADX_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SMUADX_32_ET1_PATTERN        0xfb20f010
#define PROC_E4ARDUINO_SMUADX_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMUADX_32_ET1_MASK)                   \
                    == E4ARDUINO_SMUADX_32_ET1_PATTERN)) {                  \
                smuadx_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlawb_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLAWB_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLAWB_32_ET1_PATTERN        0xfb300000
#define PROC_E4ARDUINO_SMLAWB_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLAWB_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLAWB_32_ET1_PATTERN)) {                  \
                smlawb_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlawt_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLAWT_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLAWT_32_ET1_PATTERN        0xfb300010
#define PROC_E4ARDUINO_SMLAWT_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLAWT_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLAWT_32_ET1_PATTERN)) {                  \
                smlawt_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smulwb_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMULWB_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SMULWB_32_ET1_PATTERN        0xfb30f000
#define PROC_E4ARDUINO_SMULWB_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMULWB_32_ET1_MASK)                   \
                    == E4ARDUINO_SMULWB_32_ET1_PATTERN)) {                  \
                smulwb_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smulwt_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMULWT_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SMULWT_32_ET1_PATTERN        0xfb30f010
#define PROC_E4ARDUINO_SMULWT_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMULWT_32_ET1_MASK)                   \
                    == E4ARDUINO_SMULWT_32_ET1_PATTERN)) {                  \
                smulwt_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlsd_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLSD_32_ET1_MASK            0xfff000f0
#define E4ARDUINO_SMLSD_32_ET1_PATTERN         0xfb400000
#define PROC_E4ARDUINO_SMLSD_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLSD_32_ET1_MASK)                    \
                    == E4ARDUINO_SMLSD_32_ET1_PATTERN)) {                   \
                smlsd_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlsdx_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMLSDX_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLSDX_32_ET1_PATTERN        0xfb400010
#define PROC_E4ARDUINO_SMLSDX_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLSDX_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLSDX_32_ET1_PATTERN)) {                  \
                smlsdx_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smusd_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMUSD_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_SMUSD_32_ET1_PATTERN         0xfb40f000
#define PROC_E4ARDUINO_SMUSD_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMUSD_32_ET1_MASK)                    \
                    == E4ARDUINO_SMUSD_32_ET1_PATTERN)) {                   \
                smusd_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smusdx_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMUSDX_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SMUSDX_32_ET1_PATTERN        0xfb40f010
#define PROC_E4ARDUINO_SMUSDX_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMUSDX_32_ET1_MASK)                   \
                    == E4ARDUINO_SMUSDX_32_ET1_PATTERN)) {                  \
                smusdx_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smmla_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMMLA_32_ET1_MASK            0xfff000f0
#define E4ARDUINO_SMMLA_32_ET1_PATTERN         0xfb500000
#define PROC_E4ARDUINO_SMMLA_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMMLA_32_ET1_MASK)                    \
                    == E4ARDUINO_SMMLA_32_ET1_PATTERN)) {                   \
                smmla_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smmlar_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMMLAR_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMMLAR_32_ET1_PATTERN        0xfb500010
#define PROC_E4ARDUINO_SMMLAR_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMMLAR_32_ET1_MASK)                   \
                    == E4ARDUINO_SMMLAR_32_ET1_PATTERN)) {                  \
                smmlar_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smmul_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMMUL_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_SMMUL_32_ET1_PATTERN         0xfb50f000
#define PROC_E4ARDUINO_SMMUL_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMMUL_32_ET1_MASK)                    \
                    == E4ARDUINO_SMMUL_32_ET1_PATTERN)) {                   \
                smmul_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smmulr_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SMMULR_32_ET1_MASK           0xfff0f0f0
#define E4ARDUINO_SMMULR_32_ET1_PATTERN        0xfb50f010
#define PROC_E4ARDUINO_SMMULR_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMMULR_32_ET1_MASK)                   \
                    == E4ARDUINO_SMMULR_32_ET1_PATTERN)) {                  \
                smmulr_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smmls_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMMLS_32_ET1_MASK            0xfff000f0
#define E4ARDUINO_SMMLS_32_ET1_PATTERN         0xfb600000
#define PROC_E4ARDUINO_SMMLS_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMMLS_32_ET1_MASK)                    \
                    == E4ARDUINO_SMMLS_32_ET1_PATTERN)) {                   \
                smmls_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smmlsr_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_SMMLSR_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMMLSR_32_ET1_PATTERN        0xfb600010
#define PROC_E4ARDUINO_SMMLSR_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMMLSR_32_ET1_MASK)                   \
                    == E4ARDUINO_SMMLSR_32_ET1_PATTERN)) {                  \
                smmlsr_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void usad8_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_USAD8_32_ET1_MASK            0xfff0f0f0
#define E4ARDUINO_USAD8_32_ET1_PATTERN         0xfb70f000
#define PROC_E4ARDUINO_USAD8_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_USAD8_32_ET1_MASK)                    \
                    == E4ARDUINO_USAD8_32_ET1_PATTERN)) {                   \
                usad8_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void usada8_32_et1(void *ctx, word rn, word ra, word rd, word rm);
#define E4ARDUINO_USADA8_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_USADA8_32_ET1_PATTERN        0xfb700000
#define PROC_E4ARDUINO_USADA8_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_USADA8_32_ET1_MASK)                   \
                    == E4ARDUINO_USADA8_32_ET1_PATTERN)) {                  \
                usada8_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A5.3.17
 *   Long multiply, long multiply accumulate, and divide
 */

extern void smull_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMULL_32_ET1_MASK            0xfff000f0
#define E4ARDUINO_SMULL_32_ET1_PATTERN         0xfb800000
#define PROC_E4ARDUINO_SMULL_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMULL_32_ET1_MASK)                    \
                    == E4ARDUINO_SMULL_32_ET1_PATTERN)) {                   \
                smull_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void sdiv_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_SDIV_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_SDIV_32_ET1_PATTERN          0xfb90f0f0
#define PROC_E4ARDUINO_SDIV_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SDIV_32_ET1_MASK)                     \
                    == E4ARDUINO_SDIV_32_ET1_PATTERN)) {                    \
                sdiv_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void umull_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_UMULL_32_ET1_MASK            0xfff000f0
#define E4ARDUINO_UMULL_32_ET1_PATTERN         0xfba00000
#define PROC_E4ARDUINO_UMULL_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UMULL_32_ET1_MASK)                    \
                    == E4ARDUINO_UMULL_32_ET1_PATTERN)) {                   \
                umull_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void udiv_32_et1(void *ctx, word rn, word rd, word rm);
#define E4ARDUINO_UDIV_32_ET1_MASK             0xfff0f0f0
#define E4ARDUINO_UDIV_32_ET1_PATTERN          0xfbb0f0f0
#define PROC_E4ARDUINO_UDIV_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UDIV_32_ET1_MASK)                     \
                    == E4ARDUINO_UDIV_32_ET1_PATTERN)) {                    \
                udiv_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlal_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMLAL_32_ET1_MASK            0xfff000f0
#define E4ARDUINO_SMLAL_32_ET1_PATTERN         0xfbc00000
#define PROC_E4ARDUINO_SMLAL_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLAL_32_ET1_MASK)                    \
                    == E4ARDUINO_SMLAL_32_ET1_PATTERN)) {                   \
                smlal_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlalbb_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMLALBB_32_ET1_MASK          0xfff000f0
#define E4ARDUINO_SMLALBB_32_ET1_PATTERN       0xfbc00080
#define PROC_E4ARDUINO_SMLALBB_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLALBB_32_ET1_MASK)                  \
                    == E4ARDUINO_SMLALBB_32_ET1_PATTERN)) {                 \
                smlalbb_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlalbt_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMLALBT_32_ET1_MASK          0xfff000f0
#define E4ARDUINO_SMLALBT_32_ET1_PATTERN       0xfbc00090
#define PROC_E4ARDUINO_SMLALBT_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLALBT_32_ET1_MASK)                  \
                    == E4ARDUINO_SMLALBT_32_ET1_PATTERN)) {                 \
                smlalbt_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlaltb_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMLALTB_32_ET1_MASK          0xfff000f0
#define E4ARDUINO_SMLALTB_32_ET1_PATTERN       0xfbc000a0
#define PROC_E4ARDUINO_SMLALTB_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLALTB_32_ET1_MASK)                  \
                    == E4ARDUINO_SMLALTB_32_ET1_PATTERN)) {                 \
                smlaltb_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlaltt_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMLALTT_32_ET1_MASK          0xfff000f0
#define E4ARDUINO_SMLALTT_32_ET1_PATTERN       0xfbc000b0
#define PROC_E4ARDUINO_SMLALTT_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLALTT_32_ET1_MASK)                  \
                    == E4ARDUINO_SMLALTT_32_ET1_PATTERN)) {                 \
                smlaltt_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlald_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMLALD_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLALD_32_ET1_PATTERN        0xfbc000c0
#define PROC_E4ARDUINO_SMLALD_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLALD_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLALD_32_ET1_PATTERN)) {                  \
                smlald_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlaldx_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMLALDX_32_ET1_MASK          0xfff000f0
#define E4ARDUINO_SMLALDX_32_ET1_PATTERN       0xfbc000d0
#define PROC_E4ARDUINO_SMLALDX_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLALDX_32_ET1_MASK)                  \
                    == E4ARDUINO_SMLALDX_32_ET1_PATTERN)) {                 \
                smlaldx_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlsld_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMLSLD_32_ET1_MASK           0xfff000f0
#define E4ARDUINO_SMLSLD_32_ET1_PATTERN        0xfbd000c0
#define PROC_E4ARDUINO_SMLSLD_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLSLD_32_ET1_MASK)                   \
                    == E4ARDUINO_SMLSLD_32_ET1_PATTERN)) {                  \
                smlsld_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void smlsldx_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_SMLSLDX_32_ET1_MASK          0xfff000f0
#define E4ARDUINO_SMLSLDX_32_ET1_PATTERN       0xfbd000d0
#define PROC_E4ARDUINO_SMLSLDX_32_ET1(ctx, op)                              \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_SMLSLDX_32_ET1_MASK)                  \
                    == E4ARDUINO_SMLSLDX_32_ET1_PATTERN)) {                 \
                smlsldx_32_et1(                                             \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void umlal_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_UMLAL_32_ET1_MASK            0xfff000f0
#define E4ARDUINO_UMLAL_32_ET1_PATTERN         0xfbe00000
#define PROC_E4ARDUINO_UMLAL_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UMLAL_32_ET1_MASK)                    \
                    == E4ARDUINO_UMLAL_32_ET1_PATTERN)) {                   \
                umlal_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void umaal_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
#define E4ARDUINO_UMAAL_32_ET1_MASK            0xfff000f0
#define E4ARDUINO_UMAAL_32_ET1_PATTERN         0xfbe00060
#define PROC_E4ARDUINO_UMAAL_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_UMAAL_32_ET1_MASK)                    \
                    == E4ARDUINO_UMAAL_32_ET1_PATTERN)) {                   \
                umaal_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000f00) >> 8),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A6.4
 *   Floating-point data-processing instructions
 */

extern void vsel_32_et1(void *ctx, word d, word cc, word vn, word vd, word sz, word n, word m, word vm);
#define E4ARDUINO_VSEL_32_ET1_MASK             0xff800e50
#define E4ARDUINO_VSEL_32_ET1_PATTERN          0xfe000a00
#define PROC_E4ARDUINO_VSEL_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VSEL_32_ET1_MASK)                     \
                    == E4ARDUINO_VSEL_32_ET1_PATTERN)) {                    \
                vsel_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00300000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vmla_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VMLA_32_ET1_MASK             0xffb00f50
#define E4ARDUINO_VMLA_32_ET1_PATTERN          0xee000a00
#define PROC_E4ARDUINO_VMLA_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMLA_32_ET1_MASK)                     \
                    == E4ARDUINO_VMLA_32_ET1_PATTERN)) {                    \
                vmla_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vmls_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VMLS_32_ET1_MASK             0xffb00f50
#define E4ARDUINO_VMLS_32_ET1_PATTERN          0xee000a40
#define PROC_E4ARDUINO_VMLS_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMLS_32_ET1_MASK)                     \
                    == E4ARDUINO_VMLS_32_ET1_PATTERN)) {                    \
                vmls_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vnmla_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VNMLA_32_ET1_MASK            0xffb00f50
#define E4ARDUINO_VNMLA_32_ET1_PATTERN         0xee100a40
#define PROC_E4ARDUINO_VNMLA_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VNMLA_32_ET1_MASK)                    \
                    == E4ARDUINO_VNMLA_32_ET1_PATTERN)) {                   \
                vnmla_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vnmls_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VNMLS_32_ET1_MASK            0xffb00f50
#define E4ARDUINO_VNMLS_32_ET1_PATTERN         0xee100a00
#define PROC_E4ARDUINO_VNMLS_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VNMLS_32_ET1_MASK)                    \
                    == E4ARDUINO_VNMLS_32_ET1_PATTERN)) {                   \
                vnmls_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vnmul_32_et2(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VNMUL_32_ET2_MASK            0xffb00f50
#define E4ARDUINO_VNMUL_32_ET2_PATTERN         0xee200a40
#define PROC_E4ARDUINO_VNMUL_32_ET2(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VNMUL_32_ET2_MASK)                    \
                    == E4ARDUINO_VNMUL_32_ET2_PATTERN)) {                   \
                vnmul_32_et2(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vmul_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VMUL_32_ET1_MASK             0xffb00f50
#define E4ARDUINO_VMUL_32_ET1_PATTERN          0xee200a00
#define PROC_E4ARDUINO_VMUL_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMUL_32_ET1_MASK)                     \
                    == E4ARDUINO_VMUL_32_ET1_PATTERN)) {                    \
                vmul_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vadd_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VADD_32_ET1_MASK             0xffb00f50
#define E4ARDUINO_VADD_32_ET1_PATTERN          0xee300a00
#define PROC_E4ARDUINO_VADD_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VADD_32_ET1_MASK)                     \
                    == E4ARDUINO_VADD_32_ET1_PATTERN)) {                    \
                vadd_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vsub_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VSUB_32_ET1_MASK             0xffb00f50
#define E4ARDUINO_VSUB_32_ET1_PATTERN          0xee300a40
#define PROC_E4ARDUINO_VSUB_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VSUB_32_ET1_MASK)                     \
                    == E4ARDUINO_VSUB_32_ET1_PATTERN)) {                    \
                vsub_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vdiv_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VDIV_32_ET1_MASK             0xffb00f50
#define E4ARDUINO_VDIV_32_ET1_PATTERN          0xee800a00
#define PROC_E4ARDUINO_VDIV_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VDIV_32_ET1_MASK)                     \
                    == E4ARDUINO_VDIV_32_ET1_PATTERN)) {                    \
                vdiv_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vmaxnm_32_et1(void *ctx, word d, word vn, word vd, word sz, word n, word m, word vm);
#define E4ARDUINO_VMAXNM_32_ET1_MASK           0xffb00e50
#define E4ARDUINO_VMAXNM_32_ET1_PATTERN        0xfe800a00
#define PROC_E4ARDUINO_VMAXNM_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMAXNM_32_ET1_MASK)                   \
                    == E4ARDUINO_VMAXNM_32_ET1_PATTERN)) {                  \
                vmaxnm_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vminnm_32_et1(void *ctx, word d, word vn, word vd, word sz, word n, word m, word vm);
#define E4ARDUINO_VMINNM_32_ET1_MASK           0xffb00e50
#define E4ARDUINO_VMINNM_32_ET1_PATTERN        0xfe800a40
#define PROC_E4ARDUINO_VMINNM_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMINNM_32_ET1_MASK)                   \
                    == E4ARDUINO_VMINNM_32_ET1_PATTERN)) {                  \
                vminnm_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vmov_imm_32_et1(void *ctx, word d, word imm4h, word vd, word imm4l);
#define E4ARDUINO_VMOV_IMM_32_ET1_MASK         0xffb00ff0
#define E4ARDUINO_VMOV_IMM_32_ET1_PATTERN      0xeeb00a00
#define PROC_E4ARDUINO_VMOV_IMM_32_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMOV_IMM_32_ET1_MASK)                 \
                    == E4ARDUINO_VMOV_IMM_32_ET1_PATTERN)) {                \
                vmov_imm_32_et1(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vmov_reg_32_et1(void *ctx, word d, word vd, word m, word vm);
#define E4ARDUINO_VMOV_REG_32_ET1_MASK         0xffbf0fd0
#define E4ARDUINO_VMOV_REG_32_ET1_PATTERN      0xeeb00a40
#define PROC_E4ARDUINO_VMOV_REG_32_ET1(ctx, op)                             \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMOV_REG_32_ET1_MASK)                 \
                    == E4ARDUINO_VMOV_REG_32_ET1_PATTERN)) {                \
                vmov_reg_32_et1(                                            \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vabs_32_et1(void *ctx, word d, word vd, word m, word vm);
#define E4ARDUINO_VABS_32_ET1_MASK             0xffbf0fd0
#define E4ARDUINO_VABS_32_ET1_PATTERN          0xeeb00ac0
#define PROC_E4ARDUINO_VABS_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VABS_32_ET1_MASK)                     \
                    == E4ARDUINO_VABS_32_ET1_PATTERN)) {                    \
                vabs_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vneg_32_et1(void *ctx, word d, word vd, word m, word vm);
#define E4ARDUINO_VNEG_32_ET1_MASK             0xffbf0fd0
#define E4ARDUINO_VNEG_32_ET1_PATTERN          0xeeb10a40
#define PROC_E4ARDUINO_VNEG_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VNEG_32_ET1_MASK)                     \
                    == E4ARDUINO_VNEG_32_ET1_PATTERN)) {                    \
                vneg_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vsqrt_32_et1(void *ctx, word d, word vd, word m, word vm);
#define E4ARDUINO_VSQRT_32_ET1_MASK            0xffbf0fd0
#define E4ARDUINO_VSQRT_32_ET1_PATTERN         0xeeb10ac0
#define PROC_E4ARDUINO_VSQRT_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VSQRT_32_ET1_MASK)                    \
                    == E4ARDUINO_VSQRT_32_ET1_PATTERN)) {                   \
                vsqrt_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvtb_32_et1(void *ctx, word d, word op, word vd, word sz, word m, word vm);
#define E4ARDUINO_VCVTB_32_ET1_MASK            0xffbe0ed0
#define E4ARDUINO_VCVTB_32_ET1_PATTERN         0xeeb20a40
#define PROC_E4ARDUINO_VCVTB_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVTB_32_ET1_MASK)                    \
                    == E4ARDUINO_VCVTB_32_ET1_PATTERN)) {                   \
                vcvtb_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00010000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvtt_32_et1(void *ctx, word d, word op, word vd, word sz, word m, word vm);
#define E4ARDUINO_VCVTT_32_ET1_MASK            0xffbe0ed0
#define E4ARDUINO_VCVTT_32_ET1_PATTERN         0xeeb20ac0
#define PROC_E4ARDUINO_VCVTT_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVTT_32_ET1_MASK)                    \
                    == E4ARDUINO_VCVTT_32_ET1_PATTERN)) {                   \
                vcvtt_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00010000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcmp_32_et1(void *ctx, word d, word vd, word sz, word e, word m, word vm);
#define E4ARDUINO_VCMP_32_ET1_MASK             0xffbf0e50
#define E4ARDUINO_VCMP_32_ET1_PATTERN          0xeeb40a40
#define PROC_E4ARDUINO_VCMP_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCMP_32_ET1_MASK)                     \
                    == E4ARDUINO_VCMP_32_ET1_PATTERN)) {                    \
                vcmp_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcmpe_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
#define E4ARDUINO_VCMPE_32_ET1_MASK            0xffbf0ed0
#define E4ARDUINO_VCMPE_32_ET1_PATTERN         0xeeb40ac0
#define PROC_E4ARDUINO_VCMPE_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCMPE_32_ET1_MASK)                    \
                    == E4ARDUINO_VCMPE_32_ET1_PATTERN)) {                   \
                vcmpe_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcmp_32_et2(void *ctx, word d, word vd, word sz, word e);
#define E4ARDUINO_VCMP_32_ET2_MASK             0xffbf0e7f
#define E4ARDUINO_VCMP_32_ET2_PATTERN          0xeeb50a40
#define PROC_E4ARDUINO_VCMP_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCMP_32_ET2_MASK)                     \
                    == E4ARDUINO_VCMP_32_ET2_PATTERN)) {                    \
                vcmp_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7));                        \
            }                                                               \
        } while(0)

extern void vcmpe_32_et2(void *ctx, word d, word vd, word sz);
#define E4ARDUINO_VCMPE_32_ET2_MASK            0xffbf0eff
#define E4ARDUINO_VCMPE_32_ET2_PATTERN         0xeeb50ac0
#define PROC_E4ARDUINO_VCMPE_32_ET2(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCMPE_32_ET2_MASK)                    \
                    == E4ARDUINO_VCMPE_32_ET2_PATTERN)) {                   \
                vcmpe_32_et2(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8));                        \
            }                                                               \
        } while(0)

extern void vrintx_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
#define E4ARDUINO_VRINTX_32_ET1_MASK           0xffbf0ed0
#define E4ARDUINO_VRINTX_32_ET1_PATTERN        0xeeb70a40
#define PROC_E4ARDUINO_VRINTX_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VRINTX_32_ET1_MASK)                   \
                    == E4ARDUINO_VRINTX_32_ET1_PATTERN)) {                  \
                vrintx_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vrintz_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
#define E4ARDUINO_VRINTZ_32_ET1_MASK           0xffbf0ed0
#define E4ARDUINO_VRINTZ_32_ET1_PATTERN        0xeeb60ac0
#define PROC_E4ARDUINO_VRINTZ_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VRINTZ_32_ET1_MASK)                   \
                    == E4ARDUINO_VRINTZ_32_ET1_PATTERN)) {                  \
                vrintz_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vrintr_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
#define E4ARDUINO_VRINTR_32_ET1_MASK           0xffbf0ed0
#define E4ARDUINO_VRINTR_32_ET1_PATTERN        0xeeb60a40
#define PROC_E4ARDUINO_VRINTR_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VRINTR_32_ET1_MASK)                   \
                    == E4ARDUINO_VRINTR_32_ET1_PATTERN)) {                  \
                vrintr_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvt_dp_sp_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
#define E4ARDUINO_VCVT_DP_SP_32_ET1_MASK       0xffbf0ed0
#define E4ARDUINO_VCVT_DP_SP_32_ET1_PATTERN    0xeeb70ac0
#define PROC_E4ARDUINO_VCVT_DP_SP_32_ET1(ctx, op)                           \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVT_DP_SP_32_ET1_MASK)               \
                    == E4ARDUINO_VCVT_DP_SP_32_ET1_PATTERN)) {              \
                vcvt_dp_sp_32_et1(                                          \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvt_fp_int_32_et1(void *ctx, word d, word opc2, word vd, word sz, word m, word vm);
#define E4ARDUINO_VCVT_FP_INT_32_ET1_MASK      0xffb00ed0
#define E4ARDUINO_VCVT_FP_INT_32_ET1_PATTERN    0xeeb00ac0
#define PROC_E4ARDUINO_VCVT_FP_INT_32_ET1(ctx, op)                          \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVT_FP_INT_32_ET1_MASK)              \
                    == E4ARDUINO_VCVT_FP_INT_32_ET1_PATTERN)) {             \
                vcvt_fp_int_32_et1(                                         \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvtr_fp_int_32_et1(void *ctx, word d, word opc2, word vd, word sz, word m, word vm);
#define E4ARDUINO_VCVTR_FP_INT_32_ET1_MASK     0xffb00ed0
#define E4ARDUINO_VCVTR_FP_INT_32_ET1_PATTERN    0xeeb00a40
#define PROC_E4ARDUINO_VCVTR_FP_INT_32_ET1(ctx, op)                         \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVTR_FP_INT_32_ET1_MASK)             \
                    == E4ARDUINO_VCVTR_FP_INT_32_ET1_PATTERN)) {            \
                vcvtr_fp_int_32_et1(                                        \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vrinta_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
#define E4ARDUINO_VRINTA_32_ET1_MASK           0xffbf0ed0
#define E4ARDUINO_VRINTA_32_ET1_PATTERN        0xfeb80a40
#define PROC_E4ARDUINO_VRINTA_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VRINTA_32_ET1_MASK)                   \
                    == E4ARDUINO_VRINTA_32_ET1_PATTERN)) {                  \
                vrinta_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vrintn_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
#define E4ARDUINO_VRINTN_32_ET1_MASK           0xffbf0ed0
#define E4ARDUINO_VRINTN_32_ET1_PATTERN        0xfeb90a40
#define PROC_E4ARDUINO_VRINTN_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VRINTN_32_ET1_MASK)                   \
                    == E4ARDUINO_VRINTN_32_ET1_PATTERN)) {                  \
                vrintn_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vrintp_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
#define E4ARDUINO_VRINTP_32_ET1_MASK           0xffbf0ed0
#define E4ARDUINO_VRINTP_32_ET1_PATTERN        0xfeba0a40
#define PROC_E4ARDUINO_VRINTP_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VRINTP_32_ET1_MASK)                   \
                    == E4ARDUINO_VRINTP_32_ET1_PATTERN)) {                  \
                vrintp_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vrintm_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
#define E4ARDUINO_VRINTM_32_ET1_MASK           0xffbf0ed0
#define E4ARDUINO_VRINTM_32_ET1_PATTERN        0xfebb0a40
#define PROC_E4ARDUINO_VRINTM_32_ET1(ctx, op)                               \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VRINTM_32_ET1_MASK)                   \
                    == E4ARDUINO_VRINTM_32_ET1_PATTERN)) {                  \
                vrintm_32_et1(                                              \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvta_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm);
#define E4ARDUINO_VCVTA_32_ET1_MASK            0xffbf0e50
#define E4ARDUINO_VCVTA_32_ET1_PATTERN         0xfebc0a40
#define PROC_E4ARDUINO_VCVTA_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVTA_32_ET1_MASK)                    \
                    == E4ARDUINO_VCVTA_32_ET1_PATTERN)) {                   \
                vcvta_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvtn_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm);
#define E4ARDUINO_VCVTN_32_ET1_MASK            0xffbf0e50
#define E4ARDUINO_VCVTN_32_ET1_PATTERN         0xfebd0a40
#define PROC_E4ARDUINO_VCVTN_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVTN_32_ET1_MASK)                    \
                    == E4ARDUINO_VCVTN_32_ET1_PATTERN)) {                   \
                vcvtn_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvtp_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm);
#define E4ARDUINO_VCVTP_32_ET1_MASK            0xffbf0e50
#define E4ARDUINO_VCVTP_32_ET1_PATTERN         0xfebe0a40
#define PROC_E4ARDUINO_VCVTP_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVTP_32_ET1_MASK)                    \
                    == E4ARDUINO_VCVTP_32_ET1_PATTERN)) {                   \
                vcvtp_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvtm_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm);
#define E4ARDUINO_VCVTM_32_ET1_MASK            0xffbf0e50
#define E4ARDUINO_VCVTM_32_ET1_PATTERN         0xfebf0a40
#define PROC_E4ARDUINO_VCVTM_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVTM_32_ET1_MASK)                    \
                    == E4ARDUINO_VCVTM_32_ET1_PATTERN)) {                   \
                vcvtm_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vcvt_fp_fix_32_et1(void *ctx, word d, word op, word u, word vd, word sf, word sx, word i, word imm);
#define E4ARDUINO_VCVT_FP_FIX_32_ET1_MASK      0xffba0e50
#define E4ARDUINO_VCVT_FP_FIX_32_ET1_PATTERN    0xeeba0a40
#define PROC_E4ARDUINO_VCVT_FP_FIX_32_ET1(ctx, op)                          \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VCVT_FP_FIX_32_ET1_MASK)              \
                    == E4ARDUINO_VCVT_FP_FIX_32_ET1_PATTERN)) {             \
                vcvt_fp_fix_32_et1(                                         \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00040000) >> 18),                        \
                    ((OP32(op) & 0x00010000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000100) >> 8),                         \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A6.6
 *   32-bit transfers between ARM core and extension registers
 */

extern void vmov_reg_sp_32_et1(void *ctx, word op, word vn, word rt, word n);
#define E4ARDUINO_VMOV_REG_SP_32_ET1_MASK      0xffe00f7f
#define E4ARDUINO_VMOV_REG_SP_32_ET1_PATTERN    0xee000a10
#define PROC_E4ARDUINO_VMOV_REG_SP_32_ET1(ctx, op)                          \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMOV_REG_SP_32_ET1_MASK)              \
                    == E4ARDUINO_VMOV_REG_SP_32_ET1_PATTERN)) {             \
                vmov_reg_sp_32_et1(                                         \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7));                        \
            }                                                               \
        } while(0)

extern void vmsr_32_et1(void *ctx, word rt);
#define E4ARDUINO_VMSR_32_ET1_MASK             0xffff0fff
#define E4ARDUINO_VMSR_32_ET1_PATTERN          0xeee10a10
#define PROC_E4ARDUINO_VMSR_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMSR_32_ET1_MASK)                     \
                    == E4ARDUINO_VMSR_32_ET1_PATTERN)) {                    \
                vmsr_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x0000f000) >> 12));                       \
            }                                                               \
        } while(0)

extern void vmov_reg_scl_32_et1(void *ctx, word h, word vd, word rt, word d);
#define E4ARDUINO_VMOV_REG_SCL_32_ET1_MASK     0xffd00f7f
#define E4ARDUINO_VMOV_REG_SCL_32_ET1_PATTERN    0xee000b10
#define PROC_E4ARDUINO_VMOV_REG_SCL_32_ET1(ctx, op)                         \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMOV_REG_SCL_32_ET1_MASK)             \
                    == E4ARDUINO_VMOV_REG_SCL_32_ET1_PATTERN)) {            \
                vmov_reg_scl_32_et1(                                        \
                    ctx,                                                    \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7));                        \
            }                                                               \
        } while(0)

extern void vfma_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VFMA_32_ET1_MASK             0xffb00f50
#define E4ARDUINO_VFMA_32_ET1_PATTERN          0xeea00a00
#define PROC_E4ARDUINO_VFMA_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VFMA_32_ET1_MASK)                     \
                    == E4ARDUINO_VFMA_32_ET1_PATTERN)) {                    \
                vfma_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vfms_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VFMS_32_ET1_MASK             0xffb00f50
#define E4ARDUINO_VFMS_32_ET1_PATTERN          0xeea00a40
#define PROC_E4ARDUINO_VFMS_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VFMS_32_ET1_MASK)                     \
                    == E4ARDUINO_VFMS_32_ET1_PATTERN)) {                    \
                vfms_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vfnma_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VFNMA_32_ET1_MASK            0xffb00f50
#define E4ARDUINO_VFNMA_32_ET1_PATTERN         0xee900a00
#define PROC_E4ARDUINO_VFNMA_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VFNMA_32_ET1_MASK)                    \
                    == E4ARDUINO_VFNMA_32_ET1_PATTERN)) {                   \
                vfnma_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vfnms_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
#define E4ARDUINO_VFNMS_32_ET1_MASK            0xffb00f50
#define E4ARDUINO_VFNMS_32_ET1_PATTERN         0xee900a40
#define PROC_E4ARDUINO_VFNMS_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VFNMS_32_ET1_MASK)                    \
                    == E4ARDUINO_VFNMS_32_ET1_PATTERN)) {                   \
                vfnms_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7),                         \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vmrs_32_et1(void *ctx, word rt);
#define E4ARDUINO_VMRS_32_ET1_MASK             0xffff0fff
#define E4ARDUINO_VMRS_32_ET1_PATTERN          0xeef10a10
#define PROC_E4ARDUINO_VMRS_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMRS_32_ET1_MASK)                     \
                    == E4ARDUINO_VMRS_32_ET1_PATTERN)) {                    \
                vmrs_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x0000f000) >> 12));                       \
            }                                                               \
        } while(0)

extern void vmov_scl_reg_32_et1(void *ctx, word h, word vn, word rt, word n);
#define E4ARDUINO_VMOV_SCL_REG_32_ET1_MASK     0xffd00f7f
#define E4ARDUINO_VMOV_SCL_REG_32_ET1_PATTERN    0xee100b10
#define PROC_E4ARDUINO_VMOV_SCL_REG_32_ET1(ctx, op)                         \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMOV_SCL_REG_32_ET1_MASK)             \
                    == E4ARDUINO_VMOV_SCL_REG_32_ET1_PATTERN)) {            \
                vmov_scl_reg_32_et1(                                        \
                    ctx,                                                    \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000080) >> 7));                        \
            }                                                               \
        } while(0)

/*
 *   A6.7
 *   64-bit transfers between ARM core and extension registers
 */

extern void vmov_reg_sp_sp_32_et1(void *ctx, word op, word rt2, word rt, word m, word vm);
#define E4ARDUINO_VMOV_REG_SP_SP_32_ET1_MASK    0xffe00fd0
#define E4ARDUINO_VMOV_REG_SP_SP_32_ET1_PATTERN    0xec400a10
#define PROC_E4ARDUINO_VMOV_REG_SP_SP_32_ET1(ctx, op)                       \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMOV_REG_SP_SP_32_ET1_MASK)           \
                    == E4ARDUINO_VMOV_REG_SP_SP_32_ET1_PATTERN)) {          \
                vmov_reg_sp_sp_32_et1(                                      \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

extern void vmov_reg_dp_32_et1(void *ctx, word op, word rt2, word rt, word m, word vm);
#define E4ARDUINO_VMOV_REG_DP_32_ET1_MASK      0xffe00fd0
#define E4ARDUINO_VMOV_REG_DP_32_ET1_PATTERN    0xec400b10
#define PROC_E4ARDUINO_VMOV_REG_DP_32_ET1(ctx, op)                          \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VMOV_REG_DP_32_ET1_MASK)              \
                    == E4ARDUINO_VMOV_REG_DP_32_ET1_PATTERN)) {             \
                vmov_reg_dp_32_et1(                                         \
                    ctx,                                                    \
                    ((OP32(op) & 0x00100000) >> 20),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x00000020) >> 5),                         \
                    ((OP32(op) & 0x0000000f) >> 0));                        \
            }                                                               \
        } while(0)

/*
 *   A6.5
 *   Extension register load or store instruction
 */

extern void vstm_32_et1(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8);
#define E4ARDUINO_VSTM_32_ET1_MASK             0xfe100f00
#define E4ARDUINO_VSTM_32_ET1_PATTERN          0xec000b00
#define PROC_E4ARDUINO_VSTM_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VSTM_32_ET1_MASK)                     \
                    == E4ARDUINO_VSTM_32_ET1_PATTERN)) {                    \
                vstm_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vstm_32_et2(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8);
#define E4ARDUINO_VSTM_32_ET2_MASK             0xfe100f00
#define E4ARDUINO_VSTM_32_ET2_PATTERN          0xec000a00
#define PROC_E4ARDUINO_VSTM_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VSTM_32_ET2_MASK)                     \
                    == E4ARDUINO_VSTM_32_ET2_PATTERN)) {                    \
                vstm_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vstr_32_et1(void *ctx, word u, word d, word rn, word vd, word imm8);
#define E4ARDUINO_VSTR_32_ET1_MASK             0xff300f00
#define E4ARDUINO_VSTR_32_ET1_PATTERN          0xed000b00
#define PROC_E4ARDUINO_VSTR_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VSTR_32_ET1_MASK)                     \
                    == E4ARDUINO_VSTR_32_ET1_PATTERN)) {                    \
                vstr_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vstr_32_et2(void *ctx, word u, word d, word rn, word vd, word imm8);
#define E4ARDUINO_VSTR_32_ET2_MASK             0xff300f00
#define E4ARDUINO_VSTR_32_ET2_PATTERN          0xed000a00
#define PROC_E4ARDUINO_VSTR_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VSTR_32_ET2_MASK)                     \
                    == E4ARDUINO_VSTR_32_ET2_PATTERN)) {                    \
                vstr_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vpush_32_et1(void *ctx, word d, word vd, word imm8);
#define E4ARDUINO_VPUSH_32_ET1_MASK            0xffbf0f00
#define E4ARDUINO_VPUSH_32_ET1_PATTERN         0xed2d0b00
#define PROC_E4ARDUINO_VPUSH_32_ET1(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VPUSH_32_ET1_MASK)                    \
                    == E4ARDUINO_VPUSH_32_ET1_PATTERN)) {                   \
                vpush_32_et1(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vpush_32_et2(void *ctx, word d, word vd, word imm8);
#define E4ARDUINO_VPUSH_32_ET2_MASK            0xffbf0f00
#define E4ARDUINO_VPUSH_32_ET2_PATTERN         0xed2d0a00
#define PROC_E4ARDUINO_VPUSH_32_ET2(ctx, op)                                \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VPUSH_32_ET2_MASK)                    \
                    == E4ARDUINO_VPUSH_32_ET2_PATTERN)) {                   \
                vpush_32_et2(                                               \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vldm_32_et1(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8);
#define E4ARDUINO_VLDM_32_ET1_MASK             0xfe100f00
#define E4ARDUINO_VLDM_32_ET1_PATTERN          0xec100b00
#define PROC_E4ARDUINO_VLDM_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VLDM_32_ET1_MASK)                     \
                    == E4ARDUINO_VLDM_32_ET1_PATTERN)) {                    \
                vldm_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vldm_32_et2(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8);
#define E4ARDUINO_VLDM_32_ET2_MASK             0xfe100f00
#define E4ARDUINO_VLDM_32_ET2_PATTERN          0xec100a00
#define PROC_E4ARDUINO_VLDM_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VLDM_32_ET2_MASK)                     \
                    == E4ARDUINO_VLDM_32_ET2_PATTERN)) {                    \
                vldm_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x01000000) >> 24),                        \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x00200000) >> 21),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vpop_32_et1(void *ctx, word d, word vd, word imm8);
#define E4ARDUINO_VPOP_32_ET1_MASK             0xffbf0f00
#define E4ARDUINO_VPOP_32_ET1_PATTERN          0xecbd0b00
#define PROC_E4ARDUINO_VPOP_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VPOP_32_ET1_MASK)                     \
                    == E4ARDUINO_VPOP_32_ET1_PATTERN)) {                    \
                vpop_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vpop_32_et2(void *ctx, word d, word vd, word imm8);
#define E4ARDUINO_VPOP_32_ET2_MASK             0xffbf0f00
#define E4ARDUINO_VPOP_32_ET2_PATTERN          0xecbd0a00
#define PROC_E4ARDUINO_VPOP_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VPOP_32_ET2_MASK)                     \
                    == E4ARDUINO_VPOP_32_ET2_PATTERN)) {                    \
                vpop_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vldr_32_et1(void *ctx, word u, word d, word rn, word vd, word imm8);
#define E4ARDUINO_VLDR_32_ET1_MASK             0xff300f00
#define E4ARDUINO_VLDR_32_ET1_PATTERN          0xed100b00
#define PROC_E4ARDUINO_VLDR_32_ET1(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VLDR_32_ET1_MASK)                     \
                    == E4ARDUINO_VLDR_32_ET1_PATTERN)) {                    \
                vldr_32_et1(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

extern void vldr_32_et2(void *ctx, word u, word d, word rn, word vd, word imm8);
#define E4ARDUINO_VLDR_32_ET2_MASK             0xff300f00
#define E4ARDUINO_VLDR_32_ET2_PATTERN          0xed100a00
#define PROC_E4ARDUINO_VLDR_32_ET2(ctx, op)                                 \
        do {                                                                \
            if(((OP32(op) & E4ARDUINO_VLDR_32_ET2_MASK)                     \
                    == E4ARDUINO_VLDR_32_ET2_PATTERN)) {                    \
                vldr_32_et2(                                                \
                    ctx,                                                    \
                    ((OP32(op) & 0x00800000) >> 23),                        \
                    ((OP32(op) & 0x00400000) >> 22),                        \
                    ((OP32(op) & 0x000f0000) >> 16),                        \
                    ((OP32(op) & 0x0000f000) >> 12),                        \
                    ((OP32(op) & 0x000000ff) >> 0));                        \
            }                                                               \
        } while(0)

#define PROCESS_SINGLE_INSTRUCTION(ctx, op)                                 \
do {                                                                        \
        PROC_E4ARDUINO_CLREX_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SEV_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_WFI_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_WFE_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_YIELD_32_ET2((ctx), (op));                           \
        PROC_E4ARDUINO_NOP_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_ISB_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_DMB_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_DSB_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_DBG_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_POP_32_ET3((ctx), (op));                             \
        PROC_E4ARDUINO_PUSH_32_ET3((ctx), (op));                            \
        PROC_E4ARDUINO_VMRS_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VMSR_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VCMPE_32_ET2((ctx), (op));                           \
        PROC_E4ARDUINO_VCMP_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_TBH_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_TBB_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_RRX_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_MOV_REG_32_ET3((ctx), (op));                         \
        PROC_E4ARDUINO_UXTB_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_SXTB_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_UXTB16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SXTB16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_UXTH_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_SXTH_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_LDREXH_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_LDREXB_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_PLI_REG_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_PLD_REG_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_VSQRT_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VNEG_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VABS_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VMOV_REG_32_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_VRINTM_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_VRINTP_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_VRINTN_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_VRINTA_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_VCVT_DP_SP_32_ET1((ctx), (op));                      \
        PROC_E4ARDUINO_VRINTX_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_VRINTZ_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_VRINTR_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_VCMPE_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_MRS_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_VMOV_REG_SP_32_ET1((ctx), (op));                     \
        PROC_E4ARDUINO_VMOV_SCL_REG_32_ET1((ctx), (op));                    \
        PROC_E4ARDUINO_VMOV_REG_SCL_32_ET1((ctx), (op));                    \
        PROC_E4ARDUINO_PLI_IMM_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_PLD_IMM_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_VCVTM_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VCVTP_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VCVTN_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VCVTA_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VCMP_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VCVTT_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VCVTB_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_UDIV_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_SDIV_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_USAD8_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMMULR_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMMUL_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMUSDX_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMUSD_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMULWT_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMULWB_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMUADX_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMUAD_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMULTT_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMULTB_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMULBT_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMULBB_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_MUL_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_UHSAX_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_UQSAX_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_USAX_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_SHSAX_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_QSAX_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_SSAX_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_UHSUB16_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_UQSUB16_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_USUB16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SHSUB16_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_QSUB16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SSUB16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_UHSUB8_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_UQSUB8_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_USUB8_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SHSUB8_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_QSUB8_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SSUB8_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_CLZ_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_SEL_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_UHASX_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_UQASX_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_UASX_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_SHASX_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_QASX_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_SASX_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_REVSH_32_ET2((ctx), (op));                           \
        PROC_E4ARDUINO_RBIT_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_REV16_32_ET2((ctx), (op));                           \
        PROC_E4ARDUINO_REV_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_UHADD16_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_UQADD16_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_UADD16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SHADD16_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_QADD16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SADD16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_QDSUB_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_QSUB_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_QDADD_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_QADD_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_UHADD8_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_UQADD8_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_UADD8_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SHADD8_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_QADD8_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SADD8_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_USAT16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SSAT16_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_PLI_LIT_32_ET3((ctx), (op));                         \
        PROC_E4ARDUINO_PLD_LIT_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_STREXH_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_STREXB_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_VPUSH_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VPUSH_32_ET2((ctx), (op));                           \
        PROC_E4ARDUINO_VPOP_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VPOP_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_PUSH_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_BFC_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_ROR_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_ASR_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_LSR_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_LSL_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_ROR_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ASR_IMM_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_LSR_IMM_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_LSL_IMM_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_MSR_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_VMOV_IMM_32_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_UXTAB_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SXTAB_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_UXTAB16_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SXTAB16_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_UXTAH_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SXTAH_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VCVT_FP_FIX_32_ET1((ctx), (op));                     \
        PROC_E4ARDUINO_LDRSH_REG_32_ET2((ctx), (op));                       \
        PROC_E4ARDUINO_LDRSB_REG_32_ET2((ctx), (op));                       \
        PROC_E4ARDUINO_LDR_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_STR_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_LDRH_REG_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_STRH_REG_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_LDRB_REG_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_STRB_REG_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_POP_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_VMOV_REG_DP_32_ET1((ctx), (op));                     \
        PROC_E4ARDUINO_VMOV_REG_SP_SP_32_ET1((ctx), (op));                  \
        PROC_E4ARDUINO_CMP_REG_32_ET3((ctx), (op));                         \
        PROC_E4ARDUINO_CMN_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_TEQ_REG_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_TST_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_SUB_SP_REG_32_ET1((ctx), (op));                      \
        PROC_E4ARDUINO_ADD_SP_REG_32_ET3((ctx), (op));                      \
        PROC_E4ARDUINO_MVN_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_ADR_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_SUB_SP_IMM_32_ET3((ctx), (op));                      \
        PROC_E4ARDUINO_ADR_32_ET3((ctx), (op));                             \
        PROC_E4ARDUINO_ADD_SP_IMM_32_ET4((ctx), (op));                      \
        PROC_E4ARDUINO_VFMS_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VFMA_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VFNMS_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VFNMA_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VDIV_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VSUB_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VADD_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VNMUL_32_ET2((ctx), (op));                           \
        PROC_E4ARDUINO_VMUL_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VNMLA_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VNMLS_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_VMLS_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VMLA_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VCVT_FP_INT_32_ET1((ctx), (op));                     \
        PROC_E4ARDUINO_VCVTR_FP_INT_32_ET1((ctx), (op));                    \
        PROC_E4ARDUINO_PLI_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_PLD_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_UDF_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_LDRSHT_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_LDRSBT_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_LDRT_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_STRT_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_LDRHT_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_STRHT_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_LDRBT_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_STRBT_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_LDREX_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_CMP_IMM_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_CMN_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_TEQ_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_UMAAL_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_UMLAL_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMLSLDX_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SMLSLD_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMLALDX_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SMLALD_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMLALTT_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SMLALTB_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SMLALBT_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SMLALBB_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SMLAL_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_UMULL_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMULL_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_USADA8_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMMLSR_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMMLS_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMMLAR_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMMLA_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMLSDX_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMLSD_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMLAWT_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMLAWB_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMLADX_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMLAD_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_SMLATT_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMLATB_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMLABT_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_SMLABB_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_MLS_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_MLA_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_VMINNM_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_VMAXNM_32_ET1((ctx), (op));                          \
        PROC_E4ARDUINO_LDRSH_LIT_32_ET1((ctx), (op));                       \
        PROC_E4ARDUINO_LDRSB_LIT_32_ET1((ctx), (op));                       \
        PROC_E4ARDUINO_LDR_LIT_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_LDRH_LIT_32_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_LDRB_LIT_32_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_SUB_SP_IMM_32_ET2((ctx), (op));                      \
        PROC_E4ARDUINO_ADD_SP_IMM_32_ET3((ctx), (op));                      \
        PROC_E4ARDUINO_MVN_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_MOV_IMM_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_TST_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_UBFX_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_BFI_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_SBFX_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VLDR_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VLDR_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_VSTR_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VSTR_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_LDRSH_IMM_32_ET2((ctx), (op));                       \
        PROC_E4ARDUINO_LDRSB_IMM_32_ET2((ctx), (op));                       \
        PROC_E4ARDUINO_LDR_IMM_32_ET4((ctx), (op));                         \
        PROC_E4ARDUINO_STR_IMM_32_ET4((ctx), (op));                         \
        PROC_E4ARDUINO_LDRH_IMM_32_ET3((ctx), (op));                        \
        PROC_E4ARDUINO_STRH_IMM_32_ET3((ctx), (op));                        \
        PROC_E4ARDUINO_LDRB_IMM_32_ET3((ctx), (op));                        \
        PROC_E4ARDUINO_STRB_IMM_32_ET3((ctx), (op));                        \
        PROC_E4ARDUINO_VSEL_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_STMDB_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_STM_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_LDRD_LIT_32_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_PKHTB_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_PKHBT_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_USAT_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_SSAT_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_MRRC2_32_ET2((ctx), (op));                           \
        PROC_E4ARDUINO_MCRR2_32_ET2((ctx), (op));                           \
        PROC_E4ARDUINO_LDRSH_IMM_32_ET1((ctx), (op));                       \
        PROC_E4ARDUINO_LDRSB_IMM_32_ET1((ctx), (op));                       \
        PROC_E4ARDUINO_LDR_IMM_32_ET3((ctx), (op));                         \
        PROC_E4ARDUINO_STR_IMM_32_ET3((ctx), (op));                         \
        PROC_E4ARDUINO_LDRH_IMM_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_STRH_IMM_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_LDRB_IMM_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_STRB_IMM_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_MRRC_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_MCRR_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_STREX_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_RSB_REG_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SUB_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_SBC_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_ADC_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_ADD_REG_32_ET3((ctx), (op));                         \
        PROC_E4ARDUINO_EOR_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_ORN_REG_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ORR_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_BIC_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_AND_REG_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_LDMDB_32_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_LDM_32_ET2((ctx), (op));                             \
        PROC_E4ARDUINO_MOVT_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_SUB_IMM_32_ET4((ctx), (op));                         \
        PROC_E4ARDUINO_MOV_IMM_32_ET3((ctx), (op));                         \
        PROC_E4ARDUINO_ADD_IMM_32_ET4((ctx), (op));                         \
        PROC_E4ARDUINO_LDC2_LIT_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_LDC_LIT_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_CDP2_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_CDP_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_VLDM_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VLDM_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_VSTM_32_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_VSTM_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_SEV_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_WFI_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_WFE_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_YIELD_16_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_NOP_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_RSB_IMM_32_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_SUB_IMM_32_ET3((ctx), (op));                         \
        PROC_E4ARDUINO_SBC_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ADC_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ADD_IMM_32_ET3((ctx), (op));                         \
        PROC_E4ARDUINO_EOR_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ORN_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ORR_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_BIC_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_AND_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_MRC2_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_MCR2_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_MRC_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_MCR_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_CPS_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_LDRD_IMM_32_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_STRD_IMM_32_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_ADD_SP_REG_16_ET1((ctx), (op));                      \
        PROC_E4ARDUINO_BLX_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_BX_16_ET1((ctx), (op));                              \
        PROC_E4ARDUINO_ADD_SP_REG_16_ET2((ctx), (op));                      \
        PROC_E4ARDUINO_LDC2_IMM_32_ET2((ctx), (op));                        \
        PROC_E4ARDUINO_STC2_32_ET2((ctx), (op));                            \
        PROC_E4ARDUINO_LDC_IMM_32_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_STC_32_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_BL_32_ET1((ctx), (op));                              \
        PROC_E4ARDUINO_B_32_ET4((ctx), (op));                               \
        PROC_E4ARDUINO_B_32_ET3((ctx), (op));                               \
        PROC_E4ARDUINO_REVSH_16_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_REV16_16_ET1((ctx), (op));                           \
        PROC_E4ARDUINO_REV_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_UXTB_16_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_UXTH_16_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_SXTB_16_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_SXTH_16_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_MVN_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_BIC_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_MUL_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_ORR_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_CMN_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_CMP_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_RSB_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_TST_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ROR_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SBC_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ADC_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ASR_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_LSR_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_LSL_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_EOR_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_AND_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_MOV_REG_16_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_SUB_SP_IMM_16_ET1((ctx), (op));                      \
        PROC_E4ARDUINO_ADD_SP_IMM_16_ET2((ctx), (op));                      \
        PROC_E4ARDUINO_SVC_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_UDF_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_IT_16_ET1((ctx), (op));                              \
        PROC_E4ARDUINO_BKPT_16_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_MOV_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_CMP_REG_16_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_ADD_REG_16_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_POP_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_PUSH_16_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_LDRSH_REG_16_ET1((ctx), (op));                       \
        PROC_E4ARDUINO_LDRB_REG_16_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_LDRH_REG_16_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_LDR_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_LDRSB_REG_16_ET1((ctx), (op));                       \
        PROC_E4ARDUINO_STRB_REG_16_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_STRH_REG_16_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_STR_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SUB_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ADD_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SUB_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ADD_REG_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_CBNZ_16_ET1((ctx), (op));                            \
        PROC_E4ARDUINO_CBZ_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_B_16_ET2((ctx), (op));                               \
        PROC_E4ARDUINO_LDM_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_STM_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_ADD_SP_IMM_16_ET1((ctx), (op));                      \
        PROC_E4ARDUINO_ADR_16_ET1((ctx), (op));                             \
        PROC_E4ARDUINO_LDR_IMM_16_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_STR_IMM_16_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_LDRH_IMM_16_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_STRH_IMM_16_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_LDRB_IMM_16_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_STRB_IMM_16_ET1((ctx), (op));                        \
        PROC_E4ARDUINO_LDR_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_STR_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_LDR_LIT_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_SUB_IMM_16_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_ADD_IMM_16_ET2((ctx), (op));                         \
        PROC_E4ARDUINO_CMP_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_MOV_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_ASR_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_LSR_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_LSL_IMM_16_ET1((ctx), (op));                         \
        PROC_E4ARDUINO_B_16_ET1((ctx), (op));                               \
} while(0)

#endif /* INCLUDE_E4ARDUINO_DEFS_H */
