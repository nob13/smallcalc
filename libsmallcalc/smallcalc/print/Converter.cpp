#include "Converter.h"
#include "../impl/NamedFunction.h"
#include "../impl/Value.h"
#include "BoxElements.h"
#include "SpaceStack.h"

namespace sc {

BoxPtr convertPrimitiveValue (const PrimitiveValue& value) {
	if (value.type() == PT_FRACTION) {
		Fraction64 frac = value.toFraction();
		int64_t num = frac.numerator();
		int64_t den = frac.denumerator();
		bool negate = num < 0;
		if (negate) num = -num;
		BoxPtr fractionBox = boost::make_shared<FractionBox> (
				boost::make_shared<TextBox> (boost::lexical_cast<std::string> (num)),
				boost::make_shared<TextBox> (boost::lexical_cast<std::string> (den)));
		if (negate) {
			return boost::make_shared<RegularFunctionBox> ("-", fractionBox);
		} else {
			return fractionBox;
		}
	}
	// Fallback
	return boost::make_shared<TextBox> (value.toString());
}

BoxPtr convertExpression (ExpressionPtr exp, int currentPrecedence) {
	NamedFunctionExpressionPtr namedExp = boost::dynamic_pointer_cast<NamedFunctionExpression>(exp);
	if (namedExp) {
		NamedFunctionPtr func = namedExp->function();
		if (func->name() == "divide" && namedExp->argumentCount() == 2) {
			return boost::make_shared<FractionBox> (convertExpression (namedExp->argument(0)), convertExpression(namedExp->argument(1)));
		} else if (func->notation() == FN_INFIX) {

			bool canSkipParanthesis = false;
			if (func->precedence() > currentPrecedence) {
				canSkipParanthesis = true;
			}
			if (func->precedence() == currentPrecedence && func->isAssociative()) {
				canSkipParanthesis = true;
			}

			shared_ptr<InfixFunctionBox> infixBox (new InfixFunctionBox(func->printingName()));
			for (size_t i = 0; i < namedExp->argumentCount(); i++) {
				infixBox->addArgument(convertExpression (namedExp->argument(i), func->precedence()));
			}

			if (!canSkipParanthesis){
				return BoxPtr (new ParanthesisBox(infixBox));
			}
			return infixBox;
		} else if (func->notation() == FN_REGULAR) {
			RegularFunctionBuilder builder (func->favouredName());
			for (size_t i = 0; i < namedExp->argumentCount(); i++) {
				builder.addArgument(convertExpression (namedExp->argument(i), func->precedence()));
			}
			return builder.result();
		}
	}
	ValuePtr value = boost::dynamic_pointer_cast<Value> (exp);
	if (value) {
		return convertPrimitiveValue (value->value());
	}
	// Fallback
	return boost::make_shared<TextBox> (exp->printNice());
}

}
