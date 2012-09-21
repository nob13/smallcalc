#include <smallcalc/impl/Parser.h>
#include <smallcalc/impl/Tokenizer.h>
#include <smallcalc/smallcalc.h>
#include <gtest/gtest.h>
#include <boost/lexical_cast.hpp>

using namespace sc;

class TestParser : public testing::Test {
protected:
	/// Tokenizes string, parses it and evaulates it to a string
	std::string parseAndBack (const std::string & input) {
		return calc.parse(input)->printWithoutOptimizations();
	}
	std::string parseAndBackNice (const std::string & input) {
		sc::ExpressionPtr exp = calc.parse(input);
		return exp->printNice();
	}

	Error testError (const std::string & input) {
		return calc.parse(input)->error();
	}
	SmallCalc calc;
};

TEST_F (TestParser, ToStringComparison) {
	// Simple Numbers
	EXPECT_EQ ("2", parseAndBack ("2"));
	EXPECT_EQ ("2", parseAndBack ("(2)"));
	EXPECT_EQ ("(2 + 3)", parseAndBack ("2+3"));
	EXPECT_EQ ("-2", parseAndBack ("-2"));
}

TEST_F (TestParser, TestPrecedence) {
	EXPECT_EQ ("(2 + 3 + 2)", parseAndBack ("2+3+2"));
	EXPECT_EQ ("(2 + 3 + 4 + 5)", parseAndBack ("2+3+4+5"));
	EXPECT_EQ ("(2 + (3 * 2))", parseAndBack ("2+3*2"));
	EXPECT_EQ ("((2 * 3) + 2)", parseAndBack ("2*3+2"));
	EXPECT_EQ ("((2 * 3) + (3 / 4) + 18)", parseAndBack ("2*3+3/4+18"));
	EXPECT_EQ ("(3 * 4 * 5 * 6 * (7 ^ 2) * -3)", parseAndBack ("3*4*5*6*7^2*-3"));
	EXPECT_EQ ("((1 / 1) / 2)", parseAndBack ("1/1/2"));
	EXPECT_EQ ("(1 / (1 / 2))", parseAndBack ("1/(1/2)"));
	EXPECT_EQ ("((1 - 1) - 2)", parseAndBack ("1-1-2"));
	EXPECT_EQ ("(1 - (1 - 2))", parseAndBack ("1-(1-2)"));

	EXPECT_EQ ("3 * 4 * 5 * 6 * 7 ^ 2 * -3", parseAndBackNice ("3*4*5*6*7^2*-3"));
	EXPECT_EQ ("2 * 3 + 3 / 4 + 18", parseAndBackNice ("2*3+3/4+18"));
	EXPECT_EQ ("2 * (3 + 4 * 5) * 6", parseAndBackNice ("2*(3+4*5)*6"));

	EXPECT_EQ ("(1 / 1) / 2", parseAndBackNice ("1 / 1 / 2"));
	EXPECT_EQ ("1 / (1 / 2)", parseAndBackNice ("1 / (1 / 2)"));
	EXPECT_EQ ("(1 - 1) - 2", parseAndBackNice ("1 - 1 - 2"));
	EXPECT_EQ ("1 - (1 - 2)", parseAndBackNice ("1 - (1 - 2)"));
}

TEST_F (TestParser, TestNegation) {
	EXPECT_EQ ("-2", parseAndBack ("-2"));
	EXPECT_EQ ("(-2 + 4)", parseAndBack ("-2+4"));
	EXPECT_EQ ("(4 - -2)", parseAndBack ("4--2"));
}

TEST_F (TestParser, TestImplicitMultiplication) {
	calc.addAllStandard();
	EXPECT_EQ ("(4 * sin(x))", parseAndBack ("4sin(x)"));
	EXPECT_EQ ("(2 * (2 + 4))", parseAndBack ("2(2+4)"));
	EXPECT_EQ ("((4 ^ 2) * cos(x))", parseAndBack ("4^2cos(x)"));
	EXPECT_EQ ("(2 * e)", parseAndBack ("2e"));
}

TEST_F (TestParser, TestErrorCodes) {
	EXPECT_EQ (error::Parser_NoValidToken, testError ("2 + +")); // is accepted at the moment
	EXPECT_EQ (error::Parser_ParanthesisMismatch, testError ("(3 + 4))"));
	EXPECT_EQ (error::Parser_ParanthesisMismatch, testError ("((3+4)"));
	EXPECT_EQ (error::Parser_UnknownFunction, testError ("unknownFunction(2,3)"));

	NamedFunctionPtr uno (new NamedFunction ("uno", 1));
	calc._parserContext()->addFunction(uno);
	EXPECT_EQ (error::Parser_WrongArgumentCount, testError ("uno(2,3)"));
}

TEST_F (TestParser, TestVariable) {
	EXPECT_EQ ("x", parseAndBack ("x"));
	EXPECT_EQ ("A", parseAndBack ("A"));
}

TEST_F (TestParser, TestConstant) {
	calc._parserContext()->addConstant(createConstant("TWO", 2));
	calc._parserContext()->addConstant(createConstant("PI", 3.14159), "\\pi");
	calc._parserContext()->addConstant(createConstant("µ", 1.25), "\\mu");
	EXPECT_EQ ("TWO", parseAndBack ("TWO"));
	EXPECT_EQ ("PI", parseAndBack ("\\pi"));
	EXPECT_EQ ("µ", parseAndBack ("\\mu"));
}

TEST_F (TestParser, TestFunctions) {
	NamedFunctionPtr uno (new NamedFunction ("uno", 1));
	NamedFunctionPtr duo (new NamedFunction ("duo", 2));
	NamedFunctionPtr zro (new NamedFunction ("zro", 0));
	calc._parserContext()->addFunction (uno);
	calc._parserContext()->addFunction (duo);
	calc._parserContext()->addFunction (zro);
	EXPECT_EQ ("uno(1)", parseAndBack ("uno(1)"));
	EXPECT_EQ ("duo(1,2)", parseAndBack ("duo(1,2)"));
	EXPECT_EQ ("zro()", parseAndBack ("zro()"));

	calc.addAllStandard();
	// square root via UTF8 Character √
	EXPECT_EQ ("√(x)", parseAndBack ("√(x)"));
}

TEST_F (TestParser, TestParentheses) {
	EXPECT_EQ ("(2 + 3)", parseAndBack ("(2 + 3)"));
	EXPECT_EQ ("((2 + 3) * 4)", parseAndBack ("(2+3)*4"));
	EXPECT_EQ ("(2 * (3 + 4))", parseAndBack ("2*(3+4)"));
	EXPECT_EQ ("((2 + 3) * (4 + 5))", parseAndBack ("(2+3)*(4+5)"));
}
