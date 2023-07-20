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
#include <stdarg.h>

#include "e4k64f_log.h"

static e4k64f_loglevel_t level = E4K64F_LOGLEVEL_INFO;
static FILE *fp;

void
e4k64f_log_init()
{

    level = E4K64F_LOGLEVEL_INFO;
    fp = fdopen(2, "w");
}

void
e4k64f_log_set_loglevel(e4k64f_loglevel_t loglevel)
{

    level = loglevel;
}

void
e4k64f_log_set_output(FILE *output)
{

    fp = output;
}

void
e4k64f_log_string(e4k64f_loglevel_t lv, char *format, ...)
{
    va_list list;

    if(lv >= level) {
        va_start(list, format);
        vfprintf(fp, format, list);
        fflush(fp);
        va_end(list);
    }
}

void
e4k64f_dump_registers(e4k64f_context_t *context, e4k64f_loglevel_t lv)
{
    int i;
    static word history[E4K64F_NUMBER_OF_REGISTERS];
    static word history_msp;
    static word history_psp;

    e4k64f_log_string(lv, "Cycles: %10d, N: %1d, Z: %1d, C: %1d, V: %1d, ITSTATE: 0x%02x, InItBlock: %1d, InLastItBlock: %1d\n", context->cycles, (context->registers.r[CPSR] & (1 << 31)) >> 31, (context->registers.r[CPSR] & (1 << 30)) >> 30, (context->registers.r[CPSR] & (1 << 29)) >> 29, (context->registers.r[CPSR] & (1 << 28)) >> 28, e4k64f_get_it_state(context), e4k64f_in_it_block(context), e4k64f_in_last_it_block(context));
    for(i = 0; i < E4K64F_NUMBER_OF_REGISTERS; i++) {
#ifndef DISABLE_REGISTER_DUMP_WITH_COLOR
        if(history[i] != context->registers.r[i]){
            e4k64f_log_string(lv, "[43m%*s[%08x][0m ", 4, e4k64f_register_name[i], context->registers.r[i]);
        } else {
            e4k64f_log_string(lv, "[0m%*s[%08x][0m ", 4, e4k64f_register_name[i], context->registers.r[i]);
        }
#else /* !DISABLE_REGISTER_DUMP_WITH_COLOR */
        e4k64f_log_string(lv, "%*s[%08x] ", 4, e4k64f_register_name[i], context->registers.r[i]);
#endif /* DISABLE_REGISTER_DUMP_WITH_COLOR */
        if((i % 8) == 7) {
            e4k64f_log_string(lv, "\n");
        }
    }
    e4k64f_log_string(lv, "\n");
#ifndef DISABLE_REGISTER_DUMP_WITH_COLOR
    if(history_msp != context->special_registers.r[MSP]){
        e4k64f_log_string(lv, "[43m msp[%08x][0m ", context->special_registers.r[MSP]);
    } else {
        e4k64f_log_string(lv, "[0m msp[%08x][0m ", context->special_registers.r[MSP]);
    }
    if(history_psp != context->special_registers.r[PSP]){
        e4k64f_log_string(lv, "[43m psp[%08x][0m ", context->special_registers.r[PSP]);
    } else {
        e4k64f_log_string(lv, "[0m psp[%08x][0m ", context->special_registers.r[PSP]);
    }
#else /* !DISABLE_REGISTER_DUMP_WITH_COLOR */
    e4k64f_log_string(lv, " msp[%08x] ", context->special_registers.r[MSP]);
    e4k64f_log_string(lv, " psp[%08x]", context->special_registers.r[PSP]);
#endif /* DISABLE_REGISTER_DUMP_WITH_COLOR */
    e4k64f_log_string(lv, "\n");
    for(i = 0; i < E4K64F_NUMBER_OF_REGISTERS; i++) {
        history[i] = context->registers.r[i];
    }
    history_msp = context->special_registers.r[MSP];
    history_psp = context->special_registers.r[PSP];
}

void
e4k64f_dump_memory(e4k64f_context_t *context, e4k64f_loglevel_t lv, word start, word end)
{
    int i, j;
    e4k64f_memory_t *memory = &context->memory;

    for(i = start & 0xfffffff0; i < end; i += 16) {
        e4k64f_log_string(lv, "%08x:", i);
        for(j = 0; j < 16; j++) {
            if(((i + j) >= start) && ((i + j) <= end)) {
                e4k64f_log_string(lv, " %02x", *(byte *)&memory->memory[i + j]);
            } else {
                e4k64f_log_string(lv, " --");
            }
        }
        e4k64f_log_string(lv, "\n");
    }
}
