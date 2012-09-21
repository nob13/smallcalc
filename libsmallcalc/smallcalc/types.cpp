#include "types.h"

namespace sc {

VariableId VariableIdMapping::variableIdFor (const String & name) const {
	VariableNameMap::const_iterator i = variableIds.find(name);
	if (i == variableIds.end()) {
		// new variable
		VariableId id = nextVariableId++;
		variableIds[name] = id;
		variableNames[id] = name;
		return id;
	}
	return i->second;
}



}
