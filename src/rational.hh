#ifndef RATIONAL_HH
#define RATIONAL_HH

#include <utility>
#include <iostream>
#include <algorithm>
#include <cstdlib>

class Rational {
public:

	// if denominator is 0, the behaviour is undefined
	// (you can do whatever you want)
	Rational(int numerator,
		int denominator) :
	n(numerator),d(denominator)
	{}


	// This creates a rational number which is equal to the integer "num"
	Rational(int num) : 
	n(num), d(1)
	{}

	Rational() : 
	n(0), d(0)
	{}

	Rational(const Rational& other)
	{
		n = other.n;
		d = other.d;
	}

	Rational operator=(const Rational& other)
	{
		n = other.n;
		d = other.d;
		return *this;
	}

	~Rational() 
	{}

	// accessors
	int getNumerator() const
	{
		return n;
	}
	int getDenominator() const
	{
		return d;
	}

	// You must implement these operators:
	//
	// +=, -=

	Rational operator+=(const Rational& r);

	Rational operator+=(int i);

	Rational operator-=(const Rational& r);

	Rational operator-=(int i);


	// relational operators
	// <, <=, ==, >=, >, !=

	bool operator<(const Rational& r);

	bool operator<=(const Rational& r);

	bool operator==(const Rational& r);

	bool operator>=(const Rational& r);

	bool operator>(const Rational& r);

	bool operator!=(const Rational& r);

	//
	// binary operators +, -

	friend Rational operator+(const Rational& r1, const Rational& r2);

	friend Rational operator+(int i, const Rational& r);

	friend Rational operator+(const Rational& r,int i);

	friend Rational operator-(const Rational& r1, const Rational& r2);

	friend Rational operator-(int i, const Rational& r);

	friend Rational operator-(const Rational& r,int i);

	friend Rational operator/(const Rational&, const Rational&);

	friend Rational operator*(const Rational& r1, const Rational& r2);

	//
	// unary operator -

	Rational operator-() const;

	//
	// input and output operators << and >>

	friend std::ostream& operator<<(std::ostream& os, const Rational& r);

	friend std::istream& operator>>(std::istream& is, Rational& r);

	// so the class can be streamed to and streamed from.
	// The rational number is formatted like this:
	// <numerator> <denominator>
	// For example if numerator is 4 and denominator is 5, the output is
	// 4 5
	// 
	// The output stream operators "normalize" the rational number.
	// For example, 2/4 is written to output as "1 2",
	// 1/-2 is written to output as "-1 2" and -2/-4 is written as
	// "1 2".
	//
	// You can implement these operators either as member functions,
	// or nonmember functions in the namespace scope.


	static Rational normalize(const Rational& r);

	static Rational rationalPow(const Rational& r1, const Rational& r2);

private:

	int n;
	int d;
	// This function returns the greatest common divisor
	// for a and b. It is useful for normalizing a rational number.
	//
	// For example 6/9, gcd(6, 9) == 3 so we can divide
	// both the numerator and denominator by 3 to get 2/3.

	static int gcd(int a, int b)
	{
		using std::swap;
		a = std::abs(a);
		b = std::abs(b);
		if (a>b) swap(a,b);
		while (a)
		{
			b %= a;
			swap(a,b);
		}
		return b;
	}

};


#endif
 
