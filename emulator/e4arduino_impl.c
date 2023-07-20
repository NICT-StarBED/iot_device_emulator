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

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "e4arduino_defs.h"
#include "e4arduino_log.h"
#include "e4arduino_port.h"
#include "e4arduino_impl.h"

char *e4arduino_cond_name[] = {
    "eq", "ne", "cs", "cc", "mi", "pl", "vs", "vc",
    "hi", "ls", "ge", "lt", "gt", "le", ""
};

typedef union {
    float fp32;
    int32_t i32;
    u_int32_t ui32;
} e4arduino_data32;

typedef union {
    float fp64;
    int64_t i64;
    u_int64_t ui64;
    struct {
        int32_t i32_l;
        int32_t i32_h;
    };
    struct {
        u_int32_t ui32_l;
        u_int32_t ui32_h;
    };
} e4arduino_data64;

word
e4arduino_get_negative_flag(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    return ((context->registers.r[CPSR] & E4ARDUINO_CPSR_NEGATIVE_FLAG) != 0);
}

word
e4arduino_get_zero_flag(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    return ((context->registers.r[CPSR] & E4ARDUINO_CPSR_ZERO_FLAG) != 0);
}

word
e4arduino_get_carry_flag(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    return ((context->registers.r[CPSR] & E4ARDUINO_CPSR_CARRY_FLAG) != 0);
}

word
e4arduino_get_overflow_flag(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    return ((context->registers.r[CPSR] & E4ARDUINO_CPSR_OVERFLOW_FLAG) != 0);
}

void
e4arduino_set_negative_flag(void *ctx, word flag)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(flag == 0) {
        context->registers.r[CPSR] &= ~E4ARDUINO_CPSR_NEGATIVE_FLAG;
    } else {
        context->registers.r[CPSR] |= E4ARDUINO_CPSR_NEGATIVE_FLAG;
    }
}

void
e4arduino_set_zero_flag(void *ctx, word flag)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(flag == 0) {
        context->registers.r[CPSR] &= ~E4ARDUINO_CPSR_ZERO_FLAG;
    } else {
        context->registers.r[CPSR] |= E4ARDUINO_CPSR_ZERO_FLAG;
    }
}

void
e4arduino_set_carry_flag(void *ctx, word flag)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(flag == 0) {
        context->registers.r[CPSR] &= ~E4ARDUINO_CPSR_CARRY_FLAG;
    } else {
        context->registers.r[CPSR] |= E4ARDUINO_CPSR_CARRY_FLAG;
    }
}

void
e4arduino_set_overflow_flag(void *ctx, word flag)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(flag == 0) {
        context->registers.r[CPSR] &= ~E4ARDUINO_CPSR_OVERFLOW_FLAG;
    } else {
        context->registers.r[CPSR] |= E4ARDUINO_CPSR_OVERFLOW_FLAG;
    }
}

word
e4arduino_get_it_state(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    value = context->registers.r[CPSR];
    return ((value & 0x06000000) >> 25) | ((value & 0x0000f000) >> 8) | ((value & 0x00000c00) >> 8);
}

void
e4arduino_set_itstate(void *ctx, word state)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    context->registers.r[CPSR] = (context->registers.r[CPSR] & ~0x0600fc00) | ((state & 0x000000f0) << 8) | ((state & 0x0000000c) << 8) | ((state & 0x00000003) << 25);
}

word
e4arduino_in_it_block(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    return ((e4arduino_get_it_state(context) & 0x0000000f) != 0x00000000);
}

word
e4arduino_in_last_it_block(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    return ((e4arduino_get_it_state(context) & 0x0000000f) == 0x00000008);
}

word
e4arduino_to_be_executed(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word state, result;

    state = e4arduino_get_it_state(context);
    switch((state & 0x000000e0) >> 5) {
    case 0:
        result = (e4arduino_get_zero_flag(context) != 0);
        break;
    case 1:
        result = (e4arduino_get_carry_flag(context) != 0);
        break;
    case 2:
        result = (e4arduino_get_negative_flag(context) != 0);
        break;
    case 3:
        result = (e4arduino_get_overflow_flag(context) != 0);
        break;
    case 4:
        result = ((e4arduino_get_carry_flag(context) != 0) && (e4arduino_get_zero_flag(context) == 0));
        break;
    case 5:
        result = (e4arduino_get_negative_flag(context) == e4arduino_get_overflow_flag(context));
        break;
    case 6:
        result = ((e4arduino_get_negative_flag(context) == e4arduino_get_overflow_flag(context)) && (e4arduino_get_zero_flag(context) == 0));
        break;
    default:
        result = (0 == 0);
        break;
    }
    if(((state & 0x00000010) == 0x00000010) && ((state & 0x000000f0) != 0x000000f0)) {
        result = (result == 0);
    }
    return result;
}

word
e4arduino_judge_condition(void *ctx, word cond)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word result = 0;

    switch(cond) {
    case 0x0:
        result = (e4arduino_get_zero_flag(context) != 0);
        break;
    case 0x1:
        result = (e4arduino_get_zero_flag(context) == 0);
        break;
    case 0x2:
        result = (e4arduino_get_carry_flag(context) != 0);
        break;
    case 0x3:
        result = (e4arduino_get_carry_flag(context) == 0);
        break;
    case 0x4:
        result = (e4arduino_get_negative_flag(context) != 0);
        break;
    case 0x5:
        result = (e4arduino_get_negative_flag(context) == 0);
        break;
    case 0x6:
        result = (e4arduino_get_overflow_flag(context) != 0);
        break;
    case 0x7:
        result = (e4arduino_get_overflow_flag(context) == 0);
        break;
    case 0x8:
        result = ((e4arduino_get_carry_flag(context) != 0) && (e4arduino_get_zero_flag(context) == 0));
        break;
    case 0x9:
        result = ((e4arduino_get_carry_flag(context) == 0) || (e4arduino_get_zero_flag(context) != 0));
        break;
    case 0xa:
        result = (e4arduino_get_negative_flag(context) == e4arduino_get_overflow_flag(context));
        break;
    case 0xb:
        result = (e4arduino_get_negative_flag(context) != e4arduino_get_overflow_flag(context));
        break;
    case 0xc:
        result = ((e4arduino_get_zero_flag(context) == 0) && (e4arduino_get_negative_flag(context) == e4arduino_get_overflow_flag(context)));
        break;
    case 0xd:
        result = ((e4arduino_get_zero_flag(context) != 0) || (e4arduino_get_negative_flag(context) != e4arduino_get_overflow_flag(context)));
        break;
    case 0xe:
        result = (0 == 0);
        break;
    }
    return result;
}

void
e4arduino_advance_itstate(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word state;

    state = e4arduino_get_it_state(context);
    if((state & 0x00000007) == 0x00000000) {
        e4arduino_set_itstate(context, 0x00000000);
    } else {
        e4arduino_set_itstate(context, (state & 0x000000e0) | ((state & 0x0000000f) << 1));
    }
}

char *
e4arduino_get_cond_string(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char *result;
    word state;

    if(e4arduino_in_it_block(context)) {
        state = e4arduino_get_it_state(context);
        result = e4arduino_cond_name[((state & 0x000000f0) >> 4)];
    } else {
        result = e4arduino_cond_name[14];
    }
    return result;
}

word
e4arduino_is_privileged(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    return ((context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_NPRIV) == E4ARDUINO_PRIVILEGE_MODE_PRIVILEGED);
}

word
e4arduino_current_privilege_mode(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    return context->privilege_mode;
}

word
e4arduino_get_current_execution_mode(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    return context->execution_mode;
}

void
e4arduino_set_current_execution_mode(void *ctx, word mode)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    context->execution_mode = mode;
}

word
e4arduino_have_dsp_extention(void *ctx)
{

    return 0;
}

word
e4arduino_execution_priority(void *ctx)
{

    return 0;
}

void
e4arduino_undefined(void *ctx)
{

    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "undefined");
}

void
e4arduino_unpredictable(void *ctx)
{

    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "unpredictable");
}

word
e4arduino_zero_extend(word original, word nbits)
{
    doubleword value;

    value = original & (((doubleword)1 << nbits) - 1);
    return (word)value;
}

word
e4arduino_sign_extend(word original, word nbits)
{
    doubleword value;

    value = original & (((doubleword)1 << nbits) - 1);
    if((original & ((doubleword)1 << (nbits - 1))) != 0) {
        value = (doubleword)-1 - (~original & (((doubleword)1 << (nbits - 1)) - 1));
    }
    return (word)value;
}

word
e4arduino_thumb_expand_imm_c(word original, word *carry)
{
    word temp, shift, value;

    value = 0;
    if((original & 0x0c00) == 0) {
        switch(original & 0x0300) {
        case 0x0000:
            value = e4arduino_zero_extend(original, 32);
            break;
        case 0x0100:
            value = ((original & 0xff) << 16) | ((original & 0xff) << 0);
            break;
        case 0x0200:
            value = ((original & 0xff) << 24) | ((original & 0xff) << 8);
            break;
        case 0x0300:
            value = ((original & 0xff) << 24) | ((original & 0xff) << 16) | ((original & 0xff) << 8) | ((original & 0xff) << 0);
            break;
        }
    } else {
        temp = e4arduino_zero_extend(((original & 0x7f) | 0x80), 32);
        shift = (original & 0x0f80) >> 7;
        value = ((temp & ((1 << shift) - 1)) << (32 - shift)) | (temp >> shift);
        if(carry != NULL) {
            *carry = (value & 0x80000000) >> 31;
        }
    }
    return value;
}

word
e4arduino_add_with_carry(word n0, word n1, word *carry, word *overflow)
{
    word result;
    doubleword ne0, ne1, usum, ssum;

    ne0 = E4ARDUINO_SINT(n0);
    ne1 = E4ARDUINO_SINT(n1);
    if(carry != NULL) {
        usum = (doubleword)E4ARDUINO_UINT(n0) + (doubleword)E4ARDUINO_UINT(n1) + (doubleword)E4ARDUINO_UINT(*carry);
        ssum = (doubleword)E4ARDUINO_SINT(ne0) + (doubleword)E4ARDUINO_SINT(ne1) + (doubleword)E4ARDUINO_UINT(*carry);
    } else {
        usum = (doubleword)E4ARDUINO_UINT(n0) + (doubleword)E4ARDUINO_UINT(n1);
        ssum = (doubleword)E4ARDUINO_SINT(ne0) + (doubleword)E4ARDUINO_SINT(ne1);
    }
    result = usum & 0xffffffff;
    if(carry != NULL) {
        *carry = (E4ARDUINO_UINT(result) != usum);
    }
    if(overflow != NULL) {
        *overflow = (E4ARDUINO_SINT(result) != ssum);
    }
    return result;
}

word
e4arduino_shift(word n, e4arduino_shift_type_t type, word w, word *carry)
{
    word sign_mask, carry_in;

    if((type == E4ARDUINO_SHIFT_TYPE_RRX_C) && (w == 1)) {
        return n;
    } else if(w == 0) {
        return n;
    }
    switch(type) {
    case E4ARDUINO_SHIFT_TYPE_LSL_C:
        if(w < 32) {
            if(carry != NULL) {
                if(w == 0) {
                    *carry = 0;
                } else {
                    *carry = ((n & (1 << (32 - w))) != 0) ? 1 : 0;
                }
            }
            n <<= w;
        } else if(w == 32) {
            if(carry != NULL) {
                *carry = ((n & 1) != 0) ? 1 : 0;
            }
            n = 0;
        } else {
            if(carry != NULL) {
                *carry = 0;
            }
            n = 0;
        }
        break;
    case E4ARDUINO_SHIFT_TYPE_LSL:
        if(w < 32) {
            n <<= w;
        } else {
            n = 0;
        }
        break;
    case E4ARDUINO_SHIFT_TYPE_LSR_C:
        if(w < 32) {
            if(carry != NULL) {
                if(w == 0) {
                    *carry = 0;
                } else {
                    *carry = ((n & (1 << (w - 1))) != 0) ? 1 : 0;
                }
            }
            n >>= w;
        } else if(w == 32) {
            if(carry != NULL) {
                *carry = ((n & 1) != 0) ? 1 : 0;
            }
            n = 0;
        } else {
            if(carry != NULL) {
                *carry = 0;
            }
            n = 0;
        }
        break;
    case E4ARDUINO_SHIFT_TYPE_LSR:
        if(w < 32) {
            n >>= w;
        } else {
            n = 0;
        }
        break;
    case E4ARDUINO_SHIFT_TYPE_ASR_C:
        if(w < 32) {
            if((n & (1 << 31)) != 0) {
                sign_mask = ((1 << w) - 1) << (32 - w);
            } else {
                sign_mask = 0;
            }
            if(carry != NULL) {
                if(w == 0) {
                    *carry = 0;
                } else {
                    *carry = ((n & (1 << (w - 1))) != 0) ? 1 : 0;
                }
            }
            n = (n >> w) | sign_mask;
        } else if(w == 32) {
            if(carry != NULL) {
                *carry = ((n & (1 << 31)) != 0) ? 1 : 0;
            }
            n = 0;
        } else {
            if((n & (1 << 31)) != 0) {
                if(carry != NULL) {
                    *carry = 1;
                }
                n = ~0;
            } else {
                if(carry != NULL) {
                    *carry = 0;
                }
                n = 0;
            }
        }
        break;
    case E4ARDUINO_SHIFT_TYPE_ASR:
        if(w < 32) {
            if((n & (1 << 31)) != 0) {
                sign_mask = ((1 << w) - 1) << (32 - w);
            } else {
                sign_mask = 0;
            }
            n = (n >> w) | sign_mask;
        } else {
            if((n & (1 << 31)) != 0) {
                n = ~0;
            } else {
                n = 0;
            }
        }
        break;
    case E4ARDUINO_SHIFT_TYPE_ROR_C:
        if(carry != NULL) {
            *carry = n & 0x00000001;
        }
        n = (n >> w) | (n << (32 - w));
        break;
    case E4ARDUINO_SHIFT_TYPE_ROR:
        n = (n >> w) | (n << (32 - w));
        break;
    case E4ARDUINO_SHIFT_TYPE_RRX_C:
        carry_in = *carry;
        if(carry != NULL) {
            *carry = n & 0x00000001;
        }
        n = (carry_in << 31) | (n >> 1);
        break;
    case E4ARDUINO_SHIFT_TYPE_RRX:
        carry_in = *carry;
        n = (carry_in << 31) | (n >> 1);
        break;
    default:
        break;
    }
    return n;
}

char *
e4arduino_get_scr_register_name(word addr)
{

    switch(addr) {
    case 0xe000e004:    return "ICTR";
    case 0xe000e008:    return "ACTLR";
    case 0xe000e010:    return "STCSR";
    case 0xe000e014:    return "STRVR";
    case 0xe000e018:    return "STCVR";
    case 0xe000e01c:    return "STCR";
    case 0xe000e100:    return "ISER0";
    case 0xe000e104:    return "ISER1";
    case 0xe000e108:    return "ISER2";
    case 0xe000e10c:    return "ISER3";
    case 0xe000e110:    return "ISER4";
    case 0xe000e114:    return "ISER5";
    case 0xe000e118:    return "ISER6";
    case 0xe000e11c:    return "ISER7";
    case 0xe000e180:    return "ICER0";
    case 0xe000e184:    return "ICER1";
    case 0xe000e188:    return "ICER2";
    case 0xe000e18c:    return "ICER3";
    case 0xe000e190:    return "ICER4";
    case 0xe000e194:    return "ICER5";
    case 0xe000e198:    return "ICER6";
    case 0xe000e19c:    return "ICER7";
    case 0xe000e200:    return "ISPR0";
    case 0xe000e204:    return "ISPR1";
    case 0xe000e208:    return "ISPR2";
    case 0xe000e20c:    return "ISPR3";
    case 0xe000e210:    return "ISPR4";
    case 0xe000e214:    return "ISPR5";
    case 0xe000e218:    return "ISPR6";
    case 0xe000e21c:    return "ISPR7";
    case 0xe000e280:    return "ICPR0";
    case 0xe000e284:    return "ICPR1";
    case 0xe000e288:    return "ICPR2";
    case 0xe000e28c:    return "ICPR3";
    case 0xe000e290:    return "ICPR4";
    case 0xe000e294:    return "ICPR5";
    case 0xe000e298:    return "ICPR6";
    case 0xe000e29c:    return "ICPR7";
    case 0xe000e300:    return "IABR0";
    case 0xe000e304:    return "IABR1";
    case 0xe000e308:    return "IABR2";
    case 0xe000e30c:    return "IABR3";
    case 0xe000e310:    return "IABR4";
    case 0xe000e314:    return "IABR5";
    case 0xe000e318:    return "IABR6";
    case 0xe000e31c:    return "IABR7";
    case 0xe000e400:    return "IPR0";
    case 0xe000e404:    return "IPR1";
    case 0xe000e408:    return "IPR2";
    case 0xe000e40c:    return "IPR3";
    case 0xe000e410:    return "IPR4";
    case 0xe000e414:    return "IPR5";
    case 0xe000e418:    return "IPR6";
    case 0xe000e41c:    return "IPR7";
    case 0xe000e420:    return "IPR8";
    case 0xe000e424:    return "IPR9";
    case 0xe000e428:    return "IPR10";
    case 0xe000e42c:    return "IPR11";
    case 0xe000e430:    return "IPR12";
    case 0xe000e434:    return "IPR13";
    case 0xe000e438:    return "IPR14";
    case 0xe000e43c:    return "IPR15";
    case 0xe000e440:    return "IPR16";
    case 0xe000e444:    return "IPR17";
    case 0xe000e448:    return "IPR18";
    case 0xe000e44c:    return "IPR19";
    case 0xe000e450:    return "IPR20";
    case 0xe000e454:    return "IPR21";
    case 0xe000e458:    return "IPR22";
    case 0xe000e45c:    return "IPR23";
    case 0xe000e460:    return "IPR24";
    case 0xe000e464:    return "IPR25";
    case 0xe000e468:    return "IPR26";
    case 0xe000e46c:    return "IPR27";
    case 0xe000e470:    return "IPR28";
    case 0xe000e474:    return "IPR29";
    case 0xe000e478:    return "IPR30";
    case 0xe000e47c:    return "IPR31";
    case 0xe000e480:    return "IPR32";
    case 0xe000e484:    return "IPR33";
    case 0xe000e488:    return "IPR34";
    case 0xe000e48c:    return "IPR35";
    case 0xe000e490:    return "IPR36";
    case 0xe000e494:    return "IPR37";
    case 0xe000e498:    return "IPR38";
    case 0xe000e49c:    return "IPR39";
    case 0xe000e4a0:    return "IPR40";
    case 0xe000e4a4:    return "IPR41";
    case 0xe000e4a8:    return "IPR42";
    case 0xe000e4ac:    return "IPR43";
    case 0xe000e4b0:    return "IPR44";
    case 0xe000e4b4:    return "IPR45";
    case 0xe000e4b8:    return "IPR46";
    case 0xe000e4bc:    return "IPR47";
    case 0xe000e4c0:    return "IPR48";
    case 0xe000e4c4:    return "IPR49";
    case 0xe000e4c8:    return "IPR50";
    case 0xe000e4cc:    return "IPR51";
    case 0xe000e4d0:    return "IPR52";
    case 0xe000e4d4:    return "IPR53";
    case 0xe000e4d8:    return "IPR54";
    case 0xe000e4dc:    return "IPR55";
    case 0xe000e4e0:    return "IPR56";
    case 0xe000e4e4:    return "IPR57";
    case 0xe000e4e8:    return "IPR58";
    case 0xe000e4ec:    return "IPR59";
    case 0xe000ed00:    return "CPUID";
    case 0xe000ed04:    return "ICSR";
    case 0xe000ed08:    return "VTOR";
    case 0xe000ed0c:    return "AIRCR";
    case 0xe000ed10:    return "SCR";
    case 0xe000ed14:    return "CCR";
    case 0xe000ed18:    return "SHPR1";
    case 0xe000ed1c:    return "SHPR2";
    case 0xe000ed20:    return "SHPR3";
    case 0xe000ed24:    return "SHCSR";
    case 0xe000ed28:    return "CFSR";
    case 0xe000ed2c:    return "HFSR";
    case 0xe000ed30:    return "DFSR";
    case 0xe000ed34:    return "MMFAR";
    case 0xe000ed38:    return "BFAR";
    case 0xe000ed3c:    return "AFSR";
    case 0xe000ed40:    return "PFR0";
    case 0xe000ed44:    return "PFR1";
    case 0xe000ed48:    return "DFR0";
    case 0xe000ed4c:    return "AFR0";
    case 0xe000ed50:    return "MMFR0";
    case 0xe000ed54:    return "MMFR1";
    case 0xe000ed58:    return "MMFR2";
    case 0xe000ed5c:    return "MMFR3";
    case 0xe000ed60:    return "ISAR0";
    case 0xe000ed64:    return "ISAR1";
    case 0xe000ed68:    return "ISAR2";
    case 0xe000ed6c:    return "ISAR3";
    case 0xe000ed70:    return "ISAR4";
    case 0xe000ed88:    return "CPACR";
    case 0xe000ef00:    return "STIR";
    case 0xe000ef34:    return "FPCCR";
    case 0xe000ef38:    return "FPCAR";
    case 0xe000ef3c:    return "FPDSCR";
    case 0xe000ef40:    return "MVFR0";
    case 0xe000ef44:    return "MVFR1";
    case 0xe000efd0:    return "PID4";
    case 0xe000efd4:    return "PID5";
    case 0xe000efd8:    return "PID6";
    case 0xe000efdc:    return "PID7";
    case 0xe000efe0:    return "PID0";
    case 0xe000efe4:    return "PID1";
    case 0xe000efe8:    return "PID2";
    case 0xe000efec:    return "PID3";
    case 0xe000eff0:    return "CID0";
    case 0xe000eff4:    return "CID1";
    case 0xe000eff8:    return "CID2";
    case 0xe000effc:    return "CID3";
    default:            return NULL;
    }
}

