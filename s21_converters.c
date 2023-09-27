#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "s21_decimal.h"
#include "s21_utils.h"

#define FL_EXP(x) \
  ((*((unsigned int *)(x)) & 0x7F800000U) >> 23) - 127  // IEEE-754: экспонента
#define FL_MNT(x)                        \
  (*((unsigned int *)(x)) & 0x7FFFFFU) | \
      0x800000U  // IEEE-754: мантисса (24 bits)

int s21_from_int_to_decimal(int src, s21_decimal *dst) {
  int result = CONVERTING_ERROR;
  if (dst) {
    unsigned int num = s21_from_complement_code(src);
    dst->bits[0] = num;
    dst->bits[1] = s21_ZERO;
    dst->bits[2] = s21_ZERO;
    dst->bits[3] = src < 0 ? s21_SIGN : s21_ZERO;
    dst->value_type = s21_NORMAL_VALUE;
    result = SUCCESS;
  }
  return result;
}

int s21_from_decimal_to_int(s21_decimal dec, int *dst) {
  int result = CONVERTING_ERROR;
  if (dst && dec.value_type == s21_NORMAL_VALUE) {
    if (s21_exp(dec) > 0) {
      s21_truncate(dec, &dec);
    }
    if (dec.bits[2] == 0 || dec.bits[1] == 0) {
      *dst = s21_to_integer(dec);
      result = SUCCESS;
    }
  }
  return result;
}

float vitya(s21_decimal src);
float dima(s21_decimal src);

int s21_from_decimal_to_float(s21_decimal src, float *dst) {
  int result = CONVERTING_ERROR;
  if (dst) {
    if (src.value_type == s21_NAN) {
      *dst = NAN;
    } else if (src.value_type == s21_INFINITY) {
      *dst = INFINITY;
    } else if (src.value_type == s21_NEGATIVE_INFINITY) {
      *dst = -INFINITY;
    } else {
      *dst = vitya(src);
      // *dst = dima(src);
      result = SUCCESS;
    }
  }
  return result;
}

float vitya(s21_decimal src) {
  char buffer[100] = {0};
  printf_decimal(src, buffer);
  float res = strtod(buffer, NULL);
  // res = (float)(tmp / 10000) * 10000.;

  return res;
}

// float dima(s21_decimal src) {
//    float res = 0.;
//    int exp = s21_exp(src);
//    int neg = s21_is_negative(src);
//    // обнулим экспоненту и знак
//    src.bits[3] = 0U;
//
//    s21_decimal dec_10;
//    s21_from_int_to_decimal(10, &dec_10);
//    s21_decimal dec_0;
//    s21_from_int_to_decimal(0, &dec_0);
//
//    int digs[100] = {0};
//    int digs_count = 0;
//
//    while (s21_is_not_equal(src, dec_0) == TRUE) {
//        int dig;
//        s21_from_decimal_to_int(s21_mod(src, dec_10), &dig);
//        digs[digs_count++] = dig;
//        src = s21_truncate(s21_div(src, dec_10));
//    }
//    // print_info(src);
//    res = 0.0;
//    int real_exp = digs_count - exp - 1;
//    for (int i = digs_count; i > 0; --i) {
//        res += digs[i - 1] * powf(10, real_exp--);
//    }
//    if (neg) {
//        res *= -1;
//    }
//    return res;
// }

// int s21_from_float_to_decimal_v1(float src, s21_decimal *dst) {
//    int result = CONVERTING_ERROR;
//    if (dst) {
//        // инициализируем (+/-) нулем
//        dst->bits[0] = 0;
//        dst->bits[1] = 0;
//        dst->bits[2] = 0;
//        dst->bits[3] = src < 0 ? s21_SIGN : s21_ZERO;
//
//        dst->value_type = s21_float_value_type(src);
//
//        if (dst->value_type == s21_NORMAL_VALUE) {
//            int f_exp = FL_EXP(&src);
//            int f_mnt = FL_MNT(&src);
//            if (f_exp == -127 && f_mnt == 0x800000U) {
//                // это ноль (он уже установлен)
//                result = SUCCESS;
//            } else if (f_exp > 95) {
//                // это переполнение;
//                // возвращаем бесконечность и ошибку
//                dst->value_type = src > 0 ? s21_INFINITY :
//                s21_NEGATIVE_INFINITY;
//            } else if (f_exp < -94) {
//                // слишком маленькое число;
//                // возвращаем ноль (он уже установлен) и ошибку
//            } else {
//                // нормальное число подходящего размера
//                int is_neg = 0;
//                if (src < 0) {
//                    src *= -1;  // модуль числа
//                    is_neg = 1;
//                }
//                int dec_exp = 0;
//                for (; src >= 10; src /= 10, --dec_exp) {
//                }
//                for (; src < 1; src *= 10, ++dec_exp) {
//                }
//                for (int i = 0; i < 7; src *= 10, ++dec_exp, ++i) {
//                }
//                unsigned int trunc_f = (unsigned int)roundf(src);
//                s21_from_int_to_decimal(trunc_f, dst);
//                if (dec_exp >= 0 && dec_exp <= 28) {
//                    set_exp(dst, dec_exp);
//                    result = SUCCESS;
//                } else if (dec_exp < 0) {
//                    result = SUCCESS;
//                }
//                if (is_neg) dst->bits[3] |= s21_SIGN;
//                // print_info(*dst);
//            }
//        }
//    }
//    return result;
// }

