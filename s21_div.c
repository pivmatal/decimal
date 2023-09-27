#include "s21_utils.h"

int s21_div(s21_decimal num_1, s21_decimal num_2, s21_decimal *result) {
  s21_decimal res = {{0, 0, 0, 0}, 0};
  value_type_t value_type_1 = num_1.value_type, value_type_2 = num_2.value_type,
               value_type_result = s21_NORMAL_VALUE;
  check_zeros_exp_and_sign(&num_1, &num_2, &value_type_1, &value_type_2);

  if ((!value_type_1 && !value_type_2) &&
      (is_not_zero(num_1, value_type_1) && is_not_zero(num_2, value_type_2))) {
    value_type_result = _div_decimal(num_1, num_2, &res);
  } else if (_div_res_is_NAN(value_type_1, value_type_2, num_1, num_2)) {
    res.value_type = s21_NAN;
    value_type_result = s21_NAN;
  } else if (value_type_2 == s21_INFINITY ||
             value_type_2 == s21_NEGATIVE_INFINITY) {
    if (((!get_sign(num_1) || !is_not_zero(num_1, value_type_1)) &&
         value_type_2 == s21_NEGATIVE_INFINITY) ||
        (get_sign(num_1) && is_not_zero(num_1, value_type_1) &&
         value_type_2 == s21_INFINITY)) {
      s21_negate(res, &res);
    }
  } else if (!is_not_zero(num_1, value_type_1) &&
             is_not_zero(num_2, value_type_2) && !value_type_2) {
    if (get_sign(num_2)) s21_negate(res, &res);
  } else {
    res.value_type = _div_value_type(value_type_1, value_type_2, num_1, num_2);
    result->value_type =
        _div_value_type(value_type_1, value_type_2, num_1, num_2);
  }
  *result = res;
  return value_type_result;
}

int _div_res_is_NAN(value_type_t value_type_1, value_type_t value_type_2,
                    s21_decimal num_1, s21_decimal num_2) {
  return value_type_1 == s21_NAN || value_type_2 == s21_NAN ||
         (!is_not_zero(num_1, value_type_1) &&
          !is_not_zero(num_2, value_type_2)) ||
         (((value_type_1 == s21_INFINITY ||
            value_type_1 == s21_NEGATIVE_INFINITY)) &&
          ((value_type_2 == s21_INFINITY ||
            value_type_2 == s21_NEGATIVE_INFINITY)));
}

value_type_t _div_value_type(value_type_t value_type_1,
                             value_type_t value_type_2, s21_decimal num_1,
                             s21_decimal num_2) {
  value_type_t value = 0;
  if (value_type_1 == s21_NORMAL_VALUE) {
    if (!is_not_zero(num_2, value_type_2)) {
      value = get_sign(num_1) ? s21_NEGATIVE_INFINITY : s21_INFINITY;
    }
  } else if (value_type_1 == s21_INFINITY) {
    value = (get_sign(num_2) && is_not_zero(num_2, value_type_2))
                ? s21_NEGATIVE_INFINITY
                : s21_INFINITY;
  } else if (value_type_1 == s21_NEGATIVE_INFINITY) {
    value = (!get_sign(num_2) || !is_not_zero(num_2, value_type_2))
                ? s21_NEGATIVE_INFINITY
                : s21_INFINITY;
  }

  return value;
}
