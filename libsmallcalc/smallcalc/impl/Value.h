#pragma once
#include "../Expression.h"
#include "../PrimitiveValue.h"
namespace sc {

/// A fixed value as an expression
class Value : public Expression {
public:
	Value (const PrimitiveValue & value) : mValue (value) {}
	Value (double x) : mValue (x) {}
	Value (int64_t x) : mValue (x) {}

	virtual PrimitiveValue eval (EvaluationContext * calcContext) const { return mValue; }
	virtual std::string print (PrintingContext * printingContext) const { return mValue.toString(); }
	virtual Error error () const { return mValue.type() == PT_ERROR ? mValue.error() : NoError; }

	const PrimitiveValue & value () const { return mValue; }
private:
	PrimitiveValue mValue;
};
typedef shared_ptr<Value> ValuePtr;

}
