#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QtSvg>
#include "listView/listedDataCore.h"

namespace Ui
{
class MainWindow;
}

enum EN_SUPPORTED_LANG {
    EN_LANG_DEFAULT = 0,
    EN_LANG_EN,
    EN_LANG_JA,

    EN_LANG_CENTINEL
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setArguments(QString defaultPath, QString locale);
    static QString getLocaleStr(EN_SUPPORTED_LANG);

public slots:
    void findLibrary();
    void loadLibrary();

    void findImage();
    void loadImage();
    void loadImage(QString image);
    void loadImage(QStringList images);

    void finishEdit();
    void showItem(QTableWidgetItem* );
    void remove();

protected:
    void dragEnterEvent(QDragEnterEvent* e);
    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* e);

private:
    int  loadLibData();
    int  createLibTable();
    void applyToData();
    void insertTableRow(ListedDataCore* data);

    void reset();

private:
    Ui::MainWindow* ui;
    QSvgWidget*     mSvgWid;
    QPushButton*    mRemoveBtn;

    QList<ListedDataCore*>  mData;
    QString         mLocale;

};

#endif // MAINWINDOW_H
