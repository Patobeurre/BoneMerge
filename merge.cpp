#include "merge.h"
#include "filemanager.h"

#include <iostream>
#include <algorithm>
#include <QMessageBox>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QSet>
#include <QException>

using namespace cv;
using namespace std;


void Merge::setbCortical(bool b)
{
    bCortical=b;
}

void Merge::setbCurv(bool b)
{
    bCurv=b;
}
void Merge::setbDistance(bool b)
{
    bDistance=b;
}
void Merge::setbModulus(bool b)
{
    bModulus=b;
}
void Merge::setbMoments(bool b)
{
    bMoments=b;
}

void Merge::setbStandard(bool b)
{
    bStandard=b;
}

void Merge::setbSection(bool b)
{
    bSection=b;
}

void Merge::setDirectories(QList<QString>* list)
{
    dirList = list;
}


int Merge::launchMerge(int id, bool _bStandard)
{
    cout << "LAUNCH MERGE" << endl;
    Mat mapMerge = Mat(300,360,CV_32FC1,cvScalar(0.));
    QList<bool> lb;
    QString filename = chooseFile(id);

    Mat* ave;
    Mat* dev;

    if(_bStandard)
    {
        ave=GetMapFromTxt(groupDir+"/"+chooseMap(id)+".txt");
        cout << "loading ...." << endl;
        cout << groupDir.toStdString()+"/"+chooseMap(id).toStdString()+".txt" << endl;
        cout << groupDir.toStdString()+"/"+chooseMap(id).toStdString()+"deviation.txt" << endl;
        dev=GetMapFromTxt(groupDir+"/"+chooseMap(id)+"deviation.txt");
    }

    Mat weight(300, 360, CV_32FC1, Scalar(1));

    int count=0;
    cout << "for all directories" << endl;
    for(int ii=0;ii<dirList->length();ii++)
    {
        QDir dir(dirList->at(ii));

        QDir dirMaps = QDir(dir.path() + "/maps");
        if(!dirMaps.exists()){
            displayErrorBox("Folder missing","<i><b>maps</b></i> folder missing");
            lb.append(false);
        }
        else
            lb.append(true);

        QDir dirSerie = QDir(dir.path() + "/serie");
        if(!dirSerie.exists()){
            displayErrorBox("Folder missing","<i><b>series</b></i> folder missing");
            lb.append(false);
        }
        else
            lb.append(true);

        float ma=1.0;
        if(bStandard)
        {
            ma=getStand(dirSerie.absolutePath().toStdString() + "/" + "stand.txt",id);
        }

        float stand=1.0;
        if(bStandard)
            stand=ma;

        //stand=1.0f;

        if(stand<=0.0)
            continue;

        string filename = QString(dirMaps.absolutePath() + "/" + chooseFile(id)).toStdString();
        QList<Mat*>* listImg = FileManager::importMapWithMask(filename);
        Mat* img = listImg->at(0);
        Mat* mask = listImg->at(1);

        Scalar average = 1.0;
        if(_bStandard){
            average = mean(*img);
        }


        /*
        QList<Mat*>* listImg = GetMapMaskFromTxt(dirMaps.absolutePath() + "/" + chooseFile(id), stand);
        Mat* img = listImg->at(0);
        Mat* mask = listImg->at(1);
        Scalar average =1.0;
        if(_bStandard){
            average = mean(*img);
        }
        */

        for(int i=0;i<mapMerge.cols;i++)
        {
            for(int j=0;j<mapMerge.rows;j++)
            {
                if(mask->at<float>(j,i)!=0){

                    if(!_bStandard)
                        mapMerge.at<float>(j,i) += img->at<float>(j,i);
                    else{
                        if(dev->at<float>(j,i)>0.0f){
                            mapMerge.at<float>(j,i) +=(img->at<float>(j,i)/average.val[0]);
                            cout << "Diff :" << (img->at<float>(j,i) - ave->at<float>(j,i)) << endl;
                            cout << "Div :" << ((img->at<float>(j,i) - ave->at<float>(j,i))/dev->at<float>(j,i)) << endl << endl;
                        }
                    }
                    weight.at<float>(j,i) += 1.0;
                }

            }
        }

        /*std::vector<Point> iP, iiP;
        Mat dst(360,360,CV_8UC1);
        Mat draw;
        img->copyTo(draw);

        float size=20.0;

        normalize(draw, draw, 0.0, 1.0, NORM_MINMAX, CV_32FC1);
        int incx = img->cols/size;
        int incy= img->rows/size;
        int startx = incx/2.0;
        int starty = incy/2.0;

        int incxend = dst.cols/size;
        int incyend= dst.rows/size;
        int startxend = incxend/2.0;
        int startyend = incyend/2.0;

        normalize(*img, *img, 0.0, 1.0, NORM_MINMAX, CV_32FC1);


        for(int i=0;i<int(size);i++){
            for(int j=0;j<int(size);j++){
                int x=(i*incx) + startx;
                int y=(j*incy)+ starty;
                int xend=(i*incxend) + startxend;
                int yend=(j*incyend)+ startyend;
                iP.push_back(Point(x,y));

                cout <<x <<" "<<y<< endl;

                iiP.push_back(Point(xend,yend));

                circle(draw,Point(x,y),1,Scalar( 255, 255, 255 ));
                //cout <<(i/100.0)*dst.rows<<" "<<(j/100.0)*dst.cols<< endl;

            }
        }*/


        /* iP.push_back(Point(50,50));
        iP.push_back(Point(300,50));
        iP.push_back(Point(50,300));
        iP.push_back(Point(300,300));
        iP.push_back(Point(256,256));
        iP.push_back(Point(150,256));

        iiP.push_back(Point(70,70));
        iiP.push_back(Point(330,60));
        iiP.push_back(Point(60,310));
        iiP.push_back(Point(330,320));
        iiP.push_back(Point(220,280));
        iiP.push_back(Point(180,240));*/

        /* CThinPlateSpline tps(iP,iiP);
        tps.warpImage(*img,dst,0.01,INTER_CUBIC,BACK_WARP);

        imshow("original",*img);
        imshow("distorted",dst);
        imshow("color",draw);
        waitKey(0);*/

        count++ ;
        delete img;
    }

    FileManager::exportMap(mapMerge, targetDir.toStdString() + "/mapmerge");

    cout << "for all mapMerge" << endl;
    for(int i=0;i<mapMerge.cols;i++)
    {
        for(int j=0;j<mapMerge.rows;j++)
        {
            if(!_bStandard){
                if( weight.at<float>(j,i)>1)
                    mapMerge.at<float>(j,i) /= (weight.at<float>(j,i)-1);

            }else{
                cout << mapMerge.at<float>(j,i) << endl;
            }
        }
    }


    QList<Mat>* mapMergeList = meanMat(dirList, 2, getTypeFromId(id));
    Mat mapMergeTest = mapMergeList->at(0);
    Mat weightTest = mapMergeList->at(1);
    cout << mapMergeTest.at<float>(0,0) << endl;
    Mat* weightTestPtr = &weightTest;
    cout << weightTestPtr->at<float>(0,0) << endl;

    FileManager::exportMap(weightTest, targetDir.toStdString() + "/weights");


    cout << "map to color" << endl;
    if(id==1 || id==2 || id==4)
        mapToColor2(mapMergeTest,gradient,targetDir.toStdString()+ "/" +chooseMap(id).toStdString(),&weightTest);
    else if(id==3)
        mapToColor2(mapMergeTest,gradientReverse,targetDir.toStdString()+ "/" +chooseMap(id).toStdString(),&weightTest);
    else if(id==5)
        mapToColor2(mapMergeTest,gradient2,targetDir.toStdString()+ "/" +chooseMap(id).toStdString(),&weightTest);


    // normalize(mapMerge, mapMerge, 0.0, 1.0, NORM_MINMAX, CV_32FC1);

    //   imshow("weight",weight);waitKey(0);

    /*
    cout << "map to color 2" << endl;
    if(id==1 || id==2 || id==4)
        mapToColor2(mapMerge,gradient,targetDir.toStdString()+ "/" +chooseMap(id).toStdString(),&weight);
    else if(id==3)
        mapToColor2(mapMerge,gradientReverse,targetDir.toStdString()+ "/" +chooseMap(id).toStdString(),&weight);
    else if(id==5)
        mapToColor2(mapMerge,gradient2,targetDir.toStdString()+ "/" +chooseMap(id).toStdString(),&weight);
    */


    return count;
}



