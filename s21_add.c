#include "s21_utils.h"

int s21_add(s21_decimal num_1, s21_decimal num_2, s21_decimal* result) {
  s21_decimal res = {{0, 0, 0, 0}, 0};
  *result = res;
  int exp_1 = get_exp(num_1);
  int exp_2 = get_exp(num_2);
  value_type_t value_type_1 = num_1.value_type, value_type_2 = num_2.value_type,
               value_type_result = 0;
  check_zeros_exp_and_sign(&num_1, &num_2, &value_type_1, &value_type_2);
  set_exp(&num_1, exp_1);
  set_exp(&num_2, exp_2);
  if (!value_type_1 && !value_type_2) {
    value_type_result = _add_decimal(num_1, num_2, result);
  } else if (_add_res_is_NAN(value_type_1, value_type_2)) {
    result->value_type = s21_NAN;
    value_type_result = s21_NAN;
  } else {
    result->value_type = _add_value_type(value_type_1, value_type_2);
    value_type_result = _add_value_type(value_type_1, value_type_2);
    ;
  }
  return value_type_result;
}
value_type_t _add_value_type(value_type_t num_1, value_type_t num_2) {
  return num_1 > num_2 ? num_1 : num_2;
}

int _add_res_is_NAN(value_type_t num_1, value_type_t num_2) {
  return num_1 == s21_NAN || num_2 == s21_NAN ||
         (num_1 == s21_INFINITY && num_2 == s21_NEGATIVE_INFINITY) ||
         (num_2 == s21_INFINITY && num_1 == s21_NEGATIVE_INFINITY);
}
