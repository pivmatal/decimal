#include "s21_decimal.h"
#include "s21_utils.h"

int s21_truncate(s21_decimal num, s21_decimal* result) {
  *result = num;
  unsigned int num1 = get_exp(*result);

  int i = get_sign(*result);
  s21_decimal tmp = {{1, 0, 0, 0}, 0};
  s21_decimal tmp2 = {{10, 0, 0, 0}, 0};
  if (result->value_type || !(num1)) {
    if (result->value_type != 0) {
      to_zero(result);
    }
  } else {
    set_exp(result, 0);
    while (num1 > 0) {
      s21_mul(tmp, tmp2, &tmp);
      num1--;
    }
    _div_normal(*result, tmp, result);
  }
  if (is_not_zero(*result, result->value_type) == 0 && i != 0) {
    s21_negate(*result, result);
  }

  return result->value_type > 0 ? 1 : 0;
}