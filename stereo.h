#ifndef STEREO_H
#define STEREO_H
//  ****************************************************************************************************
//
//  For non-commercial use only.  Do not distribute without permission from Carnegie Mellon University.  
//
//  Copyright (c) 2002, Carnegie Mellon University     All rights reserved.   
//
//  ****************************************************************************************************

/*******************************************************************************************************

Header file for stereo.cpp

The routines for reading in images and basic image manipulation
are not included, since these would be specific to each individual
project.  Translation to other image libraries should not be 
difficult.

Any questions should be sent to clz@cs.cmu.edu

Larry Zitnick

********************************************************************************************************/


// changing these settings to match setting in win32 version of zks
//#define BORDERRC 4
#define BORDERRC 8
// don't know what this should be //#define BORDERD 3
#define BORDERD 3
#define NO_INFORMATION -1

#include "cimage.h"


class CStereo
{
private:
    CImage *m_L0Values;                 // Reference (left) image
    CImage *m_LnValues;                 // The right image


    int m_MinD;                         // Minimum disparity
    int m_MaxD;                         // Maximum disparity

public:
    CImage *m_DisparityValues;          
    CImage *m_DisplayImage;
    
    int m_Rows;                         // Number of rows in the images
    int m_Cols;                         // Number of columns in the images
    int m_Depth;                        // Number of disparity levels
    int m_WinD;                         // Radius of window in the disparity direction
    int m_WinRC;                        // Radius of window in the row and column directions
    int m_IterationNum;                 // Number of iterations to compute
    char m_Name0[100], m_Name1[100];    // Names of the images
    char m_OutName[100];                // Name of output file

    bool m_DoneFirstIteration;          // Completed first iteration

    void FindDisparityMap(CImage *Img0, CImage *Img1,  
        int MinDisparity, int MaxDisparity, int WinRadL0, 
        int WinRC, int WinD, int NumIterations, float MaxScaler, bool USE_SAD);
    
    void ComputeL0Values(CImage *Img0, CImage *Img1, int WinRadL0, float MaxScaler, bool USE_SAD);

    void DoIterations(int NumIterations);
    void DoAnIteration();
    void FindFinalDisparityMap();
    void ScaleAndDisplayDisparityValues();
    void DisplayConfidenceValues();
    void DisplayL0Slice(int row, int scale);
    void DisplayLnSlice(int row, int scale);
    void PrintDisparityValues(const char *name);

    void Delete()
    {
        if(m_L0Values != NULL)
        {
            m_L0Values->Delete();
            delete m_L0Values;
        }

        if(m_LnValues != NULL)
        {
            m_LnValues->Delete();
            delete m_LnValues;
        }

        if(m_DisparityValues != NULL)
        {
            m_DisparityValues->Delete();
            delete m_DisparityValues;
        }
        
        // m_DisplayImage should be deleted too
        if (m_DisplayImage != NULL)
        {
            m_DisplayImage->Delete();
            delete m_DisplayImage;
        }

        m_L0Values = NULL;
        m_LnValues = NULL;
        m_DisparityValues = NULL;
        m_DoneFirstIteration = false;
        m_Rows = m_Depth = m_Cols = 0;
        m_IterationNum = -1;

    }

    CStereo()
    {
        m_DoneFirstIteration = false;
        m_IterationNum = -1;
        m_L0Values = NULL;
        m_LnValues = NULL;
        m_DisparityValues = NULL;
        m_Rows = m_Depth = m_Cols = 0;
        m_WinD = 1;
        m_WinRC = 2;
    }

    // added a deconstructor
    ~CStereo()
    {
        Delete();
    }
};

#endif

