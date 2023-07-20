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

#ifndef INCLUDED_E4K64F_IMPL_H
#define INCLUDED_E4K64F_IMPL_H

#include "e4k64f_defs.h"

#define E4K64F_CPSR_NEGATIVE_FLAG   0x80000000
#define E4K64F_CPSR_ZERO_FLAG       0x40000000
#define E4K64F_CPSR_CARRY_FLAG      0x20000000
#define E4K64F_CPSR_OVERFLOW_FLAG   0x10000000

#define E4K64F_CONTROL_NPRIV        0x00000001
#define E4K64F_CONTROL_SPSEL        0x00000002
#define E4K64F_CONTROL_FPCA         0x00000004

#define E4K64F_ICTR_ADDR            0xe000e004
#define E4K64F_ACTLR_ADDR           0xe000e008
#define E4K64F_CPUID_ADDR           0xe000ed00
#define E4K64F_ICSR_ADDR            0xe000ed04
#define E4K64F_VTOR_ADDR            0xe000ed08
#define E4K64F_AIRCR_ADDR           0xe000ed0c
#define E4K64F_SCR_ADDR             0xe000ed10
#define E4K64F_CCR_ADDR             0xe000ed14
#define E4K64F_SHPR1_ADDR           0xe000ed18
#define E4K64F_SHPR2_ADDR           0xe000ed1c
#define E4K64F_SHPR3_ADDR           0xe000ed20
#define E4K64F_SHCSR_ADDR           0xe000ed24
#define E4K64F_CFSR_ADDR            0xe000ed28
#define E4K64F_HFSR_ADDR            0xe000ed2c
#define E4K64F_DFSR_ADDR            0xe000ed30
#define E4K64F_MMFAR_ADDR           0xe000ed34
#define E4K64F_BFAR_ADDR            0xe000ed38
#define E4K64F_AFSR_ADDR            0xe000ed3c
#define E4K64F_CPACR_ADDR           0xe000ed88

#define E4K64F_CCR_STKALIGN         0x00000200
#define E4K64F_FPCCR_LSPACT         0x00000001

#define E4K64F_EXCEPTION_RESET        1
#define E4K64F_EXCEPTION_NMI          2
#define E4K64F_EXCEPTION_HARDFAULT    3
#define E4K64F_EXCEPTION_SVC         11
#define E4K64F_EXCEPTION_PENDSV      14
#define E4K64F_EXCEPTION_SYSTICK     15
#define E4K64F_EXCEPTION_INTERRUPT   16

#define E4K64F_UINT(x) ((doubleword)(x))
#define E4K64F_SINT(x) ((doubleword)(((x & 0x80000000) == 0) ? (doubleword)(x) : (0xffffffff00000000 | (doubleword)(x))))

typedef enum {
    R0,  R1,  R2,  R3,  R4,  R5,  R6,  R7,
    R8,  R9, R10, R11, R12,  SP,  LR,  PC,
    CPSR, E4K64F_NUMBER_OF_REGISTERS
} e4k64f_register_alias_t;

static char *e4k64f_register_name[E4K64F_NUMBER_OF_REGISTERS] = {
    "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7",
    "r8", "r9", "r10", "r11", "r12", "sp", "lr", "pc",
    "cpsr"
};

typedef enum {
     S0,  S1,  S2,  S3,  S4,  S5,  S6,  S7,
     S8,  S9, S10, S11, S12, S13, S14, S15,
    S16, S17, S18, S19, S20, S21, S22, S23,
    S24, S25, S26, S27, S28, S29, S30, S31,
    E4K64F_NUMBER_OF_NEON_REGISTERS
} e4k64f_neon_register_alias_t;

static char *e4k64f_neon_register_name[E4K64F_NUMBER_OF_NEON_REGISTERS] = {
     "s0",  "s1",  "s2",  "s3",  "s4",  "s5",  "s6",  "s7",
     "s8",  "s9", "s10", "s11", "s12", "s13", "s14", "s15",
    "s16", "s17", "s18", "s19", "s20", "s21", "s22", "s23",
    "s24", "s25", "s26", "s27", "s28", "s29", "s30", "s31",
};

typedef enum {
    APSR, IAPSR, EAPSR, XPSR, _4, IPSR, EPSR, IEPSR, MSP, PSP, _10, _11, _12, _13, _14, _15,
    PRIMASK, BASEPRI, BASEPRI_MAX, FAULTMASK, CONTROL, E4K64F_NUMBER_OF_SPECIAL_REGISTERS
} e4k64f_special_register_alias_t;

