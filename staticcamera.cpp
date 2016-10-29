#include "staticcamera.h"


#define CAMERA_LOOP_RESET 1
#define CAMERA_LOOP_REVERSE 2


StaticCamera::StaticCamera() :
	position(double3(0,0,10)),
	target(double3(0,0,0)),
	rotation(double3(0,0,0)),
	fieldOfView(45),
	aspectRatio(1.33),
	nearClip(1),
	farClip(1000),
	animationPeriod(10),
	animationRotation(double3(0,0,0)),
	animationPosition(double3(0,0,0)),
	animationEase(0),
	animationProgress(0),
	currentPosition(double3(0,0,10)),
	currentRotation(double3(0,0,0)),
	pivot(double3(0,0,0)),
	onPivot(false),
	loopStyle(CAMERA_LOOP_REVERSE),
	currentLoopDirection(1) {
}


void StaticCamera::OnSetSettings(){

    //cout << "Camera::AssetLibrary id = " << &Assets() << endl;

    VarMap settings = Settings();
    if(settings.IsSet("position")){
		List<double> positionList = settings.get<List<double> >("position");
		if(positionList.GetCount() == 3){
			position = double3(positionList[0],positionList[1],positionList[2]);
		}else{
			cout << " - Couldn't load position list from string settings file, no seetingsnS\n";
		}
    }
    if(settings.IsSet("rotation")){
		List<double> rotationList = settings.get<List<double> >("rotation");
		if(rotationList.GetCount() == 3){
			rotation = double3(rotationList[0],rotationList[1],rotationList[2]);
		} else if (rotationList.GetCount() == 2){
			rotation = double3(rotationList[0],rotationList[1],0);
		} else {
			cout << " - Couldn't load rotation from settings file\n";
		}
    }
    if(settings.IsSet("animation.period")){
		animationPeriod = settings.get<double>("animation.period");
		cout << "animation period is : " << animationPeriod << endl;
    }
	if(settings.IsSet("animation.rotation")){
		List<double> animationRotationList = settings.get<List<double> >("animation.rotation");
        //cout << " - set camera rotation to (" << animationRotationList[0] << "," << animationRotationList[1] << ")\n";
        if(animationRotationList.GetCount() == 3){
            animationRotation = double3(animationRotationList[0],animationRotationList[1],animationRotationList[2]);
        } else if(animationRotationList.GetCount() == 2){
            animationRotation = double3(animationRotationList[0],animationRotationList[1],0);
        } else if(animationRotationList.GetCount() == 1){
            animationRotation = double3(0,animationRotationList[1],0);
        } else {
            cout << "Come on, guy! Why would we need more than three values for 3 dimensional camera rotation?\n";
        }
	}
	if(settings.IsSet("animation.ease")){
		animationEase = settings.get<double>("animation.ease");
	}
	if(settings.IsSet("animation.position")){
		List<double> animationPositionList = settings.get<List<double> >("animation.position");
		if(animationPositionList.GetCount() == 3){
			animationPosition = double3(animationPositionList[0],animationPositionList[1],animationPositionList[2]);
		} else if(animationPositionList.GetCount() == 2){
			animationPosition = double3(animationPositionList[0],0,animationPositionList[1]);
		} else {
			cout << "animated position does not have valid values\n";
		}
	}
	if(settings.IsSet("animation.loop")){
		string loopString = settings.get<string>("animation.loop");
		stringToLower(loopString);
		if(loopString == "reset"){
			loopStyle = CAMERA_LOOP_RESET;
		} else if (loopString == "reverse"){
			loopStyle = CAMERA_LOOP_REVERSE;
		}
	}
	if(settings.IsSet("pivot")){
		onPivot = true;
		List<double> pivotList = settings.get<List<double> >("pivot");
		if(pivotList.GetCount() == 3){
			pivot = double3(pivotList[0],pivotList[1],pivotList[2]);
		} else {
			cout << "\"pivot\" property not in correct format.\n";
		}
	}
    if(settings.IsSet("animation.period")){
        animationPeriod = settings.get<double>("animation.period");
        cout << " - set animation period to " << animationPeriod << endl;
    } else {
        animationPeriod = 5;
    }
	Camera::OnSetSettings();
}

void StaticCamera::Update(){

	currentRotation = rotation
		+ (animationRotation * animationProgress * (1-animationEase))
		+ (animationRotation * (cosf(3.14*animationProgress)*.5 + .5) * animationEase);
    double prog = TimeStep() / animationPeriod;
	switch(loopStyle){
	case CAMERA_LOOP_RESET:
		animationProgress += prog;
		if(animationProgress > 1) animationProgress -= 1;
		break;
	case CAMERA_LOOP_REVERSE:
		animationProgress += prog * currentLoopDirection;
		if(currentLoopDirection == 1){
			if(animationProgress > 1){
				animationProgress = 1;
				currentLoopDirection = -1;
			}
		} else{
			if(animationProgress < 0){
				animationProgress = 0;
				currentLoopDirection = 1;
			}
		}
		break;
	}


	if(onPivot){
		double3 offset = position-pivot;
		currentPosition = double3(
							pivot.x - offset.z*sin(currentRotation.y) * cos(currentRotation.x),
							pivot.y + offset.z*sin(currentRotation.x),
							pivot.z - offset.z*cos(currentRotation.y) * cos(currentRotation.x)
						);
	} else {
		currentPosition = position
		+ (animationPosition * animationProgress * (1-animationEase))
		+ (animationPosition * (cosf(3.14*animationProgress)*.5 + .5) * animationEase);
	}

	Camera::Update();
}

bool StaticCamera::TranslateView(){
    //double r = 0;
    //h3dSetNodeTransform( _cam, 0, 10, 0, -34 ,r, 0, 1, 1, 1 );
    //h3dRender( _cam );


	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( fieldOfView, aspectRatio, nearClip, farClip );
	gluLookAt(currentPosition.x, 							//cameara currentPosition x
			currentPosition.y, 								//cameara currentPosition y
			currentPosition.z, 								//cameara currentPosition z
			currentPosition.x-sin(currentRotation.y)*cos(currentRotation.x),		//look currentPosition x
			currentPosition.y+sin(currentRotation.x),  				//look currentPosition y
			currentPosition.z-cos(currentRotation.y)*cos(currentRotation.x),  	//look currentPosition z
			sin(currentRotation.y)*sin(currentRotation.x),				//up currentRotation x
			cos(currentRotation.x),							//up currentRotation y
			cos(currentRotation.y)*sin(currentRotation.x));			//up currentRotation z

	return true;
}
