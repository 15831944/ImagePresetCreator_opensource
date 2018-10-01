#ifndef LISTEDDATACORE_H
#define LISTEDDATACORE_H

#include <QObject>
#include <QVariant>
#include <QPixmap>

#include "common/common.h"

enum LIST_DATA_TYPE {
    LIST_DATA_CORE = 0,
    LIST_DATA_LIBRARY,
    LIST_DATA_MATERIAL
};

class ListedDataCore
{
    QTTEST_OBJECT
public:
    ListedDataCore(const QMap<QString, QString>& nameList, const QMap<QString, QString>& tagList);
    ListedDataCore();
    ~ListedDataCore();

    QPixmap icon()const;
    QString name(const QString& lang)const ;
    QString tag(const QString& lang)const ;

    void setNameAndTag(const QString& lang, const QString& name, const QString& tag);

    virtual void setData(const QJsonObject& json); // MetaType登録には純粋仮想不可

    enum { Type = LIST_DATA_CORE };
    int type() const
    {
        return Type;
    }

protected:
    QMap<QString, QString> mNameList;
    QMap<QString, QString> mTagList;
    QPixmap mPixmap;
};
Q_DECLARE_METATYPE(ListedDataCore*)
#endif // LISTEDDATACORE_H
