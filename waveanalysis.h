#ifndef WAVEANALYSIS_H
#define WAVEANALYSIS_H

#include <vector>
#include <string>
#include <opencv2/core/core.hpp>

class WaveAnalysis
{
public:
    WaveAnalysis();
    ~WaveAnalysis() = default;

    void SetImage(const std::string& imagePath);

    void StartAnalysis(int threshold);
    cv::Mat PreviewThreshold(int threshold);
    double GetCoefficient() { return coefficient_;};
    void SaveImage(const std::string& fullPath);
    cv::Mat GetResultFrame() { return resultsFrame_;};
    bool HasResult() {return !resultsFrame_.empty();};

private:
    void OnAnalyseWavinessMouseClickedOpenCV(int evt, int x, int y);
    static void OnAnalyseWavinessMouseClickedOpenCV(int evt, int x, int y, int flags, void* param);

private:
    cv::Mat imageFrame_{}, resultsFrame_{};
    std::vector<std::vector<cv::Point>> waveContours_{};
    bool analysisDone_{false};
    double coefficient_{0};
    int threshold_{80};

};

#endif // WAVEANALYSIS_H
