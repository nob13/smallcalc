#pragma once

/**
 * @file some basic geometrical types
 */

namespace sc {

/** Point ins space. */
struct Point2i {
	Point2i (int _x = 0, int _y = 0) : x(_x), y (_y) {}
	int x;
	int y;
};

/** Size in space .*/
struct Dimension2i {
	Dimension2i (int w = 0, int h = 0) : width(w), height(h) {}
	int width;
	int height;
};

/** Rectangular scene. */
struct Scene {
	Point2i position;
	Dimension2i size;
};


inline Dimension2i operator+ (const Dimension2i & a, const Dimension2i & b) {
	return Dimension2i (a.width + b.width, a.height + b.height);
}

/// A Surrounding space defintion
struct Surrounding2i {
	Surrounding2i (int _left =0, int _top = 0, int _right = 0, int _bottom = 0) :
		left (_left), top (_top), right (_right), bottom (_bottom) {}
	int left,top,right,bottom;
};



}
