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

#pragma mark public

void PicturePreProcessing::initPicturePreProcessingWithJpegParameters(){
    parameters_jpg.push_back(CV_IMWRITE_JPEG_QUALITY) ;
    parameters_jpg.push_back(100) ;
}

#pragma mark getters&setters
void PicturePreProcessing::setBaseFile(FILE* baseFile){
    myBaseFile = baseFile;
}
FILE* PicturePreProcessing::getBaseFile(){
    return myBaseFile;
}
#pragma mark ---

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
    blur( pictureMask, pictureMask, Size(6,6) ); //tester filtre médian
    
    // Threshold pour avoir une image binaire
    threshold(pictureMask,pictureMask,220,255,1); //seuillage adaptatif ?
    
    /*Ouverture*/
    int erosion_size = 15;
    Mat element = getStructuringElement(MORPH_ELLIPSE,
                                        Size( 2*erosion_size + 1, 2*erosion_size+1 ),
                                        Point( erosion_size, erosion_size ));
    erode(pictureMask,pictureMask,element);
    dilate(pictureMask,pictureMask, element);
    
    /*Fermeture*/
    int dilatation_size = 5;
    element = getStructuringElement(MORPH_ELLIPSE,
                                    Size( 2*dilatation_size + 1, 2*dilatation_size+1 ),
                                    Point( dilatation_size, dilatation_size ));
    
    dilate(pictureMask,pictureMask, element);
    //erode(imgGray,imgGray,element);
    
    // Recherche de contours
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchie;
    findContours(pictureMask, contours,  hierarchie, RETR_TREE,  CHAIN_APPROX_SIMPLE, Point(0,0));
    cout<<"nb de contours trouves : "<<contours.size()<<endl;
    // Pour chaque contour
    //box a garder
    Rect boxMax;
    boxMax.height=0;
    for(int i=0;i<contours.size();i++){
        Rect box = boundingRect(contours[i]);
        if (box.height>boxMax.height) {
            boxMax=box;
        }
    }
    rectangle(pictureMask, boxMax, Scalar(255,255,255),1);
    cout<<"box max trouvee, largeur * hauteur : "<<boxMax.width<<" "<<boxMax.height<<endl;

    // cropping the picture with biggest box found
    pictureToProcess=pictureToProcess(boxMax);
    
}

void PicturePreProcessing::openBatchOfPictures(string directoryPath){
    
    browseDirectory(directoryPath);

}

bool addPictureToBase(FILE* baseFile,cv::Mat pictureToProcess){
    bool pictureAdded;
    
    return pictureAdded;
}

#pragma mark -------------------------------------------
#pragma mark private
void PicturePreProcessing::browseDirectory(string path){
    DIR* currentDirectory;
    const char* currentPath = path.c_str();
    if (!isDirectory(currentPath))
    {
        printf("%s n'est pas un dossier", currentPath);
        exit(-1);
    }
    
    currentDirectory = opendir(currentPath);
    
    if (currentDirectory == NULL)
    {
        printf("The directory '%s' couldn't be opened\n", currentPath);
        exit(-1);
    }
    
    readDirectory(currentDirectory, path);
    
    closedir(currentDirectory); /* Fermeture du répertoire. */
}

void PicturePreProcessing::readDirectory(DIR* directory, string path){
    struct dirent* ent = NULL;
    int i=1;
    while ((ent = readdir(directory)) != NULL){
        if(isJpegPicture(ent)) {
            Mat pictureToProcess = imread(path+"/"+ent->d_name);
            
            this->preProcessPicture(pictureToProcess);
            std::ostringstream img;
            img << i;
            
            string pathNewImg =path+"/"+/*to_string(i)*/img.str()+".jpg";
            // on enregistre l'image obtenue
            imwrite(pathNewImg,pictureToProcess,parameters_jpg);
            pathNewImg +="\n";
            fprintf(myBaseFile, pathNewImg.c_str());
            pictureToProcess.release();
            i++;
        }
        else if (isDirectory(ent->d_name)){
            string subPath = path+"/"+ent->d_name;
            browseDirectory(subPath);
        }
    }
}

bool PicturePreProcessing::isJpegPicture(struct dirent *file){
    bool isJpeg;
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
    
    if((ext==jpg)|| (ext==jpeg)) {
        isJpeg = true;
    }
    else isJpeg=false;
    return isJpeg;
}

bool PicturePreProcessing::isDirectory(const char* path){
    bool isDir;
    if ((strchr(path, '.'))==NULL) {
        isDir = true;
    }
    else isDir=false;
    return isDir;
}