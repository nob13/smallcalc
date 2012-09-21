#include "PrimitiveValue.h"
#include <boost/make_shared.hpp>
namespace sc {

PrimitiveValue::PrimitiveValue (Error e, const String & msg) : mType (PT_ERROR), mErrorValue (e) {
	mRefinedValue = boost::make_shared<ErrorMessage> (msg);
}

PrimitiveValue::PrimitiveValue (const Fraction64 & f) {
	Fraction64 x = f.normalize();
	if (x.isInteger()){
		mType = PT_INT64;
		mIntValue = x.numerator();
	} else if (!x.valid()){
		mType = PT_ERROR;
		mErrorValue = error::Eval_DivisionByZero;
		mRefinedValue = boost::make_shared<ErrorMessage> ("Division by zero");
	} else {
		mType = PT_FRACTION;
		mRefinedValue = boost::make_shared<FractionValue> (x);
	}
}


String PrimitiveValue::toString () const {
	if (mType == PT_DOUBLE) return boost::lexical_cast<std::string> (mDoubleValue);
	if (mType == PT_INT64) return boost::lexical_cast<std::string> (mIntValue);
	if (mType == PT_NULL) return "null";
	if (mType == PT_ERROR) {
		return "Err: " + boost::lexical_cast<std::string> (mErrorValue) + " " + mRefinedValue->toString();
	}
	return mRefinedValue->toString();
}

double PrimitiveValue::toDouble () const {
	if (mType == PT_DOUBLE) return mDoubleValue;
	if (mType == PT_INT64) return  mIntValue;
	return mRefinedValue->toDouble();
}

int64_t PrimitiveValue::intValue () const {
	if (mType == PT_INT64) return mIntValue;
	return 0;
}

Fraction64 PrimitiveValue::toFraction () const {
	if (mType == PT_INT64) return Fraction64 (mIntValue, 1);
	if (mType == PT_FRACTION) return fraction();
	assert (!"not fractionable");
	return Fraction64();
}

const Fraction64 & PrimitiveValue::fraction () const {
	assert (mType == PT_FRACTION);
	return((const FractionValue*) (mRefinedValue.get()))->value();
}


bool PrimitiveValue::operator== (const PrimitiveValue & other) const {
	if (mType == PT_ERROR) return mErrorValue == other.error();
	if (mType == PT_INT64 && other.type() == PT_INT64) return mIntValue == other.mIntValue;
	if (mType == PT_DOUBLE && other.type () == PT_DOUBLE) return mDoubleValue == other.mDoubleValue;
	if (mType == PT_FRACTION&& other.type () == PT_FRACTION) {
		return
				fraction() == other.fraction();
	}
	return false;
}



}
