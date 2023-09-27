#include "s21_utils.h"

extra_decimal convert_to_extra_decimal(s21_decimal num, int exp_2,
                                       Operation operation) {
  extra_decimal extra_num = {0};
  int sign = get_sign(num);

  if (sign) s21_negate(num, &num);

  for (int i = 0; i < DECIMAL - 1; i++) {
    extra_num.bits[i] = num.bits[i];
  }
  extra_num.bits[EXTRA_DECIMAL - 1] = num.bits[DECIMAL - 1];

  if (sign) extra_num = extra_negate(extra_num);
  extra_num = normalize_extra(extra_num, exp_2, operation);

  return extra_num;
}

extra_decimal normalize_extra(extra_decimal num, int exp_2,
                              Operation operation) {
  extra_decimal tmp_10 = {0};
  tmp_10.bits[0] = 10;

  switch (operation) {
    case ADD:
      for (int i = exp_2 - get_extra_exp(num); i > 0; i--) {
        num = _mul_extra(num, tmp_10);
      }
      break;
    case DIV:
      for (int i = exp_2 - get_extra_exp(num); i > 0; i--) {
        num = _mul_extra(num, tmp_10);
      }
      break;
    case MOD:
      for (int i = exp_2 - get_extra_exp(num); i > 0; i--) {
        num = _mul_extra(num, tmp_10);
      }
      break;
    case MUL:
      break;
    default:
      break;
  }

  set_extra_exp(&num, 0);

  return num;
}

int convert_from_extra_decimal(extra_decimal extra_num, int exp,
                               value_type_t *value_type, s21_decimal *result) {
  s21_decimal num = {0};
  extra_num = denormalize_extra(extra_num, exp);
  extra_num.value_type = check_decimal_overflow(extra_num);

  if (!extra_num.value_type) {
    int sign = get_extra_sign(extra_num);
    if (sign) {
      extra_num = extra_negate(extra_num);
    }

    for (int i = 0; i < DECIMAL - 1; i++) {
      num.bits[i] = extra_num.bits[i];
    }
    num.bits[DECIMAL - 1] = extra_num.bits[EXTRA_DECIMAL - 1];

    if (sign) {
      s21_negate(num, &num);
    }
  }
  num.value_type = extra_num.value_type;
  *value_type = num.value_type;
  *result = num;
  return *value_type;
}

extra_decimal denormalize_extra(extra_decimal num, int exp) {
  extra_decimal tmp_10 = {0};
  tmp_10.bits[0] = 10;
  extra_decimal tmp = {0};
  tmp.bits[0] = 1;
  exp = get_extra_exp(num) ? get_extra_exp(num) : exp;
  set_extra_exp(&num, 0);
  int need_bank_round = 0;

  while (exp > 0 && check_decimal_overflow(_div_main(num, tmp, 0))) {
    exp--;
    tmp = _mul_extra(tmp, tmp_10);
    need_bank_round = 1;
  }

  while (exp > 28) {
    exp--;
    need_bank_round = 1;
    tmp = _mul_extra(tmp, tmp_10);
  }

  if (need_bank_round) {
    num = bank_round(num, tmp, tmp_10, exp);
  }
  set_extra_exp(&num, exp);

  if (!is_not_zero_extra(num, num.value_type)) set_extra_exp(&num, 0);

  return num;
}

extra_decimal bank_round(extra_decimal num, extra_decimal tmp,
                         extra_decimal tmp_10, int exp) {
  int sign = get_extra_sign(num);
  extra_decimal mod_after_dot = _mod_extra(
      _div_without_remaind(num, _div_without_remaind(tmp, tmp_10)), tmp_10);
  num = _div_without_remaind(num, tmp);
  extra_decimal mod_before_dot = _mod_extra(num, tmp_10);

  if (sign) {
    mod_before_dot = extra_negate(mod_before_dot);
    mod_after_dot = extra_negate(mod_after_dot);
  }

  extra_decimal tmp_1 = {0};
  tmp_1.bits[0] = 1;
  if (mod_after_dot.bits[0] > 5) {
    num = sign ? _sub_extra(num, tmp_1) : _add_extra(num, tmp_1);
  } else if (mod_after_dot.bits[0] == 5) {
    if ((!exp && mod_before_dot.bits[0] % 2) || exp)
      num = sign ? _sub_extra(num, tmp_1) : _add_extra(num, tmp_1);
  }
  return num;
}

value_type_t check_decimal_overflow(extra_decimal num) {
  int sign = get_extra_sign(num);
  if (sign) num = extra_negate(num);

  for (int i = DECIMAL - 1; i < EXTRA_DECIMAL - 1; i++) {
    if (!get_extra_sign(num) && num.bits[i]) {
      num.value_type = s21_INFINITY;
    }
  }
  if (sign) num = extra_negate(num);

  return num.value_type;
}

