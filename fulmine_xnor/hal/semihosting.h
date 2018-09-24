/* 
 * Copyright (C) 2015 ETH Zurich and University of Bologna
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license.  See the LICENSE file for details.
 *
 */

#ifndef ARCHI_SEMIHOSTING_H
#define ARCHI_SEMIHOSTING_H

#if defined(__riscv__) || defined(RISCV)
#define PLP_SEMIHOSTING_CUSTOM_START 16
#else
#define PLP_SEMIHOSTING_CUSTOM_START 0x1000
#endif

#define PLP_SEMIHOSTING_VCD_CONFIGURE            10
#define PLP_SEMIHOSTING_VCD_CONF_TRACE           11
#define PLP_SEMIHOSTING_VCD_DECLARE_TRACE        12
#define PLP_SEMIHOSTING_VCD_DECLARE_TRACE_STRING 13
#define PLP_SEMIHOSTING_VCD_DUMP_TRACE           14
#define PLP_SEMIHOSTING_VCD_DUMP_TRACE_STRING    15


#define PLP_SEMIHOSTING_STATS_START              20
#define PLP_SEMIHOSTING_STATS_STOP               21
#define PLP_SEMIHOSTING_STATS_DUMP               22

#define PLP_SEMIHOSTING_TRACE_START              30
#define PLP_SEMIHOSTING_TRACE_STOP               31

#define PLP_SEMIHOSTING_PROF_START              40
#define PLP_SEMIHOSTING_PROF_STOP               41

#endif