static char *e4k64f_special_register_name[E4K64F_NUMBER_OF_SPECIAL_REGISTERS] =
{
    "APSR", "IAPSR", "EAPSR", "XPSR", "", "IPSR", "EPSR", "IEPSR", "MSP", "PSP", "", "", "", "", "", "",
    "PRIMASK", "BASEPRI", "BASEPRI_MAX", "FAULTMASK", "CONTROL"
};

typedef enum {
    E4K64F_PRIVILEGE_MODE_PRIVILEGED, E4K64F_PRIVILEGE_MODE_UNPRIVILEGED
} e4k64f_privilege_mode_t;

typedef enum {
    E4K64F_EXECUTION_MODE_THREAD, E4K64F_EXECUTION_MODE_HANDLER
} e4k64f_execution_mode_t;

typedef enum {
    E4K64F_SHIFT_TYPE_LSL, E4K64F_SHIFT_TYPE_LSL_C,
    E4K64F_SHIFT_TYPE_LSR, E4K64F_SHIFT_TYPE_LSR_C,
    E4K64F_SHIFT_TYPE_ASR, E4K64F_SHIFT_TYPE_ASR_C,
    E4K64F_SHIFT_TYPE_ROR, E4K64F_SHIFT_TYPE_ROR_C,
    E4K64F_SHIFT_TYPE_RRX, E4K64F_SHIFT_TYPE_RRX_C
} e4k64f_shift_type_t;

typedef struct {
    word *r;
    word bank0[E4K64F_NUMBER_OF_REGISTERS];
    word bank1[E4K64F_NUMBER_OF_REGISTERS];
    word neon_reg[E4K64F_NUMBER_OF_NEON_REGISTERS];
} e4k64f_register_t;

typedef struct {
    word r[E4K64F_NUMBER_OF_SPECIAL_REGISTERS];
} e4k64f_special_register_t;

typedef struct {
    int fd;
    byte *memory;
} e4k64f_memory_t;

struct e4k64f_context_t;
typedef struct e4k64f_context_t e4k64f_context_t;

struct e4k64f_context_t {
    byte parsed;
    word cycles;
    word wait_until;
    word wfi;
    word wfe;
    e4k64f_privilege_mode_t privilege_mode;
    e4k64f_execution_mode_t execution_mode;
    e4k64f_register_t registers;
    e4k64f_special_register_t special_registers;
    word exception_active[8];
    e4k64f_memory_t memory;
#ifdef ENABLE_XFT
    char *xft_library_path;
    int (*xft_setup)(e4k64f_context_t *context);
    int (*xft_dispatch)(word addr, e4k64f_context_t *context);
    int (*xft_teardown)(void);
#endif /* ENABLE_XFT */
};

word e4k64f_get_negative_flag(void *ctx);
word e4k64f_get_zero_flag(void *ctx);
word e4k64f_get_carry_flag(void *ctx);
word e4k64f_get_overflow_flag(void *ctx);
void e4k64f_set_negative_flag(void *ctx, word flag);
void e4k64f_set_zero_flag(void *ctx, word flag);
void e4k64f_set_carry_flag(void *ctx, word flag);
void e4k64f_set_overflow_flag(void *ctx, word flag);
word e4k64f_get_it_state(void *ctx);
void e4k64f_set_itstate(void *ctx, word state);
word e4k64f_in_it_block(void *ctx);
word e4k64f_in_last_it_block(void *ctx);
word e4k64f_to_be_executed(void *ctx);
word e4k64f_judge_condition(void *ctx, word cond);
void e4k64f_advance_itstate(void *ctx);
char *e4k64f_get_cond_string(void *ctx);
word e4k64f_is_privileged(void *ctx);
word e4k64f_current_privilege_mode(void *ctx);
word e4k64f_get_current_execution_mode(void *ctx);
void e4k64f_set_current_execution_mode(void *ctx, word mode);
word e4k64f_have_dsp_extention(void *ctx);
word e4k64f_execution_priority(void *ctx);
void e4k64f_undefined(void *ctx);
void e4k64f_unpredictable(void *ctx);
word e4k64f_zero_extend(word original, word nbits);
word e4k64f_sign_extend(word original, word nbits);
word e4k64f_thumb_expand_imm_c(word original, word *carry);
word e4k64f_add_with_carry(word n0, word n1, word *carry, word *overflow);
word e4k64f_shift(word n, e4k64f_shift_type_t type, word w, word *carry);
char *e4k64f_get_scr_register_name(word addr);
byte e4k64f_get_byte_from_memory(void *ctx, word addr);
void e4k64f_set_byte_to_memory(void *ctx, word addr, byte value);
halfword e4k64f_get_halfword_from_memory(void *ctx, word addr);
void e4k64f_set_halfword_to_memory(void *ctx, word addr, halfword value);
word e4k64f_get_word_from_memory(void *ctx, word addr);
void e4k64f_set_word_to_memory(void *ctx, word addr, word value);
void e4k64f_push_to_stack(void *ctx, word value);
word e4k64f_pop_from_stack(void *ctx);
word e4k64f_have_fpext(void *ctx);
void e4k64f_to_use_psp(void *ctx);
void e4k64f_to_use_msp(void *ctx);
void e4k64f_exception_entry(void *ctx, word exception, word return_addr);
void e4k64f_exception_return(void *ctx);
void e4k64f_handle_interrupt_pre_cycle(e4k64f_context_t *context);
void e4k64f_handle_interrupt_post_cycle(e4k64f_context_t *context);

