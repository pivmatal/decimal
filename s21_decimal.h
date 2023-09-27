#ifndef S21_DECIMAL_H_
#define S21_DECIMAL_H_

#include <stdint.h>
#include <stdio.h>

typedef enum {
  s21_NORMAL_VALUE = 0,
  s21_INFINITY = 1,
  s21_NEGATIVE_INFINITY = 2,
  s21_NAN = 3
} value_type_t;

#define DECIMAL 4
typedef struct {
  unsigned int bits[4];
  value_type_t value_type;
} s21_decimal;

#define SUCCESS 0
#define CONVERTING_ERROR 1
#define TRUE 1
#define FALSE 0

// Маски для выделения нужных последовательностей битов
enum {
  s21_ZERO = 0U,
  s21_EXP = 0xFF0000U,
  s21_SIGN = 0x80000000U,
  s21_FULL = 0xFFFFFFFFU
};

int s21_add(s21_decimal, s21_decimal, s21_decimal *);

int s21_sub(s21_decimal, s21_decimal, s21_decimal *);

int s21_mul(s21_decimal, s21_decimal, s21_decimal *);

int s21_div(s21_decimal, s21_decimal, s21_decimal *);

int s21_mod(s21_decimal, s21_decimal, s21_decimal *);

int s21_is_equal(s21_decimal, s21_decimal);

int s21_is_less(s21_decimal, s21_decimal);

int s21_is_less_or_equal(s21_decimal, s21_decimal);

int s21_is_greater(s21_decimal, s21_decimal);

int s21_is_greater_or_equal(s21_decimal, s21_decimal);

int s21_is_not_equal(s21_decimal, s21_decimal);

int s21_from_int_to_decimal(int src, s21_decimal *dst);

int s21_from_float_to_decimal(float src, s21_decimal *dst);

int s21_from_decimal_to_int(s21_decimal src, int *dst);

int s21_from_decimal_to_float(s21_decimal src, float *dst);

int s21_floor(s21_decimal, s21_decimal *);

int s21_round(s21_decimal, s21_decimal *);

int s21_truncate(s21_decimal, s21_decimal *);

int s21_negate(s21_decimal, s21_decimal *);

#endif  // S21_DECIMAL_H_
