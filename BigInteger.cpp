#include "BigInteger.h"
#include <math.h>
#include <string.h>

BigInteger::SignedArray::SignedArray(Array *result, bool negative)
    : m_array(result),
      m_negative(negative) {}

const BigInteger::SignedArray BigInteger::SignedArray::fromCNumber(const BigInteger &number, bool flip_neg)
{
    bool neg = number.m_negative;
    if (flip_neg)
        neg = !number.m_negative;
    return SignedArray(number.m_table, neg);
}

Array *BigInteger::SignedArray::getArray()
{
    return m_array;
}

const Array *BigInteger::SignedArray::getArray() const
{
    return m_array;
}

bool BigInteger::SignedArray::isNegative() const
{
    return m_negative;
}

void BigInteger::collectDigits(int x, int digits_left, Array &target) const
{
    if (digits_left == 0)
        return;

    int digit = x % 10;
    target[target.length() - digits_left] = digit;

    int reduced = floor(1.0 * x / 10);
    collectDigits(reduced, digits_left - 1, target);
}

void BigInteger::borrow(Array &number, int digit_index) const
{
    int next_index = digit_index + 1;
    if (number[next_index] == 0)
        borrow(number, next_index);

    number[next_index] -= 1;
    number[digit_index] += 10;
}

void BigInteger::mulByDigit(const Array &a, int digit, Array &result) const
{
    const int a_len = a.length();
    int carry = 0;

    for (int i = 0; i < result.length(); i++)
    {
        int a_val = 0;
        if (a_len > i)
            a_val = a[i];

        int mul_result = digit * a_val + carry;
        carry = floor(1.f * mul_result / 10.f);
        result[i] = mul_result % 10;
    }
}

int BigInteger::howManyFit(const Array &divident, const Array &divisor) const
{
    Array result(divisor.length() + 1);
    for (int i = 1; i < 10; i++)
    {
        mulByDigit(divisor, i, result);
        int c_res = compare(divident, result);
        if (c_res == 1)
            return i - 1;
        else if (c_res == 0)
            return i;
    }
    return 9;
}

void BigInteger::shiftRight(Array &a) const
{
    for (int i = a.length() - 1; i >= 1; i--)
        a[i] = a[i - 1];
    a[0] = 0;
}

int BigInteger::compare(const Array &a, const Array &b) const
{
    for (int i = std::max(a.length(), b.length()) - 1; i >= 0; i--)
    {
        int a_val = 0, b_val = 0;
        if (a.length() > i)
            a_val = a[i];
        if (b.length() > i)
            b_val = b[i];

        if (a_val > b_val)
            return -1;
        else if (a_val < b_val)
            return 1;
    }
    return 0;
}

void BigInteger::add(const Array &a, const Array &b, Array &c, int b_offset) const
{
    const int a_len = a.length();
    const int b_len = b.length();

    int carry = 0;
    for (int i = 0; i < c.length(); i++)
    {
        int a_val = 0, b_val = 0;
        if (a_len > i)
            a_val = a[i];
        if (0 <= i - b_offset && i - b_offset < b_len)
            b_val = b[i - b_offset];

        int sum = a_val + b_val + carry;
        if (sum >= 10)
        {
            sum -= 10;
            carry = 1;
        }
        else
        {
            carry = 0;
        }
        c[i] = sum;
    }
}

void BigInteger::sub(const Array &a, const Array &b, Array &result) const
{
    const int a_len = a.length();
    const int b_len = b.length();
    int c_len = std::max(a_len, b_len);
    result.fill(0);

    for (int i = 0; i < a_len; i++)
        result[i] = a[i];

    for (int i = 0; i < c_len; i++)
    {
        int c_val = 0, b_val = 0;
        c_val = result[i];
        if (b_len > i)
            b_val = b[i];

        if (c_val < b_val)
        {
            borrow(result, i);
            c_val = result[i];
        }

        result[i] = c_val - b_val;
    }
}

Array* BigInteger::mul(const Array &a, const Array &b) const
{
    const int result_len = a.length() + b.length();
    const int component_len = a.length() + 1;
    const int n_components = b.length();
    Array *result_components = new Array[n_components];
    for (int i = 0; i < n_components; i++)
        result_components[i] = Array(component_len);

    for (int i = 0; i < n_components; i++)
        mulByDigit(a, b[i], result_components[i]);

    Array *read_buffer = new Array(result_len);
    Array *write_buffer = new Array(result_len);
    read_buffer->fill(0);
    write_buffer->fill(0);
    for (size_t i = 0; i < n_components; i++)
    {
        add(*read_buffer, result_components[i], *write_buffer, i);

        Array *t = read_buffer;
        read_buffer = write_buffer;
        write_buffer = t;
    }
    
    delete write_buffer;
    delete[] result_components;

    return read_buffer;
}

BigInteger::ArrayDivResult BigInteger::div(const Array &a, const Array &b) const
{
    Array *result_p = new Array(a.length());
    Array *minuend_p = new Array(a.length());
    Array &minuend = *minuend_p;
    Array subtrahend(b.length() + 1);
    subtrahend.fill(0);
    minuend.fill(0);

    for (int i = a.length() - 1; i >= 0; i--)
    {
        shiftRight(minuend);
        minuend[0] = a[i];
        int multiplier = howManyFit(minuend, b);
        (*result_p)[i] = multiplier;
        mulByDigit(b, multiplier, subtrahend);
        Array buffor = minuend;
        sub(buffor, subtrahend, minuend);
    }

    return ArrayDivResult(result_p, minuend_p);
}

