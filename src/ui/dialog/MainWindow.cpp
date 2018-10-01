#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "common/common.h"
#include "listView/libraryListedData.h"

#include <QtCore>
#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mSvgWid = new QSvgWidget(NULL);
    mSvgWid->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    ui->GraphicFrame->layout()->addWidget(mSvgWid);

    connect(ui->FindLibraryButton,  SIGNAL(clicked()), this, SLOT(findLibrary()));
    connect(ui->LoadLibraryButton,  SIGNAL(clicked()), this, SLOT(loadLibrary()));
    connect(ui->FindImageButton,    SIGNAL(clicked()), this, SLOT(findImage()));
    connect(ui->LoadImageButton,    SIGNAL(clicked()), this, SLOT(loadImage()));
    connect(ui->FinishEdit,         SIGNAL(clicked()), this, SLOT(finishEdit()));
    connect(ui->tableWidget,        SIGNAL(itemClicked(QTableWidgetItem*)), this, SLOT(showItem(QTableWidgetItem*)));
    connect(ui->RemoveButton,       SIGNAL(clicked()), this, SLOT(remove()));

    acceptDrops();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setArguments(QString defaultPath, QString locale)
{
    QString name, attribute;
    QStringList headerStr;
    mLocale = "";
    headerStr << "name" << "attribute"; //default

    if(locale == "") {
        for(int i = EN_LANG_EN; i < EN_LANG_CENTINEL; i++ ) {
            QString localeStr = getLocaleStr((EN_SUPPORTED_LANG)i);
            if(localeStr == "") { continue; }
            name = "name_" + localeStr;
            attribute = "attribute_" + localeStr;
            headerStr << name << attribute;
        }
    } else {
        mLocale = locale;
        name = "name_" + mLocale;
        attribute = "attribute_" + mLocale;
        headerStr << name << attribute;
    }

    QTableWidget* table = ui->tableWidget;
    table->setRowCount(0);
    table->setColumnCount(headerStr.count());
    table->setHorizontalHeaderLabels(headerStr);

    if(defaultPath != "") {
        ui->LibraryPath->setText(defaultPath);
        loadLibrary();
        ui->frame_2->hide();
    }
}

QString MainWindow::getLocaleStr(EN_SUPPORTED_LANG lang)
{
    switch(lang) {
    case EN_LANG_EN:
        return "en";
    case EN_LANG_JA:
        return "ja";
    default:
        return "";
    }
}

void MainWindow::findLibrary()
{
    QString file = QFileDialog::getOpenFileName(NULL, tr("Open Library File"), ".",
                   "library (*.json *.fdsi)");
    ui->LibraryPath->setText(file);
}

void MainWindow::loadLibrary()
{
    reset();
    int ret = loadLibData();
    if(ret != SUCCESS) {
        int ret = QMessageBox::question(this, tr("info"), tr("Cannot open Library.\nCreate new Librarys?"), (QMessageBox::Yes | QMessageBox::No) );
        if(ret == QMessageBox::No) {
            return ;
        } else {
            QString libPath = QFileDialog::getSaveFileName(NULL, tr("Open Image Files"), ".",
                              "library (*.json *.fdsi)");
            ui->LibraryPath->setText(libPath);
        }
    }

    ret = createLibTable();
}

