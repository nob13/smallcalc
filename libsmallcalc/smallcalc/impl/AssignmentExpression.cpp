#include "AssignmentExpression.h"
#include "Variable.h"

namespace sc {

PrimitiveValue AssignmentExpression::eval (EvaluationContext * evaluationContext) const {
	shared_ptr<Variable> var = boost::dynamic_pointer_cast<Variable>(mVariable);
	if (!var) return errorValue (error::Eval_BadType, "Variable expected on left side");
	PrimitiveValue right = mArgument->eval(evaluationContext);

	if (right.error()) return right;
	evaluationContext->setVariable (var->id(), right);
	return right;
}

std::string AssignmentExpression::print (PrintingContext * context) const {
	PushPrecedence push (context, 0);
	return mVariable->print(context) + " = " + mArgument->print(context);
}

}
