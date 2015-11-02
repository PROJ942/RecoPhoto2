//
//  PictureProcessing.cpp
//  ReconnaissanceVisage
//
//  Created by Mikael on 12/10/2015.
//  Copyright © 2015 mikael. All rights reserved.
//

#include "PictureProcessing.hpp"

using namespace cv;
using namespace std;

#pragma mark public

void PictureProcessing::initPictureProcessing(const char* pathToBase){
    
    baseFileSeparator = ';';
    
    parameters_jpg.push_back(CV_IMWRITE_JPEG_QUALITY) ;
    parameters_jpg.push_back(100) ;
    
    myPathToBase = pathToBase;
    myPathToBaseFile=myPathToBase+"/pictureBase.txt";
}

#pragma mark getters&setters
void PictureProcessing::setBaseFile(FILE* baseFile){
    myBaseFile = baseFile;
}
FILE* PictureProcessing::getBaseFile(){
    return myBaseFile;
}
void PictureProcessing::setMyPathToBase(std::string path){
    myPathToBase = path;
}
std::string PictureProcessing::getMyPathToBase(){
    return myPathToBase;
}
void PictureProcessing::setAnswerForServer(std::string answer){
    answerForServer = answer;
}
std::string PictureProcessing::getAnswerForServer(){
    return answerForServer;
}
#pragma mark ---

