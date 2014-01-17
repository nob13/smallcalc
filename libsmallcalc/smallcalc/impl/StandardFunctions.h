#pragma once
#include "../types.h"
#include "../PrimitiveValue.h"
#include <vector>

/**
 * @file
 * Some standard named functions (sin, cos, etc.) in the notation as used by NamedFunction
 */

namespace sc {

struct EvaluationContext;

// Fundamentals
PrimitiveValue add (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue multiply (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue subtract (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue divide (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue negate (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue exponentation (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);

PrimitiveValue sin (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue cos (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue tan (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue round (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue sqrt (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);

// arc functions
PrimitiveValue acos (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue asin (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue atan (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);

// hyperbolic functions
PrimitiveValue cosh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue sinh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue tanh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);

// arc hyperbolic functions
PrimitiveValue acosh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue asinh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);
PrimitiveValue atanh (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);


// log naturalis
PrimitiveValue ln (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);

PrimitiveValue abs (const std::vector<PrimitiveValue> & arguments, const EvaluationContext* context);

}
