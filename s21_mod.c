#include "s21_utils.h"

int s21_mod(s21_decimal num_1, s21_decimal num_2, s21_decimal* result) {
  s21_decimal res = {{0, 0, 0, 0}, 0};
  value_type_t value_type_1 = num_1.value_type, value_type_2 = num_2.value_type,
               value_type_result;
  check_zeros_exp_and_sign(&num_1, &num_2, &value_type_1, &value_type_2);
  value_type_result = _mod_res_is_NAN(value_type_1, value_type_2, num_2);
  if ((!value_type_1 && !value_type_2) && is_not_zero(num_2, value_type_2)) {
    value_type_result = _mod_decimal(num_1, num_2, &res);
  } else if (value_type_result) {
    res.value_type = s21_NAN;
    value_type_result = s21_NAN;
  } else {
    s21_decimal negate_result;
    s21_negate(num_1, &negate_result);
    res = (!is_not_zero(num_1, value_type_1) && get_sign(num_1)) ? negate_result
                                                                 : num_1;
  }
  *result = res;
  return value_type_result;
}

int _mod_res_is_NAN(value_type_t value_type_1, value_type_t value_type_2,
                    s21_decimal num_2) {
  return value_type_1 == s21_NAN || value_type_2 == s21_NAN ||
         value_type_1 == s21_INFINITY ||
         value_type_1 == s21_NEGATIVE_INFINITY ||
         !is_not_zero(num_2, value_type_2);
}
