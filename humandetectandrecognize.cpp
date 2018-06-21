/*
 * humandetectandrecognize.cpp
 *
 *  Created on: Mar 3, 2015
 *      Author: admin
 */
#include "opencv2/core/core.hpp"
#include "opencv2/contrib/contrib.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <dir.h>

using namespace cv;
using namespace std;
String face_cascade_name = "C:/openCVinstaller/opencv/sources/data/haarcascades/haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
RNG rng(12345);
int counts=0;
vector<Mat> images;
vector<int> labels;
string fn_csv = "E:/BE_Project/csvdata.csv";


static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';')
{
    std::ifstream file(filename.c_str(), ifstream::in);
    if (!file) {
        string error_message = "No valid input file was given, please check the given filename.";
        CV_Error(CV_StsBadArg, error_message);
    }
    string line, path, classlabel;
    while (getline(file, line)) {
        stringstream liness(line);
        getline(liness, path, separator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            images.push_back(imread(path, 0));
            labels.push_back(atoi(classlabel.c_str()));
        }
    }
}
int recognize(Mat img)
{
    try
	  {
		read_csv(fn_csv, images, labels);
	  }
	  catch (cv::Exception& e)
	  {
	      cerr << "Error opening file \"" << fn_csv << "\". Reason: " << e.msg << endl;
	        // nothing more we can do
	        exit(1);
	  }
		    // Quit if there are not enough images for this demo.
	    if(images.size() <= 1)
	    {
		        string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
		        CV_Error(CV_StsError, error_message);
	    }
	    Ptr<FaceRecognizer> model = createEigenFaceRecognizer(0,3500);
	    model->train(images, labels);
	    cout<<"Model created and trained.\n";

	    std::vector<Rect> faces;
	    Mat frame,frame_gray,testSample;

	    frame=img;
	    cvtColor( frame, frame_gray, CV_BGR2GRAY );
	    equalizeHist( frame_gray, frame_gray );
	    //-- Detect faces
	    face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

	    for( size_t i = 0; i < faces.size(); i++ )
	    {
	      Mat faceROI = frame_gray( faces[i] );
	      resize(faceROI,testSample,Size(200.0,200.0),0,0, INTER_NEAREST );
	    }

	   // imshow("people detector", testSample);
	    //waitKey(0);
    int predictedLabel = model->predict(testSample);
    double confidence = 0.0;
    model->predict(testSample, predictedLabel, confidence);
    printf("%d",predictedLabel);
    return predictedLabel;
	    }

void MyLine(Mat img,Point centre)
{
	int thickness =2;
	int lineType=8;
	circle(img,centre,1,Scalar(0,255,0),thickness,lineType);
	}
int main(int argc, char** argv)
{
	if (!face_cascade.load(face_cascade_name))
		       cout<<"couldnt load face algo\n";
	HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    int person;
    Mat temp;
    String name;
    Size p(640,480);
    Scalar green(0,255,0);
    Scalar red(0,0,255);
    VideoCapture cap("E:/BE_Project/Videos/rishabh.mp4");
    Mat ip = imread("C:/Users/admin/Desktop/Ankita/photo.png");
    int count=0;
    while(1)
    {
    	Mat img;																//added
    	Mat imgr;
    	//cap >> imgr;
    	imgr=ip;
    	count++;
    	if(count%10==0)
    	{}
    	else
    		continue;

   	resize(imgr,img,p);
    if(!img.data)
    	return(1);

    //fflush(stdout);
    vector<Rect> found, found_filtered;
    double t = (double)getTickCount();
    // run the detector with default parameters. to get a higher hit-rate
    // (and more false alarms, respectively), decrease the hitThreshold and
    // groupThreshold (set groupThreshold to 0 to turn off the grouping completely).
    hog.detectMultiScale(img, found, 0, Size(8,8), Size(32,32), 1.05, 2);
    t = (double)getTickCount() - t;
    printf("tdetection time = %gms\n", t*1000./cv::getTickFrequency());

    size_t i, j;
    for( i = 0; i < found.size(); i++ )
    {
         Rect r = found[i];
         for( j = 0; j < found.size(); j++ )
             if( j != i && (r & found[j]) == r)
                 break;
         if( j == found.size() )
             found_filtered.push_back(r);
    }
    for( i = 0; i < found_filtered.size(); i++ )
    {
    	//Mat dperson=temp(found_filtered);
    	//Mat dperson1=img;//(found_filtered[i]);
    	//Mat dperson;
    	//resize(dperson1,dperson,Size(200,200),0,0,INTER_LINEAR);
    	//imshow("people detector", dperson1);
    	person=recognize(img);
        switch(person)
        {
        	case 0: name="ankita"; break;
        	case 1: name="parth";	break;
        	case 2: name="nidhi"; break;
        	case 3: name="rishabh"; break;
        	default: name="unknown";
        }
        putText(img,name,Point(100,100),FONT_HERSHEY_PLAIN,2.0,Scalar(255,0,0),3,8,false);
        Rect r = found_filtered[i];
                // the HOG detector returns slightly larger rectangles than the real objects.
                // so we slightly shrink the rectangles to get a nicer output.
                r.x += cvRound(r.width*0.1);
                r.width = cvRound(r.width*0.8);
                r.y += cvRound(r.height*0.07);
                r.height = cvRound(r.height*0.8);
               if(person==0||person==1||person==2||person==3)
                rectangle(img, r.tl(), r.br(), green, 3);
               else
            	   rectangle(img, r.tl(), r.br(), red, 3);
               namedWindow("people detector", 1);
               imshow("people detector", img);
                MyLine(img,Point((r.width+r.x)/2,(r.height+r.y)/2));

    }
    if(waitKey(1)=='27')
           	break;
    }
    return 0;
}

