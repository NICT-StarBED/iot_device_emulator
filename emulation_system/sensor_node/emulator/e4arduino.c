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

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#ifdef ENABLE_XFT
#include <dlfcn.h>
#endif /* ENABLE_XFT */
#include <fcntl.h>
#include <libgen.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>
#include "e4arduino_port.h"
#include "e4arduino_impl.h"
#include "e4arduino_log.h"
#include "e4arduino.h"

#if defined(__MACH__) && defined(__APPLE__)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif /*defined(__MACH__) && defined(__APPLE__)*/

#define E4ARDUINO_MEMORY_SPACE_SIZE        0xffffffff
#define E4ARDUINO_EMULATOR_FILES_PREFIX    "/var/tmp"
#define E4ARDUINO_SOCKET_BLOCKING           0
#define E4ARDUINO_SOCKET_NONBLOCKING        1

static double cycle_interval = NAN, next_cycle = NAN;

double
e4arduino_get_counter(void)
{

#if defined(__MACH__) && defined(__APPLE__)
    static double coef = INFINITY;
    if(isinf(coef)) {
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        coef = (double)info.numer / (double)info.denom / 1000000000.0;
    }
    return (double)mach_absolute_time() * coef;
#else /* !(defined(__MACH__) && defined(__APPLE__))*/
    struct timespec ts;
#if defined(CLOCK_MONOTONIC_PRECISE)
    clock_gettime(CLOCK_MONOTONIC_PRECISE, &ts);
#elif defined(CLOCK_MONOTONIC_RAW)
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#elif defined(CLOCK_REALTIME)
    clock_gettime(CLOCK_MONOTONIC_RAW, &ts);
#endif
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
#endif /*defined(__MACH__) && defined(__APPLE__)*/
    return NAN;
}

int
e4arduino_create_redirect_filo(char *path)
{
    int fd;

    unlink(path);
    if(mkfifo(path, 0644) < 0) {
        perror("mkfifo");
        return -1;
    }
    if((fd = open(path, O_RDWR | O_CREAT | O_NONBLOCK)) < 0) {
        perror("open");
        return -1;
    }
    return fd;
}

int
e4arduino_context_init(e4arduino_context_t *context)
{
    context->parsed = 0;
    context->cycles = 0;
    context->wait_until = 0;
    bzero(&context->registers, sizeof(context->registers));
    return 0;
}

void
e4arduino_context_deinit(e4arduino_context_t *context)
{
}

void
e4arduino_init_clock(e4arduino_context_t *context, double freq)
{
    if(!isnan(freq)) {
        cycle_interval = .000001 / freq;
        next_cycle = e4arduino_get_counter() + cycle_interval;
    }
}

void
e4arduino_wait_next_clock(e4arduino_context_t *context)
{

    if(!isnan(cycle_interval)) {
        while(e4arduino_get_counter() < next_cycle) {
            usleep(5);
        }
        next_cycle += cycle_interval;
    }
}

