#PULP_OMP_APP = test
#PULP_APP_FC_SRCS = test.c
#PULP_APP_CL_OMP_SRCS = cluster.c
#PULP_CFLAGS = -O3 -g


# application
PULP_OMP_APP = api

# source
#PULP_APP_OMP_SRCS = api.c
#PULP_APP_OMP_SRCS += utility/main_function.c
#PULP_APP_OMP_SRCS += utility/misc.c
#PULP_APP_OMP_SRCS += utility/tensor.c
#PULP_APP_OMP_SRCS += model/model.c
#PULP_APP_OMP_SRCS += layers/FirstLayer.c
#PULP_APP_OMP_SRCS += layers/BinaryLayer.c
#PULP_APP_OMP_SRCS += layers/LastLayer.c
#PULP_APP_OMP_SRCS += mfcc/mfcc.c

PULP_APP_FC_SRCS = api.c
PULP_APP_CL_OMP_SRCS = utility/main_function.c
PULP_APP_CL_OMP_SRCS += utility/misc.c
PULP_APP_CL_OMP_SRCS += utility/tensor.c
PULP_APP_CL_OMP_SRCS += model/model.c
PULP_APP_CL_OMP_SRCS += layers/FirstLayer.c
PULP_APP_CL_OMP_SRCS += layers/BinaryLayer.c
PULP_APP_CL_OMP_SRCS += layers/LastLayer.c
PULP_APP_CL_OMP_SRCS += mfcc/mfcc.c

#PULP_APP_OMP_SRCS += platform/gap.c
#PULP_APP_SRCS += wav_synchro.c
#PULP_APP_OMP_SRCS += mfcc/mfcc_dump.c
#PULP_APP_OMP_SRCS += mfcc/SwapTablesDef.c
#PULP_APP_OMP_SRCS += mfcc/TwiddlesDef.c
#PULP_APP_OMP_SRCS += mfcc/GenTwid_Swap.c
#PULP_APP_OMP_SRCS += mfcc/FFT_Scalar_Lib.c
#PULP_APP_OMP_SRCS += mfcc/FFT_Lib.c

#PLPBRIDGE_FLAGS += -s i2s.py

PULP_LDFLAGS = -lm
#PULP_CFLAGS = -O2 -g3 -Wno-sign-compare
PULP_CFLAGS = -O3 -g -Wno-sign-compare
include $(PULP_SDK_HOME)/install/rules/pulp_rt.mk
#include $(PULP_SDK_HOME)/install/rules/pulp.mk
