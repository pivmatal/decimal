#include "s21_dopfunc.h"
#include "s21_utils.h"

int s21_negate(s21_decimal num, s21_decimal* result) {
  *result = num;
  if (result->value_type == s21_NORMAL_VALUE) {
    if (get_sign(*result)) {
      set_sign(result->bits, DECIMAL, '+');
    } else {
      set_sign(result->bits, DECIMAL, '-');
    }
  } else if (result->value_type == s21_INFINITY) {
    result->value_type = s21_NEGATIVE_INFINITY;
    to_zero(result);
  } else if (result->value_type == s21_NEGATIVE_INFINITY) {
    result->value_type = s21_INFINITY;
    to_zero(result);
  }
  return result->value_type > 0 ? 1 : 0;
}

extra_decimal extra_negate(extra_decimal num) {
  if (num.value_type == s21_NORMAL_VALUE) {
    int is_minus = get_extra_sign(num);
    extra_decimal tmp = {0};
    tmp.bits[0] = 1;
    int remember_exp = get_extra_exp(num);
    set_extra_exp(&num, 0);
    if (is_not_zero_extra(num, num.value_type)) {
      if (is_minus) {
        set_sign(num.bits, EXTRA_DECIMAL, '+');

        num = _sub_extra(num, tmp);
        for (int i = 0; i < EXTRA_DECIMAL - 1; i++) {
          num.bits[i] = ~num.bits[i];
        }

      } else {
        for (int i = 0; i < EXTRA_DECIMAL - 1; i++) {
          num.bits[i] = ~num.bits[i];
        }
        num = _add_extra(num, tmp);

        set_sign(num.bits, EXTRA_DECIMAL, '-');
      }

      set_extra_exp(&num, remember_exp);
    } else {
      set_sign(num.bits, EXTRA_DECIMAL, '+');
    }

  } else if (num.value_type == s21_INFINITY) {
    num.value_type = s21_NEGATIVE_INFINITY;
  } else if (num.value_type == s21_NEGATIVE_INFINITY) {
    num.value_type = s21_INFINITY;
  }
  return num;
}
