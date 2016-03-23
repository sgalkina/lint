#include "lint.h"
#include <iostream>
#include <cmath>
#include <cassert>
#include <stdint.h>

namespace apa {

const Basetype BASE = UINT16_MAX;
const char MINUS = '-';
const char PLUS = '+';

std::vector<Basetype> number_to_digits(Basetype number) {
  std::vector<Basetype> digits_;
  while (number >= BASE) {
    digits_.push_back(number % BASE);
    number /= BASE;
  }
  digits_.push_back(number);
  return digits_;
}

Basetype digits_to_number(const std::vector<Basetype> & digits) {
  Basetype result = 0;
  for (size_t i = 0; i < digits.size(); ++i) {
    result += std::pow(BASE, i) * digits[i];
  }
  return result;
}

lint::lint()
  : digits_()
  , is_negative_(false)
  {}

lint::lint(const std::string &string)
  : digits_()
  , is_negative_(false)
  {
  int offset = string[0] == MINUS || string[0] == PLUS;
  *this = 0;
  lint mul = 1;
  for (auto it = string.rbegin(); it != string.rend() - offset; ++it) {
    *this += mul*(*it - '0');
    mul *= 10;
  }
  pop_leading_zeros();
  is_negative_ = (string[0] == MINUS) && !is_null();
}

lint::lint(int number)
  : digits_(number_to_digits(std::abs(number)))
  , is_negative_(number < 0)
  {}

lint::lint(long number)
  : digits_(number_to_digits(std::abs(number)))
  , is_negative_(number < 0)
  {}

lint::lint(long long number)
  : digits_(number_to_digits(std::abs(number)))
  , is_negative_(number < 0)
  {}

lint::lint(Basetype number)
  : digits_(number_to_digits(number))
  , is_negative_(false)
  {}

lint::lint(double number)
  : digits_(number_to_digits((long long) std::abs(number)))
  , is_negative_(number < 0)
  {}

lint::lint(const std::vector<Basetype> & digits, bool is_negative)
  : digits_(digits)
  , is_negative_(is_negative) {}

lint lint::operator-() const {
  return lint(digits_, !is_negative_);
}

lint lint::operator+() const {
  return lint(*this);
}

lint abs(const lint & src) {
  return lint(src.digits_, false);
}

void lint::inplace_minus() {
  is_negative_ = !is_negative_;
}

lint lint::convert_to_base(Basetype base) const {
  std::vector<Basetype> digits;
  lint number = *this;
  number.is_negative_ = false;
  while (number >= base) {
    digits.push_back((number % base).digits_[0]);
    number /= base;
  }
  digits.push_back(number.digits_[0]);
  lint result(digits, is_negative_);
  return result;
}

std::string lint::to_string() const {
  std::string result;
  if (is_negative_) result.push_back(MINUS);
  lint base10 = convert_to_base(10);
  for (auto it = base10.digits_.rbegin(); it != base10.digits_.rend(); ++it) {
    result.push_back(*it + '0');
  }
  return result;
}

template <typename T>
T lint::cast() const {
  T number = digits_to_number(digits_);
  if (is_negative_) number *= -1;
  return number;
}

lint::operator int() const {
  return cast<int>();
}

lint::operator long() const {
  return cast<long>();
}

lint::operator long long() const {
  return cast<long long>();
}

lint::operator bool() const {
  return !is_null();
}

void lint::pop_leading_zeros() {
  while (digits_.back() == 0 && digits_.size() > 1)
    digits_.pop_back();
}

void lint::add(const lint &src) {
  while (digits_.size() < src.digits_.size())
    digits_.push_back(0);
  Basetype rem = 0;
  for (size_t i = 0; i < digits_.size(); ++i) {
    Basetype to_add = src.digits_.size() > i ? src.digits_[i] : 0;
    digits_[i] += to_add + rem;
    rem = digits_[i] / BASE;
    digits_[i] %= BASE;
  }
  if (rem) digits_.push_back(rem);
}

void lint::subtract(const lint &src) {
  if (abs(*this) < abs(src)) {  // TODO: can be done without new objects' creation
    *this = src - *this;
    inplace_minus();
  }
  else {
    Basetype rem = 0;
    for (size_t i = 0; i < digits_.size(); ++i) {
      Basetype to_subtrack = src.digits_.size() > i ? src.digits_[i] : 0;
      to_subtrack += rem;
      rem = (digits_[i] < to_subtrack);
      if (rem) {
        digits_[i] += BASE;
      }
      digits_[i] -= to_subtrack;
    }
    pop_leading_zeros();
  }
}

void lint::multiply(const lint &src) {  // slow multiplication
  is_negative_ = is_negative_ != src.is_negative_;
  std::vector<Basetype> result;
  Basetype carry = 0;
  for (size_t i = 0; i < digits_.size(); ++i) {
    for (size_t j = 0; j < src.digits_.size(); ++j) {
      if (result.size() <= i + j)
        result.push_back(0);
      result[i+j] += (digits_[i] * src.digits_[j]) + carry;
      carry = result[i+j] / BASE;
      result[i+j] %= BASE;
    }
    if (carry) {
      result.push_back(carry);
      carry = 0;
    }
  }
  digits_ = result;
  pop_leading_zeros();
}

void lint::divide(const lint &src) {  // slow division
  if (src.is_null()) throw 0;
  is_negative_ = is_negative_ != src.is_negative_;
  lint pos_src(src.digits_, false);
  std::vector<Basetype> result;
  lint current_number;
  Basetype rem = 0;
  for (size_t i = digits_.size(); i > 0; --i) {
    current_number.digits_.insert(current_number.digits_.begin(), digits_[i-1]);
    current_number.pop_leading_zeros();
    while (current_number >= pos_src) {
      current_number -= pos_src;
      rem++;
    }
    result.insert(result.begin(), rem);
    rem = 0;
  }
  if (result.empty()) {
    result.push_back(0);
  }
  digits_ = result;
  if (is_negative_ && !current_number.is_null()) *this -= 1;
  pop_leading_zeros();
}

lint & lint::operator*=(const lint &src) {
  multiply(src);
  if (is_null()) is_negative_ = false;
  return *this;
}

lint & lint::operator/=(const lint &src) {
  divide(src);
  if (is_null()) is_negative_ = false;
  return *this;
}

lint & lint::operator%=(const lint &src) {
  *this -= src*(*this / src);
  return *this;
}

lint & lint::operator+=(const lint &src) {
  if (is_negative_ == src.is_negative_)
    add(src);
  else
    subtract(-src);
  if (is_null()) is_negative_ = false;
  return *this;
}

lint operator+(lint a, const lint &b) {
  return a += b;
}

lint & lint::operator-=(const lint &src) {
  *this += -src;
  return *this;
}

lint operator-(lint a, const lint &b) {
  return a -= b;
}

lint operator*(lint a, const lint &b) {
  return a *= b;
}

lint operator/(lint a, const lint &b) {
  return a /= b;
}

lint operator%(lint a, const lint &b) {
  return a %= b;
}

lint & lint::operator++() {
  *this += 1;
  return *this;
}

lint & lint::operator--() {
  *this -= 1;
  return *this;
}

lint lint::operator++(int) {
  lint tmp(digits_, is_negative_);
  operator++();
  return tmp;
}

lint lint::operator--(int) {
  lint tmp(digits_, is_negative_);
  operator--();
  return tmp;
}

std::ostream & operator<<(std::ostream & out, const lint & number) {
  out << number.to_string();
  return out;
}

std::istream & operator>>(std::istream & in, lint & number) {
  std::string s;
  in >> s;
  number = lint(s);
  return in;
}

lint pow_iterative(lint x, unsigned long long n) {
  if (n == 0) return 1;
  lint rem(1);
  while (n > 1) {
    if (n % 2 == 0) {
      x *= x;
      n /= 2;
    }
    else {
      rem *= x;
      x *= x;
      n  = (n - 1)/2;
    }
  }
  return x*rem;
}

lint pow(const lint & x, unsigned long long n) {
  if (n == 0) return 1;
  return (n%2 ? x : 1)*pow(x*x, n/2);
}

bool lint::operator<(const lint &src) const {
  if (is_negative_) {
    if (src.is_negative_) return -src < -*this;
    return true;
  }
  if (src.is_negative_) return false;
  if (digits_.size() != src.digits_.size()) {
    return digits_.size() < src.digits_.size();
  }
  for (size_t i = digits_.size(); i > 0; --i) {
    if (digits_[i-1] != src.digits_[i-1]) {
      return digits_[i-1] < src.digits_[i-1];
    }
  }
  return false;
}

bool lint::is_null() const {
  return digits_.size() == 1 && digits_[0] == 0;
}

bool lint::operator==(const lint &src) const {
  return is_negative_ == src.is_negative_ && digits_ == src.digits_;
}

} // namespace apa
