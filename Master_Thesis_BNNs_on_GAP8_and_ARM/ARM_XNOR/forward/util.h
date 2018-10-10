#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include "tensor.h"

/* Basic math functions */
int32_t max(int32_t a, int32_t b);
int32_t min(int32_t a, int32_t b);
int64_t max(int64_t a, int64_t b);
int64_t min(int64_t a, int64_t b);
float max(float a, float b);
float min(float a, float b);
int32_t sign(float a);
//int32_t popcount(uint32_t x);
inline int32_t popcount(uint32_t x){

    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
//    x = x + (x >> 8);
//    x = x + (x >> 16);
//    return x & 0x0000003F;
    return (x*0x01010101)>>24;

}

/* Read data from model file */
// 1d array of floats
void read(float *target, const float *data, int32_t size, int32_t &pos);

void read(uint32_t *target, const uint32_t *data, int32_t size, int32_t &pos);

void read(int8_t *target, const int8_t *data, int32_t size, int32_t &pos);

void read(uint8_t *target, const uint8_t *data, int32_t size, int32_t &pos);

void read(int16_t *target, const int16_t *data, int32_t size, int32_t &pos);

void read(int32_t *target, const int32_t *data, int32_t size, int32_t &pos);
// 3d array of floats
void read(float *target, const float *data, int32_t X, 
	  int32_t Y, int32_t Z, int32_t &pos);
// 3d array of 16 fix point
void read(int16_t *target, const int16_t *data, int32_t X, 
	  int32_t Y, int32_t Z, int32_t &pos);
// 3d array of 32 fix point
void read(int32_t *target, const int32_t *data, int32_t X, 
	  int32_t Y, int32_t Z, int32_t &pos);
// 3d array of unsigneds
void read(uint32_t *target, const uint32_t *data, int32_t X, 
	  int32_t Y, int32_t Z, int32_t &pos);

#endif