void PictureProcessing::preProcessPicture(Mat &pictureToProcess){
    
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
            if((ratio1<0.02) || (ratio1>0.45) || (ratio2>0.5) || (ratio2<0.1) || ((int)coul[2]<=((int)coul[0]+25)) || ((int)coul[2]<=((int)coul[1]+15)) || ((int)coul[2]>245)){
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
    vector<vector<Point> > contours;
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

    cvtColor( pictureToProcess, pictureToProcess, CV_BGR2GRAY );
    // cropping the picture with biggest box found
    pictureToProcess=pictureToProcess(boxMax);
    
}

void PictureProcessing::openBatchOfPictures(string directoryPath){
    
    browseDirectory(directoryPath);

}

bool PictureProcessing::addPictureToBase(Mat pictureToProcess, string label){ //A faire !!!!!!!!
    bool pictureAdded;
    int pictureNum;
    string pathNewImg="";

    this->preProcessPicture(pictureToProcess);
    std::ostringstream img;
    
    //check if the person is already in the base
    if (isLabelInTheBase(label)) {
        //find the existing pictures name
        pictureNum = findHighestPictureNumber(label);
        pictureNum++;
    }
    else{
        //create directory
        string cmdSystem = "mkdir "+myPathToBase+"/"+label;
        system(cmdSystem.c_str());
        pictureNum = 1;
    }
    img << pictureNum;
    
    //Definition of the path of this new picture
    pathNewImg =myPathToBase+"/"+label+"/"+img.str()+".jpg";
    
    myBaseFile = fopen(myPathToBaseFile.c_str(), "a");

    //write the processed picture
    imwrite(pathNewImg,pictureToProcess,parameters_jpg);
    
    fprintf(myBaseFile, "%s;%s\n",pathNewImg.c_str(),label.c_str());
    pictureToProcess.release();

    fclose(myBaseFile);
    
    return pictureAdded;
}

bool PictureProcessing::recognizeFace(cv::Mat pictureToProcess,std::string &predictedLabel){
    bool recognized;
    
    //Preprocess the picture to extract the face
    preProcessPicture(pictureToProcess);
    
    // These vectors hold the images and corresponding labels.
    vector<Mat> images;
    vector<string> labels;
    vector<int> labelsNum;
    
    loadBaseOfPictures(images, labels);
    extractLabelsNums(labels, labelsNum);

    // The following lines create an LBPH model for
    // face recognition and train it with the images and
    // labels read from the given CSV file.
    //
    // The LBPHFaceRecognizer uses Extended Local Binary Patterns
    // (it's probably configurable with other operators at a later
    // point), and has the following default values
    //
    //      radius = 1
    //      neighbors = 8
    //      grid_x = 8
    //      grid_y = 8
    //
    // So if you want a LBPH FaceRecognizer using a radius of
    // 2 and 16 neighbors, call the factory method with:
    //
    //      cv::createLBPHFaceRecognizer(2, 16);
    //
    // And if you want a threshold (e.g. 123.0) call it with its default values:
    //
    //      cv::createLBPHFaceRecognizer(1,8,8,8,123.0)
    //
    Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();
    model->train(images, labelsNum);
    // The following line predicts the label of a given
    // test image:
    int predictedLabelNum = model->predict(pictureToProcess);
    //
    // To get the confidence of a prediction call the model with:
    //
    //      int predictedLabel = -1;
    //      double confidence = 0.0;
    //      model->predict(testSample, predictedLabel, confidence);
    //
    double confidence = 0.0;
    int label = -1;
    model->predict(pictureToProcess,label,confidence);
    findLabel(labels, labelsNum, predictedLabelNum, predictedLabel);
    cout << "Recognized people : "<<predictedLabel<<" with confidence : "<<confidence << endl;
    
    return recognized;
}


#pragma mark -------------------------------------------
#pragma mark private
void PictureProcessing::browseDirectory(string path){
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
    
    closedir(currentDirectory);
}

void PictureProcessing::readDirectory(DIR* directory, string path){
    struct dirent* ent = NULL;
    int i=1;
    
    while ((ent = readdir(directory)) != NULL){
        if(isJpegPicture(ent)) {
            myBaseFile = fopen(myPathToBaseFile.c_str(), "a");
            if (myBaseFile==NULL) {
                cout<<"faile to open base file : "<<myPathToBaseFile <<endl;
            }

            Mat pictureToProcess = imread(path+"/"+ent->d_name);
            cout<<"nom de l'image : "<<ent->d_name<<" et taille : "<<pictureToProcess.cols<<" "<<pictureToProcess.rows<<endl;
            this->preProcessPicture(pictureToProcess);
            std::ostringstream img;
            img << i;
            
            string pathNewImg =path+"/"+/*to_string(i)*/img.str()+".jpg";
            // on enregistre l'image obtenue
            imwrite(pathNewImg,pictureToProcess,parameters_jpg);
            fprintf(myBaseFile, "%s;%s\n",pathNewImg.c_str(),extractDirectoryName(path).c_str());
            pictureToProcess.release();
            i++;
            fclose(myBaseFile);
        }
        else if (isDirectory(ent->d_name)){
            string subPath = path+"/"+ent->d_name;
            browseDirectory(subPath);
        }
    }
}

bool PictureProcessing::isJpegPicture(struct dirent *file){
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

bool PictureProcessing::isDirectory(const char* path){
    bool isDir;
    if ((strchr(path, '.'))==NULL) {
        isDir = true;
    }
    else isDir=false;
    return isDir;
}

void PictureProcessing::loadBaseOfPictures(cv::vector<cv::Mat>& pictures, cv::vector<string>& labels){
    string path, classlabel;
    char line[MAX_SIZE]="";
    myBaseFile = fopen(myPathToBaseFile.c_str(), "r");
    if (myBaseFile==NULL) {
      cout<<"fail to open : "<<myPathToBaseFile<<endl;
    }
    while (fgets(line, MAX_SIZE, myBaseFile) != NULL) {
        stringstream liness(line);
        getline(liness, path, baseFileSeparator);
        getline(liness, classlabel);
        if(!path.empty() && !classlabel.empty()) {
            pictures.push_back(imread(path, 0));
            labels.push_back(classlabel);
        }
    }
    fclose(myBaseFile);
    cout<<"number of pictures loaded : "<<pictures.size()<<endl;
}

std::string PictureProcessing::extractDirectoryName(std::string path){
    string directoryName;
    size_t directory_pos = path.find_last_of( '/' );
    
    if ( directory_pos != string::npos )
    {
        directoryName = path.substr( directory_pos+1 );
    }
    return directoryName;
}

void PictureProcessing::extractLabelsNums(cv::vector<std::string>labels,cv::vector<int>&labelsNum){
    int j=0;
    labelsNum.push_back(j);
    for (int i=1; i<labels.size(); i++) {
        if (labels[i]!=labels[i-1]) {
            j++;
        }
        labelsNum.push_back(j);
    }
}

void PictureProcessing::findLabel(cv::vector<std::string>labels,cv::vector<int>labelsNum,int predictedLabelNum,std::string &predictedLabel){
    int i=0;
    while (labelsNum[i]!=predictedLabelNum) {
        i++;
    }
    predictedLabel=labels[i];
}

bool PictureProcessing::isLabelInTheBase(std::string label){
    char entry[MAX_SIZE]="";
    myBaseFile = fopen(myPathToBaseFile.c_str(), "r");
    while (fgets(entry,MAX_SIZE,myBaseFile)!=NULL) {
        string entryString = entry;
        if (entryString.find(label)!=string::npos) {
            fclose(myBaseFile);
            return true;
        }
    }
    fclose(myBaseFile);
    return false;
}

int PictureProcessing::findHighestPictureNumber(std::string label){
    DIR* currentDirectory;
    struct dirent* ent = NULL;
    int highestNum = -1;
    string directoryToBrowse = myPathToBase+"/"+label;
    
    if (!isDirectory(directoryToBrowse.c_str()))
    {
        printf("%s n'est pas un dossier", directoryToBrowse.c_str());
        exit(-1);
    }
    
    currentDirectory = opendir(directoryToBrowse.c_str());
    
    if (currentDirectory == NULL)
    {
        printf("The directory '%s' couldn't be opened\n", directoryToBrowse.c_str());
        exit(-1);
    }
    
    while ((ent = readdir(currentDirectory)) != NULL){
        if(isJpegPicture(ent)) {
            string fileName = ent->d_name;
            size_t dotPosition = fileName.find_last_of('.');
            string fileNameNum = fileName.substr(0,dotPosition);
            int num = stoi(fileNameNum);
            if (num>highestNum) {
                highestNum = num;
            }
        }
    }
    
    closedir(currentDirectory);
    return highestNum;
}
