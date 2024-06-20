#include "thicknessanalysis.h"

#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>

ThicknessAnalysis::ThicknessAnalysis() {}

void ThicknessAnalysis::StartAnalysis(const std::string& imgPath)
{
    // reset needed variables
    pointCounter_ = 0;
    pointDistance_ = 0;

    // opencv implementation
    cv::namedWindow("Thickness Analysis");
    originalFrame_ = cv::imread(imgPath);
    cv::setMouseCallback("Thickness Analysis",&ThicknessAnalysis::OnAnalyseThicknessMouseClickedOpenCV, this);

    cv::imshow("Thickness Analysis", originalFrame_);
    cv::waitKey();
    std::cout << "closed" << std::endl;
    cv::destroyWindow("Thickness Analysis");
}

void ThicknessAnalysis::OnAnalyseThicknessMouseClickedOpenCV(int event, int x, int y, int, void* userdata)
{
    ThicknessAnalysis* settings = reinterpret_cast<ThicknessAnalysis*>(userdata);
    settings->OnAnalyseThicknessMouseClickedOpenCV(event, x, y);
}

void ThicknessAnalysis::OnAnalyseThicknessMouseClickedOpenCV(int evt, int x, int y) {
    if(evt == cv::EVENT_LBUTTONDOWN)
    {
        if(pointCounter_ >= 2)
        {
            pointCounter_ = 0;
        }

        if(pointCounter_ == 0)
        {
            // reload image and save first point
            displayFrame_ = originalFrame_.clone();
            firstPoint_ = std::make_pair(x,y);

            cv::circle(displayFrame_,
                       cv::Point(firstPoint_.first,
                                 firstPoint_.second),
                       4, CV_RGB(255,0,0), -1);
        }
        else
        {
            // second point available
            // draw circle
            cv::circle(displayFrame_,
                       cv::Point(x, y),
                       4, CV_RGB(255,0,0), -1);

            // draw line from this to the previous point
            cv::line(displayFrame_,
                     cv::Point(firstPoint_.first,
                               firstPoint_.second),
                     cv::Point(x, y),
                     CV_RGB(255,0,0), 2);

            // calculate distance
            pointDistance_ = std::sqrt(
                std::pow(firstPoint_.first - x, 2)
                +
                std::pow(firstPoint_.second - y, 2)
                );

            cv::putText(displayFrame_, //target image
                        "Dist: " + std::to_string(pointDistance_), //text
                        cv::Point(10, displayFrame_.rows / 2), //top-left position
                        cv::FONT_HERSHEY_DUPLEX,
                        1.0,
                        CV_RGB(255, 0, 0), //font color
                        2);
        }

        pointCounter_++;
        cv::imshow("Thickness Analysis", displayFrame_);
    }
}

void ThicknessAnalysis::SaveImage(const std::string& fullPath)
{
    if(!displayFrame_.empty())
    {
        cv::imwrite(fullPath, displayFrame_);
    }
}