byte
e4arduino_get_byte_from_memory(void *ctx, word addr)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    byte value;
    char *name;

    value = *(byte *)(&context->memory.memory[addr]);
    if((name = e4arduino_get_scr_register_name(addr)) == NULL) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x: %02x", addr, value);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x[%s]: %02x", addr, name, value);
    }
    return value;
}

void
e4arduino_set_byte_to_memory(void *ctx, word addr, byte value)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char *name;

    if((name = e4arduino_get_scr_register_name(addr)) == NULL) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x: %02x => %02x", addr, context->memory.memory[addr], value);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x[%s]: %02x => %02x", addr, name, context->memory.memory[addr], value);
    }
    *(byte *)(&context->memory.memory[addr]) = value;
}

halfword
e4arduino_get_halfword_from_memory(void *ctx, word addr)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    halfword value;
    char *name;

    if((addr & 0x00000001) != 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_WARN, "unaligned memory access [%08x]", addr);
    }
    value = *(halfword *)(&context->memory.memory[addr]);
    if((name = e4arduino_get_scr_register_name(addr)) == NULL) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x: %04x", addr, value);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x[%s]: %04x", addr, name, value);
    }
    return value;
}

void
e4arduino_set_halfword_to_memory(void *ctx, word addr, halfword value)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char *name;

    if((addr & 0x00000001) != 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_WARN, "unaligned memory access [%08x]", addr);
    }
    if((name = e4arduino_get_scr_register_name(addr)) == NULL) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x: %04x => %04x", addr, context->memory.memory[addr], value);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x[%s]: %04x => %04x", addr, name, context->memory.memory[addr], value);
    }
    *(halfword *)(&context->memory.memory[addr]) = value;
}

word
e4arduino_get_word_from_memory(void *ctx, word addr)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char *name;
    word value;

    if((addr & 0x00000003) != 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_WARN, "unaligned memory access [%08x]", addr);
    }
    if(addr == E4ARDUINO_SYSCON_SYSPLLSTAT) {
        value = 1;
    } else if(addr == 0x1000026c) {
        value = 30000000;   /* SystemCoreClock = 30000000; */
    } else
    value = *(word *)(&context->memory.memory[addr]);
    if((name = e4arduino_get_scr_register_name(addr)) == NULL) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x: %08x", addr, value);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x[%s]: %08x", addr, name, value);
    }
    if((addr == E4ARDUINO_SCB_STCSR) && ((context->memory.memory[E4ARDUINO_SCB_STCSR] & E4ARDUINO_STCSR_COUNTFLAG) != 0)) {
        context->memory.memory[E4ARDUINO_SCB_STCSR] &= ~E4ARDUINO_STCSR_COUNTFLAG;
    }
    return value;
}

void
e4arduino_set_word_to_memory(void *ctx, word addr, word value)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char *name;

    if((addr & 0x00000003) != 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_WARN, "unaligned memory access [%08x]", addr);
    }
    if((name = e4arduino_get_scr_register_name(addr)) == NULL) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x: %08x => %08x", addr, context->memory.memory[addr], value);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "\t%08x[%s]: %08x => %08x", addr, name, context->memory.memory[addr], value);
    }
    *(word *)(&context->memory.memory[addr]) = value;
}

void
e4arduino_push_to_stack(void *ctx, word value)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    context->registers.r[SP] -= 4;
    e4arduino_set_word_to_memory(context, context->registers.r[SP], value);
}

word
e4arduino_pop_from_stack(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    value = e4arduino_get_word_from_memory(context, context->registers.r[SP]);
    context->registers.r[SP] += 4;
    return value;
}

word
e4arduino_have_fpext(void *ctx)
{

    return 0;
}

void
e4arduino_to_use_psp(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    int i;

    if((context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_SPSEL) == 0) {
        for(i = 0; i < 16; i++) {
            if(i != SP) {
                context->registers.bank1[i] = context->registers.bank0[i];
            }
        }
    }
    context->special_registers.r[CONTROL] |= E4ARDUINO_CONTROL_SPSEL;
    context->registers.r = context->registers.bank1;
}

void
e4arduino_to_use_msp(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    int i;

    if((context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_SPSEL) != 0) {
        for(i = 0; i < 16; i++) {
            if(i != SP) {
                context->registers.bank0[i] = context->registers.bank1[i];
            }
        }
    }
    context->special_registers.r[CONTROL] &= ~E4ARDUINO_CONTROL_SPSEL;
    context->registers.r = context->registers.bank0;
}

void
e4arduino_exception_entry(void *ctx, word exception, word return_addr)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word frame_size, force_align, sp_mask, frame_ptr_align, frame_ptr, addr, tbit;

    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_DEBUG, "Entering Handler mode");
/*
 *  B1.5.6 Exception entry behavior [PushStack()]
 */
    if(e4arduino_have_fpext(context) && (context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_FPCA) != 0) {
        frame_size = 0x68;
        force_align = 1;
    } else {
        frame_size = 0x20;
        force_align = (context->memory.memory[E4ARDUINO_SCB_CCR] & E4ARDUINO_CCR_STKALIGN) != 0;
    }
    sp_mask = (force_align != 0) ? 0xfffffff8 : 0xfffffffc;
    frame_ptr_align = ((context->registers.r[SP] & 0x00000004) != 0) && (force_align != 0);
    context->registers.r[SP] = (context->registers.r[SP] - frame_size) & sp_mask;
    frame_ptr = context->registers.r[SP];
    e4arduino_set_word_to_memory(context, frame_ptr + 0x00, context->registers.r[R0]);
    e4arduino_set_word_to_memory(context, frame_ptr + 0x04, context->registers.r[R1]);
    e4arduino_set_word_to_memory(context, frame_ptr + 0x08, context->registers.r[R2]);
    e4arduino_set_word_to_memory(context, frame_ptr + 0x0c, context->registers.r[R3]);
    e4arduino_set_word_to_memory(context, frame_ptr + 0x10, context->registers.r[R12]);
    e4arduino_set_word_to_memory(context, frame_ptr + 0x14, context->registers.r[LR]);
    e4arduino_set_word_to_memory(context, frame_ptr + 0x18, return_addr);
    e4arduino_set_word_to_memory(context, frame_ptr + 0x1c, (context->registers.r[CPSR] & 0xfffffdff) | (frame_ptr_align << 9));
    if(e4arduino_have_fpext(context)) {
        if(e4arduino_get_current_execution_mode(context) == E4ARDUINO_EXECUTION_MODE_HANDLER) {
            context->registers.r[LR] = 0xffffffe0 | (~((context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_FPCA)) != 0) << 4 | 0x00000001;
        } else {
            context->registers.r[LR] = 0xffffffe0 | (~((context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_FPCA)) != 0) << 4 | (1 << 3) | (((context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_SPSEL) != 0) << 2) | 0x00000001;
        }
    } else {
        if(e4arduino_get_current_execution_mode(context) == E4ARDUINO_EXECUTION_MODE_HANDLER) {
            context->registers.r[LR] = 0xfffffff1;
        } else {
            context->registers.r[LR] = 0xfffffff8 | (((context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_SPSEL) != 0) << 2) | 0x00000001;
        }
    }

/*
 *  B1.5.6 Exception entry behavior [ExceptionTaken()]
 */
    addr = e4arduino_get_word_from_memory(context, e4arduino_get_word_from_memory(context, E4ARDUINO_SCB_VTOR) + exception * 4);
    context->registers.r[PC] = addr & 0xfffffffe;
    tbit = addr & 0x00000001;
    context->special_registers.r[IPSR] = (context->special_registers.r[IPSR] & 0xffffff00) | exception;
    context->special_registers.r[EPSR] = (context->special_registers.r[IPSR] & 0xfeff00ff) | (tbit << 24);
    if(e4arduino_have_fpext(context) != 0) {
        context->special_registers.r[CONTROL] |= E4ARDUINO_CONTROL_FPCA;
    }
    context->exception_active[exception >> 5] &= 1 << (exception & 0x0000001f);
    e4arduino_to_use_msp(context);
    e4arduino_set_current_execution_mode(context, E4ARDUINO_EXECUTION_MODE_HANDLER);
}

void
e4arduino_exception_return(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word frame_size, force_align, addr, psr, sp_mask;

    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_DEBUG, "Exiting Handler mode");
    switch(context->registers.r[PC] & 0x0000000f) {
    case 0x1:
        e4arduino_to_use_msp(context);
        e4arduino_set_current_execution_mode(context, E4ARDUINO_EXECUTION_MODE_HANDLER);
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_DEBUG, "Return to handler mode, use MSP");
        break;
    case 0x9:
        e4arduino_to_use_msp(context);
        e4arduino_set_current_execution_mode(context, E4ARDUINO_EXECUTION_MODE_THREAD);
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_DEBUG, "Return to thread mode, use MSP");
        break;
    case 0xd:
        e4arduino_to_use_psp(context);
        e4arduino_set_current_execution_mode(context, E4ARDUINO_EXECUTION_MODE_THREAD);
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_DEBUG, "Return to thread mode, use PSP");
        break;
    default:
        E4ARDUINO_PANIC("invalid EXC_RETURN");
        break;
    }
    if(e4arduino_have_fpext(context) && ((context->registers.r[PC] & 0x00000010) == 0)) {
        frame_size = 0x68;
        force_align = 1;
    } else {
        frame_size = 0x20;
        force_align = ((context->memory.memory[E4ARDUINO_CCR_ADDR] & E4ARDUINO_CCR_STKALIGN) != 0);
    }
    context->registers.r[R0] = e4arduino_get_word_from_memory(context, context->registers.r[SP] + 0x00);
    context->registers.r[R1] = e4arduino_get_word_from_memory(context, context->registers.r[SP] + 0x04);
    context->registers.r[R2] = e4arduino_get_word_from_memory(context, context->registers.r[SP] + 0x08);
    context->registers.r[R3] = e4arduino_get_word_from_memory(context, context->registers.r[SP] + 0x0c);
    context->registers.r[R12] = e4arduino_get_word_from_memory(context, context->registers.r[SP] + 0x10);
    context->registers.r[LR] = e4arduino_get_word_from_memory(context, context->registers.r[SP] + 0x14);
    addr = e4arduino_get_word_from_memory(context, context->registers.r[SP] + 0x18);
    psr = e4arduino_get_word_from_memory(context, context->registers.r[SP] + 0x1c);
    if(e4arduino_have_fpext(context)) {
        if((context->registers.r[PC] & 0x00000010) == 0) {
            if((context->memory.memory[E4ARDUINO_SCB_FPCCR] & E4ARDUINO_FPCCR_LSPACT) != 0) {
                context->memory.memory[E4ARDUINO_SCB_FPCCR] &= ~E4ARDUINO_FPCCR_LSPACT;
            } else {
                /* FP state resume */
            }
        }
        context->special_registers.r[CONTROL] = (context->special_registers.r[CONTROL] & ~E4ARDUINO_CONTROL_FPCA) | (((context->registers.r[PC] & 0x00000010) != 0) ? E4ARDUINO_CONTROL_FPCA : 0x00000000);
    }
    sp_mask = ((((psr & 0x00000100) != 0) && (force_align == 1)) != 0) << 2;
    switch(context->registers.r[PC] & 0x0000000f) {
    case 0x01:
        context->registers.r[SP] = (context->registers.r[SP] + frame_size) | sp_mask;
        break;
    case 0x09:
        context->registers.r[SP] = (context->registers.r[SP] + frame_size) | sp_mask;
        break;
    case 0x0d:
        context->registers.r[SP] = (context->registers.r[SP] + frame_size) | sp_mask;
        break;
    }
    context->special_registers.r[APSR] = (context->special_registers.r[APSR] & 0x07ffffff) | (psr & 0xf8000000);
    if(e4arduino_have_fpext(context)) {
        context->special_registers.r[APSR] = (context->special_registers.r[APSR] & 0xfff0ffff) | (psr & 0x000f0000);
    }
    context->special_registers.r[IPSR] = (context->special_registers.r[IPSR] & 0xfffffe00) | (psr & 0x000001ff);
    context->special_registers.r[EPSR] = (context->special_registers.r[EPSR] & 0xf8ff03ff) | (psr & 0x0700fc00);
    context->registers.r[PC] = addr & 0xfffffffe;
}

void
e4arduino_handle_interrupt_pre_cycle(e4arduino_context_t *context)
{

    if((context->registers.r[PC] & 0xf0000000) == 0xf0000000) {
        e4arduino_exception_return(context);
    }
}

void
e4arduino_handle_interrupt_post_cycle(e4arduino_context_t *context)
{
    word icsr;

    if(e4arduino_get_current_execution_mode(context) == E4ARDUINO_EXECUTION_MODE_THREAD) {
        if((context->memory.memory[E4ARDUINO_SCB_STCSR] & E4ARDUINO_STCSR_SYSTICK) != 0) {
            if(context->memory.memory[E4ARDUINO_SCB_STCVR]-- == 0) {
                if((context->memory.memory[E4ARDUINO_SCB_STCSR] & E4ARDUINO_STCSR_TICKINT) != 0) {
                    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "SysTick timer expired");
                    context->wfi = 0;
                    e4arduino_exception_entry(context, E4ARDUINO_EXCEPTION_SYSTICK, context->registers.r[PC]);
                    return;
                }
                context->memory.memory[E4ARDUINO_SCB_STCSR] |= E4ARDUINO_STCSR_COUNTFLAG;
                context->memory.memory[E4ARDUINO_SCB_STRVR] = context->memory.memory[E4ARDUINO_SCB_STCVR] & 0x00ffffff;
            }
        }
        icsr = e4arduino_get_word_from_memory(context, E4ARDUINO_ICSR_ADDR);
        if(icsr & 0x10000000) {
            e4arduino_set_word_to_memory(context, E4ARDUINO_ICSR_ADDR, (icsr & ~0x10000000));
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "Processing PendSV");
            context->wfi = 0;
            e4arduino_exception_entry(context, E4ARDUINO_EXCEPTION_PENDSV, context->registers.r[PC]);
            return;
        }
    }
}

void
mov_reg_16_et2(void *ctx, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mov_reg_16_et2(void *ctx[0x%p], halfword rm[0x%x], halfword rd[0x%x])", ctx, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tmovs\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if(e4arduino_in_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rm];
        context->registers.r[rd] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
lsl_imm_16_et1(void *ctx, halfword imm, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void lsl_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x], halfword rm[0x%x], halfword rd[0x%x])", ctx, imm, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tlsl%s\t%s, %s, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rd], e4arduino_register_name[rm], imm);
    carry = 0;
    value = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, imm, &carry);
    context->registers.r[rd] = value;
    if(!e4arduino_in_it_block(context)) {
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
lsr_imm_16_et1(void *ctx, halfword imm, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void lsr_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x], halfword rm[0x%x], halfword rd[0x%x])", ctx, imm, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tlsr%s\t%s, %s, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rd], e4arduino_register_name[rm], imm);
    carry = 0;
    value = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSR_C, imm, &carry);
    context->registers.r[rd] = value;
    if(!e4arduino_in_it_block(context)) {
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
asr_imm_16_et1(void *ctx, halfword imm, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void asr_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x], halfword rm[0x%x], halfword rd[0x%x])", ctx, imm, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tasr%s\t%s, %s, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rd], e4arduino_register_name[rm], imm);
    carry = 0;
    value = e4arduino_shift(context->registers.r[rd], E4ARDUINO_SHIFT_TYPE_ASR, imm, &carry);
    context->registers.r[rd] = value;
    if(!e4arduino_in_it_block(context)) {
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
add_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rd[0x%x])", ctx, rm, rn, rd);
    if(e4arduino_in_it_block(context)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadd%s\t%s, %s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadds\t%s, %s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    }
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 0;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], context->registers.r[rm], &carry, &overflow);
        context->registers.r[rd] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
sub_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rd[0x%x])", ctx, rm, rn, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tsub%s\t%s, %s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 1;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], ~(context->registers.r[rm]), &carry, &overflow);
        context->registers.r[rd] = value;
        if(e4arduino_in_it_block(context) == 0) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
add_imm_16_et1(void *ctx, halfword imm3, halfword rn, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_imm_16_et1(void *ctx[0x%p], halfword imm3[0x%x], halfword rn[0x%x], halfword rd[0x%x])", ctx, imm3, rn, rd);
    immediate = e4arduino_zero_extend(imm3, 3);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadd%s\t%s, %s, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 0;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], immediate, &carry, &overflow);
        context->registers.r[rd] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
