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

#ifndef INCLUDE_E4K64F_LOG_H
#define INCLUDE_E4K64F_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "e4k64f_impl.h"

#define E4K64F_LOG_HEADER(lv)                                        \
            do {                                                        \
                e4k64f_log_string((lv), "%s()@%s:%d [%s] ",          \
                                  __FUNCTION__, __FILE__, __LINE__,     \
                                  e4k64f_loglevel_name[(lv)]);       \
            } while(0)

#define E4K64F_LOG_LINE(lv, ...)                                     \
            do {                                                        \
                E4K64F_LOG_HEADER((lv));                             \
                e4k64f_log_string((lv), __VA_ARGS__);                \
                e4k64f_log_string((lv), "\n");                       \
            } while(0)

#ifdef E4K64F_KEEP_GOING_ON_PANIC
#define E4K64F_PANIC(reason)                                         \
            do {                                                        \
                static char buf[BUFSIZ];                                \
                E4K64F_LOG_HEADER(E4K64F_LOGLEVEL_PANIC);         \
                e4k64f_log_string(E4K64F_LOGLEVEL_PANIC, "%s\n",  \
                                     (reason));                         \
                printf("Continue? [y/N]\n");                            \
                fgets(buf, BUFSIZ, stdin);                              \
                if((buf[0] != 'y') && (buf[0] != 'Y')) {                \
                    exit(EXIT_FAILURE);                                 \
                }                                                       \
            } while(0)
#else /* !E4K64F_KEEP_GOING_ON_PANIC */
#define E4K64F_PANIC(reason)                                         \
            do {                                                        \
                E4K64F_LOG_HEADER(E4K64F_LOGLEVEL_PANIC);         \
                e4k64f_log_string(E4K64F_LOGLEVEL_PANIC, "%s\n",  \
                                     (reason));                         \
                exit(EXIT_FAILURE);                                     \
            } while(0)
#endif /* E4K64F_KEEP_GOING_ON_PANIC */

typedef enum {
    E4K64F_LOGLEVEL_TRACE,
    E4K64F_LOGLEVEL_DEBUG,
    E4K64F_LOGLEVEL_INFO,
    E4K64F_LOGLEVEL_WARN,
    E4K64F_LOGLEVEL_ERROR,
    E4K64F_LOGLEVEL_FATAL,
    E4K64F_LOGLEVEL_PANIC,
    E4K64F_NUMBER_OF_LOGLEVELS
} e4k64f_loglevel_t;

static char *e4k64f_loglevel_name[E4K64F_NUMBER_OF_LOGLEVELS] = {
    "TRACE", "DEBUG", "INFO", "WARN", "ERROR", "FATAL", "PANIC"
};

void e4k64f_log_init();
void e4k64f_log_set_loglevel(e4k64f_loglevel_t loglevel);
void e4k64f_log_set_output(FILE *output);
void e4k64f_log_string(e4k64f_loglevel_t level, char *format, ...);

void e4k64f_dump_registers(e4k64f_context_t *context, e4k64f_loglevel_t lv);
void e4k64f_dump_memory(e4k64f_context_t *context, e4k64f_loglevel_t lv, word start, word end);

#endif /* INCLUDE_E4K64F_LOG_H */
