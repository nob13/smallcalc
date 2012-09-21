#pragma once
#include "types.h"
#include "PrimitiveValue.h"
#include <vector>

namespace sc {
/**
 * The evaluation context stores the context which is used during the evaluation process of an expression
 * An example are variable values.
 */
struct EvaluationContext {
	EvaluationContext () { variables.resize (64); accurateLevel = false; }
	const PrimitiveValue& findVariable (const VariableId & id) {
		static PrimitiveValue invalidValue;
		if (id < 0 || (size_t) id >= variables.size()) return invalidValue;
		return variables[id];
	}
	void setVariable (const VariableId & id, const PrimitiveValue & val) {
		assert (id >= 0);
		if ((size_t) id >= variables.size()){
			variables.resize (variables.size() * 2);
		}
		variables[id] = val;
	}

	/// variable_id to primitive value mapping
	std::vector<PrimitiveValue> variables;
	/// try to calculate with accurate values
	bool accurateLevel;
};

/// Context for printing.
/// This context is used to nicely format inside print()
/// This can be disabled
struct PrintingContext {
	PrintingContext () : precedenceOptimization (true), currentPrecedence (0) {}
	bool precedenceOptimization; ///< Skip braces if function has higher precedence
	int  currentPrecedence;
};

/// Simple expression
class Expression {
public:
	virtual ~Expression () {}
	/// Standard calculation method
	virtual PrimitiveValue eval (EvaluationContext * evaluationContext) const = 0;
	/// standard printing method, no further formatting
	virtual std::string print (PrintingContext * printingContext) const = 0;

	virtual std::string printNice () const {
		PrintingContext defaultContext;
		return print(&defaultContext);
	}

	virtual std::string printWithoutOptimizations (){
		PrintingContext context;
		context.precedenceOptimization = false;
		return print (&context);
	}

	/// Fast check if its an error
	virtual Error error () const { return NoError; }
};
typedef shared_ptr<Expression> ExpressionPtr;

/// Shortcut for creating error expressions
ExpressionPtr createError (Error e, const String & message = String());


}