int MainWindow::loadLibData()
{
    QString path = ui->LibraryPath->text();
    QFile file(path);

    if(file.open(QIODevice::ReadWrite) == false) {
        return ERR_PROJ_LOAD_FILE_OPEN;
    }

    QByteArray data = file.readAll();
    file.close();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject json = doc.object();
    QJsonArray imgArray = json[JSON_TAG_IMAGE].toArray();

    foreach (QJsonValue imgVal, imgArray) {
        QJsonObject imgObj = imgVal.toObject();
        QMap<QString, QString> nameList;
        QMap<QString, QString> tagList;

        QString name = imgObj[JSON_TAG_IMG_NAME].toString("");
        QString tag  = imgObj[JSON_TAG_ATTRIBUTION].toString("");
        nameList.insert("default", name);
        tagList.insert("default", tag);

        for(int i = EN_LANG_EN; i < EN_LANG_CENTINEL; i++) {
            QString locale = getLocaleStr((EN_SUPPORTED_LANG)i);
            if(locale == "") { continue; }

            name = imgObj[JSON_TAG_IMG_NAME + "_" + locale].toString("");
            tag  = imgObj[JSON_TAG_ATTRIBUTION + "_" + locale].toString("");
            nameList.insert(locale, name);
            tagList.insert(locale, tag);
        }

        LibraryListedData* listedData = new LibraryListedData(nameList, tagList);
        listedData->setData(imgObj);
        mData.append(listedData);

    }

    return SUCCESS;
}

int  MainWindow::createLibTable()
{
    QTableWidget* table = ui->tableWidget;

    if(mData.count() == 0) {
        return SUCCESS;
    }

    foreach (ListedDataCore* data, mData) {
        LibraryListedData* libData = ((LibraryListedData*)data);
        insertTableRow(libData);
    }

    return SUCCESS;
}

void MainWindow::findImage()
{
    QString file = QFileDialog::getOpenFileName(NULL, tr("Open Image File"), ".",
                   "images (*.png *.jpeg *.jpg *.bmp *.png *.gif *.tiff *.svg)");
    ui->ImagePath->setText(file);
}

void MainWindow::loadImage()
{
    QString path = ui->ImagePath->text();
    QString name = path.split("/").last();
    name = name.split(".").first();

    QMap<QString, QString> nameList;
    QMap<QString, QString> tagList;
    nameList.insert("defalut", name);
    if(mLocale != "") { nameList.insert(mLocale, name); }

    LibraryListedData* newData = new LibraryListedData(nameList, tagList);
    bool ret = newData->setPath(path);
    if(ret == true) {
        mData.append(newData);
        insertTableRow(newData);
    } else {
        QMessageBox::information(this, "info", tr("Cannot Read File.\nPlease Confirm FilePath And FileData."));
    }
}

void MainWindow::loadImage(QString image)
{
    QString name = image.split("/").last();
    name = name.split(".").first();

    QMap<QString, QString> nameList;
    QMap<QString, QString> tagList;
    nameList.insert("defalut", name);
    if(mLocale != "") { nameList.insert(mLocale, name); }

    LibraryListedData* newData = new LibraryListedData(nameList, tagList);
    bool ret = newData->setPath(image);
    if(ret == true) {
        mData.append(newData);
        insertTableRow(newData);
    } else {
        QMessageBox::information(this, "info", tr("Cannot Read File.\nPlease Confirm FilePath And FileData."));
    }
}

void MainWindow::loadImage(QStringList images)
{
    foreach(QString image, images) {
        loadImage(image);
    }
}

void MainWindow::finishEdit()
{
    QString path = ui->LibraryPath->text();

    QFileInfo fInfo(path);
    if(fInfo.exists() == false) {
        path = QFileDialog::getSaveFileName(NULL, tr("Open Image Files"), ".",
                                            "library (*.json *.fdsi)");
        ui->LibraryPath->setText(path);
    }

    QFile file(path);
    if(file.open(QIODevice::WriteOnly) == false) {
        WARNING_STRING("ERR_PROJ_SAVE_FILE_OPEN_STR");
        return ;
    }
    applyToData();

    QJsonObject body;
    body[JSON_TAG_FILE_TYPE] = FILE_TYPE_LIBRARY;

    QJsonArray dataArray;
    foreach (ListedDataCore* data, mData) {
        LibraryListedData* libData = ((LibraryListedData*)data);
        QJsonObject obj;
        obj[JSON_TAG_IMG_NAME] = libData->name("default");
        obj[JSON_TAG_ATTRIBUTION] = libData->tag("default");
        for(int i = EN_LANG_EN; i < EN_LANG_CENTINEL; i++) {
            QString locale = getLocaleStr((EN_SUPPORTED_LANG)i);
            if(locale == "") { continue; }

            obj[JSON_TAG_IMG_NAME + "_" + locale] = libData->name(locale);
            obj[JSON_TAG_ATTRIBUTION + "_" + locale] = libData->tag(locale);
        }
        QJsonObject parseObj;
        libData->parser()->writeDown(parseObj);
        obj[JSON_TAG_IMG_DATA] = parseObj;
        dataArray.append(obj);
    }
    body[JSON_TAG_IMAGE] = dataArray;

    QJsonDocument doc(body);
    file.write(doc.toJson());

    file.close();

    close();
}

