#include "listedDataCore.h"

ListedDataCore::ListedDataCore(const QMap<QString, QString>& nameList, const QMap<QString, QString>& tagList)
{
    QMap<QString, QString>::const_iterator nameItr = nameList.constBegin();

    while (nameItr != nameList.constEnd()) {
        QString locale = nameItr.key();
        setNameAndTag(nameItr.key(), nameItr.value(), tagList.value(locale));
        ++nameItr;
    }
}

ListedDataCore::ListedDataCore()
{
    mNameList.clear();
    mTagList.clear();

    //　基本的には使わない
    //　Q_DECLARE_METATYPE宣言に必要なコンストラクタ
}

ListedDataCore::~ListedDataCore()
{
}

QPixmap ListedDataCore::icon()const
{
    return mPixmap;
}

void ListedDataCore::setData(const QJsonObject& json)
{

}
QString ListedDataCore::name(const QString& lang)const
{
    return mNameList.value(lang);
}

QString ListedDataCore::tag(const QString& lang)const
{
    return mTagList.value(lang);
}

void ListedDataCore::setNameAndTag(const QString& lang, const QString& name, const QString& tag)
{
    QStringList langs = mNameList.keys();
    if( langs.contains(lang) == true) {
        mNameList.remove(lang);
        mTagList.remove(lang);
    }
    mNameList.insert(lang, name);
    mTagList.insert(lang, tag);
}
