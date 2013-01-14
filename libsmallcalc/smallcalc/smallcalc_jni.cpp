#ifdef ANDROID

#include "smallcalc.h"
#include "de_cgvis_moscalc_SmallCalc.h"
#include <string>

inline std::string toCppString (JNIEnv * env, jstring string) {
	const char * bytes = env->GetStringUTFChars(string, 0);
	std::string result (bytes);
	env->ReleaseStringUTFChars(string, bytes);
	return result;
}

static sc::SmallCalc smallcalc;
bool first = false;


JNIEXPORT jstring JNICALL Java_de_cgvis_moscalc_SmallCalc_calc
  (JNIEnv * env, jclass instance, jstring arg) {

	if (!first) {
		smallcalc.addAllStandard();
		smallcalc.setAccurateLevel(true);
	}

	std::string input  = toCppString(env, arg);
	std::string output = smallcalc.eval(input).toString();
	return env->NewStringUTF(output.c_str());
}


#endif
