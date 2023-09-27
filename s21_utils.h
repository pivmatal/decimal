#ifndef _S21_UTILS_H
#define _S21_UTILS_H

#include <stddef.h>
#include <stdio.h>

#include "s21_decimal.h"

typedef union {
  float fl;
  unsigned int dw;
} conv_float_t;

typedef enum { ADD, DIV, MOD, MUL } Operation;

#define EXTRA_DECIMAL 10

typedef struct extra_decimal {
  unsigned int bits[EXTRA_DECIMAL];
  value_type_t value_type;
} extra_decimal;

#define LEN 1000

// S21_BIT_TO_STR_H_
void sum(int *buff_1, int len_1, int *buff_2, int len_2, int *result,
         int *result_length);
void output(const int *a, int n, int exp, char *buffer);
void shift_arr(int *a, int n, int step);
void _sum(int index_1, int index_2, int *len_max, int index_res,
          const int *buff_1, const int *buff_2, int *result);
void recursive_sum(int index, int *data_1, int n1, int *data_2, int n2,
                   int *result, int *result_n);
void _print_decimal(void *num, int size, int exp, value_type_t type,
                    char *buffer);
void _print_num(uint32_t *num, int size, int exp, char *buffer_str);
char *s21_strcat(char *dst, const char *src);
size_t s21_strlen(const char *str);
void printf_decimal(s21_decimal num, char *buffer);
int _is_digit(char ch);
int _is_dot(char ch);
int _from_str_to_decimal(char *str, s21_decimal *result);

// S21_BIT_H_
int get_exp(s21_decimal num);
int get_extra_exp(extra_decimal num);
int get_bit(const uint32_t *num, int index);

int set_exp(s21_decimal *num, int exp);
void set_extra_exp(extra_decimal *num, int exp);
void _set_exp(uint32_t *num, int size, int exp);

int set_sign(uint32_t *num, int size, char ch);
int get_sign(s21_decimal num);
int get_extra_sign(extra_decimal num);
int _get_sign(const uint32_t *num, int size);

int shift_decimal_bit(uint32_t *num, int size, int to_left, int shift_count);
int extra_find_first_bit(extra_decimal num);
int _find_first_bit(uint32_t *num, int size);
int is_not_zero(s21_decimal num, int value_type);
int is_not_zero_extra(extra_decimal num, int value_type);
int _buffer_is_not_zero(uint32_t *num, int size);

int extra_is_less(extra_decimal num_1, extra_decimal num_2);

// EXCIPTIONS
int _mod_res_is_NAN(value_type_t value_type_1, value_type_t value_type_2,
                    s21_decimal num_2);
int _div_res_is_NAN(value_type_t value_type_1, value_type_t value_type_2,
                    s21_decimal num_1, s21_decimal num_2);
int _add_res_is_NAN(value_type_t num_1, value_type_t num_2);
int _mul_res_is_NAN(s21_decimal num_1, s21_decimal num_2);
int _sub_res_is_NAN(s21_decimal num_1, s21_decimal num_2);
value_type_t _sub_value_type(s21_decimal num_1, s21_decimal num_2);
value_type_t _div_value_type(value_type_t value_type_1,
                             value_type_t value_type_2, s21_decimal num_1,
                             s21_decimal num_2);
value_type_t _mul_value_type(s21_decimal num_1, s21_decimal num_2);
value_type_t _add_value_type(value_type_t num_1, value_type_t num_2);

// NEW_MATH_H
extra_decimal convert_to_extra_decimal(s21_decimal num, int exp_2,
                                       Operation operation);
int convert_from_extra_decimal(extra_decimal extra_num, int exp,
                               value_type_t *value_type, s21_decimal *result);
extra_decimal normalize_extra(extra_decimal num, int exp_2,
                              Operation operation);
extra_decimal denormalize_extra(extra_decimal num, int exp);
value_type_t check_decimal_overflow(extra_decimal num);
extra_decimal extra_negate(extra_decimal num);
int _add_decimal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result);
int _sub_decimal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result);
int _mul_decimal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result);
int _div_decimal(s21_decimal num, s21_decimal devisor, s21_decimal *result);
int _mod_decimal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result);
extra_decimal _add_extra(extra_decimal num_1, extra_decimal num_2);
extra_decimal _sub_extra(extra_decimal num_1, extra_decimal num_2);
extra_decimal _mul_extra(extra_decimal num_1, extra_decimal num_2);
extra_decimal _mod_extra(extra_decimal num, extra_decimal devisor);
extra_decimal _div_extra(extra_decimal num, extra_decimal devisor);
int _div_normal(s21_decimal num, s21_decimal devisor, s21_decimal *result);
extra_decimal _div_without_remaind(extra_decimal num, extra_decimal devisor);
extra_decimal _div_main(extra_decimal num_1, extra_decimal num_2, int need_mod);
extra_decimal bank_round(extra_decimal num, extra_decimal tmp,
                         extra_decimal tmp_10, int exp);
void check_zeros_exp_and_sign(s21_decimal *num_1, s21_decimal *num_2,
                              value_type_t *value_type_1,
                              value_type_t *value_type_2);
int check_need_div(s21_decimal num_1, s21_decimal num_2);

// For converting
int s21_is_negative(s21_decimal dec);

int s21_exp(s21_decimal dec);

int s21_to_integer(s21_decimal dec);

value_type_t s21_float_value_type(float num);

unsigned int s21_to_complement_code(unsigned int num);

unsigned int s21_from_complement_code(int num);

void to_zero(s21_decimal *num);

#endif  //_S21_UTILS_H
