#pragma once
#include "../smallcalc.h"
#include "../Expression.h"
#include "../PrimitiveValue.h"
#include "BoxElements.h"

/**
 * @file
 * Converter functions for Smallcalcs's Expression into Trees of graphical boxes.
 */

namespace sc {

/** Convert a primitive value to a box. tree */
BoxPtr convertPrimitiveValue (const PrimitiveValue& value);

/** Convert a expression to a box tree. Precedence is the precendece of the box above (internally used)*/
BoxPtr convertExpression (ExpressionPtr exp, int currentPrecedence = 0);

}
