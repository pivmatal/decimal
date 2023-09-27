#include "s21_dopfunc.h"

int num_to_dec_four(s21_decimal *decimal,
                    int count) {  // переводит в десятичную систему по 4 бита
  int local_sum = 0;
  for (int i = count; i < count + 4; i++) {
    local_sum += pow_2(i) * get_bit_d(decimal, i);
  }
  return local_sum;
}

// получает данные о бите
int get_bit_d(s21_decimal *decimal, int num_bit) {
  int mask = 1 << num_bit % 32;
  int bit = 0;

  if (decimal->bits[num_bit / 32] & mask) {
    bit = 1;
  } else {
    bit = 0;
  }

  return bit;
}

// меняем в указанной позиции число на 1
void set_bit_d(s21_decimal *decimal, int num_bit) {
  decimal->bits[num_bit / 32] |= (1 << num_bit % 32);
}

int add_int(s21_decimal value, int integer, s21_decimal *result) {
  int status = 0;
  unsigned long long buf = 0ull + integer;
  for (int i = 0; i < 3; i++) {
    buf += value.bits[i];
    value.bits[i] = (unsigned)buf;
    buf >>= 32;
  }
  if (!buf) {
    for (int i = 0; i < 4; i++) {
      result->bits[i] = value.bits[i];
    }
  } else {
    status = 1;
  }
  return status;
}

double pow_2(int degree) {
  double result = 1;
  for (int i = 0; i < degree; i++) {
    result *= 2;
  }
  if (degree == 0) {
    result = 1;
  }
  return result;
}

int is_decimal_zero(s21_decimal value) {
  return !(value.bits[0] != 0 || value.bits[1] != 0 || value.bits[2] != 0);
}

int is_remainder_zero(s21_decimal value, int scale) {
  int is_zero = 0;
  if ((float)mod_by_int(value, (int)pow(10.0, scale)) == 0.0) {
    is_zero = 1;
  }
  return is_zero;
}

int div_by_10(s21_decimal *decimal) {
  s21_decimal chast = {0};
  int x = 95;  //  бит старшего разряда
  for (; x < 0 && get_bit_d(decimal, x) == 0; x--) {
  }
  int a = x, local_sum = 0;
  for (int i = 0; i < 4; i++, a--) {
    local_sum += pow_2(3 - i) * get_bit_d(decimal, a);
  }
  a++;
  for (int i = a; i >= 0; i--) {
    if (local_sum >= 10) {
      set_bit_d(&chast, i);
      local_sum = local_sum % 10;
    }
    a--;
    if (a >= 0) local_sum = local_sum * 2 + get_bit_d(decimal, a);
  }
  if (get_bit_d(decimal, 127) == 1) set_bit_d(&chast, 127);
  *decimal = chast;
  return 0;
}

// Вычисление степени
int get_scale_d(s21_decimal *decimal) {
  int local_sum = 0;

  for (int i = 112; i <= 118; i++) {
    local_sum += pow(2.0, i - 112) * get_bit_d(decimal, i);
  }

  return local_sum;
}

int comp_mant(s21_decimal *decimal1,
              s21_decimal *decimal2) {  // сравнение
                                        // мантисс
                                        // decimal1 > decimal2 = 1
                                        // decimal1 < decimal2 = -1
                                        // decimal1 == decimal2 = 0
  int res = 2;
  for (int i = 95; i >= 0; i--) {
    if (get_bit_d(decimal1, i) == get_bit_d(decimal2, i)) {
      res = 0;
    } else if (get_bit_d(decimal1, i) > get_bit_d(decimal2, i)) {
      res = 1;
      break;
    } else if (get_bit_d(decimal1, i) < get_bit_d(decimal2, i)) {
      res = -1;
      break;
    }
  }
  return res;
}

int delon10(s21_decimal *decimal) {  // проверка деления на 10
  int res = 0;
  int zero = 0;

  for (int j = 0; j <= 95; j++) {
    if (get_bit_d(decimal, j) == 0) {
      zero = 1;
    } else {
      zero = 0;
      break;
    }
  }
  if (zero == 0) {
    if (get_bit_d(decimal, 0) == 0) {
      int local_sum = 0;
      for (int i = 1; i < 96;) {
        local_sum += num_to_dec_four(decimal, i);
        i += 4;
      }
      if (local_sum % 10 == 0) {
        res = 1;
      }
    }
  }
  return res;
}

int on_10(s21_decimal *decimal) {
  int ostatok = 1;
  if (get_bit_d(decimal, 0) == 0) {
    int local_sum = 0;
    for (int x = 1; x <= 92; x += 4) {
      for (int y = 0; y < 4; y++) {
        local_sum += pow(2.0, y) * get_bit_d(decimal, x + y);
      }
    }

    if (local_sum != 0) {
      ostatok = local_sum % 5;
    }
  }
  return ostatok;
}

int mod_by_int(s21_decimal value, int integer) {
  unsigned long long buf = 0ull;
  unsigned long long mod = 0;
  for (int i = 2; i >= 0; i--) {
    mod = (buf + value.bits[i]) % integer;

    buf = mod << 32;
  }
  return mod;
}

void norm_dec(s21_decimal *decimal) {
  if (on_10(decimal) == 0) {
    s21_decimal temp = *decimal;
    int scale = get_scale_d(&temp);

    while (div_by_10(&temp) == 1 && scale > 0) {
      div_by_10(&temp);
      scale--;
    }
    temp.bits[3] = scale << 16;
    if (get_bit_d(decimal, 127) == 1) {
      set_bit_d(&temp, 127);
    }
    *decimal = temp;
  }
}

int umnon10(s21_decimal *decimal) {  // проверка умножения на 10
  int res = 0;
  int local_sum = 0;
  for (int i = 92; i < 96; i++) {
    local_sum += get_bit_d(decimal, i);
  }
  if (local_sum == 0) {
    res = 1;
  }

  return res;
}

void decimal_shift_left(s21_decimal *decimal) {
  decimal->bits[2] = decimal->bits[2] << 1;
  if (get_bit_d(decimal, 63)) set_bit_d(decimal, 64);
  decimal->bits[1] = decimal->bits[1] << 1;
  if (get_bit_d(decimal, 31)) set_bit_d(decimal, 32);
  decimal->bits[0] = decimal->bits[0] << 1;
}

void mul_on_10(s21_decimal *decimal) {
  int temp = 0;
  s21_decimal arg1 = *decimal, arg2 = {0}, local_sum = {0};
  local_sum.bits[3] = decimal->bits[3];
  decimal_shift_left(&arg1);
  arg2 = arg1;
  decimal_shift_left(&arg1);
  decimal_shift_left(&arg1);
  for (int i = 0; i < 96; i++) {
    switch (get_bit_d(&arg1, i) + get_bit_d(&arg2, i) + temp) {
      case 0:
        temp = 0;
        break;
      case 1:
        set_bit_d(&local_sum, i);
        temp = 0;
        break;
      case 2:
        temp = 1;
        break;
      case 3:
        set_bit_d(&local_sum, i);
        temp = 1;
        break;
    }
  }
  *decimal = local_sum;
}
