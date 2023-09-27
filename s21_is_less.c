#include "s21_decimal.h"
#include "s21_dopfunc.h"

int s21_is_less(s21_decimal decimal1, s21_decimal decimal2) {
  s21_decimal arg1 = decimal1, arg2 = decimal2;
  int res = 0, flag = 0;
  int degree1 = get_scale_d(&arg1), degree2 = get_scale_d(&arg2);
  int znak1 = get_bit_d(&arg1, 127), znak2 = get_bit_d(&arg2, 127);

  for (int j = 95; j >= 0; j--) {
    if (get_bit_d(&arg1, j) == 0 && get_bit_d(&arg2, j) == 0) {
      flag = 1;
      res = 0;
    } else {
      flag = 0;
      break;
    }
  }

  if (flag == 0) {
    if (znak1 > znak2) {
      res = 1;
    } else if (znak1 < znak2) {
      res = 0;
    } else {
      if (znak1 == znak2) {
        int z = 0, k = 1;
        if (znak1 == 1) {
          k = 0;
        }

        norm_dec(&arg1);
        norm_dec(&arg2);

        while (z == 0) {
          if ((degree1 >= degree2) && comp_mant(&arg1, &arg2) == -1) {
            if (k == 0) {
              res = 0;
            } else {
              res = 1;
            }
            z = 1;
          } else if ((degree1 > degree2) && comp_mant(&arg1, &arg2) == 1) {
            if (umnon10(&arg2)) {
              mul_on_10(&arg2);
              degree2++;
            } else {
              z = 1;
              if (k == 0) {
                res = 0;
              } else {
                res = 1;
              }
            }
          } else if ((degree1 < degree2) && comp_mant(&arg1, &arg2) != 1) {
            if (umnon10(&arg1)) {
              mul_on_10(&arg1);
              degree1++;
            } else {
              z = 1;
              if (k == 0) {
                res = 1;
              } else {
                res = 0;
              }
            }
          } else if ((degree1 == degree2) && comp_mant(&arg1, &arg2) == 0) {
            z = 1;
            res = 0;
          } else if ((degree1 > degree2) && comp_mant(&arg1, &arg2) == 0) {
            res = 1;
            z = 1;
          } else {
            if (k == 0) {
              res = 1;
            } else {
              res = 0;
            }
            z = 1;
          }
        }
      } else {
        res = 0;
      }
    }
  }
  return res;
}