void mov_reg_16_et2(void *ctx, halfword rm, halfword rd);
void lsl_imm_16_et1(void *ctx, halfword imm, halfword rm, halfword rd);
void lsr_imm_16_et1(void *ctx, halfword imm, halfword rm, halfword rd);
void asr_imm_16_et1(void *ctx, halfword imm, halfword rm, halfword rd);
void add_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rd);
void sub_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rd);
void add_imm_16_et1(void *ctx, halfword imm3, halfword rn, halfword rd);
void sub_imm_16_et1(void *ctx, halfword imm3, halfword rn, halfword rd);
void mov_imm_16_et1(void *ctx, halfword rd, halfword imm);
void cmp_imm_16_et1(void *ctx, halfword rn, halfword imm);
void add_imm_16_et2(void *ctx, halfword rdn, halfword imm);
void sub_imm_16_et2(void *ctx, halfword rdn, halfword imm);
void and_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void eor_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void lsl_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void lsr_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void asr_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void adc_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void sbc_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void ror_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void tst_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void rsb_imm_16_et1(void *ctx, halfword rn, halfword rd);
void cmp_reg_16_et1(void *ctx, halfword rm, halfword rn);
void cmn_reg_16_et1(void *ctx, halfword rm, halfword rn);
void orr_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void mul_16_et1(void *ctx, halfword rn, halfword rdm);
void bic_reg_16_et1(void *ctx, halfword rm, halfword rdn);
void mvn_reg_16_et1(void *ctx, halfword rm, halfword rd);
void add_reg_16_et2(void *ctx, halfword dn, halfword rm, halfword rdn);
void add_sp_reg_16_et1(void *ctx, halfword dm, halfword rdm);
void add_sp_reg_16_et2(void *ctx, halfword rm);
void cmp_reg_16_et2(void *ctx, halfword n, halfword rm, halfword rn);
void mov_reg_16_et1(void *ctx, halfword d, halfword rm, halfword rd);
void bx_16_et1(void *ctx, halfword rm);
void blx_reg_16_et1(void *ctx, halfword rm);
void ldr_lit_16_et1(void *ctx, halfword rt, halfword imm);
void str_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
void strh_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
void strb_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
void ldrsb_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
void ldr_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
void ldrh_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
void ldrb_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
void ldrsh_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt);
void str_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
void ldr_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
void strb_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
void ldrb_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
void strh_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
void ldrh_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt);
void str_imm_16_et2(void *ctx, halfword rt, halfword imm);
void ldr_imm_16_et2(void *ctx, halfword rt, halfword imm);
void adr_16_et1(void *ctx, halfword rd, halfword imm);
void add_sp_imm_16_et1(void *ctx, halfword rd, halfword imm);
void cps_16_et1(void *ctx, halfword im, halfword i, halfword f);
void add_sp_imm_16_et2(void *ctx, halfword imm);
void sub_sp_imm_16_et1(void *ctx, halfword imm);
void cbz_16_et1(void *ctx, halfword i, halfword imm, halfword rn);
void cbnz_16_et1(void *ctx, halfword i, halfword imm, halfword rn);
void sxth_16_et1(void *ctx, halfword rm, halfword rd);
void sxtb_16_et1(void *ctx, halfword rm, halfword rd);
void uxth_16_et1(void *ctx, halfword rm, halfword rd);
void uxtb_16_et1(void *ctx, halfword rm, halfword rd);
void push_16_et1(void *ctx, halfword m, halfword rl);
void rev_16_et1(void *ctx, halfword rm, halfword rd);
void rev16_16_et1(void *ctx, halfword rm, halfword rd);
void revsh_16_et1(void *ctx, halfword rm, halfword rd);
void pop_16_et1(void *ctx, halfword p, halfword rl);
void bkpt_16_et1(void *ctx, halfword imm);
void it_16_et1(void *ctx, halfword fc, halfword mask);
void nop_16_et1(void *ctx);
void yield_16_et1(void *ctx);
void wfe_16_et1(void *ctx);
void wfi_16_et1(void *ctx);
void sev_16_et1(void *ctx);
void stm_16_et1(void *ctx, halfword rn, halfword rl);
void ldm_16_et1(void *ctx, halfword rn, halfword rl);
void b_16_et1(void *ctx, halfword cond, halfword imm);
void udf_16_et1(void *ctx, halfword imm);
void svc_16_et1(void *ctx, halfword imm);
void b_16_et2(void *ctx, halfword imm);
void stm_32_et2(void *ctx, word w, word rn, word m, word rl);
void ldm_32_et2(void *ctx, word w, word rn, word p, word m, word rl);
void pop_32_et2(void *ctx, word p, word m, word rl);
void stmdb_32_et1(void *ctx, word w, word rn, word m, word rl);
void push_32_et2(void *ctx, word m, word rl);
void ldmdb_32_et1(void *ctx, word w, word rn, word p, word m, word rl);
void strex_32_et1(void *ctx, word rn, word rt, word rd, word imm);
void ldrex_32_et1(void *ctx, word rn, word rt, word imm);
void strd_imm_32_et1(void *ctx, word p, word u, word w, word rn, word rt, word rt2, word imm);
void ldrd_imm_32_et1(void *ctx, word p, word u, word w, word rn, word rt, word rt2, word imm);
void ldrd_lit_32_et1(void *ctx, word p, word u, word w, word rt, word rt2, word imm);
void strexb_32_et1(void *ctx, word rn, word rt, word rd);
void strexh_32_et1(void *ctx, word rn, word rt, word rd);
void tbb_32_et1(void *ctx, word rn, word rm);
void tbh_32_et1(void *ctx, word rn, word rm);
void ldrexb_32_et1(void *ctx, word rn, word rt);
void ldrexh_32_et1(void *ctx, word rn, word rt);
void and_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void tst_reg_32_et2(void *ctx, word rn, word imm3, word imm2, word type, word rm);
void bic_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void orr_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void mov_reg_32_et3(void *ctx, word s, word rd, word rm);
void lsl_imm_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word rm);
void lsr_imm_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word rm);
void asr_imm_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word rm);
void rrx_32_et1(void *ctx, word s, word rd, word rm);
void ror_imm_32_et1(void *ctx, word s, word imm3, word rd, word imm2, word rm);
void orn_reg_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void mvn_reg_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word type, word rm);
void eor_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void teq_reg_32_et1(void *ctx, word rn, word imm3, word imm2, word type, word rm);
void pkhbt_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word t, word rm);
void pkhtb_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word t, word rm);
void add_reg_32_et3(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void add_sp_reg_32_et3(void *ctx, word s, word imm3, word rd, word imm2, word type, word rm);
void cmn_reg_32_et2(void *ctx, word rn, word imm3, word imm2, word type, word rm);
void adc_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void sbc_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void sub_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void sub_sp_reg_32_et1(void *ctx, word s, word imm3, word rd, word imm2, word type, word rm);
void cmp_reg_32_et3(void *ctx, word rn, word imm3, word imm2, word type, word rm);
void rsb_reg_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm);
void stc_32_et1(void *ctx, word p, word u, word n, word w, word rn, word crd, word cprc, word imm8);
void stc2_32_et2(void *ctx, word p, word u, word n, word w, word rn, word crd, word cprc, word imm8);
void ldc_imm_32_et1(void *ctx, word p, word u, word d, word w, word rn, word crd, word cprc, word imm8);
void ldc2_imm_32_et2(void *ctx, word p, word u, word d, word w, word rn, word crd, word cprc, word imm8);
void ldc_lit_32_et1(void *ctx, word p, word u, word d, word w, word crd, word cprc, word imm8);
void ldc2_lit_32_et2(void *ctx, word p, word u, word d, word w, word crd, word cprc, word imm8);
void mcrr_32_et1(void *ctx, word rt2, word rt, word cprc, word opc1, word crm);
void mcrr2_32_et2(void *ctx, word rt2, word rt, word cprc, word opc1, word crm);
void mrrc_32_et1(void *ctx, word rt2, word rt, word cprc, word opc1, word crm);
void mrrc2_32_et2(void *ctx, word rt2, word rt, word cprc, word opc1, word crm);
void cdp_32_et1(void *ctx, word opc1, word crn, word crd, word cprc, word opc2, word crm);
void cdp2_32_et2(void *ctx, word opc1, word crn, word crd, word cprc, word opc2, word crm);
void mcr_32_et1(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm);
void mcr2_32_et2(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm);
void mrc_32_et1(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm);
void mrc2_32_et2(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm);
void and_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void tst_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word imm8);
void bic_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void orr_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void mov_imm_32_et2(void *ctx, word i, word s, word imm3, word rd, word imm8);
void orn_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void mvn_imm_32_et1(void *ctx, word i, word s, word imm3, word rd, word imm8);
void eor_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void teq_imm_32_et1(void *ctx, word i, word rn, word imm3, word imm8);
void add_imm_32_et3(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void add_sp_imm_32_et3(void *ctx, word i, word s, word imm3, word rd, word imm8);
void cmn_imm_32_et1(void *ctx, word i, word rn, word imm3, word imm8);
void adc_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void sbc_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void sub_imm_32_et3(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void sub_sp_imm_32_et2(void *ctx, word i, word s, word imm3, word rd, word imm8);
void cmp_imm_32_et2(void *ctx, word i, word rn, word imm3, word imm8);
void rsb_imm_32_et2(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8);
void add_imm_32_et4(void *ctx, word i, word rn, word imm3, word rd, word imm8);
void add_sp_imm_32_et4(void *ctx, word i, word imm3, word rd, word imm8);
void adr_32_et2(void *ctx, word i, word imm3, word rd, word imm8);
void mov_imm_32_et3(void *ctx, word i, word imm4, word imm3, word rd, word imm8);
void sub_imm_32_et4(void *ctx, word i, word rn, word imm3, word rd, word imm8);
void sub_sp_imm_32_et3(void *ctx, word i, word imm3, word rd, word imm8);
void adr_32_et3(void *ctx, word i, word imm3, word rd, word imm8);
void movt_32_et1(void *ctx, word i, word imm4, word imm3, word rd, word imm8);
void ssat_32_et1(void *ctx, word sh, word rn, word imm3, word rd, word imm2, word si);
void ssat16_32_et1(void *ctx, word rn, word rd, word si);
void sbfx_32_et1(void *ctx, word rn, word imm3, word rd, word imm2, word wm);
void bfi_32_et1(void *ctx, word rn, word imm3, word rd, word imm2, word msb);
void bfc_32_et1(void *ctx, word imm3, word rd, word imm2, word msb);
void usat_32_et1(void *ctx, word sh, word rn, word imm3, word rd, word imm2, word si);
void usat16_32_et1(void *ctx, word rn, word rd, word si);
void ubfx_32_et1(void *ctx, word rn, word imm3, word rd, word imm2, word wm);
void b_32_et3(void *ctx, word s, word cond, word imm6, word j1, word j2, word imm11);
void msr_32_et1(void *ctx, word rn, word mask, word sysm);
void nop_32_et2(void *ctx);
void yield_32_et2(void *ctx);
void wfe_32_et2(void *ctx);
void wfi_32_et2(void *ctx);
void sev_32_et2(void *ctx);
void dbg_32_et1(void *ctx, word opt);
void clrex_32_et1(void *ctx);
void dsb_32_et1(void *ctx, word opt);
void dmb_32_et1(void *ctx, word opt);
void isb_32_et1(void *ctx, word opt);
void mrs_32_et1(void *ctx, word rd, word sysm);
void udf_32_et2(void *ctx, word imm4, word imm12);
void b_32_et4(void *ctx, word s, word imm10, word j1, word j2, word imm11);
void bl_32_et1(void *ctx, word s, word imm10, word j1, word j2, word imm11);
void strb_imm_32_et2(void *ctx, word rn, word rt, word imm12);
void strb_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
void strbt_32_et1(void *ctx, word rn, word rt, word imm8);
void strb_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
void strh_imm_32_et2(void *ctx, word rn, word rt, word imm12);
void strh_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
void strht_32_et1(void *ctx, word rn, word rt, word imm8);
void strh_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
void str_imm_32_et3(void *ctx, word rn, word rt, word imm12);
void str_imm_32_et4(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
void strt_32_et1(void *ctx, word rn, word rt, word imm8);
void push_32_et3(void *ctx, word rt);
void str_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
void ldrb_lit_32_et1(void *ctx, word u, word rt, word rmm12);
void ldrb_imm_32_et2(void *ctx, word rn, word rt, word imm12);
void ldrb_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
void ldrbt_32_et1(void *ctx, word rn, word rt, word imm8);
void ldrb_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
void ldrsb_lit_32_et1(void *ctx, word u, word rt, word imm12);
void ldrsb_imm_32_et1(void *ctx, word rn, word rt, word imm12);
void ldrsb_imm_32_et2(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
void ldrsbt_32_et1(void *ctx, word rn, word rt, word imm8);
void ldrsb_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
void pld_lit_32_et1(void *ctx, word u, word imm12);
void pld_imm_32_et1(void *ctx, word rn, word imm12);
void pld_imm_32_et2(void *ctx, word rn, word imm8);
void pld_reg_32_et1(void *ctx, word rn, word sft, word rm);
void pli_imm_32_et1(void *ctx, word rn, word imm12);
void pli_imm_32_et2(void *ctx, word rn, word imm8);
void pli_lit_32_et3(void *ctx, word u, word imm12);
void pli_reg_32_et1(void *ctx, word rn, word imm2, word rm);
void ldrh_lit_32_et1(void *ctx, word u, word rt, word imm12);
void ldrh_imm_32_et2(void *ctx, word rn, word rt, word imm12);
void ldrh_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
void ldrh_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
void ldrht_32_et1(void *ctx, word rn, word rt, word imm8);
void ldrsh_imm_32_et1(void *ctx, word rn, word rt, word imm12);
void ldrsh_imm_32_et2(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
void ldrsh_lit_32_et1(void *ctx, word u, word rt, word imm12);
void ldrsh_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
void ldrsht_32_et1(void *ctx, word rn, word rt, word imm8);
void ldr_imm_32_et3(void *ctx, word rn, word rt, word imm12);
void ldr_imm_32_et4(void *ctx, word rn, word rt, word p, word u, word w, word imm8);
void pop_32_et3(void *ctx, word rt);
void ldrt_32_et1(void *ctx, word rn, word rt, word imm8);
void ldr_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm);
void ldr_lit_32_et2(void *ctx, word u, word rt, word imm12);
void lsl_reg_32_et2(void *ctx, word s, word rn, word rd, word rm);
void lsr_reg_32_et2(void *ctx, word s, word rn, word rd, word rm);
void asr_reg_32_et2(void *ctx, word s, word rn, word rd, word rm);
void ror_reg_32_et2(void *ctx, word s, word rn, word rd, word rm);
void sxtah_32_et1(void *ctx, word rn, word rd, word rot, word rm);
void sxth_32_et2(void *ctx, word rd, word rot, word rm);
void uxtah_32_et1(void *ctx, word rn, word rd, word rot, word rm);
void uxth_32_et2(void *ctx, word rd, word rot, word rm);
void sxtab16_32_et1(void *ctx, word rn, word rd, word rot, word rm);
void sxtb16_32_et1(void *ctx, word rd, word rot, word rm);
void uxtab16_32_et1(void *ctx, word rn, word rd, word rot, word rm);
void uxtb16_32_et1(void *ctx, word rd, word rot, word rm);
void sxtab_32_et1(void *ctx, word rn, word rd, word rot, word rm);
void sxtb_32_et2(void *ctx, word rd, word rot, word rm);
void uxtab_32_et1(void *ctx, word rn, word rd, word rot, word rm);
void uxtb_32_et2(void *ctx, word rd, word rot, word rm);
void sadd16_32_et1(void *ctx, word rn, word rd, word rm);
void sasx_32_et1(void *ctx, word rn, word rd, word rm);
void ssax_32_et1(void *ctx, word rn, word rd, word rm);
void ssub16_32_et1(void *ctx, word rn, word rd, word rm);
void sadd8_32_et1(void *ctx, word rn, word rd, word rm);
void ssub8_32_et1(void *ctx, word rn, word rd, word rm);
void qadd16_32_et1(void *ctx, word rn, word rd, word rm);
void qasx_32_et1(void *ctx, word rn, word rd, word rm);
void qsax_32_et1(void *ctx, word rn, word rd, word rm);
void qsub16_32_et1(void *ctx, word rn, word rd, word rm);
void qadd8_32_et1(void *ctx, word rn, word rd, word rm);
void qsub8_32_et1(void *ctx, word rn, word rd, word rm);
void shadd16_32_et1(void *ctx, word rn, word rd, word rm);
void uadd16_32_et1(void *ctx, word rn, word rd, word rm);
void uqadd16_32_et1(void *ctx, word rn, word rd, word rm);
void uhadd16_32_et1(void *ctx, word rn, word rd, word rm);
void rev_32_et2(void *ctx, word rm, word rd, word rm2);
void rev16_32_et2(void *ctx, word rm, word rd, word rm2);
void rbit_32_et1(void *ctx, word rm, word rd, word rm2);
void revsh_32_et2(void *ctx, word rm, word rd, word rm2);
void shasx_32_et1(void *ctx, word rn, word rd, word rm);
void uasx_32_et1(void *ctx, word rn, word rd, word rm);
void uqasx_32_et1(void *ctx, word rn, word rd, word rm);
void uhasx_32_et1(void *ctx, word rn, word rd, word rm);
void sel_32_et1(void *ctx, word rn, word rd, word rm);
void clz_32_et1(void *ctx, word rm, word rd, word rm2);
void shsax_32_et1(void *ctx, word rn, word rd, word rm);
void usax_32_et1(void *ctx, word rn, word rd, word rm);
void uqsax_32_et1(void *ctx, word rn, word rd, word rm);
void uhsax_32_et1(void *ctx, word rn, word rd, word rm);
void shsub16_32_et1(void *ctx, word rn, word rd, word rm);
void usub16_32_et1(void *ctx, word rn, word rd, word rm);
void uqsub16_32_et1(void *ctx, word rn, word rd, word rm);
void uhsub16_32_et1(void *ctx, word rn, word rd, word rm);
void shadd8_32_et1(void *ctx, word rn, word rd, word rm);
void uadd8_32_et1(void *ctx, word rn, word rd, word rm);
void uqadd8_32_et1(void *ctx, word rn, word rd, word rm);
void uhadd8_32_et1(void *ctx, word rn, word rd, word rm);
void qadd_32_et1(void *ctx, word rn, word rd, word rm);
void qdadd_32_et1(void *ctx, word rn, word rd, word rm);
void qsub_32_et1(void *ctx, word rn, word rd, word rm);
void qdsub_32_et1(void *ctx, word rn, word rd, word rm);
void shsub8_32_et1(void *ctx, word rn, word rd, word rm);
void usub8_32_et1(void *ctx, word rn, word rd, word rm);
void uqsub8_32_et1(void *ctx, word rn, word rd, word rm);
void uhsub8_32_et1(void *ctx, word rn, word rd, word rm);
void mla_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void mul_32_et2(void *ctx, word rn, word rd, word rm);
void mls_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smlabb_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smlabt_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smlatb_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smlatt_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smulbb_32_et1(void *ctx, word rn, word rd, word rm);
void smulbt_32_et1(void *ctx, word rn, word rd, word rm);
void smultb_32_et1(void *ctx, word rn, word rd, word rm);
void smultt_32_et1(void *ctx, word rn, word rd, word rm);
void smlad_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smladx_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smuad_32_et1(void *ctx, word rn, word rd, word rm);
void smuadx_32_et1(void *ctx, word rn, word rd, word rm);
void smlawb_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smlawt_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smulwb_32_et1(void *ctx, word rn, word rd, word rm);
void smulwt_32_et1(void *ctx, word rn, word rd, word rm);
void smlsd_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smlsdx_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smusd_32_et1(void *ctx, word rn, word rd, word rm);
void smusdx_32_et1(void *ctx, word rn, word rd, word rm);
void smmla_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smmlar_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smmul_32_et1(void *ctx, word rn, word rd, word rm);
void smmulr_32_et1(void *ctx, word rn, word rd, word rm);
void smmls_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smmlsr_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void usad8_32_et1(void *ctx, word rn, word rd, word rm);
void usada8_32_et1(void *ctx, word rn, word ra, word rd, word rm);
void smull_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void sdiv_32_et1(void *ctx, word rn, word rd, word rm);
void umull_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void udiv_32_et1(void *ctx, word rn, word rd, word rm);
void smlal_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void smlalbb_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void smlalbt_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void smlaltb_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void smlaltt_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void smlald_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void smlaldx_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void smlsld_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void smlsldx_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void umlal_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void umaal_32_et1(void *ctx, word rn, word rdl, word rdh, word rm);
void vsel_32_et1(void *ctx, word d, word cc, word vn, word vd, word sz, word n, word m, word vm);
void vmla_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vmls_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vnmla_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vnmls_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vnmul_32_et2(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vmul_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vadd_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vsub_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vdiv_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vmaxnm_32_et1(void *ctx, word d, word vn, word vd, word sz, word n, word m, word vm);
void vminnm_32_et1(void *ctx, word d, word vn, word vd, word sz, word n, word m, word vm);
void vmov_imm_32_et1(void *ctx, word d, word imm4h, word vd, word imm4l);
void vmov_reg_32_et1(void *ctx, word d, word vd, word m, word vm);
void vabs_32_et1(void *ctx, word d, word vd, word m, word vm);
void vneg_32_et1(void *ctx, word d, word vd, word m, word vm);
void vsqrt_32_et1(void *ctx, word d, word vd, word m, word vm);
void vcvtb_32_et1(void *ctx, word d, word op, word vd, word sz, word m, word vm);
void vcvtt_32_et1(void *ctx, word d, word op, word vd, word sz, word m, word vm);
void vcmp_32_et1(void *ctx, word d, word vd, word sz, word e, word m, word vm);
void vcmpe_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
void vcmp_32_et2(void *ctx, word d, word vd, word sz, word e);
void vcmpe_32_et2(void *ctx, word d, word vd, word sz);
void vrintx_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
void vrintz_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
void vrintr_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
void vcvt_dp_sp_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
void vcvt_fp_int_32_et1(void *ctx, word d, word rm, word vd, word sz, word m, word vm);
void vcvtr_fp_int_32_et1(void *ctx, word d, word rm, word vd, word sz, word m, word vm);
void vrinta_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
void vrintn_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
void vrintp_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
void vrintm_32_et1(void *ctx, word d, word vd, word sz, word m, word vm);
void vcvta_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm);
void vcvtn_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm);
void vcvtp_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm);
void vcvtm_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm);
void vcvt_fp_fix_32_et1(void *ctx, word d, word op, word u, word vd, word sf, word sx, word i, word imm);
void vmov_reg_sp_32_et1(void *ctx, word op, word vn, word rt, word n);
void vmsr_32_et1(void *ctx, word rt);
void vmov_reg_scl_32_et1(void *ctx, word h, word vd, word rt, word d);
void vfma_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vfms_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vfnma_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vfnms_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm);
void vmrs_32_et1(void *ctx, word rt);
void vmov_scl_reg_32_et1(void *ctx, word h, word vn, word rt, word n);
void vmov_reg_sp_sp_32_et1(void *ctx, word op, word rt2, word rt, word m, word vm);
void vmov_reg_dp_32_et1(void *ctx, word op, word rt2, word rt, word m, word vm);
void vstm_32_et1(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8);
void vstm_32_et2(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8);
void vstr_32_et1(void *ctx, word u, word d, word rn, word vd, word imm8);
void vstr_32_et2(void *ctx, word u, word d, word rn, word vd, word imm8);
void vpush_32_et1(void *ctx, word d, word vd, word imm8);
void vpush_32_et2(void *ctx, word d, word vd, word imm8);
void vldm_32_et1(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8);
void vldm_32_et2(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8);
void vpop_32_et1(void *ctx, word d, word vd, word imm8);
void vpop_32_et2(void *ctx, word d, word vd, word imm8);
void vldr_32_et1(void *ctx, word u, word d, word rn, word vd, word imm8);
void vldr_32_et2(void *ctx, word u, word d, word rn, word vd, word imm8);

#endif /* INCLUDED_E4K64F_IMPL_H */