extra_decimal _div_extra(extra_decimal num, extra_decimal devisor) {
  extra_decimal res = {0};
  extra_decimal tmp_10 = {0};
  tmp_10.bits[0] = 10;
  int exp = 0;
  extra_decimal tmp = _mod_extra(num, devisor);
  while (is_not_zero_extra(tmp, tmp.value_type) &&
         !_mul_extra(num, tmp_10).value_type) {
    num = _mul_extra(num, tmp_10);
    exp++;
    tmp = _mod_extra(num, devisor);
  }

  res = _div_without_remaind(num, devisor);
  set_extra_exp(&res, exp);

  return res;
}

int culc_exp(int exp_1, int exp_2, Operation operation) {
  int res = 0;

  switch (operation) {
    case ADD:
      res = (exp_1 > exp_2) ? exp_1 : exp_2;
      break;
    case MUL:
      res = exp_1 + exp_2;
      break;
    case DIV:
      res = (exp_1 > exp_2) ? (exp_1 - exp_2) : 0;
      break;
    case MOD:
      res = (exp_1 > exp_2) ? exp_1 : exp_2;
      break;
    default:
      break;
  }

  return res;
}

extra_decimal _add_extra(extra_decimal num_1, extra_decimal num_2) {
  extra_decimal res = {0};

  int sign_1 = get_extra_sign(num_1);
  int sign_2 = get_extra_sign(num_2);
  int add_tmp = 0;

  for (int i = 0; i < EXTRA_DECIMAL - 1; i++) {
    res.bits[i] = num_1.bits[i] + num_2.bits[i] + add_tmp;
    add_tmp = ((long long)num_1.bits[i] + (long long)num_2.bits[i] +
                   (long long)add_tmp >
               (long long)0xffffffff);
  }

  *(3 + (char *)&res.bits[EXTRA_DECIMAL - 1]) =
      *(3 + (char *)&num_1.bits[EXTRA_DECIMAL - 1]) +
      *(3 + (char *)&num_2.bits[EXTRA_DECIMAL - 1]) + (add_tmp ? 0x80 : 0);

  if (sign_1 == sign_2 && sign_1 != get_extra_sign(res)) {
    res.value_type = sign_1 ? s21_NEGATIVE_INFINITY : s21_INFINITY;
  }
  return res;
}

extra_decimal _mul_extra(extra_decimal num_1, extra_decimal num_2) {
  extra_decimal res = {0};

  int need_minus = get_extra_sign(num_1) != get_extra_sign(num_2);
  if (get_extra_sign(num_1)) num_1 = extra_negate(num_1);
  if (get_extra_sign(num_2)) num_2 = extra_negate(num_2);
  int max = (EXTRA_DECIMAL - 1) * 32 - 1;

  for (int i = 0; i < max; i++) {
    if (get_bit(num_2.bits, i)) {
      extra_decimal tmp = num_1;
      if (!shift_decimal_bit(tmp.bits, EXTRA_DECIMAL, 1, i)) {
        res = _add_extra(res, tmp);
      } else {
        res.value_type = s21_INFINITY;
      }
    }
    if (res.value_type) {
      break;
    }
  }
  if (need_minus) res = extra_negate(res);

  return res;
}

extra_decimal _div_without_remaind(extra_decimal num, extra_decimal devisor) {
  extra_decimal res = _div_main(num, devisor, 0);
  return res;
}

extra_decimal _div_main(extra_decimal num, extra_decimal devisor,
                        int need_mod) {
  extra_decimal res = {0};
  extra_decimal tmp = {0};
  tmp.bits[0] = 1;

  int need_minus = get_extra_sign(num) != get_extra_sign(devisor);
  if (get_extra_sign(num)) num = extra_negate(num);
  if (get_extra_sign(devisor)) devisor = extra_negate(devisor);
  int num_1_first_bit = extra_find_first_bit(num);
  int num_2_first_bit = extra_find_first_bit(devisor);
  shift_decimal_bit(devisor.bits, EXTRA_DECIMAL, 1,
                    num_1_first_bit - num_2_first_bit);

  for (int i = 1, is_shift_once = 0;
       extra_find_first_bit(devisor) >= num_2_first_bit; i++) {
    if (!is_not_zero_extra(num, num.value_type) &&
        !is_not_zero_extra(devisor, devisor.value_type))
      break;
    if (!extra_is_less(num, devisor)) {
      num = _sub_extra(num, devisor);
      shift_decimal_bit(res.bits, EXTRA_DECIMAL, 1, 1);
      res = _add_extra(res, tmp);
      is_shift_once = 0;
    } else {
      if (is_shift_once) {
        shift_decimal_bit(res.bits, EXTRA_DECIMAL, 1, 1);
      }
      shift_decimal_bit(devisor.bits, EXTRA_DECIMAL, 0, 1);
      if (!is_shift_once) {
        is_shift_once = 1;
      }
    }
  }
  if (need_mod) res = num;
  if (need_minus) res = extra_negate(res);
  return res;
}

