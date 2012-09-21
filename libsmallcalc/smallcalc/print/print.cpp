#include "print.h"
#include "Converter.h"
#include "TextDrawer.h"

namespace sc {

static std::string printBox (const BoxPtr & box) {
	TextDrawer drawer (box);
	drawer.drawLayouted(box);
	std::ostringstream ss;
	drawer.print (ss);
	return ss.str();
}

std::string print (const ExpressionPtr & expression) {
	BoxPtr box = convertExpression (expression);
	return printBox (box);
}

std::string print (const PrimitiveValue& val) {
	BoxPtr box = convertPrimitiveValue (val);
	return printBox (box);
}

std::string printCalcResult (const ExpressionPtr & exp, const std::string & connector, const PrimitiveValue & value) {
	shared_ptr<InfixFunctionBox> infixBox (new InfixFunctionBox(connector));
	infixBox->addArgument(convertExpression(exp));
	infixBox->addArgument(convertPrimitiveValue(value));
	return printBox (infixBox);
}

std::string printFractionResult (const PrimitiveValue& value, const std::string& connector, const std::string & decimal) {
	shared_ptr<InfixFunctionBox> infixBox (new InfixFunctionBox(connector));
	infixBox->addArgument(convertPrimitiveValue(value));
	infixBox->addArgument(BoxPtr (new TextBox(decimal)));
	return printBox (infixBox);
}



}
