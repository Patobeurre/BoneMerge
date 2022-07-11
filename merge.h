#ifndef MERGE_H
#define MERGE_H

#include "mergeexception.h"

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <QString>
#include <QList>

using namespace cv;


namespace MergeUtils {

    static const std::string EXTENSION_NORM = "_norm";
    static const std::string EXTENSION_STAND = "_stand";
    static const std::string EXTENSION_AVERAGE = "_mean";
    static const std::string EXTENSION_MEDIAN = "_median";
    static const std::string EXTENSION_DEVIATION = "_dev";

    enum class EMapType
    {
        EXTERNAL_RADIUS = 0,
        CORTICAL_THICK  = 1,
        CURVATURE       = 2,
        MOMENT_AREA     = 3,
        MODULUS         = 4,
    };

    static const std::vector<std::string> mapNameAssoc = {"ExternalRadius",
                                                          "CorticalThickness",
                                                          "ExternalCurvature",
                                                          "Moments",
                                                          "Modulus"};
    static const std::vector<std::string> mapFileNameAssoc = {"mapExtRadius",
                                                              "mapCortical",
                                                              "mapCurv",
                                                              "mapMoment",
                                                              "mapModulus"};

    static std::string getMapName(const EMapType type)
    {
        int id = static_cast<int>(type);
        return mapNameAssoc[id];
    }
    static std::string getMapFileName(const EMapType type)
    {
        int id = static_cast<int>(type);
        return mapFileNameAssoc[id];
    }
    static int getIdFromType(const EMapType type)
    {
        return static_cast<int>(type);
    }
    static EMapType getTypeFromId(const int id)
    {
        return static_cast<EMapType>(id);
    }

    struct SMergeOptions {
        QList<QString>* dirList;
        QString targetDir;
        QList<EMapType>* mapTypes;
        bool bAverage;
        bool bDeviation;
        bool bMedian;
        bool bStandardize;
        bool bBlur;
        bool bCaption;
        int threshold;
    };
}
using namespace MergeUtils;


class Merge
{

public:
    void setbMoments(bool b);
    void setbModulus(bool b);
    void setbCurv(bool b);
    void setbDistance(bool b);
    void setbCortical(bool b);
    void setbStandard(bool b);
    void setbSection(bool b);

    bool getbMoments();
    bool getbModulus();
    bool getbCruv();
    bool getbDistance();
    bool getbCortical();
    bool getbSection();
    QString getTarget();
    QString getGroup();
    QString getMinMaxTarget();

    void setDirectories(QList<QString>* list);
    int launchMerge(int id,bool _bStandard=false);
    int launchDeviation(int id,bool _bStandard=false);
    int launchMedian(int id, bool _bStandard=false);

    QList<float> launchExtraction(QString filename,int section);
    void getMinMax(int id);
    void launchNormalized();
    void displayErrorBox(QString errorTitle, QString errorMessage);
    void setTargetDir(QString dir);
    void setGroupDir(QString dir);
    void setMinMaxDir(QString dir);
    void mapToColor(Mat mapRaw, Mat gradient , std::string name,bool bnormalize=true);
    void mapToColor2(Mat mapRaw, Mat gradient , std::string name,Mat* mask,bool bnormalize=true);
    void writeHowMany();
    void writeHowManyDetail(int* tab);
    void customNormalize(Mat src, Mat* dst, float newMin, float newMax, float min, float max);
    float getMedian(std::vector<float> v);
    void setPercentage(double p);
    Mat *GetMapFromTxt(QString filename,float mass=1.0f);
    QList<Mat*>* GetMapMaskFromTxt(QString filename, float mass=1.0f);

    float* getMinMax(Mat* m,Mat* mask );

    float getStand(std::string filename,int id);
    QString chooseFile(int id);
    QString chooseFunc(int id);

    QString chooseMap(int id);

    void writeList();


    /**
     * REFACTOR CODE
     */
    Merge();

    void loadGradientImages() throw(MergeException);

    Mat mapToColorGradient(const Mat* mapRaw, const Mat* gradient);
    Mat mapToColorGradientWithMask(const Mat* mapRaw, const Mat* gradient, const Mat* mask);

    int launch(SMergeOptions* options) throw(MergeException);

    void exportOptions(SMergeOptions* options) throw(MergeException);
    SMergeOptions* importOptions(const QString filepath) throw(MergeException);

    QList<QString> getSimilarFilenames(const QList<QString>* directories);
    void normalizeGroup(const QList<QString>* directories, const QString saveDir) throw(MergeException);

    void launchRatio(const QList<QString>* dirList, const QString targetDir, const QList<EMapType>* mapTypes, const bool section = false, const float secVal = 0.0);
    QList<QList<float>> computeRatio(const QList<QString>* dirList, const EMapType type, const bool section = false, const float secVal = 0.0);


private:
    bool bMoments ;
    bool bModulus ;
    bool bCurv ;
    bool bDistance;
    bool bCortical ;
    bool bStandard;
    bool bSection;
    QList<QString>* dirList;
    QString targetDir;
    QString groupDir;
    QString minMaxDir;
    double perc;


    /**
     * REFACTOR CODE
     */
    Mat gradient;
    Mat gradient2;
    Mat gradientReverse;

    void exportMapColor(Mat map,
                        const Mat mask,
                        const QString targetDir,
                        const EMapType type,
                        const bool blured,
                        const bool caption,
                        const std::string ext = "")  throw(MergeException);

    void exportMapNormalized(Mat map,
                             const Mat mask,
                             const QString targetDir,
                             const EMapType type,
                             const float min,
                             const float max,
                             const std::string ext = "")  throw(MergeException);

    void sumMat(Mat* sumMat, Mat* weight, const Mat* mat, const Mat* mask);
    void normalizeMat(Mat src, Mat* dst, const Mat* mask, float newMin, float newMax);
    void getMinMaxMat(const Mat* mat, const Mat* mask, float* min, float* max);
    void getMinMaxGroup(const QList<QString>* directories, float* min, float* max, const QString filename) throw(MergeException);

    QList<Mat>* meanMat(const QList<QString>* directories, const int threshold, const EMapType type) throw(MergeException);
    QList<Mat>* deviationMat(const QList<QString>* directories, const int threshold, const EMapType type) throw(MergeException);

    void addCaption(Mat map);

};

#endif // MERGE_H
