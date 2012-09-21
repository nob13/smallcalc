#pragma once

#include "Geometry.h"
#include <assert.h>
#include <stack>

namespace sc {

/// A Tree of different spaces, used to calculate position and size of viewable elements
class SpaceStack {
public:
	SpaceStack () {

	}

	void setSize (const Dimension2i & size) {
		mScene.size = size;
	}

	SpaceStack (const Dimension2i & dim) {
		mScene.size = dim;
	}


	void moveCursor (const Point2i & p) {
		mScene.position.x +=  p.x;
		mScene.position.y +=  p.y;
		mScene.size.width  -= p.x;
		mScene.size.height -= p.y;
		assert (mScene.size.width >= 0);
		assert (mScene.size.height >= 0);
	}

	void limitSpace (const Dimension2i & limit) {
		assert (limit.width <= mScene.size.width);
		mScene.size.width = limit.width;
		assert (limit.height <= mScene.size.height);
		mScene.size.height = limit.height;
	}

	void push () {
		mCoordinateStack.push (mScene);
	}

	void pop () {
		mScene = mCoordinateStack.top();
		mCoordinateStack.pop();
	}

	/// Returns available space
	const Dimension2i & space () const { return mScene.size; }

	/// Returns position in current scene
	const Point2i& pos () const { return mScene.position; }
protected:

	Scene mScene;
	std::stack<Scene> mCoordinateStack;
};


}
