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
	assert (mStack.pos().x >= 0);
	line().replace (mStack.pos().x, s);
}

Surrounding2i TextDrawer::textSize (const std::string & s) const {
	return Surrounding2i (0,0,Utf8Line::stringLength(s), 1);
}

void TextDrawer::drawLine (int length) {
	line().replacePattern (mStack.pos().x, length, "─");
}

void TextDrawer::drawParanthesis (const Surrounding2i & size, ParanthesisType type) {
	if (size.height() < 2) {
		std::string c = type == B_LEFT ? "(" : ")";
		for (int i = 0; i < size.bottom; i++) {
			line (i).replace (mStack.pos().x, c);
		}
	} else {
		std::string c = type == B_LEFT ? "╭" : "╮";
		line(-size.top).replace(mStack.pos().x, c);
		c = "│";
		for (int i = 1; i < size.height() - 1; i++) {
			line(i - size.top).replace(mStack.pos().x, c);
		}
		c = type == B_LEFT ? "╰" : "╯";
		line(size.bottom-1).replace(mStack.pos().x, c);
	}
}

void TextDrawer::drawSquareRoot (const Surrounding2i & size) {
	if (size.height() == 1) {
		line(0).replace (mStack.pos().x - size.left, "√");
	} else {
		line(0).replace (mStack.pos().x - size.left, "┤");
		int x = mStack.pos().x - size.left;
		for (int i = 1; i < size.bottom; i++) {
			line(i).replace (x, "│");
		}
		for (int i = 1; i < size.top; i++) {
			line(-i).replace (x, "│");
		}
		int y = -(size.top);
		line (y).replace (x, "┌");
		for (int i = 0; i < size.right; i++) {
			line(y).replace(mStack.pos().x + i, "─");
		}
	}
}


Surrounding2i TextDrawer::paranthesisExtraSpace (const Surrounding2i & i) const {
	if (i.height() == 1) return Surrounding2i ();
	return Surrounding2i (1,1,1,1);
}

Surrounding2i TextDrawer::squareRootExtraSpace (const Surrounding2i & i) const {
	return Surrounding2i (1,1,0,0);
}

void TextDrawer::layoutTree (const BoxPtr & box) {
	box->layoutChildren(*this);
	for (int i = 0; i < box->childCount(); i++) {
		layoutTree (box->child(i));
	}
}


void TextDrawer::draw (const BoxPtr & box) {
	mStack.push();
	mStack.moveCursor (box->position());
	box->draw(*this);
	for (int i = 0; i < box->childCount(); i++) {
		BoxPtr child (box->child(i));
		draw (child);
	}
	mStack.pop();
}

void TextDrawer::drawLayouted (BoxPtr box) {
	Surrounding2i size = box->minSize (*this);
	assert (mStack.space().width >= size.width());
	assert (mStack.space().height >= size.height());
	BoxPtr root = boost::make_shared<RootBox> (size, box);
	root->setPosition (Point2i (size.left, size.top));
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
