#include "smallcalc.h"
#include "impl/Tokenizer.h"
#include "impl/Parser.h"
#include <math.h>
#include "impl/NamedFunction.h"
#include "impl/StandardFunctions.h"
#include "impl/AssignmentExpression.h"

namespace sc {

SmallCalc::SmallCalc () {
	mParserContext = new ParserContext();
	mParserContext->variableMapping = &mVariableIdMapping;
	addFundamentalFunctions();
}
SmallCalc::~SmallCalc () {
	delete mParserContext;
}

void SmallCalc::addStandardConstants () {
	mParserContext->addConstant(createConstant ("π", M_PI), "PI");
	mParserContext->addConstant(createConstant ("e", M_E), "E");
}

void SmallCalc::addStandardFunctions () {
	mParserContext->addFunction (createNamedFunction("sin", 1, &sc::sin));
	mParserContext->addFunction (createNamedFunction("cos", 1, &sc::cos));
	mParserContext->addFunction (createNamedFunction("tan", 1, &sc::tan));
	mParserContext->addFunction (createNamedFunction("round", 1, &sc::round));
	mParserContext->addFunction (createNamedFunction("sqrt", 1, &sc::sqrt, "√"));

	mParserContext->addFunction (createNamedFunction("acos", 1, &sc::acos));
	mParserContext->addFunction (createNamedFunction("asin", 1, &sc::asin));
	mParserContext->addFunction (createNamedFunction("atan", 1, &sc::atan));

	mParserContext->addFunction (createNamedFunction("cosh", 1, &sc::cosh));
	mParserContext->addFunction (createNamedFunction("sinh", 1, &sc::sinh));
	mParserContext->addFunction (createNamedFunction("tanh", 1, &sc::tanh));
	mParserContext->addFunction (createNamedFunction("acosh", 1, &sc::acosh));
	mParserContext->addFunction (createNamedFunction("asinh", 1, &sc::asinh));
	mParserContext->addFunction (createNamedFunction("atanh", 1, &sc::atanh));

	mParserContext->addFunction (createNamedFunction("ln", 1, &sc::ln));

	mParserContext->addFunction (createNamedFunction("abs", 1, &sc::abs));
}

void SmallCalc::addAllStandard () {
	addStandardConstants();
	addStandardFunctions();
}


PrimitiveValue SmallCalc::eval (const std::string & input) {
	mLastExpression = parse (input);
	return mLastExpression->eval(&mEvaluationContext);
}

ExpressionPtr SmallCalc::parse (const std::string & input) {
	Tokenizer tokenizer;
	Error e = tokenizer.tokenize(input);
	if (e) {
		return createError(e, tokenizer.errorMessage());
	}
	Parser parser (mParserContext);
	return parser.parse (tokenizer.result());
}

static ExpressionPtr createAssignmentExpression (const NamedFunctionPtr & func, const std::vector<ExpressionPtr> & arguments) {
	assert (arguments.size() == 2);
	return ExpressionPtr (new AssignmentExpression(arguments[0], arguments[1]));
}

void SmallCalc::addFundamentalFunctions () {
	mParserContext->addFunction (NamedFunctionPtr(new NamedFunction ("add", -1, &sc::add, FN_INFIX, 2, true, "+")));
	mParserContext->addFunction (NamedFunctionPtr(new NamedFunction ("multiply", -1, &sc::multiply, FN_INFIX, 3, true, "*")));
	mParserContext->addFunction (NamedFunctionPtr(new NamedFunction ("subtract", 2, &sc::subtract, FN_INFIX, 2, false, "-")));
	mParserContext->addFunction (NamedFunctionPtr(new NamedFunction ("divide", 2, &sc::divide, FN_INFIX, 3, false, "/")));
	mParserContext->addFunction (NamedFunctionPtr(new NamedFunction ("negate", 1, &sc::negate, FN_PREFIX, 10, false, "-")));
	mParserContext->addFunction (NamedFunctionPtr(new NamedFunction ("pow", 2, &sc::exponentation, FN_INFIX, 4, false, "^")));

	// Assignment is trickier
	NamedFunctionPtr assignment  (new NamedFunction ("assignment", 2, 0, FN_INFIX, 1, false, "="));
	assignment->setCreateExpressionCallback(&createAssignmentExpression);
	mParserContext->addFunction(assignment);
}

ExpressionPtr parse (const std::string & input) {
	SmallCalc calc;
	return calc.parse(input);
}

PrimitiveValue eval (const std::string & input) {
	SmallCalc calc;
	return calc.eval (input);
}


}