void get_str_float(float f, char *buf) {
  size_t buf_size = 13;
  snprintf(buf, buf_size, "%e", f);
}

int get_str_exp(char *src) {
  char exp[3] = {0};
  strncpy(exp, src + 10, 2);
  int sign = (src[9] == '-') ? -1 : 1;
  return atoi(exp) * sign;
}

int get_int_part(float f, int *dec_exp) {
  char str_fl[13] = {0};
  get_str_float(f, str_fl);

  // printf("str_fl = %s\n", str_fl);

  int exp = get_str_exp(str_fl);

  // printf("exp = %d\n", exp);

  str_fl[8] = '\0';
  int scale = 6;
  for (int i = 7; i >= 0; --i) {
    if (str_fl[i] == '0') {
      --scale;
      str_fl[i] = '\0';
    } else {
      break;
    }
  }

  // printf("scale = %d\n", scale);

  *dec_exp = exp - scale;
  char str_mant[8] = {0};
  for (size_t i = 0, j = 0; i < strlen(str_fl); ++i) {
    if (str_fl[i] != '.') {
      str_mant[j++] = str_fl[i];
    }
  }
  return atoi(str_mant);
}

int s21_from_float_to_decimal_v2(float src, s21_decimal *dst) {
  int result = CONVERTING_ERROR;
  if (dst) {
    // инициализируем (+/-) нулем
    dst->bits[0] = 0;
    dst->bits[1] = 0;
    dst->bits[2] = 0;
    dst->bits[3] = src < 0 ? s21_SIGN : s21_ZERO;

    dst->value_type = s21_float_value_type(src);

    if (dst->value_type == s21_NORMAL_VALUE) {
      int f_exp = FL_EXP(&src);
      int f_mnt = FL_MNT(&src);
      if (f_exp == -127 && f_mnt == 0x800000U) {
        // это ноль (он уже установлен)
        result = SUCCESS;
      } else if (f_exp > 95) {
        // это переполнение;
        // возвращаем бесконечность и ошибку
        dst->value_type = src > 0 ? s21_INFINITY : s21_NEGATIVE_INFINITY;
      } else if (f_exp < -94) {
        // слишком маленькое число;
        // возвращаем ноль (он уже установлен) и ошибку
      } else {
        // нормальное число подходящего размера
        int is_neg = 0;
        if (src < 0) {
          src *= -1;  // модуль числа
          is_neg = 1;
        }
        int dec_exp;
        int scaled_int = get_int_part(src, &dec_exp);

        // printf("scaled_int = %d\n", scaled_int);

        s21_from_int_to_decimal(scaled_int, dst);
        if (dec_exp < 0 && dec_exp >= -28) {
          set_exp(dst, -dec_exp);
          result = SUCCESS;
        } else if (dec_exp > 0) {
          s21_decimal dec_10;
          s21_from_int_to_decimal(10, &dec_10);
          for (int i = 0; i < dec_exp; ++i) {
            s21_mul(*dst, dec_10, dst);
          }
          result = SUCCESS;
        } else if (dec_exp == 0) {
          result = SUCCESS;
        }
        if (is_neg) dst->bits[3] |= s21_SIGN;
        // print_info(*dst);
      }
    }
  }
  return result;
}

int s21_from_float_to_decimal(float src, s21_decimal *dst) {
  return s21_from_float_to_decimal_v2(src, dst);
}