extra_decimal _mod_extra(extra_decimal num, extra_decimal devisor) {
  int change_sign = (get_extra_sign(num) && get_extra_sign(devisor)) ||
                    (!get_extra_sign(num) && get_extra_sign(devisor));

  extra_decimal res = _div_main(num, devisor, 1);

  if (change_sign) {
    res = extra_negate(res);
  }
  return res;
}

extra_decimal _sub_extra(extra_decimal num_1, extra_decimal num_2) {
  extra_decimal res = _add_extra(num_1, extra_negate(num_2));
  return res;
}

int _div_normal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result) {
  extra_decimal tmp =
      _div_main(convert_to_extra_decimal(num_1, get_exp(num_2), DIV),
                convert_to_extra_decimal(num_2, get_exp(num_1), DIV), 0);
  value_type_t value_type = 0;
  s21_decimal res;
  convert_from_extra_decimal(tmp, culc_exp(get_exp(num_1), get_exp(num_2), DIV),
                             &value_type, &res);
  *result = res;
  return value_type;
}

int _div_decimal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result) {
  extra_decimal tmp =
      _div_extra(convert_to_extra_decimal(num_1, get_exp(num_2), DIV),
                 convert_to_extra_decimal(num_2, get_exp(num_1), DIV));
  value_type_t value_type = 0;
  convert_from_extra_decimal(tmp, culc_exp(get_exp(num_1), get_exp(num_2), DIV),
                             &value_type, result);

  return value_type;
}

int _mod_decimal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result) {
  *result = num_1;
  s21_decimal negate_result_1, negate_result_2;
  s21_negate(num_1, &negate_result_1);
  s21_negate(num_2, &negate_result_2);
  extra_decimal tmp =
      _mod_extra(convert_to_extra_decimal(num_1, get_exp(num_2), MOD),
                 convert_to_extra_decimal(num_2, get_exp(num_1), MOD));
  value_type_t value_type = 0;
  if (check_need_div(get_sign(num_1) ? negate_result_1 : num_1,
                     get_sign(num_2) ? negate_result_2 : num_2)) {
    convert_from_extra_decimal(tmp,
                               culc_exp(get_exp(num_1), get_exp(num_2), MOD),
                               &value_type, result);

    if (!is_not_zero(*result, value_type) &&
        is_not_zero(num_1, num_1.value_type) && get_sign(num_1)) {
      s21_negate(*result, result);
    }
  }
  return value_type;
}

int _mul_decimal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result) {
  extra_decimal tmp =
      _mul_extra(convert_to_extra_decimal(num_1, get_exp(num_2), MUL),
                 convert_to_extra_decimal(num_2, get_exp(num_1), MUL));
  value_type_t value_type = 0;
  convert_from_extra_decimal(tmp, culc_exp(get_exp(num_1), get_exp(num_2), MUL),
                             &value_type, result);
  return value_type;
}

int _add_decimal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result) {
  extra_decimal tmp =
      _add_extra(convert_to_extra_decimal(num_1, get_exp(num_2), ADD),
                 convert_to_extra_decimal(num_2, get_exp(num_1), ADD));
  value_type_t value_type = 0;
  convert_from_extra_decimal(tmp, culc_exp(get_exp(num_1), get_exp(num_2), ADD),
                             &value_type, result);

  return value_type;
}

int _sub_decimal(s21_decimal num_1, s21_decimal num_2, s21_decimal *result) {
  // Взятие отрицательного значения num_2
  s21_decimal negate_result;
  s21_negate(num_2, &negate_result);

  s21_add(num_1, negate_result, result);
  return result->value_type;
}

void check_zeros_exp_and_sign(s21_decimal *num_1, s21_decimal *num_2,
                              value_type_t *value_type_1,
                              value_type_t *value_type_2) {
  if (!is_not_zero(*num_1, *value_type_1)) {
    set_exp(num_1, 0);
    set_sign(num_1->bits, DECIMAL, '+');
  }
  if (!is_not_zero(*num_2, *value_type_2)) {
    set_exp(num_2, 0);
    set_sign(num_2->bits, DECIMAL, '+');
  }
  return;
}

int check_need_div(s21_decimal num_1, s21_decimal num_2) {
  return s21_is_less(num_1, num_2) == 0 ? 1 : 0;
}