void MainWindow::showItem(QTableWidgetItem* selected)
{
    int row = selected->row();
    if(row >= mData.count()) { return; }

    LibraryListedData* data = ((LibraryListedData*)mData.at(row));
    mSvgWid->load(data->parser()->toByte());
}

void MainWindow::remove()
{
    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
    if(items.count() == 0) {
        return;
    }

    QList<int> rows;
    foreach (QTableWidgetItem* item, items) {
        int row = item->row();
        if(rows.contains(row) == false)
        { rows.append(row); }
    }

    qSort(rows.begin(), rows.end());
    while(rows.empty() == false) {
        int row = rows.takeLast();
        mData.removeAt(row);
        ui->tableWidget->removeRow(row);
    }

    mSvgWid->load(QString());
}

void MainWindow::applyToData()
{
    QTableWidget* table = ui->tableWidget;
    for(int i = 0; i < table->rowCount(); i++) {
        QString name = table->item(i, 0)->text();
        QString tag = table->item(i, 1)->text();
        mData.at(i)->setNameAndTag("default", name, tag);
        if(mLocale != "") {
            name = table->item(i, 2)->text();
            tag  = table->item(i, 3)->text();
            mData.at(i)->setNameAndTag(mLocale, name, tag);
            break;
        }
        for(int j = EN_LANG_EN; j < EN_LANG_CENTINEL; j++) {
            QString locale = getLocaleStr((EN_SUPPORTED_LANG)j);
            if(locale == "") { continue; }
            name = table->item(i, j * 2)->text();
            tag  = table->item(i, j * 2 + 1)->text();
            mData.at(i)->setNameAndTag(locale, name, tag);
        }
    }
}

void MainWindow::insertTableRow(ListedDataCore* data)
{
    QTableWidget* table = ui->tableWidget;
    int rowCount = table->rowCount();
    table->insertRow(rowCount);
    QTableWidgetItem* item = new QTableWidgetItem(data->name("default"));
    table->setItem(rowCount, 0, item);
    item = new QTableWidgetItem(data->tag("default"));
    table->setItem(rowCount, 1, item);
    if(mLocale != "") {
        item = new QTableWidgetItem(data->name(mLocale));
        table->setItem(rowCount, 2, item);
        item = new QTableWidgetItem(data->tag(mLocale));
        table->setItem(rowCount, 3, item);
        return ;
    }

    for(int j = EN_LANG_EN; j < EN_LANG_CENTINEL; j++) {
        QString locale = getLocaleStr((EN_SUPPORTED_LANG)j);
        if(locale == "") { continue; }

        item = new QTableWidgetItem(data->name(locale));
        table->setItem(rowCount, j * 2, item);
        item = new QTableWidgetItem(data->tag(locale));
        table->setItem(rowCount, j * 2 + 1, item);
    }
}

void MainWindow::reset()
{
    mData.clear();
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(0);
}

void MainWindow::dragEnterEvent(QDragEnterEvent* e)
{
    if(e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dragMoveEvent(QDragMoveEvent *e)
{

}

void MainWindow::dropEvent(QDropEvent* e)
{
    QStringList fileList;
    foreach (const QUrl& url, e->mimeData()->urls()) {
        fileList.append( url.toLocalFile() );
    }

    loadImage(fileList);
}
