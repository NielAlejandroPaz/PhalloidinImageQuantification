#include "phalloidinimagequantification.h"
#include "ui_phalloidinimagequantification.h"

#include <QFileDialog>
#include <filesystem>
//#include <iostream>

PhalloidinImageQuantification::PhalloidinImageQuantification(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PhalloidinImageQuantification)
{
    ui->setupUi(this);

    // Connect buttons
    connect(ui->BrowsePushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::BrowseButtonClicked);
    connect(ui->AnalyseThicknessPushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::AnalyseThicknessClicked);
    connect(ui->AnalyseWavesPushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::AnalyseWavinessClicked);
    connect(ui->ClearPushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::ClearButtonClicked);
    connect(ui->WaveSaveImagePushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::SaveWaveAnalysisButtonClicked);
    connect(ui->ThicknessSaveImagePushButton, &QPushButton::clicked, this, &PhalloidinImageQuantification::SaveThicknessAnalysisButtonClicked);

    // Connect lineEdits
    connect(ui->BrowseLineEdit, &QLineEdit::returnPressed, this, &PhalloidinImageQuantification::BrowseLineEditChanged);
    connect(ui->FilteringLineEdit, &QLineEdit::returnPressed, this, &PhalloidinImageQuantification::LineEditFilterValueChanged);

    // Connect Slider
    connect(ui->FilteringHorizontalSlider, &QSlider::valueChanged, this, &PhalloidinImageQuantification::SliderFilterValueChanged);

    ui->FilteringLineEdit->setText(
        QString::number(
            ui->FilteringHorizontalSlider->value()
            )
        );
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
    if(imagePathValid_)
    {
        ui->DistanceLineEdit->clear();

        // call analyse thickness
        thicknessAnalysis_.StartAnalysis(currentImagePath_.toStdString());
        double result = thicknessAnalysis_.GetDistance();

        // post last dist value to GUI
        QString distStr(std::to_string(result).c_str());
        ui->DistanceLineEdit->setText(distStr);

        // get image and display it in label
        ui->ThicknessAnalysisResultLabel->setPixmap(
            QPixmap::fromImage(
                CvMat2QImage(
                    thicknessAnalysis_.GetResultFrame())
                )
            );
    }
    else
    {
        ui->statusbar->showMessage("Cannot analyse. Image is not valid!", 5000);
    }
}

void PhalloidinImageQuantification::AnalyseWavinessClicked()
{
    if(imagePathValid_)
    {
        // call analyse wave
        waveAnalysis_.StartAnalysis(ui->FilteringLineEdit->text().toInt());
        double result = waveAnalysis_.GetCoefficient();

        // post last coefficient value to GUI
        QString distStr(std::to_string(result).c_str());
        ui->CoefficientLineEdit->setText(distStr);

        // get image and display it in label
        ui->WaveAnalysisResultLabel->setPixmap(
            QPixmap::fromImage(
                CvMat2QImage(
                    waveAnalysis_.GetResultFrame())
                )
            );
    }
    else
    {
        ui->statusbar->showMessage("Cannot analyse. Image is not valid!", 5000);
    }
}

void PhalloidinImageQuantification::ClearButtonClicked()
{
    ui->BrowseLineEdit->clear();
    imagePathValid_ = false;

    //clear images
    ui->WaveAnalysisResultLabel->clear();
    ui->WaveAnalysisResultLabel->setText("No Image Available");

    ui->ThicknessAnalysisResultLabel->clear();
    ui->ThicknessAnalysisResultLabel->setText("No Image Available");
}

void PhalloidinImageQuantification::SaveWaveAnalysisButtonClicked()
{
    if(waveAnalysis_.HasResult())
    {
        std::string savePath;
        GetSaveImagePath(savePath);
        if (!savePath.empty())
        {
            waveAnalysis_.SaveImage(savePath);
        }
    }
    else
    {
        ui->statusbar->showMessage("Cannot save image. No result available!", 3000);
    }
}

void PhalloidinImageQuantification::SaveThicknessAnalysisButtonClicked()
{
    if(thicknessAnalysis_.HasResult())
    {
        std::string savePath;
        GetSaveImagePath(savePath);
        if (!savePath.empty())
        {
            thicknessAnalysis_.SaveImage(savePath);
        }
    }
    else
    {
        ui->statusbar->showMessage("Cannot save image. No result available!", 3000);
    }
}

