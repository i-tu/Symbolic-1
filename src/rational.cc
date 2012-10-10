#include "rational.hh"
#include <cmath>
#include <sstream>
#include <stdexcept>

// You must implement these operators:
//
// +=, -=

Rational Rational::operator+=(const Rational& r)
{
	n = r.d*n + d*r.n;
	d = r.d*d;
	return *this;
}

Rational Rational::operator+=(int i)
{
	n = n + d*i;
	return *this;
}

Rational Rational::operator-=(const Rational& r)
{
	*this += -r;
	return *this;
}

Rational Rational::operator-=(int i)
{
	*this += -i;
	return *this;
}



// relational operators
// <, <=, ==, >=, >, !=

bool Rational::operator<(const Rational& r)
{
	return (float)n/d < (float)r.n/r.d;
}

bool Rational::operator<=(const Rational& r)
{
	return (float)n/d <= (float)r.n/r.d;
}

bool Rational::operator==(const Rational& r)
{
	return (float)n/d == (float)r.n/r.d;
}


bool Rational::operator>=(const Rational& r)
{
	return (float)n/d >= (float)r.n/r.d;
}


bool Rational::operator>(const Rational& r)
{
	return (float)n/d > (float)r.n/r.d;
}


bool Rational::operator!=(const Rational& r)
{
	return !(*this == r);
}


//
// binary operators +, -

Rational operator+(const Rational& r1, const Rational& r2)
{
	Rational rr1(r1.n,r1.d);
	return rr1 += r2;
}

Rational operator+(int i, const Rational& r)
{
	return Rational(r.n + r.d*i,r.d);
}


Rational operator+(const Rational& r,int i)
{
	return Rational(r.n + r.d*i,r.d);
}

Rational operator-(const Rational& r1, const Rational& r2)
{
	return r1 + (-r2);
}

Rational operator-(int i, const Rational& r)
{
	return i + -r;
}

Rational operator-(const Rational& r,int i)
{
	return r + (-i);
}

Rational operator/(const Rational& r1, const Rational& r2)
{
	return Rational::normalize(Rational(r1.n*r2.d, r1.d*r2.n));
}

Rational operator*(const Rational& r1, const Rational& r2)
{
	return Rational::normalize(Rational(r1.n*r2.n,r1.d*r2.d));
}

//
// unary operator -

Rational Rational::operator-() const
{
	return Rational(-n,d);
}

//
// input and output operators << and >>

std::ostream& operator<<(std::ostream& os, const Rational& r)
{
	Rational rr = Rational::normalize(r);
	if (rr.d == 1)
	{
		return os << rr.n;
	}
	else
	{
		return os << rr.n << "/" << rr.d;
	}
}

std::istream& operator>>(std::istream& is, Rational& r)
{
	int nn, dd;
	if ((is >> nn) && (is >> dd))
	{
		r.n = nn;
		r.d = dd;
	}
	return is;
}

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

Rational Rational::normalize(const Rational &r)
{
	using namespace std;
	int gcd = Rational::gcd(r.n,r.d);
	int sign = 1;
	if (r.n != 0)
		sign = r.n*r.d/abs(r.n*r.d);
	return Rational(sign*abs(r.n)/gcd,abs(r.d)/gcd);
}

Rational Rational::rationalPow(const Rational& r1, const Rational& r2)
{
	if (r2.d == 1)
	{
		return Rational((int)std::pow((double)r1.n,r2.n),(int)std::pow((double)r1.d,r2.n));
	}
	else
	{
		std::ostringstream oss;
		oss << r2 << " is not a valid power.";
		throw new std::logic_error(oss.str().c_str());
	}
} 
