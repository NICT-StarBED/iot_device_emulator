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

#ifndef INCLUDED_E4ARDUINO_PORT_H
#define INCLUDED_E4ARDUINO_PORT_H

#define E4ARDUINO_SYSCON_SYSMEMREMAP    0x40048000
#define E4ARDUINO_SYSCON_PRESETCTRL     0x40048004
#define E4ARDUINO_SYSCON_SYSPLLCTRL     0x40048008
#define E4ARDUINO_SYSCON_SYSPLLSTAT     0x4004800c
#define E4ARDUINO_SYSCON_SYSOSCCTRL     0x40048020
#define E4ARDUINO_SYSCON_WDTOSCCTRL     0x40048024
#define E4ARDUINO_SYSCON_IRCCTRL        0x40048028
#define E4ARDUINO_SYSCON_SYSRSTSTAT     0x40048030
#define E4ARDUINO_SYSCON_SYSPLLCLKSEL   0x40048040
#define E4ARDUINO_SYSCON_SYSPLLCLKUEN   0x40048044
#define E4ARDUINO_SYSCON_MAINCLKSEL     0x40048070
#define E4ARDUINO_SYSCON_MAINCLKUEN     0x40048074
#define E4ARDUINO_SYSCON_SYSAHBCLKDIV   0x40048078
#define E4ARDUINO_SYSCON_SYSAHBCLKCTRL  0x40048080
#define E4ARDUINO_SYSCON_UARTCLKDIV     0x40048094
#define E4ARDUINO_SYSCON_CLKOUTSEL      0x400480e0
#define E4ARDUINO_SYSCON_CLKOUTUEN      0x400480e4
#define E4ARDUINO_SYSCON_CLKOUTDIV      0x400480e8
#define E4ARDUINO_SYSCON_UARTFRGDIV     0x400480f0
#define E4ARDUINO_SYSCON_UARTFRGMULT    0x400480f4
#define E4ARDUINO_SYSCON_EXTTRACECMD    0x400480fc
#define E4ARDUINO_SYSCON_PIOPORCAP0     0x40048100
#define E4ARDUINO_SYSCON_IOCONCLKDIV0   0x40048134
#define E4ARDUINO_SYSCON_IOCONCLKDIV1   0x40048138
#define E4ARDUINO_SYSCON_IOCONCLKDIV2   0x4004813c
#define E4ARDUINO_SYSCON_IOCONCLKDIV3   0x40048140
#define E4ARDUINO_SYSCON_IOCONCLKDIV4   0x40048144
#define E4ARDUINO_SYSCON_IOCONCLKDIV5   0x40048148
#define E4ARDUINO_SYSCON_IOCONCLKDIV6   0x4004814c
#define E4ARDUINO_SYSCON_BODCTRL        0x40048150
#define E4ARDUINO_SYSCON_SYSTCKCAL      0x40048154
#define E4ARDUINO_SYSCON_IRQLATENCY     0x40048170
#define E4ARDUINO_SYSCON_NMISRC         0x40048174
#define E4ARDUINO_SYSCON_PINTSEL0       0x40048178
#define E4ARDUINO_SYSCON_PINTSEL1       0x4004817c
#define E4ARDUINO_SYSCON_PINTSEL2       0x40048180
#define E4ARDUINO_SYSCON_PINTSEL3       0x40048184
#define E4ARDUINO_SYSCON_PINTSEL4       0x40048188
#define E4ARDUINO_SYSCON_PINTSEL5       0x4004818c
#define E4ARDUINO_SYSCON_PINTSEL6       0x40048190
#define E4ARDUINO_SYSCON_PINTSEL7       0x40048194
#define E4ARDUINO_SYSCON_STARTERP0      0x40048204
#define E4ARDUINO_SYSCON_STARTERP1      0x40048214
#define E4ARDUINO_SYSCON_PDSLEEPCFG     0x40048230
#define E4ARDUINO_SYSCON_PDAWAKECFG     0x40048234
#define E4ARDUINO_SYSCON_PDRUNCFG       0x40048238
#define E4ARDUINO_SYSCON_DEVICEID       0x400483f8

