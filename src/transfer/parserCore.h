#ifndef PARSER_H
#define PARSER_H

#include <QtCore>
#include "common/common.h"

enum PARSER_TYPE {
    PARSER_TYPE_CORE = 0,
    PARSER_TYPE_RASTER,
    PARSER_TYPE_VECTOR
};

class ParserCore
{
    QTTEST_OBJECT
public:
    ParserCore() {;}
    virtual ~ParserCore() {;}

    virtual QPointF getStartPoint() = 0;
    virtual QByteArray toByte() = 0;
    virtual int  readData(const QJsonObject &json) = 0;
    virtual void writeDown(QJsonObject &json) const = 0;
    virtual bool loadFile(const QString &strFilePath) = 0;
    virtual QList<QColor> colorMap() = 0;
    virtual QSizeF outerSize() = 0;
    virtual qreal dpi() = 0;
    int neededTime() {return mNeededTime;}

    enum { Type = PARSER_TYPE_CORE };
    virtual int type() const
    {
        return Type;
    }

protected:
    int mNeededTime;

};
//Q_DECLARE_METATYPE(ParserCore)
#endif // PARSER_H
