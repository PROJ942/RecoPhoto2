//
//  PicturePocessing.hpp
//  ReconnaissanceVisage
//
//  Created by Mikael on 12/10/2015.
//  Copyright © 2015 mikael. All rights reserved.
//

#ifndef PicturePocessing_hpp
#define PicturePocessing_hpp

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <vector>

#endif /* PicturePocessing_hpp */

class PictureProcessing{
    
public:
    void initPictureProcessingWithJpegParameters();
    void preProcessPicture(cv::Mat &pictureToProcess);
    void openBatchOfPictures(std::string directoryPath);
    bool addPictureToBase(FILE* baseFile,cv::Mat pictureToProcess);
    void setBaseFile(FILE* baseFile);
    FILE* getBaseFile();
    
private:
    cv::vector<int> parameters_jpg;
    FILE* myBaseFile;
    
    void browseDirectory(std::string path);
    void readDirectory(DIR* directory, std::string path);
    bool isJpegPicture(struct dirent *file);
    bool isDirectory(const char* path);
};