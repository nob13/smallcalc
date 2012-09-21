#include <gtest/gtest.h>
#include <smallcalc/impl/Tokenizer.h>
using namespace sc;

class TokenizerTest : public testing::Test {
protected:

	void shortSimpleTypeTest (const std::string & in, Token::Type expected) {
		Tokenizer t;
		Error e = t.tokenize(in);
		ASSERT_TRUE (e == NoError);
		ASSERT_EQ (1, t.result().size()) << " in " << in;
		Token parsed = t.result()[0];
		ASSERT_EQ (expected, parsed.type) << in << " is not of expected type " << expected;
		ASSERT_EQ (in, parsed.text);
	}

	std::vector<Token> tokenize (const std::string & input) {
		Tokenizer t;
		t.tokenize(input);
		return t.result();
	}
};

TEST_F (TokenizerTest, SimpleTypes) {
	shortSimpleTypeTest ("3", Token::TT_INT);
	shortSimpleTypeTest ("3.4", Token::TT_DOUBLE);
	shortSimpleTypeTest ("+", Token::TT_PLUS);
	shortSimpleTypeTest ("-", Token::TT_MINUS);
	shortSimpleTypeTest ("bla", Token::TT_UNKNOWN);
	shortSimpleTypeTest ("(", Token::TT_LP);
	shortSimpleTypeTest (")", Token::TT_RP);
}

TEST_F (TokenizerTest, NegateHandling) {
	{
		std::vector<Token> tokens = tokenize ("-2");
		ASSERT_EQ (1, tokens.size());
		// No negates
	}
	{
		std::vector<Token> tokens = tokenize ("(-2)");
		ASSERT_EQ (3, tokens.size());
		// No negate
	}
	{
		std::vector<Token> tokens = tokenize ("-(3*x)");
		ASSERT_EQ (6, tokens.size());
		ASSERT_EQ (Token::TT_NEGATE, tokens[0].type);
	}
	{
		std::vector<Token> tokens = tokenize ("3*-(-2)");
		ASSERT_EQ (6, tokens.size());
		ASSERT_EQ (Token::TT_NEGATE, tokens[2].type);
	}
}

TEST_F (TokenizerTest, NumSplit) {
	{
		std::vector<Token> tokens = tokenize ("2sin");
		ASSERT_EQ (2, tokens.size());
		EXPECT_EQ (Token::TT_INT, tokens[0].type);
		EXPECT_EQ (Token::TT_UNKNOWN, tokens[1].type);
	}
	{
		std::vector<Token> tokens = tokenize ("2(3*2)");
		ASSERT_EQ (6, tokens.size());
		EXPECT_EQ (Token::TT_INT, tokens[0].type);
		EXPECT_EQ (Token::TT_LP, tokens[1].type);
	}
}

TEST_F (TokenizerTest, ExponentialWriting) {
	// Exponential writing
	{
		std::vector<Token> tokens = tokenize ("2.23e-4");
		ASSERT_EQ (1, tokens.size());
		EXPECT_EQ (Token::TT_DOUBLE, tokens[0].type);
	}
	// Exponential writing with num split
	{
		std::vector<Token> tokens = tokenize ("2.23e-4 cos");
		ASSERT_EQ (2, tokens.size());
		EXPECT_EQ (Token::TT_DOUBLE, tokens[0].type);
		EXPECT_EQ (Token::TT_UNKNOWN, tokens[1].type);
	}
}

TEST_F (TokenizerTest, Simple1) {
	const char * inputs [] = {
			"2+3",
			"2 + 3",
			" 2 + 3 ",
			" 2+ 3",
			" 2+3  ",
			"2    +3"
	};
	for (size_t i = 0; i < sizeof (inputs) / sizeof (const char*); i++) {
		const char * input = inputs[i];
		std::vector<Token> result = tokenize (input);
		ASSERT_EQ (3, result.size()) << " in " << input;
		ASSERT_EQ (Token::TT_INT, result[0].type) << " in " << input;
		ASSERT_EQ (Token::TT_PLUS, result[1].type) << " in " << input;
		ASSERT_EQ (Token::TT_INT, result[2].type) << " in " << input;
	}
}

TEST_F (TokenizerTest, IntAndDouble) {
	Token tInt ("23", 0);
	ASSERT_EQ (23, tInt.vInt);
	Token tDouble ("23.2", 0);
	ASSERT_EQ (23.2, tDouble.vDouble);
}

TEST_F (TokenizerTest, Simple2) {
	const char * inputs [] = {
			"(2+3)",
			"( 2 + 3 )",
			"( 2 + 3)",
			" (2+ 3)  ",
			"( 2+3  )",
			"(   2    +3   )   "
	};
	for (size_t i = 0; i < sizeof (inputs) / sizeof (const char*); i++) {
		const char * input = inputs[i];
		std::vector<Token> result = tokenize (input);
		ASSERT_EQ (5, result.size()) << " in " << input;
		ASSERT_EQ (Token::TT_LP, result[0].type) << " in " << input;
		ASSERT_EQ (Token::TT_INT, result[1].type) << " in " << input;
		ASSERT_EQ (Token::TT_PLUS, result[2].type) << " in " << input;
		ASSERT_EQ (Token::TT_INT, result[3].type) << " in " << input;
		ASSERT_EQ (Token::TT_RP, result[4].type) << " in " << input;
	}
}