int Merge::launchMedian(int id, bool _bStandard)
{
    Mat mapMed = Mat(300,360,CV_32FC1,cvScalar(0.));
    vector<Mat*> vecImg;
    vector<Mat*> vecMask;
    QList<bool> lb;
    QString filename = chooseFile(id);
    Mat gradient = imread("rsc/gradient.png");
    if(!gradient.data)
        return -1;

    Mat gradient2 = imread("rsc/gradient2.png");
    if(!gradient2.data)
        return -1;

    Mat gradientReverse = imread("rsc/gradientReverse.png");
    if(!gradientReverse.data)
        return -1;


    Mat* ave;
    Mat* dev;

    if(_bStandard)
    {
        ave=GetMapFromTxt(groupDir+"/"+chooseMap(id)+".txt");
        cout << "loading ...." << endl;
        cout << groupDir.toStdString()+"/"+chooseMap(id).toStdString()+".txt" << endl;
        cout << groupDir.toStdString()+"/"+chooseMap(id).toStdString()+"deviation.txt" << endl;
        dev=GetMapFromTxt(groupDir+"/"+chooseMap(id)+"deviation.txt");
    }


    Mat weight;
    weight.create(300,360,CV_32FC1);
    for(int i=0;i<weight.cols;i++)
    {
        for(int j=0;j<weight.rows;j++)
        {
            weight.at<float>(j,i) = 1.0;
        }
    }
    int count=0;
    for(int i=0;i<dirList->length();i++)
    {
        QDir dir(dirList->at(i));

        QDir dirMaps = QDir(dir.path() + "/maps");
        if(!dirMaps.exists()){
            displayErrorBox("Folder missing","<i><b>maps</b></i> folder missing");
            lb.append(false);
        }
        else
            lb.append(true);

        QDir dirSerie = QDir(dir.path() + "/serie");
        if(!dirSerie.exists()){
            displayErrorBox("Folder missing","<i><b>series</b></i> folder missing");
            lb.append(false);
        }
        else
            lb.append(true);

        float ma=1.0;
        if(bStandard)
        {


            ma=getStand(dirSerie.absolutePath().toStdString() + "/" + "stand.txt",id);


        }



        float stand=1.0;
        if(bStandard)
            stand=ma;
        if(stand>0.0){


            QList<Mat*>* listImg = GetMapMaskFromTxt(dirMaps.absolutePath() + "/" + chooseFile(id), stand);
            vecImg.push_back(listImg->at(0));
            vecMask.push_back(listImg->at(1));
            count++;
        }
    }
    cout << "ok 1" << endl;
    for(int i=0;i<mapMed.cols;i++)
    {
        for(int j=0;j<mapMed.rows;j++)
        {

            vector<float> v;
            for(unsigned int k=0;k<vecImg.size();k++)
            {
                if(vecMask.at(k)->at<float>(j,i)!=0)
                {
                    if(!_bStandard)
                        v.push_back(vecImg.at(k)->at<float>(j,i));
                    else
                    {
                        float val=((vecImg.at(k)->at<float>(j,i) - ave->at<float>(j,i))/dev->at<float>(j,i));
                        v.push_back(val);

                    }
                    weight.at<float>(j,i) += 1.0;
                }
            }
            mapMed.at<float>(j,i) =getMedian(v);
        }
    }


    cout << "ok 2" << endl;



    //normalize(mapMed, mapMed, 0.0, 1.0, NORM_MINMAX, CV_32FC1);

    if(id==1 || id==2 || id==4)
        mapToColor2(mapMed,gradient,targetDir.toStdString()+ "/" +chooseMap(id).toStdString()+"median",&weight);
    else if(id==3)
        mapToColor2(mapMed,gradientReverse,targetDir.toStdString()+ "/" +chooseMap(id).toStdString()+"median",&weight);
    else if(id==5)
        mapToColor2(mapMed,gradient2,targetDir.toStdString()+ "/" +chooseMap(id).toStdString()+"median",&weight);

    return count;
}


void Merge::launchNormalized()
{

    Mat gradient = imread("rsc/gradient.png");
    if(!gradient.data)
        return;

    Mat gradient2 = imread("rsc/gradient2.png");
    if(!gradient2.data)
        return;

    Mat gradientReverse = imread("rsc/gradientReverse.png");
    if(!gradientReverse.data)
        return;

    for(int i=0;i<dirList->length();i++)
    {

        QDir dir(dirList->at(i));
        QDir res(dirList->at(i) + "/normalized");
        if (!res.exists())
            res.mkpath(".");


        QStringList nameFilter;
        nameFilter << "*.png";
        QStringList filesList = dir.entryList(nameFilter);
        for(int j=0;j<filesList.length();j++)
        {
            float min=0;
            float max=0;

            QString filename = filesList.at(j);
            QString basename = filename.split(".").at(0);
            QString filePath = minMaxDir +"/MinMax" + basename + ".txt";
            cout << "open " << filePath.toStdString() << endl;

            QFile file(filePath);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QString value;
                value = file.readLine();
                QString s(value.data());
                cout << s.toStdString() << endl;
                min=atof(s.toStdString().c_str());
                value = file.readLine();
                s=QString(value.data());
                cout << s.toStdString() << endl;
                max=atof(s.toStdString().c_str());
                file.close();
            }
            else
            {
                cout << "ERROR NO" << minMaxDir.toStdString() +"/MinMax" + basename.toStdString() + ".txt" <<"FILE FOUNDED" << endl;
            }

            /* XTODO fstream -> QFile
            ifstream file (minMaxDir.toStdString() +"/MinMax" + basename.toStdString() + ".txt");
            if(!file.eof()){

                string value;
                getline(file,value);
                QString s(value.data());
                cout << s.toStdString() << endl;
                min=atof(s.toStdString().c_str());
                getline(file,value);
                s=(value.data());
                cout << s.toStdString() << endl;
                max=atof(s.toStdString().c_str());

            }
            else
            {
                cout << "ERROR NO" << minMaxDir.toStdString() +"/MinMax" + basename.toStdString() + ".txt" <<"FILE FOUNDED" << endl;
            }
            */


            Mat* img = GetMapFromTxt(dir.absolutePath() + "/" +basename+".txt");
            cout << " min " << min << " max " << max << endl;


            customNormalize(*img,img,0.0,1.0,min,max);
            if(basename.contains("Curvature") && !basename.contains("deviation"))
                mapToColor(*img,gradientReverse,res.absolutePath().toStdString()+ "/" + basename.toStdString(),false);
            else if(basename.contains("Modulus"))
                mapToColor(*img,gradient2,res.absolutePath().toStdString()+ "/" + basename.toStdString(),false);
            else
                mapToColor(*img,gradient,res.absolutePath().toStdString()+ "/" + basename.toStdString(),false);

        }
    }

}

