#include "s21_utils.h"

int s21_mul(s21_decimal num_1, s21_decimal num_2, s21_decimal* result) {
  s21_decimal res = {{0, 0, 0, 0}, 0};
  value_type_t value_type_1 = num_1.value_type, value_type_2 = num_2.value_type,
               value_type_result = 0;
  check_zeros_exp_and_sign(&num_1, &num_2, &value_type_1, &value_type_2);

  if (!value_type_1 && !value_type_2 &&
      _buffer_is_not_zero(num_1.bits, DECIMAL) &&
      _buffer_is_not_zero(num_2.bits, DECIMAL)) {
    _mul_decimal(num_1, num_2, &res);
  } else if (_mul_res_is_NAN(num_1, num_2)) {
    res.value_type = s21_NAN;
    value_type_result = s21_NAN;
  } else if (!is_not_zero(num_1, value_type_1) ||
             !is_not_zero(num_2, value_type_2)) {
    s21_decimal negate_result = {{0, 0, 0, 0}, 0};
    s21_negate(res, &negate_result);
    res = (get_sign(num_1) ^ get_sign(num_2)) ? negate_result : res;
  } else {
    res.value_type = _mul_value_type(num_1, num_2);
    value_type_result = _mul_value_type(num_1, num_2);
  }
  *result = res;
  return value_type_result;
}

int _mul_res_is_NAN(s21_decimal num_1, s21_decimal num_2) {
  return num_1.value_type == s21_NAN || num_2.value_type == s21_NAN ||
         ((num_1.value_type == s21_INFINITY ||
           num_1.value_type == s21_NEGATIVE_INFINITY) &&
          !is_not_zero(num_2, num_2.value_type)) ||
         ((num_2.value_type == s21_INFINITY ||
           num_2.value_type == s21_NEGATIVE_INFINITY) &&
          !is_not_zero(num_1, num_1.value_type));
}

value_type_t _mul_value_type(s21_decimal num_1, s21_decimal num_2) {
  value_type_t value = 0;
  if (num_1.value_type == s21_INFINITY) {
    if (get_sign(num_2) || num_2.value_type == s21_NEGATIVE_INFINITY) {
      value = s21_NEGATIVE_INFINITY;
    } else {
      value = s21_INFINITY;
    }
  } else if (num_1.value_type == s21_NEGATIVE_INFINITY) {
    if (get_sign(num_2) || num_2.value_type == s21_NEGATIVE_INFINITY) {
      value = s21_INFINITY;
    } else {
      value = s21_NEGATIVE_INFINITY;
    }
  } else if (!num_1.value_type) {
    value = _mul_value_type(num_2, num_1);
  }
  return value;
}
