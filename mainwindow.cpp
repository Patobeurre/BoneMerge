#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QMessageBox>


/**
 * @brief Constructor
 * @details
 * Initialize class variables and UI components
 * @param parent
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    dirList = new QList<QString>();
    mapTypes = new QList<EMapType>();
    saveDir = "";
    bAverage = bDeviation = bMedian = false;
    bStandardize = false;
    bBlur = bCaption = false;
    threshold = 1;

    dirListNorm = new QList<QString>();
    saveDirNorm = "";

    dirListRatio = new QList<QString>();
    mapTypesRatio = new QList<EMapType>();
    saveDirRatio = "";
    section = false;
    sectionValue = 0.0;

    try
    {
        merge = Merge();
        merge.loadGradientImages();
    }
    catch (MergeException &e)
    {
        displayErrorBox(QString::fromStdString(e.getTitle()),
                        QString::fromStdString(e.getMessage()),
                        QString::fromStdString(e.getDetails()));
    }

    dirModel = new QFileSystemModel(this);
    dirModel->setRootPath(QDir::rootPath());
    dirModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

    model = new QStringListModel(this);
    ui->listSelectedFolders->setModel(model);

    modelNorm = new QStringListModel(this);
    ui->listSelectedFoldersNorm->setModel(modelNorm);

    ui->treeSelectFolder->setModel(dirModel);
    ui->treeSelectFolder->setColumnHidden(1,true);
    ui->treeSelectFolder->setColumnHidden(2,true);
    ui->treeSelectFolder->setColumnHidden(3,true);
    ui->treeSelectFolder->setSelectionMode(QAbstractItemView::MultiSelection);

    ui->treeSelectFolderNorm->setModel(dirModel);
    ui->treeSelectFolderNorm->setColumnHidden(1,true);
    ui->treeSelectFolderNorm->setColumnHidden(2,true);
    ui->treeSelectFolderNorm->setColumnHidden(3,true);
    ui->treeSelectFolderNorm->setSelectionMode(QAbstractItemView::MultiSelection);

    tabModelRatio = new QStandardItemModel(0, 2, this);
    tabModelRatio->setHorizontalHeaderItem(0, new QStandardItem(QString("Femur")));
    tabModelRatio->setHorizontalHeaderItem(1, new QStandardItem(QString("Humerus")));
    ui->tableViewSelectFolders_ratio->setModel(tabModelRatio);

    ui->btnLaunch->setEnabled(false);
    ui->btnLaunchNorm->setEnabled(false);
    ui->btnLaunchRatio->setEnabled(false);

    ui->tabWidget->setCurrentIndex(0);

    updateThresholdMax();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**
 * @brief Display an error message window
 * @param[in] title the window's title
 * @param[in] msg the main message
 * @param[in] details the additionnal detailed message
 */
void MainWindow::displayErrorBox(const QString title, const QString msg, const QString details)
{
    QMessageBox msgBox;
    msgBox.critical(0, title, msg + "\n\n" + details);
}

/**
 * @brief Display a warning message window
 * @param[in] title the window's title
 * @param[in] msg the main message
 * @param[in] details the additionnal detailed message
 */
void MainWindow::displayWarningBox(const QString title, const QString msg, const QString details)
{
    QMessageBox msgBox;
    msgBox.warning(0, title, msg + "\n\n" + details);
}

/**
 * @brief Display an information message window
 * @param[in] title the window's title
 * @param[in] msg the main message
 * @param[in] details the additionnal detailed message
 */
void MainWindow::displayInformationBox(const QString title, const QString msg, const QString details)
{
    QMessageBox msgBox;
    msgBox.information(0, title, msg + "\n\n" + details);
}

/**
 * @brief Update the maximum value authorized for the threshold option
 * @details
 * The threshold value must be between 1 and the number of selected folders
 */
void MainWindow::updateThresholdMax()
{
    int max = (dirList->isEmpty()) ? 1 : dirList->length();

    ui->sbThreshold->setMaximum(max);
}


/**
 * @brief Search a QString whithin a list and return it's position index
 * @param[in] str the QString to search for
 * @param[in] listStr the list to search from
 * @return the position's index of the QString in the list, return -1 if not found
 */
