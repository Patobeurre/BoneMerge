#include "filemanager.h"

#include <iostream>
#include <QFile>
#include <QString>
#include <QTextStream>

using namespace std;
using namespace cv;


/**
 * @brief Export a map to the given filepath
 * @param[in] map the matrix to export
 * @param[in] filepath the absolute path of the file
 * @return
 */
bool FileManager::exportMap(const Mat map, const string filepath)  throw(MergeException)
{
    QString filename = QString::fromStdString(filepath) + ".txt";
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        for (int j = 0; j < map.rows; j++)
        {
            QTextStream out(&file);
            for (int i = 0; i < map.cols; i++)
            {
                float val = map.at<float>(j, i);
                if (val > MASK_MIN)
                    out << map.at<float>(j, i) << " ";
                else
                    out << "none" << " ";
            }
            out << endl;
        }
        file.close();

        return true;
    }
    else
    {
        throw MergeException("Error", "Can't write in file", "At location: " + filename.toStdString());
    }

    return false;
}

/**
 * @brief Read a map from the given filepath
 * @param[in] filepath the absolute path to file
 * @return the map lines
 */
QList<QStringList> FileManager::readMapFromFile(const string filepath) throw(MergeException)
{
    QList<QStringList> mat;

    QString filename = QString::fromStdString(filepath);
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString line = file.readLine();
        while (!line.isEmpty()) {
            QString s(line.data());
            QStringList sl = s.split(" ");
            mat.append(sl);
            line = file.readLine();
        }
        file.close();
    }
    else
    {
        throw MergeException("Error", "File not found", "At location: " + filename.toStdString());
    }

    return mat;
}

/**
 * @brief Import a map from file associated with it's mask
 * @details
 * The mask share the same size and shape of the map.
 * The value of the mask is set to 0 if the associated map's value is "none"
 * or 255 otherwise.
 * @param[in] filepath
 * @return a list which contains the map and it's mask.
 */
QList<Mat*>* FileManager::importMapWithMask(const string filepath)
{
    QList<QStringList> mat = readMapFromFile(filepath);

    QList<Mat*>* list = new QList<Mat*>();
    Mat* img = new Mat();
    img->create(mat.length(), mat.at(0).length()-1, CV_32FC1);

    Mat* mask = new Mat();
    mask->create(mat.length(), mat.at(0).length()-1, CV_32FC1);

    for(int i = 0; i < img->cols; i++)
    {
        for(int j = 0; j < img->rows; j++)
        {
            string value = QString(mat.at(j).at(i)).toStdString();

            img->at<float>(j, i) = atof(value.c_str());

            if(value == "none")
                mask->at<float>(j, i) = MASK_MIN;
            else
                mask->at<float>(j, i) = MASK_MAX;
        }
    }

    //resize(*img,*img,Size(360,300));
    list->push_back(img);
    list->push_back(mask);
    return list;
}

bool FileManager::writeInFile(const QList<QString> *data, const string filepath)
{
    QString filename = QString::fromStdString(filepath) + ".txt";
    QFile file(filename);

    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);

        for (QString str : *(data))
        {
            out << str << endl;
        }
        file.close();

        return true;
    }

    return false;
}

QList<QStringList> FileManager::readFromFile(const string filepath)
{
    QList<QStringList> data;

    QString filename = QString::fromStdString(filepath);
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString line = file.readLine();
        while (!line.isEmpty())
        {
            QString s(line.data());
            QStringList sl = s.split(" ");
            data.append(sl);

            line = file.readLine();
        }
        file.close();
    }

    return data;
}

Mat* FileManager::importMap(const string filepath)
{
    QList<QStringList> mat = readMapFromFile(filepath);

    Mat* img = new Mat();
    img->create(mat.length(), mat.at(0).length()-1, CV_32FC1);

    for(int i = 0; i < img->cols; i++)
    {
        for(int j = 0; j < img->rows; j++)
        {
            string value = QString(mat.at(j).at(i)).toStdString();

            img->at<float>(j, i) = atof(value.c_str());
        }
    }

    //resize(*img,*img,Size(360,300));

    return img;
}
