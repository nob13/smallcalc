#include "Expression.h"
#include "PrimitiveValue.h"
#include "impl/Value.h"

namespace sc {

ExpressionPtr createError (Error e, const String & message) {
	return ExpressionPtr(new Value (errorValue (e, message)));
}

}