int MainWindow::searchFor(const QString str, const QList<QString>* listStr)
{
    for(int i = 0; i < listStr->size(); i++)
    {
        if(listStr->at(i) == str)
        {
            return i;
        }
    }

    return -1;
}


/**
 * @brief Check if minimum requiered informations are set
 * @details
 * The minimum requiered informations are:
 *  - at least one folder is selected.
 *  - at least one operation is checked.
 *  - at least one map type is checked.
 *  - the save folder is set.
 * @return true if the conditions are valid, false otherwise
 */
bool MainWindow::isLaunchEnable()
{
    if (dirList->isEmpty()) return false;
    if (!bAverage && !bDeviation && !bMedian) return false;
    if (mapTypes->isEmpty()) return false;
    if (saveDir.isEmpty()) return false;

    return true;
}

/**
 * @brief Check if a selected folder path leads to a valid analysis
 * @details
 * A folder is considered valid if it contains:
 *  - two folders named "maps" and "series"
 *  - the "maps" folder contains at least one .png file
 * @param[in] dirPath the path to check
 * @return true if the folder is valid, false otherwise
 */
bool MainWindow::isValidSelectedFolder(QString dirPath)
{
    QDir dirMaps = QDir(dirPath + "/maps");
    if (!dirMaps.exists()) return false;

    QDir dirSerie = QDir(dirPath + "/serie");
    if (!dirSerie.exists()) return false;

    QStringList nameFilter;
    nameFilter << "*.png";

    QFileInfoList files = dirMaps.entryInfoList(nameFilter, QDir::Files);

    return (files.count() > 0);
}

/**
 * @brief Create and return a SMergeOptions using current parameter setting
 * @return the options
 */
SMergeOptions *MainWindow::generateOptions()
{
    SMergeOptions* options = new SMergeOptions();
    options->dirList = new QList<QString>(*dirList);
    options->mapTypes = new QList<EMapType>(*mapTypes);
    options->targetDir = saveDir;
    options->bAverage = bAverage;
    options->bDeviation = bDeviation;
    options->bMedian = bMedian;
    options->threshold = threshold;
    options->bStandardize = bStandardize;
    options->bBlur = bBlur;
    options->bCaption = bCaption;

    return options;
}

/**
 * @brief Copy given options data into the corresponding class variables
 * @details
 * Also update the UI to the new values.
 * @param[in] options
 */
void MainWindow::loadOptions(SMergeOptions *options)
{
    dirList = new QList<QString>(*(options->dirList));
    mapTypes = new QList<EMapType>(*(options->mapTypes));
    saveDir = options->targetDir;
    bAverage = options->bAverage;
    bDeviation = options->bDeviation;
    bMedian = options->bMedian;
    threshold = options->threshold;
    bStandardize = options->bStandardize;
    bBlur = options->bBlur;
    bCaption = options->bCaption;

    updateUIFromOptions(options);
}

/**
 * @brief Set the UI components states match the given options
 * @param[in] options
 */
void MainWindow::updateUIFromOptions(SMergeOptions *options)
{
    model = new QStringListModel(this);
    ui->listSelectedFolders->setModel(model);
    dirList = new QList<QString>();

    for (QString dir : *(options->dirList))
    {
        addSelectedFolder(dir);
    }

    ui->cbCorticalThick->setCheckState(Qt::CheckState::Unchecked);
    ui->cbExtRadius->setCheckState(Qt::CheckState::Unchecked);
    ui->cbExtCurv->setCheckState(Qt::CheckState::Unchecked);
    ui->cbModulus->setCheckState(Qt::CheckState::Unchecked);
    ui->cbSecondMomentArea->setCheckState(Qt::CheckState::Unchecked);

    for (EMapType type : *(options->mapTypes))
    {
        switch (type) {
        case EMapType::CORTICAL_THICK:
            ui->cbCorticalThick->setCheckState(Qt::CheckState::Checked);
            break;
        case EMapType::EXTERNAL_RADIUS:
            ui->cbExtRadius->setCheckState(Qt::CheckState::Checked);
            break;
        case EMapType::CURVATURE:
            ui->cbExtCurv->setCheckState(Qt::CheckState::Checked);
            break;
        case EMapType::MODULUS:
            ui->cbModulus->setCheckState(Qt::CheckState::Checked);
            break;
        case EMapType::MOMENT_AREA:
            ui->cbSecondMomentArea->setCheckState(Qt::CheckState::Checked);
            break;
        default:
            break;
        }
    }

    ui->lineEditSaveDir->setText(options->targetDir);

    ui->cbAverage->setChecked(options->bAverage);
    ui->cbDeviation->setChecked(options->bDeviation);
    ui->cbMedian->setChecked(options->bMedian);

    ui->cbStandardize->setChecked(options->bStandardize);
    ui->cbBlur->setChecked(options->bBlur);
    ui->cbCaption->setChecked(options->bCaption);
    ui->sbThreshold->setValue(options->threshold);

    ui->btnLaunch->setEnabled(isLaunchEnable());
}