int
e4arduino_load_binary(e4arduino_memory_t *memory, char *mem_img, char *binary, int memimg_hidden)
{
    int i;
    char buf[BUFSIZ];
    byte data[BUFSIZ];
    FILE *fp;
    unsigned int length, address, type;
    unsigned int offset = 0;

    if((memory->fd = open(mem_img, O_RDWR | O_CREAT, 0600)) == -1) {
        perror("open");
        return -1;
    }
    if(memimg_hidden != 0) {
        unlink(mem_img);
    }
    if(ftruncate(memory->fd, E4ARDUINO_MEMORY_SPACE_SIZE) == -1) {
        perror("ftruncate");
        close(memory->fd);
        return -1;
    }
    if((memory->memory = mmap(NULL, E4ARDUINO_MEMORY_SPACE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, memory->fd, 0)) == NULL) {
        perror("mmap");
        close(memory->fd);
        return -1;
    }
    if((fp = fopen(binary, "r")) == NULL) {
        perror("fopen");
        munmap(memory->memory, E4ARDUINO_MEMORY_SPACE_SIZE);
        close(memory->fd);
        return -1;
    }
    while(fgets(buf, BUFSIZ, fp) != NULL) {
        if(buf[0] != ':') {
            fprintf(stderr, "failed to read %s\n", binary);
            munmap(memory->memory, E4ARDUINO_MEMORY_SPACE_SIZE);
            close(memory->fd);
            return -1;
        }
        if(sscanf(&buf[1], "%02x", &length) != 1) {
            fprintf(stderr, "failed to read %s\n", binary);
            munmap(memory->memory, E4ARDUINO_MEMORY_SPACE_SIZE);
            close(memory->fd);
            return -1;
        }
        if(sscanf(&buf[3], "%04x", &address) != 1) {
            fprintf(stderr, "failed to read %s\n", binary);
            munmap(memory->memory, E4ARDUINO_MEMORY_SPACE_SIZE);
            close(memory->fd);
            return -1;
        }
        if(sscanf(&buf[7], "%02x", &type) != 1) {
            fprintf(stderr, "failed to read %s\n", binary);
            munmap(memory->memory, E4ARDUINO_MEMORY_SPACE_SIZE);
            close(memory->fd);
            return -1;
        }
        for(i = 0; i < length; i++) {
            if(sscanf(&buf[i * 2 + 9], "%02hhx", (byte *)&data[i]) != 1) {
                fprintf(stderr, "failed to read %s\n", binary);
                munmap(memory->memory, E4ARDUINO_MEMORY_SPACE_SIZE);
                close(memory->fd);
                return -1;
            }
        }
        switch(type) {
        case 0x00:  /* Data */
            memcpy(&memory->memory[address + offset], data, length);
if(0)   {
            unsigned int start, end, i, j;
            start = (address + offset) / 16 * 16;
            end = (address + offset + length + 15) / 16 * 16;
            for(i = start; i < end; i += 16) {
                e4arduino_log_string(E4ARDUINO_LOGLEVEL_INFO, "%08x:", i);
                for(j = 0; j < 16; j++) {
                    e4arduino_log_string(E4ARDUINO_LOGLEVEL_INFO, " %02x", memory->memory[i + j]);
                }
                e4arduino_log_string(E4ARDUINO_LOGLEVEL_INFO, "\n");
            }
        }
            break;
        case 0x01:  /* End of file  */
            /* Do nothing   */
            break;
        case 0x02:  /* Offset address   */
            offset = (data[0] << 8 | data[1]) << 4;
            break;
        case 0x03:  /* Start address    */
            /* Do nothing   */
            break;
        }
    }
    if(ferror(fp) != 0) {
        fprintf(stderr, "failed to read %s\n", binary);
        munmap(memory->memory, E4ARDUINO_MEMORY_SPACE_SIZE);
        close(memory->fd);
        return -1;
    }
    return 0;
}

void
e4arduino_release_binary(e4arduino_memory_t *memory)
{

    munmap(memory->memory, E4ARDUINO_MEMORY_SPACE_SIZE);
    close(memory->fd);
}

void
e4arduino_reset(e4arduino_context_t *context)
{
    word *op;

    context->parsed = 0;
    context->cycles = 0;
    context->wait_until = 0;
    context->wfi = 0;
    context->wfe = 0;
    context->privilege_mode = E4ARDUINO_PRIVILEGE_MODE_UNPRIVILEGED;
    context->execution_mode = E4ARDUINO_EXECUTION_MODE_THREAD;
    bzero(&context->registers, sizeof(e4arduino_register_t));
    bzero(&context->special_registers, sizeof(e4arduino_special_register_t));
    bzero(&context->exception_active, sizeof(word) * 8);
    context->registers.r = ((context->special_registers.r[CONTROL] & E4ARDUINO_CONTROL_SPSEL) == 0) ? context->registers.bank0 : context->registers.bank1;
    op = (word *)&context->memory.memory[0x00000000];
    context->registers.r[SP] = *op;
    op = (word *)&context->memory.memory[0x00000004];
    context->registers.r[PC] = *op & 0xfffffffe;
    context->registers.bank0[LR] = 0xffffffff;
    context->registers.bank1[LR] = 0xffffffff;
    bzero(&context->memory.memory[0xe000e000], 0x00001fff);
    *(word *)&context->memory.memory[E4ARDUINO_SCB_CPUID] = 0x41ffffff;
    *(word *)&context->memory.memory[E4ARDUINO_SCB_MVFR0] = 0x10110021;
    *(word *)&context->memory.memory[E4ARDUINO_SCB_MVFR1] = 0x11000011;
}

void
usage(char *argv0)
{

#ifdef ENABLE_XFT
    fprintf(stderr, "Usage: %s [-u] [-c cycles_to_execute] [-d log_level (1:quiet - %d:verbose)] [-f freq (MHz)] binary.hex [-e external_function.so]\n", argv0, E4ARDUINO_NUMBER_OF_LOGLEVELS);
#else /* !ENABLE_XFT */
    fprintf(stderr, "Usage: %s [-u] [-c cycles_to_execute] [-d log_level (1:quiet - %d:verbose)] [-f freq (MHz)] binary.hex\n", argv0, E4ARDUINO_NUMBER_OF_LOGLEVELS);
#endif /* ENABLE_XFT */
}

