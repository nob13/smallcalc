#pragma once
#include "../types.h"
#include "Tokenizer.h"
#include "../Expression.h"
#include <vector>
#include <stack>
#include "Constant.h"
#include "NamedFunction.h"

namespace sc {


/// Context for parsing
struct ParserContext {
	ParserContext () {}

	/// Add a constant wit hup to 3 alternative names
	/// The real name of the constant will always be inserted
	void addConstant (ConstantPtr constant, const char * alt0 = 0, const char * alt1 = 0, const char * alt2 = 0);
	ConstantPtr findConstant (const std::string & name) const { return findValuePtr (constants, name); }

	void addFunction (const NamedFunctionPtr& function);
	NamedFunctionPtr findFunction (const String&name) const { return findValuePtr (functions, name); }

	NamedFunctionPtr findNonPrefixFunction (const String & name) const { return findValuePtr (nonPrefixFunctions, name); }

	typedef boost::unordered_map<String, ConstantPtr> ConstantMap;
	typedef boost::unordered_map<String, NamedFunctionPtr> NamedFunctionMap;
	ConstantMap constants;	///< Registered constans
	NamedFunctionMap functions; ///< Registered named functions
	NamedFunctionMap nonPrefixFunctions; ///< Regirstered named functions who have a printing name and are not prefix

	VariableIdMapping * variableMapping;
};

/// Parser for smallscalc
/// Use it only once and then throw it away
/// Parsing algorithm:
/// Conversion to postfix notation (parse) and then interpreting via compress
/// Also see: http://scriptasylum.com/tutorials/infix_postfix/algorithms/infix-postfix/index.htm
class Parser {
public:
	Parser (const ParserContext * context);

	/// Parses a token stream into a expression
	/// Only to be used once
	ExpressionPtr parse (const std::vector<Token> & tokens);

	int errorPosition () const {
		return mErrorPosition;
	}
	Error error () const {
		return mError;
	}
	std::string errorMessage() const {
		return mErrorMessage;
	}

private:

	/// Compress current expression which is now in postfix notation
	ExpressionPtr compress ();

	/// Compress current expression which is known to be a function
	ExpressionPtr compressFunction ();

	/// Insert a command (pop elements from stack if necessary)
	void insertCommandInStack (const Token & token);

	/// Finalizes the current state (moves operations to the end of the postfix notation)
	void finalizeCurrentState ();

	/// Token is a regular argument (number or variable or encapsualed expression)
	bool isArgumentToken (const Token & t) const;

	/// Converts regular arguments to expressions
	ExpressionPtr convertArgumentToken (const Token & t) const;

	/// Token is a function
	bool isFunction (const Token & t) const;

	/// Find non regular function for token (for infix,postfix, prefix)
	NamedFunctionPtr findNonRegular (const Token & t) const;
	/// Find function for a regular funciton token
	NamedFunctionPtr findRegular (const Token & t) const;

	/// Stores an error and returns an encapsulated one in an ErrorExpression
	ExpressionPtr createError (Error e, const String & message, int position);


	struct State {
		State () : isFunction (false), begin(0) {}
		void clear () { isFunction = false; tokens.clear(); commandStack.clear(); begin = 0; }
		bool empty() const { return tokens.empty() && commandStack.empty(); }
		bool isFunction;				///< State is inside a funciton argument list
		int begin;
		std::vector<Token> tokens; /// tokens in postfix notation
		std::vector<Token> commandStack; /// current stack
	};

	String mErrorMessage;
	State mCurrentState;			///< Current state
	std::stack<State> mStateStack;	///< Stack of states while inside braces
	Error mError;
	int mErrorPosition;
	const ParserContext * mContext;
};

}
