#pragma once
#include <boost/unordered_map.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>
#include "MathFunctions.h"

namespace sc {

using boost::shared_ptr;
using boost::unordered_map;

using boost::function;
typedef std::string String;

/// Small extensions to unorderd_map which finds a given key and returns it
/// assuming the value is a shared_ptr
template <class Key, class Type> shared_ptr<Type> findValuePtr(const unordered_map<Key, shared_ptr<Type> > & map, const String & key){
	typename unordered_map<Key,shared_ptr<Type> >::const_iterator i = map.find (key);
	if (i == map.end()) return shared_ptr<Type> ();
	return shared_ptr<Type> (i->second);
}

/// Variable names
typedef int VariableId;

struct VariableIdMapping {
	VariableIdMapping ()  : nextVariableId (1) {}
	VariableId variableIdFor (const String & name) const;

	typedef boost::unordered_map<String, VariableId> VariableNameMap;			///< Maps variable names to Ids
	typedef boost::unordered_map<VariableId, String> ReverseVariableNameMap;    ///< Maps variable Ids to names

	mutable VariableNameMap variableIds;
	mutable ReverseVariableNameMap variableNames;
	mutable VariableId nextVariableId;
};


namespace error {
	enum Error {
		NoError = 0,
		NotSupported,

		Parser_ParanthesisMismatch, ///< Too much closing or wrong paranthesis count
		Parser_NoValidToken,
		Parser_NoTokens,			///< No tokens given
		Parser_WrongArgumentCount,	///< Function applied with wrong number of arguments
		Parser_UnknownFunction,
		Eval_BadType,				///< Expression's check fails

		Eval_InvalidOperation,		///< Cannot evaluate this expression (e.g. multiplying an error)
		Eval_UnboundVariable,		///< Cannot evaluate variable; its unbound
		Eval_DivisionByZero,		///< Division by Zero error

	};
}
typedef error::Error Error;
using error::NoError;

typedef Fraction<int64_t> Fraction64;

}
