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
	Mat img = imread("/Users/mikael/Documents/applications_moi/ReconnaissanceVisage/ReconnaissanceVisage/FullSizeRender.jpg", 1 );
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
            if(ratio1<0.02 || ratio1>0.45 || ratio2>0.5 || ratio2<0.1){
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
    
    /*Ouverture*/
    int erosion_size = 10;
    Mat element = getStructuringElement(MORPH_ELLIPSE,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ));
    erode(imgGray,imgGray,element);
    dilate(imgGray,imgGray, element);
    
    /*Fermeture*/
    dilate(imgGray,imgGray, element);
    erode(imgGray,imgGray,element);

    
    imshow("Erode", imgGray);
    waitKey(0);

    std::cout<<"lignes : "<<rows<<" et colonnes : "<<cols<<std::endl;
    int debut1 = 0;
    int ligneHautTete=10;
    int colonneGauche=10;
    int colonneDroit=cols-10;
    int moy1, moy2, diff;
    int difference=0;
    

    
    
    // Threshold pour avoir une image binaire
    threshold(imgGray,imgGray,180,255,1);
    imshow("Binaire", imgGray);
    waitKey(0);
    
    //detection du haut de la tete
    do {
        moy1=0;
        moy2=0;
        difference=0;
        for (int i=0; i<(cols/40); i++) {
            for(int j=0;j<40;j++){
                moy1+=imgGray.at<uchar>(ligneHautTete,debut1+j+(i*40));
                moy2+=imgGray.at<uchar>((ligneHautTete+1),debut1+j+(i*40));

            }
            moy1/=40;
            moy2/=40;
            diff = (moy1-moy2)*(moy1-moy2);
            if(diff>difference)
                difference=diff;
        }
 
        ligneHautTete++;
    }
    while (difference < 100 && ligneHautTete <rows);
    std::cout<<"Ligne du haut de la tete : "<<ligneHautTete<<std::endl;
    
    //detection du cote gauche
    int debut2 = ligneHautTete;
    do {
        moy1=0;
        moy2=0;
        difference=0;
        for(int i=1;i<((rows-ligneHautTete)/80);i++){
            for(int j=0;j<40;j++){
                moy1+=imgGray.at<uchar>(debut2+j+(i*40),colonneGauche);
                moy2+=imgGray.at<uchar>((debut2+j+(i*40)),colonneGauche+1);
            }
            moy1/=40;
            moy2/=40;
            diff = (moy1-moy2)*(moy1-moy2);
            if(diff>difference)
                difference=diff;
        }
        colonneGauche++;
    }
    while (difference < 100 && colonneGauche <cols);
    std::cout<<"colonne a gauche de la tete : "<<colonneGauche<<std::endl;
    
    //detection du cote droit
    int debut3 = ligneHautTete;
    do {
        moy1=0;
        moy2=0;
        difference=0;
        for(int i=0;i<((rows-ligneHautTete)/80);i++){
            for(int j=0;j<40;j++){
                moy1+=imgGray.at<uchar>(debut3+j+(i*40),colonneDroit);
                moy2+=imgGray.at<uchar>((debut3+j+(i*40)),colonneDroit-1);
            }
            moy1/=40;
            moy2/=40;
            diff = (moy1-moy2)*(moy1-moy2);
            if(diff>difference)
                difference=diff;
        }
        colonneDroit--;
    }
    while (difference < 100 && colonneDroit >0);
    std::cout<<"colonne a droite de la tete : "<<colonneDroit<<std::endl;
    
    //detection du bas
    /*int debut4 = colonneGauche;
    int fin = colonneDroit;
    do {
        moy1=0;
        moy2=0;
        difference=0;
        for(int i=0;i<((rows-ligneHautTete)/80);i++){
            for(int j=0;j<40;j++){
                moy1+=imgGray.at<uchar>(debut3+j+(i*40),colonneDroit);
                moy2+=imgGray.at<uchar>((debut3+j+(i*40)),colonneDroit-1);
            }
            moy1/=40;
            moy2/=40;
            diff = (moy1-moy2)*(moy1-moy2);
            if(diff>difference)
                difference=diff;
        }
        colonneDroit--;
    }
    while (difference < 100 && colonneDroit >0);
    std::cout<<"colonne a droite de la tete : "<<colonneDroit<<std::endl;*/
    
    
    //Decoupe de l'image (Rect(x,y,w,h)
    Rect roi(colonneGauche,ligneHautTete,(colonneDroit-colonneGauche),(colonneDroit-colonneGauche)*1.3);
    Mat cropped=img(roi);
    imshow("Cropped", cropped);
    
	waitKey(0);

	return 0;
}



