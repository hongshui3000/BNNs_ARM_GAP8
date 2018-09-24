#include "rt/rt_api.h"

rt_dev_t __rt_devices[] = {
  {"uart", -1, 0, (void *)NULL, {{}}},
  {"jtag_proxy", -1, 0, (void *)NULL, {{}}},
  {"jtag_proxy", -1, 0, (void *)NULL, {{}}},
  {"camera", -1, 0, (void *)&himax_desc, {{}}},
};

int __rt_nb_devices = 4;
