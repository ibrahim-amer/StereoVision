#include "cimage.h"
#include "stereo.h"


// filename is a BMP file
CImage::CImage(const char * filename, int colors)
{
    BMP bmp;
    bmp.ReadFromFile(filename);
    init(bmp.TellWidth(), bmp.TellHeight(), colors);

    // convert pixels to float values
    for (int col = 0; col < m_width; col++)
    {
        for (int row = 0; row < m_height; row++)
        {
            RGBApixel* pixel = bmp(col, row);

            // m_color == 1 means grayscale... so all components should be equal
            // (i.e. Red = Green = Blue)
            if (m_color == 1)
                setValue(row, col, 0, (float) pixel->Red);
            else if (m_color == 3)
            {
                setValue(row, col, 0, (float) pixel->Red);
                setValue(row, col, 1, (float) pixel->Green);
                setValue(row, col, 2, (float) pixel->Blue);
            }
        }
    }

}

CImage::CImage(int width, int height, int depth)
{
    init(width, height, depth);
}

void CImage::init(int width, int height, int depth)
{
    m_width = width;
    m_height = height;
    m_color = depth;

    // allocate entire array with one call (rather than using a 3D array), so that
    // the array is in one chunk of contiguous memory.
    // otherwise, the pointer arithmetic in CStereo's CImage::sumImage() could break
    m_img = new float [m_width * m_height * m_color];

    // zero the array, just in case
    //clearImage();
}

CImage::~CImage()
{
    Delete();
}

void CImage::Delete()
{
    delete [] m_img;
    m_img = NULL;

    m_height = 0;
    m_width = 0;
    m_color = 0;
}

float CImage::getValue(int row, int col, int i)
{
    if (! inBounds(row, col, i, "getValue")) {
            //return 1.0;
            //return NO_INFORMATION;
    }

    return m_img[row*m_width*m_color + col*m_color + i];
}

void CImage::setValue(int row, int col, int i, float value)
{
    inBounds(row, col, i, "setValue");

    m_img[row*m_width*m_color + col*m_color + i] = value;
}

void CImage::addValue(int row, int col, int i, float value)
{
    setValue(row, col, i, getValue(row, col, i) + value);
}

float * CImage::getPointer(int row, int col, int i)
{
    inBounds(row, col, i, "getPointer");

    return (& (m_img[row*m_width*m_color + col*m_color + i]));
}

// check if passed pixel is within the bounds of m_img's dimensions
bool CImage::inBounds(int row, int col, int i, const char * funName)
{
    if (row >= m_height || row < 0)
        cout << "ERROR: " << funName << "(" << row << ", " << col << ", " << i <<
            ") : " << row << " is an invalid row\n";
    else if (col >= m_width || col < 0)
        cout << "ERROR: " << funName << "(" << row << ", " << col << ", " << i <<
            ") : " << col << " is an invalid col\n";
    else if (i >= m_color || i < 0)
        cout << "ERROR: " << funName << "(" << row << ", " << col << ", " << i <<
            ") : " << i << " is an invalid depth\n";
    else
        return true;

    return false;
}

void CImage::clearImage()
{
    for (int i = 0; i < (m_width*m_height*m_color); i++)
        m_img[i] = (float) 0;
}

// creates a BMP from a CImage
// assumes all float values have already been scaled to [0.0, 255.0]
BMP * CImage::toBmp()
{
    BMP * bmp = new BMP();
    bmp->SetSize(m_width, m_height);

    // for smaller output file size
    bmp->SetBitDepth(8);
    // 8-bit bitmaps need a color table
    CreateGrayscaleColorTable(*bmp);

    for (int col = 0; col < m_width; col++)
    {
        for (int row = 0; row < m_height; row++)
        {
            // Output is grayscale, so R, G, and B components are equal.
            // ScaleAndDisplayDisparityValues() et. al. in stereo.cpp have already
            //  scaled all float pixel values to [0, 255].
            ebmpBYTE byteVal = (ebmpBYTE) getValue(row, col, 0);
            (* bmp)(col, row)->Red = byteVal;
            (* bmp)(col, row)->Green = byteVal;
            (* bmp)(col, row)->Blue = byteVal;
        }
    }

    
    return bmp;
}

// same as above, but highlights a range of depths in red,
// and, colors all depths outside that range black in refImage
// highlight_low and high are values in [1,100]
BMP * CImage::toHighlightedBmp(int highlight_low, int highlight_high, BMP * refBmp)
{
    // scale range to be highlighted to [0,255]
    highlight_low = ((float) highlight_low) * 255.0/100.0 - 2.55;
    highlight_high = ((float) highlight_high) * 255.0/100.0;

    BMP * bmp = new BMP();
    bmp->SetSize(m_width, m_height);

    // for smaller output file size
    bmp->SetBitDepth(8);
    // 8-bit bitmaps need a color table
    CreateGrayscaleColorTable(*bmp);

    // add red to the color table
    RGBApixel highlightColor;
    highlightColor.Red = 255;
    highlightColor.Green = 0;
    highlightColor.Blue = 0;
    highlightColor.Alpha = 0;
    bmp->SetColor(highlight_low, highlightColor);

    // copy pixels to bmp
    for (int col = 0; col < m_width; col++)
    {
        for (int row = 0; row < m_height; row++)
        {
            float pixel = getValue(row, col, 0);
            if (highlight_low <= pixel && pixel <= highlight_high)
            {
                (* bmp)(col, row)->Red = 255;
                (* bmp)(col, row)->Green = 0;
                (* bmp)(col, row)->Blue = 0;
            }
            else
            {
                // Output is grayscale, so R, G, and B components are equal.
                // ScaleAndDisplayDisparityValues() et. al. in stereo.cpp have already
                //  scaled all float pixel values to [0, 255].
                ebmpBYTE byteVal = (ebmpBYTE) pixel;
                (* bmp)(col, row)->Red = byteVal;
                (* bmp)(col, row)->Green = byteVal;
                (* bmp)(col, row)->Blue = byteVal;

                // color non-highlighted areas black in refBmp
                (* refBmp)(col, row)->Red = 0;
                (* refBmp)(col, row)->Green = 0;
                (* refBmp)(col, row)->Blue = 0;
            }
        }
    }

    
    return bmp;
}


