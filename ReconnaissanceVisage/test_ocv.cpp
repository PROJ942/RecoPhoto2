// Modele d'utilisation d'openCV
//
#include <iostream>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{

	// lecture et affichage  image
	Mat img = imread("/Users/mikael/Documents/applications_moi/ReconnaissanceVisage/ReconnaissanceVisage/IMG_4474.jpg", 1 );
	imshow("Example1", img);
    
    Mat imgRouge = img.clone();

	// recupération et affichage des couleurs d'un pixel
	Vec3b coul, coul2;
    	waitKey(0);
    
    //taille de l'image
    int cols = img.cols;
    int rows = img.rows;
    
    //Mise ˆ blanc des pixels dont le rouge n'est prŽdominant
    
    for (int i=0; i<cols; i++) {
        for(int j=0;j<rows;j++){
            coul=imgRouge.at<Vec3b>(j,i);
            float ratio1 = ((float)coul[2]-(float)coul[1])/((float)coul[2]+(float)coul[1]);
            float ratio2 = (float)coul[0]/((float)coul[2]+(float)coul[1]);
            //cout<<"ratio 1 : "<<ratio1<<" ratio 2 : "<<ratio2<<endl;
           // if((int)coul[2]<=((int)coul[0]+10) || (int)coul[2]<=((int)coul[1]+5)){
            if(ratio1<0.02 || ratio1>0.45 || ratio2>0.5 || ratio2<0.1 || (int)coul[2]<=((int)coul[0]+25) || (int)coul[2]<=((int)coul[1]+15) || (int)coul[2]>245){
                coul[0]=255;
                coul[1]=255;
                coul[2]=255;
                imgRouge.at<Vec3b>(j,i)=coul;
            }
        }
    }
    
    
    imshow("Mask", imgRouge);
    waitKey(0);
    
    //Passage en niveau de gris
    Mat imgGray;
    cvtColor( imgRouge, imgGray, CV_BGR2GRAY );
    //blur pour Žliminer les artefacts
    blur( imgGray, imgGray, Size(6,6) );
    
    imshow("Gray et blur", imgGray);
    waitKey(0);
    
    // Threshold pour avoir une image binaire
    threshold(imgGray,imgGray,170,255,1);
    imshow("Binaire", imgGray);
    waitKey(0);
    
    /*Ouverture*/
    int erosion_size = 15;
    Mat element = getStructuringElement(MORPH_ELLIPSE,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ));
    erode(imgGray,imgGray,element);
    dilate(imgGray,imgGray, element);
    
    imshow("ouverture", imgGray);
    waitKey(0);
    
    /*Fermeture*/
    erosion_size = 10;
    element = getStructuringElement(MORPH_ELLIPSE,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ));

    dilate(imgGray,imgGray, element);
    //erode(imgGray,imgGray,element);

    
    imshow("fermeture", imgGray);
    waitKey(0);

    //std::cout<<"lignes : "<<rows<<" et colonnes : "<<cols<<std::endl;
    
    // Recherche de contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchie;
    findContours(imgGray, contours,  hierarchie, RETR_TREE,  CHAIN_APPROX_SIMPLE, Point(0,0));
    cout<<"nb de contours trouves : "<<contours.size()<<endl;
    // Pour chaque contour
    // Approximation ˆ un polygone
    //box a garder
    int maxBoxHeight=0;
    Rect boxMax;
    for(int i=0;i<contours.size();i++){
        
        
        Rect box = boundingRect(contours[i]);
        cout<<"box trouvee, largeur * hauteur : "<<box.width<<" "<<box.height<<endl;
        //drawContours(img, contours, i, Scalar(0,0,255), 2,8,hierarchie,0,Point());
        if (box.height>maxBoxHeight) {
            boxMax=box;
            
        }
        
    }
    // drawing the resulting image
    rectangle(img,boxMax,cv::Scalar(0,0,255), 1, 8, 0);
    
    


    //Decoupe de l'image (Rect(x,y,w,h)
    //Rect roi(colonneGauche,ligneHautTete,(colonneDroit-colonneGauche),(colonneDroit-colonneGauche)*1.3);
    //Mat cropped=img(roi);
    imshow("Cropped", img);

	waitKey(0);

	return 0;
}



