#include <iostream>
#include <sstream>
#include <assert.h>
#include "lint.h"

using apa::lint;

// #define RUN_FAILED_COMP_TEST

static void test_construction()
{
    lint x;
    const lint y(x);
    x = y;
    x = x;
    (void)test_construction;
}

static void test_conversions()
{
    int ordinary = 42;
    lint long_int(ordinary);
    long_int = ordinary;
    ordinary = static_cast<int>(long_int); // ok
    std::string s("-15");
    lint z(s);
    bool check = false;
    check = z.to_string() == s;
    (void)test_conversions;
}

static void failed_conversions_test()
{
#ifdef RUN_FAILED_COMP_TEST
    int ordinary = 42;
    std::string str = "-42";
    lint x = 5;
    ordinary = x; // must be compilation error!
    x = str; // must be compilation error!
    str = x; // must be compilation error!
#endif
    (void)failed_conversions_test;
}

static void test_comparison()
{
    lint long_int1 = 2;
    lint long_int2(3.); // explicit constructing from double value
    // all comparisons
    bool check = false;
    lint pi(3.14);
    check = pi < long_int1;
    check = long_int1 != long_int2;
    check = 5 > long_int1;
    check = long_int1 == long_int2;
    check = 5 <= long_int2;
    check = long_int1 >= 6;
    assert(check);
    (void)test_comparison;
}

static void test_check()
{
    lint value = 0;
    if (value)
        std::cout << "non zero value" << std::endl;
    (void)test_check;
}

static void test_inc_n_dec()
{
    lint x = 42;
    lint y = ++x;
    assert(y == 43 && x == 43);
    y = x--;
    assert(y == 43 && x == 42);
    (void)test_inc_n_dec;
}

static void test_op_n_eq()
{
    lint a = 123456;
    const lint b = 789012;
    lint c = a * b;
    a *= b;
    a += b;
    c /= a;
    c -= c;
    a /= c; // division by zero
    (void)test_op_n_eq;
}

static void test_bin_op()
{
    lint a = 123456789;
    const lint b = 987654321;
    lint c = a * b;
    lint pi(3.14);
    lint d = pi + c;
    c = a * b;
    c = a + b;
    c = c / a;
    c = c + (-c);
    c = a / c; // division by zero
    c = +c;
    assert(d); //avoid warning
    (void)test_bin_op;
}

static void test_free_func()
{
    apa::lint a = -42;
    bool check = (-a == abs(a));
    apa::lint c = pow(a, 0);
    check = check && (c == 1);
    assert(check);
    (void)test_free_func;
}

static void test_io()
{
    std::stringstream s;
    const lint a = -123454321;
    s << a;
    lint b;
    s >> b;
    bool check = a == b;
    assert(check);

	std::stringstream s2("+00015");
	lint c;
	s2 >> c;
	assert(c == 15);

    (void)test_io;
}

int main(int argc, char* argv[]) {
  return 0;
}