void PhalloidinImageQuantification::GetSaveImagePath(std::string& outPath)
{
    if(imagePathValid_)
    {
        //ask for path
        QString savePath = QFileDialog::getSaveFileName(this, tr("Save Image"),
                                                        "",
                                                        tr("Images (*.png)"));
        if (!savePath.isEmpty())
        {
            std::filesystem::path newPath(savePath.toStdString());
            newPath.replace_extension(".png");
            outPath = newPath.string();
        }
    }
    else
    {
        ui->statusbar->showMessage("Cannot save image. Image is not valid!", 3000);
    }
}

void PhalloidinImageQuantification::LineEditFilterValueChanged()
{
    bool isInt = false;
    int value = ui->FilteringLineEdit->text().toInt(&isInt);
    if(isInt)
    {
        SliderFilterValueChanged(value);
    }
    else
    {
         ui->statusbar->showMessage("Value must be integer", 3000);
    }
}

void PhalloidinImageQuantification::SliderFilterValueChanged(int value)
{
    if(value < 0 || value > 255)
    {
        ui->statusbar->showMessage("Filtering values must be between 0 and 255", 3000);
    }
    else
    {
        if(imagePathValid_)
        {
            ui->FilteringLineEdit->setText(std::to_string(value).c_str());

            // get thresholded image and display
            ui->WaveAnalysisResultLabel->setPixmap(
                QPixmap::fromImage(
                    CvMat2QImage(
                        waveAnalysis_.PreviewThreshold(value))
                    )
                );
        }
    }
}

void PhalloidinImageQuantification::BrowseLineEditChanged()
{
    LoadImage(ui->BrowseLineEdit->text());
}

void PhalloidinImageQuantification::LoadImage(const QString& inputPath)
{
    // Validate
    imagePathValid_ = false;
    ::std::string inputString(inputPath.toStdString());
    if (std::filesystem::exists(inputString) && std::filesystem::is_regular_file(inputString))
    {
        std::filesystem::path input(inputString);
        auto extension = input.extension();
        if (extension == ".jpg" || extension == ".png" || extension == ".tiff" )
        {
            imagePathValid_ = true;
        }
    }

    if (imagePathValid_)
    {
        // save path
        currentImagePath_ = inputPath;

        // Load it in image pixmap
        QPixmap img(currentImagePath_);
        ui->ThicknessAnalysisResultLabel->setPixmap(img);

        // load image as preview already
        waveAnalysis_.SetImage(currentImagePath_.toStdString());
        bool isInt = false;
        int th = ui->FilteringLineEdit->text().toInt(&isInt);
        if(isInt)
        {

            ui->WaveAnalysisResultLabel->setPixmap(
                QPixmap::fromImage(
                    CvMat2QImage(
                        waveAnalysis_.PreviewThreshold(th)
                        )
                    )
                );
        }
        else
        {
            ui->WaveAnalysisResultLabel->setPixmap(img);
        }

        ui->statusbar->showMessage("Image Loaded", 3000);
    }
    else
    {
        ui->statusbar->showMessage("Selected path is not a valid image!", 5000);
    }

}

QImage PhalloidinImageQuantification::CvMat2QImage(const cv::Mat& mat)
{
    // 8-bits unsigned, NO. OF CHANNELS=1
    switch(mat.type())
    {
        case CV_8UC3:
        {
            // Create QImage with same dimensions as input Mat
            //std::cout << "CV_8UC3" << std::endl;
            QImage image(mat.data,
                       mat.cols,
                       mat.rows,
                       static_cast<int>(mat.step),
                       QImage::Format_RGB888
                       );
            return image.rgbSwapped();
        }
        case CV_8UC4:
        {
            //std::cout << "CV_8UC4" << std::endl;
            QImage image(mat.data,
                         mat.cols,
                         mat.rows,
                         static_cast<int>(mat.step),
                         QImage::Format_ARGB32
                         );
            return image;
        }
        case CV_8UC1:
        {
            //std::cout << "CV_8UC1" << std::endl;
            QImage image(mat.data,
                         mat.cols,
                         mat.rows,
                         static_cast<int>(mat.step),
                         QImage::Format_Grayscale8
                         );
            return image;
        }
        default:
        {
            //std::cout << "UNKNOWN" << std::endl;
            return QImage();
        }
    }
}


