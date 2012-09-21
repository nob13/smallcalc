#pragma once
#include <stdint.h>
#include <math.h>
#include <boost/math/common_factor.hpp>
#include <sstream>
#include <string>
#include <assert.h>

/**
 * @file Some core operations, especially with range check.
 */
namespace sc {

/** Addition with overflow detection. */
template <class Type>
inline Type addWithOverflowCheck (Type a, Type b, bool * overflow) {
	if (b < 0 && std::numeric_limits<Type>::min() - b > a){
		*overflow = true;
	}
	if (b > 0 && std::numeric_limits<Type>::max() - b < a){
		*overflow = true;
	}
	return a + b;
}

/** Subtraction with overflow detection. */
template <class Type>
Type subWithOverflowCheck (Type a, Type b, bool * overflow) {
	if(b < 0 && std::numeric_limits<Type>::max() + b < a) {
		*overflow = true;
	}
	if(b > 0 && std::numeric_limits<Type>::min() + b > a) {
		*overflow = true;
	}
	return a - b;
}

/** Absolute value with overflow detection. */
template <class Type>
inline Type absWithOverflowCheck (Type x, bool * overflow) {
	if (x < 0 && x == std::numeric_limits<Type>::min()) { *overflow = true; }
	return std::abs(x);
}

/** Multiplication with overflow detection. */
// Not the fastest one
// Source: https://www.securecoding.cert.org/confluence/display/seccode/INT32-C.+Ensure+that+operations+on+signed+integers+do+not+result+in+overflow?showComments=false
template <class Type>
Type multWithOverflowCheck (Type a, Type b, bool * overflow) {
	if (a > 0){  /* si1 is positive */
	  if (b > 0) {  /* si1 and si2 are positive */
	    if (a > (std::numeric_limits<Type>::max() / b)) {
	    	*overflow = true;
	    }
	  } /* end if si1 and si2 are positive */
	  else { /* si1 positive, si2 non-positive */
	    if (b < (std::numeric_limits<Type>::min() / a)) {
	    	*overflow = true;
	    }
	  } /* si1 positive, si2 non-positive */
	} /* end if si1 is positive */
	else { /* si1 is non-positive */
	  if (b > 0) { /* si1 is non-positive, si2 is positive */
	    if (a < (std::numeric_limits<Type>::min() / b)) {
	    	*overflow = true;
	    }
	  } /* end if si1 is non-positive, si2 is positive */
	  else { /* si1 and si2 are non-positive */
	    if ( (a != 0) && (b < (std::numeric_limits<Type>::max() / a))) {
	    	*overflow = true;
	    }
	  } /* end if si1 and si2 are non-positive */
	} /* end if si1 is non-positive */
	return a * b;
}

// Faster implementation with 128 bit types
#if 0
inline int64_t multWithOverflowCheck (int64_t a, int64_t b, bool * overflow) {
	__int128_t x = (__int128_t) a * b;
	if (x >   std::numeric_limits<int64_t>::max()) *overflow = true;
	if (x <   std::numeric_limits<int64_t>::min()) *overflow = true;
	return (int64_t) x;
}
#endif

/** Least common multiple with overflow detection. */
template <class Type>
inline Type lcmWithOverflowCheck (Type a, Type b, bool * overflow) {
	// Avoid overflows
	Type gcd = boost::math::gcd(a,b);
	Type result = multWithOverflowCheck (a / gcd, b, overflow);
	return result;
}

/** Represents a fraction. */
template <class Type> class Fraction {
public:
	/// Constructs invalid fraction
	Fraction () : mNumerator(0), mDenumerator(0) {}
	/// Constructs a fraction from numerator and denumerator
	Fraction (Type numerator, Type denumerator = 1) : mNumerator (numerator), mDenumerator(denumerator) {}

	/// Assignment operator
	Fraction<Type>& operator= (const Fraction<Type> & other) {
		mNumerator   = other.numerator();
		mDenumerator = other.denumerator();
		return *this;
	}

	/// Returns true if fraction is valid
	bool valid () const { return mDenumerator != 0; }

	/// Is an integer value
	bool isInteger() const { return mDenumerator == 1; }

	/// Returns numerator
	Type numerator() const { return mNumerator; }

	/// Returns denumerator
	Type denumerator () const { return mDenumerator; }

	/// Returns the negation
	Fraction<Type> negation () const { return Fraction<Type> (0 - mNumerator, mDenumerator); }

	/// Normalizes the fraction
	Fraction normalize () const {
		if (mDenumerator < 0) {
			return Fraction<Type> (-mNumerator,-mDenumerator).normalize();
		}
		Type gcd = boost::math::gcd(mNumerator, mDenumerator);
		if (gcd == 0) {
			return Fraction(); // illegal
		}
		return Fraction (mNumerator / gcd, mDenumerator / gcd);
	}

	bool operator== (const Fraction<Type> & other) const {
		return mNumerator == other.numerator() && mDenumerator == other.denumerator();
	}

	/** Converts to a decimal expression.
	 * If the conversion is exact, exact will be set to true.
	 * The conversion will use up to maxValidLength digits after the dot.
	 * */
	std::string toDecimal (bool * exact = 0, int maxValidLength = 8) const {
		if (exact) *exact = true;
		std::ostringstream ss;
		Fraction<Type> current = normalize();
		if (current.numerator() < 0) {
			ss << "-";
			current = current.negation();
		}
		Type dec = current.numerator() / current.denumerator();
		ss << dec;
		current = Fraction<Type> (current.numerator() - dec * current.denumerator(), current.denumerator());
		if (current.numerator() == 0) return ss.str(); // no point
		ss << ".";
		int length = 0;
		bool began = false;
		bool overflow = false;
		while (length < maxValidLength && current.numerator() != 0){
			Type newNumerator = multWithOverflowCheck<Type>(current.numerator(), 10, &overflow);
			if (overflow) {
				break;
			}
			dec = newNumerator / current.denumerator();
			assert (dec >=0 && dec < 10);
			ss << dec;
			current = Fraction<Type> (newNumerator - dec * current.denumerator(), current.denumerator());
			if (dec != 0) {
				began = true;
			}
			if (began) {
				length++;
			}
		}
		if (overflow || length == maxValidLength) {
			if (exact) *exact = false;
		}
		return ss.str();
	}

private:
	Type mNumerator;
	Type mDenumerator;
};

template <class Type>
Fraction<Type> addWithOverflowCheck (const Fraction<Type> & a, const Fraction<Type> & b, bool * overflow) {
	Type gcd = boost::math::gcd (a.denumerator(), b.denumerator());
	Type am  = b.denumerator() / gcd;
	Type bm  = a.denumerator() / gcd;

	Type an  = multWithOverflowCheck (a.numerator(), am, overflow);
	Type bn  = multWithOverflowCheck (b.numerator(), bm, overflow);
	Type lcm = multWithOverflowCheck (a.denumerator() / gcd, b.denumerator(), overflow);

	Type n = addWithOverflowCheck(an, bn, overflow);
	return Fraction<Type> (n, lcm).normalize();
}

template <class Type>
Fraction<Type> subWithOverflowCheck (const Fraction<Type> & a, const Fraction<Type> & b, bool * overflow) {
	Type gcd = boost::math::gcd (a.denumerator(), b.denumerator());
	Type am  = b.denumerator() / gcd;
	Type bm  = a.denumerator() / gcd;

	Type an  = multWithOverflowCheck (a.numerator(), am, overflow);
	Type bn  = multWithOverflowCheck (b.numerator(), bm, overflow);
	Type lcm = multWithOverflowCheck (a.denumerator() / gcd, b.denumerator(), overflow);

	Type n = subWithOverflowCheck(an, bn, overflow);
	return Fraction<Type> (n, lcm).normalize();
}

template <class Type>
Fraction<Type> multWithOverflowCheck (const Fraction<Type> & a, const Fraction<Type> & b, bool * overflow){
	Type numerator = multWithOverflowCheck(a.numerator(), b.numerator(), overflow);
	Type denumerator = multWithOverflowCheck(a.denumerator(),b.denumerator(), overflow);
	return Fraction<Type> (numerator, denumerator).normalize();
}

template <class Type>
Fraction<Type> divWithOverflowCheck (const Fraction<Type> & a, const Fraction<Type> & b, bool * overflow){
	Type numerator = multWithOverflowCheck(a.numerator(), b.denumerator(), overflow);
	Type denumerator = multWithOverflowCheck(b.numerator(), a.denumerator(), overflow);
	return Fraction<Type> (numerator, denumerator).normalize();
}

}
