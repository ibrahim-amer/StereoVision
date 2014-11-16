/* run-zks.cc (Barak Shilo)
 *
 * command line interface for ZKS
 * takes a BMP argument and runs ZK Stereo algorithm on it
 * saves output (a disparity image) as filename.disparity.bmp
 */

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "stereo.h"
#include "cimage.h"
using namespace std;



// value must be a float or a range of floats
// e.g. "12" or "12-15"
void range(char * str, float &low, float &high)
{
    char * next;
    low = strtod(str, &next);
    if (*next != NULL &&
            (strncmp("-", next, 1) == 0))
        high = strtod(next+1, NULL);
    else
        high = low;
    
   cout << low << " " << high << endl;
}


// value must be an int or a range of ints
// e.g. "12" or "12-15"
void range(char * str, int &low, int &high)
{
    float lowf, highf;
    range(str, lowf, highf);
    low = lowf;
    high = highf;
}


// runs zks and writes results to output files
void doZKS(string outFilename, char * refFilename, char * auxFilename,
        CImage * refImage, CImage * auxImage, int minDisparity,
        int maxDisparity, int winRadL0, int winRadRC, int winRadD,
        int numIterations, float maxScaler, int use_sad,
        int highlight_low, int highlight_high)
{   
    CStereo cs;
    stringstream params;

    params << "(" << minDisparity << "," << maxDisparity <<
                "," << winRadL0 << "," << winRadRC << "," << winRadD <<
                "," << numIterations << "," << maxScaler << ",";
    if (use_sad)
        params << "SAD)"; // sums of absolute differences
    else
        params << "NC)"; // normalized correlation

    cout << "\ncomputing disparity map of input files " << refFilename <<
        " and " << auxFilename << ",\n\tusing parameters: " <<
        params.str() << endl;
    if (highlight_low != 0)
        cout << "\tHighlight " << highlight_low << "," << highlight_high << endl;

    if (abs(minDisparity) > abs(maxDisparity))
    {
        cerr << "MinDisparity must be less than MaxDisparity\n";
        exit(-1);
    }
    if (minDisparity > maxDisparity)
        // this is ok b/c abs(minDisparity) is still smaller than abs(maxDisparity)
        // (e.g. making both min and max negative is equivalent to
        // swapping the left and right images)
        // just swap min and max parameters so this call doesn't choke
        cs.FindDisparityMap(refImage, auxImage, maxDisparity, minDisparity,
                winRadL0, winRadRC, winRadD, numIterations, maxScaler, use_sad);
    else
        cs.FindDisparityMap(refImage, auxImage, minDisparity, maxDisparity,
                winRadL0, winRadRC, winRadD, numIterations, maxScaler, use_sad);
    
    cout << "map computed, writing results to output files...\n\n";

    // output results
    
    outFilename = outFilename + "_" + params.str();

    // disparity map result is stored in m_DisplayImage, so convert to a BMP
    BMP * bmp;
    if (highlight_low == 0)
        bmp = cs.m_DisplayImage->toBmp();
    else
    {   // option enabled to highlight a depth range in red, and
        // output a version of the refImage with only the highlighted regions visible
        BMP * isolatedRefBmp = new BMP();
        isolatedRefBmp->ReadFromFile(refFilename);
        bmp = cs.m_DisplayImage->toHighlightedBmp(highlight_low, highlight_high, isolatedRefBmp);
        isolatedRefBmp->WriteToFile((outFilename + ".iso.bmp").c_str());
        delete isolatedRefBmp;
    }

    bool success = bmp->WriteToFile((outFilename + ".dis.bmp").c_str());
    if (! success)
    {
        cerr << "Error: Could not write to " << outFilename << ".dis.bmp\n"; 
        exit(-1);
    }
    delete bmp;

    // output confidence values as an image
    cs.DisplayConfidenceValues();
    bmp = cs.m_DisplayImage->toBmp(); 
    success = bmp->WriteToFile((outFilename + ".conf.bmp").c_str());
    if (! success)
    {
        cerr << "Error: Could not write to " << outFilename << ".conf.bmp\n"; 
        exit(-1);
    }
    delete bmp;

    // output images of slices
	//DisplayL0Slice(int row, int scale);
	//DisplayLnSlice(int row, int scale);

    // text file with the (float) disparity and confidence value
    // at each pixel
    cs.PrintDisparityValues((outFilename + ".dis").c_str());

}


