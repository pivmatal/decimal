#ifndef S21_DOPFUNC_H
#define S21_DOPFUNC_H

#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s21_decimal.h"
#include "s21_utils.h"

int delon10(s21_decimal *decimal);
int num_to_dec_four(s21_decimal *decimal, int count);
int get_bit_d(s21_decimal *decimal, int num_bit);
void set_bit_d(s21_decimal *decimal, int num_bit);
int div_by_10(s21_decimal *decimal);
int get_scale_d(s21_decimal *decimal);
int umnon10(s21_decimal *decimal);
void norm_dec(s21_decimal *decimal);
int comp_mant(s21_decimal *decimal1, s21_decimal *decimal2);
void decimal_shift_left(s21_decimal *decimal);
void mul_on_10(s21_decimal *decimal);
int on_10(s21_decimal *decimal);
int add_int(s21_decimal value, int integer, s21_decimal *result);
int mod_by_int(s21_decimal value, int integer);
int is_remainder_zero(s21_decimal value, int scale);
int is_decimal_zero(s21_decimal value);
double pow_2(int degree);
int delon10(s21_decimal *decimal);

#endif /* S21_DOPFUNC_H */