#include "ColorTracker.h"

ColorTracker::ColorTracker()
{
    
}

/*void ColorTracker::piGrabImage(raspicam::RaspiCam_Cv* camera)
{
    //Grabbing an image from the camera
    camera->grab();
    camera->retrieve(lastImg);

    //Storing data from the image now to save time in the future
    imgRows = lastImg.rows;
    imgCols = lastImg.cols;

    isConverted = false;
}*/
void ColorTracker::setImage(cv::Mat img)
{
   lastImg.create(img.rows, img.cols, CV_8UC3);
   lastImg.data = img.data;

   imgRows = lastImg.rows;
   imgCols = lastImg.cols;

   isConverted = false;
}

Vec3 ColorTracker::getObjectColor()
{
    Vec3 result;

    //Convert to HSV
    convertToHSV();
    uint8_t* hsvData = hsvImg.data;
    
    Vec3 colorSum;
    
    //Calculating the average color of the object
    int avgSize = 5;
    for(int x = -avgSize; x <= avgSize; x++)
    {
        for(int y = -avgSize; y <= avgSize; y++)
        {
            int pixelX = x + imgRows / 2;
            int pixelY = y + imgRows / 2;
            //std::cout << "processing pixel: " << x << ", " << y << std::endl;
            Vec3 pixel = ImgFunc::getPixel(hsvData, pixelX, pixelY, imgRows, imgCols, 3);

            //Going through all the pixels
            for(int i = 0; i < 3; i++)
            {
                colorSum.val[i] += pixel.val[i];
            }
        }
    }

    int avgSearchAmount = pow(avgSize * 2 + 1, 2);

    for(int i = 0; i < 3; i++)
    {
        result.val[i] = colorSum.val[i] / avgSearchAmount;
    }

    return result;
}
void ColorTracker::generateBinary(Vec3 minThresh, Vec3 maxThresh, bool calcMiddle)
{
    //binaryImg.create(cv::Size(imgRows, imgCols), CV_8UC1);
    binaryImg.create(cv::Size(lastImg.cols, lastImg.rows), CV_8UC1);

    //Converting the image to HSV
    //cv::cvtColor(lastImg, hsvImg, CV_BGR2HSV);
    convertToHSV();

    //Getting the data
    uint8_t* hsvData = hsvImg.data;
    uint8_t* binaryData = binaryImg.data;

    
    Vec2 sumPos; //Stores the sum of the position of found pixels. Used if 
    int pixelsFound = 0;
    
    //Looking for the pixels within the bounds
    for(int i = 0; i < imgRows * imgCols; i++)
    {
        bool inBounds = true;
        for(int n = 0; n < 3; n++)
        {
            //std::cout << "Searching pixel pos: " << i*3+n << std::endl;
            if(hsvData[i * 3 + n] > maxThresh.val[n] || hsvData[i * 3 + n] < minThresh.val[n])
            {
                inBounds = false;
            }
        }

        //Storing the data
        if(inBounds == true)
        {
            binaryData[i] = 255;

            if(calcMiddle == true) //If the avg middlepoint should be calculated
            {
                pixelsFound++;

                Vec2 pixelCoord = ImgFunc::getCoordsFromPixel(i, imgRows);
                sumPos.val[0] += pixelCoord.val[0];
                sumPos.val[1] += pixelCoord.val[1];
            }
        }
    }

    //Calculating the average pixel position
    if(calcMiddle == true)
    {
        middlePos.val[0] = sumPos.val[0] / pixelsFound;
        middlePos.val[1] = sumPos.val[1] / pixelsFound;
    }
}
void ColorTracker::convertToHSV()
{
    //Don't convert if it's alread HSV
    if(isConverted == false)
    {
        cv::cvtColor(lastImg, hsvImg, CV_BGR2HSV);
    }

    isConverted = true;
}

void ColorTracker::saveImage(std::string filename)
{
    cv::imwrite(filename, lastImg); 
}
void ColorTracker::saveBinary(std::string filename)
{
    cv::imwrite(filename, binaryImg);
}

Vec2 ColorTracker::getMiddlePos()
{
    return middlePos;
}