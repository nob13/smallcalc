#include <gtest/gtest.h>
#include <smallcalc/smallcalc.h>
#include <smallcalc/impl/Parser.h>

using namespace sc;

class TestEval : public testing::Test {
protected:
	double evalToDouble (const std::string & s){
		return calc.eval (s).toDouble();
	}
	Error evalToError (const std::string & s) {
		return calc.eval (s).error();
	}
	PrimitiveValue eval (const std::string & s) {
		return calc.eval(s);
	}

	SmallCalc calc;
};

TEST_F (TestEval, simpleEval) {
	EXPECT_EQ (2, evalToDouble("2"));
	EXPECT_EQ (4, evalToDouble("2*2"));
	EXPECT_EQ (8, evalToDouble("2*2*2"));
	EXPECT_EQ (10, evalToDouble("2*3 + 4"));
	EXPECT_EQ (14, evalToDouble("2+3*4"));
	EXPECT_EQ (16, evalToDouble("2+3*4+2"));
	EXPECT_EQ (-2, evalToDouble("-2"));
	EXPECT_EQ (4, evalToDouble("2^2"));
	EXPECT_EQ(0.25, evalToDouble("2^-2"));
}

TEST_F (TestEval, constantEval) {
	calc._parserContext()->addConstant(createConstant("µ", 2), "\\mu");
	EXPECT_EQ (4, evalToDouble ("2*\\mu"));
}

TEST_F (TestEval, errors) {
	EXPECT_EQ (error::Eval_UnboundVariable, evalToError("4*x"));
	EXPECT_EQ (error::Eval_BadType, evalToError ("3=4"));
	calc.addAllStandard();
	EXPECT_EQ (error::Eval_BadType, evalToError ("PI=4"));
}

TEST_F (TestEval, variables) {
	EXPECT_EQ (error::Eval_UnboundVariable, evalToError("4*x"));
	EXPECT_EQ (4, evalToDouble("x=4"));
	EXPECT_EQ (4, evalToDouble("x"));
	EXPECT_EQ (5, evalToDouble ("x=3*4-7"));
	EXPECT_EQ (6, evalToDouble ("x+1"));
}

TEST_F (TestEval, functions) {
	calc.addAllStandard();
	EXPECT_EQ (0, evalToDouble ("round(sin(0))"));
	EXPECT_EQ (-1, evalToDouble ("round(cos(PI))"));
	EXPECT_EQ (0, evalToDouble ("round(sin(0))"));
	EXPECT_EQ (1, evalToDouble("round(tan(PI/4))"));
	EXPECT_EQ (2, evalToDouble("round(sqrt(4))"));

	EXPECT_NEAR(M_PI / 2, evalToDouble ("acos(0)"), 0.1);
	EXPECT_NEAR(M_PI / 2, evalToDouble ("asin(1)"), 0.1);
	EXPECT_NEAR(M_PI / 4, evalToDouble ("atan(1)"), 0.1);

	EXPECT_NEAR(M_PI / 2, evalToDouble ("cosh(1)"), 0.1);
	EXPECT_NEAR(3.626, evalToDouble ("sinh(2)"), 0.1);
	EXPECT_NEAR(0.964, evalToDouble ("tanh(2)"), 0.1);

	EXPECT_NEAR(1.0, evalToDouble ("asinh(1.1752011936438014)"), 0.0001);
	EXPECT_NEAR(1.0, evalToDouble ("acosh(1.5430806348152437)"), 0.0001);
	EXPECT_NEAR(1.0, evalToDouble ("atanh(0.76159415595576485)"), 0.0001);

	EXPECT_NEAR(2, evalToDouble ("ln(e^2)"), 0.001);
	EXPECT_NEAR(3.4, evalToDouble ("abs(-3.4)"), 0.001);
}

TEST_F (TestEval, accurateEval) {
	calc.setAccurateLevel(true);
	EXPECT_EQ (PrimitiveValue ((int64_t)1), eval("2/2"));
	EXPECT_EQ (PrimitiveValue (Fraction64(2,3)), eval ("2/3"));
	EXPECT_EQ (PrimitiveValue (Fraction64(2,3)), eval ("4/6"));
	EXPECT_EQ (PrimitiveValue (Fraction64(-1,5)), eval ("2/-10"));
	EXPECT_EQ (PrimitiveValue ((int64_t)1), eval ("4/5 + 1/5"));
	EXPECT_EQ (PrimitiveValue (Fraction64(4,5)), eval ("1 - 1/5"));
	EXPECT_EQ (PrimitiveValue (Fraction64(-1,5)), eval ("1-6/5"));
	EXPECT_EQ (PrimitiveValue (error::Eval_DivisionByZero, ""), eval ("1/0"));
	EXPECT_EQ (PrimitiveValue (Fraction64(1,3)), eval ("2/6"));
	EXPECT_EQ (PrimitiveValue ((int64_t)2), eval ("2"));
	EXPECT_EQ (PrimitiveValue (Fraction64(2,3)), eval ("2/6 * 2"));
	EXPECT_EQ (PrimitiveValue (Fraction64(1,3)), eval ("1/2 * 2/3"));


	EXPECT_EQ (PrimitiveValue (1L), eval ("1^0"));
	EXPECT_EQ (PrimitiveValue (1L), eval ("1^1"));
	EXPECT_EQ (PrimitiveValue (1L), eval ("1^2"));

	EXPECT_EQ (PrimitiveValue (4L), eval ("2^2"));
	EXPECT_EQ (PrimitiveValue (8L), eval ("2^3"));

	EXPECT_EQ (PrimitiveValue (Fraction64(1,2)), eval ("2^-1"));
	EXPECT_EQ (PrimitiveValue (Fraction64(1,4)), eval ("2^-2"));
	EXPECT_EQ (PrimitiveValue (Fraction64(1,8)), eval ("2^-3"));

	EXPECT_EQ (eval ("(1/2)^2"), PrimitiveValue (Fraction64(1,4)));
	EXPECT_EQ (eval ("(1/2)^3"), PrimitiveValue (Fraction64(1,8)));
	EXPECT_EQ (eval ("(1/2)^-2"), PrimitiveValue (Fraction64(4)));
	EXPECT_EQ (eval ("(1/2)^-3"), PrimitiveValue (Fraction64(8)));
	EXPECT_EQ (eval ("(1/10)^18"), PrimitiveValue (Fraction64 (1,1000000000000000000L)));
}

TEST_F (TestEval, ranges) {
	calc.setAccurateLevel(true);
	EXPECT_NEAR (1.0e21, evalToDouble ("1e21"), 1e5);
	EXPECT_NEAR (1.0e21, evalToDouble ("1000000000000000000000"), 1e5);
	EXPECT_NEAR (1.0e24, evalToDouble ("1000*1000*1000*1000*1000*1000*1000*1000"), 1);
	EXPECT_NEAR (-1.0e24, evalToDouble ("-1000*1000*1000*1000*1000*1000*1000*1000"), 1);
	EXPECT_NEAR (evalToDouble ("1/100000000000000000 + 1 / (100000000000000000 + 1)"), 2e-17,   1e-30);
	// EXPECT_NEAR (evalToDouble ("1/100000000000000000 - 1 / (100000000000000000 + 1)"), 1e-34,   1e-40); // this exceeds accuracy of double
	EXPECT_NEAR (evalToDouble ("1/100000000000000000 - 1 / (100000000000000000 + 1)"), 0,   1e-34);
	EXPECT_NEAR (evalToDouble ("(1/10)^21"), 1e-21, 1e-23);
}
