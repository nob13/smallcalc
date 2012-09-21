#include "NamedFunction.h"

namespace sc {

bool NamedFunction::checkArity (int count) const {
	if (mArity >= 0) return mArity == count;
	else return (0-mArity) - 1 <= count;
}

ExpressionPtr NamedFunction::createExpression (const NamedFunctionPtr & me, const std::vector<ExpressionPtr>& arguments) {
	assert (checkArity ((int)arguments.size()));
	if (mCreateExpressionCallback) {
		return mCreateExpressionCallback (me, arguments);
	} else {
		return ExpressionPtr (new NamedFunctionExpression (me, arguments));
	}
}

ExpressionPtr NamedFunction::createExpression (const NamedFunctionPtr & me, const ExpressionPtr & argument) {
	std::vector<ExpressionPtr> args;
	args.push_back (argument);
	return createExpression (me, args);
}

ExpressionPtr NamedFunction::createExpression (const NamedFunctionPtr & me, const ExpressionPtr & argument0, const ExpressionPtr & argument1) {
	std::vector<ExpressionPtr> args;
	args.push_back (argument0);
	args.push_back (argument1);
	return createExpression (me, args);
}

void NamedFunctionExpression::addArgument (const ExpressionPtr & arg) {
	assert (mFunction->arity() < 0); /// must be able to hold multiple arguments
	mArguments.push_back (arg);
}

std::string NamedFunctionExpression::print (PrintingContext * printingContext) const {
	bool canSkipParenthesis = false;
	if (printingContext && printingContext->precedenceOptimization) {
		if (mFunction->precedence() > printingContext->currentPrecedence) {
			canSkipParenthesis = true;
		}
		if (mFunction->isAssociative() && (mFunction->precedence() == printingContext->currentPrecedence)) {
			canSkipParenthesis = true;
		}
	}
	PushPrecedence push (printingContext, mFunction->precedence());
	std::ostringstream ss;
	if (mFunction->notation() == FN_INFIX){
		// Infix like +-*/
		if (!canSkipParenthesis) ss << "(";
		bool first = true;
		for (ExpressionVector::const_iterator i = mArguments.begin(); i != mArguments.end(); i++){
			if (!first) { ss << " " << mFunction->favouredName() << " ";}
			first = false;
			ss << (*i)->print(printingContext);
		}
		if (!canSkipParenthesis) ss << ")";
		return ss.str();
	}
	if (mFunction->notation() == FN_PREFIX && mArguments.size() == 1){
		// Prefix like (-2)
		if (!canSkipParenthesis) ss << "(";
		ss << mFunction->favouredName() << mArguments[0]->print(printingContext);
		if (!canSkipParenthesis) ss << ")";
		return ss.str();
	}

	// Default / Fallback
	ss << mFunction->favouredName() << "(";
	bool first = true;
	for (ExpressionVector::const_iterator i = mArguments.begin(); i != mArguments.end(); i++){
		if (!first) { ss << ",";}
		first = false;
		ss << (*i)->print(printingContext);
	}
	ss << ")";
	return ss.str();
}

PrimitiveValue NamedFunctionExpression::eval (EvaluationContext * calcContext) const {
	NamedFunction::PrimitiveArgumentVector parguments;
	parguments.reserve (mArguments.size());
	for (ExpressionVector::const_iterator i = mArguments.begin(); i != mArguments.end(); i++) {
		PrimitiveValue val ((*i)->eval(calcContext));
		parguments.push_back (val);
	}
//	std::cout << "Eval " << this->function()->name() << std::endl;
//	BOOST_FOREACH(PrimitiveValue & val, parguments){
//		std::cout << "   val " << val.toString() << std::endl;
//	}
	return mFunction->evaluationCallback() (parguments, calcContext);
}


}
