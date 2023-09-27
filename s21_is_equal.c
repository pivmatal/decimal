#include "s21_decimal.h"
#include "s21_dopfunc.h"

int s21_is_equal(s21_decimal decimal1, s21_decimal decimal2) {
  s21_decimal arg1 = decimal1, arg2 = decimal2;
  int res = 0;

  int degree1 = get_scale_d(&arg1);
  int degree2 = get_scale_d(&arg2);

  for (int j = 0; j <= 95; j++) {
    int arg1_j = get_bit_d(&arg1, j);
    int arg2_j = get_bit_d(&arg2, j);

    if (arg1_j == 0 && arg2_j == 0) {
      res = 1;
    } else {
      res = 0;
      break;
    }
  }

  if (res == 0) {
    if (get_bit_d(&arg1, 127) == get_bit_d(&arg2, 127)) {
      while (delon10(&arg1) == 1) {
        div_by_10(&arg1);
        degree1--;
      }

      while (delon10(&arg2) == 1) {
        div_by_10(&arg2);
        degree2--;
      }

      if (degree1 == degree2) {
        for (int i = 95; i >= 0; i--) {
          if (get_bit_d(&arg1, i) == get_bit_d(&arg2, i)) {
            res = 1;
          } else {
            res = 0;
            break;
          }
        }
      }
    }
  }
  return res;
}
