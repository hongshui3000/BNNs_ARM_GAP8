#include "cluster.h"
#include "rt/rt_api.h"

void cluster_entry(void *arg)
{
  rt_omp_start();

  printf("(%d, %d) Enter cluster entry\n", rt_cluster_id(), rt_core_id());

#pragma omp parallel
  {
    printf("(%d, %d) Enter parallel section\n", rt_cluster_id(), rt_core_id());
  }

#pragma omp parallel for schedule(static)
 for(int i =0; i<32;i++) 
    {
    printf("(%d, %d) Enter for iteration (iter=%d)\n", rt_cluster_id(), rt_core_id(), i);
    }
}
