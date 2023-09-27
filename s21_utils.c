#include "s21_utils.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Проверяет, является ли число decimal отрицательным.
 * Не делает никаких проверок. Число должно быть корректным!
 *
 * @param dec число типа s21_decimal
 * @return int 1 - true, 0 - false
 */
int s21_is_negative(s21_decimal dec) {
  return ((dec.bits[3] & s21_SIGN) == s21_SIGN) ? 1 : 0;
}

int s21_exp(s21_decimal dec) { return (dec.bits[3] & s21_EXP) >> 16; }

int s21_to_integer(s21_decimal dec) {
  unsigned int result = dec.bits[0];
  if (s21_is_negative(dec)) {
    result = s21_to_complement_code(result);
    result |= s21_SIGN;
  }
  return (int)result;
}

value_type_t s21_float_value_type(float num) {
  value_type_t result = s21_NORMAL_VALUE;
  if (isnan(num)) {
    result = s21_NAN;
  } else if (isinf(num) && num > 0) {
    result = s21_INFINITY;
  } else if (isinf(num) && num < 0) {
    result = s21_NEGATIVE_INFINITY;
  }
  return result;
}

/**
 * Преобразует num в дополнительный код.
 * @param num
 * @return
 */
unsigned int s21_to_complement_code(unsigned int num) { return ~num + 1; }

unsigned int s21_from_complement_code(int num) {
  unsigned int uint = (unsigned int)num;
  if (num < 0) {
    uint -= 1;
    uint = ~uint;
  }
  return uint;
}

int _is_digit(char ch) { return ch >= '0' && ch <= '9'; }

int _is_dot(char ch) { return ch == '.'; }

int _from_str_to_decimal(char *str, s21_decimal *result) {
  s21_decimal num = {0};
  s21_decimal tmp = {0};
  s21_decimal tmp_10 = {0};
  tmp_10.bits[0] = 10;
  int exp = 0;
  int dot = 0;

  if (!strcmp(str, "INFINITY")) {
    num.value_type = s21_INFINITY;
  } else if (!strcmp(str, "NEGATIVE_INFINITY")) {
    num.value_type = s21_NEGATIVE_INFINITY;
  } else if (!strcmp(str, "NAN")) {
    num.value_type = s21_NAN;
  } else {
    for (int i = 0; str[i]; i++) {
      if (dot) exp++;
      if (_is_digit(str[i])) {
        s21_mul(num, tmp_10, &num);
        tmp.bits[0] = (int)(str[i] - '0');
        s21_add(num, tmp, &num);
      }
      if (_is_dot(str[i])) dot = 1;
    }
    set_exp(&num, exp);
  }
  *result = num;
  return result->value_type;
}

char *s21_strcat(char *dst, const char *src) {
  size_t dst_len = s21_strlen(dst);
  for (; *src; *(dst + dst_len++) = *src++) {
  }
  *(dst + dst_len) = '\0';
  return dst;
}

size_t s21_strlen(const char *str) {
  size_t len = 0;
  for (; *(str + len); ++len) {
  }
  return len;
}

void printf_decimal(s21_decimal num, char *buffer) {
  _print_decimal(&num, DECIMAL, get_exp(num), num.value_type, buffer);
}

void _print_decimal(void *num, int size, int exp, value_type_t type,
                    char *buffer) {
  switch (type) {
    case s21_NORMAL_VALUE:

      if (size == DECIMAL) {
        if (_get_sign(num, size)) {
          s21_strcat(buffer, "-");
        }
        _print_num(((s21_decimal *)num)->bits, size, exp, buffer);
      }

      break;
    case s21_INFINITY:
      s21_strcat(buffer, "INFINITY");
      break;
    case s21_NEGATIVE_INFINITY:
      s21_strcat(buffer, "NEGATIVE_INFINITY");
      break;
    case s21_NAN:
      s21_strcat(buffer, "NAN");
      break;
    default:
      break;
  }
  return;
}