// parses the input file
int main(int argc, char **argv)
{
    // settings from *.zks config file
    char refFilename[80];
    char auxFilename[80];
    int colors; // 1 for B/W, 3 for RGB
    int minDisparity_low, minDisparity_high;
    int maxDisparity_low, maxDisparity_high;
    int winRadL0_low, winRadL0_high;
    int winRadRC_low, winRadRC_high;
    int winRadD_low, winRadD_high;
    int numIterations_low, numIterations_high;
    float maxScaler_low, maxScaler_high;
    int use_sad;
    int minMaxDiff_low, minMaxDiff_high;
    int highlight_low, highlight_high;
    string outFilename;

    if (argc < 2)
    {
        cout << "usage: ./run-zks config-filename.zks\n";
        exit(-1);
    }

    char * configFilename = argv[1];

    ifstream ifs;
    ifs.open(configFilename);
    if (! ifs)
    {
        cerr << "Error: Cannot open " << configFilename << endl;
        exit(-1);
    }

    char toss[80]; // label of the value... these are ignored.
    char value[80]; // the value

    while (ifs >> refFilename)
    {
        ifs >> auxFilename;
        ifs >> colors;

        ifs >> toss >> value;
        range(value, minDisparity_low, minDisparity_high);

        ifs >> toss >> value;
        range(value, maxDisparity_low, maxDisparity_high);

        ifs >> toss >> value;

        if (strcmp("MinMaxDiff", toss) == 0)
        {
            range(value, minMaxDiff_low, minMaxDiff_high);
            ifs >> toss >> value;
        }
        else
        {
            minMaxDiff_low = INT_MIN;
            minMaxDiff_high = INT_MAX;
        }

        if (strcmp("Highlight", toss) == 0)
        {
            range(value, highlight_low, highlight_high);
            ifs >> toss >> value;
        }
        else
        {
            highlight_low = 0;
        }

        range(value, winRadL0_low, winRadL0_high);

        ifs >> toss >> value;
        range(value, winRadRC_low, winRadRC_high);

        ifs >> toss >> value;
        range(value, winRadD_low, winRadD_high);

        ifs >> toss >> value;
        range(value, numIterations_low, numIterations_high);

        ifs >> toss >> value;
        range(value, maxScaler_low, maxScaler_high);

        ifs >> toss >> use_sad;
        if (! use_sad)
        {
            cerr << "Error: Only SAD is supported at this time. " <<
                "Please change setting to \"USE_SAD 1\"\n";
            ifs.close();
            exit(-1);
        }

        ifs >> toss >> toss >> outFilename;

        // done reading one config block, now process it

        CImage * refImage = new CImage(refFilename, colors);
        CImage * auxImage = new CImage(auxFilename, colors);

        for (int minDisparity = minDisparity_low; minDisparity <= minDisparity_high;
                minDisparity++)
            for (int maxDisparity = maxDisparity_low; maxDisparity <= maxDisparity_high;
                    maxDisparity++)
                if ((minMaxDiff_low <= maxDisparity - minDisparity) &&
                        (minMaxDiff_high >= maxDisparity - minDisparity))
                    for (int winRadL0 = winRadL0_low; winRadL0 <= winRadL0_high;
                            winRadL0++)
                        for (int winRadRC = winRadRC_low; winRadRC <= winRadRC_high;
                                winRadRC++)
                            for (int winRadD = winRadD_low; winRadD <= winRadD_high;
                                    winRadD++)
                                for (int numIterations = numIterations_low;
                                        numIterations <= numIterations_high;
                                        numIterations++)
                                    for (float maxScaler = maxScaler_low;
                                            maxScaler <= maxScaler_high;
                                            maxScaler += 0.01)
                                        doZKS(outFilename, refFilename, auxFilename,
                                                refImage, auxImage, minDisparity,
                                                maxDisparity, winRadL0, winRadRC, winRadD,
                                                numIterations, maxScaler, use_sad,
                                                highlight_low, highlight_high);

        delete refImage;
        delete auxImage;

    } // end config file input loop

    ifs.close();

    return 0;
}


