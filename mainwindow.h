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

    /// the merge object
    Merge merge;

    /// Search for a string in a given list
    int searchFor(const QString str, const QList<QString>* listStr);

    /// Display an error message box
    void displayErrorBox(const QString title, const QString msg, const QString details);
    /// Display a warning message box
    void displayWarningBox(const QString title, const QString msg, const QString details);
    /// Display an information message box
    void displayInformationBox(const QString title, const QString msg, const QString details);

    /**
     * TAB MERGE
     */
    /// model for folder selection's list
    QFileSystemModel* dirModel;
    /// model for the list of selected folders
    QStringListModel* model;

    /// list of selected folders
    QList<QString>* dirList;
    /// target directory
    QString saveDir;
    /// list of types
    QList<EMapType>* mapTypes;

    /// parameters
    bool bMedian;
    bool bAverage;
    bool bDeviation;
    bool bStandardize;
    bool bBlur;
    bool bCaption;
    int threshold;

    /// Update the maximum value for the threshold
    void updateThresholdMax();
    /// Update the list of types
    void updateMapTypes(EMapType type, bool toAdd);
    /// Add a folder to the list
    void addSelectedFolder(QString dirPath);

    /// Check if launch can be performed
    bool isLaunchEnable();
    /// Check if a selected folder path is valid for merging
    bool isValidSelectedFolder(QString dirPath);

    /// Generate options with current settings
    SMergeOptions* generateOptions();
    /// Set parameters according to the given options
    void loadOptions(SMergeOptions *options);
    /// Update UI according to the given options
    void updateUIFromOptions(SMergeOptions* options);

    /**
     * TAB NORMALIZE
     */
    /// model for the list of selected folders
    QStringListModel* modelNorm;

    /// list of selected folders
    QList<QString>* dirListNorm;
    /// target directory
    QString saveDirNorm;

    /// Add a folder path to the list of selected folders
    void addSelectedFolderNorm(QString dirPath);

    /// Check if launch can be performed
    bool isLaunchNormEnable();
    /// Check if a selected folder path is valid for normalization
    bool isValidSelectedFolderNorm(QString dirPath);

    /**
     * TAB RATIO
     */
    /// model for the list of selected folders
    QStandardItemModel* tabModelRatio;

    /// list of types
    QList<EMapType>* mapTypesRatio;
    /// list of selected folders
    QList<QString>* dirListRatio;
    /// target directory
    QString saveDirRatio;
    /// output filename
    QString filenameRatio;
    /// define if section is used
    bool section;
    /// the value of the section
    float sectionValue;

    /// Check if launch can be performed
    bool isLaunchRatioEnable();
    /// Check if a selected folder path is valid for ratio process
    bool isValidSelectedFolderRatio(QString dirPath);

    /// Update the list of types
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
