#pragma once
#include "../types.h"
#include "Geometry.h"
#include "DrawEngine.h"
#include "Utf8Line.h"
#include <deque>
#include <boost/make_shared.hpp>

namespace sc {

class Box;
typedef shared_ptr<Box> BoxPtr;

/// Empty Box
class Box {
public:
	Box () : mParent(0), mMinSizeDirty (true) {}
	virtual ~Box () {}

	/// Layout all children. is NOT recursive
	virtual void layoutChildren (const DrawEngine & engine) {
	}

	// Draw. Is NOT recursive!
	virtual void draw (DrawEngine & engine) const {
	}

	const Point2i & position () const { return mPosition; }
	const Dimension2i & size() const { return mSize; }

	/// Set Position of a Box
	void setPosition (const Point2i & pos) { mPosition = pos; }

	/// Set size of a box.
	void setSize (const Dimension2i & size) { mSize = size; }

	/// Set parent
	void setParent (Box* box) { mParent = box; }

	/// Returns the parent
	Box* parent () const { return mParent; }

	/// Retruns number of children
	virtual int childCount () const { return 0; }
	/// Returns child with given index
	virtual BoxPtr child (int i) const { return BoxPtr(); }

	enum BoxFlag {
		F_NeedsClosedHSpace = 1 // The box needs closed horizontal space
	};

	/// The flags a box has
	virtual int flags () const { return 0; }

	const Dimension2i & minSize (const DrawEngine & engine) const {
		if (mMinSizeDirty) {
			mMinSize = calcMinSpace(engine);
			mMinSizeDirty = false;
		}
		return mMinSize;
	}

protected:
	/// Returns minimal space the box will use
	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const = 0;

	/// Minimum size got dirty
	void minSizeGotDirty () { mMinSizeDirty = true; }

	Point2i mPosition; ///< Position in term of parent element
	Dimension2i mSize; ///< Occupied size
	Box* mParent;
private:
	mutable Dimension2i mMinSize;
	mutable bool mMinSizeDirty;
};

/** A Box with one child. */
class SingleHolder : public Box {
public:
	SingleHolder (BoxPtr child) : mChild (child) {
		mChild->setParent(this);
	}
	~SingleHolder () {
		mChild->setParent(0);
	}

	/// Returns minimal space the box will use
	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const { return mChild->minSize(engine); }

	virtual void layoutChildren (const DrawEngine & engine) {
		mChild->setPosition (Point2i (0,0));
		mChild->setSize (mSize);
	}

	virtual void draw (DrawEngine & engine) const {
		// Nothing to do
	}


	/// Retruns number of children
	virtual int childCount () const { return 1; }
	/// Returns child with given index
	virtual BoxPtr child (int i) const { return mChild; }

	/// Returns flags of the child
	virtual int flags () const { return mChild->flags();}

protected:
	BoxPtr mChild;
};

/** A Box which has a fixed size, working as the root of all. */
class RootBox : public SingleHolder {
public:
	RootBox (const Dimension2i & size, const BoxPtr & child) : SingleHolder (child) {
		mSize = size;
	}
};

class TextBox : public Box {
public:
	TextBox (const std::string & text) : mText (text) {}
	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const { return engine.textSize (mText); }
	virtual void draw (DrawEngine & engine) const {
		engine.drawText(mText);
	}
private:
	std::string mText;
};

/** A Box holding a horizontal line. */
class LineBox : public Box {
public:
	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const {
		return Dimension2i(0,1);
	}
	virtual void draw (DrawEngine & engine) const {
		engine.drawLine(mSize.width);
	}
};

/// Horizontal center box
class HCenterBox : public SingleHolder {
public:
	HCenterBox (const BoxPtr & child) : SingleHolder (child) {

	}

	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const {
		return mChild->minSize(engine);
	}

