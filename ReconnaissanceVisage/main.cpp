// Modele d'utilisation d'openCV
//
#include <iostream>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "PicturePrePocessing.hpp"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{

    PicturePreProcessing myPicturePreProcessor;
    
    // lecture et affichage  image
	Mat img = imread("/Users/mikael/Documents/applications_moi/ReconnaissanceVisage/ReconnaissanceVisage/BaseImages/nathan_sartori/IMG_4385.jpg", 1 );
	imshow("Example1", img);
    waitKey(0);

    myPicturePreProcessor.preProcessPicture(img);
    
    //Mat cropped=img(roi);
    imshow("Cropped", img);

	waitKey(0);
    /*
    myPicturePreProcessor.openBatchOfPictures("/Users/mikael/Documents/applications_moi/ReconnaissanceVisage/ReconnaissanceVisage/BaseImages");*/

	return 0;
}