void Merge::customNormalize(Mat src, Mat* dst, float newMin, float newMax, float min, float max)
{
    src.copyTo(*dst);

    for (int j = 0; j < src.rows; j++)
    {
        for (int i = 0; i < src.cols; i++)
        {
            float v = ((src.at<float>(j, i) - min) * ((newMax - newMin) / (max - min))) + newMin ;

            if(v < 0)
                v = 0.0f;
            else if(v > 1.0)
                v = 1.0;
            dst->at<float>(j, i) = v;
        }
    }
}


void Merge::getMinMax(int id)
{

    float gmin, gmax;
    cout << "getminmax" << endl;
    for(int j=0;j<3;j++)
    {

        cout << chooseFunc(j).toStdString() << endl;
        for(int i=0;i<dirList->length();i++)
        {

            QDir dir(dirList->at(i));

            cout << (dir.absolutePath() + "/" + chooseMap(id)+chooseFunc(j)+".txt").toStdString() << endl;
            Mat* img = GetMapFromTxt(dir.absolutePath() + "/" + chooseMap(id)+chooseFunc(j)+".txt", 1);




            double min, max;
            minMaxLoc(*img, &min, &max);
            if(i==0)
            {
                gmin=min;
                gmax=max;
            }
            else
            {
                if(min<gmin)
                {
                    gmin=min;
                }
                if(max>gmax)
                {
                    gmax=max;
                }
            }

            QString filePath = targetDir+"/" + "MinMax" +chooseMap(id) + chooseFunc(j)+ ".txt";
            QFile file(filePath);
            if (file.open(QIODevice::WriteOnly)) {
                QTextStream out(&file);
                out << gmin << endl;
                out << gmax << endl;
                file.close();
            }

            /* XTODO fstream -> QFile
            ofstream file_1 (targetDir.toStdString()+"/" + "MinMax" +chooseMap(id).toStdString() + chooseFunc(j).toStdString()+ ".txt");
            file_1 << gmin << endl;
            file_1 << gmax << endl;
            file_1.close();
            */

            delete img;
        }
    }
}


float Merge::getMedian(vector<float> v)
{
    if(v.size()>0)
    {
        std::sort (v.begin(), v.end());
        return v.at(v.size()/2);
    }
    return 0.0;

}


Mat* Merge::GetMapFromTxt(QString filename, float stand)
{
    string line;
    cout << filename.toStdString() << endl;
    QList<QStringList> mat;

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

    /* XTODO fstream -> QFile
    ifstream file (filename.toStdString());
    while(!file.eof()){
        getline(file,line);
        QString s(line.data());
        QStringList sl = s.split(" ");
        mat.append(sl);

    }

    file.close();
    */
    QList<Mat*>* list = new QList<Mat*>();
    Mat* img = new Mat();
    img->create(mat.length()-1,mat.at(0).length()-1,CV_32FC1);

    Mat* mask = new Mat();
    mask->create(mat.length()-1,mat.at(0).length()-1,CV_32FC1);

    cout << filename.toStdString()  << img->rows << " c " << img->cols << endl;
    for(int i=0;i<img->cols;i++)
    {
        for(int j=0;j<img->rows;j++)
        {
            img->at<float>(j,i)=atof(QString(mat.at(j).at(i)).toStdString().c_str())/stand;
            //cout << i << " " << j << " " << atof(QString(mat.at(j).at(i)).toStdString().c_str()) <<" " << QString(mat.at(j).at(i)).toStdString() << endl;
            if(QString(mat.at(j).at(i)).toStdString() == "none")
                mask->at<float>(j,i)=0.0;
            else
                mask->at<float>(j,i)=255.0;


        }
    }


    resize(*img,*img,Size(360,300));
    list->push_back(img);
    list->push_back(mask);
    return img;
}


QList<Mat*>* Merge::GetMapMaskFromTxt(QString filename, float stand)
{
    QList<QStringList> mat;

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

    /* XTODO fstream -> QFile
    ifstream file (filename.toStdString());
    while(!file.eof()){
        getline(file,line);
        QString s(line.data());
        QStringList sl = s.split(" ");
        mat.append(sl);

    }

    file.close();
    */

    QList<Mat*>* list = new QList<Mat*>();
    Mat* img = new Mat();
    img->create(mat.length(),mat.at(0).length()-1,CV_32FC1);

    Mat* mask = new Mat();
    mask->create(mat.length(),mat.at(0).length()-1,CV_32FC1);

    cout << filename.toStdString()  << img->rows << " c " << img->cols << endl;
    cout <<" PROCESSING : " <<  filename.toStdString()  << " WITH " << stand << endl;
    for(int i=0;i<img->cols;i++)
    {
        for(int j=0;j<img->rows;j++)
        {
            img->at<float>(j,i)=atof(QString(mat.at(j).at(i)).toStdString().c_str());// / stand;
            //cout << i << " " << j << " " << atof(QString(mat.at(j).at(i)).toStdString().c_str()) <<" " << QString(mat.at(j).at(i)).toStdString() << endl;
            if(QString(mat.at(j).at(i)).toStdString() == "none")
                mask->at<float>(j,i)=0.0;
            else
                mask->at<float>(j,i)=255.0;


        }
    }

    resize(*img,*img,Size(360,300));
    list->push_back(img);
    list->push_back(mask);
    return list;
}


