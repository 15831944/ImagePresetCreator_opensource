//---------------------------------------------------------------------------
#include "svgParser.h"
#include "kdTree.h"
#include "common/common.h"
//---------------------------------------------------------------------------
SvgParser::SvgParser():
    ParserCore()
{
    initEnableTags();
}
//---------------------------------------------------------------------------
SvgParser::SvgParser(const SvgParser &src):
    ParserCore()
{
    initEnableTags();
    copyMenber(src);
}
//---------------------------------------------------------------------------
SvgParser::~SvgParser()
{
    deleteMenber();
}
//---------------------------------------------------------------------------
SvgParser &SvgParser::operator=(const SvgParser &src)
{
    deleteMenber();

    return copyMenber(src);
}
//---------------------------------------------------------------------------
void SvgParser::initEnableTags()
{
    m_listEnableTags.clear();
    m_listEnableTags.append("g");
    m_listEnableTags.append("path");
    m_listEnableTags.append("circle");
    m_listEnableTags.append("ellipse");
    m_listEnableTags.append("rect");
    m_listEnableTags.append("line");
    m_listEnableTags.append("polyline");
    m_listEnableTags.append("polygon");
}
//---------------------------------------------------------------------------
void SvgParser::initMenber()
{
    // 初期化
    m_listPath.clear();
    m_listColor.clear();
    m_enSoft = EN_SVG_SOFT_OTHER;
    m_dDpi = 0.0;
    m_dWidth = 0.0;
    m_dHeight = 0.0;
    m_enUnit = EN_SVG_UNIT_NON;
    m_stViewBox.x = 0.0;
    m_stViewBox.y = 0.0;
    m_stViewBox.width = 0.0;
    m_stViewBox.height = 0.0;
    m_bPreserveAspectRatio = true;
    m_stEleCom.fill.setColor("black");
    m_stEleCom.stroke.setColor("none");
    m_stEleCom.strokeWidth = 0.5;
    m_stEleCom.transform.initMenber();
    m_stEleCom.display = true;
    m_stBBox.p1 = QPointF(0.0, 0.0);
    m_stBBox.p2 = QPointF(0.0, 0.0);
}
//---------------------------------------------------------------------------
void SvgParser::deleteMenber()
{
}
//---------------------------------------------------------------------------
SvgParser &SvgParser::copyMenber(const SvgParser &src)
{
    m_listPath = src.m_listPath;
    m_listColor = src.m_listColor;
    m_enSoft = src.m_enSoft;
    m_dDpi = src.m_dDpi;
    m_dWidth = src.m_dWidth;
    m_dHeight = src.m_dHeight;
    m_enUnit = src.m_enUnit;
    m_stViewBox = src.m_stViewBox;
    m_bPreserveAspectRatio = src.m_bPreserveAspectRatio;
    m_stEleCom = src.m_stEleCom;
    m_stBBox = src.m_stBBox;

    return *this;
}
//---------------------------------------------------------------------------
int SvgParser::getPathCount()
{
    return m_listPath.length();
}
//---------------------------------------------------------------------------
// SVG読み込み
bool SvgParser::loadFile(const QString &strSvgPath)
{
    bool bRet;

    QByteArray  svgData;
    QFileInfo   fileInfo(strSvgPath);
    QString     targetSuffix = fileInfo.suffix().toLower();

    // DXFファイルの場合は DxfParser を通してSVGに変換する
    if(targetSuffix == "dxf") {
        DxfParser dxfParser;
        if(dxfParser.dxfToSvg(strSvgPath, svgData) == false) {
            CRITICAL_STRING("Cannot parse DXF file.");
            return false;
        }
    } else {
        // ファイルを開く
        QFile fSvgFile(strSvgPath);
        fSvgFile.setFileName(strSvgPath);
        bRet = fSvgFile.open(QFile::ReadOnly | QFile::Text);
        if (bRet == false) {
            CRITICAL_STRING("Cannot open svg file.");
            return false;
        }
        svgData = fSvgFile.readAll();
        fSvgFile.close();
    }

    // 初期化
    initMenber();

    // 使用ソフト設定
    QByteArray  svgFirst = svgData.left(400);
    if (svgFirst.indexOf("Inkscape") != -1) {
        m_enSoft = EN_SVG_SOFT_INKSCAPE;
    } else if (svgFirst.indexOf("Illustrator") != -1) {
        m_enSoft = EN_SVG_SOFT_ILLUSTRATOR;
    } else if (svgFirst.indexOf("Intaglio") != -1) {
        m_enSoft = EN_SVG_SOFT_INTAGLIO;
    } else if (svgFirst.indexOf("CorelDraw") != -1) {
        m_enSoft = EN_SVG_SOFT_COREDRAW;
    } else if (svgFirst.indexOf("Qt") != -1) {
        m_enSoft = EN_SVG_SOFT_QT;
    } else if (svgFirst.indexOf("Visio") != -1) {
        m_enSoft = EN_SVG_SOFT_VISIO;
    } else if (svgFirst.indexOf("FaboolDxfToSvg") != -1) {
        m_enSoft = EN_SVG_SOFT_DXF2SVG;
    }

    // DPI設定
    switch (m_enSoft) {
    case EN_SVG_SOFT_OTHER:
    case EN_SVG_SOFT_INKSCAPE:
    case EN_SVG_SOFT_QT:
        m_dDpi = 90.0;
        break;
    case EN_SVG_SOFT_ILLUSTRATOR:
    case EN_SVG_SOFT_INTAGLIO:
    case EN_SVG_SOFT_VISIO:
        m_dDpi = 72.0;
        break;
    case EN_SVG_SOFT_COREDRAW:
        m_dDpi = 96.0;
        break;
    case EN_SVG_SOFT_DXF2SVG:
        m_dDpi = 100.0;
        break;
    default:
        CRITICAL_STRING("Using unsupported soft.");
        return false;
    }

    // DOMに読み込む
    QDomDocument doc;
    bRet = doc.setContent(svgData, true);
    if (bRet == false) {
        CRITICAL_STRING("Cannot parse DOM document.");
        return false;
    }

    // SVG属性取得
    QDomElement root = doc.documentElement();
    if (parseRoot(root) == false) {
        CRITICAL_STRING("Failed to parse root.");
        return false;
    }

    if(parseDefs(root) == false) {
        DEBUG_STRING("Failed to parse defs.");
        return false;
    }

    // style解析
    if(parseStyle(root) == false) {
        DEBUG_STRING("Failed to parse style.");
        return false;
    }

    // 再帰的に子要素を解析
    if (parseChildren(root, m_stEleCom) == false) {
        CRITICAL_STRING("Failed to parse children.");
        return false;
    }

    // Bounding Box計算
    if (calcBBox() == false) {
        CRITICAL_STRING("Failed to calcurate bounding box.");
        return false;
    }

    // 要素がない場合はエラー
    if (getPathCount() == 0) {
        CRITICAL_STRING("There is not path.");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool SvgParser::parseRoot(const QDomElement &root)
{
    QString strBuf;
    QStringList listBuf;
    EN_SVG_UNIT enWHUnit = EN_SVG_UNIT_NON;

    if (root.tagName() != "svg") {
        DEBUG_STRING("Root is not SVG.");
        return false;
    }

    //Inkscape version check
    if(m_enSoft == EN_SVG_SOFT_INKSCAPE && root.hasAttribute("version")) {
        QString version = root.attribute("version").trimmed();
        bool checkReal = false;
        qreal verNum = version.left(4).toDouble(&checkReal);
        if(checkReal == true && verNum >= 0.92) {
            m_dDpi = 96.0;
        }
    }

    // width
    if (root.hasAttribute("width")) {
        strBuf = root.attribute("width").trimmed();
        if (strBuf.length() > 0) {
            // 幅、高さの単位取得
            if (strBuf.endsWith("px")) {
                enWHUnit = EN_SVG_UNIT_PX;
            } else if(strBuf.endsWith("pt")) {
                enWHUnit = EN_SVG_UNIT_PT;
            } else if(strBuf.endsWith("pc")) {
                enWHUnit = EN_SVG_UNIT_PC;
            } else if(strBuf.endsWith("cm")) {
                enWHUnit = EN_SVG_UNIT_CM;
            } else if(strBuf.endsWith("mm")) {
                enWHUnit = EN_SVG_UNIT_MM;
            } else if(strBuf.endsWith("in")) {
                enWHUnit = EN_SVG_UNIT_IN;
            }
            // 単位文字列を削除
            if (enWHUnit != EN_SVG_UNIT_NON) {
                strBuf = strBuf.left(strBuf.length() - 2);
            }
            // 数値に変換
            m_dWidth = UnitTransfer::toSvgPixel(strBuf.toDouble(), enWHUnit, m_dDpi);
        }
    }
    // height
    if (root.hasAttribute("height")) {
        strBuf = root.attribute("height").trimmed();
        if (strBuf.length() > 0) {
            // 単位文字列を削除
            if (enWHUnit != EN_SVG_UNIT_NON) {
                strBuf = strBuf.left(strBuf.length() - 2);
            }
            // 数値に変換
            m_dHeight = UnitTransfer::toSvgPixel(strBuf.toDouble(), enWHUnit, m_dDpi);
        }
    }

    // SVG全体の単位設定
    m_enUnit = enWHUnit;

    // 元がDXFファイルの場合は、MM
    if (m_enSoft == EN_SVG_SOFT_DXF2SVG) {
        m_enUnit = EN_SVG_UNIT_MM;
    }

    // Visioの場合は、PX
    if (m_enSoft == EN_SVG_SOFT_VISIO) {
        m_enUnit = EN_SVG_UNIT_PX;
    }

    // Inkscapeの場合、単位を取得する
    if (m_enSoft == EN_SVG_SOFT_INKSCAPE) {
        for (QDomNode node = root.firstChild(); node.isNull() == false; node = node.nextSibling()) {
            QDomElement childElement = node.toElement();
            QString strTag = childElement.tagName();

            if (strTag != "namedview") {
                continue;
            }
            if (childElement.hasAttribute("document-units")) {
                strBuf = childElement.attribute("document-units").trimmed();
                if (strBuf.endsWith("px")) {
                    m_enUnit = EN_SVG_UNIT_PX;
                } else if(strBuf.endsWith("pt")) {
                    m_enUnit = EN_SVG_UNIT_PT;
                } else if(strBuf.endsWith("pc")) {
                    m_enUnit = EN_SVG_UNIT_PC;
                } else if(strBuf.endsWith("cm")) {
                    m_enUnit = EN_SVG_UNIT_CM;
                } else if(strBuf.endsWith("mm")) {
                    m_enUnit = EN_SVG_UNIT_MM;
                } else if(strBuf.endsWith("in")) {
                    m_enUnit = EN_SVG_UNIT_IN;
                } else {
                    m_enUnit = EN_SVG_UNIT_PX;
                }
            }
        }
    }

    // preserveAspectRatio
    if (root.hasAttribute("preserveAspectRatio")) {
        strBuf = root.attribute("preserveAspectRatio").trimmed();
        if (strBuf == "none") {
            m_bPreserveAspectRatio = false;
        }
    }

    // viewBox
    if (root.hasAttribute("viewBox")) {
        strBuf = root.attribute("viewBox").trimmed();
        listBuf = strBuf.split(' ');
        if (listBuf.length() != 4) {
            listBuf = strBuf.split(',');
        }
        if (listBuf.length() == 4) {
            m_stViewBox.x = UnitTransfer::toSvgPixel(listBuf.at(0).toDouble(), m_enUnit, m_dDpi);;
            m_stViewBox.y = UnitTransfer::toSvgPixel(listBuf.at(1).toDouble(), m_enUnit, m_dDpi);;
            m_stViewBox.width = UnitTransfer::toSvgPixel(listBuf.at(2).toDouble(), m_enUnit, m_dDpi);;
            m_stViewBox.height = UnitTransfer::toSvgPixel(listBuf.at(3).toDouble(), m_enUnit, m_dDpi);;
        }
    }

    // viewBoxの調整
    if (m_dWidth != 0.0 && m_dHeight != 0.0 &&
            m_stViewBox.width != 0.0 && m_stViewBox.height != 0.0) {
        qreal dAspectX = m_dWidth / m_stViewBox.width;
        qreal dAspectY = m_dHeight / m_stViewBox.height;
        qreal dPow;
        qreal dNew;

        if (m_bPreserveAspectRatio == true) {
            if (dAspectX > dAspectY) {
                dPow = dAspectY / dAspectX;
                dNew = m_stViewBox.width / dPow;
                m_stViewBox.x = m_stViewBox.x - (dNew -  m_stViewBox.width) / 2;
                m_stViewBox.width = dNew;
            } else if (dAspectX < dAspectY) {
                dPow = dAspectX / dAspectY;
                dNew = m_stViewBox.height / dPow;
                m_stViewBox.y = m_stViewBox.y - (dNew -  m_stViewBox.height) / 2;
                m_stViewBox.height  = dNew;
            }
        }
    }

    // 全体transform設定
    m_stEleCom.transform.setTranslate(m_stViewBox.x * -1, m_stViewBox.y * -1);
    if (m_dWidth != 0.0 && m_dHeight != 0.0 &&
            m_stViewBox.width != 0.0 && m_stViewBox.height != 0.0) {
        m_stEleCom.transform.setScale(m_dWidth / m_stViewBox.width, m_dHeight / m_stViewBox.height);
    }

    return true;
}
//---------------------------------------------------------------------------
bool SvgParser::parseDefs(const QDomElement &root)
{
    QDomElement element ;
    bool isFoundDefs = false;
    for (element = root.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {
        if(element.tagName() == "defs") {
            isFoundDefs = true;
            break;
        }
    }
    if(isFoundDefs == false) {
        return true;
    }

    parseStyle(element);

    return true;
}
//---------------------------------------------------------------------------
bool SvgParser::parseStyle(const QDomElement &root )
{
    QDomElement element ;
    bool isFoundStyle = false;
    for (element = root.firstChildElement(); !element.isNull(); element = element.nextSiblingElement()) {
        if(element.tagName() == "style") {
            isFoundStyle = true;
            break;
        }
    }

    if(isFoundStyle == false) {
        return true;
    }

    QDomNode node = element.firstChild();
    QString nodeContent = node.nodeValue();
    QStringList styleList = nodeContent.split(QRegExp("[{}]"), QString::SkipEmptyParts);
    while(styleList.isEmpty() == false) {
        if(styleList.count() < 2) {
            break;
        }
        QString className = styleList.takeFirst();
        QString styleStr = styleList.takeFirst();
        className.remove(QRegExp("[\t\n \.]"));
        styleStr.remove(QRegExp("[\t\n ]"));

        m_stEleCom.style.insert(className, styleStr);
    }


    return true;
}
//---------------------------------------------------------------------------
bool SvgParser::parseChildren(const QDomElement &element, ST_SVG_ELE_COM stEleCom)
{
    for (QDomNode node = element.firstChild(); node.isNull() == false; node = node.nextSibling()) {
        QDomElement childElement = node.toElement();
        QString strTag = childElement.tagName();

        if( m_listEnableTags.contains(strTag, Qt::CaseInsensitive) == false) {
            continue;
        }

        // 属性取得
        ST_SVG_ELE_COM tempEleCom = stEleCom;
        if (getAttribute(childElement, &tempEleCom) == false) {
            DEBUG_STRING("Failed to get attributes.");
            return false;
        }

        if (strTag == "g") {
            // 子要素の解析
            if (parseChildren(childElement, tempEleCom) == false) {
                DEBUG_STRING("Failed to parse children.");
                return false;
            }
        } else {
            // ノードに設定
            SvgPath svgPath;
            // 要素を読み込む
            if (svgPath.setNode(childElement, tempEleCom, m_enUnit, m_dDpi) == false) {
                DEBUG_STRING("Failed to parse nodes.");
                // 無効なエレメントがあった際は読み飛ばす（エラーにはしない）
                continue;
            }
            // リストに追加
            m_listPath.append(svgPath);

            // 色配列に追加
            addColorlist(svgPath.getColor());
        }
    }

    return true;
}
//---------------------------------------------------------------------------
// 属性取得
bool SvgParser::getAttribute(const QDomElement &element, ST_SVG_ELE_COM* pstEleCom)
{
    QString strBuf;

    // style
    if(element.hasAttribute("class")) {
        QStringList clsList = element.attribute("class").trimmed().split(QRegExp("[\,\.\ ]"));
        QMap<QString, QString> styleMap = pstEleCom->style;
        QList<QString> keyList = styleMap.keys();

        QString style;
        foreach (QString clsName, clsList) {
            int index = keyList.indexOf(clsName);
            if(index != -1) {
                style += styleMap.value(clsName) + ";";
            }
        }
        applyStyle(style, pstEleCom);
    } else if (element.hasAttribute("style")) {
        strBuf = element.attribute("style").trimmed();

        applyStyle(strBuf, pstEleCom);
    }
    // fill
    if (element.hasAttribute("fill")) {
        strBuf = element.attribute("fill").trimmed();
        if (pstEleCom->fill.setColor(strBuf) == false) {
            DEBUG_STRING("Failed to set fill color.");
            return false;
        }
    }
    // stroke
    if (element.hasAttribute("stroke")) {
        strBuf = element.attribute("stroke").trimmed();
        if (pstEleCom->stroke.setColor(strBuf) == false) {
            DEBUG_STRING("Failed to set stroke color.");
            return false;
        }
    }
    // strokeWidth
    if (element.hasAttribute("stroke-width")) {
        strBuf = element.attribute("stroke-width").trimmed();
        bool check = false;
        qreal sWidth = 1.0;
        EN_SVG_UNIT enWHUnit = EN_SVG_UNIT_NON;
        if (strBuf.length() > 0) {
            // 幅、高さの単位取得
            if (strBuf.endsWith("px")) {
                enWHUnit = EN_SVG_UNIT_PX;
            } else if(strBuf.endsWith("pt")) {
                enWHUnit = EN_SVG_UNIT_PT;
            } else if(strBuf.endsWith("pc")) {
                enWHUnit = EN_SVG_UNIT_PC;
            } else if(strBuf.endsWith("cm")) {
                enWHUnit = EN_SVG_UNIT_CM;
            } else if(strBuf.endsWith("mm")) {
                enWHUnit = EN_SVG_UNIT_MM;
            } else if(strBuf.endsWith("in")) {
                enWHUnit = EN_SVG_UNIT_IN;
            }
            // 単位文字列を削除
            if (enWHUnit != EN_SVG_UNIT_NON) {
                strBuf = strBuf.left(strBuf.length() - 2);
            }
            // 数値に変換
            sWidth = UnitTransfer::toSvgPixel(strBuf.toDouble(&check), enWHUnit, m_dDpi);
            if (check == false) {
                DEBUG_STRING("Failed to set stroke width.");
                return false;
            }
        }
        pstEleCom->strokeWidth = sWidth;
    }
    // transform
    if (element.hasAttribute("transform")) {
        strBuf = element.attribute("transform").trimmed();
        if (pstEleCom->transform.setTransform(strBuf, m_enUnit, m_dDpi) == false) {
            DEBUG_STRING("Failed to set transform.");
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool SvgParser::applyStyle(const QString &style, ST_SVG_ELE_COM* pstEleCom)
{
    QString strBuf = style;
    QStringList listStyles = strBuf.split(";");
    for (int i = 0; i < listStyles.length(); i++) {
        strBuf = listStyles.at(i).trimmed();
        if (strBuf == "") {
            continue;
        }
        QStringList listOne = strBuf.split(":");
        if (listOne.length() != 2) {
            continue;
        }
        QString strName = listOne.at(0).trimmed();
        QString strValue = listOne.at(1).trimmed();
        if (strName == "fill") {
            if (pstEleCom->fill.setColor(strValue) == false) {
                DEBUG_STRING("Failed to set fill style color.");
                return false;
            }
        } else if (strName == "stroke") {
            if (pstEleCom->stroke.setColor(strValue) == false) {
                DEBUG_STRING("Failed to set stroke style color.");
                return false;
            }
        } else if (strName == "stroke-width") {
            bool check = false;
            qreal sWidth = 1.0;
            EN_SVG_UNIT enWHUnit = EN_SVG_UNIT_NON;
            if (strValue.length() > 0) {
                // 幅、高さの単位取得
                if (strValue.endsWith("px")) {
                    enWHUnit = EN_SVG_UNIT_PX;
                } else if(strValue.endsWith("pt")) {
                    enWHUnit = EN_SVG_UNIT_PT;
                } else if(strValue.endsWith("pc")) {
                    enWHUnit = EN_SVG_UNIT_PC;
                } else if(strValue.endsWith("cm")) {
                    enWHUnit = EN_SVG_UNIT_CM;
                } else if(strValue.endsWith("mm")) {
                    enWHUnit = EN_SVG_UNIT_MM;
                } else if(strValue.endsWith("in")) {
                    enWHUnit = EN_SVG_UNIT_IN;
                }
                // 単位文字列を削除
                if (enWHUnit != EN_SVG_UNIT_NON) {
                    strValue = strValue.left(strValue.length() - 2);
                }
                // 数値に変換
                sWidth = UnitTransfer::toSvgPixel(strValue.toDouble(&check), enWHUnit, m_dDpi);
                if (check == false) {
                    DEBUG_STRING("Failed to set stroke width.");
                    return false;
                }
            }
            pstEleCom->strokeWidth = sWidth;
        } else if (strName == "display") {
            if (strValue == "none") {
                pstEleCom->display = false;
            } else {
                pstEleCom->display = true;
            }
        }
    }
    return true;
}
//---------------------------------------------------------------------------
// 色配列に色を追加(すでに登録済みの色は追加しない)
void SvgParser::addColorlist(QColor color)
{
    bool bAddFlg = true;

    for (int i = 0; i < m_listColor.length(); i++) {
        if (m_listColor.at(i) == color) {
            bAddFlg = false;
            break;
        }
    }
    if (bAddFlg == true) {
        m_listColor.append(color);
    }
}
//---------------------------------------------------------------------------
bool SvgParser::calcBBox()
{
    ST_SVG_BBOX stBBox;

    if (m_listPath.length() == 0) {
        return true;
    }

    if (m_listPath[0].getBBox(&m_stBBox) == false) {
        return false;
    }
    qreal w = m_stBBox.p2.x() - m_stBBox.p1.x();
    qreal h = m_stBBox.p2.y() - m_stBBox.p1.y();
    if( w < 0 ) {
        m_stBBox.p2.setX(m_stBBox.p2.x() - w);
        m_stBBox.p1.setX(m_stBBox.p1.x() + w);
    }
    if( h < 0 ) {
        m_stBBox.p2.setY(m_stBBox.p2.y() - h);
        m_stBBox.p1.setY(m_stBBox.p1.y() + h);
    }


    for (int i = 1; i < m_listPath.length(); ++i) {
        if (m_listPath[i].getBBox(&stBBox) == false) {
            return false;
        }
        w = stBBox.p2.x() - stBBox.p1.x();
        h = stBBox.p2.y() - stBBox.p1.y();
        if( w < 0 ) {
            stBBox.p2.setX(stBBox.p2.x() - w);
            stBBox.p1.setX(stBBox.p1.x() + w);
        }
        if( h < 0 ) {
            stBBox.p2.setY(stBBox.p2.y() - h);
            stBBox.p1.setY(stBBox.p1.y() + h);
        }

        if (m_stBBox.p1.x() > stBBox.p1.x()) {
            m_stBBox.p1.setX(stBBox.p1.x());
        }
        if (m_stBBox.p1.y() > stBBox.p1.y()) {
            m_stBBox.p1.setY(stBBox.p1.y());
        }
        if (m_stBBox.p2.x() < stBBox.p2.x()) {
            m_stBBox.p2.setX(stBBox.p2.x());
        }
        if (m_stBBox.p2.y() < stBBox.p2.y()) {
            m_stBBox.p2.setY(stBBox.p2.y());
        }
    }

    return true;
}
//---------------------------------------------------------------------------
// 描画用のSVG文字列を返す
QByteArray SvgParser::toByte()
{
    QString strAttr;
    QDomDocument doc;
    QDomElement root = doc.createElement("svg");

    // SVG全体の属性設定
    qreal boxW = m_stBBox.p2.x() - m_stBBox.p1.x();
    qreal boxH = m_stBBox.p2.y() - m_stBBox.p1.y();
    if(boxW <= 0) { boxW = 1; }
    if(boxH <= 0) { boxH = 1; }
    strAttr = QString("%1 %2 %3 %4").arg(m_stBBox.p1.x()).arg(m_stBBox.p1.y()).arg(boxW).arg(boxH);
    root.setAttribute("viewBox", strAttr);
    doc.appendChild(root);

    for (int i = 0; i < m_listPath.length(); ++i) {
        root.appendChild(m_listPath[i].getNodeElement(&doc));
    }

    QByteArray byRet = doc.toByteArray();

    return byRet;
}
//---------------------------------------------------------------------------
QPointF SvgParser::getStartPoint()
{
    QPointF distance = m_stBBox.p1;
    qreal mmW = UnitTransfer::SvgPixelto(distance.x(), EN_SVG_UNIT_MM, m_dDpi);
    qreal mmH = UnitTransfer::SvgPixelto(distance.y(), EN_SVG_UNIT_MM, m_dDpi);
    return QPointF(mmW, mmH);
}
//---------------------------------------------------------------------------
// Jsonへ書き込む
void SvgParser::writeDown(QJsonObject &json) const
{
    QJsonArray attributeArray;
    foreach (QString attr, m_listEnableTags) {
        attributeArray.append(QJsonValue(attr));
    }
    json[JSON_TAG_SVG_EFFECTIVE_ATTR] = attributeArray;

    QJsonArray pathArray;
    foreach (SvgPath path, m_listPath) {
        QJsonObject pathObj;
        path.writeDown(pathObj);
        pathArray.append(pathObj);
    }
    json[JSON_TAG_SVG_PATH_LIST] = pathArray;

    QJsonArray colorArray;
    foreach (QColor color, m_listColor) {
        QJsonObject colorObj;
        colorObj[JSON_TAG_SVG_COLOR] = color.name();
        colorArray.append(colorObj);
    }
    json[JSON_TAG_SVG_COLOR_MAP] = colorArray;

    json[JSON_TAG_SVG_MADE_BY] = m_enSoft;
    json[JSON_TAG_SVG_DPI]     = m_dDpi;
    json[JSON_TAG_SVG_WIDTH]   = m_dWidth;
    json[JSON_TAG_SVG_HEIGHT]  = m_dHeight;
    json[JSON_TAG_SVG_UNIT]    = m_enUnit;

    json[JSON_TAG_SVG_KEEP_ASPTECT]   = m_bPreserveAspectRatio;

    QJsonObject  cmn;
    QJsonObject  cmn_fill;
    QJsonObject  cmn_stroke;
    QJsonObject  cmn_transform;
    m_stEleCom.fill.writeDown(cmn_fill);
    m_stEleCom.stroke.writeDown(cmn_stroke);
    m_stEleCom.transform.writeDown(cmn_transform);
    cmn[JSON_TAG_SVG_COMMON_FILL]       = cmn_fill;
    cmn[JSON_TAG_SVG_COMMON_STROKE]     = cmn_stroke;
    cmn[JSON_TAG_SVG_COMMON_STROKE_WIDTH] = m_stEleCom.strokeWidth;
    cmn[JSON_TAG_SVG_COMMON_TRANSFORM]  = cmn_transform;
    cmn[JSON_TAG_SVG_COMMON_DISPLAY]    = m_stEleCom.display;
    json[JSON_TAG_SVG_COMMON_SETTING]   = cmn;

    QJsonObject  vb;
    vb[JSON_TAG_SVG_VIEW_BOX_X] = m_stViewBox.x;
    vb[JSON_TAG_SVG_VIEW_BOX_Y] = m_stViewBox.y;
    vb[JSON_TAG_SVG_VIEW_BOX_W] = m_stViewBox.width;
    vb[JSON_TAG_SVG_VIEW_BOX_H] = m_stViewBox.height;
    json[JSON_TAG_SVG_VIEW_BOX] = vb;
}
//---------------------------------------------------------------------------
// Jsonから読み込んで解析
int SvgParser::readData(const QJsonObject &json)
{
    // 初期化
    initMenber();

    QJsonArray attributeArray =  json[JSON_TAG_SVG_EFFECTIVE_ATTR].toArray();
    foreach (QJsonValue attrVal, attributeArray) {
        m_listEnableTags.append(attrVal.toString());
    }

    QJsonArray pathArray = json[JSON_TAG_SVG_PATH_LIST].toArray();
    foreach (QJsonValue pathVal, pathArray) {
        SvgPath path;
        QJsonObject pathObj = pathVal.toObject();
        path.readData(pathObj);
        m_listPath.append(path);
    }

    QJsonArray colorArray = json[JSON_TAG_SVG_COLOR_MAP].toArray();
    foreach (QJsonValue colorVal, colorArray) {
        QColor color;
        QJsonObject colorObj = colorVal.toObject();
        color.setNamedColor(colorObj[JSON_TAG_SVG_COLOR].toString());
        m_listColor.append(color);
    }

    m_enSoft    = (EN_SVG_SOFT)(json[JSON_TAG_SVG_MADE_BY].toInt());
    m_dDpi      = json[JSON_TAG_SVG_DPI].toDouble();
    m_dWidth    = json[JSON_TAG_SVG_WIDTH].toDouble();
    m_dHeight   = json[JSON_TAG_SVG_HEIGHT].toDouble();
    m_enUnit    = (EN_SVG_UNIT)(json[JSON_TAG_SVG_UNIT].toInt());
    m_bPreserveAspectRatio = json[JSON_TAG_SVG_KEEP_ASPTECT].toBool();


    QJsonObject  cmn            = json[JSON_TAG_SVG_COMMON_SETTING].toObject();
    QJsonObject  cmn_fill       = cmn[JSON_TAG_SVG_COMMON_FILL].toObject();
    QJsonObject  cmn_stroke     = cmn[JSON_TAG_SVG_COMMON_STROKE].toObject();
    QJsonObject  cmn_transform  = cmn[JSON_TAG_SVG_COMMON_TRANSFORM].toObject();
    qreal strokeWidth = cmn[JSON_TAG_SVG_COMMON_STROKE_WIDTH].toDouble();
    if(strokeWidth <= 0) { strokeWidth = 1.0; }
    m_stEleCom.fill.readData(cmn_fill);
    m_stEleCom.stroke.readData(cmn_stroke);
    m_stEleCom.strokeWidth = strokeWidth;
    m_stEleCom.transform.readData(cmn_transform);
    m_stEleCom.display = cmn[JSON_TAG_SVG_COMMON_DISPLAY].toBool();

    QJsonObject vb      = json[JSON_TAG_SVG_VIEW_BOX].toObject();
    m_stViewBox.x       = vb[JSON_TAG_SVG_VIEW_BOX_X].toDouble();
    m_stViewBox.y       = vb[JSON_TAG_SVG_VIEW_BOX_Y].toDouble();
    m_stViewBox.width   = vb[JSON_TAG_SVG_VIEW_BOX_W].toDouble();
    m_stViewBox.height  = vb[JSON_TAG_SVG_VIEW_BOX_H].toDouble();

    // Bounding Box計算
    if (calcBBox() == false) {
        CRITICAL_STRING("Failed to calcurate bounding box.");
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }

    // 要素がない場合はエラー
    if (getPathCount() == 0) {
        CRITICAL_STRING("There is not path.");
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }

    return SUCCESS;
}
//---------------------------------------------------------------------------
// 使う色を返す
QList<QColor> SvgParser::colorMap()
{
    return m_listColor;
}
//---------------------------------------------------------------------------
QSizeF SvgParser::outerSize()
{
    QSizeF size = QSizeF(m_stBBox.p2.x() - m_stBBox.p1.x(), m_stBBox.p2.y() - m_stBBox.p1.y());
    return size;
}
//---------------------------------------------------------------------------
qreal SvgParser::dpi()
{
    return m_dDpi;
}
//---------------------------------------------------------------------------
void SvgParser::setStrokeWidth(qreal wid)
{
    m_stEleCom.strokeWidth = wid;

    for(int i = 0; i < m_listPath.count(); i++) {
        m_listPath[i].setStrokeWid(wid);
    }
}
//---------------------------------------------------------------------------
void SvgParser::getSubPoints(QSubPointList* pList, const SvgTransform &transform)
{
    qreal tolerance2 = UnitTransfer::SvgPixelto(DF_SVG_TO_GCODE_TOLERANCE, EN_SVG_UNIT_MM, m_dDpi)
                       * UnitTransfer::SvgPixelto(DF_SVG_TO_GCODE_TOLERANCE, EN_SVG_UNIT_MM, m_dDpi);

    for (int i = 0; i < m_listPath.length(); ++i) {
        // Pathをポイントに変換する
        m_listPath[i].getSubPoint(&pList[i], transform, m_dDpi, tolerance2, m_stBBox.p1);
    }
}
//---------------------------------------------------------------------------
void SvgParser::separationColor(QMap<QString, QSubPointList>& boundarys, QSubPointList* pList)
{
    qreal epsilon = 0.1 * DF_SVG_TO_GCODE_TOLERANCE;
    qreal epsilon2 = epsilon * epsilon;

    for (int i = 0; i < m_listPath.length(); ++i) {
        QString color = m_listPath[i].getColor().name();
        for (int j = 0; j < pList[i].length(); ++j) {
            if (pList[i][j].length() == 0) {
                continue;
            }
            if (boundarys.count(color) == 0) {
                boundarys[color] = QSubPointList();
                boundarys[color].append(pList[i][j]);
                continue;
            }
            QPointList lastsubpath = boundarys[color][boundarys[color].length() - 1];
            QPointF endpoint = lastsubpath[lastsubpath.length() - 1];
            qreal d2 = qPow(endpoint.x() - pList[i][j][0].x(), 2.0) + qPow(endpoint.y() - pList[i][j][0].y(), 2.0);

            if (d2 < epsilon2) {
                pList[i][j].removeFirst();
                boundarys[color][boundarys[color].length() - 1].append(pList[i][j]);
            } else {
                boundarys[color].append(pList[i][j]);
            }
        }
    }
}
//---------------------------------------------------------------------------
void SvgParser::optimizeAll(QMap<QString, QSubPointList>& boundarys)
{
    QList<QString> key = boundarys.keys();

    for (int i = 0; i < key.length(); ++i) {
        connect_segments(boundarys[key[i]]);
        simplify_all(boundarys[key[i]]);
        sort_by_seektime(boundarys[key[i]]);
    }
}
//---------------------------------------------------------------------------
void SvgParser::connect_segments(QSubPointList& path)
{
    qreal epsilon = 0.1 * DF_SVG_TO_GCODE_TOLERANCE;
    qreal epsilon2 = epsilon * epsilon;

    int newIdx = 0;
    for (int i = 1; i < path.length(); ++i) {
        QPointF point = path[newIdx][path[newIdx].length() - 1];
        QPointF startpoint = path[i][0];
        qreal d2_start = qPow(point.x() - startpoint.x(), 2.0) + qPow(point.y() - startpoint.y(), 2.0);
        if (d2_start < epsilon2) {
            for (int j = 1; j < path[i].length(); j++) {
                path[newIdx].append(path[i][j]);
            }
        } else {
            newIdx += 1;
            path[newIdx] = path[i];
        }
    }

    int removeCnt = path.length() - (newIdx + 1);
    for (int i = 0; i < removeCnt; i++) {
        path.removeFirst();
    }
}
//---------------------------------------------------------------------------
void SvgParser::simplify_all(QSubPointList& path)
{
    qreal tolerance2 = DF_SVG_TO_GCODE_TOLERANCE * DF_SVG_TO_GCODE_TOLERANCE;
    int totalverts = 0;
    int optiverts = 0;

    for (int u = 0; u < path.length(); u++) {
        totalverts += path[u].length();
        path[u] = simplify(path[u], tolerance2);
        optiverts += path[u].length();
    }
}
//---------------------------------------------------------------------------
QPointList SvgParser::simplify(const QPointList &V, qreal tol2)
{
    int n = V.length();
    int k;
    int pv;
    QPointList vt;

    k = 1;
    pv = 0;
    vt.append(V[0]);
    for (int i = 1; i < n; i++) {
        if (d2(V[i], V[pv]) < tol2) {
            continue;
        }
        vt.append(V[i]);
        k++;
        pv = i;
    }
    if (pv < n - 1) {
        vt.append(V[n - 1]);
        k++;
    }

    bool* mk = new bool[k];
    for (int i = 0; i < k; ++i) {
        mk[i] = false;
    }
    mk[0] = true;
    mk[k - 1] = true;
    simplifyDP(tol2, vt, 0, k - 1, mk);

    QPointList sV;
    for (int i = 0; i < k; i++) {
        if (mk[i]) {
            sV.append(vt[i]);
        }
    }

    delete[] mk;

    return sV;
}
//---------------------------------------------------------------------------
void SvgParser::simplifyDP(qreal tol2, const QPointList &v, int j, int k, bool mk[])
{
    if (k <= j + 1) {
        return;
    }

    int maxi = j;
    qreal maxd2 = 0.0;
    QPointF S[2];
    S[0] = v[j];
    S[1] = v[k];
    QPointF u = diff(S[1], S[0]);
    qreal cu = norm2(u);

    for (int i = j + 1; i < k; i++) {
        QPointF w = diff(v[i], S[0]);
        qreal cw = dot(w, u);
        qreal dv2;
        if (cw <= 0.0) {
            dv2 = d2(v[i], S[0]);
        } else if (cu <= cw) {
            dv2 = d2(v[i], S[1]);
        } else {
            qreal b = cw / cu;
            QPointF Pb = QPointF(S[0].x() + b * u.x(), S[0].y() + b * u.y());
            dv2 = d2(v[i], Pb);
        }
        if (dv2 <= maxd2) {
            continue;
        }
        maxi = i;
        maxd2 = dv2;
    }
    if (maxd2 > tol2) {
        mk[maxi] = true;
        simplifyDP(tol2, v, j, maxi, mk);
        simplifyDP(tol2, v, maxi, k, mk);
    }
}
//---------------------------------------------------------------------------
void SvgParser::sort_by_seektime(QSubPointList& path)
{
    QSubPointList path_unsorted;
    kt::Tree tree = kt::Tree();
    QList<bool> usedIdxs;

    for (int i = 0; i < path.length(); ++i) {
        QPointList pathseg = path[i];
        path_unsorted.append(pathseg);
        tree.insert(pathseg[0], new kt::Data(i, false));
        tree.insert(pathseg[pathseg.length() - 1], new kt::Data(i, true));
        usedIdxs.append(false);
    }

    QPointF endpoint(0, 0);
    int newIdx = 0;

    for (int i = 0; i < path_unsorted.length() * 2; i++) {
        kt::Tuple ret = tree.nearest(endpoint, true);
        int index = ret.node->data->index;
        int rev   = ret.node->data->rev;

        ret.node->data = NULL;
        if (usedIdxs[index] == false) {
            path[newIdx] = path_unsorted[index];
            if (rev) {
                QPointList pathRev;
                int pathLength = path[newIdx].length();
                for (int j = 0; j < path[newIdx].length(); ++j) {
                    pathRev.append(path[newIdx][pathLength - 1 - j]);
                }
                path[newIdx] = pathRev;
            }
            endpoint = path[newIdx][path[newIdx].length() - 1];
            newIdx += 1;
            usedIdxs[index] = true;
        }
    }
}
//---------------------------------------------------------------------------
QJsonObject SvgParser::transferPathToCurrent(const QJsonObject& json, const qreal& dpi)
{
    QJsonObject retObj;
    QJsonArray pathData = json["path"].toArray();

    QJsonObject common;
    common[JSON_TAG_SVG_COMMON_DISPLAY] = json["visible"].toBool();
    QJsonObject commonFill;
    commonFill[JSON_TAG_SVG_COLOR] = json["fill"].toString();
    common[JSON_TAG_SVG_COMMON_FILL] = commonFill;
    QJsonObject commonStroke;
    commonStroke[JSON_TAG_SVG_COLOR] = json["stroke"].toString();
    common[JSON_TAG_SVG_COMMON_STROKE] = commonStroke;
    qreal strokeWid = json["strokeWidth"].toDouble();
    if(strokeWid == 0) { strokeWid = 1; }
    common[JSON_TAG_SVG_COMMON_STROKE_WIDTH] = strokeWid;
    QJsonObject commonTransform;
    commonTransform[JSON_TAG_SVG_TRANSFORM_0] = 1;
    commonTransform[JSON_TAG_SVG_TRANSFORM_1] = 0;
    commonTransform[JSON_TAG_SVG_TRANSFORM_2] = 0;
    commonTransform[JSON_TAG_SVG_TRANSFORM_3] = 1;
    commonTransform[JSON_TAG_SVG_TRANSFORM_4] = 0;
    commonTransform[JSON_TAG_SVG_TRANSFORM_5] = 0;
    common[JSON_TAG_SVG_COMMON_TRANSFORM] = commonTransform;

    SvgPath svgpath;
    svgpath.setPathJson(common, pathData, dpi);

    // リストに追加
    m_listPath.append(svgpath);

    // 色配列に追加
    addColorlist(svgpath.getColor());

    svgpath.writeDown(retObj);

    retObj[JSON_TAG_SVG_COMMON_SETTING] = common;

    return retObj;
}
//---------------------------------------------------------------------------
QRectF SvgParser::getBBox()
{
    calcBBox();
    QRectF retRect(m_stBBox.p1, m_stBBox.p2);
    return retRect;
}
