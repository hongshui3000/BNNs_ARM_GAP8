
#
# HEADER RULES
#

define declareInstallFile

$(PULP_SDK_INSTALL)/$(1): $(1)
	install -D $(1) $$@

INSTALL_HEADERS += $(PULP_SDK_INSTALL)/$(1)

endef


define declareWsInstallFile

$(PULP_SDK_WS_INSTALL)/$(1): $(1)
	install -D $(1) $$@

WS_INSTALL_HEADERS += $(PULP_SDK_WS_INSTALL)/$(1)

endef


$(foreach file, $(INSTALL_FILES), $(eval $(call declareInstallFile,$(file))))

$(foreach file, $(WS_INSTALL_FILES), $(eval $(call declareWsInstallFile,$(file))))


#
# CC RULES for domain: cluster
#

PULP_LIB_NAME_api ?= api

PULP_CL_EXTRA_SRCS_api = 
PULP_CL_EXTRA_ASM_SRCS_api = 
PULP_CL_EXTRA_OMP_SRCS_api = 

api_CL_OBJS =     $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/cl/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/cl/%.o, $(PULP_APP_CL_SRCS_api) $(PULP_CL_SRCS_api) $(PULP_APP_CL_SRCS) $(PULP_APP_SRCS) $(PULP_CL_EXTRA_SRCS_api)))
api_CL_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/cl/%.o, $(PULP_APP_CL_ASM_SRCS_api) $(PULP_CL_ASM_SRCS_api) $(PULP_APP_CL_ASM_SRCS) $(PULP_APP_ASM_SRCS) $(PULP_CL_EXTRA_ASM_SRCS_api))
api_CL_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/cl/omp/%.o, $(PULP_APP_CL_OMP_SRCS_api) $(PULP_CL_OMP_SRCS_api) $(PULP_APP_CL_OMP_SRCS) $(PULP_APP_OMP_SRCS) $(PULP_CL_EXTRA_OMP_SRCS_api))

ifneq '$(api_CL_OMP_OBJS)' ''
PULP_LDFLAGS_api += $(PULP_OMP_LDFLAGS_api)
endif

-include $(api_CL_OBJS:.o=.d)
-include $(api_CL_ASM_OBJS:.o=.d)
-include $(api_CL_OMP_OBJS:.o=.d)

api_cl_cflags = $(PULP_CL_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_CL_CFLAGS) $(PULP_CFLAGS_api) $(PULP_CL_CFLAGS_api) $(PULP_APP_CFLAGS_api)
api_cl_omp_cflags = $(api_cl_cflags) $(PULP_OMP_CFLAGS) $(PULP_CL_OMP_CFLAGS) $(PULP_OMP_CFLAGS_api) $(PULP_CL_OMP_CFLAGS_api)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/cl/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(api_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/cl/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(api_cl_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/cl/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_CL_CC) $(api_cl_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/cl/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_CL_CC) $(api_cl_omp_cflags) -MMD -MP -c $< -o $@

api_OBJS += $(api_CL_OBJS) $(api_CL_ASM_OBJS) $(api_CL_OMP_OBJS)



#
# CC RULES for domain: fabric_controller
#

PULP_LIB_NAME_api ?= api

PULP_FC_EXTRA_SRCS_api = /scratch/sem17f11/xnornet_gap8/gap8/build/wolfe/rt_conf.c /scratch/sem17f11/xnornet_gap8/gap8/build/wolfe/rt_pad_conf.c
PULP_FC_EXTRA_ASM_SRCS_api = 
PULP_FC_EXTRA_OMP_SRCS_api = 

api_FC_OBJS =     $(patsubst %.cpp,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/fc/%.o, $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/fc/%.o, $(PULP_APP_FC_SRCS_api) $(PULP_FC_SRCS_api) $(PULP_APP_FC_SRCS)  $(PULP_FC_EXTRA_SRCS_api)))
api_FC_ASM_OBJS = $(patsubst %.S,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/fc/%.o, $(PULP_APP_FC_ASM_SRCS_api) $(PULP_FC_ASM_SRCS_api) $(PULP_APP_FC_ASM_SRCS)  $(PULP_FC_EXTRA_ASM_SRCS_api))
api_FC_OMP_OBJS = $(patsubst %.c,$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/fc/omp/%.o, $(PULP_APP_FC_OMP_SRCS_api) $(PULP_FC_OMP_SRCS_api) $(PULP_APP_FC_OMP_SRCS)  $(PULP_FC_EXTRA_OMP_SRCS_api))

