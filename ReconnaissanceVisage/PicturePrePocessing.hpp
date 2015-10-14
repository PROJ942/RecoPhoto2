//
//  PicturePrePocessing.hpp
//  ReconnaissanceVisage
//
//  Created by Mikael on 12/10/2015.
//  Copyright © 2015 mikael. All rights reserved.
//

#ifndef PicturePrePocessing_hpp
#define PicturePrePocessing_hpp

#include <stdio.h>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#endif /* PicturePrePocessing_hpp */

class PicturePreProcessing{
    
public:
    void preProcessPicture(cv::Mat &pictureToProcess);
    void openBatchOfPictures(char* directoryPath);
    
};