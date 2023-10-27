#include <iostream>
#include "Array.h"

Array::Array()
{
    m_table = 0;
    m_len = 0;
}

Array::Array(int len)
{
    m_table = new int[len];
    this->m_len = len;
}

Array::Array(const Array &other)
{
    m_len = other.m_len;
    m_table = new int[m_len];
    for (int i = 0; i < m_len; i++)
        m_table[i] = other[i];
}

int Array::length() const
{
    return m_len;
}

int Array::operator[](int i) const
{
    return m_table[i];
}

int &Array::operator[](int i)
{
    return m_table[i];
}

void Array::operator=(const Array &other)
{
    delete[] m_table;

    m_len = other.m_len;
    m_table = new int[m_len];
    std::copy(other.m_table, other.m_table + m_len, m_table);
}

void Array::fill(int value)
{
    for (int i = 0; i < m_len; i++)
        m_table[i] = value;
}

Array::~Array()
{
    delete[] m_table;
}