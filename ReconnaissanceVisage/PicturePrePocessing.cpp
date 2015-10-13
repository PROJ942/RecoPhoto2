//
//  PicturePrePocessing.cpp
//  ReconnaissanceVisage
//
//  Created by Mikael on 12/10/2015.
//  Copyright © 2015 mikael. All rights reserved.
//

#include "PicturePrePocessing.hpp"

using namespace cv;
using namespace std;

void PicturePreProcessing::preProcessPicture(Mat &pictureToProcess){
    
    Mat pictureMask = pictureToProcess.clone();
    
    // definition d'un vecteur pour les couleurs d'un pixel
    Vec3b coul;
    
    //taille de l'image
    int cols = pictureMask.cols;
    int rows = pictureMask.rows;
    
    //Mise à blanc des pixels qui ne sont pas de la peau
    for (int i=0; i<cols; i++) {
        for(int j=0;j<rows;j++){
            coul=pictureMask.at<Vec3b>(j,i);
            float ratio1 = ((float)coul[2]-(float)coul[1])/((float)coul[2]+(float)coul[1]);
            float ratio2 = (float)coul[0]/((float)coul[2]+(float)coul[1]);
            if(ratio1<0.02 || ratio1>0.45 || ratio2>0.5 || ratio2<0.1 || (int)coul[2]<=((int)coul[0]+25) || (int)coul[2]<=((int)coul[1]+15) || (int)coul[2]>245){
                coul[0]=255;
                coul[1]=255;
                coul[2]=255;
                pictureMask.at<Vec3b>(j,i)=coul;
            }
        }
    }
    
    //Passage en niveau de gris
    cvtColor( pictureMask, pictureMask, CV_BGR2GRAY );
    //blur pour éliminer les artefacts
    blur( pictureMask, pictureMask, Size(6,6) );
    
    // Threshold pour avoir une image binaire
    threshold(pictureMask,pictureMask,170,255,1);
    
    /*Ouverture*/
    int erosion_size = 15;
    Mat element = getStructuringElement(MORPH_ELLIPSE,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ));
    erode(pictureMask,pictureMask,element);
    dilate(pictureMask,pictureMask, element);
    
    /*Fermeture*/
    erosion_size = 10;
    element = getStructuringElement(MORPH_ELLIPSE,
                                    Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                    Point( erosion_size, erosion_size ));
    
    dilate(pictureMask,pictureMask, element);
    //erode(imgGray,imgGray,element);
    
    // Recherche de contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchie;
    findContours(pictureMask, contours,  hierarchie, RETR_TREE,  CHAIN_APPROX_SIMPLE, Point(0,0));
    cout<<"nb de contours trouves : "<<contours.size()<<endl;
    // Pour chaque contour
    //box a garder
    int maxBoxHeight=0;
    Rect boxMax;
    for(int i=0;i<contours.size();i++){
        
        Rect box = boundingRect(contours[i]);
        cout<<"box trouvee, largeur * hauteur : "<<box.width<<" "<<box.height<<endl;
        if (box.height>maxBoxHeight) {
            boxMax=box;
            
        }
        
    }
    // cropping the picture with biggest box found
    pictureToProcess=pictureToProcess(boxMax);
    
}