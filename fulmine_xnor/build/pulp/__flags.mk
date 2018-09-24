PULP_LDFLAGS      += 
PULP_CFLAGS       += 
PULP_CL_ARCH_CFLAGS ?=  -march=rv32imfdcxpulpv2 -D__riscv__
PULP_CL_CFLAGS    += -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections -I/home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/include/io -I/home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/include -include /home/msc18f9/Documents/convNet_on_PULP/api/build/pulp/cl_config.h
PULP_CL_OMP_CFLAGS    += -fopenmp -I/home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/include/libgomp -I/home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/include/libgomp/pulp
ifdef PULP_RISCV_GCC_TOOLCHAIN
PULP_CL_CC = $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc 
PULP_CC = $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc 
PULP_AR ?= $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-ar
PULP_LD ?= $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
PULP_CL_OBJDUMP ?= $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-objdump
PULP_OBJDUMP ?= $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-objdump
else
PULP_CL_CC = $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-gcc 
PULP_CC = $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-gcc 
PULP_AR ?= $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-ar
PULP_LD ?= $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-gcc
PULP_CL_OBJDUMP ?= $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-objdump
PULP_OBJDUMP ?= $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-objdump
endif
PULP_ARCH_CL_OBJDFLAGS ?= -Mmarch=rv32imfdcxpulpv2
PULP_ARCH_OBJDFLAGS ?= -Mmarch=rv32imfdcxpulpv2
PULP_FC_ARCH_CFLAGS ?=  -march=rv32imcxpulpv2 -D__riscv__
PULP_FC_CFLAGS    += -Wextra -Wall -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wundef -fdata-sections -ffunction-sections -I/home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/include/io -I/home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/include -include /home/msc18f9/Documents/convNet_on_PULP/api/build/pulp/fc_config.h
PULP_FC_OMP_CFLAGS    += -fopenmp -I/home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/include/libgomp -I/home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/include/libgomp/pulp
ifdef PULP_RISCV_GCC_TOOLCHAIN
PULP_FC_CC = $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc 
PULP_CC = $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc 
PULP_AR ?= $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-ar
PULP_LD ?= $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-gcc
PULP_FC_OBJDUMP ?= $(PULP_RISCV_GCC_TOOLCHAIN)/bin/riscv32-unknown-elf-objdump
else
PULP_FC_CC = $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-gcc 
PULP_CC = $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-gcc 
PULP_AR ?= $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-ar
PULP_LD ?= $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-gcc
PULP_FC_OBJDUMP ?= $(PULP_RISCV_GCC_TOOLCHAIN_CI)/bin/riscv32-unknown-elf-objdump
endif
PULP_ARCH_FC_OBJDFLAGS ?= -Mmarch=rv32imcxpulpv2
PULP_ARCH_LDFLAGS ?=  -march=rv32imcxpulpv2 -D__riscv__
PULP_LDFLAGS_api = /home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/lib/pulp/crt0.o -nostartfiles -nostdlib -Wl,--gc-sections -T/home/msc18f9/Documents/convNet_on_PULP/api/build/pulp/api.ld -L/home/msc18f9/Documents/pulp-sdk/pkg/sdk/dev/install/lib/pulp -lrt -lrtio -lrt -lgcc
PULP_OMP_LDFLAGS_api = -lgomp
pulpRunOpt        += --platform=vp --dir=/home/msc18f9/Documents/convNet_on_PULP/api/build/pulp --config-file=/home/msc18f9/Documents/convNet_on_PULP/api/build/pulp/config.json
pulpRunOpt        += $(PLT_OPT)
