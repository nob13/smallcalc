#pragma once
#include "../types.h"
#include "../Expression.h"

namespace sc {

/// A variable as an expression
class Variable : public Expression {
public:
	Variable (const String& name, VariableId id) : mName (name), mId (id) {}

	virtual PrimitiveValue eval (EvaluationContext * evaluationContext) const {
		PrimitiveValue val = evaluationContext ? evaluationContext->findVariable(mId) : PrimitiveValue();
		return val ? val : errorValue (error::Eval_UnboundVariable, "Variable " + mName + " is not bound");
	};

	virtual std::string print (PrintingContext * printingContext) const { return mName; }

	const VariableId & id () const { return mId; }

private:
	String mName;
	VariableId mId;
};


}
