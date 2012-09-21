#include "Parser.h"
#include "Variable.h"
#include "../Expression.h"
#include "../PrimitiveValue.h"
#include "Value.h"
#include "Tokenizer.h"
#include <assert.h>

namespace sc {

void ParserContext::addConstant (ConstantPtr constant, const char * alt0, const char * alt1, const char * alt2){
	constants[constant->name()] = constant;
	if (alt0){
		constants[alt0] = constant;
	}
	if (alt1){
		constants[alt1] = constant;
	}
	if (alt2){
		constants[alt2] = constant;
	}
}

void ParserContext::addFunction (const NamedFunctionPtr& function) {
	functions[function->name()] = function;
	if (function->notation() != FN_PREFIX && !function->printingName().empty()){
		nonPrefixFunctions[function->printingName()] = function;
	}
}


Parser::Parser (const ParserContext * context) {
	mErrorPosition = 0;
	mError = NoError;
	mContext = context;
}

ExpressionPtr Parser::parse (const std::vector<Token> & tokens) {
	if (tokens.empty()) return createError (error::Parser_NoTokens, "No input", 0);
	bool awaitFunction = false;
	Token tokenForAwaitFunction;
	Token invalidToken;
	for (std::vector<Token>::const_iterator i = tokens.begin(); i != tokens.end(); i++) {
		const Token & ct (*i);
		const Token & nextToken (i + 1 != tokens.end() ? *(i+1) : invalidToken);
		if (ct.type == Token::TT_LP) {
			mStateStack.push (mCurrentState);
			mCurrentState.clear();
			mCurrentState.begin = ct.position;
			if (awaitFunction) {
				mCurrentState.isFunction = true;
				mCurrentState.commandStack.push_back (tokenForAwaitFunction);
				awaitFunction = false;
				tokenForAwaitFunction = Token ();
				mStateStack.push (mCurrentState);
				mCurrentState.clear();
			}
			continue;
		}
		if (awaitFunction) {
			return createError (error::Parser_NoValidToken, "Awaited function arguments for " + tokenForAwaitFunction.text, ct.position);
		}
		if (ct.type == Token::TT_COMMA){
			if (mStateStack.empty()) return createError (error::Parser_NoValidToken, "Unexpected comma", ct.position);
			State & subState (mStateStack.top());
			if (!subState.isFunction) return createError (error::Parser_NoValidToken, "Unexpected comma", ct.position);
			finalizeCurrentState();
			ExpressionPtr subResult = compress();
			if (subResult->error()) return subResult;
			mCurrentState.clear();
			subState.tokens.push_back (Token (subResult));
			continue;
		}
		if (ct.type == Token::TT_RP) {
			if (mStateStack.empty())
				return createError (error::Parser_ParanthesisMismatch, "Too much closing parenthesis", ct.position);
			finalizeCurrentState();

			bool ignoreNoArguments = false;
			if (mStateStack.top().isFunction) {
				ignoreNoArguments = true;
			}
			ExpressionPtr subResult;
			if (!(mCurrentState.tokens.empty() && ignoreNoArguments)){ // compress would fail if there are no arguments
				subResult = compress ();
				if (subResult->error()) return subResult;
			}

			mCurrentState = mStateStack.top();
			mStateStack.pop();
			if (subResult){
				mCurrentState.tokens.push_back (Token (subResult));
			}
			if (mCurrentState.isFunction) {
				/// Apply function
				subResult = compressFunction();
				if (subResult->error()) return subResult;
				mCurrentState = mStateStack.top();
				mStateStack.pop();
				mCurrentState.tokens.push_back (Token (subResult));
			}
			continue;
		}
		if (nextToken.type == Token::TT_LP && ct.type == Token::TT_UNKNOWN){
			if (!isFunction(ct)){
				return createError (error::Parser_UnknownFunction, "Unknown function "+ ct.text, ct.position);
			}
			awaitFunction = true;
			tokenForAwaitFunction = ct;
			continue;
		}
		if (ct.isNumber() && (nextToken.type == Token::TT_UNKNOWN || nextToken.type == Token::TT_LP)){
			// insert implicit multiplication
			mCurrentState.tokens.push_back (ct);
			Token implicitMultiplication ("*", ct.position);
			insertCommandInStack (implicitMultiplication);
			continue;
		}
		if (isArgumentToken(ct)) {
			mCurrentState.tokens.push_back (ct);
			continue;
		}
		if (findNonRegular(ct)){
			insertCommandInStack (ct);
			continue;
		}
		return createError (error::NotSupported, "Unsupported Token: " + ct.text, ct.position);
	}
	if (!mStateStack.empty()){
		return createError (error::Parser_ParanthesisMismatch, "Missing closing parenthesis", mCurrentState.begin);
	}
	finalizeCurrentState();
	return compress ();
}

ExpressionPtr Parser::compress () {
	assert (mCurrentState.commandStack.empty() && "command stack must be finalized before that");
	if (mCurrentState.tokens.empty()){
		return createError (error::Parser_NoValidToken, "Expected an argument", mCurrentState.begin);
	}
	std::vector<ExpressionPtr> argumentStack;
	for (std::vector<Token>::const_iterator i = mCurrentState.tokens.begin(); i != mCurrentState.tokens.end(); i++) {
		const Token & ct (*i);
		if (isArgumentToken(ct)){
			argumentStack.push_back (convertArgumentToken (ct));
			continue;
		}
		NamedFunctionPtr function = findNonRegular (ct);
		if (function) {
			size_t having = argumentStack.size();
			ExpressionPtr last  = argumentStack.size() > 0 ? argumentStack.back() : ExpressionPtr();
			ExpressionPtr last2 = argumentStack.size() > 1 ? *(argumentStack.end() - 2) : ExpressionPtr ();
			if (last2 && function->arity() < 0){
				// lets check if we can add last1 to last2
				NamedFunctionExpressionPtr exp = boost::dynamic_pointer_cast<NamedFunctionExpression>(last2);
				if (exp && exp->function() == function){
					// hooray
					argumentStack.pop_back();
					exp->addArgument (last);
					continue;
				}
			}
			if (having >= 2 && (function->checkArity(2))){
				argumentStack.pop_back();
				argumentStack.pop_back();
				argumentStack.push_back (function->createExpression(function, last2, last));
				continue;
			}
			if (having >= 1 && (function->checkArity(1)) && function->notation() != FN_INFIX) {
				// We do not want infix functions adding one argument here
				// otherwise we get "1+" or "1*" valid
				argumentStack.pop_back ();
				argumentStack.push_back(function->createExpression (function, last));
				continue;
			}
			return createError (error::Parser_NoValidToken, "Could not handle command: " + ct.text, ct.position);
		}
		return createError (error::Parser_NoValidToken, "Token too much: " + ct.text, ct.position);
	}
	if (argumentStack.size () !=  1) {
		return createError (error::Parser_NoValidToken, "Expected Operation", mCurrentState.begin);
	}
	return argumentStack[0];
}

ExpressionPtr Parser::compressFunction () {
	assert (mCurrentState.commandStack.size() == 1);
	Token functionToken = mCurrentState.commandStack[0];
	NamedFunctionPtr func = findRegular (functionToken);
	assert (func);
	std::vector<ExpressionPtr> arguments;
	for (std::vector<Token>::const_iterator i = mCurrentState.tokens.begin(); i != mCurrentState.tokens.end(); i++) {
		const Token & t (*i);
		assert (t.type == Token::TT_EXPRESSION);
		arguments.push_back (t.expression);
	}
	if (func->arity() >= 0 && (int) arguments.size() != func->arity()){
		std::string args   = boost::lexical_cast<std::string> (arguments.size());
		std::string aritys = boost::lexical_cast<std::string> (func->arity());
		return createError (error::Parser_WrongArgumentCount, func->name() + "/" + aritys + " called with " + args + " arguments", functionToken.position);
	}
	return ExpressionPtr (new NamedFunctionExpression (func, arguments));
}


void Parser::insertCommandInStack (const Token & token) {
	NamedFunctionPtr func = findNonRegular(token);
	assert (func);
	int precedence = func->precedence(); // commandPrecendence (token);
	while (!mCurrentState.commandStack.empty()){
		NamedFunctionPtr stackFunc = findNonRegular(mCurrentState.commandStack.back());
		assert (stackFunc);
		int stackPrecedence = stackFunc->precedence();
		if (stackPrecedence >= precedence) {
			mCurrentState.tokens.push_back (mCurrentState.commandStack.back());
			mCurrentState.commandStack.pop_back();
		} else {
			break;
		}
	}
	mCurrentState.commandStack.push_back (token);
}

void Parser::finalizeCurrentState () {
	/// pop last elements into postfix notation
	while (!mCurrentState.commandStack.empty()){
		mCurrentState.tokens.push_back (mCurrentState.commandStack.back());
		mCurrentState.commandStack.pop_back();
	}
}


bool Parser::isArgumentToken (const Token & t) const {
	return t.type == Token::TT_DOUBLE || t.type == Token::TT_INT || t.type == Token::TT_EXPRESSION || t.type == Token::TT_UNKNOWN;
}

ExpressionPtr Parser::convertArgumentToken (const Token & t) const {
	if (t.type == Token::TT_DOUBLE) return ExpressionPtr (new Value (t.vDouble));
	if (t.type == Token::TT_INT) return ExpressionPtr (new Value (t.vInt)); // TODO: real int support
	if (t.type == Token::TT_EXPRESSION) return t.expression;
	if (t.type == Token::TT_UNKNOWN) {
		// Check for a constant
		ConstantPtr constant = mContext ? mContext->findConstant(t.text) : ConstantPtr ();
		if (constant) {
			return constant;
		}

		// Assume its a variable
		return ExpressionPtr (new Variable (t.text, mContext->variableMapping->variableIdFor(t.text)));
	}
	return ExpressionPtr(); // Not supported
}

bool Parser::isFunction (const Token & t) const {
	return t.type == Token::TT_UNKNOWN && findRegular (t);
}

NamedFunctionPtr Parser::findNonRegular (const Token & t) const {
	if (!mContext) return NamedFunctionPtr();
	if (t.type == Token::TT_NEGATE) return mContext->findFunction("negate"); // work around
	return mContext->findNonPrefixFunction(t.text);
}

NamedFunctionPtr Parser::findRegular (const Token & t) const {
	if (!mContext) return NamedFunctionPtr();
	NamedFunctionPtr func = mContext->findFunction (t.text);
	if (func) return func;
	// also try printing names
	return mContext->findNonPrefixFunction(t.text);
}


ExpressionPtr Parser::createError (Error e, const String & message, int position) {
	mError = e;
	mErrorPosition = position;
	mErrorMessage = message;
	return sc::createError (e, message);
}


}
