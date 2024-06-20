#ifndef THICKNESSANALYSIS_H
#define THICKNESSANALYSIS_H

#include <string>
#include <cstdint>
#include <opencv2/core/core.hpp>

class ThicknessAnalysis
{
public:
    ThicknessAnalysis();

    void StartAnalysis(const std::string& imgPath);
    double GetDistance() { return pointDistance_; };
    void SaveImage(const std::string& fullPath);
    cv::Mat GetResultFrame() { return displayFrame_;};
    bool HasResult() {return !displayFrame_.empty();};

private:
    void OnAnalyseThicknessMouseClickedOpenCV(int evt, int x, int y);
    static void OnAnalyseThicknessMouseClickedOpenCV(int evt, int x, int y, int flags, void* param);

private:
    cv::Mat originalFrame_{}, displayFrame_{};
    std::pair<int,int> firstPoint_{};
    std::uint8_t pointCounter_{0};
    double pointDistance_{0};
};

#endif // THICKNESSANALYSIS_H
