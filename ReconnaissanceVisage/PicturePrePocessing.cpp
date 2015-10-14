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
    
    resize(pictureToProcess, pictureToProcess, Size(600,pictureToProcess.rows*600/pictureToProcess.cols));
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
    threshold(pictureMask,pictureMask,220,255,1);
    
    /*Ouverture*/
    int erosion_size = 15;
    Mat element = getStructuringElement(MORPH_ELLIPSE,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ));
    erode(pictureMask,pictureMask,element);
    dilate(pictureMask,pictureMask, element);
    
    /*Fermeture*/
    int dilatationion_size = 5;
    element = getStructuringElement(MORPH_ELLIPSE,
                                    Size( 2*dilatationion_size + 1, 2*dilatationion_size+1 ),
                                    Point( dilatationion_size, dilatationion_size ));
    
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
    rectangle(pictureMask, boxMax, Scalar(255,255,255),1);
    cout<<boxMax.size()<<endl;

    // cropping the picture with biggest box found
    pictureToProcess=pictureToProcess(boxMax);
    
}

void PicturePreProcessing::openBatchOfPictures(char* directoryPath){
    DIR *directory = NULL;
    struct dirent *file;
    
    vector<int> parameters_jpg;
    parameters_jpg.push_back(CV_IMWRITE_JPEG_QUALITY) ;
    parameters_jpg.push_back(100) ;
    string outPath("output/");
    
    // on récupère le chemin passé en paramètre
    const char* dir = directoryPath;
    string path = dir;
    cout << path << endl;
    // on ouvre le dossier
    directory = opendir(dir);
    if (directory==NULL)
        exit(1);
    // on lit le nom du premier élément du dossier
    file = readdir(directory);
    int i=1;
    do{
        //on test si on a affaire à une image
        // Extraction de l'extension
        string file_name = file->d_name;
        size_t ext_pos = file_name.find_last_of( '.' );
        
        string ext("");
        string jpg("jpg");
        string jpeg("jpeg");
        
        
        if ( ext_pos != string::npos )
        {
            // 3 manières d'extraire l'entension
            ext = file_name.substr( ext_pos+1 );
            //cout << ext <<endl;
            /*string ext2( file_name, ext_pos );
             string ext3;
             ext3.assign( file_name, ext_pos, file_name.size() - ext_pos );*/
        }
        
        if(ext==jpg|ext==jpeg) {
            Mat pictureToProcess = imread(path+file_name);

            this->preProcessPicture(pictureToProcess);
            
            // on enregistre l'image obtenue
            imwrite(path+to_string(i)+".jpg",pictureToProcess,parameters_jpg);
            i++;
        }
        file = readdir(directory);    }
    while (file != NULL);
    closedir(directory);

}
