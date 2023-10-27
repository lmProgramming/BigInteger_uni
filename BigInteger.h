#pragma once

#include <iostream>
#include "Array.h"

class DivResult;

class BigInteger
{
    struct SignedArray
    {
    private:
        Array *m_array;
        bool m_negative;

    public:
        SignedArray(Array *result, bool negative);

        static const SignedArray fromCNumber(const BigInteger &number, bool flip_neg = false);

        Array *getArray();

        const Array *getArray() const;

        bool isNegative() const;
    };

    struct ArrayDivResult {
        Array *divident;
        Array *remainder;

        ArrayDivResult(Array *divident, Array *remainder)
            :divident(divident),
            remainder(remainder) {};
    };

    Array *m_table;
    bool m_negative;

    void collectDigits(int x, int digits_left, Array &target) const;

    void borrow(Array &number, int digit_index) const;

    void mulByDigit(const Array &a, int digit, Array &result) const;

    int howManyFit(const Array &divident, const Array &divisor) const;

    void shiftRight(Array &a) const;

    int compare(const Array &a, const Array &b) const;

    void add(const Array &a, const Array &b, Array &result, int b_offset = 0) const;

    void sub(const Array &a, const Array &b, Array &result) const;

    Array* mul(const Array &a, const Array &b) const;

    ArrayDivResult div(const Array &a, const Array &b) const;

    SignedArray addSigned(const SignedArray &a, const SignedArray &b) const;

    int compare(const BigInteger &a, const BigInteger &b) const;

public:
    BigInteger(Array *table, bool negative);

    BigInteger();

    BigInteger(int number);

    BigInteger(const BigInteger &other);

    BigInteger operator-() const;
    BigInteger operator+(const BigInteger &other) const;
    BigInteger operator-(const BigInteger &other) const;
    BigInteger operator*(const BigInteger &other) const;
    DivResult operator/(const BigInteger &other) const;
    BigInteger operator%(const BigInteger &other) const;
    BigInteger operator++() const;
    BigInteger operator--() const;
    bool operator<(const BigInteger &other) const;
    bool operator<=(const BigInteger &other) const;
    bool operator==(const BigInteger &other) const;
    bool operator!=(const BigInteger &other) const;
    bool operator>=(const BigInteger &other) const;
    bool operator>(const BigInteger &other) const;

    std::string toString() const;
};

class DivResult
{
public:
    BigInteger result;
    BigInteger remainder;

    DivResult(BigInteger result, BigInteger remainder);
};