sub_imm_16_et1(void *ctx, halfword imm3, halfword rn, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_imm_16_et1(void *ctx[0x%p], halfword imm3[0x%x], halfword rn[0x%x], halfword rd[0x%x])", ctx, imm3, rn, rd);
    immediate = e4arduino_zero_extend(imm3, 3);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tsub%s\t%s, %s, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 1;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], ~immediate, &carry, &overflow);
        context->registers.r[rd] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
mov_imm_16_et1(void *ctx, halfword rd, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mov_imm_16_et1(void *ctx[0x%p], halfword rd[0x%x], halfword imm[0x%x])", ctx, rd, imm);
    immediate = e4arduino_zero_extend(imm, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tmov%s%s\t%s, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[rd] = immediate;
    }
    if(e4arduino_in_it_block(context) == 0) {
        e4arduino_set_negative_flag(context, (immediate & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (immediate == 0));
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
cmp_imm_16_et1(void *ctx, halfword rn, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cmp_imm_16_et1(void *ctx[0x%p], halfword rn[0x%x], halfword imm[0x%x])", ctx, rn, imm);
    immediate = e4arduino_zero_extend(imm, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tcmp%s\t%s, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 1;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], ~immediate, &carry, &overflow);
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
        e4arduino_set_overflow_flag(context, overflow);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
add_imm_16_et2(void *ctx, halfword rdn, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_imm_16_et2(void *ctx[0x%p], halfword rdn[0x%x], halfword imm[0x%x])", ctx, rdn, imm);
    immediate = e4arduino_zero_extend(imm, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadd%s\t%s, %s, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rdn], e4arduino_register_name[rdn], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 0;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rdn], immediate, &carry, &overflow);
        context->registers.r[rdn] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
sub_imm_16_et2(void *ctx, halfword rdn, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_imm_16_et2(void *ctx[0x%p], halfword rdn[0x%x], halfword imm[0x%x])", ctx, rdn, imm);
    immediate = e4arduino_zero_extend(imm, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tsub%s\t%s, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rdn], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 1;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rdn], ~immediate, &carry, &overflow);
        context->registers.r[rdn] = value;
        if(e4arduino_in_it_block(context) == 0) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
and_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void and_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tand%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rdn] & context->registers.r[rm];
        context->registers.r[rdn] = value;
        carry = 0;
        overflow = 0;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
eor_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void eor_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\teor%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "", e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rdn] ^ context->registers.r[rm];
        context->registers.r[rdn] = value;
        carry = 0;
        overflow = 0;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
lsl_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, shift, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void lsl_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tlsl%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        shift = context->registers.r[rm] & 0x000000ff;
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rdn], E4ARDUINO_SHIFT_TYPE_LSL, shift, &carry);
        context->registers.r[rdn] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
lsr_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void lsr_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    if(e4arduino_in_it_block(context)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tlsrs\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tlsr%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    }
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        shift_n = context->registers.r[rm] & 0x000000ff;
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rdn], E4ARDUINO_SHIFT_TYPE_LSR, shift_n, &carry);
        context->registers.r[rdn] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
asr_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void asr_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
adc_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, shifted, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void adc_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadc%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        shifted = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, 0, &carry);
        value = e4arduino_add_with_carry(context->registers.r[rdn], shifted, &carry, &overflow);
        context->registers.r[rdn] = value;
        if(!e4arduino_in_last_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
sbc_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sbc_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    if(e4arduino_in_it_block(context)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tsbc\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tsbcs\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    }
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rdn], ~context->registers.r[rm], &carry, &overflow);
        context->registers.r[rdn] = value;
        if(!e4arduino_in_last_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
ror_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ror_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tror%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), (e4arduino_in_it_block(context) ?  e4arduino_get_cond_string(context) : "s"), e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rdn], E4ARDUINO_SHIFT_TYPE_ROR, context->registers.r[rm], &carry);
        context->registers.r[rdn] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
tst_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void tst_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\ttst%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rdn] & e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, 0, &carry);
        carry = e4arduino_get_carry_flag(context);
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
rsb_imm_16_et1(void *ctx, halfword rn, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void rsb_imm_16_et1(void *ctx[0x%p], halfword rn[0x%x], halfword rd[0x%x])", ctx, rn, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\trsb%s\t%s, %s, #0", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rd], e4arduino_register_name[rn]);
    carry = 1;
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = e4arduino_add_with_carry(~context->registers.r[rn], 0, &carry, &overflow);
        context->registers.r[rd] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
cmp_reg_16_et1(void *ctx, halfword rm, halfword rn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cmp_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x])", ctx, rm, rn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tcmp%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 1;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], ~context->registers.r[rm], &carry, &overflow);
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
        e4arduino_set_overflow_flag(context, overflow);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
cmn_reg_16_et1(void *ctx, halfword rm, halfword rn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cmn_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x])", ctx, rm, rn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
orr_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void orr_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\torr%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rdn] | context->registers.r[rm];
        context->registers.r[rdn] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
mul_16_et1(void *ctx, halfword rn, halfword rdm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mul_16_et1(void *ctx[0x%p], halfword rn[0x%x], halfword rdm[0x%x])", ctx, rn, rdm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tmul%s\t%s, %s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rdm], e4arduino_register_name[rn], e4arduino_register_name[rdm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rn] * context->registers.r[rdm];
        context->registers.r[rdm] = value & 0xffffffff;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
bic_reg_16_et1(void *ctx, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void bic_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rdn[0x%x])", ctx, rm, rdn);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tbic%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? e4arduino_get_cond_string(context) : "s", e4arduino_register_name[rdn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rdn] & ~context->registers.r[rm];
        context->registers.r[rdn] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
        }
    }
    context->registers.r[PC] += 2;
}

void
mvn_reg_16_et1(void *ctx, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mvn_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rd[0x%x])", ctx, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tmvn%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? "s" : e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = ~context->registers.r[rm];
        context->registers.r[rd] = value;
        if(!e4arduino_in_it_block(context)) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
add_reg_16_et2(void *ctx, halfword dn, halfword rm, halfword rdn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word rd, value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_reg_16_et2(void *ctx[0x%p], halfword dn[0x%x], halfword rm[0x%x], halfword rdn[0x%x])", ctx, dn, rm, rdn);
    rd = (dn << 3) | rdn;
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadd%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if(((rd == 15) && e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) || ((rd == 15) && (rm == 15))) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = e4arduino_add_with_carry(context->registers.r[rd], context->registers.r[rm], NULL, NULL);
        context->registers.r[rd] = value;
        context->wait_until = context->cycles + 1;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
add_sp_reg_16_et1(void *ctx, halfword dm, halfword rdm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_sp_reg_16_et1(void *ctx[0x%p], halfword dm[0x%x], halfword rdm[0x%x])", ctx, dm, rdm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
add_sp_reg_16_et2(void *ctx, halfword rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_sp_reg_16_et2(void *ctx[0x%p], halfword rm[0x%x])", ctx, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadd%s\tSP, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[SP] = e4arduino_add_with_carry(context->registers.r[SP], context->registers.r[rm], NULL, NULL);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
cmp_reg_16_et2(void *ctx, halfword n, halfword rm, halfword rn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, shifted, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cmp_reg_16_et2(void *ctx[0x%p], halfword n[0x%x], halfword rm[0x%x], halfword rn[0x%x])", ctx, n, rm, rn);
    rn |= (n << 3);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tcmp%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rm]);
    carry = 1;
    overflow = 1;
    if(((rn < 8) && (rm < 8)) || (rn == 15) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        shifted = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, 0, &carry);
        value = e4arduino_add_with_carry(context->registers.r[rn], ~shifted, &carry, &overflow);
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
        e4arduino_set_overflow_flag(context, overflow);
    }
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
mov_reg_16_et1(void *ctx, halfword d, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mov_reg_16_et1(void *ctx[0x%p], halfword d[0x%x], halfword rm[0x%x], halfword rd[0x%x])", ctx, d, rm, rd);
    rd |= (d << 3);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tmov%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if((rd == 15) && e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rm];
        if(rd == 15) {
            context->registers.r[rd] = value - 2;
        } else {
            context->registers.r[rd] = value;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
bx_16_et1(void *ctx, halfword rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void bx_16_et1(void *ctx[0x%p], halfword rm[0x%x])", ctx, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tbx%s\t%s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rm]);
    if(e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[PC] = context->registers.r[rm];
    } else {
        context->registers.r[PC] += 2;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;  /* from 2 to 4 cycles */
}

void
blx_reg_16_et1(void *ctx, halfword rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void blx_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x])", ctx, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tblx%s\t%s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rm]);
    if((rm == 15) || (e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context))) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[LR] = context->registers.r[PC] + 3;
        context->registers.r[PC] = context->registers.r[rm];
    } else {
        context->registers.r[PC] += 2;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;  /* from 2 to 4 cycles */
}

void
ldr_lit_16_et1(void *ctx, halfword rt, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word base, value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldr_lit_16_et1(void *ctx[0x%p], halfword rt[0x%x], halfword imm[0x%x])", ctx, rt, imm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tldr%s\t%s, [PC, #%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], imm * 4);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        base = (context->registers.r[PC] + 4) & 0xfffffffc;
        value = e4arduino_get_word_from_memory(context, base + e4arduino_zero_extend(imm, 32) * 4);
        context->registers.r[rt] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
str_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr, offset, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void str_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, rm, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstr%s\t%s, [%s, %s]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        offset = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, 0, &carry);
        addr = context->registers.r[rn] + offset;
        e4arduino_set_word_to_memory(context, addr, context->registers.r[rt]);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
strh_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strh_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, rm, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
strb_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr, carry, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strb_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, rm, rn, rt);
     E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstrb%s\t%s, [%s, %s]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        offset = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, 0, &carry);
        addr = context->registers.r[rn] + offset;
        e4arduino_set_byte_to_memory(context, addr, context->registers.r[rt] & 0xff);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
ldrsb_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsb_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, rm, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tldrsb%s\t%s, %s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn] + context->registers.r[rm];
        context->registers.r[rt] = e4arduino_sign_extend(e4arduino_get_byte_from_memory(context, addr), 8);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
ldr_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, addr, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldr_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, rm, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tldr%s\t%s, [%s, %s]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    carry = e4arduino_to_be_executed(context);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn] + e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, 0, &carry);
        value = e4arduino_get_word_from_memory(context, addr);
        if ((rt == 15) || ((addr & 0x00000003) != 0x00000000)) {
            e4arduino_unpredictable(context);
        } else {
            context->registers.r[rt] = value;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
ldrh_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr, carry, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrh_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, rm, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tldrh%s\t%s, [%s, %s]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        offset = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, 0, &carry);
        addr = context->registers.r[rn] + offset;
        context->registers.r[rt] = e4arduino_zero_extend(e4arduino_get_halfword_from_memory(context, addr), 32);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
ldrb_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrb_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, rm, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tldrb%s\t%s, %s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn] + context->registers.r[rm];
        context->registers.r[rt] = e4arduino_zero_extend(e4arduino_get_byte_from_memory(context, addr), 32);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
