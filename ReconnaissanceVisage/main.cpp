// Modele d'utilisation d'openCV
//
#include <iostream>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "PictureProcessing.hpp"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{

    string path ="/Users/mikael/Documents/applications_moi/ReconnaissanceVisage/ReconnaissanceVisage/BaseImages";
    PictureProcessing myPictureProcessor;
    myPictureProcessor.initPictureProcessing(path.c_str());
    
    
    // load a picture, second attr of imread : >0 -> 3 ch. =0 -> grayscale
	Mat img = imread("/Users/mikael/Documents/applications_moi/ReconnaissanceVisage/ReconnaissanceVisage/moi.jpg", 1 );
    /*
    myPictureProcessor.openBatchOfPictures("/Users/mikael/Documents/applications_moi/ReconnaissanceVisage/ReconnaissanceVisage/BaseImages");
    string result;
    myPictureProcessor.recognizeFace(img,result);
    cout<<"found : "<<result<<endl;*/
    myPictureProcessor.addPictureToBase(img, "bob_denart");
    
	return 0;
}



