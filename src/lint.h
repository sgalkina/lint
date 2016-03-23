#pragma once
#include <vector>
#include <string>
#include <iostream>

namespace apa {

using Basetype = uint32_t;

template<typename T> class comparable {
    friend bool operator!=(const T & a, const T & b) { return !(a == b); }
    friend bool operator<=(const T & a, const T & b) { return a == b || a < b; }
    friend bool operator>(const T & a, const T & b) { return !(a <= b); }
    friend bool operator>=(const T & a, const T & b) { return !(a < b); }
};

class lint : comparable<lint> {
  public:
    lint();
    lint(int);
    lint(long);
    lint(long long);
    lint(Basetype);
    explicit lint(double);
    explicit lint(const std::string &string);

    explicit operator int() const;
    explicit operator long() const;
    explicit operator long long() const;
    explicit operator bool() const;
    std::string to_string() const;

    lint(const lint & src) = default;

    lint & operator+=(const lint &src);
    lint & operator-=(const lint &src);
    lint & operator*=(const lint &src);
    lint & operator/=(const lint &src);
    lint & operator%=(const lint &src);
    lint & operator++();
    lint & operator--();
    lint operator++(int);
    lint operator--(int);
    bool operator<(const lint &src) const;
    bool operator==(const lint &src) const;
    lint operator-() const;
    lint operator+() const;
    friend lint abs(const lint &);
  private:
    lint(const std::vector<Basetype> & digits, bool is_negative);
    template <typename T>
    T cast() const;
    void inplace_minus();
    lint convert_to_base(Basetype base) const;
    void add(const lint &src);
    void subtract(const lint &src);
    void multiply(const lint &src);
    void divide(const lint &src);
    bool is_null() const;
    void pop_leading_zeros();
    std::vector<Basetype> digits_;
    bool is_negative_;
};

std::ostream & operator<<(std::ostream &, const lint &);
std::istream & operator>>(std::istream &, lint &);
lint operator+(lint, const lint &);
lint operator-(lint, const lint &);
lint operator*(lint, const lint &);
lint operator/(lint, const lint &);
lint operator%(lint, const lint &);

lint pow_iterative(lint x, unsigned long long n);
lint pow(const lint & x, unsigned long long n);

} // namespace apa
