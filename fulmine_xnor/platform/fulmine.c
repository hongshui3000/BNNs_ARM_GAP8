


/* ==================================================================================
 * Includes
 * ================================================================================*/

#include "fulmine.h"


/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/

#define SOC_FLL_BASE_ADDR     ( CVP_BASE_ADDR  + 0x00 )
#define SOC_FLL_CONFIG_REG_1  ( SOC_FLL_BASE_ADDR + 0x04 )
#define SOC_FLL_CONFIG_REG_2  ( SOC_FLL_BASE_ADDR + 0x08 )
#define SOC_FLL_CONFIG_REG_3  ( SOC_FLL_BASE_ADDR + 0x0C )
#define SOC_FLL_LOCK_REG      ( CVP_BASE_ADDR + 0x20 )

#define CLK_REG_DIV2 (APB_SOC_CTRL_ADDR + 0x4C)

#define SOC_CLUSTER_BASE_ADDR     ( CVP_BASE_ADDR  + 0x10 )
#define SOC_CLUSTER_CONFIG_REG_1  ( SOC_CLUSTER_BASE_ADDR + 0x04 )
#define SOC_CLUSTER_CONFIG_REG_2  ( SOC_CLUSTER_BASE_ADDR + 0x08 )
#define SOC_CLUSTER_CONFIG_REG_3  ( SOC_CLUSTER_BASE_ADDR + 0x0C )
#define SOC_CLUSTER_LOCK_REG      ( CVP_BASE_ADDR + 0x20 )

#define CLUSTER_FLL_BASE_ADDR     ( CVP_BASE_ADDR  + 0x10 )
#define CLUSTER_FLL_CONFIG_REG_1  ( CLUSTER_FLL_BASE_ADDR + 0x04 )
#define CLUSTER_FLL_CONFIG_REG_2  ( CLUSTER_FLL_BASE_ADDR + 0x08 )
#define CLUSTER_FLL_CONFIG_REG_3  ( CLUSTER_FLL_BASE_ADDR + 0x0C )
#define CLUSTER_FLL_LOCK_REG      ( CVP_BASE_ADDR + 0x20 )

/* ==================================================================================
 * Global function implementations
 * ================================================================================*/

/* fill set workaround to go about 150Mhz */
void fll_set_cluster_frequency(int frequency) {
  int multfact = frequency * 1000000 / 32768;

  *(volatile int*)CLUSTER_FLL_CONFIG_REG_1 = multfact & 0xFFFF;
  *(volatile int*)CLUSTER_FLL_CONFIG_REG_2 = 0x00010009;
  *(volatile int*)CLUSTER_FLL_CONFIG_REG_3 = 0x00401002;

  while(*((volatile int*)CLUSTER_FLL_LOCK_REG) == 0);

} 

void fll_set_soc_frequency(int frequency) {
  int multfact = frequency * 1000000 / 32768;

  *(volatile int*)CLUSTER_FLL_CONFIG_REG_1 = multfact & 0xFFFF;
  *(volatile int*)CLUSTER_FLL_CONFIG_REG_2 = 0x00010009;
  *(volatile int*)CLUSTER_FLL_CONFIG_REG_3 = 0x00401002;

  while(*((volatile int*)CLUSTER_FLL_LOCK_REG) == 0);

} 