void _print_num(uint32_t *num, int size, int exp, char *buffer_str) {
  int n1 = 1, result_n = 1, data_1[LEN] = {2}, result[LEN] = {0};
  int max = (size - 1) * 32;
  for (int i = 0; i < max; i++) {
    if (get_bit(num, i)) {
      if (i) {
        int buffer[LEN] = {0};
        int buffer_n = 1;
        recursive_sum(i, buffer, buffer_n, data_1, n1, buffer, &buffer_n);
        sum(result, result_n, buffer, buffer_n, result, &result_n);

      } else {
        result[0] = 1;
      }
    }
  }
  output(result, result_n, exp, buffer_str);
  return;
}

void recursive_sum(int index, int *data_1, int n1, int *data_2, int n2,
                   int *result, int *result_n) {
  if (index == 1) {
    sum(data_1, n1, data_2, n2, result, result_n);  // 0 + 2
    return;
  }

  recursive_sum(--index, data_1, n1, data_2, n2, result, result_n);
  sum(result, *result_n, result, *result_n, result, result_n);
}

void sum(int *buff1, int len1, int *buff2, int len2, int *result,
         int *result_length) {
  int len_max = len1 > len2 ? len1 : len2;
  int index_res = len_max - 1;
  int index_1 = len1 - 1;
  int index_2 = len2 - 1;

  _sum(index_1, index_2, &len_max, index_res, buff1, buff2, result);

  *result_length = len_max;
  return;
}

void _sum(int index_1, int index_2, int *len_max, int index_res,
          const int *buff1, const int *buff2, int *result) {
  int decade = 0;
  for (int i = 0; i < LEN; i++) {
    if (index_1 - i < 0 && !(index_2 - i < 0)) {
      result[index_res - i] = buff2[index_2 - i] + decade;
      decade = 0;
    }
    if (!(index_1 - i < 0) && index_2 - i < 0) {
      result[index_res - i] = buff1[index_1 - i] + decade;
      decade = 0;
    }
    if (!(index_1 - i < 0) && !(index_2 - i < 0)) {
      result[index_res - i] = buff1[index_1 - i] + buff2[index_2 - i] + decade;
      decade = 0;
    }
    if (index_1 - i < 0 && index_2 - i < 0 && decade > 0) {
      (*len_max)++;
      shift_arr(result, *len_max, -1);
      result[0] = decade;
      break;
    }
    if (index_1 - i < 0 && index_2 - i < 0 && decade == 0) {
      break;
    }
    if (result[index_res - i] > 9) {
      int tmp = result[index_res - i];
      result[index_res - i] = tmp % 10;
      decade = tmp / 10;
    }
  }
}

void output(const int *a, int n, int exp, char *buffer) {
  char num[5] = {0};
  if (exp >= n) {
    s21_strcat(buffer, "0.");
    exp = exp - n;
    while (exp--) {
      s21_strcat(buffer, "0");
    }
  }
  for (int i = 0; i < n; i++) {
    num[0] = a[i] + '0';
    s21_strcat(buffer, num);

    if (exp && !(n - i - 1 - exp)) s21_strcat(buffer, ".");
  }
  return;
}

void shift_arr(int *a, int n, int step) {
  if (step < 0) {
    for (int j = 0; j > step; j--) {
      int tmp1 = a[n - 1];
      for (int i = n - 1; i > 0; i--) {
        a[i] = a[i - 1];
      }
      a[0] = tmp1;
    }
  }
  return;
}

int shift_decimal_bit(uint32_t *num, int size, int bool_is_left, int count) {
  int error = 0;
  for (int k = 0; k < count; k++) {
    int tmp = 0;
    if (bool_is_left) {
      for (int i = 0; i < size - 1; i++) {
        int tmp_2 = num[i] & s21_SIGN;
        num[i] <<= 1;
        if (tmp) {
          num[i] |= 1;
        }
        tmp = tmp_2;
      }
      error = tmp ? 1 : 0;

    } else {
      for (int i = size - 2; i >= 0; i--) {
        int tmp_2 = num[i] & 1;
        num[i] >>= 1;
        if (tmp) {
          num[i] |= s21_SIGN;
        } else {
          if (num[i] & s21_SIGN) {
            num[i] ^= s21_SIGN;
          }
        }
        tmp = tmp_2;
      }
    }
  }
  return error;
}