ifneq '$(api_FC_OMP_OBJS)' ''
PULP_LDFLAGS_api += $(PULP_OMP_LDFLAGS_api)
endif

-include $(api_FC_OBJS:.o=.d)
-include $(api_FC_ASM_OBJS:.o=.d)
-include $(api_FC_OMP_OBJS:.o=.d)

api_fc_cflags = $(PULP_FC_ARCH_CFLAGS) $(PULP_CFLAGS) $(PULP_FC_CFLAGS) $(PULP_CFLAGS_api) $(PULP_FC_CFLAGS_api) $(PULP_APP_CFLAGS_api)
api_fc_omp_cflags = $(api_fc_cflags) $(PULP_OMP_CFLAGS) $(PULP_FC_OMP_CFLAGS) $(PULP_OMP_CFLAGS_api) $(PULP_FC_OMP_CFLAGS_api)

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/fc/%.o: %.c
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(api_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/fc/%.o: %.cpp
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(api_fc_cflags) -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/fc/%.o: %.S
	@mkdir -p `dirname $@`
	$(PULP_FC_CC) $(api_fc_cflags) -DLANGUAGE_ASSEMBLY -MMD -MP -c $< -o $@

$(CONFIG_BUILD_DIR)/$(PULP_LIB_NAME_api)/fc/omp/%.o: %.c
	@mkdir -p `dirname $@`
	touch libgomp.spec
	$(PULP_FC_CC) $(api_fc_omp_cflags) -MMD -MP -c $< -o $@

api_OBJS += $(api_FC_OBJS) $(api_FC_ASM_OBJS) $(api_FC_OMP_OBJS)



#
# LINKER RULES for application: api
#

$(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP): $(api_OBJS)
	mkdir -p `dirname $@`
	$(PULP_LD) $(PULP_ARCH_LDFLAGS) -MMD -MP -o $@ $^ $(PULP_LDFLAGS_api) $(PULP_LDFLAGS)

$(CONFIG_BUILD_DIR)/api.ld: $(PULP_SDK_HOME)/install/rules/tools.mk
	plpflags genlink $(FLAGS_OPT) --output-dir=$(CONFIG_BUILD_DIR) $(apps) --config-file=$(CONFIG_BUILD_DIR)/config.json

$(CONFIG_BUILD_DIR)/api_ld:
	plpflags genlink $(FLAGS_OPT) --output-dir=$(CONFIG_BUILD_DIR) $(apps) --config-file=$(CONFIG_BUILD_DIR)/config.json

$(PULP_SDK_INSTALL)/bin/$(PULP_APP): $(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP)
	mkdir -p `dirname $@`
	cp $< $@

disopt ?= -d

dis:
	$(PULP_OBJDUMP) $(PULP_ARCH_OBJDFLAGS) $(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP) $(disopt)

extract:
	for symbol in $(symbols); do $(PULP_PREFIX)readelf -s $(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP) | grep $$symbol | gawk '{print $$2}' > $(CONFIG_BUILD_DIR)/$(PULP_APP)/$$symbol.txt; done

disdump:
	$(PULP_OBJDUMP) $(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP) $(disopt) > $(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP).s
	@echo
	@echo  "Disasembled binary to $(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP).s"

TARGETS += $(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP)
GEN_TARGETS += $(CONFIG_BUILD_DIR)/api.ld
GEN_TARGETS_FORCE += $(CONFIG_BUILD_DIR)/api_ld
CLEAN_TARGETS += $(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP) $(api_OBJS)
RUN_BINARY = $(PULP_APP)/$(PULP_APP)
override CONFIG_OPT += **/loader/binaries=$(CONFIG_BUILD_DIR)/$(PULP_APP)/$(PULP_APP)
INSTALL_TARGETS += $(PULP_SDK_INSTALL)/bin/$(PULP_APP)


header:: $(INSTALL_HEADERS) $(WS_INSTALL_HEADERS)

fullclean::
	rm -rf $(CONFIG_BUILD_DIR)

clean:: $(GEN_TARGETS) $(CONFIG_BUILD_DIR)/config.mk
	rm -rf $(CLEAN_TARGETS)

prepare:: $(GEN_TARGETS) $(CONFIG_BUILD_DIR)/config.mk
	pulp-run $(pulpRunOpt) prepare

gen: $(GEN_TARGETS_FORCE)

build:: $(GEN_TARGETS) $(CONFIG_BUILD_DIR)/config.mk $(TARGETS)

all:: build prepare

install:: $(INSTALL_TARGETS)

run::
	pulp-run $(pulpRunOpt)

.PHONY: clean header prepare all install run
