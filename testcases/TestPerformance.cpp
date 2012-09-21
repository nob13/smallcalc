#include <gtest/gtest.h>
#include <smallcalc/smallcalc.h>
#include <smallcalc/impl/Parser.h>
#include <math.h>

using namespace sc;

class TestPerformance : public testing::Test {
protected:
	double evalToDouble (const std::string & s){
		return calc.eval (s).toDouble();
	}
	Error evalToError (const std::string & s) {
		return calc.eval (s).error();
	}

	SmallCalc calc;
};


TEST_F (TestPerformance, simpleTest) {
	// Like we use in plotting
	calc.addAllStandard();
	ExpressionPtr exp = calc.parse ("sin(x)"); // simple to calculate
	EvaluationContext context;
	VariableId xId = calc.idOfVariable ("x");
	for (int i = 0; i < 2000000; i++) {
		double x = i / 8.0 * M_PI;
		context.setVariable (xId, doubleValue(x));
		PrimitiveValue y = exp->eval (&context);
		double yD = y.toDouble();
		if (i % 77 == 0) ASSERT_NEAR(yD, ::sin(x), 0.1);
	}
	ASSERT_TRUE(true);
}
