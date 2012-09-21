#pragma once
#include "types.h"
#include <boost/lexical_cast.hpp>
#include <ostream>

namespace sc {

enum PrimitiveValueType {
	PT_NULL,
	PT_DOUBLE,
	PT_INT64,
	PT_FRACTION,
	PT_ERROR
};

class RefinedPrimitiveValue;
typedef shared_ptr<RefinedPrimitiveValue> RefinedPrimitiveValuePtr;
class FractionValue;

/// A primitive value (a number or error code or similar)
/// Note: this was a class structure, but was refactored due performance causes
/// Other subtypes shall use union's or similar
class PrimitiveValue {
public:
	PrimitiveValue () : mType (PT_NULL) {}
	PrimitiveValue (double d) : mType (PT_DOUBLE), mDoubleValue (d) {}
	PrimitiveValue (int64_t i) : mType (PT_INT64), mIntValue (i) {}
	PrimitiveValue (Error e, const String & msg);
	PrimitiveValue (const Fraction64 & fraction);
	~PrimitiveValue () {}
	String toString () const;
	PrimitiveValueType type() const { return mType; }
	/// Try conversion to double value; only valid if convertable
	double toDouble () const;
	/// Returns int value, only valid if is an int
	int64_t intValue () const;
	/// Type is accurate (integer, fraction etc.) not rounded like double
	bool isAccurateType () const { return mType == PT_ERROR || mType == PT_INT64 || mType == PT_FRACTION; }
	/// Check encoded error if there is one
	Error error () const { return mType == PT_ERROR ? mErrorValue : NoError; }

	/// Looks for a valid PrimitiveValue
	operator bool () const { return mType != PT_NULL; }

protected:
	/// Returns fraction subtype, if the Value itself is a fraction, otherwise 0.
	const Fraction64 & fraction () const;
public:

	/// The value is convertable to a fraction
	bool fractionable () const { return mType == PT_INT64 || mType == PT_FRACTION; }

	/// Converts the value to a fraction (if PT_INT64 or PT_FRACTION)
	Fraction64 toFraction () const;

	const RefinedPrimitiveValuePtr& refinedValue () const { return mRefinedValue; }

	/// Exact comparison operator
	bool operator== (const PrimitiveValue & other) const;
private:
	PrimitiveValueType mType;
	union {
		double      mDoubleValue;
		int64_t     mIntValue;
		Error       mErrorValue;
	};
	RefinedPrimitiveValuePtr mRefinedValue;
};

/// A refined sub type for primitive values
class RefinedPrimitiveValue {
public:
	virtual ~RefinedPrimitiveValue () {}
	virtual String toString () const = 0;
	virtual double toDouble () const = 0;
};

/// A refined sub type for error codes
class ErrorMessage : public RefinedPrimitiveValue {
public:
	ErrorMessage (const std::string & msg) : mErrorMessage (msg) {}
	virtual String toString () const { return mErrorMessage; }
	virtual double toDouble () const { return 0; }
private:
	String mErrorMessage;
};

// A refined subtype for a fraction
class FractionValue : public RefinedPrimitiveValue {
public:
	FractionValue (int64_t numerator, int64_t denumerator) : mFraction (numerator, denumerator){}
	FractionValue (const Fraction64& fraction) : mFraction (fraction) {}
	virtual String toString () const { return boost::lexical_cast<String> (mFraction.numerator()) + "/" + boost::lexical_cast<std::string>(mFraction.denumerator()); }
	virtual double toDouble () const { return (double) mFraction.numerator() / (double) mFraction.denumerator();}

	int64_t numerator () const { return mFraction.numerator(); }
	int64_t denumerator() const { return mFraction.denumerator(); }
	const Fraction64 & value() const { return mFraction;}
private:
	Fraction64 mFraction;
};


inline PrimitiveValue errorValue (Error e, const String & msg = String()) { return PrimitiveValue (e, msg);}
inline PrimitiveValue doubleValue (double d) { return PrimitiveValue (d); }

/// Output operator
inline std::ostream & operator<< (std::ostream & stream, const PrimitiveValue & value) { return stream << value.toString(); }

}