/**
 * @brief Add a selected folder path to the list
 * @param[in] str
 */
void MainWindow::addSelectedFolderNorm(QString str)
{
    int id = searchFor(str, dirListNorm);
    if (id >= 0) return;

    modelNorm->insertRow(modelNorm->rowCount());
    QModelIndex index = modelNorm->index(modelNorm->rowCount() - 1);
    modelNorm->setData(index, str);

    dirListNorm->append(str);
}

/**
 * @brief Check if minimum requiered informations are set for normalization process
 * @details
 * The minimum requiered informations are:
 *  - at least one folder is selected.
 *  - the save folder is set.
 * @return true if the conditions are valid, false otherwise
 */
bool MainWindow::isLaunchNormEnable()
{
    if (dirListNorm->isEmpty()) return false;
    if (saveDirNorm.isEmpty()) return false;

    return true;
}

/**
 * @brief Check if a selected folder path for normalization leads to a valid analysis
 * @details
 * A folder is considered valid if it contains:
 *  - at least one .txt file
 * @param[in] dirPath the path to check
 * @return true if the folder is valid, false otherwise
 */
bool MainWindow::isValidSelectedFolderNorm(QString dirPath)
{
    QDir dir = QDir(dirPath);
    if (!dir.exists()) return false;

    QStringList nameFilter;
    nameFilter << "*.txt";

    QFileInfoList files = dir.entryInfoList(nameFilter, QDir::Files);

    return (files.count() > 0);
}

/**
 * @brief Add or remove a type from the map type list
 * @param[in] type the map type to update
 * @param[in] toAdd specify if the type has to be added (true) or removed (false)
 */
void MainWindow::updateMapTypes(EMapType type, bool toAdd)
{
    if (toAdd)
    {
        if (!mapTypes->contains(type))
            mapTypes->append(type);
    }
    else
    {
        int index = mapTypes->indexOf(type);
        if (index >= 0)
            mapTypes->removeAt(index);
    }

    ui->btnLaunch->setEnabled(isLaunchEnable());
}

/**
 * @brief Add a selected folder path to the list
 * @param[in] str
 */
void MainWindow::addSelectedFolder(QString str)
{
    int id = searchFor(str, dirList);
    if (id >= 0) return;

    model->insertRow(model->rowCount());
    QModelIndex index = model->index(model->rowCount() - 1);
    model->setData(index, str);

    dirList->append(str);

    updateThresholdMax();
}

/**
 * @brief Check if minimum requiered informations are set for ratio process
 * @details
 * The minimum requiered informations are:
 *  - at least one folder is selected.
 *  - at least one map type is checked.
 *  - the save folder is set.
 * @return true if the conditions are valid, false otherwise
 */
bool MainWindow::isLaunchRatioEnable()
{
    if (dirListRatio->isEmpty()) return false;
    if (mapTypesRatio->isEmpty()) return false;
    if (saveDirRatio.isEmpty()) return false;

    return true;
}

/**
 * @brief Check if a selected folder path for ratio leads to a valid analysis
 * @details
 * A folder is considered valid if it contains:
 *  - two folders named "maps"
 *  - the "maps" folder contains at least one .png file
 * @param[in] dirPath the path to check
 * @return true if the folder is valid, false otherwise
 */
bool MainWindow::isValidSelectedFolderRatio(QString dirPath)
{
    QDir dirMaps = QDir(dirPath + "/maps");
    if (!dirMaps.exists()) return false;

    QStringList nameFilter;
    nameFilter << "*.png";

    QFileInfoList files = dirMaps.entryInfoList(nameFilter, QDir::Files);

    return (files.count() > 0);
}

