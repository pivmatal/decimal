#include "s21_decimal.h"
#include "s21_dopfunc.h"

int s21_floor(s21_decimal value, s21_decimal *result) {
  if (result == NULL) return 1;

  int status = 0;
  if (value.bits[3] & 0x80000000) {
    status = s21_truncate(value, result);

    int scale = get_scale_d(&value);
    if (!is_decimal_zero(value) && scale && !is_remainder_zero(value, scale)) {
      add_int(*result, 1, result);
    }
  } else {
    status = s21_truncate(value, result);
  }

  return status;
}
