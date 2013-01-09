#include <gtest/gtest.h>
#include <smallcalc/MathFunctions.h>
#include <string>
#include <iostream>
using namespace sc;

TEST(MathFunctions, noOverflow){
	bool overflow = false;
	EXPECT_EQ (addWithOverflowCheck (3,4,&overflow), 7);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (addWithOverflowCheck (-2,4, &overflow), 2);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (addWithOverflowCheck (-2,-2, &overflow), -4);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (addWithOverflowCheck (4,-5, &overflow), -1);
	EXPECT_EQ (overflow, false);

	overflow = false;
	EXPECT_EQ (subWithOverflowCheck(3,2, &overflow), 1);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (subWithOverflowCheck(4,-2, &overflow), 6);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (subWithOverflowCheck(4, 5, &overflow), -1);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (subWithOverflowCheck(-4,5, &overflow), -9);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (subWithOverflowCheck(-4,-5, &overflow), 1);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (subWithOverflowCheck(4,-5, &overflow), 9);
	EXPECT_EQ (overflow, false);

	overflow = false;
	EXPECT_EQ (subWithOverflowCheck(std::numeric_limits<int64_t>::min(), -std::numeric_limits<int64_t>::max(), &overflow), -1); // 2 complement
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (addWithOverflowCheck(std::numeric_limits<int64_t>::min(), std::numeric_limits<int64_t>::max(), &overflow), -1); // 2 complement
	EXPECT_EQ (overflow, false);


	overflow = false;
	EXPECT_EQ (multWithOverflowCheck(2,3, &overflow), 6);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (multWithOverflowCheck (2, -3, &overflow), -6);
	EXPECT_EQ (overflow, false);
	EXPECT_EQ (multWithOverflowCheck(-2,-3, &overflow), 6);
	EXPECT_EQ (overflow, false);
}

TEST (MathFunctions, overflows){
	// Addition
	bool overflow = true;
	EXPECT_EQ (addWithOverflowCheck (std::numeric_limits<int64_t>::max(), (int64_t)1, &overflow), std::numeric_limits<int64_t>::min());
	EXPECT_EQ (overflow, true);
	overflow = false;

	EXPECT_EQ (addWithOverflowCheck (std::numeric_limits<int64_t>::min(), (int64_t)-1, &overflow), std::numeric_limits<int64_t>::max());
	EXPECT_EQ (overflow, true);
	overflow = false;

	EXPECT_EQ (addWithOverflowCheck (std::numeric_limits<int64_t>::max(), std::numeric_limits<int64_t>::max(), &overflow), -2); // on 2 complement
	EXPECT_EQ (overflow, true);
	overflow = false;

	// Subtraction
	EXPECT_EQ (subWithOverflowCheck (std::numeric_limits<int64_t>::max(), (int64_t)-1, &overflow), std::numeric_limits<int64_t>::min());
	EXPECT_EQ (overflow, true);
	overflow = false;

	EXPECT_EQ (subWithOverflowCheck (std::numeric_limits<int64_t>::min(), (int64_t)1, &overflow), std::numeric_limits<int64_t>::max());
	EXPECT_EQ (overflow, true);
	overflow = false;

	EXPECT_EQ (subWithOverflowCheck (std::numeric_limits<int64_t>::max(), -std::numeric_limits<int64_t>::max(), &overflow), -2); // on 2 complement
	EXPECT_EQ (overflow, true);
	overflow = false;

	// Multiplication
	overflow = false;
	EXPECT_EQ (multWithOverflowCheck(std::numeric_limits<int64_t>::max(), (int64_t)2, &overflow), -2); // on 2 complement
	EXPECT_EQ (overflow, true);

	overflow = false;
	EXPECT_EQ (multWithOverflowCheck(std::numeric_limits<int64_t>::min(), (int64_t)2, &overflow), 0); // on 2 complement
	EXPECT_EQ (overflow, true);

	overflow = false;
	multWithOverflowCheck (1000000000000L, 1000000000000L, &overflow);
	EXPECT_EQ (overflow, true);

	overflow = false;
	multWithOverflowCheck (-1000000000000000000, 1000L, &overflow); // regression
	EXPECT_EQ (overflow, true);
}

TEST (MathFunctions, testLcm){
	bool overflow = false;
	EXPECT_EQ (lcmWithOverflowCheck(2,6,&overflow), 6);
	EXPECT_EQ (lcmWithOverflowCheck(4,3,&overflow), 12);
	EXPECT_EQ (overflow, false);

	EXPECT_EQ (lcmWithOverflowCheck(-2,6,&overflow), -6);
	EXPECT_EQ (lcmWithOverflowCheck(-2,-3,&overflow), 6);
	EXPECT_EQ (overflow, false);

	EXPECT_EQ (lcmWithOverflowCheck(std::numeric_limits<int64_t>::max(),(int64_t)2,&overflow), -2);
	EXPECT_EQ (overflow, true);

	overflow = false;
	EXPECT_EQ (lcmWithOverflowCheck(std::numeric_limits<int64_t>::min(),(int64_t)2,&overflow), std::numeric_limits<int64_t>::min());
	EXPECT_EQ (overflow, false);

	overflow = false;
	lcmWithOverflowCheck(std::numeric_limits<int64_t>::min(),std::numeric_limits<int64_t>::min()-1,&overflow);
	EXPECT_EQ (overflow, true);
}


std::string printFraction (int64_t n, int64_t d) {
	bool exact = false;
	std::string decimal = Fraction<int64_t> (n,d).toDecimal(&exact);
	std::cout << n << "/" << d << "=" << decimal << " exact=" << exact << std::endl;
	return decimal;
}

TEST (MathFunctions, testDecialNotation) {
	EXPECT_EQ (printFraction (1,5), "0.2");
	EXPECT_EQ (printFraction (2,5), "0.4");
	EXPECT_EQ (printFraction (6,5), "1.2");
	EXPECT_EQ (printFraction (2,3), "0.66666666");
	EXPECT_EQ (printFraction (-2,5), "-0.4");
	EXPECT_EQ (printFraction (-2,-5), "0.4");
	EXPECT_EQ (printFraction (1, 17), "0.058823529");
	EXPECT_EQ (printFraction (10000, 17), "588.23529411");
	EXPECT_EQ (printFraction (10000, 10), "1000");
	EXPECT_EQ (printFraction (10000000000000000,10000000000000001), "0.99999999");
}
