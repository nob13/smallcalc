#pragma once
#include "../smallcalc.h"
namespace sc {
/**
 * @file
 * Nice expression --> string methods, combiningall other converts and scenegraphs.
 */

/** Prints an expression into a string. */
std::string print (const ExpressionPtr & expression);

/** Prints an primitive  value into a string.*/
std::string print (const PrimitiveValue& val);

/** Prints calculation result in format, "expression connector value", like sin(x) = value. */
std::string printCalcResult (const ExpressionPtr & exp, const std::string & connector, const PrimitiveValue & value);

/** Print calculation result in format, "value connector string-representation", like 3/4 = 0.75. */
std::string printFractionResult (const PrimitiveValue& value, const std::string& connector, const std::string & decimal);

}
