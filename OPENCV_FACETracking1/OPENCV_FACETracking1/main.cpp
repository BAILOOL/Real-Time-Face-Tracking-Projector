#include <objdetect\objdetect.hpp> //opencv library
#include <highgui\highgui.hpp> //opencv library
#include <imgproc\imgproc.hpp> //opencv library
#include <time.h> 
#include <iostream>
#include <stdio.h>
#include "wtypes.h" //for obtaining screen resolution

using namespace std;
using namespace cv;

/*structure variables type for saving parameters of region of interest(roi)*/
struct roi_variables
{
	int x;
	int y;
	int w;
	int h;
	int offset_x;
	int offset_y;
};

int sW = 0, sH = 0; //screen width and hight
int xloc=0, yloc=0, wval=0, hval=0; //parameters for window calibration
int captureH, captureW; //width and hight of frame obtained from camera
int cuttedH, cuttedW; //width and hight of the cutted version of the frame obtained from camera
int calibration_offset = 50; //calibration variable
double cutted_percentage_y =0.4, cutted_percentage_w = 0.5; //percentage that is used for cutting the frame obtained form camera


/* Get the horizontal and vertical screen sizes in pixel */
void GetDesktopResolution(int& horizontal, int& vertical)
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}


/*obtaining region of interest(roi) parameters*/
struct roi_variables roi(int x1,int y1,int w1,int h1,int offset_x1, int offset_y1)
{
	int region_par = 50;
	struct roi_variables parameters1;

			x1 =offset_x1 +x1;
			y1 =offset_y1+y1;

			if (x1-region_par<0)
				x1 = 0;
			else
				x1 = x1 - region_par;
			
			if (y1-region_par<0)
				y1 = 0;
			else 
				y1 = y1 - region_par;

			if (x1 + w1 + 2*region_par >captureW)
				w1 = captureW-x1;
			else 
				w1 = w1+2*region_par;
			
			if (y1 + h1 + 2*region_par>floor(cutted_percentage_w*captureH))
				h1 = floor(cutted_percentage_w*captureH)-y1;
			else
				h1 = h1+2*region_par;

			offset_x1 = x1;
			offset_y1 = y1;

			parameters1.x = x1;
			parameters1.y = y1;
			parameters1.w = w1;
			parameters1.h = h1;
			parameters1.offset_x = offset_x1;
			parameters1.offset_y = offset_y1;

			return parameters1;
}

int main(int argc, const char** argv)
{
	int movingRectH,movingRectW; //width and hight of the black rectangular window drawn on the screen 
	struct roi_variables parameters;
	
	//initializing region of interest(roi) parameters
	parameters.x = 0;
	parameters.y = 0;
	parameters.w = 640;
	parameters.h = floor(0.5*480);
	parameters.offset_x = 0;
	parameters.offset_y = 0;

    //create the cascade classifier object used for the face detection
    CascadeClassifier face_cascade;
    //use the haarcascade_frontalface_alt2.xml library
    face_cascade.load("haarcascade_frontalface_alt2.xml");
 
    //setup video capture device and link it to the first capture device
    VideoCapture captureDevice;
    captureDevice.open(0);
	waitKey(3000);

    //setup image files used in the capture process
    Mat captureFrame;
	Mat movingRect(640,480,CV_64F);
    Mat grayscaleFrame;

    //obtaining screen resolution
	GetDesktopResolution(sW, sH);

    //create a loop to capture and find faces
    while(true)
    {
        //capture a new image frame
        captureDevice>>captureFrame;
		captureH = captureFrame.rows;
		captureW = captureFrame.cols;

		movingRectH = movingRect.rows;
		movingRectW = movingRect.cols;

		//geting cutted version of the captureFrame
		Mat cuttedVersion(captureFrame(Rect(0,floor(cutted_percentage_y*captureH),captureW,floor(cutted_percentage_w*captureH))));
		cuttedH = cuttedVersion.rows;
		cuttedW = cuttedVersion.cols;

		//gettting region of interest from captureFrame
		Mat roiImg(cuttedVersion,Rect(parameters.x,parameters.y,parameters.w,parameters.h));
	
		//convert captured image to gray scale and equalize
        cvtColor(roiImg, grayscaleFrame, CV_BGR2GRAY);
        equalizeHist(grayscaleFrame, grayscaleFrame);
		
        //create a vector array to store the face found
        std::vector<Rect> faces;
 
        //find faces and store them in the vector array
        face_cascade.detectMultiScale(grayscaleFrame, faces, 1.1, 3, CV_HAAR_FIND_BIGGEST_OBJECT|CV_HAAR_SCALE_IMAGE, Size(30,30));

        //draw a black rectangle in the moving window
		Point pt1(0, 0);
        Point pt2(movingRectW, movingRectH);
        rectangle(movingRect, pt1, pt2, cvScalar(0, 0, 0, 0), -1, 8, 0);
		
		for(int i = 0; i < faces.size(); i++)
        {
			
			xloc = faces[i].x+parameters.offset_x;
			yloc = faces[i].y+parameters.offset_y;
			wval = faces[i].width;
			hval = faces[i].height;
			
			//obtaining region of interest(roi) parameters
			parameters = roi(faces[i].x,faces[i].y,faces[i].width,faces[i].height,parameters.offset_x,parameters.offset_y);
			
			Point pt1(faces[i].x + faces[i].width, faces[i].y + faces[i].height);
            Point pt2(faces[i].x, faces[i].y);
			
            rectangle(roiImg, pt1, pt2, cvScalar(0, 255, 0, 0), 1, 8, 0);
        }

		
		//print the output
        //imshow("outputCapture", captureFrame);
		//imshow("ROI", roiImg);
		imshow("Rectangle", movingRect);
		//imshow("Cutted",cuttedVersion);

		//Calibrate window
		if(faces.size() > 0)
		{
			xloc = xloc*sW/cuttedW;
			yloc = yloc*sH/cuttedH;
			wval = wval*sW/cuttedW;
			hval = hval*sH/cuttedH;
			
			resizeWindow("Rectangle",wval, hval);
			moveWindow("Rectangle", xloc-calibration_offset, yloc+calibration_offset);

		}

		else
		{
			resizeWindow("Rectangle",wval, hval);
			moveWindow("Rectangle", -1000, -1000);
			parameters.x = 0;
			parameters.y = 0;
			parameters.w = captureW;
			parameters.h = floor(cutted_percentage_w*captureH);
			parameters.offset_x = 0;
			parameters.offset_y = 0;
		}
 
        //pause for 1	ms
        waitKey(1);
    }
    return 0;
}