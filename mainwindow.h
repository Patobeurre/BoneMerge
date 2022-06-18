#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "merge.h"

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

    int searchFor(const QString str, const QList<QString>* listStr);
    bool isLaunchEnable();
    bool isValidSelectedFolder(QString dirPath);

    SMergeOptions* generateOptions();
    void loadOptions(SMergeOptions *options);
    void updateUIFromOptions(SMergeOptions* options);

private slots:
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

    void on_btnLoadOptions_clicked();
    void on_btnTEST_clicked();
};

#endif // MAINWINDOW_H