/**
 * @brief Add or remove a type from the map type list
 * @param[in] type the map type to update
 * @param[in] toAdd specify if the type has to be added (true) or removed (false)
 */
void MainWindow::updateMapTypesRatio(EMapType type, bool toAdd)
{
    if (toAdd)
    {
        if (!mapTypesRatio->contains(type))
            mapTypesRatio->append(type);
    }
    else
    {
        int index = mapTypesRatio->indexOf(type);
        if (index >= 0)
            mapTypesRatio->removeAt(index);
    }

    ui->btnLaunchRatio->setEnabled(isLaunchRatioEnable());
}


/**
 * SLOTS
 */


void MainWindow::on_btnLaunch_clicked()
{
    qDebug() << "LAUNCH" << endl;
    if (!isLaunchEnable()) return;

    qDebug() << "generate options" << endl;
    SMergeOptions* options = generateOptions();

    try
    {
        qDebug() << "export options" << endl;
        merge.exportOptions(options);
    }
    catch (MergeException &e)
    {
        displayWarningBox(QString::fromStdString(e.getTitle()),
                          QString::fromStdString(e.getMessage()),
                          QString::fromStdString(e.getDetails()));
        return;
    }

    try
    {
        qDebug() << "merge" << endl;
        merge.launch(options);
    }
    catch (MergeException &e)
    {
        displayErrorBox(QString::fromStdString(e.getTitle()),
                      QString::fromStdString(e.getMessage()),
                      QString::fromStdString(e.getDetails()));
        return;
    }

    displayInformationBox("Done", "Process finished successfuly!", "");
}


void MainWindow::on_btnAddFolder_clicked()
{
    QModelIndexList list = ui->treeSelectFolder->selectionModel()->selectedIndexes();

    for(int i = 0; i < list.length(); i += 4)
    {
        QString str = dirModel->filePath(list.at(i));

        if(isValidSelectedFolder(str))
        {
            addSelectedFolder(str);
        }
        else
        {
            //displayErrorBox(dir0.dirName(),"Missing Files <i>.png</i> in " + str);
            continue;
        }
    }

    updateThresholdMax();
    ui->treeSelectFolder->clearSelection();
    ui->btnLaunch->setEnabled(isLaunchEnable());
}


void MainWindow::on_btnRemoveFolder_clicked()
{
    QModelIndexList list = ui->listSelectedFolders->selectionModel()->selectedIndexes();

    for(int i = 0; i < list.length(); i++)
    {
        QString str = list.at(i).data(Qt::DisplayRole).toString();
        ui->listSelectedFolders->model()->removeRow(list.at(i).row());

        int index = searchFor(str, dirList);
        if(index >= 0)
        {
            dirList->removeAt(index);
        }
    }

    ui->btnLaunch->setEnabled(isLaunchEnable());
}

void MainWindow::on_btnBrowseSaveDir_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    QString dirName;
    dirName = dialog.getExistingDirectory();

    if(dirName == "") return;

    QDir dir = QDir(dirName);

    saveDir = dir.absolutePath();

    ui->lineEditSaveDir->setText(dir.absolutePath());
    ui->btnLaunch->setEnabled(isLaunchEnable());
}


void MainWindow::on_cbAverage_toggled(bool checked)
{
    bAverage = checked;

    ui->btnLaunch->setEnabled(isLaunchEnable());
}

void MainWindow::on_cbDeviation_toggled(bool checked)
{
    bDeviation = checked;

    ui->btnLaunch->setEnabled(isLaunchEnable());
}

void MainWindow::on_cbMedian_toggled(bool checked)
{
    bMedian = checked;

    ui->btnLaunch->setEnabled(isLaunchEnable());
}

void MainWindow::on_cbStandardize_toggled(bool checked)
{
    bStandardize = checked;
}

void MainWindow::on_lineEditSaveDir_editingFinished()
{
    saveDir = ui->lineEditSaveDir->text();
}

void MainWindow::on_cbExtRadius_toggled(bool checked)
{
    updateMapTypes(EMapType::EXTERNAL_RADIUS, checked);
}

