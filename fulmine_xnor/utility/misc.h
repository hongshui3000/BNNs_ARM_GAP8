
/**
 * \file misc.h
 * \brief miscellaneous functions
 * \date 18.01.2017
 * 
 * \details 
 * 
 */

#ifndef MISC_H
#define MISC_H

/**
 * \defgroup Utility  
 * [\addtogroup] Utility
 * @{
 */
/* ==================================================================================
 * Includes
 * ================================================================================*/
#include "../config.h"

/* ==================================================================================
 * Definitions (typedef, constants, macros)
 * ================================================================================*/
#define popcount(a) ({\
    __builtin_popcount(a); })

/* ==================================================================================
 * Global variables declarations
 * ================================================================================*/
/* ==================================================================================
 * Global function prototypes
 * ================================================================================*/

/* Basic math functions */
extern int32_t 	s32_max(int32_t s32_a, int32_t s32_b);
extern int32_t 	s32_min(int32_t s32_a, int32_t s32_b);
extern float 	f_max(float f_a, float f_b);
extern float 	f_min(float f_a, float f_b);
extern int32_t 	s32_sign(float s32_a);
extern int32_t 	s32_popcount(uint32_t u32_x);

/**
 * \brief			start core transfer from l2 to l1 
 * @param[in]	data				..l2 base	
 * @param[in]	pos					..offset (address = base + offset)
 * @param[in]	l1_address			..l1 address 
 * @param[in]	length				..number of floats or int32 (4 Bytes) to transfer
 * @retVal			..dma identifier
 */
extern void mcu_transfer_ext2loc(const uint32_t* data, uint32_t* pos, uint32_t* l1_address, uint32_t length, uint8_t element_size);
/**
 * \brief			start dam transfer fro#define popcount(a) ({\
    __builtin_popcount(a); })
m l2 to l1 
 * @param[in]	l2_base_address		..l2 base	
 * @param[in]	l2_offset			..offset (address = base + offset)
 * @param[in]	l1_address			..l1 address 
 * @param[in]	size				..number of floats or int32 (4 Bytes) to transfer
 * @param[in]	element_size		..sizeof(type) in bytes
 * @retVal			..dma identifier
 */
extern uint32_t dma_transfer_ext2loc(uint32_t* l2_base_address, uint32_t* l2_offset, uint32_t* l1_address, uint32_t size, uint8_t element_size);

/**
 * \brief			wait until dma transfer is finished
 * @param[in]	identifier			..dma identifier
 */
extern void dma_wait(uint32_t identifier);


extern void led_init(void);

extern void led_toggle(void);

/**
 * \brief			printf double 
 * @param[in]	fVal		..value to print
 */
extern void myPrintf( double fVal);


#ifdef PERF 

extern void perf_start(void);

extern void perf_print(void);
#endif



#endif
