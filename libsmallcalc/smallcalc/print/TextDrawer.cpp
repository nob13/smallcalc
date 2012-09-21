#include "TextDrawer.h"
#include <boost/foreach.hpp>

namespace sc {

void TextDrawer::setSize (const Dimension2i & dim) {
	mStack.setSize (dim);
	mLines.resize(dim.height);

	BOOST_FOREACH(Utf8Line & s, mLines) {
		s.resize (dim.width, ' ');
	}
}

TextDrawer::TextDrawer (const Dimension2i & dim){
	setSize (dim);
}

void TextDrawer::drawText (const std::string & s) {
	line().replace (mStack.pos().x, s);
}

Dimension2i TextDrawer::textSize (const std::string & s) const {
	return Dimension2i (Utf8Line::stringLength(s), 1);
}

void TextDrawer::drawLine (int length) {
	line().replacePattern (mStack.pos().x, length, "─");
}

void TextDrawer::drawParanthesis (const Dimension2i & size, ParanthesisType type) {
	if (size.height < 2) {
		std::string c = type == B_LEFT ? "(" : ")";
		for (int i = 0; i < size.height; i++) {
			line (i).replace (mStack.pos().x, c);
		}
	} else {
		std::string c = type == B_LEFT ? "╭" : "╮";
		line(0).replace(mStack.pos().x, c);
		c = "│";
		for (int i = 1; i < size.height - 1; i++) {
			line(i).replace(mStack.pos().x, c);
		}
		c = type == B_LEFT ? "╰" : "╯";
		line(size.height-1).replace(mStack.pos().x, c);
	}
}

Surrounding2i TextDrawer::paranthesisExtraSpace (const Dimension2i & i) const {
	if (i.height == 1) return Surrounding2i ();
	return Surrounding2i (1,1,1,1);
}

void TextDrawer::layoutTree (const BoxPtr & box) {
	box->layoutChildren(*this);
	for (int i = 0; i < box->childCount(); i++) {
		layoutTree (box->child(i));
	}
}


void TextDrawer::draw (const BoxPtr & box) {
	box->draw(*this);
	for (int i = 0; i < box->childCount(); i++) {
		BoxPtr child (box->child(i));
		mStack.push ();
		mStack.moveCursor (child->position());
		draw (child);
		mStack.pop ();
	}
}

void TextDrawer::drawLayouted (BoxPtr box) {
	BoxPtr root = boost::make_shared<RootBox> (mStack.space(), box);
	layoutTree (root);
	draw (root);
}

void TextDrawer::print (std::ostream & stream) {
	// omit std::endl on last line to be compatible with other outputs
    for (std::vector<Utf8Line>::const_iterator i = mLines.begin(); i != mLines.end(); i++) {
        stream << *i;
        if ((i + 1) != mLines.end()) stream << std::endl;
    }
}


}
