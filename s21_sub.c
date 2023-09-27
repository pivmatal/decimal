#include "s21_utils.h"

int s21_sub(s21_decimal num_1, s21_decimal num_2, s21_decimal *result) {
  s21_decimal res = {{0, 0, 0, 0}, 0};
  int exp_1 = get_exp(num_1);
  int exp_2 = get_exp(num_2);
  value_type_t value_type_1 = s21_NORMAL_VALUE, value_type_2 = s21_NORMAL_VALUE;
  check_zeros_exp_and_sign(&num_1, &num_2, &value_type_1, &value_type_2);
  set_exp(&num_1, exp_1);
  set_exp(&num_2, exp_2);
  if (!num_1.value_type && !num_2.value_type) {
    _sub_decimal(num_1, num_2, &res);
  } else if (_sub_res_is_NAN(num_1, num_2)) {
    res.value_type = s21_NAN;
  } else {
    res.value_type = _sub_value_type(num_1, num_2);
  }
  *result = res;
  return result->value_type;
}

value_type_t _sub_value_type(s21_decimal num_1, s21_decimal num_2) {
  value_type_t value = 0;
  if (num_2.value_type == s21_INFINITY) {
    value = s21_NEGATIVE_INFINITY;
  } else if (num_2.value_type == s21_NEGATIVE_INFINITY) {
    value = s21_INFINITY;
  } else {
    value = num_1.value_type;
  }
  return value;
}

int _sub_res_is_NAN(s21_decimal num_1, s21_decimal num_2) {
  return num_1.value_type == s21_NAN || num_2.value_type == s21_NAN ||
         (num_1.value_type == s21_INFINITY &&
          num_2.value_type == s21_INFINITY) ||
         (num_2.value_type == s21_NEGATIVE_INFINITY &&
          num_1.value_type == s21_NEGATIVE_INFINITY);
}