int Merge::launchDeviation(int id,bool _bStandard)
{
    Mat mapMerge = Mat(300,360,CV_32FC1,cvScalar(0.));
    Mat mapDeviate = Mat(300,360,CV_32FC1,cvScalar(0.));
    QList<bool> lb;
    QString filename = chooseFile(id);
    Mat gradient = imread("rsc/gradient.png");
    if(!gradient.data)
        return -1;

    Mat gradient2 = imread("rsc/gradient2.png");
    if(!gradient2.data)
        return -1;

    Mat gradientReverse = imread("rsc/gradientReverse.png");
    if(!gradientReverse.data)
        return -1;


    Mat* ave;
    Mat* dev;

    if(_bStandard)
    {
        ave=GetMapFromTxt(groupDir+"/"+chooseMap(id)+".txt");
        cout << "loading ...." << endl;
        cout << groupDir.toStdString()+"/"+chooseMap(id).toStdString()+".txt" << endl;
        cout << groupDir.toStdString()+"/"+chooseMap(id).toStdString()+"deviation.txt" << endl;
        dev=GetMapFromTxt(groupDir+"/"+chooseMap(id)+"deviation.txt");
    }


    Mat weight;
    weight.create(300,360,CV_32FC1);
    for(int i=0;i<weight.cols;i++)
    {
        for(int j=0;j<weight.rows;j++)
        {
            weight.at<float>(j,i) = 1.0;
        }
    }

    float ma=1.0;
    int count=0;
    for(int i=0;i<dirList->length();i++)
    {
        QDir dir(dirList->at(i));

        QDir dirMaps = QDir(dir.path() + "/maps");
        if(!dirMaps.exists()){
            displayErrorBox("Folder missing","<i><b>maps</b></i> folder missing");
            lb.append(false);
        }
        else
            lb.append(true);

        QDir dirSerie = QDir(dir.path() + "/serie");
        if(!dirSerie.exists()){
            displayErrorBox("Folder missing","<i><b>series</b></i> folder missing");
            lb.append(false);
        }
        else
            lb.append(true);

        if(bStandard)
        {

            ma=getStand(dirSerie.absolutePath().toStdString() + "/" + "stand.txt",id);

        }



        float stand=1.0;
        if(bStandard)
            stand=ma;


        if(stand<=0.0)
            continue;
        QList<Mat*>* listImg = GetMapMaskFromTxt(dirMaps.absolutePath() + "/" + chooseFile(id), stand);
        Mat* img = listImg->at(0);
        Mat* mask = listImg->at(1);


        for(int i=0;i<mapMerge.cols;i++)
        {
            for(int j=0;j<mapMerge.rows;j++)
            {

                if(mask->at<float>(j,i)!=0){

                    if(!_bStandard)
                        mapMerge.at<float>(j,i) += img->at<float>(j,i);
                    else{
                        mapMerge.at<float>(j,i) +=((img->at<float>(j,i) - ave->at<float>(j,i))/dev->at<float>(j,i));
                    }
                    weight.at<float>(j,i) += 1.0;
                }
            }
        }



        delete img;
        count ++;
    }

    for(int i=0;i<mapMerge.cols;i++)
    {
        for(int j=0;j<mapMerge.rows;j++)
        {
            if( weight.at<float>(j,i)>1)
                mapMerge.at<float>(j,i) /= (weight.at<float>(j,i)-1);


        }
    }


    for(int i=0;i<dirList->length();i++)
    {
        QDir dir(dirList->at(i));

        QDir dirMaps = QDir(dir.path() + "/maps");
        if(!dirMaps.exists()){
            displayErrorBox("Folder missing","<i><b>maps</b></i> folder missing");
            lb.append(false);
        }
        else
            lb.append(true);

        QDir dirSerie = QDir(dir.path() + "/serie");
        if(!dirSerie.exists()){
            displayErrorBox("Folder missing","<i><b>series</b></i> folder missing");
            lb.append(false);
        }
        else
            lb.append(true);

        if(bStandard)
        {

            ma=getStand(dirSerie.absolutePath().toStdString() + "/" + "stand.txt",id);

        }



        float stand=1.0;
        if(bStandard)
            stand=ma;


        if(stand<=0.0)
            continue;
        Mat* img = GetMapFromTxt(dirMaps.absolutePath() + "/" + chooseFile(id), stand);




        for(int i=0;i<mapMerge.cols;i++)
        {
            for(int j=0;j<mapMerge.rows;j++)
            {
                if(img->at<float>(j,i) != 0.0){
                    if(!_bStandard)
                        mapDeviate.at<float>(j,i) += pow(img->at<float>(j,i)-mapMerge.at<float>(j,i),2);
                    else
                    {
                        float val=((img->at<float>(j,i) - ave->at<float>(j,i))/dev->at<float>(j,i));
                        mapDeviate.at<float>(j,i) += pow(val-mapMerge.at<float>(j,i),2);

                    }

                }

            }
        }



        delete img;
    }

    for(int i=0;i<mapDeviate.cols;i++)
    {
        for(int j=0;j<mapDeviate.rows;j++)
        {

            if( weight.at<float>(j,i)>1){
                mapDeviate.at<float>(j,i) /= weight.at<float>(j,i);
                mapDeviate.at<float>(j,i) = sqrtf( mapDeviate.at<float>(j,i));
            }
        }
    }

    // normalize(mapDeviate, mapDeviate, 0.0, 1.0, NORM_MINMAX, CV_32FC1);

    //normalize(mapMerge, mapMerge, 0.0, 1.0, NORM_MINMAX, CV_32FC1);
    mapToColor2(mapDeviate,gradient,targetDir.toStdString()+ "/" +chooseMap(id).toStdString()+"deviation",&weight);

    return count;
}


QList<float> Merge::launchExtraction(QString filename,int section)
{



    QList<float> res;
    Mat* img = GetMapFromTxt(filename, 1.0);
    int index=int(((section/100.0)*img->rows)+0.5);
    float min = img->at<float>(index,0);
    float max = img->at<float>(index,0);
    for(int i=0;i<img->cols;i++)
    {
        res.append(img->at<float>(index,i));

        if(img->at<float>(index,i) <min)
            min=img->at<float>(index,i);
        else if(img->at<float>(index,i) > max)
            max=img->at<float>(index,i);
    }

    res.append(min);
    res.append(max);
    delete img;
    return res;
}

void Merge::displayErrorBox(QString errorTitle, QString errorMessage)
{
    QMessageBox msgBox;
    msgBox.setWindowTitle(errorTitle);
    msgBox.setText(errorMessage);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.exec();

}
float* Merge::getMinMax(Mat *m , Mat *mask)
{
    float* val = new float[2];
    val[0] = m->at<float>(0,0);
    val[1] = m->at<float>(0,0);
    for (int j=0; j < m->rows;j++)
    {
        for (int i=0; i < m->cols;i++)
        {
            if(mask->at<float>(j,i)>1){
                if(val[0]>m->at<float>(j,i))
                    val[0]=m->at<float>(j,i);
                if(val[1]<m->at<float>(j,i))
                    val[1]=m->at<float>(j,i);
            }
        }
    }

    return val;

}


