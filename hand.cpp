/*
 * 
 * Copyright 2012 Benn Snyder <benn.snyder@gmail.com>
 */

#include <iostream>
#include <sstream>
#include <string>

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

#include <ni/XnCppWrapper.h>

#include "jmolwrapper.h"


// gestures: Click, Wave
// actions: rotate, select, translate

const int CV_KEY_ESC = 1048603;

xn::GestureGenerator gestureGen;
xn::HandsGenerator handsGen;

JmolWrapper jmol = JmolWrapper("localhost", 3000);

struct handpos {
	//XnUserID id; // unsigned, so can't check for < 0
	int id;
	float x_coord;
	float y_coord;
	std::string action;
};

handpos hand1;
handpos hand2;


std::pair<float, float> coords_kinect2jmol(float kx, float ky, int kresx, int kresy, int jresx, int jresy) {
	std::pair<float, float> coords;
	coords.first = (kx * jresx) / kresx + jresx/2;
	coords.second = (ky * jresy) / kresy + jresy/2;
	return coords;
}


void XN_CALLBACK_TYPE gesture_recognized(xn::GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie) {
	printf("gesture_recognized: %s\n", strGesture);
	//gestureGen.RemoveGesture(strGesture);
	
	float fuzz = 100;
	//if (strcmp(strGesture, "Click") == 0)
		//fuzz = 100;
	//else if (strcmp(strGesture, "Wave") == 0)
		//fuzz = 300;
	
	// well, this was completely useless since it's the same as hand1.id, but I'll leave it for reference (get it?)
	//std::cout << (*(handpos* *)(pCookie))[0].id << std::endl;
	
	// we don't have hand IDs, so we have to do some rather ugly coordinate checking
	// if we had IDs, we could do
	//if (hand1.id == nId) {
	// but we don't, so
	if (hand1.x_coord > pEndPosition->X - fuzz/2 && hand1.x_coord < pEndPosition->X + fuzz/2 && hand1.y_coord > pEndPosition->Y - fuzz/2 && hand1.y_coord < pEndPosition->Y + fuzz/2) {
		printf("hand1\n");
		printf("hand1: x = %f, y = %f; pEndPosition: x = %f, y = %f; pIDPosition: x = %f, y = %f\n", hand1.x_coord, hand1.y_coord, pEndPosition->X, pEndPosition->Y, pIDPosition->X, pIDPosition->Y);
		if (strcmp(strGesture, "Click") == 0) {
			std::pair<float, float> jcoords = coords_kinect2jmol(pIDPosition->X, pIDPosition->Y, 1080, 720, 750, 550);	
			//jmol.selectWithinDistance(jcoords.first, jcoords.second, 25, 25);
			jmol.selectMoleculeWithinDistance(jcoords.first, jcoords.second, 25, 25);
			hand1.action = "select";
		} 
		else if (strcmp(strGesture, "Wave") == 0) {
			hand1.action = "translate";
		}
	}
	else if (hand2.x_coord > pEndPosition->X - fuzz/2 && hand2.x_coord < pEndPosition->X + fuzz/2 && hand2.y_coord > pEndPosition->Y - fuzz/2 && hand2.y_coord < pEndPosition->Y + fuzz/2) {
		printf("hand2\n");
		if (strcmp(strGesture, "Click") == 0) {
			//hand2.action = "translate";
			hand2.action = "rotate";
		}
		else if (strcmp(strGesture, "Wave") == 0) {
			//hand2.action = "rotate";
			//jmol.selectNone();
			hand2.action = "rotate_all";
		}
	}
	
	handsGen.StartTracking(*pEndPosition);
}

void XN_CALLBACK_TYPE gesture_process(xn::GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie) {
	printf("hello from gesture_process\n");
}

void XN_CALLBACK_TYPE hand_create(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie) {
	printf("hand_create: %d @ (%f,%f,%f)\n", nId, pPosition->X, pPosition->Y, pPosition->Z);
	
	if (hand1.id < 0) {
		hand1.id = nId;
		hand1.x_coord = pPosition->X;
		hand1.y_coord = pPosition->Y;
	}
	else if (hand2.id < 0) {
		hand2.id = nId;
		hand2.x_coord = pPosition->X;
		hand2.y_coord = pPosition->Y;
	}
}