	virtual void layoutChildren (const DrawEngine & engine) {
		int childWidth = mChild->minSize(engine).width;
		int i  = (mSize.width - childWidth) / 2;
		mChild->setPosition (Point2i (i, 0));
		mChild->setSize (Dimension2i (childWidth, mSize.height));
	}
};

/// Vertical center box
class VCenterBox : public SingleHolder {
public:
	VCenterBox (const BoxPtr & child) : SingleHolder (child) {

	}

	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const {
		return mChild->minSize(engine);
	}

	virtual void layoutChildren (const DrawEngine & engine) {
		int childHeight = mChild->minSize (engine).height;
		int i = (mSize.height - childHeight) / 2;
		mChild->setPosition (Point2i (0, i));
		mChild->setSize (Dimension2i (mSize.width, childHeight));
	}
};

/** Provides some horizontal extra space. */
class HSpace : public Box {
public:
	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const {
		return Dimension2i (engine.horizontalExtraSpace(), 0);
	}
};

class Paranthesis : public Box {
public:
	Paranthesis (DrawEngine::ParanthesisType t) : mType (t) {}

	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const {
		return engine.paranthesisMinSize(mType);
	}

	virtual void draw (DrawEngine & engine) const {
		engine.drawParanthesis(mSize, mType);
	}

private:
	DrawEngine::ParanthesisType mType;
};

/** Provides some space about objects which are inside paranthesis.*/
class ParanthesisSpaceProvider : public SingleHolder {
public:
	ParanthesisSpaceProvider (const BoxPtr & child) : SingleHolder (child) {
	}

	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const {
		Dimension2i childSpace = mChild->minSize(engine);
		Surrounding2i s = engine.paranthesisExtraSpace(childSpace);
		return Dimension2i (s.left + s.right + childSpace.width, s.top + s.bottom + childSpace.height);
	}

	virtual void layoutChildren (const DrawEngine & engine) {
		Surrounding2i s = engine.paranthesisExtraSpace(mSize);
		mChild->setPosition (Point2i (s.left, s.top));
		mChild->setSize (Dimension2i (mSize.width - s.left - s.right, mSize.height - s.top - s.bottom));
	}

};

class HorizontalSpaceProviderBox : public SingleHolder {
public:
	HorizontalSpaceProviderBox (const BoxPtr & child) : SingleHolder (child) {

	}

	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const {
		Dimension2i childSpace = mChild->minSize (engine);
		if (mChild->flags() & F_NeedsClosedHSpace) {
			childSpace.width += 2 * engine.horizontalExtraSpace();
		}
		return childSpace;
	}

	virtual int flags () const {
		return mChild->flags() & ~F_NeedsClosedHSpace;
	}

	virtual void layoutChildren (const DrawEngine & engine) {
		if (mChild->flags() & F_NeedsClosedHSpace) {
			int extra = engine.horizontalExtraSpace();
			mChild->setPosition (Point2i (extra, 0));
			mChild->setSize (Dimension2i (mSize.width - 2 * extra, mSize.height));
			// mChild->layoutChildren(engine);
		} else {
			SingleHolder::layoutChildren(engine);
		}
	}
};

class Container : public Box {
public:
	virtual int childCount () const { return mChildren.size(); }
	virtual BoxPtr child (int i) const {
		assert (i >= 0);
		return mChildren[i];
	}

	void addChild (const BoxPtr & child) {
		minSizeGotDirty();
		mChildren.push_back (child);
		child->setParent(this);
	}

	void prependChild (const BoxPtr & child) {
		minSizeGotDirty();
		mChildren.push_front(child);
		child->setParent(this);
	}

protected:
	std::deque<BoxPtr> mChildren;
};



/** Draws item vertically above each other. */
class VerticalContainer : public Container {
public:

	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const {
		Dimension2i minSpace;
		for (int i = 0; i < childCount(); i++) {
			Dimension2i childSpace = child(i)->minSize(engine);
			minSpace.width = std::max (minSpace.width, childSpace.width);
			minSpace.height += childSpace.height;
		}
		return minSpace;
	}

