#pragma once

class Array
{
    int *m_table;
    int m_len;

public:
    Array();

    Array(int len);

    Array(const Array &other);

    int length() const;

    int operator[](int i) const;

    int &operator[](int i);

    void operator=(const Array &other);

    void fill(int value);

    ~Array();
};