void Merge::mapToColor(Mat mapRaw, Mat gradient , string name, bool bnormalize)
{
    FileManager::exportMap(mapRaw, name);
    /*
    QString filePath = QString::fromStdString(name) + ".txt";
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        for (int j=0; j < mapRaw.rows;j++)
        {
            QTextStream out(&file);
            for (int i=0; i < mapRaw.cols;i++)
            {
                out << mapRaw.at<float>(j,i) << " ";
            }
            out << endl;
        }
        file.close();
    }
    */

    /* XTODO fstream -> QFile
    ofstream file_1 (name+  + ".txt");
    for (int j=0; j < mapRaw.rows;j++)
    {
        for (int i=0; i < mapRaw.cols;i++)
        {
            file_1 << mapRaw.at<float>(j,i) << " ";
        }

        file_1 << endl;
    }
    file_1.close();
    */

    blur(mapRaw,mapRaw,Size(1,35) );

    if(bnormalize)
        normalize(mapRaw, mapRaw, 0.0, 1.0, NORM_MINMAX, CV_32FC1);

    Mat mapColor = mapToColorGradient(&mapRaw, &gradient);

    /*
    Mat mapColor(mapRaw.rows,mapRaw.cols,CV_8UC3);

    for (int j=0; j < mapRaw.rows;j++)
    {
        for (int i=0; i < mapRaw.cols;i++)
        {
            int index = int(mapRaw.at<float>(j,i)*1000);
            if(index > 1000 || index < 0)
            {
                cout << " Error index " << index;
                break;
            }
            mapColor.at<Vec3b >(j,i)[0]=gradient.at<Vec3b>(index,0)[0];
            mapColor.at<Vec3b >(j,i)[1]=gradient.at<Vec3b>(index,0)[1];
            mapColor.at<Vec3b >(j,i)[2]=gradient.at<Vec3b>(index,0)[2];

        }
    }
    */


    putText(mapColor, "L", Point(0, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
    putText(mapColor, "A", Point(90, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
    putText(mapColor, "M", Point(180, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
    putText(mapColor, "P", Point(270, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
    putText(mapColor, "L", Point(350, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
    imshow( name, mapColor );
    //cout << targetDir.toStdString() + name+ ".png";
    imwrite(name + ".png",mapColor );
}

void Merge::mapToColor2(Mat mapRaw, Mat gradient , string name,Mat* mask, bool bNormalize)
{
    cout << "export map" << endl;
    FileManager::exportMap(mapRaw, name);
    /*
    QString filePath = QString::fromStdString(name) + ".txt";
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        for (int j=0; j < mapRaw.rows;j++)
        {
            QTextStream out(&file);
            for (int i=0; i < mapRaw.cols;i++)
            {
                out << mapRaw.at<float>(j,i) << " ";
            }
            out << endl;
        }
        file.close();
    }
    */

    /* XTODO fstream -> QFile
    ofstream file_1 (name+  + ".txt");
    for (int j=0; j < mapRaw.rows;j++)
    {
        for (int i=0; i < mapRaw.cols;i++)
        {
            file_1 << mapRaw.at<float>(j,i) << " ";
        }

        file_1 << endl;
    }
    file_1.close();
    */

    cout << "apply blur" << endl;
    blur(mapRaw,mapRaw,Size(1,25) );

    float * min_Max = getMinMax(&mapRaw,mask);
    if(bNormalize)
        customNormalize(mapRaw,&mapRaw,0.0,1.0,min_Max[0],min_Max[1]);

    cout << "map to gradient color with mask" << endl;
    Mat mapColor = mapToColorGradientWithMask(&mapRaw, &gradient, mask);

    /*
    Mat mapColor(mapRaw.rows,mapRaw.cols,CV_8UC3);

    for (int j=0; j < mapRaw.rows;j++)
    {
        for (int i=0; i < mapRaw.cols;i++)
        {   if(mask->at<float>(j,i)>1){
                int index = int(mapRaw.at<float>(j,i)*1000);
                if(index > 1000 || index < 0)
                {
                    cout << " Error index " << index;
                    break;
                }
                mapColor.at<Vec3b >(j,i)[0]=gradient.at<Vec3b>(index,0)[0];
                mapColor.at<Vec3b >(j,i)[1]=gradient.at<Vec3b>(index,0)[1];
                mapColor.at<Vec3b >(j,i)[2]=gradient.at<Vec3b>(index,0)[2];
            }
            else
            {
                mapColor.at<Vec3b >(j,i)[0]=0;
                mapColor.at<Vec3b >(j,i)[1]=0;
                mapColor.at<Vec3b >(j,i)[2]=0;
            }
        }
    }
    */

    addCaption(mapColor);

    imshow( name, mapColor );
    //cout << targetDir.toStdString() + name+ ".png";
    imwrite(name+ ".png",mapColor );
}


void Merge::setTargetDir(QString dir)
{
    targetDir = dir;

}
void Merge::setGroupDir(QString dir)
{
    groupDir = dir;

}


void Merge::setMinMaxDir(QString dir)
{
    minMaxDir = dir;

}


bool Merge::getbCortical()
{
    return bCortical;
}

bool Merge::getbCruv()
{
    return bCurv;
}

bool Merge::getbDistance()
{
    return bDistance;
}

bool Merge::getbModulus()
{
    return bModulus;
}

bool Merge::getbMoments()
{
    return bMoments;
}


bool Merge::getbSection()
{
    return bSection;
}

QString Merge::chooseFunc(int id)
{
    switch (id) {
    case 0:
        return "median";
        break;
    case 1:
        return "";
        break;
    case 2:
        return "deviation";
        break;

    default:
        return "";
        break;
    }
    return "";
}


QString Merge::chooseMap(int id)
{
    switch (id) {
    case 1:
        return "ExternalRadius";
        break;
    case 2:
        return "CorticalThickness";
        break;
    case 3:
        return "ExternalCurvature";
        break;
    case 4:
        return "Moments";
        break;
    case 5:
        return "Modulus";
        break;

    default:
        return "";
        break;
    }
    return "";
}
QString Merge::chooseFile(int id)
{
    switch (id) {
    case 1:
        return "mapExtRadius.txt";
        break;
    case 2:
        return "mapCortical.txt";
        break;
    case 3:
        return "mapCurv.txt";
        break;
    case 4:
        return "mapMoment.txt";
        break;
    case 5:
        return "mapModulus.txt";
        break;

    default:
        return "";
        break;
    }
    return "";
}


float Merge::getStand(string filename, int id)
{
    cout << "getstand ################################## 2" << endl;
    string value;
    float ma=-1.0;
    float ta=-1.0;

    QFile file(QString::fromStdString(filename));
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString line = file.readLine();
        while (!line.isEmpty()) {
            QString s(line.data());
            QStringList list = s.split(" ");
            if(list.length() == 2)
            {
                if(list.at(0) == "m")
                {
                    ma = atof(list.at(1).toStdString().c_str());
                }
                else if(list.at(0) == "s")
                {
                    ta = atoi(list.at(1).toStdString().c_str());
                }
            }
            line = file.readLine();
        }
        file.close();
    }

    /* XTODO fstream -> QFile
    ifstream file (filename);

    while(!file.eof())
    {
        getline(file,value);
        QString s(value.data());
        QStringList list = s.split(" ");
        if(list.length()==2)
        {
            if(list.at(0) == "m")
            {
                ma=atof(list.at(1).toStdString().c_str());
            }
            else if(list.at(0) == "s")
            {
                ta=atoi(list.at(1).toStdString().c_str());
            }

        }
        cout << filename << " "<< file.eof() << endl;
        cout << ta << " " << ma << endl;
    }

    file.close();
    */

    cout << "getstand ################################## 2" << ta << " " << ma << endl;

    switch (id) {
    case 1:
        cout << "stand ma" <<ma << endl;
        return ma;
        break;
    case 2:
        cout << "stand ma" <<ma << endl;
        return ma;
        break;
    case 3:
        cout << "stand ma" <<ma << endl;
        return ma;
        break;
    case 4:
        cout << "stand ma ta" <<ma*ta << endl;
        return ma*ta;
        break;
    case 5:
        cout << "stand ma ta" <<ma*ta << endl;
        return ma*ta;
        break;

    default:
        return 1.0;
        break;
    }
    return 1.0;

}

QString Merge::getTarget()
{
    return targetDir;
}
QString Merge::getGroup()
{
    return groupDir;
}

QString Merge::getMinMaxTarget(){
    return minMaxDir;
}


void Merge::setPercentage(double p)
{
    perc=p;
}

void Merge::writeHowMany()
{
    QString filePath = targetDir + "/nInd.txt";
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        string value = QString::number(dirList->length()).toStdString();
        file.write(value.c_str(), qstrlen(value.c_str()));
        file.close();
    }

    /* XTODO fstream -> QFile
    ofstream file_1 (targetDir.toStdString()+"/nInd.txt");
    file_1 << dirList->length() << endl;
    file_1.close();
    */
}




void Merge::writeHowManyDetail(int* tab)
{
    QString filePath = targetDir + "/nIndDetail.txt";
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        for(int i = 0; i < 5; i++)
        {
            out << chooseMap(i+1) << " " << tab[i] << endl;
        }
        file.close();
    }

    /* XTODO fstream -> QFile
    ofstream file_1 (targetDir.toStdString()+"/nIndDetail.txt");
    for(int i=0;i<5;i++)
    {
        file_1 << chooseMap(i+1).toStdString() << " " << tab[i]<< endl;
        cout <<  chooseMap(i+1).toStdString() << " " << tab[i] << endl;

    }
    file_1.close();
    */
}


void Merge::writeList()
{
    QString filePath = targetDir + "/group.txt";
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        for(int i = 0; i < dirList->length(); i++)
        {
            string value = dirList->at(i).toStdString();
            file.write(value.c_str(), qstrlen(value.c_str()));
        }
        file.close();
    }

    /* XTODO fstream -> QFile
    ofstream file_1 (targetDir.toStdString()+"/group.txt");
    for(int i=0;i<dirList->length();i++)
        file_1 << dirList->at(i).toStdString() << endl;

    file_1.close();
    */
}



/**
 *  REFACTOR CODE
 */


/**
 * @brief Constructor
 */
Merge::Merge()
{
    bMoments =false;
    bModulus=false ;
    bCurv=false ;
    bDistance=false;
    bCortical=false ;
    bStandard=false;
    bSection=false;
    perc = 0.0;
}

/**
 * @brief Load gradient images from resource directory
 * @throw MergeException if file not found
 */
void Merge::loadGradientImages() throw(MergeException)
{
    gradient = imread("rsc/gradient.png");
    if(!gradient.data)
        throw MergeException("Error",
                             "File not found. Can't load gradient images",
                             "Make sure the folder /rsc exists and contains all gradient image files");

    gradient2 = imread("rsc/gradient2.png");
    if(!gradient2.data)
        throw MergeException("Error",
                             "File not found. Can't load gradient images",
                             "Make sure the folder /rsc exists and contains all gradient image files");

    gradientReverse = imread("rsc/gradientReverse.png");
    if(!gradientReverse.data)
        throw MergeException("Error",
                             "File not found. Can't load gradient images",
                             "Make sure the folder /rsc exists and contains all gradient image files");
}

/**
 * @brief Normalize map's values within given range
 * @param[in] src the map to normalize
 * @param[out] dst the normalized map
 * @param[in] mask the source matrix mask
 * @param[in] newMin the normalize minimum
 * @param[in] newMax the normalize maximum
 */
void Merge::normalizeMat(Mat src, Mat* dst, const Mat* mask, float newMin, float newMax)
{
    float min, max;
    getMinMaxMat(&src, mask, &min, &max);

    src.copyTo(*dst);

    for (int j = 0; j < src.rows; j++)
    {
        for (int i = 0; i < src.cols; i++)
        {
            float v = ((src.at<float>(j, i) - min) * ((newMax - newMin) / (max - min))) + newMin ;

            if(v < 0)
                v = 0.0f;
            else if(v > newMax)
                v = newMax;

            dst->at<float>(j, i) = v;
        }
    }
}

/**
 * @brief Retreive minimum and maximum values from a matrix
 * @param[in] mat the matrix
 * @param[in] mask the matrix mask
 * @param[out] min the minimum value
 * @param[out] max the maximum value
 */
void Merge::getMinMaxMat(const Mat *mat, const Mat *mask, float *min, float *max)
{
    if (mat->empty()) return;

    *min = *max = mat->at<float>(0, 0);

    for (int j = 0; j < mat->rows; j++)
    {
        if (mask->at<float>(j, 0) <= MASK_MIN)
            continue;

        for (int i = 0; i < mat->cols; i++)
        {
            float value = mat->at<float>(j, i);

            if (value < *min)
                *min = value;
            if (value > *max)
                *max = value;
        }
    }
}

/**
 * @brief Get the minimum and maximum value of a whole group
 * @param[in] directories
 * @param[out] minGrp
 * @param[out] maxGrp
 * @param[in] type
 */
void Merge::getMinMaxGroup(const QList<QString> *directories, float *minGrp, float *maxGrp, const QString filename) throw(MergeException)
{
    for(QString dir : *(directories))
    {
        string filepath = dir.toStdString() + "/" + filename.toStdString() + ".txt";
        QList<Mat*>* listImg = FileManager::importMapWithMask(filepath);

        Mat* img = listImg->at(0);
        Mat* mask = listImg->at(1);

        float newMin, newMax;
        getMinMaxMat(img, mask, &newMin, &newMax);

        cout << "new:" << newMin << " " << newMax << endl;

        *minGrp = min(newMin, *minGrp);
        *maxGrp = max(newMax, *maxGrp);

        delete listImg;
    }
}

/**
 * @brief Generate a colored map using a gradient color image.
 * @details
 *  This function simply call mapToColorGradient() with a mask
 *  set to maximum values.
 * @param[in] mapRaw the map's raw data
 * @param[in] gradient the gradient color image
 * @return the colored map
 */
Mat Merge::mapToColorGradient(const Mat* mapRaw, const Mat* gradient)
{
    Mat mask(mapRaw->rows, mapRaw->cols, CV_8UC3, Scalar(255));

    return mapToColorGradientWithMask(mapRaw, gradient, &mask);
}

/**
 * @brief Generate a colored map using gradient and mask.
 * @details
 *  For each position where the value of the mask is <= 1,
 *  the corresponding result image pixel will be set to 0.
 * @param[in] mapRaw the map's raw data
 * @param[in] gradient the gradient color image
 * @param[in] mask the mask to apply
 * @return the colored map
 */
Mat Merge::mapToColorGradientWithMask(const Mat* mapRaw, const Mat* gradient, const Mat* mask)
{
    Mat mapColor(mapRaw->rows, mapRaw->cols, CV_8UC3);

    Mat normalizedMap;
    normalizeMat(*(mapRaw), &normalizedMap, mask, 0.0, 1.0);

    cout << "mapToColorGradientWithMask" << endl;
    for (int j = 0; j < normalizedMap.rows; j++)
    {
        for (int i = 0; i < normalizedMap.cols; i++)
        {
            if(mask->at<float>(j, i) > 1)
            {
                int index = int(normalizedMap.at<float>(j, i) * 1000);
                if(index > 1000 || index < 0)
                {
                    cout << " Error index " << index;
                    break;
                }
                mapColor.at<Vec3b>(j, i)[0] = gradient->at<Vec3b>(index, 0)[0];
                mapColor.at<Vec3b>(j, i)[1] = gradient->at<Vec3b>(index, 0)[1];
                mapColor.at<Vec3b>(j, i)[2] = gradient->at<Vec3b>(index, 0)[2];
            }
            else
            {
                mapColor.at<Vec3b>(j, i)[0] = 0;
                mapColor.at<Vec3b>(j, i)[1] = 0;
                mapColor.at<Vec3b>(j, i)[2] = 0;
            }
        }
    }

    return mapColor;
}


/**
 * @brief Add caption text on image
 * @param[in/out] map
 */
void Merge::addCaption(Mat map)
{
    putText(map, "L", Point(0, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
    putText(map, "A", Point(90, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
    putText(map, "M", Point(180, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
    putText(map, "P", Point(270, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
    putText(map, "L", Point(350, 30), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0,0,0), 2.0);
}


/**
 * @brief Add a map to another
 * @param[out] sumMat the sum map
 * @param[out] weight keep track of the number of added values
 * @param[in] mat the map to add
 * @param[in] mask the mask of the map that defines values to ignored
 */
void Merge::sumMat(Mat* sumMat, Mat* weight, const Mat* mat, const Mat* mask)
{
    for(int i = 0; i < mat->cols; i++)
    {
        for(int j = 0; j < mat->rows; j++)
        {
            float value = mat->at<float>(j, i);

            if(mask->at<float>(j, i) > 0)
            {
                sumMat->at<float>(j, i) += value;
                weight->at<float>(j, i) += 1.0;
            }
        }
    }
}

/**
 * @brief Compute the mean map
 * @param[in] directories the list of map's files
 * @param[in] threshold the number of valid values necessary to calculate the mean
 * @param[in] type the map type
 * @param[in] norm set it to true if each map has to be normalized between 0 and 1
 * @return a list which contains the mean map and it's mask
 */
QList<Mat>* Merge::meanMat(const QList<QString>* directories, const int threshold, const EMapType type) throw(MergeException)
{
    cout << "MEAN" << endl;
    Mat mapMerge(300, 360, CV_32FC1, cvScalar(0.));
    Mat maskMerge(300, 360, CV_32FC1, Scalar(255));
    Mat weight(300, 360, CV_32FC1, Scalar(1));

    cout << "sum maps" << endl;
    for(int i = 0; i < directories->length(); i++)
    {
        QDir dir(directories->at(i));
        QDir dirMaps = QDir(dir.path() + "/maps");

        cout << dirMaps.absolutePath().toStdString() << endl;

        string filename = dirMaps.absolutePath().toStdString() + "/" + getMapFileName(type) + ".txt";
        QList<Mat*>* listImg = FileManager::importMapWithMask(filename);

        Mat* img = listImg->at(0);
        Mat* mask = listImg->at(1);

        sumMat(&mapMerge, &weight, img, mask);
    }

    cout << "merge maps" << endl;
    for(int i = 0; i < mapMerge.cols; i++)
    {
        for(int j = 0; j < mapMerge.rows; j++)
        {
            if(weight.at<float>(j, i) > threshold)
            {
                mapMerge.at<float>(j, i) /= (weight.at<float>(j, i)-1);
            }
            else
            {
                mapMerge.at<float>(j, i) = 0;
                maskMerge.at<float>(j, i) = 0;
            }
        }
    }

    cout << "return map and mask" << endl;
    QList<Mat>* mapMergeList = new QList<Mat>();
    mapMergeList->push_back(mapMerge);
    mapMergeList->push_back(maskMerge);

    return mapMergeList;
}

/**
 * @brief Compute the deviation map
 * @param[in] directories the list of map's files
 * @param[in] threshold the number of valid values necessary to calculate the mean
 * @param[in] type the map's type
 * @return a list which contains the deviation map and it's mask
 */
QList<Mat>* Merge::deviationMat(const QList<QString>* directories, const int threshold, const EMapType type) throw(MergeException)
{
    QList<Mat>* mapMergeList = meanMat(directories, threshold, type);
    Mat mapMerge = mapMergeList->at(0);
    Mat weight = mapMergeList->at(1);

    Mat mapDeviate = Mat(300, 360, CV_32FC1, cvScalar(0.));

    for(int n = 0; n < directories->length(); n++)
    {
        QDir dir(directories->at(n));
        QDir dirMaps = QDir(dir.path() + "/maps");

        string filename = dirMaps.absolutePath().toStdString() + "/" + getMapFileName(type) + ".txt";
        QList<Mat*>* listImg = FileManager::importMapWithMask(filename);

        Mat* img = listImg->at(0);
        //Mat* mask = listImg->at(1);

        for(int i = 0; i < mapMerge.cols; i++)
        {
            for(int j = 0; j < mapMerge.rows; j++)
            {
                if(img->at<float>(j, i) != 0.0){
                    mapDeviate.at<float>(j, i) += pow(img->at<float>(j, i) - mapMerge.at<float>(j, i), 2);
                }
            }
        }

        delete img;
    }

    for(int i = 0; i < mapDeviate.cols; i++)
    {
        for(int j = 0; j < mapDeviate.rows; j++)
        {
            if(weight.at<float>(j, i) > 1)
            {
                mapDeviate.at<float>(j, i) /= weight.at<float>(j, i);
                mapDeviate.at<float>(j, i) = sqrtf(mapDeviate.at<float>(j, i));
            }
        }
    }

    QList<Mat>* mapDeviateList = new QList<Mat>();
    mapDeviateList->push_back(mapDeviate);
    mapDeviateList->push_back(weight);

    return mapDeviateList;
}


/**
 * @brief Launch merge with given options
 * @param[in] options
 * @return
 */
int Merge::launch(SMergeOptions *options) throw(MergeException)
{
    cout << "load gradient images" << endl;

    cout << "retreive options" << endl;
    QList<QString>* dirList = options->dirList;
    QString targetDir = options->targetDir;
    QList<EMapType>* mapTypes = options->mapTypes;

    for (EMapType type : *(mapTypes))
    {
        cout << getMapName(type) << endl;
        cout << "compute ";
        if (options->bAverage)
        {
            cout << "average" << endl;
            QList<Mat>* mapMergeList;

            mapMergeList = meanMat(dirList, options->threshold, type);

            Mat mapMergeTest = mapMergeList->at(0);
            Mat weightTest = mapMergeList->at(1);

            exportMapColor(mapMergeTest, weightTest, targetDir, type, options->bBlur, options->bCaption, EXTENSION_AVERAGE);

            exportMapNormalized(mapMergeTest, weightTest, targetDir, type, 0.0, 1.0, EXTENSION_AVERAGE);
        }
        if (options->bDeviation)
        {
            cout << "deviation" << endl;
            QList<Mat>* mapMergeList;

            mapMergeList = deviationMat(dirList, options->threshold, type);

            Mat mapMergeTest = mapMergeList->at(0);
            Mat weightTest = mapMergeList->at(1);

            exportMapColor(mapMergeTest, weightTest, targetDir, type, options->bBlur, options->bCaption, EXTENSION_DEVIATION);

            exportMapNormalized(mapMergeTest, weightTest, targetDir, type, 0.0, 1.0, EXTENSION_DEVIATION);
        }
    }

    return 0;
}

void Merge::launchRatio(const QList<QString> *dirList,
                        const QString targetDir,
                        const QList<EMapType> *mapTypes,
                        const bool section /* = false */,
                        const float secVal /* = 0 */)
{
    QList<QList<QList<float>>> listRes1;
    QList<QList<QList<float>>> listRes2;

    for(EMapType type : (*mapTypes))
    {
        QList<QList<float>> lines1;
        QList<QList<float>> lines2;

        QList<QList<float>> res = computeRatio(dirList, type, section, secVal);

        for(int i = 0; i < res.length()-1; i += 2)
        {
            QList<float> val1 = res.at(i);
            QList<float> val2 = res.at(i+1);

            QList<float> line1;
            QList<float> line2;

            for(int j = 0; j < val1.length(); ++j)
            {
                if(val1.at(j) && val2.at(j))
                {
                    float resu = val1.at(j) / val2.at(j);
                    float resu2 = val2.at(j) / val1.at(j);
                    line1.append(resu);
                    line2.append(resu2);
                }
                else
                {
                    line1.append(0);
                    line2.append(0);
                }
            }
            lines1.append(line1);
            lines2.append(line2);
        }

        listRes1.append(lines1);
        listRes2.append(lines2);
    }

    cout <<" ... " << listRes1.length() << endl;
    cout << " ... "<< listRes1.at(0).length() << endl;

    if (section)
    {
        //saveRatioSection(lres,dirList_Tab->length()/2);
        //saveRatioSection(lres2,dirList_Tab->length()/2,false);
    }
    else
    {
        //saveRatio(lres,dirList_Tab->length()/2);
        //saveRatio(lres2,dirList_Tab->length()/2,false);
    }
}

QList<QList<float>> Merge::computeRatio(const QList<QString>* dirList,
                                        const EMapType type,
                                        const bool section /* = false */,
                                        const float secVal /* = 0 */)
{
    QList<QList<float>> res;

    for(int i = 0; i < dirList->length(); ++i)
    {
        QDir dir(dirList->at(i));
        QDir dirMaps = QDir(dir.path() + "/maps");

        string filename = dirMaps.absolutePath().toStdString() + "/" + getMapFileName(type) + ".txt";
        QList<Mat*>* listImg = FileManager::importMapWithMask(filename);

        Mat* img = listImg->at(0);

        QList<float> val;

        int index = int(((secVal / 100.0) * img->rows) + 0.5);

        for(int i = 0; i < img->cols; ++i)
        {
            if(!section)
            {
                for(int j = 0; j < img->rows; ++j)
                {
                    val.append(img->at<float>(j, i));
                }
            }
            else
            {
                val.append(img->at<float>(index, i));
            }
        }

        res.append(val);
        delete img;
    }

    return res;
}

/**
 * @brief Export a map's raw data and the colored map associated using the corresponding gradient
 * @param[in/out] map
 * @param[in] mask
 * @param[in] targetDir destination folder
 * @param[in] type define the gradient to use
 * @param[in] blured blur the image
 * @param[in] caption integrate caption
 * @param[in] ext the string to add to the output filename
 */
void Merge::exportMapColor(Mat map,
                           const Mat mask,
                           const QString targetDir,
                           const EMapType type,
                           const bool blured,
                           const bool caption,
                           string ext)
    throw(MergeException)
{
    cout << "export image result" << endl;
    string outputName = targetDir.toStdString() + "/" + getMapFileName(type) + ext;

    FileManager::exportMap(map, outputName);

    Mat mapColor;

    if (blured)
        blur(map, map, Size(1,25));

    if(getIdFromType(type)==0 || getIdFromType(type)==1 || getIdFromType(type)==3)
    {
        mapColor = mapToColorGradientWithMask(&map, &gradient, &mask);
    }
    else if(getIdFromType(type)==2)
    {
        mapColor = mapToColorGradientWithMask(&map, &gradientReverse, &mask);
    }
    else if(getIdFromType(type)==4)
    {
        mapColor = mapToColorGradientWithMask(&map, &gradient2, &mask);
    }

    if (caption)
        addCaption(mapColor);

    outputName = targetDir.toStdString() + "/" + getMapName(type) + ext;

    imshow(outputName, mapColor);
    imwrite(outputName + ".png", mapColor);
}

/**
 * @brief Perform a minmax normalization and esport the result map
 * @param map
 * @param mask
 * @param targetDir the destination folder
 * @param type the map's type
 * @param min the new min
 * @param max the new max
 */
void Merge::exportMapNormalized(Mat map,
                                const Mat mask,
                                const QString targetDir,
                                const EMapType type,
                                const float min,
                                const float max,
                                string ext)
    throw (MergeException)
{
    string outputName = targetDir.toStdString() + "/" + getMapFileName(type) + ext + EXTENSION_NORM;

    normalizeMat(map, &map, &mask, min, max);

    FileManager::exportMap(map, outputName);
}


/**
 * @brief Get files with similar name which are present in all given directories
 * @param directories the list of directories
 * @return the list of similar files names
 */
QList<QString> Merge::getSimilarFilenames(const QList<QString> *directories)
{
    QList<QString> filenames;

    QStringList nameFilter;
    nameFilter << "map*.txt";

    QDir dir(directories->at(0));
    QFileInfoList files = dir.entryInfoList(nameFilter, QDir::Files);
    for (QFileInfo fileInfo : files)
    {
        QString filename = fileInfo.baseName();

        if (!filename.contains(QString::fromStdString(EXTENSION_NORM)))
        {
            filenames.append(filename);
        }
    }

    for (QString directory : *(directories))
    {
        dir = QDir(directory);

        files = dir.entryInfoList(nameFilter, QDir::Files);

        QList<QString> nextFiles;
        for (QFileInfo fileInfo : files)
        {
             nextFiles.append(fileInfo.baseName());
        }

        QSet<QString> subtraction = filenames.toSet().subtract(nextFiles.toSet());

        for (QString filename : subtraction)
        {
            filenames = filenames.toSet().subtract(subtraction).toList();
        }
    }

    return filenames;
}


/**
 * @brief Normalize all maps from different analysis
 * @param directories the list of analysis directories
 * @param saveDir the path where result maps will be saved
 */
void Merge::normalizeGroup(const QList<QString> *directories, const QString saveDir) throw(MergeException)
{
    QList<QString> filenames = getSimilarFilenames(directories);

    for (QString filename : filenames)
    {
        // TODO
        float minGrp, maxGrp;
        getMinMaxGroup(directories, &minGrp, &maxGrp, filename);

        cout << minGrp << " " << maxGrp << endl;

        for(QString dir : *(directories))
        {
            string filepath = dir.toStdString() + "/" + filename.toStdString() + ".txt";
            QList<Mat*>* listImg = FileManager::importMapWithMask(filepath);

            Mat* img = listImg->at(0);
            Mat* mask = listImg->at(1);

            Mat normalizedMap;
            normalizeMat(*(img), &normalizedMap, mask, minGrp, maxGrp);

            string outputName = saveDir.toStdString() + "/" + (dir.split("/").last()).toStdString() + "_" + filename.toStdString();
            FileManager::exportMap(normalizedMap, outputName);

            Mat mapColor = mapToColorGradientWithMask(&normalizedMap, &gradient, mask);
            imwrite(outputName + ".png", mapColor);
        }
    }
}


/**
 * @brief Export the merge options as file
 * @param options
 */
void Merge::exportOptions(SMergeOptions *options) throw(MergeException)
{
    QList<QString>* data = new QList<QString>();

    for (QString dir : *(options->dirList))
    {
        data->append(dir + " ");
    }

    data->append("TARGET_DIR " + options->targetDir + " ");

    QString types = "MAP_TYPES ";
    for (EMapType type : *(options->mapTypes))
    {
        types += QString::number(getIdFromType(type)) + " ";
    }
    data->append(types);

    data->append("MEAN " + QString::number(options->bAverage) + " ");
    data->append("DEVIATION " + QString::number(options->bDeviation) + " ");
    data->append("MEDIAN " + QString::number(options->bMedian) + " ");

    data->append("STANDARDIZE " + QString::number(options->bStandardize) + " ");
    data->append("BLUR " + QString::number(options->bBlur) + " ");
    data->append("CAPTION " + QString::number(options->bCaption) + " ");
    data->append("THRESHOLD " + QString::number(options->threshold) + " ");

    string outputName = (options->targetDir).toStdString() + "/" + "options";

    FileManager::writeInFile(data, outputName);
}

/**
 * @brief Import the merge options from file
 * @param filepath
 * @return the merge options
 */
SMergeOptions *Merge::importOptions(const QString filepath) throw(MergeException)
{
    string filename = filepath.toStdString();
    QList<QStringList> data = FileManager::readFromFile(filename);

    cout << "IMPORT OPTIONS" << endl;

    SMergeOptions* options = new SMergeOptions();

    QList<QString>* dirList = new QList<QString>();
    QList<EMapType>* mapTypes = new QList<EMapType>();

    for (QStringList line : data)
    {
        QString key = QString(line.at(0));

        if (key == "MAP_TYPES")
        {
            line.takeFirst();
            line.takeLast();
            while (!line.isEmpty())
            {
                int id = QVariant(line.takeFirst()).toInt();
                mapTypes->append(getTypeFromId(id));
            }
        }
        else if (key == "TARGET_DIR")
        {
            options->targetDir = QString(line.at(1));
        }
        else if (key == "MEAN")
        {
            options->bAverage = QVariant(line.at(1)).toBool();
        }
        else if (key == "DEVIATION")
        {
            options->bDeviation = QVariant(line.at(1)).toBool();
        }
        else if (key == "MEDIAN")
        {
            options->bMedian = QVariant(line.at(1)).toBool();
        }
        else if (key == "STANDARDIZE")
        {
            options->bStandardize = QVariant(line.at(1)).toBool();
        }
        else if (key == "BLUR")
        {
            options->bBlur = QVariant(line.at(1)).toBool();
        }
        else if (key == "CAPTION")
        {
            options->bCaption = QVariant(line.at(1)).toBool();
        }
        else if (key == "THRESHOLD")
        {
            options->threshold = QVariant(line.at(1)).toInt();
        }
        else if (!key.isEmpty())
        {
            dirList->append(key);
        }
    }

    options->dirList = dirList;
    options->mapTypes = mapTypes;

    return options;
}


