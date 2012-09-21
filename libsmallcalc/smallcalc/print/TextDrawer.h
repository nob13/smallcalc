#pragma once
#include "BoxElements.h"
#include "SpaceStack.h"
#include <boost/make_shared.hpp>
#include "Utf8Line.h"
#include <vector>

namespace sc {

/**
 * A drawer which draws into text displays
 */
class TextDrawer : public DrawEngine{
public:

	TextDrawer () {

	}

	TextDrawer (BoxPtr destination) {
		setSize (destination->minSize(*this));
	}

	/// Set text box size
	void setSize (const Dimension2i & dim);

	TextDrawer (const Dimension2i & dim);

	// Implementation of DrawEngine
	virtual void drawText (const std::string & s);

	Dimension2i textSize (const std::string & s) const;

	virtual void drawLine (int length);

	virtual void drawParanthesis (const Dimension2i & size, ParanthesisType type);
	virtual Surrounding2i paranthesisExtraSpace (const Dimension2i & i) const;


	/** Layout a whole tree.*/
	void layoutTree (const BoxPtr & box);

	/// Recursivly draws all elements.
	void draw (const BoxPtr & box);

	/// Convenience function, creates a root
	/// Note: modifies box' parent flag
	void drawLayouted (BoxPtr box);

	/// Print to stdout stream
	void print (std::ostream & stream);

private:

	/// Returns current line together with an offset
	Utf8Line & line (int offset = 0) {
		int idx = mStack.pos().y + offset;
		assert (idx >= 0 && idx < (int) mLines.size());
		return mLines[idx];
	}

	std::vector<Utf8Line> mLines;
	SpaceStack mStack;
};


}
