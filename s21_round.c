#include "s21_decimal.h"
#include "s21_dopfunc.h"

int s21_round(s21_decimal value, s21_decimal *result) {
  if (result == NULL || value.value_type > 0) return 1;

  for (int i = 0; i < 4; i++) {
    result->bits[i] = value.bits[i];
  }

  int scale = get_scale_d(&value);
  if (scale) {
    result->bits[3] = value.bits[3] & 0x80000000;

    float first_mod = 0;
    if (scale > 1) {
      first_mod = mod_by_int(value, 10) * 0.1;
    }

    for (int j = scale; j > 1; j--) {
      div_by_10(result);
    }

    float mod = mod_by_int(*result, 10);
    mod += first_mod;

    int is_rounded = mod >= 5 ? 1 : 0;

    if (s21_truncate(value, result) == 0 && is_rounded == 1) {
      add_int(*result, 1, result);
    }
  }

  return 0;
}