ldrsh_reg_16_et1(void *ctx, halfword rm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsh_reg_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, rm, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
str_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word base, value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void str_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, imm, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstr%s\t%s, [%s, #%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], imm * 4);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        base = (context->registers.r[rn]) & 0xfffffffc;
        value = context->registers.r[rt];
        e4arduino_set_word_to_memory(context, base + e4arduino_zero_extend(imm, 32) * 4, value);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
ldr_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldr_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, imm, rn, rt);
    immediate = e4arduino_zero_extend(imm << 2, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tldr%s\t%s, [%s, #%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        offset = context->registers.r[rn] + immediate;
        value = e4arduino_get_word_from_memory(context, offset);
        context->registers.r[rt] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
strb_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strb_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, imm, rn, rt);
    immediate = e4arduino_zero_extend(imm, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstrb%s\t%s, [%s, #%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        offset = context->registers.r[rn] + immediate;
        e4arduino_set_byte_to_memory(context, offset, context->registers.r[rt] & 0x000000ff);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
ldrb_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrb_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, imm, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tldrb%s\t%s, [%s, #%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], imm);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        offset = context->registers.r[rn] + e4arduino_zero_extend(imm, 32);
        context->registers.r[rt] = e4arduino_zero_extend(e4arduino_get_byte_from_memory(context, offset), 32);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
strh_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word base, offset, value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strh_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, imm, rn, rt);
    offset = e4arduino_zero_extend(imm << 1, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstrh%s\t%s, [%s, #%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], offset);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        base = (context->registers.r[rn]) & 0xfffffffe;
        value = context->registers.r[rt];
        e4arduino_set_halfword_to_memory(context, base + offset, value);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
ldrh_imm_16_et1(void *ctx, halfword imm, halfword rn, halfword rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrh_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x], halfword rn[0x%x], halfword rt[0x%x])", ctx, imm, rn, rt);
    immediate = e4arduino_zero_extend(imm << 1, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tldrh%s\t%s, [%s, #%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        offset = context->registers.r[rn] + immediate;
        value = e4arduino_get_halfword_from_memory(context, offset);
        context->registers.r[rt] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
str_imm_16_et2(void *ctx, halfword rt, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void str_imm_16_et2(void *ctx[0x%p], halfword rt[0x%x], halfword imm[0x%x])", ctx, rt, imm);
    value = e4arduino_zero_extend(imm << 2, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstr%s\t%s, [SP, #%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], value);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        offset = context->registers.r[SP] + value;
        e4arduino_set_word_to_memory(context, offset, context->registers.r[rt]);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 2;
}

void
ldr_imm_16_et2(void *ctx, halfword rt, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, addr;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldr_imm_16_et2(void *ctx[0x%p], halfword rt[0x%x], halfword imm[0x%x])", ctx, rt, imm);
    immediate = e4arduino_zero_extend(imm << 2, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tldr%s\t%s, [SP, #%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[SP] + immediate;
        context->registers.r[rt] = e4arduino_get_word_from_memory(context, addr);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
adr_16_et1(void *ctx, halfword rd, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void adr_16_et1(void *ctx[0x%p], halfword rd[0x%x], halfword imm[0x%x])", ctx, rd, imm);
    immediate = e4arduino_zero_extend((imm << 2), 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadr%s\t%s, %x", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = ((context->registers.r[PC] + 4) & 0xfffffffc) + immediate;
        context->registers.r[rd] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
add_sp_imm_16_et1(void *ctx, halfword rd, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_sp_imm_16_et1(void *ctx[0x%p], halfword rd[0x%x], halfword imm[0x%x])", ctx, rd, imm * 4);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadd%s\t%s, SP, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], imm * 4);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[SP] + e4arduino_zero_extend(imm * 4, 32);
        context->registers.r[rd] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
cps_16_et1(void *ctx, halfword im, halfword i, halfword f)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cps_16_et1(void *ctx[0x%p], halfword im[0x%x], halfword i[0x%x], halfword f[0x%x])", ctx, im, i, f);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tcps%s\t%s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), (im == 0) ? "ie" : "id", (i == 1) ? "i" : (f == 1) ? "f" : "");
    if(e4arduino_in_it_block(context)) {
        e4arduino_unpredictable(context);
    } else {
        if(e4arduino_is_privileged(context)) {
            if(i == 1) {
                if(im == 0) {
                    context->special_registers.r[PRIMASK] = 0;
                } else {
                    context->special_registers.r[PRIMASK] = 1;
                }
            } else if(f == 1) {
                if(im == 0) {
                    context->special_registers.r[FAULTMASK] = 0;
                } else {
                    context->special_registers.r[FAULTMASK] = 1;
                }
            } else {
                E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_WARN, "cps: unexpected combination of argument [i == 0 && f == 0]");
            }
        } else {
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_WARN, "not privileged");
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;  /* 1 or 2 cycles */
    context->registers.r[PC] += 2;
}

void
add_sp_imm_16_et2(void *ctx, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_sp_imm_16_et2(void *ctx[0x%p], halfword imm[0x%x])", ctx, imm);
    immediate = e4arduino_zero_extend(imm << 2, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tadd%s\tsp, sp, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[SP] += immediate;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;  /* 1 or 2 cycles */
    context->registers.r[PC] += 2;
}

void
sub_sp_imm_16_et1(void *ctx, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_sp_imm_16_et1(void *ctx[0x%p], halfword imm[0x%x])", ctx, imm);
    immediate = e4arduino_zero_extend(imm << 2, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tsub%s\tsp, sp, #%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), immediate);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 1;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[SP], ~immediate, &carry, &overflow);
        context->registers.r[SP] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
cbz_16_et1(void *ctx, halfword i, halfword imm, halfword rn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cbz_16_et1(void *ctx[0x%p], halfword i[0x%x], halfword imm[0x%x], halfword rn[0x%x])", ctx, i, imm, rn);
    value = context->registers.r[PC] + 4 + e4arduino_zero_extend((i << 6) | (imm << 1), 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tcbz\t%x", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), value);
    if(e4arduino_in_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(context->registers.r[rn] == 0) {
        context->registers.r[PC] = value;
    } else {
        context->registers.r[PC] += 2;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;  /* from 1 to 4 cycles */
}

void
cbnz_16_et1(void *ctx, halfword i, halfword imm, halfword rn)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cbnz_16_et1(void *ctx[0x%p], halfword i[0x%x], halfword imm[0x%x], halfword rn[0x%x])", ctx, i, imm, rn);
    value = context->registers.r[PC] + 4 + e4arduino_zero_extend((i << 6) | (imm << 1), 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tcbnz\t%x", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), value);
    if(e4arduino_in_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(context->registers.r[rn] != 0) {
        context->registers.r[PC] = value;
    } else {
        context->registers.r[PC] += 2;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;  /* from 1 to 4 cycles */
}

void
sxth_16_et1(void *ctx, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sxth_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rd[0x%x])", ctx, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tsxth%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[rd] = e4arduino_sign_extend(context->registers.r[rm], 16);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
sxtb_16_et1(void *ctx, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sxtb_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rd[0x%x])", ctx, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tsxtb%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[rd] = e4arduino_sign_extend(context->registers.r[rm], 8);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
uxth_16_et1(void *ctx, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uxth_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rd[0x%x])", ctx, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tuxth%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        /* 20190215 bugfix & 0x000000ff -> & 0x0000ffff ZeroExtend(rotated<15:0>,32) */
        context->registers.r[rd] = e4arduino_zero_extend((context->registers.r[rm] & 0x0000ffff), 32);
    }
    context->registers.r[PC] += 2;
}

void
uxtb_16_et1(void *ctx, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uxtb_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rd[0x%x])", ctx, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tuxtb%s\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[rd] = context->registers.r[rm] & 0x000000ff;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
push_16_et1(void *ctx, halfword m, halfword rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char strbuf[BUFSIZ];
    int i, count;
    word list;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void push_16_et1(void *ctx[0x%p], halfword m[0x%x], halfword rl[0x%x])", ctx, m, rl);
    list = (m << 14) | rl;
    strbuf[0] = '\0';
    for(i = 0; i <= 15; i++) {
        if((list & (1 << i)) != 0) {
            strncat(strbuf, e4arduino_register_name[i], BUFSIZ - 1);
            strncat(strbuf, ", ", BUFSIZ - 1);
        }
    }
    strbuf[strlen(strbuf) - 2] = '\0';
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tpush%s\t{%s}", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), strbuf);
    count = 0;
    if(list == 0x00000000) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        for(i = 15; i >= 0; i--) {
            if((list & (1 << i)) != 0) {
                e4arduino_push_to_stack(context, context->registers.r[i]);
                count++;
            }
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1 + count;  /* 1 + nregs */
    context->registers.r[PC] += 2;
}

void
rev_16_et1(void *ctx, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void rev_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rd[0x%x])", ctx, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
rev16_16_et1(void *ctx, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void rev16_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rd[0x%x])", ctx, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
revsh_16_et1(void *ctx, halfword rm, halfword rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void revsh_16_et1(void *ctx[0x%p], halfword rm[0x%x], halfword rd[0x%x])", ctx, rm, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
pop_16_et1(void *ctx, halfword p, halfword rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char strbuf[BUFSIZ];
    int i, count;
    word list;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pop_16_et1(void *ctx[0x%p], halfword p[0x%x], halfword rl[0x%x])", ctx, p, rl);
    list = (p << 15) | rl;
    strbuf[0] = '\0';
    for(i = 0; i <= 15; i++) {
        if((list & (1 << i)) != 0) {
            strncat(strbuf, e4arduino_register_name[i], BUFSIZ - 1);
            strncat(strbuf, ", ", BUFSIZ - 1);
        }
    }
    strbuf[strlen(strbuf) - 2] = '\0';
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tpop%s\t{%s}", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), strbuf);
    count = 0;
    if(list == 0x00000000) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        for(i = 0; i <= 15; i++) {
            if((list & (1 << i)) != 0) {
                context->registers.r[i] = e4arduino_pop_from_stack(context);
                if(i == 15) {
                    context->registers.r[i] -= 2;
                }
                count++;
            }
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1 + count;  /* 1 + nregs */
    context->registers.r[PC] += 2;
}

void
bkpt_16_et1(void *ctx, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void bkpt_16_et1(void *ctx[0x%p], halfword imm[0x%x])", ctx, imm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
it_16_et1(void *ctx, halfword fc, halfword mask)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char xyz[4];
    int i, j, count;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    count = 0;
    if(e4arduino_in_it_block(context)) {
        e4arduino_unpredictable(context);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void it_16_et1(void *ctx[0x%p], halfword fc[0x%x], halfword mask[0x%x])", ctx, fc, mask);
        bzero(xyz, 4);
        for(i = 0; i < 4; i++) {
            if((mask & (1 << i)) != 0) {
                count++;
                break;
            }
        }
        for(j = 0; j < 3 - i; j++) {
            if((mask & (1 << (3 - j))) == 0) {
                xyz[j] = 'e';
            } else {
                xyz[j] = 't';
                count++;
            }
        }
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tit%s\t%s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), xyz, e4arduino_cond_name[fc]);
        if((fc == 15) || ((fc == 14) && (count != 1)) || e4arduino_in_it_block(context)) {
            e4arduino_unpredictable(context);
        } else {
            value = (fc << 4) | mask;
            e4arduino_set_itstate(context, value);
        }
    }
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
nop_16_et1(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void nop_16_et1(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tnop", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 2;
}

void
yield_16_et1(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void yield_16_et1(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
wfe_16_et1(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void wfe_16_et1(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\twfe%s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context));
    context->wfe = 1;
    context->registers.r[PC] += 2;
}

void
wfi_16_et1(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void wfi_16_et1(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\twfi%s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context));
    context->wfi = 1;
    context->registers.r[PC] += 2;
}

void
sev_16_et1(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sev_16_et1(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
stm_16_et1(void *ctx, halfword rn, halfword rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    int i, count;
    char strbuf[BUFSIZ];

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void stm_16_et1(void *ctx[0x%p], halfword rn[0x%x], halfword rl[0x%x])", ctx, rn, rl);
    strbuf[0] = '\0';
    for(i = 0; i <= 7; i++) {
        if((rl & (1 << i)) != 0) {
            strncat(strbuf, e4arduino_register_name[i], BUFSIZ - 1);
            strncat(strbuf, ", ", BUFSIZ - 1);
        }
    }
    strbuf[strlen(strbuf) - 2] = '\0';
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstm%s\t%s!, {%s}", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], strbuf);
    count = 0;
    if(rl == 0x00000000) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        for(i = 0; i <= 7; i++) {
            if((rl & (1 << i)) != 0) {
                e4arduino_set_word_to_memory(context, context->registers.r[rn] + count * 4, context->registers.r[i]);
                count++;
            }
        }
    }
    e4arduino_advance_itstate(context);
    context->registers.r[rn] = context->registers.r[rn] + count * 4;
    context->wait_until = context->cycles + 1 + count;  /* 1 + nregs */
    context->registers.r[PC] += 2;
}

void
ldm_16_et1(void *ctx, halfword rn, halfword rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char strbuf[BUFSIZ];
    int i;
    word addr, count;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldm_16_et1(void *ctx[0x%p], halfword rn[0x%x], halfword rl[0x%x])", ctx, rn, rl);
    count = 0;
    strbuf[0] = '{';
    strbuf[1] = '\0';
    for(i = 0; i <= 7; i++) {
        if((rl & (1 << i)) != 0) {
            strncat(strbuf, e4arduino_register_name[i], BUFSIZ - 1);
            strncat(strbuf, ", ", BUFSIZ - 1);
            count++;
        }
    }
    strbuf[strlen(strbuf) - 2] = '}';
    strbuf[strlen(strbuf) - 1] = '\0';
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t\tldm%s.w\t%s%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], ((rl & (1 << rn)) != 0) ? "!" : "", strbuf);
    if(count == 0) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn];
        for(i = 0; i < 8; i++) {
            if((rl & (1 << i)) != 0) {
                context->registers.r[i] = e4arduino_get_word_from_memory(context, addr);
                addr += 4;
            }
        }
        if(rn == 0) {
            context->registers.r[rn] = addr;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1 + count;  /* 1 + nregs */
    context->registers.r[PC] += 2;
}

void
b_16_et1(void *ctx, halfword cond, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void b_16_et1(void *ctx[0x%p], halfword cond[0x%x], halfword imm[0x%x])", ctx, cond, imm);
    value = e4arduino_sign_extend(imm << 1, 9);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tb%s\t%x", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_cond_name[cond], context->registers.r[PC] + (4 + value));
    if(cond == 0xe) {
        e4arduino_undefined(context);
    } else if(e4arduino_in_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(e4arduino_judge_condition(context, cond)) {
        context->registers.r[PC] += (4 + value);
    } else {
        context->registers.r[PC] += 2;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;  /* from 1 to 4 cycles */
}

void
udf_16_et1(void *ctx, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void udf_16_et1(void *ctx[0x%p], halfword imm[0x%x])", ctx, imm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\t", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 2;
}

void
svc_16_et1(void *ctx, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void svc_16_et1(void *ctx[0x%p], halfword imm[0x%x])", ctx, imm);
    immediate = e4arduino_zero_extend(imm, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tsvc%s\t#%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), immediate);
    e4arduino_exception_entry(context, E4ARDUINO_EXCEPTION_SVC, context->registers.r[PC] + 2);
}

void
b_16_et2(void *ctx, halfword imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void b_16_et2(void *ctx[0x%p], halfword imm[0x%x])", ctx, imm);
    value = e4arduino_sign_extend(imm << 1, 12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tb%s\t%x", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), context->registers.r[PC] + (4 + value));
    if(e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[PC] += (4 + value);
    } else {
        context->registers.r[PC] += 2;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;  /* from 1 to 4 cycles */
}

void
stm_32_et2(void *ctx, word w, word rn, word m, word rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char strbuf[BUFSIZ];
    int i;
    word list, addr, count;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void stm_32_et2(void *ctx[0x%p], word w[0x%x], word rn[0x%x], word m[0x%x], word rl[0x%x])", ctx, w, rn, m, rl);
    /*E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");*/

    /* Experimental Implemented 20190214 */
    list = (m << 14) | rl;
    strbuf[0] = '{';
    strbuf[1] = '\0';
    for(i = 0; i <= 14; i++) {
        if((list & (1 << i)) != 0) {
            strncat(strbuf, e4arduino_register_name[i], BUFSIZ - 1);
            strncat(strbuf, ", ", BUFSIZ - 1);
        }
    }
    strbuf[strlen(strbuf) - 2] = '}';
    strbuf[strlen(strbuf) - 1] = '\0';
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t\tstm%s.w\t%s%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rn], (w != 0) ? "!" : "", strbuf);
    count = 0;
    for(i = 0; i < 16; i++) {
        if((list & (1 << i)) != 0) {
            count++;
        }
    }
    if((rn == 15) || (count < 2)) {
        e4arduino_unpredictable(context);
    } else if((w == 1) && (list & (1 << rn) != 0)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn];
        for(i = 0; i <= 14; i++) {
            if((list & (1 << i)) != 0) {
                e4arduino_set_word_to_memory(context, addr, context->registers.r[i]);
                addr += 4;
            }
        }
        if (w == 1) {
            context->registers.r[rn] = addr;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1 + count;  /* 1 + nregs */

    context->registers.r[PC] += 4;
}

void
ldm_32_et2(void *ctx, word w, word rn, word p, word m, word rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char strbuf[BUFSIZ];
    int i;
    word list, addr, count;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldm_32_et2(void *ctx[0x%p], word w[0x%x], word rn[0x%x], word p[0x%x], word m[0x%x], word rl[0x%x])", ctx, w, rn, p, m, rl);
    list = (p << 15) | (m << 14) | rl;
    count = 0;
    strbuf[0] = '{';
    strbuf[1] = '\0';
    for(i = 0; i <= 15; i++) {
        if((list & (1 << i)) != 0) {
            strncat(strbuf, e4arduino_register_name[i], BUFSIZ - 1);
            strncat(strbuf, ", ", BUFSIZ - 1);
        }
    }
    strbuf[strlen(strbuf) - 2] = '}';
    strbuf[strlen(strbuf) - 1] = '\0';
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldm%s.w\t%s%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], (w != 0) ? "!" : "", strbuf);
    count = 0;
    for(i = 0; i < 16; i++) {
        if((list & (1 << i)) != 0) {
            count++;
        }
    }
    if((rn == 15) || (count < 2) || ((p == 1) && (m == 1))) {
        e4arduino_unpredictable(context);
    } else if(((list & 0x8000) != 0) && e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if((w == 1) && ((list & (1 << rn)) != 0)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn];
        for(i = 0; i < 16; i++) {
            if((list & (1 << i)) != 0) {
                context->registers.r[i] = e4arduino_get_word_from_memory(context, addr);
                addr += 4;
            }
        }
        if((w == 1) && ((list & (1 << rn)) == 0)) {
            context->registers.r[rn] = addr;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1 + count;
    context->registers.r[PC] += 4;
}

void
pop_32_et2(void *ctx, word p, word m, word rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char strbuf[BUFSIZ];
    int i, count;
    word list;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pop_32_et2(void *ctx[0x%p], word p[0x%x], word m[0x%x], word rl[0x%x])", ctx, p, m, rl);
    list = (p << 15) | (m << 14) | rl;
    strbuf[0] = '\0';
    count = 0;
    for(i = 0; i <= 15; i++) {
        if((list & (1 << i)) != 0) {
            strncat(strbuf, e4arduino_register_name[i], BUFSIZ - 1);
            strncat(strbuf, ", ", BUFSIZ - 1);
            count++;
        }
    }
    strbuf[strlen(strbuf) - 2] = '\0';
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tpop%s\t%s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), strbuf);
    if((count < 2) || ((p == 1) && (m == 1))) {
        e4arduino_unpredictable(context);
    } else if ((p == 1) && e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) {
        e4arduino_unpredictable(context);
    } else {
        for(i = 0; i <= 15; i++) {
            if((list & (1 << i)) != 0) {
                context->registers.r[i] = e4arduino_pop_from_stack(context);
                if(i == 15) {
                    context->registers.r[i] -= 4;
                }
                count++;
            }
        }
    }
    context->wait_until = context->cycles + 1 + count;
    context->registers.r[PC] += 4;
}

void
stmdb_32_et1(void *ctx, word w, word rn, word m, word rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char strbuf[BUFSIZ];
    int i;
    word list, addr, count;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void stmdb_32_et1(void *ctx[0x%p], word w[0x%x], word rn[0x%x], word m[0x%x], word rl[0x%x])", ctx, w, rn, m, rl);
    /*E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");*/

    /* 20190218 experimental implemented */
    list = (m << 14) | rl;
    strbuf[0] = '{';
    strbuf[1] = '\0';
    for(i = 0; i <= 14; i++) {
        if((list & (1 << i)) != 0) {
            strncat(strbuf, e4arduino_register_name[i], BUFSIZ - 1);
            strncat(strbuf, ", ", BUFSIZ - 1);
        }
    }
    strbuf[strlen(strbuf) - 2] = '}';
    strbuf[strlen(strbuf) - 1] = '\0';
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t\tstmdb%s\t%s%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rn], (w != 0) ? "!" : "", strbuf);
    count = 0;
    for(i = 0; i < 16; i++) {
        if((list & (1 << i)) != 0) {
            count++;
        }
    }
    if((rn == 15) || (count < 2)) {
        e4arduino_unpredictable(context);
    } else if((w == 1) && (list & (1 << rn) != 0)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn] - 4 * count;
        for(i = 0; i <= 14; i++) {
            if((list & (1 << i)) != 0) {
                e4arduino_set_word_to_memory(context, addr, context->registers.r[i]);
                addr += 4;
            }
        }
        if (w == 1) {
            context->registers.r[rn] -= 4 * count;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1 + count;  /* 1 + nregs */

    context->registers.r[PC] += 4;
}

void
push_32_et2(void *ctx, word m, word rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    char strbuf[BUFSIZ];
    int i, count;
    word list;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void push_32_et2(void *ctx[0x%p], word m[0x%x], word rl[0x%x])", ctx, m, rl);
    list = (m << 14) | rl;
    strbuf[0] = '\0';
    count = 0;
    for(i = 0; i <= 15; i++) {
        if((list & (1 << i)) != 0) {
            strncat(strbuf, e4arduino_register_name[i], BUFSIZ - 1);
            strncat(strbuf, ", ", BUFSIZ - 1);
        }
        count++;
    }
    strbuf[strlen(strbuf) - 2] = '\0';
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tpush%s.w\t{%s}", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), strbuf);
    if(count < 2) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        for(i = 15; i >= 0; i--) {
            if((list & (1 << i)) != 0) {
                e4arduino_push_to_stack(context, context->registers.r[i]);
            }
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1 + count;  /* 1 + nregs */
    context->registers.r[PC] += 4;
}

void
ldmdb_32_et1(void *ctx, word w, word rn, word p, word m, word rl)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldmdb_32_et1(void *ctx[0x%p], word w[0x%x], word rn[0x%x], word p[0x%x], word m[0x%x], word rl[0x%x])", ctx, w, rn, p, m, rl);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
strex_32_et1(void *ctx, word rn, word rt, word rd, word imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strex_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word rd[0x%x], word imm[0x%x])", ctx, rn, rt, rd, imm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrex_32_et1(void *ctx, word rn, word rt, word imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrex_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm[0x%x])", ctx, rn, rt, imm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
strd_imm_32_et1(void *ctx, word p, word u, word w, word rn, word rt, word rt2, word imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, offset, addr;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strd_imm_32_et1(void *ctx[0x%p], word p[0x%x], word u[0x%x], word w[0x%x], word rn[0x%x], word rt[0x%x], word rt2[0x%x], word imm[0x%x])", ctx, p, u, w, rn, rt, rt2, imm);
    immediate = e4arduino_zero_extend((imm << 2), 32);
    if((p == 1) && (w == 0)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tstrd%s\t%s, [%s, #%d]!", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rt2], e4arduino_register_name[rn], immediate);
    } else if((p == 1) && (w == 1)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tstrd%s\t%s, [%s, #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rt2], e4arduino_register_name[rn], immediate);
    } else if((p == 0) && (w == 1)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tstrd%s\t%s, [%s], #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rt2], e4arduino_register_name[rn], immediate);
    }
    if((w == 1) && ((rn == rt) || (rn == rt2))) {
        e4arduino_unpredictable(context);
    } else if((rn == 15) || (rt == 13) || (rt == 15) || (rt2 == 13) || (rt2 == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        offset = (u == 1) ? context->registers.r[rn] + immediate : context->registers.r[rn] - immediate;
        addr = (p == 1) ? offset : context->registers.r[rn];
        e4arduino_set_word_to_memory(context, addr + 0x0, context->registers.r[rt]);
        e4arduino_set_word_to_memory(context, addr + 0x4, context->registers.r[rt2]);
        if(w == 1) {
            context->registers.r[rn] = offset;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 4;  /* 1 + n cycles */
    context->registers.r[PC] += 4;
}

void
ldrd_imm_32_et1(void *ctx, word p, word u, word w, word rn, word rt, word rt2, word imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrd_imm_32_et1(void *ctx[0x%p], word p[0x%x], word u[0x%x], word w[0x%x], word rn[0x%x], word rt[0x%x], word rt2[0x%x], word imm[0x%x])", ctx, p, u, w, rn, rt, rt2, imm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldrd%s.w\t%s, %s, [%s, #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rt2], e4arduino_register_name[rn], imm);
    if((w == 1) && ((rn == rt) || (rn == rt2))) {
        e4arduino_unpredictable(context);
    } else if((rt == 13) || (rt == 15) || (rt2 == 13) || (rt2 == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        offset = context->registers.r[rn] + ((u == 1) ? imm : -imm);
        addr = (p == 1) ? offset : context-> registers.r[rn];
        context->registers.r[rt] = e4arduino_get_word_from_memory(context, addr + 0);
        context->registers.r[rt2] = e4arduino_get_word_from_memory(context, addr + 4);
        if(w == 1) {
            context->registers.r[rn] = offset;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
ldrd_lit_32_et1(void *ctx, word p, word u, word w, word rt, word rt2, word imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrd_lit_32_et1(void *ctx[0x%p], word p[0x%x], word u[0x%x], word w[0x%x], word rt[0x%x], word rt2[0x%x], word imm[0x%x])", ctx, p, u, w, rt, rt2, imm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
strexb_32_et1(void *ctx, word rn, word rt, word rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strexb_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word rd[0x%x])", ctx, rn, rt, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
strexh_32_et1(void *ctx, word rn, word rt, word rd)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strexh_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word rd[0x%x])", ctx, rn, rt, rd);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
tbb_32_et1(void *ctx, word rn, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void tbb_32_et1(void *ctx[0x%p], word rn[0x%x], word rm[0x%x])", ctx, rn, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
tbh_32_et1(void *ctx, word rn, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void tbh_32_et1(void *ctx[0x%p], word rn[0x%x], word rm[0x%x])", ctx, rn, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrexb_32_et1(void *ctx, word rn, word rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrexb_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x])", ctx, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrexh_32_et1(void *ctx, word rn, word rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrexh_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x])", ctx, rn, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
and_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void and_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tand%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        switch(shift_t) {
        case E4ARDUINO_SHIFT_TYPE_LSL:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tand%s%s.w\t%s, %s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_LSR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tand%s%s.w\t%s, %s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ASR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tand%s%s.w\t%s, %s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_RRX:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tand%s%s.w\t%s, %s, %s, rrx %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ROR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tand%s%s.w\t%s, %s, %s, ror %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        }
    }
    if((rd == 13) || ((rd == 15) && (s == 0)) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = context->registers.r[rn] & e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
tst_reg_32_et2(void *ctx, word rn, word imm3, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void tst_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word imm3[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, rn, imm3, imm2, type, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
bic_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void bic_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tbic%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        switch(shift_t) {
        case E4ARDUINO_SHIFT_TYPE_LSL:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tbic%s%s.w\t%s, %s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_LSR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tbic%s%s.w\t%s, %s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ASR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tbic%s%s.w\t%s, %s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_RRX:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tbic%s%s.w\t%s, %s, %s, rrx %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ROR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tbic%s%s.w\t%s, %s, %s, ror %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        }
    }
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        context->registers.r[rd] = context->registers.r[rn] & ~value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
orr_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void orr_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\torr%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        switch(shift_t) {
        case E4ARDUINO_SHIFT_TYPE_LSL:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\torr%s%s.w\t%s, %s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_LSR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\torr%s%s.w\t%s, %s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ASR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\torr%s%s.w\t%s, %s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_RRX:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\torr%s%s.w\t%s, %s, %s, rrx %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ROR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\torr%s%s.w\t%s, %s, %s, ror %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        }
    }
    if((rd == 13) || (rd == 15) || (rn == 13) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = context->registers.r[rn] | e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
mov_reg_32_et3(void *ctx, word s, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mov_reg_32_et3(void *ctx[0x%p], word s[0x%x], word rd[0x%x], word rm[0x%x])", ctx, s, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
lsl_imm_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, shift_t;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void lsl_imm_32_et2(void *ctx[0x%p], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, s, imm3, rd, imm2, rm);
    immediate = (imm3 << 2) | imm2;
    shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
    if(immediate == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tlsl%s%s.w\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tlsl%s%s.w\t%s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm], immediate);
    }
    if((rd == 13) || (rd == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL_C, immediate, &carry);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
lsr_imm_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, shift_t;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void lsr_imm_32_et2(void *ctx[0x%p], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, s, imm3, rd, imm2, rm);
    immediate = (imm3 << 2) | imm2;
    shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
    if(immediate == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tlsr%s%s.w\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tlsr%s%s.w\t%s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm], immediate);
    }
    if((rd == 13) || (rd == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rm], shift_t, immediate, &carry);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
asr_imm_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, shift_t;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
   }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void asr_imm_32_et2(void *ctx[0x%p], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, s, imm3, rd, imm2, rm);
    immediate = (imm3 << 2) | imm2;
    shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
    if(immediate == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tasr%s%s.w\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tasr%s%s.w\t%s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm], immediate);
    }
    if((rd == 13) || (rd == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_ASR, immediate, &carry);
        context->registers.r[rd] = value;
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
rrx_32_et1(void *ctx, word s, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void rrx_32_et1(void *ctx[0x%p], word s[0x%x], word rd[0x%x], word rm[0x%x])", ctx, s, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\trrx%s%s\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if((rd == 13) || (rd == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_RRX, 1, &carry);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
ror_imm_32_et1(void *ctx, word s, word imm3, word rd, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ror_imm_32_et1(void *ctx[0x%p], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, s, imm3, rd, imm2, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
orn_reg_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void orn_reg_32_et1(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mvn_reg_32_et2(void *ctx, word s, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mvn_reg_32_et2(void *ctx[0x%p], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmvn%s%s.w\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    } else {
        switch(shift_t) {
        case E4ARDUINO_SHIFT_TYPE_LSL:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmvn%s%s.w\t%s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_LSR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmvn%s%s.w\t%s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ASR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmvn%s%s.w\t%s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_RRX:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmvn%s%s.w\t%s, %s, rrx %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ROR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmvn%s%s.w\t%s, %s, ror %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm], shift_n);
            break;
        }
    }
    if((rd == 13) || (rd == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        context->registers.r[rd] = ~value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
eor_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void eor_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\teor%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        switch(shift_t) {
        case E4ARDUINO_SHIFT_TYPE_LSL:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\teor%s.w\t%s, %s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_LSR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\teor%s.w\t%s, %s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ASR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\teor%s.w\t%s, %s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_RRX:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\teor%s.w\t%s, %s, %s, rrx %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ROR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\teor%s.w\t%s, %s, %s, ror %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        }
    }
    if((rd == 13) || ((rd == 15) && (s == 0)) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = context->registers.r[rn] ^ e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
teq_reg_32_et1(void *ctx, word rn, word imm3, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void teq_reg_32_et1(void *ctx[0x%p], word rn[0x%x], word imm3[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, rn, imm3, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tteq%s.w\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        switch(shift_t) {
        case E4ARDUINO_SHIFT_TYPE_LSL:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tteq%s.w\t%s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_LSR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tteq%s.w\t%s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ASR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tteq%s.w\t%s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_RRX:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tteq%s.w\t%s, %s, rrx %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ROR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tteq%s.w\t%s, %s, ror %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        }
    }
    if((rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else {
        carry = e4arduino_get_carry_flag(context);
        value = context->registers.r[rn] ^ e4arduino_shift(context->registers.r[rn], shift_t, shift_n, &carry);
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
pkhbt_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word t, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pkhbt_32_et1(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word t[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, t, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
pkhtb_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word t, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pkhtb_32_et1(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word t[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, t, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
add_reg_32_et3(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, shift_t, shift_n, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_reg_32_et3(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadd%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        switch(shift_t) {
        case E4ARDUINO_SHIFT_TYPE_LSL:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadd%s%s.w\t%s, %s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_LSR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadd%s%s.w\t%s, %s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ASR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadd%s%s.w\t%s, %s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_RRX:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadd%s%s.w\t%s, %s, %s, rrx %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ROR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadd%s%s.w\t%s, %s, %s, ror %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        }
    }
    if((rd == 13) || ((rd == 15) && (s == 0)) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        overflow = 0;
        value = e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        context->registers.r[rd] = e4arduino_add_with_carry(context->registers.r[rn], value, &carry, &overflow);
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
add_sp_reg_32_et3(void *ctx, word s, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_sp_reg_32_et3(void *ctx[0x%p], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, imm3, rd, imm2, type, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
cmn_reg_32_et2(void *ctx, word rn, word imm3, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cmn_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word imm3[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, rn, imm3, imm2, type, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
adc_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, shifted, carry, overflow, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void adc_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadc%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        switch(shift_t) {
        case E4ARDUINO_SHIFT_TYPE_LSL:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadc%s%s.w\t%s, %s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_LSR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadc%s%s.w\t%s, %s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ASR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadc%s%s.w\t%s, %s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_RRX:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadc%s%s.w\t%s, %s, %s, rrx %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ROR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadc%s%s.w\t%s, %s, %s, ror %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        }
    }
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        overflow = 0;
        shifted = e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        value = e4arduino_add_with_carry(context->registers.r[rn], shifted, &carry, &overflow);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, e4arduino_get_overflow_flag(context));
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
sbc_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, shifted, carry, overflow, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sbc_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tsbc%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tsbc%s%s.w\t%s, %s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], immediate);
    }
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        overflow = 0;
        shifted = e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        value = e4arduino_add_with_carry(context->registers.r[rn], ~shifted, &carry, &overflow);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
       }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
sub_reg_32_et2(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, shifted, carry, overflow, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tsub%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tsub%s%s.w\t%s, %s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], immediate);
    }
    if((rd == 13) || ((rd == 15) && (s == 0)) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        overflow = 0;
        shifted = e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        carry = 1;
        value = e4arduino_add_with_carry(context->registers.r[rn], ~shifted, &carry, &overflow);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
sub_sp_reg_32_et1(void *ctx, word s, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_sp_reg_32_et1(void *ctx[0x%p], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, imm3, rd, imm2, type, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
cmp_reg_32_et3(void *ctx, word rn, word imm3, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cmp_reg_32_et3(void *ctx[0x%p], word rn[0x%x], word imm3[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, rn, imm3, imm2, type, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
rsb_reg_32_et1(void *ctx, word s, word rn, word imm3, word rd, word imm2, word type, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, shifted, carry, overflow, shift_t, shift_n;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void rsb_reg_32_et1(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word type[0x%x], word rm[0x%x])", ctx, s, rn, imm3, rd, imm2, type, rm);
    immediate = (imm3 << 2) | imm2;
    switch(type) {
    case 0:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSL;
        shift_n = immediate;
        break;
    case 1:
        shift_t = E4ARDUINO_SHIFT_TYPE_LSR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 2:
        shift_t = E4ARDUINO_SHIFT_TYPE_ASR;
        shift_n = (immediate == 0) ? 32 : immediate;
        break;
    case 3:
        if(immediate == 0) {
            shift_t = E4ARDUINO_SHIFT_TYPE_RRX;
            shift_n = 1;
        } else {
            shift_t = E4ARDUINO_SHIFT_TYPE_ROR;
            shift_n = immediate;
        }
        break;
    }
    if(shift_n == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\trsb%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        switch(shift_t) {
        case E4ARDUINO_SHIFT_TYPE_LSL:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\trsb%s%s.w\t%s, %s, %s, lsl %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_LSR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\trsb%s%s.w\t%s, %s, %s, lsr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ASR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\trsb%s%s.w\t%s, %s, %s, asr %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_RRX:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\trsb%s%s.w\t%s, %s, %s, rrx %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        case E4ARDUINO_SHIFT_TYPE_ROR:
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\trsb%s%s.w\t%s, %s, %s, ror %d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm], shift_n);
            break;
        }
    }
    if((rd == 13) || ((rd == 15) && (s == 0)) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        overflow = 1;
        shifted = e4arduino_shift(context->registers.r[rm], shift_t, shift_n, &carry);
        carry = 1;
        value = e4arduino_add_with_carry(~(context->registers.r[rn]), shifted, &carry, &overflow);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, e4arduino_get_overflow_flag(context));
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
stc_32_et1(void *ctx, word p, word u, word n, word w, word rn, word crd, word cprc, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void stc_32_et1(void *ctx[0x%p], word p[0x%x], word u[0x%x], word n[0x%x], word w[0x%x], word rn[0x%x], word crd[0x%x], word cprc[0x%x], word imm8[0x%x])", ctx, p, u, n, w, rn, crd, cprc, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
stc2_32_et2(void *ctx, word p, word u, word n, word w, word rn, word crd, word cprc, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void stc2_32_et2(void *ctx[0x%p], word p[0x%x], word u[0x%x], word n[0x%x], word w[0x%x], word rn[0x%x], word crd[0x%x], word cprc[0x%x], word imm8[0x%x])", ctx, p, u, n, w, rn, crd, cprc, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldc_imm_32_et1(void *ctx, word p, word u, word d, word w, word rn, word crd, word cprc, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldc_imm_32_et1(void *ctx[0x%p], word p[0x%x], word u[0x%x], word d[0x%x], word w[0x%x], word rn[0x%x], word crd[0x%x], word cprc[0x%x], word imm8[0x%x])", ctx, p, u, d, w, rn, crd, cprc, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldc2_imm_32_et2(void *ctx, word p, word u, word d, word w, word rn, word crd, word cprc, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldc2_imm_32_et2(void *ctx[0x%p], word p[0x%x], word u[0x%x], word d[0x%x], word w[0x%x], word rn[0x%x], word crd[0x%x], word cprc[0x%x], word imm8[0x%x])", ctx, p, u, d, w, rn, crd, cprc, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldc_lit_32_et1(void *ctx, word p, word u, word d, word w, word crd, word cprc, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldc_lit_32_et1(void *ctx[0x%p], word p[0x%x], word u[0x%x], word d[0x%x], word w[0x%x], word crd[0x%x], word cprc[0x%x], word imm8[0x%x])", ctx, p, u, d, w, crd, cprc, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldc2_lit_32_et2(void *ctx, word p, word u, word d, word w, word crd, word cprc, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldc2_lit_32_et2(void *ctx[0x%p], word p[0x%x], word u[0x%x], word d[0x%x], word w[0x%x], word crd[0x%x], word cprc[0x%x], word imm8[0x%x])", ctx, p, u, d, w, crd, cprc, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mcrr_32_et1(void *ctx, word rt2, word rt, word cprc, word opc1, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mcrr_32_et1(void *ctx[0x%p], word rt2[0x%x], word rt[0x%x], word cprc[0x%x], word opc1[0x%x], word crm[0x%x])", ctx, rt2, rt, cprc, opc1, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mcrr2_32_et2(void *ctx, word rt2, word rt, word cprc, word opc1, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mcrr2_32_et2(void *ctx[0x%p], word rt2[0x%x], word rt[0x%x], word cprc[0x%x], word opc1[0x%x], word crm[0x%x])", ctx, rt2, rt, cprc, opc1, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mrrc_32_et1(void *ctx, word rt2, word rt, word cprc, word opc1, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mrrc_32_et1(void *ctx[0x%p], word rt2[0x%x], word rt[0x%x], word cprc[0x%x], word opc1[0x%x], word crm[0x%x])", ctx, rt2, rt, cprc, opc1, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mrrc2_32_et2(void *ctx, word rt2, word rt, word cprc, word opc1, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mrrc2_32_et2(void *ctx[0x%p], word rt2[0x%x], word rt[0x%x], word cprc[0x%x], word opc1[0x%x], word crm[0x%x])", ctx, rt2, rt, cprc, opc1, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
cdp_32_et1(void *ctx, word opc1, word crn, word crd, word cprc, word opc2, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cdp_32_et1(void *ctx[0x%p], word opc1[0x%x], word crn[0x%x], word crd[0x%x], word cprc[0x%x], word opc2[0x%x], word crm[0x%x])", ctx, opc1, crn, crd, cprc, opc2, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
cdp2_32_et2(void *ctx, word opc1, word crn, word crd, word cprc, word opc2, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cdp2_32_et2(void *ctx[0x%p], word opc1[0x%x], word crn[0x%x], word crd[0x%x], word cprc[0x%x], word opc2[0x%x], word crm[0x%x])", ctx, opc1, crn, crd, cprc, opc2, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mcr_32_et1(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mcr_32_et1(void *ctx[0x%p], word opc1[0x%x], word crn[0x%x], word rt[0x%x], word cprc[0x%x], word opc2[0x%x], word crm[0x%x])", ctx, opc1, crn, rt, cprc, opc2, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mcr2_32_et2(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mcr2_32_et2(void *ctx[0x%p], word opc1[0x%x], word crn[0x%x], word rt[0x%x], word cprc[0x%x], word opc2[0x%x], word crm[0x%x])", ctx, opc1, crn, rt, cprc, opc2, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mrc_32_et1(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mrc_32_et1(void *ctx[0x%p], word opc1[0x%x], word crn[0x%x], word rt[0x%x], word cprc[0x%x], word opc2[0x%x], word crm[0x%x])", ctx, opc1, crn, rt, cprc, opc2, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mrc2_32_et2(void *ctx, word opc1, word crn, word rt, word cprc, word opc2, word crm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mrc2_32_et2(void *ctx[0x%p], word opc1[0x%x], word crn[0x%x], word rt[0x%x], word cprc[0x%x], word opc2[0x%x], word crm[0x%x])", ctx, opc1, crn, rt, cprc, opc2, crm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
and_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void and_imm_32_et1(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    carry = e4arduino_get_carry_flag(context);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | (imm8 << 0), &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tand%s%s.w\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if((rd == 13) || ((rd == 15) && (s == 0)) || (rn == 13) || (rn == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rn] & immediate;
        context->registers.r[rd] = value;
        if(s != 0) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
tst_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void tst_imm_32_et1(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, imm8);
    carry = e4arduino_get_carry_flag(context);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | imm8, &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\ttst%s\t%s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], immediate);
    if((rn == 13) || (rn == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rn] & immediate;
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
bic_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void bic_imm_32_et1(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    carry = e4arduino_get_carry_flag(context);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | imm8, &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tbic%s%s\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if((rd == 13) || (rd == 15) || (rn == 13) || (rd == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rn] & ~immediate;
        context->registers.r[rd] = value;
        if(s != 0) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
orr_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void orr_imm_32_et1(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    carry = e4arduino_get_carry_flag(context);
    value = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | imm8, &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\torr%s%s\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], value);
    if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[rd] = context->registers.r[rn] | value;
        if(s != 0) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, e4arduino_get_overflow_flag(context));
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
mov_imm_32_et2(void *ctx, word i, word s, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word carry, value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mov_imm_32_et2(void *ctx[0x%p], word i[0x%x], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, imm3, rd, imm8);
    carry = e4arduino_get_carry_flag(context);
    value = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | imm8, &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmov%s%s.w\t%s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], value);
    if((rd == 13) || (rd == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[rd] = value;
        if(s != 0) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, e4arduino_get_overflow_flag(context));
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
orn_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void orn_imm_32_et1(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    carry = e4arduino_get_carry_flag(context);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | imm8, &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\torn%s%s\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if((rd == 13) || (rd == 15) || (rn == 13)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rn] | ~immediate;
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
mvn_imm_32_et1(void *ctx, word i, word s, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mvn_imm_32_et1(void *ctx[0x%p], word i[0x%x], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, imm3, rd, imm8);
    carry = e4arduino_get_carry_flag(context);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | (imm8 << 0), &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmvn%s%s\t%s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], immediate);
    if((rd == 13) || (rd == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = ~immediate;
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
eor_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void eor_imm_32_et1(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    carry = e4arduino_get_carry_flag(context);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | imm8, &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\teos%s%s\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if((rd == 13) || ((rd == 15) && (s == 0)) || (rn == 13) || (rn == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rn] ^ immediate;
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
teq_imm_32_et1(void *ctx, word i, word rn, word imm3, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void teq_imm_32_et1(void *ctx[0x%p], word i[0x%x], word rn[0x%x], word imm3[0x%x], word imm8[0x%x])", ctx, i, rn, imm3, imm8);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | (imm8 << 0), &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tteq%s\t%s, #%d", context->registers.r[PC], OP32((word *)&
     context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], immediate);
    if((rn == 13) || (rn == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rn] ^ immediate;
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
        e4arduino_advance_itstate(context);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
add_imm_32_et3(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_imm_32_et3(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | (imm8 << 0), NULL);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadd%s%s.w\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if((rd == 13) || ((rd == 15) && (s == 0))) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 0;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], immediate, &carry, &overflow);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
add_sp_imm_32_et3(void *ctx, word i, word s, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_sp_imm_32_et3(void *ctx[0x%p], word i[0x%x], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, imm3, rd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
cmn_imm_32_et1(void *ctx, word i, word rn, word imm3, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cmn_imm_32_et1(void *ctx[0x%p], word i[0x%x], word rn[0x%x], word imm3[0x%x], word imm8[0x%x])", ctx, i, rn, imm3, imm8);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | imm8, NULL);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tcmn%s\t%s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], immediate);
    if(rn == 15) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 0;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], immediate, &carry, &overflow);
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
        e4arduino_set_overflow_flag(context, overflow);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
adc_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void adc_imm_32_et1(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | imm8, &carry);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tadc%s%s\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], immediate, &carry, &overflow);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
sbc_imm_32_et1(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sbc_imm_32_et1(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | (imm8 << 0), NULL);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tsbc%s%s\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], ~immediate, &carry, &overflow);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
sub_imm_32_et3(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, value, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_imm_32_et3(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | (imm8 << 0), NULL);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tsub%s%s.w\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s != 0) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if((rd == 13) || ((rd == 15) && (s == 0)) || (rn == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 1;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], ~immediate, &carry, &overflow);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
sub_sp_imm_32_et2(void *ctx, word i, word s, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_sp_imm_32_et2(void *ctx[0x%p], word i[0x%x], word s[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, imm3, rd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
cmp_imm_32_et2(void *ctx, word i, word rn, word imm3, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void cmp_imm_32_et2(void *ctx[0x%p], word i[0x%x], word rn[0x%x], word imm3[0x%x], word imm8[0x%x])", ctx, i, rn, imm3, imm8);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | imm8, NULL);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tcmp%s.w\t%s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], immediate);
    if(rn == 15) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 1;
        overflow = 0;
        value = e4arduino_add_with_carry(context->registers.r[rn], ~immediate, &carry, &overflow);
        e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
        e4arduino_set_zero_flag(context, (value == 0));
        e4arduino_set_carry_flag(context, carry);
        e4arduino_set_overflow_flag(context, overflow);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
rsb_imm_32_et2(void *ctx, word i, word s, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, carry, overflow;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void rsb_imm_32_et2(void *ctx[0x%p], word i[0x%x], word s[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, s, rn, imm3, rd, imm8);
    immediate = e4arduino_thumb_expand_imm_c((i << 11) | (imm3 << 8) | (imm8 << 0), NULL);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\trsb%s%s.w\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_in_it_block(context) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], immediate);
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = 1;
        overflow = 0;
        value = e4arduino_add_with_carry(~context->registers.r[rn], immediate, &carry, &overflow);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
            e4arduino_set_overflow_flag(context, overflow);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
add_imm_32_et4(void *ctx, word i, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_imm_32_et4(void *ctx[0x%p], word i[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, rn, imm3, rd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
add_sp_imm_32_et4(void *ctx, word i, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void add_sp_imm_32_et4(void *ctx[0x%p], word i[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, imm3, rd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
adr_32_et2(void *ctx, word i, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void adr_32_et2(void *ctx[0x%p], word i[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, imm3, rd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mov_imm_32_et3(void *ctx, word i, word imm4, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mov_imm_32_et3(void *ctx[0x%p], word i[0x%x], word imm4[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, imm4, imm3, rd, imm8);
    value = e4arduino_zero_extend((imm4 << 12) | (i << 11) | (imm3 << 8) | imm8, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmovw%s\t%s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], value);
    if((rd == 13) || (rd == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[rd] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
sub_imm_32_et4(void *ctx, word i, word rn, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_imm_32_et4(void *ctx[0x%p], word i[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, rn, imm3, rd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sub_sp_imm_32_et3(void *ctx, word i, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sub_sp_imm_32_et3(void *ctx[0x%p], word i[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, imm3, rd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
adr_32_et3(void *ctx, word i, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void adr_32_et3(void *ctx[0x%p], word i[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, imm3, rd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
movt_32_et1(void *ctx, word i, word imm4, word imm3, word rd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void movt_32_et1(void *ctx[0x%p], word i[0x%x], word imm4[0x%x], word imm3[0x%x], word rd[0x%x], word imm8[0x%x])", ctx, i, imm4, imm3, rd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ssat_32_et1(void *ctx, word sh, word rn, word imm3, word rd, word imm2, word si)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ssat_32_et1(void *ctx[0x%p], word sh[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word si[0x%x])", ctx, sh, rn, imm3, rd, imm2, si);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ssat16_32_et1(void *ctx, word rn, word rd, word si)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ssat16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word si[0x%x])", ctx, rn, rd, si);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sbfx_32_et1(void *ctx, word rn, word imm3, word rd, word imm2, word wm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sbfx_32_et1(void *ctx[0x%p], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word wm[0x%x])", ctx, rn, imm3, rd, imm2, wm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
bfi_32_et1(void *ctx, word rn, word imm3, word rd, word imm2, word msb)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word lsbit;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void bfi_32_et1(void *ctx[0x%p], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word msb[0x%x])", ctx, rn, imm3, rd, imm2, msb);
    lsbit = (imm3 << 2) | imm2;
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tbfi%s\t%s, %s, #%d, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], lsbit, (msb - lsbit + 1));
    if((rd == 13) || (rd == 15) || (rn == 13)) {
        e4arduino_unpredictable(context);
    } else if (!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        if(msb >= lsbit) {
            context->registers.r[rd] = (context->registers.r[rd] & ((((1 << (msb - lsbit + 1))) - 1) << lsbit)) | (context->registers.r[rn] & (((1 << (msb - lsbit + 1))) - 1));
        } else {
            e4arduino_unpredictable(context);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
bfc_32_et1(void *ctx, word imm3, word rd, word imm2, word msb)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void bfc_32_et1(void *ctx[0x%p], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word msb[0x%x])", ctx, imm3, rd, imm2, msb);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
usat_32_et1(void *ctx, word sh, word rn, word imm3, word rd, word imm2, word si)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void usat_32_et1(void *ctx[0x%p], word sh[0x%x], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word si[0x%x])", ctx, sh, rn, imm3, rd, imm2, si);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
usat16_32_et1(void *ctx, word rn, word rd, word si)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void usat16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word si[0x%x])", ctx, rn, rd, si);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ubfx_32_et1(void *ctx, word rn, word imm3, word rd, word imm2, word wm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ubfx_32_et1(void *ctx[0x%p], word rn[0x%x], word imm3[0x%x], word rd[0x%x], word imm2[0x%x], word wm[0x%x])", ctx, rn, imm3, rd, imm2, wm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
b_32_et3(void *ctx, word s, word cond, word imm6, word j1, word j2, word imm11)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void b_32_et3(void *ctx[0x%p], word s[0x%x], word cond[0x%x], word imm6[0x%x], word j1[0x%x], word j2[0x%x], word imm11[0x%x])", ctx, s, cond, imm6, j1, j2, imm11);
    value = e4arduino_sign_extend((s << 20) | (j2 << 19) | (j1 << 18) | (imm6 << 12) | (imm11 << 1), 21);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tb%s.w\t%x", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_cond_name[cond], context->registers.r[PC] + value + 4);
    if(e4arduino_in_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(e4arduino_judge_condition(context, cond)) {
        context->registers.r[PC] += (value + 4);
    } else {
        context->registers.r[PC] += 4;
    }
    e4arduino_advance_itstate(context);
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;  /* from 2 to 4 cycles */
}

void
msr_32_et1(void *ctx, word rn, word mask, word sysm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void msr_32_et1(void *ctx[0x%p], word rn[0x%x], word mask[0x%x], word sysm[0x%x])", ctx, rn, mask, sysm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmsr%s\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_special_register_name[sysm], e4arduino_register_name[rn]);
    if((mask == 0) || ((mask != 2) && !(sysm < 4)) || (rn == 13) || (rn == 15) || (sysm == 4) || ((sysm >= 10) && (sysm <= 15)) || (sysm > 20)) {
        e4arduino_unpredictable(context);
    } else if (!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        switch((sysm & 0x000000f8) >> 3) {
        case 0:
            if((sysm & (1 << 2)) == 0) {
                if((mask & (1 << 0)) != 0) {
                    if(e4arduino_have_dsp_extention(context)) {
                        e4arduino_unpredictable(context);
                    } else {
                        context->special_registers.r[APSR] = (context->special_registers.r[APSR] & ~(0x0000000f << 16)) | (context->registers.r[rn] & (0x0000000f << 16));
                    }
                }
                if((mask & (1 << 1)) != 0) {
                    context->special_registers.r[APSR] = (context->special_registers.r[APSR] & ~(0x0000001f << 27)) | (context->registers.r[rn] & (0x0000001f << 27));
                }
            }
            break;
        case 1:
            if(e4arduino_is_privileged(context)) {
                if((sysm & 3) == 0) {
                    context->registers.bank0[SP] = context->registers.r[rn];
                } else if((sysm & 3) == 1) {
                    context->registers.bank1[SP] = context->registers.r[rn];
                }
            }
            break;
        case 2:
            if((sysm & 7) == 0) {
                if(e4arduino_is_privileged(context)) {
                    context->special_registers.r[PRIMASK] = (context->special_registers.r[PRIMASK] & ~0x00000001) | (context->registers.r[rn] & 0x00000001);
                }
            } else if((sysm & 7) == 1) {
                if(e4arduino_is_privileged(context)) {
                    context->special_registers.r[BASEPRI] = (context->special_registers.r[BASEPRI] & ~0x000000ff) | (context->registers.r[rn] & 0x000000ff);
                }
            } else if((sysm & 7) == 2) {
                if((e4arduino_is_privileged(context)) && ((context->registers.r[rn] & 0x000000ff) != 0x00000000) && (((context->registers.r[rn] & 0x000000ff) < (context->special_registers.r[BASEPRI] & 0x000000ff)) || ((context->special_registers.r[BASEPRI] & 0x000000ff) == 0x00000000))) {
                    context->special_registers.r[BASEPRI] = (context->special_registers.r[BASEPRI] & ~0x000000ff) | (context->registers.r[rn] & 0x000000ff);
                }
            } else if((sysm & 7) == 3) {
                if(e4arduino_is_privileged(context) && (e4arduino_execution_priority(context) > -1)) {
                    context->special_registers.r[FAULTMASK] = (context->special_registers.r[FAULTMASK] & ~0x00000001) | (context->registers.r[rn] & 0x00000001);
                }
            } else if((sysm & 7) == 4) {
                if(e4arduino_is_privileged(context)) {
                    context->special_registers.r[CONTROL] = (context->special_registers.r[CONTROL] & ~E4ARDUINO_CONTROL_NPRIV) | (context->registers.r[rn] & E4ARDUINO_CONTROL_NPRIV);
                    if(e4arduino_get_current_execution_mode(context) == E4ARDUINO_EXECUTION_MODE_THREAD) {
                        word curr_stack, next_stack, sp;
                        curr_stack = context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_SPSEL;
                        next_stack = context->registers.r[rn] & E4ARDUINO_CONTROL_SPSEL;
                        if(curr_stack != next_stack) {
                            if(curr_stack == 0) {
                                sp = context->registers.bank1[SP];
                                e4arduino_to_use_psp(context);
                                context->registers.bank1[SP] = sp;
                            } else {
                                sp = context->registers.bank0[SP];
                                e4arduino_to_use_msp(context);
                                context->registers.bank0[SP] = sp;
                            }
                        }
                        context->special_registers.r[CONTROL] = (context->special_registers.r[CONTROL] & ~E4ARDUINO_CONTROL_SPSEL) | (context->registers.r[rn] & E4ARDUINO_CONTROL_SPSEL);
                    }
                    if(e4arduino_have_dsp_extention(context)) {
                        context->special_registers.r[CONTROL] = (context->special_registers.r[CONTROL] & ~E4ARDUINO_CONTROL_FPCA) | (context->registers.r[rn] & E4ARDUINO_CONTROL_FPCA);
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;  /* 1 or 2 cycles */
    context->registers.r[PC] += 4;
}

void
nop_32_et2(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void nop_32_et2(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
yield_32_et2(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void yield_32_et2(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
wfe_32_et2(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void wfe_32_et2(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\twfe%s.w", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context));
    context->wfe = 1;
    context->registers.r[PC] += 4;
}

void
wfi_32_et2(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void wfi_32_et2(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\twfi%s.w", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context));
    context->wfi = 1;
    context->registers.r[PC] += 4;
}

void
sev_32_et2(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sev_32_et2(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
dbg_32_et1(void *ctx, word opt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void dbg_32_et1(void *ctx[0x%p], word opt[0x%x])", ctx, opt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
clrex_32_et1(void *ctx)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void clrex_32_et1(void *ctx[0x%p], )", ctx);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
dsb_32_et1(void *ctx, word opt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void dsb_32_et1(void *ctx[0x%p], word opt[0x%x])", ctx, opt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tdsb%s\t%s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), (opt == 15) ? "sy" : "");
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;  /* 1 + B cycles */
    context->registers.r[PC] += 4;
}

void
dmb_32_et1(void *ctx, word opt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void dmb_32_et1(void *ctx[0x%p], word opt[0x%x])", ctx, opt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
isb_32_et1(void *ctx, word opt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void isb_32_et1(void *ctx[0x%p], word opt[0x%x])", ctx, opt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tisb%s\t%s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), (opt == 15) ? "sy" : "");
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;  /* 1 + B cycles */
    context->registers.r[PC] += 4;
}

void
mrs_32_et1(void *ctx, word rd, word sysm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mrs_32_et1(void *ctx[0x%p], word rd[0x%x], word sysm[0x%x])", ctx, rd, sysm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tmrs\t%s, %s", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_register_name[rd], e4arduino_special_register_name[sysm]);
    context->registers.r[rd] = 0;
    switch((sysm & 0x000000f8) >> 3) {
    case 0:
        if((sysm & 0x00000001) != 0) {
            context->registers.r[rd] |= (context->special_registers.r[IPSR] & 0x000001ff);
        }
        if((sysm & 0x00000002) != 0) {
            context->registers.r[rd] &= ~0x07000000;
            context->registers.r[rd] &= ~0x0000fc00;
        }
        if((sysm & 0x00000004) != 0) {
            context->registers.r[rd] |= (context->special_registers.r[APSR] & 0xf8000000);
            if(e4arduino_have_dsp_extention(context)) {
                context->registers.r[rd] |= (context->special_registers.r[APSR] & 0x000f0000);
            }
        }
        break;
    case 1:
        if(e4arduino_is_privileged(context)) {
            switch((sysm & 0x00000007) >> 0) {
            case 0:
                context->registers.r[rd] = context->registers.bank0[SP];
                break;
            case 1:
                context->registers.r[rd] = context->registers.bank1[SP];
                break;
            }
        }
        break;
    case 2:
        switch((sysm & 0x00000007) >> 0) {
        case 0:
            if(e4arduino_is_privileged(context)) {
                context->registers.r[rd] |= (context->special_registers.r[PRIMASK] & 0x00000001);
            } else {
                context->registers.r[rd] &= ~0x00000001;
            }
            break;
        case 1:
        case 2:
            if(e4arduino_is_privileged(context)) {
                context->registers.r[rd] |= (context->special_registers.r[BASEPRI] & 0x000000ff);
            } else {
                context->registers.r[rd] &= ~0x000000ff;
            }
            break;
        case 3:
            if(e4arduino_is_privileged(context)) {
                context->registers.r[rd] |= (context->special_registers.r[FAULTMASK] & 0x00000001);
            } else {
                context->registers.r[rd] &= ~0x00000001;
            }
            break;
        case 4:
            if(e4arduino_have_dsp_extention(context)) {
                context->registers.r[rd] |= (context->special_registers.r[CONTROL] & 0x00000007);
            } else {
                context->registers.r[rd] |= (context->special_registers.r[CONTROL] & 0x00000003);
            }
            break;
        }
        break;
    }
    context->wait_until = context->cycles + 2;  /* 1 or 2 */
    context->registers.r[PC] += 4;
}

void
udf_32_et2(void *ctx, word imm4, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void udf_32_et2(void *ctx[0x%p], word imm4[0x%x], word imm12[0x%x])", ctx, imm4, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
b_32_et4(void *ctx, word s, word imm10, word j1, word j2, word imm11)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, addr, i1, i2;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void b_32_et4(void *ctx[0x%p], word s[0x%x], word imm10[0x%x], word j1[0x%x], word j2[0x%x], word imm11[0x%x])", ctx, s, imm10, j1, j2, imm11);
    i1 = ~(j1 ^ s) & 1;
    i2 = ~(j2 ^ s) & 1;
    /* 20190207 bugfix */
    immediate = e4arduino_sign_extend((s << 24) | (i1 << 23) | (i2 << 22) | (imm10 << 12) | (imm11 << 1), 25);
    addr = context->registers.r[PC] + immediate + 4;
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tb%s.w\t%x", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), addr);
    if(e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[PC] = addr;
    } else {
        context->registers.r[PC] += 4;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
}

void
bl_32_et1(void *ctx, word s, word imm10, word j1, word j2, word imm11)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void bl_32_et1(void *ctx[0x%p], word s[0x%x], word imm10[0x%x], word j1[0x%x], word j2[0x%x], word imm11[0x%x])", ctx, s, imm10, j1, j2, imm11);
    value = e4arduino_sign_extend((s << 24) | (((~(j1 ^ s)) & 1) << 23) | (((~(j2 ^ s)) & 1) << 22) | (imm10 << 12) | (imm11 << 1), 25) + context->registers.r[PC] + 4;
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tbl%s\t%x", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), value);
    if(e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
#ifdef ENABLE_XFT
        if(context->xft_dispatch != NULL) {
            int result;

            result = context->xft_dispatch(value, context);
            if(result < 0) {
                E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_ERROR, "xft_dispatch() returned error");
                exit(EXIT_FAILURE);
            } else if(result == 0) {
                context->registers.r[LR] = context->registers.r[PC] + 5;
                context->registers.r[PC] = value;
            } else if(result > 0) {
                context->registers.r[PC] += 4;
            }
        }
#else /* !ENABLE_XFT */
        context->registers.r[LR] = context->registers.r[PC] + 5;
        context->registers.r[PC] = value;
#endif /* ENABLE_XFT */
    } else {
        context->registers.r[PC] += 4;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;  /* from 2 to 4 cycles */
}

void
strb_imm_32_et2(void *ctx, word rn, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, addr;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strb_imm_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, rn, rt, imm12);
    immediate = e4arduino_zero_extend(imm12, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tstrb%s.w\t%s, [%s, #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], immediate);
    if((rt == 13) || (rt == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn] + immediate;
        e4arduino_set_byte_to_memory(context, addr, context->registers.r[rt] & 0xff);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
strb_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, addr, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strb_imm_32_et3(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word p[0x%x], word u[0x%x], word w[0x%x], word imm8[0x%x])", ctx, rn, rt, p, u, w, imm8);
    immediate = e4arduino_zero_extend(imm8, 32);
    if((p != 0) && (w != 0)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstrb%s\t%s, [%s, #%s%d]!", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], (u != 0) ? "" : "-", immediate);
    } else if((p == 0) && (w != 0)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstrb%s\t%s, [%s], #%s%d", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], (u != 0) ? "" : "-", immediate);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %04x\t\tstrb%s\t%s, [%s, #%s%d]", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], (u != 0) ? "" : "-", immediate);
    }
    if((rn == 15) || ((p == 0) && (w == 0))) {
        e4arduino_undefined(context);
    } else if (((rt == 13) || (rt == 15)) && ((w == 1) && (rn == rt))) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
            offset = (u != 0) ?  context->registers.r[rn] + immediate : context->registers.r[rn] - immediate;
            addr = (p != 0) ? offset : context->registers.r[rn];
            e4arduino_set_byte_to_memory(context, addr, context->registers.r[rt] & 0x000000ff);
            if(w != 0) {
                context->registers.r[rn] = offset;
            }
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
strbt_32_et1(void *ctx, word rn, word rt, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strbt_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm8[0x%x])", ctx, rn, rt, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
strb_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strb_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, rn, rt, imm2, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
strh_imm_32_et2(void *ctx, word rn, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strh_imm_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, rn, rt, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
strh_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strh_imm_32_et3(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word p[0x%x], word u[0x%x], word w[0x%x], word imm8[0x%x])", ctx, rn, rt, p, u, w, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
strht_32_et1(void *ctx, word rn, word rt, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strht_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm8[0x%x])", ctx, rn, rt, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
strh_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strh_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, rn, rt, imm2, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
str_imm_32_et3(void *ctx, word rn, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word base, value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void str_imm_32_et3(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, rn, rt, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tstr%s.w\t%s, [%s, #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], imm12);
    if(rn == 15) {
        e4arduino_undefined(context);
    } else if(rt == 15) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        base = (context->registers.r[rn]) & 0xfffffffc;
        value = context->registers.r[rt];
        e4arduino_set_word_to_memory(context, base + e4arduino_zero_extend(imm12, 32), value);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
str_imm_32_et4(void *ctx, word rn, word rt, word p, word u, word w, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void str_imm_32_et4(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word p[0x%x], word u[0x%x], word w[0x%x], word imm8[0x%x])", ctx, rn, rt, p, u, w, imm8);
    if((p != 0) && (w == 0)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tstr%s.w\t%s, [%s, #%s%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], (u != 0) ? "" : "-", imm8);
    } else if((p != 0) && (w != 0)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tstr%s.w\t%s, [%s, #%s%d]!", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], (u != 0) ? "" : "-", imm8);
    } else if((p == 0) && (w != 0)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tstr%s.w\t%s, [%s], #%s%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], (u != 0) ? "" : "-", imm8);
    }
    if((rn == 15) || ((p == 0) && (w == 0))) {
        e4arduino_undefined(context);
    } else if((rt == 15) || ((w != 0) && (rn == rt))) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        if(u != 0) {
            offset = context->registers.r[rn] + e4arduino_zero_extend(imm8, 32);
        } else {
            offset = context->registers.r[rn] - e4arduino_zero_extend(imm8, 32);
        }
        if(p != 0) {
            addr = offset;
        } else {
            addr = context->registers.r[rn];
        }
        e4arduino_set_word_to_memory(context, addr, context->registers.r[rt]);
        if(w != 0) {
            context->registers.r[rn] = offset;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
strt_32_et1(void *ctx, word rn, word rt, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void strt_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm8[0x%x])", ctx, rn, rt, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
push_32_et3(void *ctx, word rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void push_32_et3(void *ctx[0x%p], word rt[0x%x])", ctx, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
str_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void str_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, rn, rt, imm2, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tstr%s.w\t%s, [%s, %s, lsl #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm], imm2);
    if((rt == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        addr = context->registers.r[rn] + e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, imm2, &carry);
        e4arduino_set_word_to_memory(context, addr, context->registers.r[rt]);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
ldrb_lit_32_et1(void *ctx, word u, word rt, word rmm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrb_lit_32_et1(void *ctx[0x%p], word u[0x%x], word rt[0x%x], word rmm12[0x%x])", ctx, u, rt, rmm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrb_imm_32_et2(void *ctx, word rn, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, addr;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrb_imm_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, rn, rt, imm12);
    immediate = e4arduino_zero_extend(imm12, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldrb%s.w\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], immediate);
    if(rt == 13) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn] + immediate;
        context->registers.r[rt] = e4arduino_zero_extend(e4arduino_get_byte_from_memory(context, addr), 32);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
ldrb_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrb_imm_32_et3(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word p[0x%x], word u[0x%x], word w[0x%x], word imm8[0x%x])", ctx, rn, rt, p, u, w, imm8);
    /*E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
      E4ARDUINO_PANIC("not implemented");*/

    // 20190206 experimatal implement
    if((p == 0) && (w == 0)) {
        e4arduino_undefined(context);
    } else if((rt == 13) || ((w == 1) && (rn == rt))) {
        e4arduino_unpredictable(context);
    } else if((rt == 15) && ((p == 0) || (u == 1) || (w == 1))) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        if(u != 0) {
            offset = context->registers.r[rn] + e4arduino_zero_extend(imm8, 32);
        } else {
            offset = context->registers.r[rn] - e4arduino_zero_extend(imm8, 32);
        }
        if(p != 0) {
            addr = offset;
        } else {
            addr = context->registers.r[rn];
        }
        context->registers.r[rt] = e4arduino_zero_extend(e4arduino_get_word_from_memory(context, addr), 32);
        if(w == 1) {
            context->registers.r[rn] = offset;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
ldrbt_32_et1(void *ctx, word rn, word rt, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrbt_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm8[0x%x])", ctx, rn, rt, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrb_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrb_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, rn, rt, imm2, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrsb_lit_32_et1(void *ctx, word u, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsb_lit_32_et1(void *ctx[0x%p], word u[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, u, rt, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrsb_imm_32_et1(void *ctx, word rn, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word immediate, addr;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsb_imm_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, rn, rt, imm12);
    /*E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");*/

    /* experimental implement 20190207 */
    immediate = e4arduino_zero_extend(imm12, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldrsb%s.w\t%s, %s, #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], immediate);
    if(rt == 13) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        addr = context->registers.r[rn] + immediate;
          context->registers.r[rt] = e4arduino_sign_extend(e4arduino_get_byte_from_memory(context, addr), 8);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;

    context->registers.r[PC] += 4;
}

void
ldrsb_imm_32_et2(void *ctx, word rn, word rt, word p, word u, word w, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsb_imm_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word p[0x%x], word u[0x%x], word w[0x%x], word imm8[0x%x])", ctx, rn, rt, p, u, w, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrsbt_32_et1(void *ctx, word rn, word rt, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsbt_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm8[0x%x])", ctx, rn, rt, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrsb_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsb_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, rn, rt, imm2, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
pld_lit_32_et1(void *ctx, word u, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pld_lit_32_et1(void *ctx[0x%p], word u[0x%x], word imm12[0x%x])", ctx, u, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
pld_imm_32_et1(void *ctx, word rn, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pld_imm_32_et1(void *ctx[0x%p], word rn[0x%x], word imm12[0x%x])", ctx, rn, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
pld_imm_32_et2(void *ctx, word rn, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pld_imm_32_et2(void *ctx[0x%p], word rn[0x%x], word imm8[0x%x])", ctx, rn, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
pld_reg_32_et1(void *ctx, word rn, word sft, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pld_reg_32_et1(void *ctx[0x%p], word rn[0x%x], word sft[0x%x], word rm[0x%x])", ctx, rn, sft, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
pli_imm_32_et1(void *ctx, word rn, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pli_imm_32_et1(void *ctx[0x%p], word rn[0x%x], word imm12[0x%x])", ctx, rn, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
pli_imm_32_et2(void *ctx, word rn, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pli_imm_32_et2(void *ctx[0x%p], word rn[0x%x], word imm8[0x%x])", ctx, rn, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
pli_lit_32_et3(void *ctx, word u, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pli_lit_32_et3(void *ctx[0x%p], word u[0x%x], word imm12[0x%x])", ctx, u, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
pli_reg_32_et1(void *ctx, word rn, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pli_reg_32_et1(void *ctx[0x%p], word rn[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, rn, imm2, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}


void
ldrh_lit_32_et1(void *ctx, word u, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrh_lit_32_et1(void *ctx[0x%p], word u[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, u, rt, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrh_imm_32_et2(void *ctx, word rn, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrh_imm_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, rn, rt, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrh_imm_32_et3(void *ctx, word rn, word rt, word p, word u, word w, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrh_imm_32_et3(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word p[0x%x], word u[0x%x], word w[0x%x], word imm8[0x%x])", ctx, rn, rt, p, u, w, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrh_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, addr, offset, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrh_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, rn, rt, imm2, rm);
    if(imm2 == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldrh%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldrh%s.w\t%s, %s, %s, lsl #%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm], imm2);
    }
    if((rt == 13) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        offset = e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, imm2, &carry);
        addr = context->registers.r[rn] + offset;
        value = e4arduino_get_halfword_from_memory(context, addr);
        context->registers.r[rt] = e4arduino_zero_extend(value, 32);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
ldrht_32_et1(void *ctx, word rn, word rt, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrht_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm8[0x%x])", ctx, rn, rt, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrsh_imm_32_et1(void *ctx, word rn, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsh_imm_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, rn, rt, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrsh_imm_32_et2(void *ctx, word rn, word rt, word p, word u, word w, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsh_imm_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word p[0x%x], word u[0x%x], word w[0x%x], word imm8[0x%x])", ctx, rn, rt, p, u, w, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrsh_lit_32_et1(void *ctx, word u, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsh_lit_32_et1(void *ctx[0x%p], word u[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, u, rt, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrsh_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsh_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, rn, rt, imm2, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldrsht_32_et1(void *ctx, word rn, word rt, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrsht_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm8[0x%x])", ctx, rn, rt, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldr_imm_32_et3(void *ctx, word rn, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word base, value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldr_imm_32_et3(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, rn, rt, imm12);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldr%s.w\t%s, [%s, #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], imm12);
    if((rt == 15) && e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        base = context->registers.r[rn] & 0xfffffffc;
        value = e4arduino_get_word_from_memory(context, base + e4arduino_zero_extend(imm12, 32) * 4);
        context->registers.r[rt] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
ldr_imm_32_et4(void *ctx, word rn, word rt, word p, word u, word w, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word addr, data, offset;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldr_imm_32_et4(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word p[0x%x], word u[0x%x], word w[0x%x], word imm8[0x%x])", ctx, rn, rt, p, u, w, imm8);
    if((p == 1) && (w == 0)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldr%s.w\t%s, [%s, #%s%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], (u != 0) ? "" : "-", imm8);
    } else if((p == 1) && (w == 1)) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldr%s.w\t%s, [%s, #%s%d]!", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], (u != 0) ? "" : "-", imm8);
    } else if((p == 0) && (w == 0)) {
        e4arduino_undefined(context);
    } else if(((w == 1) && (rn == rt)) || ((rt == 15) && e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context))) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        if(u != 0) {
            offset = context->registers.r[rn] + e4arduino_zero_extend(imm8, 32);
        } else {
            offset = context->registers.r[rn] - e4arduino_zero_extend(imm8, 32);
        }
        if(p != 0) {
            addr = offset;
        } else {
            addr = context->registers.r[rn];
        }
        data = e4arduino_get_word_from_memory(context, addr);
        if(w != 0) {
            context->registers.r[rn] = offset;
        }
        if(rt == 15) {
            if((addr & 0x00000003) == 0x00000000) {
                context->registers.r[rt] = addr;
            } else {
                e4arduino_unpredictable(context);
            }
            context->wait_until = context->cycles + 3;  /* from 3 to 5 cycles */

        } else {
            context->registers.r[rt] = data;
            context->wait_until = context->cycles + 2;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
pop_32_et3(void *ctx, word rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void pop_32_et3(void *ctx[0x%p], word rt[0x%x])", ctx, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tpop%s.w\t%s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt]);
    if((rt == 13) || ((rt == 15) && e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context))) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[rt] = e4arduino_pop_from_stack(context);
        if(rt == 15) {
            context->registers.r[rt] -= 4;
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;  /* 1 + nregs */
    context->registers.r[PC] += 4;
}

void
ldrt_32_et1(void *ctx, word rn, word rt, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldrt_32_et1(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm8[0x%x])", ctx, rn, rt, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ldr_reg_32_et2(void *ctx, word rn, word rt, word imm2, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word offset, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldr_reg_32_et2(void *ctx[0x%p], word rn[0x%x], word rt[0x%x], word imm2[0x%x], word rm[0x%x])", ctx, rn, rt, imm2, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldr%s.w\t%s, [%s, %s, lsl #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_register_name[rn], e4arduino_register_name[rm], imm2);
    carry = e4arduino_get_carry_flag(context);
    offset = context->registers.r[rn] + e4arduino_shift(context->registers.r[rm], E4ARDUINO_SHIFT_TYPE_LSL, imm2, &carry);
    if((rt == 15) && ((offset & 3) != 0)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        context->registers.r[rt] = e4arduino_get_word_from_memory(context, offset);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
ldr_lit_32_et2(void *ctx, word u, word rt, word imm12)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, immediate, base, addr;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ldr_lit_32_et2(void *ctx[0x%p], word u[0x%x], word rt[0x%x], word imm12[0x%x])", ctx, u, rt, imm12);
    immediate = e4arduino_zero_extend(imm12, 32);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tldr%s.w\t%s, [PC, #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], immediate);
    if((rt == 15) && e4arduino_in_it_block(context) && !e4arduino_in_last_it_block(context)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        base = (context->registers.r[PC] + 4) & 0xfffffffc;
        addr = (u != 0) ? base + immediate : base - immediate;
        value = e4arduino_get_word_from_memory(context, addr);
        context->registers.r[rt] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
lsl_reg_32_et2(void *ctx, word s, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, shift, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void lsl_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, s, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tlsl%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else {
        shift = context->registers.r[rm] & 0x000000ff;
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rn], E4ARDUINO_SHIFT_TYPE_LSL, shift, &carry);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
lsr_reg_32_et2(void *ctx, word s, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, shift, carry;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void lsr_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, s, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tlsr%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), !e4arduino_in_it_block(context) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        shift = context->registers.r[rm] & 0xff;
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rn], E4ARDUINO_SHIFT_TYPE_LSR, shift, &carry);
        context->registers.r[rd] = value;
        if(s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
asr_reg_32_et2(void *ctx, word s, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value, carry, shift_t;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void asr_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, s, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tasr%s%s.w\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), (s == 1) ? "s" : "", e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        carry = e4arduino_get_carry_flag(context);
        value = e4arduino_shift(context->registers.r[rn], E4ARDUINO_SHIFT_TYPE_ASR, context->registers.r[rm], &carry);
        context->registers.r[rd] = value;
        if (s == 1) {
            e4arduino_set_negative_flag(context, (value & (1 << 31)) >> 31);
            e4arduino_set_zero_flag(context, (value == 0));
            e4arduino_set_carry_flag(context, carry);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
ror_reg_32_et2(void *ctx, word s, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ror_reg_32_et2(void *ctx[0x%p], word s[0x%x], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, s, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sxtah_32_et1(void *ctx, word rn, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sxtah_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rn, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sxth_32_et2(void *ctx, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sxth_32_et2(void *ctx[0x%p], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uxtah_32_et1(void *ctx, word rn, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uxtah_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rn, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uxth_32_et2(void *ctx, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uxth_32_et2(void *ctx[0x%p], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sxtab16_32_et1(void *ctx, word rn, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sxtab16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rn, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sxtb16_32_et1(void *ctx, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sxtb16_32_et1(void *ctx[0x%p], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uxtab16_32_et1(void *ctx, word rn, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uxtab16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rn, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uxtb16_32_et1(void *ctx, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uxtb16_32_et1(void *ctx[0x%p], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sxtab_32_et1(void *ctx, word rn, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sxtab_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rn, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sxtb_32_et2(void *ctx, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sxtb_32_et2(void *ctx[0x%p], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uxtab_32_et1(void *ctx, word rn, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uxtab_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rn, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uxtb_32_et2(void *ctx, word rd, word rot, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uxtb_32_et2(void *ctx[0x%p], word rd[0x%x], word rot[0x%x], word rm[0x%x])", ctx, rd, rot, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sadd16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sadd16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sasx_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sasx_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ssax_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ssax_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ssub16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ssub16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sadd8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sadd8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
ssub8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void ssub8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qadd16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qadd16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qasx_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qasx_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qsax_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qsax_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qsub16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qsub16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qadd8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qadd8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qsub8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qsub8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
shadd16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void shadd16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uadd16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uadd16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uqadd16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uqadd16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uhadd16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uhadd16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
rev_32_et2(void *ctx, word rm, word rd, word rm2)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void rev_32_et2(void *ctx[0x%p], word rm[0x%x], word rd[0x%x], word rm2[0x%x])", ctx, rm, rd, rm2);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
rev16_32_et2(void *ctx, word rm, word rd, word rm2)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void rev16_32_et2(void *ctx[0x%p], word rm[0x%x], word rd[0x%x], word rm2[0x%x])", ctx, rm, rd, rm2);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
rbit_32_et1(void *ctx, word rm, word rd, word rm2)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void rbit_32_et1(void *ctx[0x%p], word rm[0x%x], word rd[0x%x], word rm2[0x%x])", ctx, rm, rd, rm2);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
revsh_32_et2(void *ctx, word rm, word rd, word rm2)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void revsh_32_et2(void *ctx[0x%p], word rm[0x%x], word rd[0x%x], word rm2[0x%x])", ctx, rm, rd, rm2);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
shasx_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void shasx_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uasx_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uasx_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uqasx_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uqasx_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uhasx_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uhasx_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
sel_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sel_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
clz_32_et1(void *ctx, word rm, word rd, word rm2)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    int i;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void clz_32_et1(void *ctx[0x%p], word rm[0x%x], word rd[0x%x], word rm2[0x%x])", ctx, rm, rd, rm2);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tclz%s\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rm]);
    if((rm != rm2) || (rd == 13) || (rd == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = 0;
        for(i = 0; i < 32; i++) {
            if((context->registers.r[rm] & (1 << (31 - i))) == 0) {
                value++;
            } else {
                break;
            }
        }
        context->registers.r[rd] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
shsax_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void shsax_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
usax_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void usax_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uqsax_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uqsax_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uhsax_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uhsax_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
shsub16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void shsub16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
usub16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void usub16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uqsub16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uqsub16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uhsub16_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uhsub16_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
shadd8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void shadd8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uadd8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uadd8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uqadd8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uqadd8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uhadd8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uhadd8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qadd_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qadd_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qdadd_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qdadd_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qsub_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qsub_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
qdsub_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void qdsub_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
shsub8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void shsub8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
usub8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void usub8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uqsub8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uqsub8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
uhsub8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void uhsub8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mla_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mla_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
mul_32_et2(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    doubleword value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mul_32_et2(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tmul%s\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if((rd == 13) || (rd == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rn] * context->registers.r[rm];
        context->registers.r[rd] = value & 0xffffffff;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
mls_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void mls_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlabb_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlabb_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlabt_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlabt_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlatb_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlatb_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlatt_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlatt_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smulbb_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smulbb_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smulbt_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smulbt_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smultb_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smultb_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smultt_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smultt_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlad_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlad_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smladx_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smladx_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smuad_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smuad_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smuadx_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smuadx_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlawb_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlawb_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlawt_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlawt_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smulwb_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smulwb_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smulwt_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smulwt_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlsd_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlsd_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlsdx_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlsdx_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smusd_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smusd_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smusdx_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smusdx_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smmla_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smmla_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smmlar_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smmlar_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smmul_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smmul_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smmulr_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smmulr_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smmls_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smmls_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smmlsr_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smmlsr_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
usad8_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void usad8_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
usada8_32_et1(void *ctx, word rn, word ra, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void usada8_32_et1(void *ctx[0x%p], word rn[0x%x], word ra[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, ra, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smull_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    doubleword value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smull_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tsmul%s\t%s, %s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rdl], e4arduino_register_name[rdh], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if((rdl == 13) || (rdl == 15) || (rdh == 13) || (rdh == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(rdl == rdh) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = context->registers.r[rn] * context->registers.r[rm];
        context->registers.r[rdh] = (value & 0xffffffff00000000) >> 32;
        context->registers.r[rdl] = (value & 0x00000000ffffffff) >> 0;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
sdiv_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void sdiv_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
umull_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    doubleword value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void umull_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tumull%s\t%s, %s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rdl], e4arduino_register_name[rdh], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if((rdl == 13) || (rdl == 15) || (rdh == 13) || (rdh == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15) || (rdh == rdl)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        value = (doubleword)context->registers.r[rn] * (doubleword)context->registers.r[rm];
        context->registers.r[rdh] = value >> 32;
        context->registers.r[rdl] = value >> 0;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
udiv_32_et1(void *ctx, word rn, word rd, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void udiv_32_et1(void *ctx[0x%p], word rn[0x%x], word rd[0x%x], word rm[0x%x])", ctx, rn, rd, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tudiv%s\t%s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rd], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if((rd == 13) || (rn == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        if(context->registers.r[rm] == 0) {
            value = 0;
        } else {
            value = context->registers.r[rn] / context->registers.r[rm];
        }
        context->registers.r[rd] = value;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 8;  /* from 2 to 12 cycles */
    context->registers.r[PC] += 4;
}

void
smlal_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlal_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlalbb_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlalbb_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlalbt_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlalbt_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlaltb_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlaltb_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlaltt_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlaltt_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlald_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlald_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlaldx_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlaldx_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlsld_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlsld_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
smlsldx_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void smlsldx_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
umlal_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    doubleword result;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void umlal_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tumlal%s\t%s, %s, %s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rdl], e4arduino_register_name[rdh], e4arduino_register_name[rn], e4arduino_register_name[rm]);
    if((rdl == 13) || (rdl == 15) || (rdh == 13) || (rdh == 15) || (rn == 13) || (rn == 15) || (rm == 13) || (rm == 15) || (rdh == rdl)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        result = E4ARDUINO_UINT(context->registers.r[rn]) * E4ARDUINO_UINT(context->registers.r[rm]) + (((doubleword)context->registers.r[rdh] << 32) | context->registers.r[rdl]);
        context->registers.r[rdh] = (result & 0xffffffff00000000) >> 32;
        context->registers.r[rdl] = (result & 0x00000000ffffffff) >> 0;
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
umaal_32_et1(void *ctx, word rn, word rdl, word rdh, word rm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void umaal_32_et1(void *ctx[0x%p], word rn[0x%x], word rdl[0x%x], word rdh[0x%x], word rm[0x%x])", ctx, rn, rdl, rdh, rm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vsel_32_et1(void *ctx, word d, word cc, word vn, word vd, word sz, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vsel_32_et1(void *ctx[0x%p], word d[0x%x], word cc[0x%x], word vn[0x%x], word vd[0x%x], word sz[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, cc, vn, vd, sz, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmla_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmla_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmls_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmls_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vnmla_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vnmla_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vnmls_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vnmls_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vnmul_32_et2(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vnmul_32_et2(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmul_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word dest, op1, op2;
    e4arduino_data32 res, opr1, opr2;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmul_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    dest = (vd << 1) | d;
    op1 = (vn << 1) | n;
    op2 = (vm << 1) | m;
    opr1.ui32 = context->registers.neon_reg[op1];
    opr2.ui32 = context->registers.neon_reg[op2];
    res.fp32 = opr1.fp32 * opr2.fp32;
    context->registers.neon_reg[dest] = res.ui32;
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvmul%s.f32\ts%d, s%d, s%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), dest, op1, op2);
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
vadd_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word dest, op1, op2;
    e4arduino_data32 res, opr1, opr2;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vadd_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    dest = (vd << 1) | d;
    op1 = (vn << 1) | n;
    op2 = (vm << 1) | m;
    opr1.ui32 = context->registers.neon_reg[op1];
    opr2.ui32 = context->registers.neon_reg[op2];
    res.fp32 = opr1.fp32 + opr2.fp32;
    context->registers.neon_reg[dest] = res.ui32;
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvadd%s.f32\ts%d, s%d, s%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), dest, op1, op2);
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
vsub_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vsub_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vdiv_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vdiv_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmaxnm_32_et1(void *ctx, word d, word vn, word vd, word sz, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmaxnm_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word sz[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vminnm_32_et1(void *ctx, word d, word vn, word vd, word sz, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vminnm_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word sz[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmov_imm_32_et1(void *ctx, word d, word imm4h, word vd, word imm4l)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmov_imm_32_et1(void *ctx[0x%p], word d[0x%x], word imm4h[0x%x], word vd[0x%x], word imm4l[0x%x])", ctx, d, imm4h, vd, imm4l);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmov_reg_32_et1(void *ctx, word d, word vd, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word src, dest;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmov_reg_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, m, vm);
    src = (vm << 1) | m;
    dest = (vd << 1) | d;
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvmov%s.f32\ts%d, s%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), dest, src);
    context->registers.neon_reg[dest] = context->registers.neon_reg[src];
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
vabs_32_et1(void *ctx, word d, word vd, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vabs_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vneg_32_et1(void *ctx, word d, word vd, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vneg_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vsqrt_32_et1(void *ctx, word d, word vd, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vsqrt_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvtb_32_et1(void *ctx, word d, word op, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvtb_32_et2(void *ctx[0x%p], word d[0x%x], word op[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, op, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvtt_32_et1(void *ctx, word d, word op, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvtt_32_et2(void *ctx[0x%p], word d[0x%x], word op[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, op, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcmp_32_et1(void *ctx, word d, word vd, word sz, word e, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcmp_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word e[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, e, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcmpe_32_et1(void *ctx, word d, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcmpe_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcmp_32_et2(void *ctx, word d, word vd, word sz, word e)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcmp_32_et2(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word e[0x%x])", ctx, d, vd, sz, e);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcmpe_32_et2(void *ctx, word d, word vd, word sz)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcmpe_32_et2(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x])", ctx, d, vd, sz);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vrintx_32_et1(void *ctx, word d, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vrintx(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vrintz_32_et1(void *ctx, word d, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vrintz(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vrintr_32_et1(void *ctx, word d, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vrintr(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvt_dp_sp_32_et1(void *ctx, word d, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvt_dp_sp_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvt_fp_int_32_et1(void *ctx, word d, word opc2, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvt_fp_int_32_et1(void *ctx[0x%p], word d[0x%x], word opc2[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, opc2, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvtr_fp_int_32_et1(void *ctx, word d, word opc2, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word to_integer, dp_operation;
    e4arduino_data32 value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvtr_fp_int_32_et1(void *ctx[0x%p], word d[0x%x], word opc2[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, opc2, vd, sz, m, vm);
    to_integer = ((opc2 & 0x4) != 0);
    dp_operation = (sz != 0);
    if(to_integer != 0) {
        word dest;

        dest = (vd << 1) | d;
        if(dp_operation != 0) {
            word src;
            e4arduino_data64 value;

            src = (m << 4) | vm;
            value.ui32_h = context->registers.neon_reg[src + 1];
            value.ui32_l = context->registers.neon_reg[src + 0];
            context->registers.neon_reg[dest] = value.ui64;
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvcvtr.s32.f64%s\ts%d, d%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), dest, src);
        } else {
            word src;
            e4arduino_data32 value;

            src = (vm << 1) | m;
            value.ui32 = context->registers.neon_reg[src];
            context->registers.neon_reg[dest] = (u_int32_t)value.fp32;
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvcvtr.s32.f32%s\ts%d, s%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), dest, src);
        }
    } else {
        word src;

        src = (vm << 1) | m;
        if(dp_operation != 0) {
            word dest;
            e4arduino_data64 value;

            dest = (d << 4) | vd;
            value.fp64 = 1.0 * (((int64_t)context->registers.neon_reg[src + 1] << 32) | ((int64_t)context->registers.neon_reg[src + 0] << 0));
            context->registers.neon_reg[src + 1] = value.ui32_h;
            context->registers.neon_reg[src + 0] = value.ui32_l;
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvcvtr.f64.u64%s\td%d, d%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), dest, src);
        } else {
            word dest;
            e4arduino_data32 value;

            dest = (vd << 1) | d;
            value.fp32 = 1.0 * context->registers.neon_reg[src];
            context->registers.neon_reg[dest] = value.ui32;
            E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvcvtr.u32.f32%s\ts%d, s%d", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), dest, src);
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
vrinta_32_et1(void *ctx, word d, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vrinta_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vrintn_32_et1(void *ctx, word d, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vrintn_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vrintp_32_et1(void *ctx, word d, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vrintp_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vrintm_32_et1(void *ctx, word d, word vd, word sz, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vrintm_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvta_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvta_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word op[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, op, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvtn_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvtn_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word op[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, op, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvtp_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvtp_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word op[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, op, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvtm_32_et1(void *ctx, word d, word vd, word sz, word op, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvtm_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word sz[0x%x], word op[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vd, sz, op, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vcvt_fp_fix_32_et1(void *ctx, word d, word op, word u, word vd, word sf, word sx, word i, word imm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vcvt_fp_fix_32_et1(void *ctx[0x%p], word d[0x%x], word op[0x%x], word u[0x%x], word vd[0x%x], word sf[0x%x], word i[0x%x], word imm[0x%x])", ctx, d, op, u, vd, sf, sx, i, imm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmov_reg_sp_32_et1(void *ctx, word op, word vn, word rt, word n)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word neon_reg;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmov_reg_sp_32_et1(void *ctx[0x%p], word op[0x%x], word vn[0x%x], word rt[0x%x], word n[0x%x])", ctx, op, vn, rt, n);
    neon_reg = (vn << 1) | n;
    if(op == 1) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvmov%s\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rt], e4arduino_neon_register_name[neon_reg]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvmov%s\t%s, %s", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_neon_register_name[neon_reg], e4arduino_register_name[rt]);
    }
    if((rt == 13) || (rt == 15)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        if(op == 1) {
            context->registers.r[rt] = context->registers.neon_reg[neon_reg];
        } else {
            context->registers.neon_reg[neon_reg] = context->registers.r[rt];
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1;
    context->registers.r[PC] += 4;
}

void
vmsr_32_et1(void *ctx, word rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmsr_32_et1(void *ctx[0x%p], word rt[0x%x])", ctx, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmov_reg_scl_32_et1(void *ctx, word h, word vd, word rt, word d)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmov_reg_scl_32_et1(void *ctx[0x%p], word h[0x%x], word vd[0x%x], word rt[0x%x], word d[0x%x])", ctx, h, vd, rt, d);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vfma_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vfma_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vfms_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vfms_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vfnma_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vfnma_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vfnms_32_et1(void *ctx, word d, word vn, word vd, word n, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vfnms_32_et1(void *ctx[0x%p], word d[0x%x], word vn[0x%x], word vd[0x%x], word n[0x%x], word m[0x%x], word vm[0x%x])", ctx, d, vn, vd, n, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmrs_32_et1(void *ctx, word rt)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmrs_32_et1(void *ctx[0x%p], word rt[0x%x])", ctx, rt);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmov_scl_reg_32_et1(void *ctx, word h, word vn, word rt, word n)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmov_scl_reg_32_et1(void *ctx[0x%p], word h[0x%x], word vn[0x%x], word rt[0x%x], word n[0x%x])", ctx, h, vn, rt, n);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmov_reg_sp_sp_32_et1(void *ctx, word op, word rt2, word rt, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmov_reg_sp_sp_32_et1(void *ctx[0x%p], word op[0x%x], word rt2[0x%x], word rt[0x%x], word m[0x%x], word vm[0x%x])", ctx, op, rt2, rt, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vmov_reg_dp_32_et1(void *ctx, word op, word rt2, word rt, word m, word vm)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vmov_reg_dp_32_et1(void *ctx[0x%p], word op[0x%x], word rt2[0x%x], word rt[0x%x], word m[0x%x], word vm[0x%x])", ctx, op, rt2, rt, m, vm);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vstm_32_et1(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vstm_32_et1(void *ctx[0x%p], word p[0x%x], word u[0x%x], word d[0x%x], word w[0x%x], word rn[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, p, u, d, w, rn, vd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vstm_32_et2(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    int i;
    word imm32, addr, dest;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vstm_32_et2(void *ctx[0x%p], word p[0x%x], word u[0x%x], word d[0x%x], word w[0x%x], word rn[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, p, u, d, w, rn, vd, imm8);
    /*E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");*/

    /* Experimental Implement 20190218 */
    dest = (vd << 1) | d;
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t\tvstm%s.f32\t%s%s, {%s - %s}", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], (w != 0) ? "!" : "", e4arduino_neon_register_name[dest], e4arduino_neon_register_name[dest + imm8 - 1]);
    imm32 = e4arduino_zero_extend((imm8 << 2), 32);
    if((p == u) && (w == 1)) {
        e4arduino_undefined(context);
    } else if(rn == 15) {
        e4arduino_unpredictable(context);
    } else if((imm8 == 0) || ((dest + imm8) > (word)32)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        if ((p == 0) && (u == 1)) {
            /* PU == 01 IA */
            E4ARDUINO_PANIC("not implemented");
        } else if ((p == 1) && (u == 0) && (w == 1)) {
            /* PUW == 101 DB with ! */
            addr = context->registers.r[rn] - imm32;
            context->registers.r[rn] = addr;
            for(i = 0; i <= imm8 - 1; i++) {
                e4arduino_set_word_to_memory(context, addr, context->registers.neon_reg[dest + i]);
                addr += 4;
            }
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1 + imm8;  /* 1 + nregs */

    context->registers.r[PC] += 4;
}

void
vstr_32_et1(void *ctx, word u, word d, word rn, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vstr_32_et1(void *ctx[0x%p], word u[0x%x], word d[0x%x], word rn[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, u, d, rn, vd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vstr_32_et2(void *ctx, word u, word d, word rn, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word base, offset, dest, value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vstr_32_et2(void *ctx[0x%p], word u[0x%x], word d[0x%x], word rn[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, u, d, rn, vd, imm8);
    dest = (vd << 1) | d;
    if(u != 0) {
        offset = imm8 * 4;
    } else {
        offset = -imm8 * 4;
    }
    if(offset == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvstr%s\t%s, [%s]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_neon_register_name[dest], e4arduino_register_name[rn]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvstr%s\t%s, [%s, #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_neon_register_name[dest], e4arduino_register_name[rn], offset);
    }
    if(rn == 15) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        base = context->registers.r[rn];
    }
    e4arduino_set_word_to_memory(context, base + offset, context->registers.neon_reg[dest]);
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}

void
vpush_32_et1(void *ctx, word d, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vpush_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, d, vd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vpush_32_et2(void *ctx, word d, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vpush_32_et2(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, d, vd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vldm_32_et1(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vldm_32_et1(void *ctx[0x%p], word p[0x%x], word u[0x%x], word d[0x%x], word w[0x%x], word rn[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, p, u, d, w, rn, vd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vldm_32_et2(void *ctx, word p, word u, word d, word w, word rn, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    int i;
    word imm32, addr, dest;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vldm_32_et2(void *ctx[0x%p], word p[0x%x], word u[0x%x], word d[0x%x], word w[0x%x], word rn[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, p, u, d, w, rn, vd, imm8);
    /*E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");*/
    /* Experimental Implement 20190218 */
    dest = (vd << 1) | d;
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t\tvldm%s.f32\t%s%s, {%s - %s}", context->registers.r[PC], OP16((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_register_name[rn], (w != 0) ? "!" : "", e4arduino_neon_register_name[dest], e4arduino_neon_register_name[dest + imm8 - 1]);
    imm32 = e4arduino_zero_extend((imm8 << 2), 32);
    if((p == u) && (w == 1)) {
        e4arduino_undefined(context);
    } else if(rn == 15) {
        e4arduino_unpredictable(context);
    } else if((imm8 == 0) || ((dest + imm8) > (word)32)) {
        e4arduino_unpredictable(context);
    } else if(!e4arduino_in_it_block(context) || (e4arduino_in_it_block(context) && e4arduino_to_be_executed(context))) {
        if ((p == 0) && (u == 1)) {
            /* PU == 01 IA */
            addr = context->registers.r[rn];
            if (w != 0) {
                context->registers.r[rn] += imm32;
            }
            for(i = 0; i <= imm8 - 1; i++) {
                context->registers.neon_reg[dest + i] = e4arduino_get_word_from_memory(context, addr);
                addr += 4;
            }
        } else if ((p == 1) && (u == 0) && (w == 1)) {
            /* PUW == 101 DB with ! */
            E4ARDUINO_PANIC("not implemented");
        }
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 1 + imm8;  /* 1 + nregs */

    context->registers.r[PC] += 4;
}

void
vpop_32_et1(void *ctx, word d, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vpop_32_et1(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, d, vd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vpop_32_et2(void *ctx, word d, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vpop_32_et2(void *ctx[0x%p], word d[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, d, vd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vldr_32_et1(void *ctx, word u, word d, word rn, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vldr_32_et1(void *ctx[0x%p], word u[0x%x], word d[0x%x], word rn[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, u, d, rn, vd, imm8);
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\t", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]));
    E4ARDUINO_PANIC("not implemented");
    context->registers.r[PC] += 4;
}

void
vldr_32_et2(void *ctx, word u, word d, word rn, word vd, word imm8)
{
    e4arduino_context_t *context = (e4arduino_context_t *)ctx;
    word base, offset, dest, value;

    if(context->parsed != 0) {
        return;
    } else {
        context->parsed = 1;
    }
    E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_TRACE, "void vldr_32_et2(void *ctx[0x%p], word u[0x%x], word d[0x%x], word rn[0x%x], word vd[0x%x], word imm8[0x%x])", ctx, u, d, rn, vd, imm8);
    dest = (vd << 1) | d;
    if(u != 0) {
        offset = imm8;
    } else {
        offset = -imm8;
    }
    if(rn == 15) {
        base = (context->registers.r[PC] + 4) & 0xfffffffc;
    } else {
        base = context->registers.r[rn];
    }
    value = e4arduino_get_word_from_memory(context, base + offset);
    context->registers.neon_reg[dest] = value;
    if(offset == 0) {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvldr%s\t%s, [%s]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_neon_register_name[dest], e4arduino_register_name[rn]);
    } else {
        E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_INFO, "%08x: %08x\tvldr%s\t%s, [%s, #%d]", context->registers.r[PC], OP32((word *)&context->memory.memory[context->registers.r[PC]]), e4arduino_get_cond_string(context), e4arduino_neon_register_name[dest], e4arduino_register_name[rn], offset);
    }
    e4arduino_advance_itstate(context);
    context->wait_until = context->cycles + 2;
    context->registers.r[PC] += 4;
}
