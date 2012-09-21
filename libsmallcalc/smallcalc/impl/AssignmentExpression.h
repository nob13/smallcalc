#pragma once
#include "../Expression.h"
#include "NamedFunction.h" // for push precedence

namespace sc {

/**
 * Expression for assignment of variables
 *
 * e.g.
 * x = 3
 */
class AssignmentExpression : public Expression {
public:
	AssignmentExpression (const ExpressionPtr & variable, const ExpressionPtr & argument) : mVariable (variable), mArgument (argument) {}
	virtual ~AssignmentExpression (){}

	// Implementation of Expression
	virtual PrimitiveValue eval (EvaluationContext * evaluationContext) const;
	virtual std::string print (PrintingContext * printingContext) const;

private:
	ExpressionPtr mVariable;
	ExpressionPtr mArgument;
};

}
