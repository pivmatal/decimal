#include "s21_decimal.h"

int s21_is_not_equal(s21_decimal decimal1, s21_decimal decimal2) {
  int res = 0;
  if (s21_is_equal(decimal1, decimal2) == 0) {
    res = 1;
  }
  return res;
}
