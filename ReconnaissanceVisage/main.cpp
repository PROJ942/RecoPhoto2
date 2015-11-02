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
    //chemin vers la base d'images
    string path = argv[1];
    string toReturn="";
    PictureProcessing myPictureProcessor;
    myPictureProcessor.initPictureProcessing(path.c_str());
    
    if (strcmp(argv[2], "createBase")==0) {
        cout<<"chemin vers la base : "<<argv[1]<<endl<<"creation de la base d'images..."<<endl;
        myPictureProcessor.openBatchOfPictures(argv[1]);
    }
    else{
    // charge l'image à traiter, second attr d'imread : >0 -> 3 ch. =0 -> grayscale
    Mat img = imread(argv[2], 1 );
    
        if (strcmp(argv[3],"add")==0) {
            cout<<"ajout d'une photo a la base..."<<endl;
            myPictureProcessor.addPictureToBase(img, argv[4]);
        }
        else if (strcmp(argv[3],"reco")==0){
            cout<<"reconnaissance en cours..."<<endl;
            myPictureProcessor.recognizeFace(img,toReturn);
        }
    }
	return 0;
}



