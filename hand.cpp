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


#define GESTURE_TO_USE "Click"
//#define GESTURE_TO_USE "Wave"
const int CV_KEY_ESC = 1048603;

xn::GestureGenerator gestureGen;
xn::HandsGenerator handsGen;

JmolWrapper jwrap = JmolWrapper("localhost", 3000);

float x_coord, y_coord = 0;

struct handpos {
	int id;
	float x_coord;
	float y_coord;
};

handpos hand1;
handpos hand2;


void XN_CALLBACK_TYPE gesture_recognized(xn::GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie) {
	printf("gesture_recognized: %s\n", strGesture);
	//gestureGen.RemoveGesture(strGesture);
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
	
	
	//x_coord = pPosition->X;
	//y_coord = pPosition->Y;
}

void XN_CALLBACK_TYPE hand_update(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie) {
	printf("hand_update: hand %d is at (%f,%f,%f)\n", nId, pPosition->X, pPosition->Y, pPosition->Z);

	float x_diff, y_diff;
	
	if (hand1.id == nId) {
		x_diff = hand1.x_coord - pPosition->X;
		y_diff = hand1.y_coord - pPosition->Y;
		hand1.x_coord = pPosition->X;
		hand1.y_coord = pPosition->Y;
		jwrap.rotate(x_diff, y_diff);
	}
	else if (hand2.id == nId) {
		x_diff = hand2.x_coord - pPosition->X;
		y_diff = hand2.y_coord - pPosition->Y;
		hand2.x_coord = pPosition->X;
		hand2.y_coord = pPosition->Y;
		jwrap.translate(-x_diff, y_diff);
	}
	
	
	
	//float x_diff = x_coord - pPosition->X;
	//float y_diff = y_coord - pPosition->Y;
	//x_coord = pPosition->X;
	//y_coord = pPosition->Y;
	
	//jwrap.rotate(x_diff, y_diff);
	//jwrap.translate(-x_diff, y_diff);
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
	//todo: add proper error handling
	if (ret != XN_STATUS_OK) {
		printf("XN error code %i: %s\n", ret, xnGetStatusString(ret));
	}
}

int main(int argc, char **argv)
{	
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
	gestureGen.RegisterGestureCallbacks(gesture_recognized, gesture_process, NULL, h1);
	handsGen.RegisterHandCallbacks(hand_create, hand_update, hand_destroy, NULL, h2);
	
	retNI = contextNI.StartGeneratingAll();
	retCheckNI(retNI);
	
	retNI = gestureGen.AddGesture(GESTURE_TO_USE, NULL);
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
	
	for (int key; key != CV_KEY_ESC; key = cvWaitKey(5)) {
		retNI = contextNI.WaitAndUpdateAll();
		retCheckNI(retNI);
		
		imageMap = imageGen.GetRGB24ImageMap();
		XnRGB24Pixel* imageRaw = const_cast<XnRGB24Pixel*> (imageMap);
		cvSetData(bgrimg, imageRaw, xres*3);
		//cvCvtColor(bgrimg, bgrimg, CV_BGR2RGB); // segfault, wtf?
		cvShowImage("BGR", bgrimg);
	}
	
	//contextNI.Shutdown();
	contextNI.Release();

	
	return 0;
}
