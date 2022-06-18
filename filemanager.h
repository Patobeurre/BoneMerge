#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <opencv2/imgproc/imgproc.hpp>
#include <QList>

#define MASK_MIN    0
#define MASK_MAX    255


/**
 * @brief The FileManager class
 * @details
 * Provide static functions to import/export cv::Mat from/to files
 * @author Brice Maussang
 */
class FileManager
{
public:
    FileManager();

    /// Export a map to file
    static bool exportMap(const cv::Mat map, const std::string filepath);
    /// Read a map from file
    static QList<QStringList> readMapFromFile(const std::string filepath);
    /// Import a map from file
    static cv::Mat* importMap(const std::string filepath);
    /// Import a map and his mask from file
    static QList<cv::Mat*>* importMapWithMask(const std::string filepath);

    static bool writeInFile(const QList<QString>* data, const std::string filepath);
    static QList<QStringList> readFromFile(const std::string filepath);
};

#endif // FILEMANAGER_H
