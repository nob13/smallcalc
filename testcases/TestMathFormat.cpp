#include <gtest/gtest.h>
#include <smallcalc/MathFunctions.h>
#include <smallcalc/types.h>
#include <string>
#include <iostream>
#include <smallcalc/print/SpaceStack.h>
#include <smallcalc/print/BoxElements.h>
#include <smallcalc/print/TextDrawer.h>
#include <smallcalc/print/Converter.h>
#include <smallcalc/print/print.h>

#include <smallcalc/smallcalc.h>
#include <boost/make_shared.hpp>
#include <boost/weak_ptr.hpp>

using namespace sc;

TEST (TestMathFormat2, testUtf8Line) {
	Utf8Line line;
	line.resize (10);
	ASSERT_EQ (line.asUtf8(), "          ");
	line.replace(3,"Hallo");
	ASSERT_EQ (line.length(), 10);
	ASSERT_EQ (line.asUtf8(), "   Hallo  ");
	std::cout << line << std::endl;
	line.replace(8,"Bier");
	ASSERT_EQ (line.length(), 12);
	ASSERT_EQ (line.asUtf8(), "   HalloBier");
	std::cout << line << std::endl;
	line.replacePattern(0,3,"Bier");
	ASSERT_EQ (line.length(), 12);
	ASSERT_EQ (line.asUtf8(), "BierBierBier");
	line.replacePattern (10, 12, "*");
	ASSERT_EQ (line.asUtf8(), "BierBierBi************");
	line.replacePattern (10, 8, "-");
	ASSERT_EQ (line.asUtf8(), "BierBierBi--------****");
}

TEST (TestMathFormat2, testUtf8Lineregression) {
	Utf8Line line;
	line.resize(6);
	line.replace (0, "sin(x)");
	line.replacePattern(0, 6, "-");
	ASSERT_EQ (line.asUtf8(), "------");
}


TEST (TestMathFormat2, testHCenter) {
	BoxPtr target  = boost::make_shared<HCenterBox> (boost::make_shared<TextBox> ("Hallo"));
	Dimension2i size (12, 1);
	TextDrawer drawer (Dimension2i (12, 1));
	drawer.drawLayouted(target);
	drawer.print (std::cout);
}

TEST (TestMathFormat2, simpleFraction) {
	BoxPtr upper  = boost::make_shared<TextBox> ("sin(x)");
	BoxPtr lower  = boost::make_shared<TextBox> ("2");
	BoxPtr target = boost::make_shared<FractionBox> (upper,lower);
	TextDrawer drawer (target);
	drawer.drawLayouted(target);
	drawer.print (std::cout);
}



TEST (TestMathFormat2, simplePrint) {
	BoxPtr upper  = boost::make_shared<TextBox> ("sin(x)*µ");

	BoxPtr subUpper = boost::make_shared<TextBox> ("2");
	BoxPtr subLower = boost::make_shared<TextBox> ("3");
	BoxPtr lower  = boost::make_shared<FractionBox> (subUpper, subLower);
	BoxPtr target = boost::make_shared<FractionBox> (upper, lower);
	TextDrawer drawer (target);
	drawer.drawLayouted(target);
	drawer.print (std::cout);
}

void printAsBox (ExpressionPtr exp) {
	std::cout << "Formatting: " << exp->printNice() << " " << std::endl;
	std::cout << print (exp) << std::endl;
}

void printAsBox (const PrimitiveValue& val) {
	std::cout << "Formatting: " << val.toString() << " " << std::endl;
	std::cout << print (val) << std::endl;
}


TEST (TestMathFormat2, autoConersion) {
	SmallCalc sc;
	sc.addAllStandard();
	printAsBox (sc.parse("1/x"));
	printAsBox (sc.parse("sin(x)/cos(x)"));
	printAsBox (sc.parse("sin(x)/(1/π)"));
	printAsBox (sc.parse("sin(µ)/(1/cos(x))"));
	printAsBox (sc.parse("(1-4)+2/2+3+(4/x)+ -2/µ"));
	printAsBox (sc.parse ("1*(2+3)"));
	printAsBox (sc.parse ("-(1/2)"));
	printAsBox (sc.parse ("2*(3+1/4)"));
	printAsBox (sc.parse ("(3*1/4)"));
	printAsBox (sc.parse ("sin(1/x)"));
	printAsBox (sc.parse ("sqrt(3/(sin (µ)))"));
	printAsBox (PrimitiveValue (Fraction64 (2,3)));
	printAsBox (PrimitiveValue (Fraction64 (-2,3)));
	printAsBox (PrimitiveValue (Fraction64 (-1204,23458)));
	printAsBox (sc.parse ("(1/4)^2"));
	printAsBox (sc.parse ("4^2"));
	printAsBox (sc.parse ("12 + x^2 + x^3 + 4 * x ^ (1/2)"));

}

