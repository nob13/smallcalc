#pragma once
#include "types.h"
#include "Expression.h"

namespace sc {

class ParserContext;

/// Smallcalc  including context
/// Note: this is stateful and NOT threadsafe
class SmallCalc {
public:
	SmallCalc ();
	~SmallCalc ();
	/// Add standard constans like pi or e
	void addStandardConstants ();
	/// Add standard functions like sin, cos, tan
	void addStandardFunctions ();

	/// Add all standard constants / functions
	void addAllStandard ();

	PrimitiveValue eval (const std::string & input);
	ExpressionPtr parse (const std::string & input);

	/// Returns variable id of a given variable
	VariableId idOfVariable (const String & variableName) const { return mVariableIdMapping.variableIdFor(variableName); }

	/// Sets a variable
	void setVariable (const VariableId & id, const PrimitiveValue & value) { return mEvaluationContext.setVariable(id, value); }

	/// Returns evaluation of last expression
	const ExpressionPtr& lastExpression () const { return mLastExpression; }

	/// Returns parser context (I hope you know what you are doing!)
	ParserContext * _parserContext () { return mParserContext; }

	/// Enables/Disables accurate level for calculation, default is disabled.
	void setAccurateLevel (bool v = true) { mEvaluationContext.accurateLevel = v; }
private:

	/// inserts fundamental functions (they are always inserted!)
	void addFundamentalFunctions ();
	// forbidden
	void operator= (const SmallCalc &);
	SmallCalc (const SmallCalc&);

	EvaluationContext mEvaluationContext;
	ParserContext * mParserContext;
	ExpressionPtr mLastExpression;
	VariableIdMapping mVariableIdMapping;
};

/// Parses an expression
/// If there was an error; you will get an Error encoded in a primitive error value
ExpressionPtr parse (const std::string & input);

/// Tries to evaluate a simple expression. If there was an error you will get a primitive error value
PrimitiveValue eval (const std::string & input);

}
