# BigInteger_uni

This is a small c++ project for university. Please do not use in real projects, there exists better, faster and more reliable versions of this.

This project is an implementation of BigInteger in Java. With this implementation, you can add, subtract, multiply, divide and compare arbitrarily big integers. Usage is presented in main.cpp as test cases. 
As a way of storing these integers, BigInteger uses array of decimal digits represented as int. Arithmetic operations are implemented similarly to how you would do these operation on paper, with some slight optimizations. One thing to note is that as you do operations on these numbers, their array size will grow, as all operations allocate enough memory to handle worst case scenarios (multiplication of two 20digit numbers will always allocate array of 40digits) and currently there's no way to shrink these arrays.
