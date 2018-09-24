


# application
PULP_OMP_APP = api

# source
PULP_APP_OMP_SRCS = api.c

PULP_APP_OMP_SRCS += utility/misc.c
PULP_APP_OMP_SRCS += utility/tensor.c

PULP_APP_OMP_SRCS += model/model.c

PULP_APP_OMP_SRCS += layers/FirstLayer.c
PULP_APP_OMP_SRCS += layers/BinaryLayer.c
PULP_APP_OMP_SRCS += layers/LastLayer.c

PULP_APP_OMP_SRCS += platform/fulmine.c

PULP_LDFLAGS = -lm
PULP_CFLAGS = -O2 -g3

include $(PULP_SDK_HOME)/install/rules/pulp.mk
