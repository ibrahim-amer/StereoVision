#ifndef CIMAGE_H
#define CIMAGE_H
/* File: "cimage.h"
 * Barak Shilo
 * CImage class -- represents an image that can have multiple layers
 * (this implementation of CImage fills a gap left in the ZKS code -- a CImage class
 *  and associated functions are used in ZKS, but the code is not provided)
 *  Uses the open-source EasyBMP library.
 */

#include <map>
#include <iostream>
#include "EasyBMP.h"

using namespace std;

class CImage {
    public:
        CImage(const char * filename, int colors); // (not referenced in ZKS code)
        CImage(int width, int height, int depth);
        ~CImage();
        void Delete(); // required by ZKS code
                       // this is also called by the deconstructor
        float getValue(int row, int col, int i); // i is the depth coordinate
        void setValue(int row, int col, int i, float value);
        void addValue(int row, int col, int i, float value);
        float * getPointer(int row, int col, int i);
        // sets all pixel values to zero
        void clearImage();

        // these have already been defined in stereo.cpp
        void averageImage(int radius);
        void sumImage(int radius);

        // fields that probably should be private, but aren't
        int m_height;
        int m_width;
        int m_color; // number of color bands, 1 for b/w and 3 for rgb
                     // this is the depth dimension of m_img
                     // (i.e. Red, Green, and Blue values are stored in separate layers)

        // utility functions i've added
        BMP * toBmp(); // creates a BMP from a CImage
        // creates highlighted BMP from a CImage, and colors non-highlighted depths black in refBmp
        BMP * toHighlightedBmp(int highlight_low, int highlight_high, BMP * refBmp);                                         


    private:
        void init(int width, int height, int depth);
        bool inBounds(int row, int col, int i, const char * funName); // error-checking function

        // the float value of each (row, col, i) coordinate is at
        //      m_img[row*m_width*m_color + col*m_color + i]
        // (i is along the depth (m_color) dimension)
        float* m_img;

};


#endif

