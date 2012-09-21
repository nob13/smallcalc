#pragma once
#include "../types.h"
#include <vector>
#include "../Expression.h"

namespace sc {

/** Token for the tokenizer. */
struct Token {
	enum Type {
		TT_LP,         // left parenthesis
		TT_RP,         // right parenthesis
		TT_INT,        // integer number
		TT_DOUBLE,     // point number
		TT_PLUS,       // plus sign
		TT_MINUS,      // minus sign
		TT_NEGATE,     // negate sign (after other calculation signs and '(')
		TT_ASTERISK,   // *, for Multipplication
		TT_SLASH,      // /, for Divison
		TT_CIRCUMFLEX, // ^ => exponentation
		TT_COMMA,      // Comma, for function arguments
		TT_EQUALS,     // = for assignment
		TT_EXPRESSION, // an expression (cannot be parsed naturally but induced)
		TT_UNKNOWN,    // unknown, perhaps a function?
		TT_INVALID,
		TT_NULL		   // a token which got optimized away
	};

	Token ();
	Token (const std::string & text, int position);
	Token (const ExpressionPtr & expression);

	bool isNumber () const { return type == TT_INT || type == TT_DOUBLE; }

	std::string text;
	int position;
	Type type;
	/// plain types
	union {
		double vDouble;
		int64_t vInt;
	};
	ExpressionPtr expression;
};

/// Simple tokenizer
class Tokenizer {
public:
	Tokenizer () {
		mErrorPosition = -1;
	}
	Error tokenize (const std::string & input);
	const std::vector <Token> & result () { return mResult; }
	std::string errorMessage () const { return mErrorMessage; }
	int errorPosition () const { return mErrorPosition; }
private:
	// Push a token to the result; its possible to combine it with others
	void pushToken (const std::string & s, int position);
	/// Search for minus signs which are negations and replace thems
	/// Suitable negations will be merged with following numbers
	void fixupNegations ();
	/// Remove null tokens which got in there during greedy parsing / negation handling
	void removeNull ();
	std::vector<Token> mResult;
	std::string mErrorMessage;
	int mErrorPosition;
};


}
