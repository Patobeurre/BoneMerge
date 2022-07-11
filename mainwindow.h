#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "merge.h"
#include "mergeexception.h"

#include <QMainWindow>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QStringListModel>
#include <QFileDialog>
#include <QTreeView>
#include <QList>

using namespace MergeUtils;


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    Merge merge;

    int searchFor(const QString str, const QList<QString>* listStr);
    void displayErrorBox(const QString title, const QString msg, const QString details);
    void displayWarningBox(const QString title, const QString msg, const QString details);

    /**
     * TAB MERGE
     */
    QFileSystemModel* dirModel;
    QStringListModel* model;

    QList<QString>* dirList;
    QString saveDir;
    QList<EMapType>* mapTypes;
    bool bMedian;
    bool bAverage;
    bool bDeviation;
    bool bStandardize;
    bool bBlur;
    bool bCaption;
    int threshold;

    void updateThresholdMax();
    void updateMapTypes(EMapType type, bool toAdd);
    void addSelectedFolder(QString dirPath);

    bool isLaunchEnable();
    bool isValidSelectedFolder(QString dirPath);

    SMergeOptions* generateOptions();
    void loadOptions(SMergeOptions *options);
    void updateUIFromOptions(SMergeOptions* options);

    /**
     * TAB NORMALIZE
     */
    QStringListModel* modelNorm;

    QList<QString>* dirListNorm;
    QString saveDirNorm;

    void addSelectedFolderNorm(QString dirPath);

    bool isLaunchNormEnable();
    bool isValidSelectedFolderNorm(QString dirPath);

    /**
     * TAB RATIO
     */
    QStandardItemModel* tabModelRatio;

    QList<EMapType>* mapTypesRatio;
    QList<QString>* dirListRatio;
    QString saveDirRatio;
    QString filenameRatio;
    bool section;
    float sectionValue;

    bool isLaunchRatioEnable();
    bool isValidSelectedFolderRatio(QString dirPath);

    void updateMapTypesRatio(EMapType type, bool toAdd);


private slots:
    /**
     * UI TAB MERGE
     */
    void on_btnAddFolder_clicked();
    void on_btnRemoveFolder_clicked();
    void on_btnBrowseSaveDir_clicked();
    void on_cbAverage_toggled(bool checked);
    void on_cbDeviation_toggled(bool checked);
    void on_cbMedian_toggled(bool checked);
    void on_cbStandardize_toggled(bool checked);
    void on_lineEditSaveDir_editingFinished();
    void on_cbExtRadius_toggled(bool checked);
    void on_cbCorticalThick_toggled(bool checked);
    void on_cbExtCurv_toggled(bool checked);
    void on_cbSecondMomentArea_toggled(bool checked);
    void on_cbModulus_toggled(bool checked);
    void on_btnLaunch_clicked();
    void on_sbThreshold_valueChanged(const QString &arg1);
    void on_cbBlur_toggled(bool checked);
    void on_cbCaption_toggled(bool checked);

    /* TEST BTN */
    void on_btnLoadOptions_clicked();
    void on_btnTEST_clicked();

    /**
     * UI TAB NORMALIZE
     */
    void on_btnAddFolderNorm_clicked();
    void on_btnRemoveFolderNorm_clicked();
    void on_btnBrowseSaveDirNorm_clicked();
    void on_btnLaunchNorm_clicked();

    /**
     * UI TAB RATIO
     */
    void on_cbSection_ratio_toggled(bool checked);
    void on_sliderSection_ratio_sliderMoved(int position);
    void on_sbSection_ratio_editingFinished();
    void on_cbExtRadius_ratio_toggled(bool checked);
    void on_cbCorticalThick_ratio_toggled(bool checked);
    void on_cbExtCurv_ratio_toggled(bool checked);
    void on_cbSecondMomentArea_ratio_toggled(bool checked);
    void on_cbModulus_ratio_toggled(bool checked);
    void on_lineEditFilename_ratio_editingFinished();
    void on_lineEditSaveDir_ratio_editingFinished();
    void on_btnBrowseSaveDir_ratio_clicked();
    void on_btnAddSelectedFolder_ratio_clicked();
    void on_btnRemoveSelectedFolder_ratio_clicked();
    void on_btnLaunchRatio_clicked();
};

#endif // MAINWINDOW_H