int
main(int argc, char *argv[])
{
    e4arduino_context_t context;
    word *op;
    int opt, cycles_to_execute = -1, loglevel = E4ARDUINO_LOGLEVEL_INFO;
    double freq = NAN;
    char memimg_path[BUFSIZ];
    int memimg_hidden = 1;

#ifdef ENABLE_XFT
    while ((opt = getopt(argc, argv, "huc:d:f:e:")) != -1) {
#else /* !ENABLE_XFT */
    while ((opt = getopt(argc, argv, "huc:d:f:")) != -1) {
#endif /* ENABLE_XFT */
        switch (opt) {
        case 'c':
            if((sscanf(optarg, "%d", &cycles_to_execute) != 1) || (cycles_to_execute <= 0)) {
                fprintf(stderr, "execution cycles needs to be grater than 0\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 'd':
            if((sscanf(optarg, "%d", &loglevel) != 1) || ((loglevel < 1) || (loglevel > E4ARDUINO_NUMBER_OF_LOGLEVELS))) {
                fprintf(stderr, "%d: undefined log level (1:quiet - %d:verbose)\n", loglevel, E4ARDUINO_NUMBER_OF_LOGLEVELS);
                exit(EXIT_FAILURE);
            }
            loglevel = E4ARDUINO_NUMBER_OF_LOGLEVELS - loglevel;
            break;
#ifdef ENABLE_XFT
        case 'e':
            context.xft_library_path = optarg;
            break;
#endif /* ENABLE_XFT */
        case 'f':
            if((sscanf(optarg, "%lf", &freq) != 1) || ((freq < 0) || (freq >= 400.0))) {
                fprintf(stderr, "%f: invalid processor clock frequency (0 - 400)\n", freq);
                exit(EXIT_FAILURE);
            }
            break;
        case 'u':
            memimg_hidden = 0;
            break;
        case 'h':
        default:
            usage(argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    if(optind != argc - 1) {
        usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    e4arduino_log_init();
    e4arduino_log_set_loglevel(loglevel);
    e4arduino_log_set_output(stdout);
    if(e4arduino_context_init(&context) < 0) {
        fprintf(stderr, "failed to initialize emultor context.\n");
        exit(EXIT_FAILURE);
    }
    snprintf(memimg_path, BUFSIZ, "%s/%s_%d.img", E4ARDUINO_EMULATOR_FILES_PREFIX, basename(argv[0]), getpid());
    if(e4arduino_load_binary(&context.memory, memimg_path, argv[optind], memimg_hidden) == -1) {
        fprintf(stderr, "failed to load binary %s.\n", argv[optind]);
        exit(EXIT_FAILURE);
    }
#ifdef ENABLE_XFT
    {
        void *xft_library_handle;

        context.xft_setup = NULL;
        context.xft_dispatch = NULL;
        context.xft_teardown = NULL;
        if((xft_library_handle = dlopen(context.xft_library_path, RTLD_NOW)) == NULL) {
            perror(dlerror());
            exit(EXIT_FAILURE);
        } else {
            if((context.xft_setup = dlsym(xft_library_handle, "xft_setup")) == NULL) {
                perror(dlerror());
                exit(EXIT_FAILURE);
            }
            if(context.xft_setup(&context) != 0) {
                E4ARDUINO_LOG_LINE(E4ARDUINO_LOGLEVEL_ERROR, "xft_setup returned non-zero status.");
                exit(EXIT_FAILURE);
            }
            if((context.xft_dispatch = dlsym(xft_library_handle, "xft_dispatch")) == NULL) {
                perror(dlerror());
                exit(EXIT_FAILURE);
            }
            if((context.xft_teardown = dlsym(xft_library_handle, "xft_teardown")) == NULL) {
                perror(dlerror());
                exit(EXIT_FAILURE);
            }
        }
    }
#endif /* ENABLE_XFT */
    e4arduino_reset(&context);
    e4arduino_init_clock(&context, freq);
    for(;;) {
        context.parsed = 0;
        e4arduino_handle_interrupt_pre_cycle(&context);
        if((context.cycles >= context.wait_until) && (context.wfi == 0) && (context.wfe == 0)) {
            context.registers.r[PC] &= 0xfffffffe;
            op = (word *)&context.memory.memory[context.registers.r[PC]];
            e4arduino_dump_registers(&context, E4ARDUINO_LOGLEVEL_DEBUG);
            PROCESS_SINGLE_INSTRUCTION(&context, op);
            if(context.parsed == 0) {
                fprintf(stderr, "failed to parse byte code %08x at %08x\n", *(word *)op, context.registers.r[PC]);
                abort();
            }
        }
        context.cycles++;
#ifndef E4ARDUINO_NOWAIT
        e4arduino_wait_next_clock(&context);
#endif /* E4ARDUINO_NOWAIT */
        e4arduino_handle_interrupt_post_cycle(&context);
        if(cycles_to_execute > 0) {
            if(--cycles_to_execute == 0) {
                break;
            }
        }
    }
#ifdef ENABLE_XFT
    context.xft_teardown();
#endif /* ENABLE_XFT */
    e4arduino_release_binary(&context.memory);
    e4arduino_context_deinit(&context);
    return 0;
}