void MainWindow::on_cbCorticalThick_toggled(bool checked)
{
    updateMapTypes(EMapType::CORTICAL_THICK, checked);
}

void MainWindow::on_cbExtCurv_toggled(bool checked)
{
    updateMapTypes(EMapType::CURVATURE, checked);
}

void MainWindow::on_cbSecondMomentArea_toggled(bool checked)
{
    updateMapTypes(EMapType::MOMENT_AREA, checked);
}

void MainWindow::on_cbModulus_toggled(bool checked)
{
    updateMapTypes(EMapType::MODULUS, checked);
}

void MainWindow::on_sbThreshold_valueChanged(const QString &value)
{
    threshold = value.toInt();
}

void MainWindow::on_cbBlur_toggled(bool checked)
{
    bBlur = checked;
}

void MainWindow::on_cbCaption_toggled(bool checked)
{
    bCaption = checked;
}

void MainWindow::on_btnLoadOptions_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    QString filepath;
    filepath = dialog.getOpenFileName();

    qDebug() << filepath << endl;

    if(filepath == "") return;

    loadOptions(merge.importOptions(filepath));

    ui->btnLaunch->setEnabled(isLaunchEnable());
}

void MainWindow::on_btnTEST_clicked()
{
    QString line = "C:/LV_47_FEM_R/LV_47 C:/LV_60_FEM_R/LV_60";
    QList<QString> splited = line.split(" ");
    QList<QString> files = merge.getSimilarFilenames(&splited);

    merge.normalizeGroup(&splited, "C:/norm_grp_TEST");

    foreach(auto &x, files)
        qDebug() << x;
}

/**
 * UI TAB NORMALIZE SLOTS
 */

void MainWindow::on_btnAddFolderNorm_clicked()
{
    QModelIndexList list = ui->treeSelectFolderNorm->selectionModel()->selectedIndexes();

    for(int i = 0; i < list.length(); i += 4)
    {
        QString str = dirModel->filePath(list.at(i));

        if(isValidSelectedFolderNorm(str))
        {
            addSelectedFolderNorm(str);
        }
        else
        {
            //displayErrorBox(dir0.dirName(),"Missing Files <i>.png</i> in " + str);
            continue;
        }
    }

    ui->treeSelectFolderNorm->clearSelection();
    ui->btnLaunchNorm->setEnabled(isLaunchNormEnable());
}

void MainWindow::on_btnRemoveFolderNorm_clicked()
{
    QModelIndexList list = ui->listSelectedFoldersNorm->selectionModel()->selectedIndexes();

    for(int i = 0; i < list.length(); i++)
    {
        QString str = list.at(i).data(Qt::DisplayRole).toString();
        ui->listSelectedFoldersNorm->model()->removeRow(list.at(i).row());

        int index = searchFor(str, dirListNorm);
        if(index >= 0)
        {
            dirListNorm->removeAt(index);
        }
    }

    ui->btnLaunchNorm->setEnabled(isLaunchNormEnable());
}

void MainWindow::on_btnBrowseSaveDirNorm_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    QString dirName;
    dirName = dialog.getExistingDirectory();

    if(dirName == "") return;

    QDir dir = QDir(dirName);

    saveDirNorm = dir.absolutePath();

    ui->lineEditSaveDirNorm->setText(dir.absolutePath());
    ui->btnLaunchNorm->setEnabled(isLaunchNormEnable());
}

void MainWindow::on_btnLaunchNorm_clicked()
{
    qDebug() << "LAUNCH" << endl;
    if (!isLaunchNormEnable()) return;

    try
    {
        qDebug() << "normalize" << endl;
        merge.normalizeGroup(dirListNorm, saveDirNorm);
    }
    catch (MergeException &e)
    {
        displayErrorBox(QString::fromStdString(e.getTitle()),
                        QString::fromStdString(e.getMessage()),
                        QString::fromStdString(e.getDetails()));
        return;
    }

    displayInformationBox("Done", "Process finished successfuly!", "");
}

/**
 * UI TAB RATIO SLOTS
 */

void MainWindow::on_cbSection_ratio_toggled(bool checked)
{
    section = checked;
    ui->sliderSection_ratio->setEnabled(checked);
    ui->sbSection_ratio->setEnabled(checked);
}