#define E4ARDUINO_FTM3_SC               0x400b9000
#define E4ARDUINO_FTM3_SC_CLKS          0x00000018
#define E4ARDUINO_FTM3_MOD              0x400b9008
#define E4ARDUINO_FTM3_C0V              0x400b9010

#define E4ARDUINO_SCB_ICTR          0xe000e004
#define E4ARDUINO_SCB_ACTLR         0xe000e008
#define E4ARDUINO_SCB_STCSR         0xe000e010
#define E4ARDUINO_SCB_STRVR         0xe000e014
#define E4ARDUINO_SCB_STCVR         0xe000e018
#define E4ARDUINO_SCB_STCR          0xe000e01c
#define E4ARDUINO_SCB_ISER0         0xe000e100
#define E4ARDUINO_SCB_ISER1         0xe000e104
#define E4ARDUINO_SCB_ISER2         0xe000e108
#define E4ARDUINO_SCB_ISER3         0xe000e10c
#define E4ARDUINO_SCB_ISER4         0xe000e110
#define E4ARDUINO_SCB_ISER5         0xe000e114
#define E4ARDUINO_SCB_ISER6         0xe000e118
#define E4ARDUINO_SCB_ISER7         0xe000e11c
#define E4ARDUINO_SCB_ICER0         0xe000e180
#define E4ARDUINO_SCB_ICER1         0xe000e184
#define E4ARDUINO_SCB_ICER2         0xe000e188
#define E4ARDUINO_SCB_ICER3         0xe000e18c
#define E4ARDUINO_SCB_ICER4         0xe000e190
#define E4ARDUINO_SCB_ICER5         0xe000e194
#define E4ARDUINO_SCB_ICER6         0xe000e198
#define E4ARDUINO_SCB_ICER7         0xe000e19c
#define E4ARDUINO_SCB_ISPR0         0xe000e200
#define E4ARDUINO_SCB_ISPR1         0xe000e204
#define E4ARDUINO_SCB_ISPR2         0xe000e208
#define E4ARDUINO_SCB_ISPR3         0xe000e20c
#define E4ARDUINO_SCB_ISPR4         0xe000e210
#define E4ARDUINO_SCB_ISPR5         0xe000e214
#define E4ARDUINO_SCB_ISPR6         0xe000e218
#define E4ARDUINO_SCB_ISPR7         0xe000e21c
#define E4ARDUINO_SCB_ICPR0         0xe000e280
#define E4ARDUINO_SCB_ICPR1         0xe000e284
#define E4ARDUINO_SCB_ICPR2         0xe000e288
#define E4ARDUINO_SCB_ICPR3         0xe000e28c
#define E4ARDUINO_SCB_ICPR4         0xe000e290
#define E4ARDUINO_SCB_ICPR5         0xe000e294
#define E4ARDUINO_SCB_ICPR6         0xe000e298
#define E4ARDUINO_SCB_ICPR7         0xe000e29c
#define E4ARDUINO_SCB_IABR0         0xe000e300
#define E4ARDUINO_SCB_IABR1         0xe000e304
#define E4ARDUINO_SCB_IABR2         0xe000e308
#define E4ARDUINO_SCB_IABR3         0xe000e30c
#define E4ARDUINO_SCB_IABR4         0xe000e310
#define E4ARDUINO_SCB_IABR5         0xe000e314
#define E4ARDUINO_SCB_IABR6         0xe000e318
#define E4ARDUINO_SCB_IABR7         0xe000e31c
#define E4ARDUINO_SCB_IPR0          0xe000e400
#define E4ARDUINO_SCB_IPR1          0xe000e404
#define E4ARDUINO_SCB_IPR2          0xe000e408
#define E4ARDUINO_SCB_IPR3          0xe000e40c
#define E4ARDUINO_SCB_IPR4          0xe000e410
#define E4ARDUINO_SCB_IPR5          0xe000e414
#define E4ARDUINO_SCB_IPR6          0xe000e418
#define E4ARDUINO_SCB_IPR7          0xe000e41c
#define E4ARDUINO_SCB_IPR8          0xe000e420
#define E4ARDUINO_SCB_IPR9          0xe000e424
#define E4ARDUINO_SCB_IPR10         0xe000e428
#define E4ARDUINO_SCB_IPR11         0xe000e42c
#define E4ARDUINO_SCB_IPR12         0xe000e430
#define E4ARDUINO_SCB_IPR13         0xe000e434
#define E4ARDUINO_SCB_IPR14         0xe000e438
#define E4ARDUINO_SCB_IPR15         0xe000e43c
#define E4ARDUINO_SCB_IPR16         0xe000e440
#define E4ARDUINO_SCB_IPR17         0xe000e444
#define E4ARDUINO_SCB_IPR18         0xe000e448
#define E4ARDUINO_SCB_IPR19         0xe000e44c
#define E4ARDUINO_SCB_IPR20         0xe000e450
#define E4ARDUINO_SCB_IPR21         0xe000e454
#define E4ARDUINO_SCB_IPR22         0xe000e458
#define E4ARDUINO_SCB_IPR23         0xe000e45c
#define E4ARDUINO_SCB_IPR24         0xe000e460
#define E4ARDUINO_SCB_IPR25         0xe000e464
#define E4ARDUINO_SCB_IPR26         0xe000e468
#define E4ARDUINO_SCB_IPR27         0xe000e46c
#define E4ARDUINO_SCB_IPR28         0xe000e470
#define E4ARDUINO_SCB_IPR29         0xe000e474
#define E4ARDUINO_SCB_IPR30         0xe000e478
#define E4ARDUINO_SCB_IPR31         0xe000e47c
#define E4ARDUINO_SCB_IPR32         0xe000e480
#define E4ARDUINO_SCB_IPR33         0xe000e484
#define E4ARDUINO_SCB_IPR34         0xe000e488
#define E4ARDUINO_SCB_IPR35         0xe000e48c
#define E4ARDUINO_SCB_IPR36         0xe000e490
#define E4ARDUINO_SCB_IPR37         0xe000e494
#define E4ARDUINO_SCB_IPR38         0xe000e498
#define E4ARDUINO_SCB_IPR39         0xe000e49c
#define E4ARDUINO_SCB_IPR40         0xe000e4a0
#define E4ARDUINO_SCB_IPR41         0xe000e4a4
#define E4ARDUINO_SCB_IPR42         0xe000e4a8
#define E4ARDUINO_SCB_IPR43         0xe000e4ac
#define E4ARDUINO_SCB_IPR44         0xe000e4b0
#define E4ARDUINO_SCB_IPR45         0xe000e4b4
#define E4ARDUINO_SCB_IPR46         0xe000e4b8
#define E4ARDUINO_SCB_IPR47         0xe000e4bc
#define E4ARDUINO_SCB_IPR48         0xe000e4c0
#define E4ARDUINO_SCB_IPR49         0xe000e4c4
#define E4ARDUINO_SCB_IPR50         0xe000e4c8
#define E4ARDUINO_SCB_IPR51         0xe000e4cc
#define E4ARDUINO_SCB_IPR52         0xe000e4d0
#define E4ARDUINO_SCB_IPR53         0xe000e4d4
#define E4ARDUINO_SCB_IPR54         0xe000e4d8
#define E4ARDUINO_SCB_IPR55         0xe000e4dc
#define E4ARDUINO_SCB_IPR56         0xe000e4e0
#define E4ARDUINO_SCB_IPR57         0xe000e4e4
#define E4ARDUINO_SCB_IPR58         0xe000e4e8
#define E4ARDUINO_SCB_IPR59         0xe000e4ec
#define E4ARDUINO_SCB_CPUID         0xe000ed00
#define E4ARDUINO_SCB_ICSR          0xe000ed04
#define E4ARDUINO_SCB_VTOR          0xe000ed08
#define E4ARDUINO_SCB_AIRCR         0xe000ed0c
#define E4ARDUINO_SCB_SCR           0xe000ed10
#define E4ARDUINO_SCB_CCR           0xe000ed14
#define E4ARDUINO_SCB_SHPR1         0xe000ed18
#define E4ARDUINO_SCB_SHPR2         0xe000ed1c
#define E4ARDUINO_SCB_SHPR3         0xe000ed20
#define E4ARDUINO_SCB_SHCSR         0xe000ed24
#define E4ARDUINO_SCB_CFSR          0xe000ed28
#define E4ARDUINO_SCB_HFSR          0xe000ed2c
#define E4ARDUINO_SCB_DFSR          0xe000ed30
#define E4ARDUINO_SCB_MMFAR         0xe000ed34
#define E4ARDUINO_SCB_BFAR          0xe000ed38
#define E4ARDUINO_SCB_AFSR          0xe000ed3c
#define E4ARDUINO_SCB_ID_PFR0       0xe000ed40
#define E4ARDUINO_SCB_ID_PFR1       0xe000ed44
#define E4ARDUINO_SCB_ID_DFR0       0xe000ed48
#define E4ARDUINO_SCB_ID_AFR0       0xe000ed4c
#define E4ARDUINO_SCB_ID_MMFR0      0xe000ed50
#define E4ARDUINO_SCB_ID_MMFR1      0xe000ed54
#define E4ARDUINO_SCB_ID_MMFR2      0xe000ed58
#define E4ARDUINO_SCB_ID_MMFR3      0xe000ed5c
#define E4ARDUINO_SCB_ID_ISAR0      0xe000ed60
#define E4ARDUINO_SCB_ID_ISAR1      0xe000ed64
#define E4ARDUINO_SCB_ID_ISAR2      0xe000ed68
#define E4ARDUINO_SCB_ID_ISAR3      0xe000ed6c
#define E4ARDUINO_SCB_ID_ISAR4      0xe000ed70
#define E4ARDUINO_SCB_CPACR         0xe000ed88
#define E4ARDUINO_SCB_STIR          0xe000ef00
#define E4ARDUINO_SCB_FPCCR         0xe000ef34
#define E4ARDUINO_SCB_FPCAR         0xe000ef38
#define E4ARDUINO_SCB_FPDSCR        0xe000ef3c
#define E4ARDUINO_SCB_MVFR0         0xe000ef40
#define E4ARDUINO_SCB_MVFR1         0xe000ef44
#define E4ARDUINO_SCB_PID4          0xe000efd0
#define E4ARDUINO_SCB_PID5          0xe000efd4
#define E4ARDUINO_SCB_PID6          0xe000efd8
#define E4ARDUINO_SCB_PID7          0xe000efdc
#define E4ARDUINO_SCB_PID0          0xe000efe0
#define E4ARDUINO_SCB_PID1          0xe000efe4
#define E4ARDUINO_SCB_PID2          0xe000efe8
#define E4ARDUINO_SCB_PID3          0xe000efec
#define E4ARDUINO_SCB_CID0          0xe000eff0
#define E4ARDUINO_SCB_CID1          0xe000eff4
#define E4ARDUINO_SCB_CID2          0xe000eff8
#define E4ARDUINO_SCB_CID3          0xe000effc

#define E4ARDUINO_STCSR_SYSTICK    ((1 <<  0))
#define E4ARDUINO_STCSR_TICKINT    ((1 <<  1))
#define E4ARDUINO_STCSR_CLKSOURCE  ((1 <<  2))
#define E4ARDUINO_STCSR_COUNTFLAG  ((1 << 16))

#endif /* INCLUDED_E4ARDUINO_PORT_H */
