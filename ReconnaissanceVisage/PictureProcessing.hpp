//
//  PictureProcessing.hpp
//  ReconnaissanceVisage
//
//  Created by Mikael on 12/10/2015.
//  Copyright © 2015 mikael. All rights reserved.
//

#ifndef PictureProcessing_hpp
#define PictureProcessing_hpp

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>

#endif /* PictureProcessing_hpp */

#define MAX_SIZE 1000

class PictureProcessing{
    
public:
    void initPictureProcessing(const char* pathToBase);
    void preProcessPicture(cv::Mat &pictureToProcess);
    void openBatchOfPictures(std::string directoryPath);
    bool addPictureToBase(cv::Mat pictureToProcess, std::string label);
    bool recognizeFace(cv::Mat pictureToProcess,std::string &label);
    void setBaseFile(FILE* baseFile);
    FILE* getBaseFile();
    void setMyPathToBase(std::string path);
    std::string getMyPathToBase();
    void setAnswerForServer(std::string answer);
    std::string getAnswerForServer();
    
private:
    cv::vector<int> parameters_jpg;
    FILE* myBaseFile;
    std::string myPathToBase;
    std::string answerForServer;
    char baseFileSeparator;
    
    void browseDirectory(std::string path);
    void readDirectory(DIR* directory, std::string path);
    bool isJpegPicture(struct dirent *file);
    bool isDirectory(const char* path);
    void loadBaseOfPictures(cv::vector<cv::Mat>& pictures, cv::vector<std::string>& labels);
    std::string extractDirectoryName(std::string path);
    void extractLabelsNums(cv::vector<std::string>labels,cv::vector<int>&labelsNum);
    void findLabel(cv::vector<std::string>labels,cv::vector<int>labelsNum,int predictedLabelNum,std::string &predictedLabel);
};