void MainWindow::on_sliderSection_ratio_sliderMoved(int position)
{
    sectionValue = (float) position;
    ui->sbSection_ratio->setValue(sectionValue);
}

void MainWindow::on_sbSection_ratio_editingFinished()
{
    sectionValue = (float) ui->sbSection_ratio->value();
    ui->sliderSection_ratio->setValue((int) sectionValue);
}

void MainWindow::on_cbExtRadius_ratio_toggled(bool checked)
{
    updateMapTypesRatio(EMapType::EXTERNAL_RADIUS, checked);
}

void MainWindow::on_cbCorticalThick_ratio_toggled(bool checked)
{
    updateMapTypesRatio(EMapType::CORTICAL_THICK, checked);
}

void MainWindow::on_cbExtCurv_ratio_toggled(bool checked)
{
    updateMapTypesRatio(EMapType::CURVATURE, checked);
}

void MainWindow::on_cbSecondMomentArea_ratio_toggled(bool checked)
{
    updateMapTypesRatio(EMapType::MOMENT_AREA, checked);
}

void MainWindow::on_cbModulus_ratio_toggled(bool checked)
{
    updateMapTypesRatio(EMapType::MODULUS, checked);
}

void MainWindow::on_lineEditFilename_ratio_editingFinished()
{
    filenameRatio = ui->lineEditFilename_ratio->text();
}

void MainWindow::on_lineEditSaveDir_ratio_editingFinished()
{
    saveDirRatio = ui->lineEditSaveDir_ratio->text();
}

void MainWindow::on_btnBrowseSaveDir_ratio_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    QString dirName;
    dirName = dialog.getExistingDirectory();

    if(dirName == "") return;

    QDir dir = QDir(dirName);

    saveDirRatio = dir.absolutePath();

    ui->lineEditSaveDir_ratio->setText(dir.absolutePath());
    ui->btnLaunchRatio->setEnabled(isLaunchRatioEnable());
}

void MainWindow::on_btnAddSelectedFolder_ratio_clicked()
{
    QFileDialog dialog;
    QList<QStandardItem*> list;

    dialog.setFileMode(QFileDialog::Directory);

    QString dirName;
    dirName = dialog.getExistingDirectory();

    if (!isValidSelectedFolderRatio(dirName))
    {
        displayErrorBox("Error",
                        "Missing Files <i>.png</i> missing.",
                        "At location: " + dirName);
        return;
    }

    QString dirName_2;
    dirName_2 = dialog.getExistingDirectory();

    if (!isValidSelectedFolderRatio(dirName_2))
    {
        displayErrorBox("Error",
                        "Missing Files <i>.png</i> missing.",
                        "At location: " + dirName_2);
        return;
    }

    QDir dir1(dirName);
    QDir dir2(dirName_2);

    list.append(new QStandardItem(dir1.dirName()));
    list.append(new QStandardItem(dir2.dirName()));

    tabModelRatio->appendRow(list);
    dirListRatio->append(dirName);
    dirListRatio->append(dirName_2);

    ui->btnLaunchRatio->setEnabled(isLaunchRatioEnable());
}

void MainWindow::on_btnRemoveSelectedFolder_ratio_clicked()
{
    QModelIndexList list = ui->tableViewSelectFolders_ratio->selectionModel()->selectedIndexes();

    for(int i = 0; i < list.length(); ++i)
    {
        ui->tableViewSelectFolders_ratio->model()->removeRow(list.at(i).row());
        dirListRatio->removeAt(list.at(i).row() * 2);
        dirListRatio->removeAt(list.at(i).row() * 2);
    }

    ui->btnLaunchRatio->setEnabled(isLaunchRatioEnable());
}

void MainWindow::on_btnLaunchRatio_clicked()
{
    if (!isLaunchRatioEnable())
    {
        return;
    }

    try
    {
        merge.launchRatio(dirListRatio, saveDirRatio, mapTypesRatio, section, sectionValue);
    }
    catch (MergeException &e)
    {
        displayErrorBox(QString::fromStdString(e.getTitle()),
                        QString::fromStdString(e.getMessage()),
                        QString::fromStdString(e.getDetails()));
        return;
    }

    displayInformationBox("Done", "Process finished successfuly!", "");
}
