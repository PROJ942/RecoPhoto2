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
    //string path = argv[1];
    string toReturn="";
    string path ="/Users/mikael/Documents/applications_moi/ReconnaissanceVisage/ReconnaissanceVisage/BaseImages";
    PictureProcessing myPictureProcessor;
    myPictureProcessor.initPictureProcessing(path.c_str());
    
    if (strcmp(argv[2], "createBase")==0) {
        myPictureProcessor.openBatchOfPictures(argv[1]);
    }
    else{
    // load a picture, second attr of imread : >0 -> 3 ch. =0 -> grayscale
    //Mat img = imread(argv[2], 1 );
	Mat img = imread("/Users/mikael/Documents/applications_moi/ReconnaissanceVisage/ReconnaissanceVisage/moi.jpg", 1 );
    
        if (strcmp(argv[3],"add")==0) {
            myPictureProcessor.addPictureToBase(img, "bob_denart");
        }
        else if (strcmp(argv[3],"reco")==0){
            myPictureProcessor.recognizeFace(img,toReturn);
            cout<<"found : "<<toReturn<<endl;
        }
    }
	return 0;
}