	virtual void layoutChildren (const DrawEngine & engine) {
		Point2i current = (Point2i (0,0));
		for (int i = 0; i < childCount(); i++) {
			Dimension2i minSpace = child(i)->minSize(engine);
			child(i)->setPosition(current);
			child(i)->setSize (Dimension2i (mSize.width, minSpace.height));
			current.y += minSpace.height;
		}
	}
};

/** Draws item vertically next to each other. */
class HorizontalContainer : public Container {
public:

	virtual Dimension2i calcMinSpace (const DrawEngine & engine) const {
		Dimension2i minSpace;
		for (int i = 0; i < childCount(); i++) {
			Dimension2i childSpace = child(i)->minSize(engine);
			minSpace.width += childSpace.width;
			minSpace.height =  std::max (minSpace.height, childSpace.height);
		}
		return minSpace;
	}

	virtual void layoutChildren (const DrawEngine & engine) {
		Point2i current = (Point2i (0,0));
		for (int i = 0; i < childCount(); i++) {
			Dimension2i minSpace = child(i)->minSize(engine);
			child(i)->setPosition(current);
			child(i)->setSize (Dimension2i (minSpace.width, mSize.height));
			// child(i)->layoutChildren(engine);
			current.x += minSpace.width;
		}
	}
};


/** Draws something in paranthesis. */
class ParanthesisBox : public HorizontalContainer {
public:
	ParanthesisBox (const BoxPtr & child){
		addChild (BoxPtr(new Paranthesis(DrawEngine::B_LEFT)));
		addChild (BoxPtr(new ParanthesisSpaceProvider(child)));
		addChild (BoxPtr(new Paranthesis(DrawEngine::B_RIGHT)));
	}
};


/** A Box which paints a fraction.*/
class FractionBox : public VerticalContainer {
public:
	FractionBox (const BoxPtr & numerator, const BoxPtr & denumerator) {
		addChild (BoxPtr (new HCenterBox (BoxPtr (new HorizontalSpaceProviderBox (numerator))))); // numerator
		addChild (BoxPtr (new LineBox()));
		addChild (BoxPtr (new HCenterBox (BoxPtr (new HorizontalSpaceProviderBox (denumerator)))));
	}

	virtual int flags() const { return F_NeedsClosedHSpace; }
};

/** A Box which paints a primitive (+,-,*) function.*/
class InfixFunctionBox : public HorizontalContainer {
public:
	InfixFunctionBox (const std::string & separator) {
		mSeparator = separator;
	}

	void addArgument (const BoxPtr & box) {
		if (!mChildren.empty()) {
			addChild (BoxPtr (new HSpace ()));
			addChild (BoxPtr (new VCenterBox (BoxPtr (new TextBox(mSeparator)))));
			addChild (BoxPtr (new HSpace()));
		}
		addChild (BoxPtr (new VCenterBox (box)));
	}

private:
	std::string mSeparator;
};

/// A Regular function like sin x
class RegularFunctionBox : public HorizontalContainer {
public:
	RegularFunctionBox (const std::string & name, const BoxPtr & argument) {
		addChild (BoxPtr (new VCenterBox (BoxPtr (new TextBox (name)))));
		// addChild (BoxPtr (new HSpace ()));
		addChild (BoxPtr (new VCenterBox (BoxPtr (argument))));
	}
};

class RegularFunctionBuilder  {
public:
	RegularFunctionBuilder (const std::string & name) {
		mName = name;
	}
	void addArgument (const BoxPtr & arg) {
		mArguments.push_back (arg);
	}

	BoxPtr result () {
		shared_ptr<InfixFunctionBox> convertedArguments = boost::make_shared<InfixFunctionBox>(",");
		BOOST_FOREACH(BoxPtr & box, mArguments) {
			convertedArguments->addArgument(box);
		}
		mArguments.clear();
		return boost::make_shared<RegularFunctionBox> (mName,
				BoxPtr (new ParanthesisBox (convertedArguments)));
	}
private:
	std::string mName;
	std::vector<BoxPtr> mArguments;

};




}

