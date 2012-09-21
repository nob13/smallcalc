#include "Tokenizer.h"
#include <boost/lexical_cast.hpp>

namespace sc {

template <class T>
bool tryCast (const std::string & input, T * value = 0) {
	try {
		if (value == 0) {
			boost::lexical_cast<T> (input);
		} else {
			*value = boost::lexical_cast<T> (input);
		}
	} catch (...){
		return false;
	}
	return true;
}

Token::Token () {
	position = 0;
	type = Token::TT_INVALID;
}

Token::Token (const std::string & input, int pos) {
	this->text = input;
	this->position = pos;

	if (input == "(") {
		type = Token::TT_LP;
		return;
	}
	if (input == ")") {
		type = Token::TT_RP;
		return;
	}
	if (input == "+") {
		type = Token::TT_PLUS;
		return;
	}
	if (input == "-") {
		type = Token::TT_MINUS;
		return;
	}
	if (input == "*") {
		type = Token::TT_ASTERISK;
		return;
	}
	if (input == "^"){
		type = Token::TT_CIRCUMFLEX;
		return;
	}
	if (input == "/") {
		type = Token::TT_SLASH;
		return;
	}
	if (input == ",") {
		type = Token::TT_COMMA;
		return;
	}
	if (input == "=") {
		type = Token::TT_EQUALS;
		return;
	}
	if (tryCast<int64_t> (input, &vInt)){
		type = Token::TT_INT;
		return;
	}
	if (tryCast<double> (input, &vDouble)){
		type = Token::TT_DOUBLE;
		return;
	}
	type = Token::TT_UNKNOWN;
}

Token::Token (const ExpressionPtr & expression) {
	type = Token::TT_EXPRESSION;
	this->expression = expression;
}

static bool isEmpty (char c) {
	return c == ' ' || c == '\n' || c == '\t' || c == '\r';
}

/// Character forming a single token by itself
static bool isSingleChar (char c) {
	return c == '(' || c == ')' || c =='+' || c == '-' || c == '*' || c == '/' || c == '!' || c == '^' || c == ',' || c == '=';
}

/// Token of the type NumberText can be split into Number Text
static bool splitToNumAndText (const Token & token, Token * num = 0, Token * text = 0) {
	if (token.type != Token::TT_UNKNOWN) return false;

	bool foundNum  = false;
	bool foundExp  = false; // found exponent symbol
	bool foundDot  = false;
	bool foundSign = false;
	size_t i = 0;
	for (i = 0; i < token.text.length(); i++) {
		char c = token.text[i];
		if (c >= '0' && c <= '9') {
			foundNum = true;
			continue;
		}
		if ((c == '-' || c == '+') && !foundSign) {
			foundSign = true;
			continue;
		}
		if (c == 'e' && foundNum && !foundExp) {
			foundExp = true;
			foundSign = false; // may await another sign
			foundNum = false; //  must await another num
			continue;
		}
		if (c == '.' && !foundDot && !foundExp) { // Only one dot, exponent has no number
			foundDot = true;
			continue;
		}
		break;
	}
	if (i == token.text.length() && foundExp && !foundNum && token.text[token.text.length() -1 ]) {
		// something like 2341e --> multiplication with euler number, this actually works
		foundNum = true;
		i--;
	}
	if (!foundNum) return false; // found not even a number
	if (i == token.text.length()) return false; // did not found a number but iam at the end?
	if (num) {
		*num = Token (token.text.substr (0,i), token.position);
	}
	if (text) {
		*text = Token (token.text.substr(i), token.position + i);
	}
	return true;
}

Error Tokenizer::tokenize (const std::string & input) {
	bool inEmpty = true;
	int tokenStart = 0;
	for (int i = 0; i < (int) input.length(); i++) {
		char c = input[i];
		if (inEmpty){
			if (isEmpty (c)) continue;
			tokenStart = i;
			inEmpty = false;
		}
		if (isEmpty (c)) {
			// last token is at its end
			pushToken (input.substr (tokenStart, i - tokenStart), tokenStart);
			inEmpty = true;
			// add null token (will get removed at end)
			Token nullToken (" ", i);
			nullToken.type = Token::TT_NULL;
			mResult.push_back (nullToken);
		}
		if (isSingleChar (c)) {
			if (i - tokenStart > 0) {
				pushToken (input.substr (tokenStart, i - tokenStart), tokenStart);
			}
			pushToken (input.substr (i, 1), i);
			inEmpty = true; // ignores following empties
		}
	}
	// last character
	if (!inEmpty){
		pushToken (input.substr (tokenStart), tokenStart);
	}
	fixupNegations ();
	removeNull ();
	return NoError;
}

// Push a token to the result; its possible to combine it with others
void Tokenizer::pushToken (const std::string & s, int position) {
	Token token = Token (s, position);
	size_t size = mResult.size();

	// Find exponential numbers via greedy combining
	if (token.type == Token::TT_INT) {
		Token invalidToken;
		Token & first  = size >= 3 ? mResult[size-3] : invalidToken;
		Token & second = size >= 2 ? mResult[size-2] : invalidToken;
		Token & third  = size >= 1 ? mResult[size-1] : invalidToken;

		if (size >= 3) {
			// 3e+4
			Token expNumber (first.text + second.text + third.text + s, first.position);
			if (expNumber.type == Token::TT_DOUBLE) {
				mResult.pop_back();
				mResult.pop_back();
				mResult.pop_back();
				mResult.push_back (expNumber);
				return;
			}
		}
		if (size >= 2) {
			// 3 e 4
			Token expNumber (second.text + third.text + s, second.position);
			if (expNumber.type == Token::TT_DOUBLE) {
				mResult.pop_back ();
				mResult.pop_back ();
				mResult.push_back (expNumber);
				return;
			}
		}
	}

	// Try to split up 3sin --> 3*sin
	if (splitToNumAndText (token)) {
		Token numToken;
		Token textToken;
		splitToNumAndText(token, &numToken, &textToken);
		mResult.push_back (numToken);
		mResult.push_back (textToken);
	} else {
		mResult.push_back (token);
	}
}


/// The next '-' could be a negate
static bool nextCouldBeNegate (const Token & lastToken) {
	return
			lastToken.type == Token::TT_LP ||
			lastToken.type == Token::TT_PLUS  ||
			lastToken.type == Token::TT_MINUS ||
			lastToken.type == Token::TT_ASTERISK ||
			lastToken.type == Token::TT_NEGATE ||
			lastToken.type == Token::TT_SLASH ||
			lastToken.type == Token::TT_CIRCUMFLEX ||
			lastToken.type == Token::TT_COMMA ||
			lastToken.type == Token::TT_EQUALS;
}

void Tokenizer::fixupNegations () {
	if (mResult.size() < 2) return;
	// Phase 1 Find negations
	bool first = true; // first can also be a  token if a pure "-"
	for (std::vector<Token>::iterator i = mResult.begin(); i != mResult.end(); i++) {
		Token & current (*i);
		if (current.type == Token::TT_MINUS && (first || nextCouldBeNegate (*(i-1)))){
			current.type = Token::TT_NEGATE;
		}
		first = false;
	}
	// Phase 2 Replace negations
	Token invalidToken;
	for (std::vector<Token>::iterator i = mResult.begin(); i != mResult.end(); i++) {
		Token & current (*i);
		Token & next (i + 1 != mResult.end() ? *(i+1) : invalidToken);
		if (current.type == Token::TT_NEGATE){
			Token::Type oldNextType = next.type;
			if (next.type == Token::TT_INT || next.type == Token::TT_DOUBLE){
				// merge them
				next = Token ("-" + next.text, current.position);
				assert (oldNextType == next.type); // type shall not change if I put a '-' before it.
				current.type = Token::TT_NULL;
			}
		}
	}
}

void Tokenizer::removeNull () {
	std::vector<Token> newResult;
	newResult.reserve (mResult.size());
	for (std::vector<Token>::const_iterator i = mResult.begin(); i != mResult.end(); i++){
		const Token & current (*i);
		if (current.type != Token::TT_NULL){
			newResult.push_back (current);
		}
	}
	mResult.swap(newResult);
}


}