int get_bit(const uint32_t *num, int _index) {
  int index = _index % 32;
  int buffer_num = 0;
  for (int i = _index - 32; i >= 0; i -= 32) {
    buffer_num++;
  }

  return (num[buffer_num] >> index) & 1;
}

int get_exp(s21_decimal num) {
  return (int)*(2 + (char *)&num.bits[DECIMAL - 1]);
}

int get_extra_exp(extra_decimal num) {
  return (int)*(2 + (char *)&num.bits[EXTRA_DECIMAL - 1]);
}

int set_exp(s21_decimal *num, int exp) {
  int error = 0;
  if (exp > 28 || exp < 0) {
    error = 1;
  } else {
    _set_exp(num->bits, DECIMAL, exp);
  }
  return error;
}

void set_extra_exp(extra_decimal *num, int exp) {
  _set_exp(num->bits, EXTRA_DECIMAL, exp);
  return;
}

void _set_exp(uint32_t *num, int size, int exp) {
  char *num_mini = 2 + (char *)&num[size - 1];
  *num_mini = exp;

  return;
}

int get_sign(s21_decimal num) { return _get_sign(num.bits, DECIMAL); }

int get_extra_sign(extra_decimal num) {
  return _get_sign(num.bits, EXTRA_DECIMAL);
}

int _get_sign(const uint32_t *num, int size) {
  return num[size - 1] & s21_SIGN;
}

int set_sign(uint32_t *num, int size, char ch) {
  int error = !(ch == '-' || ch == '+');
  if (size == DECIMAL) {
    if (!error) {
      char *num_mini = 3 + (char *)&num[DECIMAL - 1];
      if (ch == '-') {
        *num_mini = 0x80;
      } else {
        *num_mini = 0;
      }
    }
  } else if (size == EXTRA_DECIMAL) {
    if (!error) {
      char *num_mini = 3 + (char *)&num[EXTRA_DECIMAL - 1];
      if (ch == '-') {
        *num_mini = 0x80;
      } else {
        *num_mini = 0;
      }
    }
  }
  return error;
}

int extra_find_first_bit(extra_decimal num) {
  return _find_first_bit(num.bits, EXTRA_DECIMAL);
}

int _find_first_bit(uint32_t *num, int size) {
  int res = 0;
  int start = (size - 1) * 32 - 1;
  for (int i = start; i >= 0; i--) {
    if (get_bit(num, i)) {
      res = i;
      break;
    }
  }

  return res;
}

int is_not_zero(s21_decimal num, int value_type) {
  return value_type || _buffer_is_not_zero(num.bits, DECIMAL);
}

int is_not_zero_extra(extra_decimal num, int value_type) {
  return value_type || _buffer_is_not_zero(num.bits, EXTRA_DECIMAL);
}

int _buffer_is_not_zero(uint32_t *num, int size) {
  int res = 0;
  for (int i = 0; i < size - 1; i++) {
    if (num[i]) {
      res = 1;
      break;
    }
  }
  return res;
}

int extra_is_less(extra_decimal num_1, extra_decimal num_2) {
  int res = 0;
  int start = (EXTRA_DECIMAL - 1);
  for (int i = start; i >= 0; i--) {
    if (num_1.bits[i] != num_2.bits[i]) {
      if (num_1.bits[i] < num_2.bits[i]) {
        res = 1;

      } else {
        res = 0;
      }
      break;
    }
  }
  return res;
}

void to_zero(s21_decimal *num) {
  for (int i = 0; i != 4; i++) {
    num->bits[i] = 0;
  }
}
