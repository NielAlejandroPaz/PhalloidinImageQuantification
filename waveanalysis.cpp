#include "waveanalysis.h"

#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
#include <algorithm>
#include <cmath>

#include "spline.h"

WaveAnalysis::WaveAnalysis() {}

void WaveAnalysis::SetImage(const std::string& imagePath)
{
    imageFrame_ = cv::imread(imagePath);
}

void WaveAnalysis::StartAnalysis(int threshold)
{
    if (!imageFrame_.empty())
    {
        // reset
        waveContours_.clear();
        analysisDone_ = false;
        coefficient_ = 0;

        //opencv implementation
        cv::Mat contours = PreviewThreshold(threshold);
        cv::namedWindow("Wave Analysis");
        imshow("Wave Analysis", contours);
        cv::setMouseCallback("Wave Analysis",&WaveAnalysis::OnAnalyseWavinessMouseClickedOpenCV, this);

        cv::waitKey();

        std::cout << "closed\n";
        cv::destroyWindow("Wave Analysis");
    }
}

cv::Mat WaveAnalysis::PreviewThreshold(int threshold)
{
    if (!imageFrame_.empty())
    {
        //opencv implementation
        cv::Mat img_gray;
        cv::cvtColor(imageFrame_, img_gray, cv::COLOR_BGR2GRAY);

        cv::Mat thresh;
        cv::threshold(img_gray, thresh, threshold, 255, cv::THRESH_BINARY);

        // detect the contours on the binary image using cv2.CHAIN_APPROX_NONE
        std::vector<cv::Vec4i> hierarchy;
        findContours(thresh, waveContours_, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_NONE);

        // draw contours on the original image
        cv::Mat contours = imageFrame_.clone();
        drawContours(contours, waveContours_, -1, cv::Scalar(255, 255, 255), 2);
        return contours;
    }
    return cv::Mat();
}

void WaveAnalysis::OnAnalyseWavinessMouseClickedOpenCV(int event, int x, int y, int, void* userdata)
{
    WaveAnalysis* settings = reinterpret_cast<WaveAnalysis*>(userdata);
    settings->OnAnalyseWavinessMouseClickedOpenCV(event, x, y);
}

void WaveAnalysis::OnAnalyseWavinessMouseClickedOpenCV(int evt, int x, int y) {
    if(evt == cv::EVENT_LBUTTONDOWN && !analysisDone_)
    {
        cv::Point mouseCoordinate(x, y);
        int hitId = -1;

        for (size_t i=0; i<waveContours_.size(); i++) {
            if (pointPolygonTest(waveContours_[i], mouseCoordinate, false) > 0) {
                hitId = i;
                break;
            }
        }

        if(hitId > 0)
        {
            // make a copy of the contour of interest for sorting
            std::vector<std::vector<cv::Point>> contourOfInterest;
            contourOfInterest.emplace_back(waveContours_[hitId]);

            // Draw contour before we sort it
            resultsFrame_ = imageFrame_.clone();
            drawContours(resultsFrame_, contourOfInterest, -1, cv::Scalar(255, 255, 255), 2);

            // Sort contour of interest
            std::sort(contourOfInterest[0].begin(),
                      contourOfInterest[0].end(),
                      [](cv::Point a, cv::Point b)
                      {
                          return a.x < b.x;
                      }
                      );

            size_t contourSize = contourOfInterest[0].size();
            int middleIdx = contourSize / 2;
            int firstQuartileIdx = contourSize / 4;
            int thirdQuartileIdx = middleIdx + firstQuartileIdx;

            auto min = contourOfInterest[0][0];
            auto max = contourOfInterest[0][contourSize-1];
            auto middle = contourOfInterest[0][middleIdx];
            auto firstQuartile = contourOfInterest[0][firstQuartileIdx];
            auto thirdQuartile = contourOfInterest[0][thirdQuartileIdx];

            // create spline
            // 5 points will be used
            std::vector<double> x, y;
            x.reserve(5);
            y.reserve(5);

            x.emplace_back(min.x);
            x.emplace_back(firstQuartile.x);
            x.emplace_back(middle.x);
            x.emplace_back(thirdQuartile.x);
            x.emplace_back(max.x);

            y.emplace_back(min.y);
            y.emplace_back(firstQuartile.y);
            y.emplace_back(middle.y);
            y.emplace_back(thirdQuartile.y);
            y.emplace_back(max.y);

            tk::spline s(x,y);

            // create polyline given spline and calculate diff
            int splineSize = (max.x - min.x) + 1;
            std::vector<cv::Point> splineAsPolyLine;
            splineAsPolyLine.reserve(splineSize);


            for(int i = 0; i < splineSize; i++)
            {
                cv::Point newPoint;
                newPoint.x = min.x + i;
                newPoint.y = s(newPoint.x);
                splineAsPolyLine.emplace_back(newPoint);

                // calculate 'waviness' == mean value of the diffrence between the spline and the contour
                coefficient_ += std::abs(cv::pointPolygonTest(contourOfInterest[0], newPoint, true));
            }
            coefficient_ = coefficient_ / splineSize;

            // Draw Results
            cv::putText(resultsFrame_, //target image
                        "Wave Coefficient: " + std::to_string(coefficient_), //text
                        cv::Point(10, resultsFrame_.rows / 2), //top-left position
                        cv::FONT_HERSHEY_DUPLEX,
                        1.0,
                        CV_RGB(255, 0, 0), //font color
                        2);

            // draw spline
            cv::polylines(resultsFrame_,
                          std::vector<std::vector<cv::Point>>(1,splineAsPolyLine),
                          false,
                          cv::Scalar(0, 0, 255),
                          2);

            // draw original spline points
            cv::Scalar splinePointColor(0, 255, 255); // yellow

            cv::circle(resultsFrame_, min, 3, splinePointColor, 3);
            cv::circle(resultsFrame_, firstQuartile, 3, splinePointColor, 3);
            cv::circle(resultsFrame_, middle, 3, splinePointColor, 3);
            cv::circle(resultsFrame_, thirdQuartile, 3, splinePointColor, 3);
            cv::circle(resultsFrame_, max, 3, splinePointColor, 3);

            // display everything
            cv::imshow("Wave Analysis", resultsFrame_);

            // disable further analyses
            analysisDone_ = true;
        }
        else
        {
            std::cout << "no contour found" << std::endl;
        }
    }
}

void WaveAnalysis::SaveImage(const std::string& fullPath)
{
    if(!resultsFrame_.empty())
    {
        cv::imwrite(fullPath, resultsFrame_);
    }
}
