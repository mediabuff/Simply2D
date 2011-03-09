#include "Shape.h"

/*
	shapes uses the top left to locate themselfs
	player and objects uses the center
	only one of these should exist!
	in the editor POS = center thoughh
*/
Shape::Shape()
{
	sides = 0;
}

// the order in which the points gets added is important
// CreateShape() function should be created
// to much to know for the user about the order
void Shape::addPoint(Point point)
{
	//point.x += origin.x;
	//point.y += origin.y;

	// calculate the AABB
	if(point.x < aabb.left)
		aabb.left = point.x;
	else if(point.x > aabb.right)
		aabb.right = point.x;

	if(point.y < aabb.top)
		aabb.top = point.y;
	else if(point.y > aabb.bottom)
		aabb.bottom = point.y;

	sides++;

	pointList.push_back(point);
}

Shape::AABB Shape::getAABB(void)
{
	return aabb;
}