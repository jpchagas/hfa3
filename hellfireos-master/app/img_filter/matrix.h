#ifndef _MATRIX_H_
#define _MATRIX_H_

#include <hellfire.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

void matrix_print(uint8_t *a, int16_t h, int16_t w);
void matrix_print_sub(uint8_t *a, int16_t h, int16_t w, int16_t k, int16_t l, int16_t bh, int16_t bw);
void crop_sub_matrix(uint8_t *a, uint8_t *b, int16_t h, int16_t w, int16_t k, int16_t l, int16_t bh, int16_t bw);
void set_sub_matrix(uint8_t *a, uint8_t *b, int16_t h, int16_t w, int16_t k, int16_t l, int16_t bh, int16_t bw);
void set_matrix_borders(uint8_t *a, int16_t ah, int16_t aw, uint8_t *b, int16_t bh, int16_t bw);

#endif
