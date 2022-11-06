#pragma once
#include "ofMain.h"

// Troy Perez - CS134 SJSU


//  Base class for any object that needs a transform.
//
class TransformObject {
protected:
	TransformObject();
	ofVec3f position, scale;
	float	rotation;
	bool	bSelected;
	void setPosition(const ofVec3f &);
};