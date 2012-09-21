#pragma once
#include <string>
#include <stdlib.h>
#include <list>
#include "utf8/checked.h"
#include <vector>

namespace sc {

/**
 * UTF8 Line, internally working with an utf32 array
 *
 */
class Utf8Line {
public:
	Utf8Line () {
	}

	void resize (size_t len, char fill = ' ') {
		size_t current = length();
		mUtf32.resize(len);
		if (len <= current) {
			return;
		} else {
			// Fill with characters
			for (size_t i = current; i < len; i++) {
				mUtf32[i] = fill;
			}
		}
	}

	size_t length () const {
		return mUtf32.size();
	}

	static size_t stringLength (const std::string& s) {
		return utf8::distance (s.begin(), s.end());
	}

	void replace (size_t pos, const std::string & s) {
		size_t len = utf8::distance(s.begin(), s.end());
		size_t afterwards = pos + len;
		if (afterwards > length()) {
			resize (afterwards);
		}
		utf8::utf8to32(s.begin(),s.end(),mUtf32.begin() + pos);
	}

	void replacePattern (const size_t pos, const size_t count, const std::string & s) {
		size_t len = utf8::distance(s.begin(), s.end());
		size_t afterwards = pos + count * len;
		if (afterwards > length()) {
			resize (afterwards);
		}
		std::vector<int32_t> temp;
		temp.resize (len);
		utf8::utf8to32 (s.begin(), s.end(), temp.begin());
		size_t p = pos;
		for (size_t i = 0; i < count; i++) {
			for (size_t j = 0; j < len; j++) {
				assert (p < mUtf32.size());
				mUtf32[p] = temp[j];
				p++;
			}
		}
	}

	std::string asUtf8 () const {
		std::string result;
		utf8::utf32to8 (mUtf32.begin(), mUtf32.end(), back_inserter (result));
		return result;
	}

	void streamOut (std::ostream & o) const {
		o << asUtf8 ();
	}

private:
	std::vector<int32_t> mUtf32;
};

inline std::ostream& operator<< (std::ostream & o, const Utf8Line & s) {
	s.streamOut(o);
	return o;
}

}
