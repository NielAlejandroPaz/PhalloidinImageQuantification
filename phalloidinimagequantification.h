#ifndef PHALLOIDINIMAGEQUANTIFICATION_H
#define PHALLOIDINIMAGEQUANTIFICATION_H

#include <QMainWindow>
#include <string>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>

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
    Ui::PhalloidinImageQuantification *ui;
    void AnalyseWaviness(const ::std::string& imgPath);
    void OnMouseClickedOpenCV(int evt, int x, int y);
    static void OnMouseClickedOpenCV(int evt, int x, int y, int flags, void* param);

private Q_SLOTS:
    void BrowseButtonClicked();
    void AnalyseThicknessClicked();
    void AnalyseWavinessClicked();
    void ClearButtonClicked();

    void BrowseLineEditChanged();
    void LoadImage(const QString& inputPath);

private:
    QString currentImage_{};
    bool imageValid_{false};
    std::vector<std::pair<int, int>> clickedPoints_{};
    std::uint8_t clickedPointsCounter_{0};
    cv::Mat frame_{};
    double pointDistance_{0};
};
#endif // PHALLOIDINIMAGEQUANTIFICATION_H