void XN_CALLBACK_TYPE hand_update(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie) {
	//printf("hand_update: hand %d is at (%f,%f,%f)\n", nId, pPosition->X, pPosition->Y, pPosition->Z);

	handpos* hands[2] = {&hand1, &hand2};
	
	for (int i = 0; i < 2; i++) {
		if (hands[i]->id == nId) {
			if (hands[i]->action == "select") {
				std::pair<float, float> jcoords = coords_kinect2jmol(pPosition->X, pPosition->Y, 1080, 720, 750, 550);
				jmol.drawPoint2D(jcoords.first, jcoords.second);
			}
			else {
				float x_diff = hands[i]->x_coord - pPosition->X;
				float y_diff = hands[i]->y_coord - pPosition->Y;
				if (hands[i]->action == "translate")
					jmol.translate(-x_diff, y_diff, true);
				else if (hands[i]->action == "translate_all")
					jmol.translate(-x_diff, y_diff, false);
				else if (hands[i]->action == "rotate")
					jmol.rotate(x_diff, y_diff, true);
				else if (hands[i]->action == "rotate_all")
					jmol.rotate(x_diff, y_diff, false);
			}
			hands[i]->x_coord = pPosition->X;
			hands[i]->y_coord = pPosition->Y;
			break;
		}
	}
}

void XN_CALLBACK_TYPE hand_destroy(xn::HandsGenerator&generator, XnUserID nId, XnFloat fTime, void* pCookie) {
	printf("hand_destroy: %d\n", nId);
	
	if (hand1.id == nId)
		hand1.id = -1;
	else if (hand2.id == nId)
		hand2.id = -1;
	
	//gestureGen.AddGesture(GESTURE_TO_USE, NULL);
}

void retCheckNI(XnStatus ret) {
	//todo: proper error handling
	if (ret != XN_STATUS_OK) {
		printf("XN error code %i: %s\n", ret, xnGetStatusString(ret));
	}
}

int main(int argc, char **argv) {	
	hand1.id = -1;
	hand2.id = -1;
	
	XnStatus retNI;
	
	xn::Context contextNI;
	//retNI = contextNI.Init();
	retNI = contextNI.InitFromXmlFile("configNI/BasicColorAndDepth.xml", NULL);
	retCheckNI(retNI);
	
	retNI = gestureGen.Create(contextNI);
	retCheckNI(retNI);
	retNI = handsGen.Create(contextNI);
	retCheckNI(retNI);
	
	//handsGen.SetSmoothing(0.1);
	
	XnCallbackHandle h1, h2;
	
	//handpos* hands[2] = {&hand1, &hand2};	
	//gestureGen.RegisterGestureCallbacks(gesture_recognized, gesture_process, &hands, h1);
	gestureGen.RegisterGestureCallbacks(gesture_recognized, gesture_process, NULL, h1);
	handsGen.RegisterHandCallbacks(hand_create, hand_update, hand_destroy, NULL, h2);
	
	retNI = contextNI.StartGeneratingAll();
	retCheckNI(retNI);
	
	retNI = gestureGen.AddGesture("Click", NULL);
	retCheckNI(retNI);
	retNI = gestureGen.AddGesture("Wave", NULL);
	retCheckNI(retNI);
	
	xn::ImageGenerator imageGen;
	xn::ImageMetaData imageGenMD;
	retNI = imageGen.Create(contextNI);
	retCheckNI(retNI);
	imageGen.GetMetaData(imageGenMD);
	
	int xres = imageGenMD.FullXRes();
	int yres = imageGenMD.FullYRes();
	
	printf("xres = %i; yres = %i\n", xres, yres);
	
	const XnRGB24Pixel *imageMap;
	
	IplImage *bgrimg = cvCreateImage(cvSize(xres, yres), IPL_DEPTH_8U, 3);
	IplImage *rgbimg = cvCreateImage(cvSize(xres, yres), IPL_DEPTH_8U, 3);
	
	for (int key; key != CV_KEY_ESC; key = cvWaitKey(5)) {
		retNI = contextNI.WaitAndUpdateAll();
		retCheckNI(retNI);
		
		imageMap = imageGen.GetRGB24ImageMap();
		XnRGB24Pixel* imageRaw = const_cast<XnRGB24Pixel*> (imageMap);
		cvSetData(bgrimg, imageRaw, xres*3);
		cvCvtColor(bgrimg, rgbimg, CV_BGR2RGB);
		cvShowImage("RGB", rgbimg);
	}
	
	
	contextNI.Release();	
	return 0;
}
