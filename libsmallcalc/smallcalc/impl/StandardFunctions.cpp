#include "StandardFunctions.h"
#include <math.h>
#include <boost/math/common_factor.hpp>
#include <assert.h>
#include "../Expression.h" // for EvaluationContext
#include "../MathFunctions.h"

namespace sc {

#define CHECK_ERROR1 assert(arguments.size() == 1); if (arguments[0].type() == PT_ERROR) return arguments[0];
/// Checks for errors in a or b and returns them
#define CHECK_ERROR2(A,B)\
		if(A.type()==PT_ERROR) return A;\
		if(B.type()==PT_ERROR) return B;

PrimitiveValue accurateAdd (const PrimitiveValue & a, const PrimitiveValue & b, bool * overflow) {
	CHECK_ERROR2(a,b);
	if (a.type() == PT_INT64 && b.type() == PT_INT64) {
		int64_t x = addWithOverflowCheck (a.intValue(), b.intValue(), overflow);
		return PrimitiveValue (x);
	}
	Fraction64 fa (a.toFraction());
	Fraction64 fb (b.toFraction());
	Fraction64 result = addWithOverflowCheck(fa, fb, overflow);
	return PrimitiveValue (result);
}

PrimitiveValue accurateMultiply (const PrimitiveValue & a, const PrimitiveValue & b, bool * overflow) {
	CHECK_ERROR2(a,b);
	if (a.type() == PT_INT64 && b.type() == PT_INT64) {
		int64_t x = multWithOverflowCheck(a.intValue(), b.intValue(), overflow);
		return PrimitiveValue (x);
	}
	Fraction64 fa (a.toFraction());
	Fraction64 fb (b.toFraction());
	Fraction64 result = multWithOverflowCheck(fa, fb, overflow);
	return PrimitiveValue (result);
}


PrimitiveValue accurateSubtract (const PrimitiveValue & a, const PrimitiveValue & b, bool * overflow) {
	CHECK_ERROR2(a,b);
	if (a.type() == PT_INT64 && b.type() == PT_INT64) {
		int64_t x = subWithOverflowCheck(a.intValue(), b.intValue(), overflow);
		return PrimitiveValue(x);
	}
	Fraction64 fa (a.toFraction());
	Fraction64 fb (b.toFraction());

	Fraction64 result = subWithOverflowCheck(fa, fb, overflow);
	return PrimitiveValue (result);
}

PrimitiveValue accurateDivide (const PrimitiveValue & a, const PrimitiveValue & b, bool * overflow) {
	CHECK_ERROR2(a,b);
	if (b.type() == PT_INT64 && b.intValue() == 0) {
		return errorValue(error::Eval_DivisionByZero, "Division by zero");
	}
	Fraction64 fa (a.toFraction());
	Fraction64 fb (b.toFraction());
	Fraction64 result = divWithOverflowCheck(fa, fb, overflow);
	return PrimitiveValue (result);
}

PrimitiveValue accuratePower (const PrimitiveValue & a, const PrimitiveValue & b, bool * overflow) {
	CHECK_ERROR2(a,b);
	if (b.type() != PT_INT64) {
		// will lead to overflow
		*overflow = true;
		return PrimitiveValue (error::Eval_InvalidOperation, "overflow");
	}
	int64_t bv = b.intValue();
	if (b.intValue() == 0) {
		if (a.type() == PT_INT64 && a.intValue() == 0) {
			return PrimitiveValue (error::Eval_DivisionByZero, "0^0 is not defined");
		}
		return PrimitiveValue ((int64_t)1);
	}
	if (a.type() == PT_INT64 && a.intValue() == 1) {
		return PrimitiveValue ((int64_t)1);
	}

	if (bv < 0) {
		PrimitiveValue current = PrimitiveValue (int64_t(1));
		for (int64_t i = 0; i > bv; i--) {
			current = accurateDivide (current, a, overflow);
			if (*overflow) return PrimitiveValue (error::Eval_InvalidOperation, "overflow");
		}
		return current;
	} else {
		PrimitiveValue current = PrimitiveValue (int64_t(1));
		for (int64_t i = 0; i < bv; i++) {
			current = accurateMultiply (current, a, overflow);
			if (*overflow) return PrimitiveValue (error::Eval_InvalidOperation, "overflow");
		}
		return current;
	}
}

PrimitiveValue add (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	if (context->accurateLevel){
		bool overflow = false;
		PrimitiveValue accurateSum ((int64_t) 0);
		std::vector<PrimitiveValue>::const_iterator i = arguments.begin();
		for (; i != arguments.end(); i++) {
			if (!i->isAccurateType()) break;
			accurateSum = accurateAdd (accurateSum, *i, &overflow);
			if (overflow) break;
		}
		if (i == arguments.end()) return accurateSum;
	}

	// Double calculation (as fallback)
	double sum = 0;
	BOOST_FOREACH (const PrimitiveValue & arg, arguments){
		if (arg.type() == PT_ERROR) return arg;
		sum+=arg.toDouble();
	}
	return doubleValue(sum);
}

PrimitiveValue multiply (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	if (context->accurateLevel){
		bool overflow = false;
		PrimitiveValue accurateProduct ((int64_t) 1);
		std::vector<PrimitiveValue>::const_iterator i = arguments.begin();
		for (; i != arguments.end(); i++) {
			if (!i->isAccurateType()) break;
			accurateProduct = accurateMultiply (accurateProduct, *i, &overflow);
			if (overflow) break;
		}
		if (i == arguments.end()) return accurateProduct;
	}

	// Double calculation (as fallback)
	double product = 1.0;
	BOOST_FOREACH (const PrimitiveValue & arg, arguments){
		if (arg.type() == PT_ERROR) return arg;
		product*=arg.toDouble();
	}
	return doubleValue(product);
}

PrimitiveValue subtract (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	assert (arguments.size() == 2);
	const PrimitiveValue & a (arguments[0]);
	const PrimitiveValue & b (arguments[1]);
	CHECK_ERROR2(a,b);
	if (context->accurateLevel && a.isAccurateType() && b.isAccurateType()){
		bool overflow = false;
		PrimitiveValue candidate =  accurateSubtract(a,b, &overflow);
		if (!overflow) return candidate;
	}
	return a.toDouble() - b.toDouble();
}

PrimitiveValue divide (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	assert (arguments.size() == 2);
	const PrimitiveValue & a (arguments[0]);
	const PrimitiveValue & b (arguments[1]);
	CHECK_ERROR2(a,b);
	if (context->accurateLevel && a.isAccurateType() && b.isAccurateType()){
		bool overflow = false;
		PrimitiveValue candidate = accurateDivide(a,b,&overflow);
		if (!overflow) return candidate;
	}
	return a.toDouble() / b.toDouble();
}

PrimitiveValue negate (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue (0 - arguments[0].toDouble());
}

PrimitiveValue exponentation (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	assert (arguments.size() == 2);
	if (arguments.size() != 2) return errorValue (error::Eval_InvalidOperation, "Invalid argument count");
	CHECK_ERROR2(arguments[0], arguments[1]);
	if (context->accurateLevel && arguments[0].isAccurateType() && arguments[1].isAccurateType()) {
		bool overflow = false;
		PrimitiveValue candidate = accuratePower(arguments[0], arguments[1], &overflow);
		if (!overflow) return candidate;
	}
	return doubleValue (::pow (arguments[0].toDouble(), arguments[1].toDouble()));
}


PrimitiveValue sin (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue (::sin (arguments[0].toDouble()));
}

PrimitiveValue cos (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue (::cos (arguments[0].toDouble()));
}

PrimitiveValue tan (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue (::tan (arguments[0].toDouble()));
}

PrimitiveValue round (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue (::round (arguments[0].toDouble()));
}

PrimitiveValue sqrt (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::sqrt(arguments[0].toDouble()));
}

PrimitiveValue acos (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::acos(arguments[0].toDouble()));
}

PrimitiveValue asin (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::asin(arguments[0].toDouble()));
}

PrimitiveValue atan (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::atan(arguments[0].toDouble()));
}

PrimitiveValue cosh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::cosh(arguments[0].toDouble()));
}

PrimitiveValue sinh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::sinh(arguments[0].toDouble()));
}

PrimitiveValue tanh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::tanh(arguments[0].toDouble()));
}

PrimitiveValue acosh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::acosh(arguments[0].toDouble()));
}

PrimitiveValue asinh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::asinh(arguments[0].toDouble()));
}

PrimitiveValue atanh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::atanh(arguments[0].toDouble()));
}


PrimitiveValue ln (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::log(arguments[0].toDouble()));
}

PrimitiveValue abs (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context) {
	CHECK_ERROR1;
	return doubleValue(::fabs(arguments[0].toDouble()));
}





}
