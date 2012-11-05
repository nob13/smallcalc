#pragma once
#include "../Expression.h"
#include <assert.h>
#include <vector>

namespace sc {

/** Function notation. */
enum FuncNotation {
	FN_REGULAR,// func(..arguments..)
	FN_INFIX,
	FN_PREFIX, ///< Note: they need special support by tokenizer/parser
	FN_POSTFIX
};

class NamedFunction;
typedef shared_ptr<NamedFunction> NamedFunctionPtr;

/// A mathemtical function definition
/// aritiy < 0 means the argument count is variable
/// artiy == -1 means minimum 0 arguments are needed
/// arity == -2 means minimum 1 arguments
///
/// Create Expression:
///   Per default a NamedFunctionExpression will be created
/// TODO: Remove the namedfunction self reference in CreateExpressionCallback
class NamedFunction  {
public:
	typedef std::vector<PrimitiveValue> PrimitiveArgumentVector;
	typedef function<PrimitiveValue(const PrimitiveArgumentVector& arguments, const EvaluationContext* context)> EvaluationCallback;
	typedef function<ExpressionPtr (const NamedFunctionPtr &, const std::vector<ExpressionPtr> &)> CreateExpressionCallback;

	NamedFunction (
			const std::string& name,
			int arity,
			const EvaluationCallback& evaluationCallback = EvaluationCallback (),
			FuncNotation notation = FN_REGULAR,
			int precendence = 0,
			bool associative = false,
			String printingName = "") :
		mName(name),
		mArity (arity),
		mPrecendence (precendence),
		mAssociative (associative),
		mPrintingName (printingName),
		mFuncNotation (notation),
		mEvaluationCallback (evaluationCallback){
	}

	/// Overwrite default create expression callback
	void setCreateExpressionCallback (const CreateExpressionCallback & createExpressionCallback){
		mCreateExpressionCallback = createExpressionCallback;
	}

	/// Returns function name
	const String & name() const { return mName; }
	/// Returns special short name for infix notation
	const String & printingName () const { return mPrintingName;}
	/// Returns the favoured name of the function, printing name is favoured to name
	const String & favouredName () const { return mPrintingName.empty() ? mName : mPrintingName; }
	/// Returns the callback for function evaluation, if set
	const EvaluationCallback& evaluationCallback () const { return mEvaluationCallback; }
	/// Returns aritiy of the function, see description!
	int arity () const { return mArity; }
	/// Checks arity if named function can handle the given count of arguments
	bool checkArity (int count) const;
	/// Returns precedence of the function, important for non FN_REGULAR functions
	int precedence () const { return mPrecendence; }
	/// If function is assiocative
	bool isAssociative () const { return mAssociative;}
	/// Returns the notation of the function
	FuncNotation notation () const { return mFuncNotation;}

	/// Crate an expression from this function
	ExpressionPtr createExpression (const NamedFunctionPtr & me, const std::vector<ExpressionPtr>& arguments);
	/// Shortcut for creating an expression of just one argument
	ExpressionPtr createExpression (const NamedFunctionPtr & me, const ExpressionPtr & argument);
	/// Shortcut for creating an expression of just two arguments
	ExpressionPtr createExpression (const NamedFunctionPtr & me, const ExpressionPtr & argument0, const ExpressionPtr & argument2);
protected:
	const String mName;
	int mArity;
	int mPrecendence;	///< Precedence for infix functions
	bool mAssociative;  ///< If the function is associative
	String mPrintingName; ///< For not FN_REGULAR functions this is the symbol to be printeds (instead of name), not used if 0
	FuncNotation mFuncNotation;
	EvaluationCallback mEvaluationCallback;
	CreateExpressionCallback mCreateExpressionCallback;
};

/// Creates a standard named function (e.g. sin, cos)
inline NamedFunctionPtr createNamedFunction (const std::string& name, int arity, const NamedFunction::EvaluationCallback& callback, const std::string & printingName = "") {
	return NamedFunctionPtr (new NamedFunction (name, arity, callback, FN_REGULAR, 0, false, printingName));
}

/// Tool RAII struct for pushing precedence into a printing context and releasing it from return.
struct PushPrecedence {
	PushPrecedence (PrintingContext * context, int precedence) {
		this->context = context;
		this->oldPrecedence = context->currentPrecedence;
		this->context->currentPrecedence = precedence;
	}
	~PushPrecedence () {
		this->context->currentPrecedence = this->oldPrecedence;
	}
	int oldPrecedence;
	PrintingContext * context;
};


/// An expression bound to a named function
class NamedFunctionExpression : public Expression {
public:
	typedef std::vector<ExpressionPtr> ExpressionVector;
	NamedFunctionExpression (const NamedFunctionPtr & function, const ExpressionVector& arguments = ExpressionVector())
	: mFunction (function),
	  mArguments (arguments) {
		assert (mFunction->arity() < 0 || (int) mArguments.size () == mFunction->arity());
	}

	/// Adds an argument to the function; note this is only useful during parsing stage
	void addArgument (const ExpressionPtr & arg);

	/// Returns bound named function
	const NamedFunctionPtr function() const {
		return mFunction;
	}

	/// Returns argument count
	size_t argumentCount () const { return mArguments.size(); }

	/// Returns argument
	ExpressionPtr argument (size_t i) const { return mArguments[i]; }


	// Implementation of Expression
	virtual std::string print (PrintingContext * printingContext) const;
	virtual PrimitiveValue eval (EvaluationContext * calcContext) const;


private:
	NamedFunctionPtr mFunction;
	ExpressionVector mArguments;
};
typedef shared_ptr<NamedFunctionExpression> NamedFunctionExpressionPtr;

}
