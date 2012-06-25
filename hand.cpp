/*
 * hand.cpp
 * Tracks hand movements and gestures using OpenNI and sends scripting events to Jmol.
 * 	gestures: Click, Wave
 * 	actions: rotate, rotate_all, select, select_all, select_none, select_atom, select_molecule, translate, translate_all
 *
 *
 * Copyright 2012 Benn Snyder <benn.snyder@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <sstream>
#include <string>

#include <boost/program_options.hpp>

#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>

#include <ni/XnCppWrapper.h>

#include "jmolwrapper.h"

// configuration globals
int jport, jres_x, jres_y;
std::string jhost, xml_config, h1_click, h1_wave, h2_click, h2_wave;
float smoothing_factor;
bool verbose;

// other globals
const int num_actions = 9;
const std::string actions[num_actions] = {"rotate", "rotate_all", "select", "select_all",
"select_none", "select_atom", "select_molecule", "translate", "translate_all"};
const int CV_KEY_ESC = 1048603;
xn::GestureGenerator gestureGen;
xn::HandsGenerator handsGen;
JmolWrapper* jmol;

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

bool string_in_array(std::string target, const std::string arr[], int size) {
	for (int i = 0; i < size; i++)
		if (target == arr[i])
			return true;
	return false;
}

void XN_CALLBACK_TYPE gesture_recognized(xn::GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pIDPosition, const XnPoint3D* pEndPosition, void* pCookie) {
	if (verbose)
		printf("gesture_recognized: %s\n", strGesture);

	float fuzz = 50;
	std::pair<float, float> jcoords = coords_kinect2jmol(pIDPosition->X, pIDPosition->Y, 1080, 720, jres_x, jres_y);

	// this was completely useless since it's the same as hand1.id, but I'll leave it for reference (get it?)
	//std::cout << (*(handpos* *)(pCookie))[0].id << std::endl;

	// todo: generalize hands

	// we don't have hand IDs, so we have to do some rather ugly coordinate checking
	// x coords are closer matches than y coords, so divide fuzz by 2 for x
	// if we had IDs, we could do
	//if (hand1.id == nId) {
	// but we don't, so
	if (hand1.x_coord > pEndPosition->X - fuzz/2 && hand1.x_coord < pEndPosition->X + fuzz/2 && hand1.y_coord > pEndPosition->Y - fuzz && hand1.y_coord < pEndPosition->Y + fuzz) {
		if (verbose)
			printf("hand1 performed a %s at x = %f, y = %f; pEndPosition: x = %f, y = %f; pIDPosition: x = %f, y = %f\n", strGesture, hand1.x_coord, hand1.y_coord, pEndPosition->X, pEndPosition->Y, pIDPosition->X, pIDPosition->Y);

		if (strcmp(strGesture, "Click") == 0)
			hand1.action = h1_click;
		else if (strcmp(strGesture, "Wave") == 0)
			hand1.action = h1_wave;

		if (hand1.action == "select_atom")
			jmol->selectWithinDistance(jcoords.first, jcoords.second, 25, 25);
		else if (hand1.action == "select_molecule")
			jmol->selectMoleculeWithinDistance(jcoords.first, jcoords.second, 25, 25);
	}
	else if (hand2.x_coord > pEndPosition->X - fuzz/2 && hand2.x_coord < pEndPosition->X + fuzz/2 && hand2.y_coord > pEndPosition->Y - fuzz && hand2.y_coord < pEndPosition->Y + fuzz) {
		if (verbose)
			printf("hand2 performed a %s at x = %f, y = %f; pEndPosition: x = %f, y = %f; pIDPosition: x = %f, y = %f\n", strGesture, hand1.x_coord, hand1.y_coord, pEndPosition->X, pEndPosition->Y, pIDPosition->X, pIDPosition->Y);

		if (strcmp(strGesture, "Click") == 0)
			hand2.action = h2_click;
		else if (strcmp(strGesture, "Wave") == 0)
			hand2.action = h2_wave;

		if (hand2.action == "select_atom")
			jmol->selectWithinDistance(jcoords.first, jcoords.second, 25, 25);
		else if (hand2.action == "select_molecule")
			jmol->selectMoleculeWithinDistance(jcoords.first, jcoords.second, 25, 25);
	}

	handsGen.StartTracking(*pEndPosition);
}

void XN_CALLBACK_TYPE gesture_process(xn::GestureGenerator& generator, const XnChar* strGesture, const XnPoint3D* pPosition, XnFloat fProgress, void* pCookie) {
	if (verbose)
		printf("%s in progress\n", strGesture);
}

void XN_CALLBACK_TYPE hand_create(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie) {
	if (verbose)
		printf("hand_create: %d @ (%f,%f,%f)\n", nId, pPosition->X, pPosition->Y, pPosition->Z);

	handpos* hands[2] = {&hand1, &hand2};

	for (int i = 0; i < 2; i++) {
		if (hands[i]->id < 0) {
			hands[i]->id = nId;
			hands[i]->x_coord = pPosition->X;
			hands[i]->y_coord = pPosition->Y;
			break;
		}
		else if (i == 2)
			if (verbose)
				printf("already tracking two hands\n");
	}
}

void XN_CALLBACK_TYPE hand_update(xn::HandsGenerator& generator, XnUserID nId, const XnPoint3D* pPosition, XnFloat fTime, void* pCookie) {
	//if (verbose)
	//	printf("hand_update: hand %d is at (%f,%f,%f)\n", nId, pPosition->X, pPosition->Y, pPosition->Z);

	handpos* hands[2] = {&hand1, &hand2};

	for (int i = 0; i < 2; i++) {
		if (hands[i]->id == nId) {
			if (hands[i]->action.compare(0, 6, "select") == 0) {
				std::pair<float, float> jcoords = coords_kinect2jmol(pPosition->X, pPosition->Y, 1080, 720, jres_x, jres_y);
				jmol->drawPoint2D(jcoords.first, jcoords.second);
				if (hands[i]->action == "select_all")
					jmol->selectAll();
				else if (hands[i]->action == "select_none")
					jmol->selectNone();
			}
			else {
				printf("here with action = %s\n", hands[i]->action.c_str());
				float x_diff = hands[i]->x_coord - pPosition->X;
				float y_diff = hands[i]->y_coord - pPosition->Y;
				if (hands[i]->action == "translate")
					jmol->translate(-x_diff, y_diff, true);
				else if (hands[i]->action == "translate_all")
					jmol->translate(-x_diff, y_diff, false);
				else if (hands[i]->action == "rotate")
					jmol->rotate(x_diff, y_diff, true);
				else if (hands[i]->action == "rotate_all")
					jmol->rotate(x_diff, y_diff, false);
			}
			hands[i]->x_coord = pPosition->X;
			hands[i]->y_coord = pPosition->Y;
			break;
		}
	}
}

void XN_CALLBACK_TYPE hand_destroy(xn::HandsGenerator&generator, XnUserID nId, XnFloat fTime, void* pCookie) {
	if (verbose)
		printf("hand_destroy: %d\n", nId);

	handpos* hands[2] = {&hand1, &hand2};

	for (int i = 0; i < 2; i++) {
		if (hands[i]->id == nId) {
			hands[i]->id = -1;
			break;
		}
	}
}

void retCheckNI(XnStatus ret) {
	//todo: proper error handling
	if (ret != XN_STATUS_OK) {
		printf("XN error code %i: %s\n", ret, xnGetStatusString(ret));
	}
}

int parse_args(int argc, char **argv) {
	namespace po = boost::program_options;

	po::options_description desc("Options");
	desc.add_options()
		("help,h", "Print help message")
		("verbose,v", po::value<bool>(&verbose)->zero_tokens(), "Be verbose")
		("xml-config,x", po::value<std::string>(&xml_config)->default_value("configNI/BasicColorAndDepth.xml"), "Configuration file for OpenNI")
		("jhost,j", po::value<std::string>(&jhost)->default_value("localhost"), "Host where Jmol is running")
		("jport,p", po::value<int>(&jport)->default_value(3000), "Port where Jmol is listening")
		("jres-x,x", po::value<int>(&jres_x)->default_value(750), "x resolution of the Jmol view")
		("jres-y,y", po::value<int>(&jres_y)->default_value(550), "y resolution of the Jmol view")
		("smoothing,s", po::value<float>(&smoothing_factor), "Smoothing value for hand tracking - 0 <= s <= 1")
		("h1-click", po::value<std::string>(&h1_click)->default_value("select_molecule"), "Click action for first hand")
		("h1-wave", po::value<std::string>(&h1_wave)->default_value("translate"), "Wave action for first hand")
		("h2-click", po::value<std::string>(&h2_click)->default_value("rotate"), "Click action for second hand")
		("h2-wave", po::value<std::string>(&h2_wave)->default_value("rotate_all"), "Wave action for second hand")
	;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		std::cout << "Available actions:" << std::endl;
		for (int i = 0; i < num_actions; i++)
			std::cout << actions[i] << ", ";
		std::cout << std::endl;
		exit(1);
	}
	if (!vm.count("smoothing"))
		smoothing_factor = -1;

	if (vm.count("h1-click")) {
		if (!string_in_array(h1_click, actions, num_actions)) {
			printf("invalid parameter for h1-click\n");
			exit(1);
		}
		else
			printf("%s\n", h1_click.c_str());
	}
	if (vm.count("h1-wave")) {
		if (!string_in_array(h1_wave, actions, num_actions)) {
			printf("invalid parameter for h1-wave\n");
			exit(1);
		}
	}
	if (vm.count("h2-click")) {
		if (!string_in_array(h2_click, actions, num_actions)) {
			printf("invalid parameter for h2-click\n");
			exit(1);
		}
	}
	if (vm.count("h2-wave")) {
		if (!string_in_array(h2_wave, actions, num_actions)) {
			printf("invalid parameter for h2-wave\n");
			exit(1);
		}
	}
}

int main(int argc, char **argv) {
	parse_args(argc, argv);
	verbose = true;

	jmol = new JmolWrapper(jhost, jport);
	hand1.id = -1;
	hand2.id = -1;

	XnStatus retNI;
	xn::Context contextNI;

	retNI = contextNI.InitFromXmlFile(xml_config.c_str(), NULL);
	retCheckNI(retNI);
	retNI = gestureGen.Create(contextNI);
	retCheckNI(retNI);
	retNI = handsGen.Create(contextNI);
	retCheckNI(retNI);

	if (smoothing_factor >= 0 && smoothing_factor <= 1)
		handsGen.SetSmoothing(smoothing_factor);
	else if (verbose)
		printf("smoothing factor not set or out of range, using default\n");

	XnCallbackHandle h1, h2;
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

	if (verbose)
		printf("image: xres = %i; yres = %i\n", xres, yres);

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
		cvShowImage("Molect Viewer", rgbimg);
	}


	contextNI.Release();
	return 0;
}
