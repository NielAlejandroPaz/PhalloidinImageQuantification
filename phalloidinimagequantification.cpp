#include "phalloidinimagequantification.h"
#include "ui_phalloidinimagequantification.h"

#include <opencv2/imgcodecs/imgcodecs.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <QFileDialog>
#include <cmath>
#include <filesystem>

PhalloidinImageQuantification::PhalloidinImageQuantification(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PhalloidinImageQuantification)
{
    ui->setupUi(this);

    // class variables init
    clickedPoints_.reserve(2);

    // Connect buttons
    connect(ui->BrowsePushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::BrowseButtonClicked);
    connect(ui->AnalyseThicknessPushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::AnalyseThicknessClicked);
    connect(ui->AnalyseWavesPushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::AnalyseWavinessClicked);
    connect(ui->ClearPushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::ClearButtonClicked);

    // Connect LineEdit
    connect(ui->BrowseLineEdit, &QLineEdit::returnPressed, this, &PhalloidinImageQuantification::BrowseLineEditChanged);

}

PhalloidinImageQuantification::~PhalloidinImageQuantification()
{
    delete ui;
}

void PhalloidinImageQuantification::BrowseButtonClicked()
{
    QString inputImage = QFileDialog::getOpenFileName(this, tr("Open Image"),
                                                    "/home",
                                                    tr("Images (*.png *.tiff *.jpg)"));
    LoadImage(inputImage);
    ui->BrowseLineEdit->setText(inputImage);
}

void PhalloidinImageQuantification::AnalyseThicknessClicked()
{
    if(imageValid_)
    {
        // reset needed variables
        clickedPointsCounter_ = 0;
        pointDistance_ = 0;
        ui->DistanceLineEdit->clear();
        // opencv implementation
        cv::namedWindow("Output Window");
        frame_ = cv::imread(currentImage_.toStdString());
        cv::setMouseCallback("Output Window",&PhalloidinImageQuantification::OnMouseClickedOpenCV, this);

        cv::putText(frame_, //target image
                    "Use mouse to select points. Click <Space> to quit", //text
                    cv::Point(10, 25), //top-left position
                    cv::FONT_HERSHEY_DUPLEX,
                    1.0,
                    CV_RGB(255, 0, 0), //font color
                    2);

        cv::imshow("Output Window", frame_);
        cv::waitKey(); //blocking until window is closed

        cv::destroyAllWindows();
        QString distStr(std::to_string(pointDistance_).c_str());
        ui->DistanceLineEdit->setText(distStr);
    }
    else
    {
        ui->statusbar->showMessage("Cannot analyse. Image is not valid!", 5000);
    }
}

void PhalloidinImageQuantification::AnalyseWavinessClicked()
{

}

void PhalloidinImageQuantification::ClearButtonClicked()
{
    ui->BrowseLineEdit->clear();
    imageValid_ = false;

    // clear results

    //clear image
    ui->ImageLabel->clear();
    ui->ImageLabel->setText("No Image Selected");
}

void PhalloidinImageQuantification::BrowseLineEditChanged()
{
    LoadImage(ui->BrowseLineEdit->text());
}

void PhalloidinImageQuantification::LoadImage(const QString& inputPath)
{
    // Validate
    imageValid_ = false;
    ::std::string inputString(inputPath.toStdString());
    if (std::filesystem::exists(inputString) && std::filesystem::is_regular_file(inputString))
    {
        std::filesystem::path input(inputString);
        auto extension = input.extension();
        if (extension == ".jpg" || extension == ".png" || extension == ".tiff" )
        {
            imageValid_ = true;
        }
    }

    if (imageValid_)
    {
        // save path
        currentImage_ = inputPath;

        // Load it in image pixmap
        QPixmap img(currentImage_);
        ui->ImageLabel->setPixmap(img);
    }
    else
    {
        ui->statusbar->showMessage("Selected path is not a valid image!", 5000);
    }

}


void PhalloidinImageQuantification::OnMouseClickedOpenCV(int event, int x, int y, int, void* userdata)
{
    PhalloidinImageQuantification* settings = reinterpret_cast<PhalloidinImageQuantification*>(userdata);
    settings->OnMouseClickedOpenCV(event, x, y);
}


void PhalloidinImageQuantification::OnMouseClickedOpenCV(int evt, int x, int y) {
    if(evt == cv::EVENT_LBUTTONDOWN && clickedPointsCounter_ < 2)
    {
        clickedPoints_[clickedPointsCounter_] = std::make_pair(x, y);
        if(clickedPointsCounter_ == 1)
        {
            // we have two points.
            // draw circle
            cv::circle(frame_,
                       cv::Point(clickedPoints_[1].first,
                                 clickedPoints_[1].second),
                       4, CV_RGB(255,0,0), -1);
            // draw line from this to the previous point
            cv::line(frame_,
                     cv::Point(clickedPoints_[0].first,
                               clickedPoints_[0].second),
                     cv::Point(clickedPoints_[1].first,
                               clickedPoints_[1].second),
                     CV_RGB(255,0,0), 2);

            // calculate distance
            pointDistance_ = std::sqrt(
                std::pow(clickedPoints_[0].first - clickedPoints_[1].first, 2)
                +
                std::pow(clickedPoints_[0].second - clickedPoints_[1].second, 2)
                );

            cv::putText(frame_, //target image
                        "Dist: " + std::to_string(pointDistance_), //text
                        cv::Point(10, frame_.rows / 2), //top-left position
                        cv::FONT_HERSHEY_DUPLEX,
                        1.0,
                        CV_RGB(255, 0, 0), //font color
                        2);
        }
        else
        {
            cv::circle(frame_,
                       cv::Point(clickedPoints_[0].first,
                                 clickedPoints_[0].second),
                       4, CV_RGB(255,0,0), -1);
        }

        clickedPointsCounter_++;
        cv::imshow("Output Window", frame_);
    }
}