BigInteger::SignedArray BigInteger::addSigned(const SignedArray &s_array_1, const SignedArray &s_array_2) const
{
    const Array *a = s_array_1.getArray();
    const Array *b = s_array_2.getArray();
    bool a_neg = s_array_1.isNegative();
    bool b_neg = s_array_2.isNegative();
    Array *c = new Array(std::max(a->length(), b->length()) + 1);

    bool negate = false;

    if (a_neg != b_neg)
    {
        if (a_neg)
        {
            const Array *temp = a; // Put negative number on the 'right' side
            a = b;
            b = temp;
        }
        if (compare(*a, *b) > 0)
        {
            const Array *temp = a; // Put bigger absolute on the 'left' side
            a = b;
            b = temp;
            negate = true;
        }
        sub(*a, *b, *c);
    }
    else
    {
        if (a_neg)
            negate = true;
        add(*a, *b, *c);
    }

    return SignedArray(c, negate);
}

int BigInteger::compare(const BigInteger &a, const BigInteger &b) const
{
    if(a.m_negative == b.m_negative) {
        if(a.m_negative == false) 
            return compare(*a.m_table, *b.m_table);
        else
            return compare(*b.m_table, *a.m_table);
    } else {
        if(a.m_negative == true) 
            return 1;
        else 
            return -1;
    }
}

BigInteger::BigInteger(Array *table, bool negative)
{
    m_table = table;
    m_negative = negative;
};

BigInteger::BigInteger()
{
    m_table = new Array(0);
    m_negative = false;
}

BigInteger::BigInteger(int number)
{
    if (number == 0)
    {
        m_table = new Array(0);
        m_negative = false;
        return;
    }
    else if (number < 0)
    {
        m_negative = true;
        number = -number;
    }
    else
    {
        m_negative = false;
    }

    int len = ceil(log10(1.f * number + 1));
    m_table = new Array(len);
    collectDigits(number, len, *m_table);
}

BigInteger::BigInteger(const BigInteger &other)
{
    m_negative = other.m_negative;
    m_table = new Array(*other.m_table);
}

BigInteger BigInteger::operator-() const
{
    if (m_table->length() == 0)
        return BigInteger(0);

    Array *copy = new Array(*m_table);
    return BigInteger(copy, !m_negative);
}

BigInteger BigInteger::operator+(const BigInteger &other) const
{
    const SignedArray a = SignedArray::fromCNumber(*this);
    const SignedArray b = SignedArray::fromCNumber(other);
    SignedArray c = addSigned(a, b);
    return BigInteger(c.getArray(), c.isNegative());
}

BigInteger BigInteger::operator-(const BigInteger &other) const
{
    const SignedArray a = SignedArray::fromCNumber(*this);
    const SignedArray b = SignedArray::fromCNumber(other, true);
    SignedArray c = addSigned(a, b);
    return BigInteger(c.getArray(), c.isNegative());
}

BigInteger BigInteger::operator*(const BigInteger &other) const
{
    Array *result = mul(*m_table, *other.m_table);
    return BigInteger(result, m_negative != other.m_negative);
}

DivResult BigInteger::operator/(const BigInteger &other) const
{
    if (compare(*m_table, *other.m_table) == 1)
        return DivResult(BigInteger(0), other);
    if (compare(Array(0), *other.m_table) == 0)
        return DivResult(BigInteger(0), BigInteger(0));

    ArrayDivResult res = div(*m_table, *other.m_table);
    bool res_negative = m_negative != other.m_negative;
    BigInteger a(res.divident, res_negative);
    BigInteger b(res.remainder, res_negative);

    return DivResult(a, b);
}

BigInteger BigInteger::operator%(const BigInteger &other) const {
    return ((*this)/other).remainder;
}

BigInteger BigInteger::operator++() const
{
    return (*this) + BigInteger(1);
}

BigInteger BigInteger::operator--() const
{
    return (*this) - BigInteger(1);
}

bool BigInteger::operator<(const BigInteger &other) const
{
    return compare(*this, other) == 1;
}

bool BigInteger::operator<=(const BigInteger &other) const
{
    return compare(*this, other) != -1;
}

bool BigInteger::operator==(const BigInteger &other) const
{
    return compare(*this, other) == 0;
}

bool BigInteger::operator!=(const BigInteger &other) const
{
    return compare(*this, other) != 0;
}

bool BigInteger::operator>=(const BigInteger &other) const
{
    return compare(*this, other) != 1;
}

bool BigInteger::operator>(const BigInteger &other) const
{
    return compare(*this, other) == -1;
}

std::string BigInteger::toString() const
{
    std::string str = "";

    int i = m_table->length() - 1;
    while ((*m_table)[i] == 0) // Skip zeros
        i--;

    for (; i >= 0; i--) // Write numbers to string
        str += '0' + (*m_table)[i];

    if (str == "") // replace emtpy string with 0
        str = "0";

    if (m_negative && str != "0") // add negative sign
        str = "-" + str;

    return str;
}

DivResult::DivResult(BigInteger result, BigInteger remainder) : result(result),
                                                          remainder(remainder) {}
