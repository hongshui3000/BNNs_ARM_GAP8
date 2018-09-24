#include "util.h"

int32_t max(int32_t a, int32_t b){
    
    return (a > b) ? a: b;

}

int32_t min(int32_t a, int32_t b){

    return (a < b) ? a: b;

}

int64_t max(int64_t a, int64_t b){
    
    return (a > b) ? a: b;

}

int64_t min(int64_t a, int64_t b){

    return (a < b) ? a: b;

}

float max(float a, float b){
    
    return (a > b) ? a: b;

}

float min(float a, float b){

    return (a < b) ? a: b;

}

int32_t sign(float a){

    return (a > 0) ? 1: -1;

}

/*int32_t popcount(uint32_t x){

    x = x - ((x >> 1) & 0x55555555);
    x = (x & 0x33333333) + ((x >> 2) & 0x33333333);
    x = (x + (x >> 4)) & 0x0F0F0F0F;
    x = x + (x >> 8);
    x = x + (x >> 16);
    return x & 0x0000003F;

}*/

void read(float *target, const float *data, int32_t size, int32_t &pos){

    int32_t i;

    for(i = 0; i < size; i++){
	target[i] = data[pos];
	pos++;
    }

}

void read(uint32_t *target, const uint32_t *data, int32_t size, int32_t &pos){

    int32_t i;

    for(i = 0; i < size; i++){
	target[i] = data[pos];
	pos++;
    }

}

void read(int8_t *target, const int8_t *data, int32_t size, int32_t &pos){

    int32_t i;

    for(i = 0; i < size; i++){
	target[i] = data[pos];
	pos++;
    }

}
void read(uint8_t *target, const uint8_t *data, int32_t size, int32_t &pos){

    int32_t i;

    for(i = 0; i < size; i++){
	target[i] = data[pos];
	pos++;
    }

}

void read(int16_t *target, const int16_t *data, int32_t size, int32_t &pos){

    int32_t i;

    for(i = 0; i < size; i++){
	target[i] = data[pos];
	pos++;
    }

}

void read(int32_t *target, const int32_t *data, int32_t size, int32_t &pos){

    int32_t i;

    for(i = 0; i < size; i++){
	target[i] = data[pos];
	pos++;
    }

}

void read(float *target, const float *data, int32_t X,
	  int32_t Y, int32_t Z, int32_t &pos){
    
    int32_t i, j, k;
    
    for(i = 0; i < X; i++){
	for(j = 0; j < Y; j++){
	    for(k = 0; k < Z; k++){
		target[(i*Y+j)*Z+k] = data[pos];
		pos++;
	    }
	}
    }


}

void read(int16_t *target, const int16_t *data, int32_t X,
	  int32_t Y, int32_t Z, int32_t &pos){
    
    int32_t i, j, k;
    
    for(i = 0; i < X; i++){
	for(j = 0; j < Y; j++){
	    for(k = 0; k < Z; k++){
		target[(i*Y+j)*Z+k] = data[pos];
		pos++;
	    }
	}
    }


}

void read(int32_t *target, const int32_t *data, int32_t X,
	  int32_t Y, int32_t Z, int32_t &pos){
    
    int32_t i, j, k;
    
    for(i = 0; i < X; i++){
	for(j = 0; j < Y; j++){
	    for(k = 0; k < Z; k++){
		target[(i*Y+j)*Z+k] = data[pos];
		pos++;
	    }
	}
    }


}

void read(uint32_t *target, const uint32_t *data, int32_t X,
	  int32_t Y, int32_t Z, int32_t &pos){

    int32_t i, j, k;

    for(i = 0; i < X; i++){
	for(j = 0; j < Y; j++){
	    for(k = 0; k < Z; k++){
		target[(i*Y+j)*Z+k] = data[pos];
		pos++;
	    }
	}
    }

}
