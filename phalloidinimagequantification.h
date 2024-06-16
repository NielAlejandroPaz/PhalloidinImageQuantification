#ifndef PHALLOIDINIMAGEQUANTIFICATION_H
#define PHALLOIDINIMAGEQUANTIFICATION_H

#include "waveanalysis.h"
#include "thicknessanalysis.h"
#include <opencv2/core/core.hpp>

#include <QMainWindow>


QT_BEGIN_NAMESPACE
namespace Ui {
class PhalloidinImageQuantification;
}
QT_END_NAMESPACE

class PhalloidinImageQuantification : public QMainWindow
{
    Q_OBJECT

public:
    PhalloidinImageQuantification(QWidget *parent = nullptr);
    ~PhalloidinImageQuantification();

private:
    void LoadImage(const QString& inputPath);
    static QImage CvMat2QImage(const cv::Mat& mat);
    void GetSaveImagePath(std::string& outPath);

private Q_SLOTS:
    void BrowseButtonClicked();
    void AnalyseThicknessClicked();
    void AnalyseWavinessClicked();
    void ClearButtonClicked();
    void SaveWaveAnalysisButtonClicked();
    void SaveThicknessAnalysisButtonClicked();

    void SliderFilterValueChanged(int value);
    void LineEditFilterValueChanged();

    void BrowseLineEditChanged();

private:
    Ui::PhalloidinImageQuantification *ui;
    QString currentImagePath_{};
    bool imagePathValid_{false};

    WaveAnalysis waveAnalysis_{};
    ThicknessAnalysis thicknessAnalysis_{};

};
#endif // PHALLOIDINIMAGEQUANTIFICATION_H
