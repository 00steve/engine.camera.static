#ifndef STATIC_CAMERA_H
#define STATIC_CAMERA_H

#include <engine/core/double3.h>
#include <engine/graphics/camera.h>
#include <gl/gl.h>
#include <gl/glu.h>


class StaticCamera : public Camera{
private:

	double3 position;
	double3 target;
	double3 rotation;

	double fieldOfView;
	double aspectRatio;
	double nearClip;
	double farClip;

	double animationPeriod;
	double3 animationRotation;
	double3 animationPosition;
	double animationEase;
	double animationProgress;

	double3 currentPosition;
	double3 currentRotation;
	double3 pivot;
	bool onPivot;

	unsigned short loopStyle;
	double currentLoopDirection;



protected:

    virtual void OnSetSettings();

public:

	StaticCamera();

	virtual void Update();
	bool TranslateView();

};




#endif // STATIC_CAMERA_H
