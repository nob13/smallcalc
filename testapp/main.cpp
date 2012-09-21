#include <smallcalc/smallcalc.h>
#include <smallcalc/print/print.h>
#include <iostream>

int main (int argc, char * argv[]) {
	sc::SmallCalc smallCalc;
	smallCalc.addStandardConstants();
	smallCalc.addStandardFunctions();
	smallCalc.setAccurateLevel();
	std::string line;
	while (true) {
		std::cout << "> ";
		bool suc = std::getline (std::cin, line);
		if (!suc) break;
		sc::PrimitiveValue val = smallCalc.eval (line);
		if (val.error() == sc::error::Parser_NoTokens){
			continue; // ignore, empty line
		}
		sc::ExpressionPtr exp = smallCalc.lastExpression();
		std::cout << printCalcResult (exp, "=>", val) << std::endl;
		if (val.type() == sc::PT_FRACTION){
			bool exact = false;
			std::string dec = val.toFraction().toDecimal(&exact);
			if (exact) {
				std::cout << "=" << dec << std::endl;
			} else {
				std::cout << "=~" << dec << "..." << std::endl;
			}
		}
	}
	return 0;
}
