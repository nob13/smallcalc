#pragma once
#include "../Expression.h"

namespace sc {

/// A constant with a given name and value
class Constant : public Expression {
public:
	Constant (const std::string & name, const PrimitiveValue & value) : mName (name), mValue (value) {};

	virtual PrimitiveValue eval (EvaluationContext * calcContext) const { return mValue; }
	virtual std::string print (PrintingContext * printingContext) const { return mName; }

	const std::string& name() const { return mName; }

	/// Returns value
	const PrimitiveValue & value () const { return mValue; }
private:
	std::string mName;
	PrimitiveValue mValue;
};
typedef shared_ptr<Constant> ConstantPtr;
inline ConstantPtr createConstant (const std::string & name, double value) { return ConstantPtr (new Constant (name, doubleValue (value))); }

}
