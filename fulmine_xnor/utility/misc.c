

/**
 * \file misc.c
 * \brief   miscellaneous functions
 * \date 18.01.2017
 * 
 * \details 
 */ 

/**
 * \defgroup Utility <DESCRIPTION>
 * [\addtogroup] Utility
 * @{
 */

/* ==================================================================================
 * Includes
 * ================================================================================*/
#include <omp.h>
#include "misc.h"
/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
#define LED_PIN             16
#define LED_FUNCTION_MAP    17


/**
 * \brief Struct with function pointers for all fram functions
 */

/* ==================================================================================
 * Local/Global variables definitions
 * ================================================================================*/
static uint8_t toggle;
/* ==================================================================================
 * Local function declarations
 * ================================================================================*/
/* ==================================================================================
 * Local function implementations
 * ================================================================================*/
/* ==================================================================================
 * Global function implementations
 * ================================================================================*/


extern int32_t  s32_max(int32_t s32_a, int32_t s32_b)
{
    return (s32_a > s32_b) ? s32_a: s32_b;
}


extern int32_t  s32_min(int32_t s32_a, int32_t s32_b)
{
    return (s32_a < s32_b) ? s32_a: s32_b;
}

extern float    f_max(float f_a, float f_b)
{   
    return (f_a > f_b) ? f_a: f_b;
}

extern float    f_min(float f_a, float f_b)
{
    return (f_a < f_b) ? f_a: f_b;
}

extern int32_t  s32_sign(float s32_a)
{
    return (s32_a > 0) ? 1: -1;
}

extern inline int32_t s32_popcount(uint32_t u32_x)
{
    return __builtin_popcount(u32_x);              
}

extern void mcu_transfer_ext2loc
(
    const uint32_t* data,
    uint32_t* pos,
    uint32_t* l1_address,
    uint32_t length,
    uint8_t element_size
)
{   
    uint32_t i;
    
    #ifdef MULTICORES
    #pragma omp parallel for schedule(static) 
    #endif    
    for(i = 0; i < length; i++)
    {

        if(element_size == 2)
        {
            ((int16_t*)l1_address)[i] = ((int16_t*)data)[*pos + i];
        }
        else    
        {
            l1_address[i] = data[*pos + i];
        } 
    }
    *pos += length;
}


extern uint32_t dma_transfer_ext2loc
(
    uint32_t* l2_base_address,
    uint32_t* l2_offset, 
    uint32_t* l1_address, 
    uint32_t size,
    uint8_t element_size
)
{
    uint32_t identifier;
    uint32_t l2_address; 
    int bytes;

    /* dma transfer */
    l2_address = ((uint32_t) l2_base_address) + element_size * (*l2_offset);
    identifier = plp_dma_memcpy(l2_address, (uint32_t)l1_address, size*element_size, PLP_DMA_EXT2LOC);

    /* increase offset/position */
    *l2_offset =  *l2_offset + size;   
    
    return identifier; 
}



extern void dma_wait(uint32_t identifier)
{
    plp_dma_wait(identifier);
}


extern void led_init(void)
{
    toggle = 0;
    set_pin_function(LED_FUNCTION_MAP, 1);
    set_gpio_pin_direction(LED_PIN, 1);
    set_gpio_pin_value(LED_PIN, 0);  
}

extern void led_toggle(void)
{
    toggle ^= 1;
    set_gpio_pin_value(LED_PIN, toggle);
}


extern void myPrintf(double fVal)
{
    int dVal, i;
    int long dec;
    char c[6];
    c[5] = 0;


    dVal = fVal;
    dec = (int)(fVal * 100000) % 100000;

    if(dec < 0)
    {
        dec *= (-1);
        if(dVal == 0)
        {
            printf("-");
        }
    }

    for(i = 5-1; i >= 0; i--)
    {
        c[i] = dec % 10 + '0';
        dec /= 10;
    }

    printf("%d.", dVal);
    printf("%s", c);
    printf("\n");
}



#ifdef PERF 

    void perf_start(void)
    {
        // hw_perf_t perf;
        #pragma omp parallel
        {
        int id = plp_coreId();
        hw_perf_init(&perf[id]);
        hw_perf_step(&perf[id]);
        hw_perf_start(&perf[id]);
        }
    }

    void perf_print(void)
    {
        #pragma omp parallel
        {
        int i;
        int id = plp_coreId();
        hw_perf_stop(&perf[id]);
        hw_perf_commit(&perf[id]);

        #pragma omp critical
        {
        printf("[CORE %d] HW performance counters results\n", plp_coreId());
      
        for (i=0; i<hw_perf_nb_events(&perf[id]); i++)
        {
            printf("%20s: %d\n", hw_perf_get_name(&perf[id], i), hw_perf_get_value(&perf[id], i));
      
        }    
        }}
    }
#endif
