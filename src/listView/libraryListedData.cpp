#include "libraryListedData.h"

#include <QIcon>
#include <QSvgRenderer>
#include <QPainter>

LibraryListedData::LibraryListedData(const QMap<QString, QString>& nameList, const QMap<QString, QString>& tagList):
    ListedDataCore(nameList, tagList)
{
    mParser = new SvgParser();
}

LibraryListedData::LibraryListedData():
    ListedDataCore()
{
    mParser = NULL;
    //　基本的には使わない
    //　Q_DECLARE_METATYPE宣言に必要なコンストラクタ
}

LibraryListedData::~LibraryListedData()
{
    if(mParser != NULL)
    { delete mParser; }
}


void LibraryListedData::setData(const QJsonObject& json)
{
    mParser->readData(json[JSON_TAG_IMG_DATA].toObject());
    QSvgRenderer render(mParser->toByte());
    QPixmap image(mParser->outerSize().toSize());
    image.fill(0xaaA08080);

    QPainter painter(&image);
    render.render(&painter);

    mPixmap = image.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

bool LibraryListedData::setPath(QString path)
{
    return mParser->loadFile(path);
}
