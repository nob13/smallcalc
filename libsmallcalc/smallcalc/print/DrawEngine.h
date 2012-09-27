#pragma once
#include <string>

namespace sc {

class DrawEngine {
public:
	enum ParanthesisType { B_LEFT, B_RIGHT };


	virtual void drawText (const std::string & s) = 0;
	virtual Surrounding2i textSize (const std::string & s) const = 0;
	virtual void drawLine (int length) = 0;

	virtual void drawParanthesis (const Surrounding2i & size, ParanthesisType type) = 0;

	virtual void drawSquareRoot (const Surrounding2i & size) = 0;

	/// How many space if some elements needs horizontal extra space
	virtual int horizontalExtraSpace () const { return 1;}
	/// How many space if some elements needs vertical extra space
	virtual int verticalExtraSpace() const { return 1;}

	virtual Surrounding2i paranthesisMinSize (ParanthesisType type) const { return Surrounding2i (0,0,1,1); }

	/// Returns surroing if paranthesis for a given object need them
	virtual Surrounding2i paranthesisExtraSpace (const Surrounding2i & i) const { return Surrounding2i (); }

	/// Returns surrounding needed for a square root sign around a given object
	virtual Surrounding2i squareRootExtraSpace (const Surrounding2i & i) const = 0;
};


}
