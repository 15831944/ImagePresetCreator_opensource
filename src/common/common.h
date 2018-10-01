#ifndef COMMON_H
#define COMMON_H

#ifndef QTTEST_OBJECT   // test用
#define QTTEST_OBJECT   // これがないとPrivate領域のテストが行えない
#endif

#define SUCCESS 0   // 成功

#define ERR_PROJ_LOAD_FILE_NOT_EXIST     601        // ロード対象が存在しない
#define ERR_PROJ_LOAD_FILE_OPEN          602        // ロード対象が開けない
#define ERR_PROJ_LOAD_INVAILD_DATA       603        // ロードデータに不正がある
#define ERR_PROJ_LOAD_CANCEL             604        // ロードキャンセル

#define ERR_LIBRARY_OPEN                 700
#define ERR_MATERIAL_OPEN                701

//JSON Element Name for Project
#define JSON_TAG_PROJ_NAME                  QString("ProjectName")
#define JSON_TAG_FORMAT_VER                 QString("FormatVersion")
#define JSON_TAG_IMAGE                      QString("Image")
#define JSON_TAG_IMG_TYPE                   QString("ImageType")
#define JSON_TAG_IMG_EXTENTION              QString("ImageExtention")
#define JSON_TAG_IMG_NAME                   QString("ImageName")
#define JSON_TAG_IMG_DATA                   QString("ImageData")
#define JSON_TAG_POS_X                      QString("PosX")
#define JSON_TAG_POS_Y                      QString("PosY")
#define JSON_TAG_WIDTH                      QString("Width")
#define JSON_TAG_HEIGHT                     QString("Height")
#define JSON_TAG_ROTATE                     QString("Rotation")
#define JSON_TAG_GENERIC_PATH               QString("GenericPAth")
#define JSON_TAG_DETAIL_PATH                QString("DetailPAth")
#define JSON_TAG_PATH_SPEED                 QString("Speed")
#define JSON_TAG_PATH_LASER                 QString("LaserPower")
#define JSON_TAG_PATH_TIMES                 QString("ProccessTimes")
#define JSON_TAG_PATH_COLOR                 QString("Color")
#define JSON_TAG_PATH_USE_TYPE              QString("UseType")
#define JSON_TAG_SVG_EFFECTIVE_ATTR         QString("EffectiveAttribute")
#define JSON_TAG_SVG_PATH_LIST              QString("PathList")
#define JSON_TAG_SVG_COLOR_MAP              QString("ColorMap")
#define JSON_TAG_SVG_COLOR                  QString("Color")
#define JSON_TAG_SVG_MADE_BY                QString("MadeBy")
#define JSON_TAG_SVG_DPI                    QString("Dpi")
#define JSON_TAG_SVG_WIDTH                  QString("Width")
#define JSON_TAG_SVG_HEIGHT                 QString("Height")
#define JSON_TAG_SVG_UNIT                   QString("Unit")
#define JSON_TAG_SVG_VIEW_BOX               QString("ViewBox")
#define JSON_TAG_SVG_VIEW_BOX_X             QString("ViewBox_X")
#define JSON_TAG_SVG_VIEW_BOX_Y             QString("ViewBox_Y")
#define JSON_TAG_SVG_VIEW_BOX_W             QString("ViewBox_W")
#define JSON_TAG_SVG_VIEW_BOX_H             QString("ViewBox_H")
#define JSON_TAG_SVG_KEEP_ASPTECT           QString("KeepAspectRatio")
#define JSON_TAG_SVG_COMMON_SETTING         QString("Common")
#define JSON_TAG_SVG_COMMON_FILL            QString("Common_Fill")
#define JSON_TAG_SVG_COMMON_STROKE          QString("Common_Stroke")
#define JSON_TAG_SVG_COMMON_STROKE_WIDTH    QString("Common_StrokeDash")
#define JSON_TAG_SVG_COMMON_TRANSFORM       QString("Common_Transform")
#define JSON_TAG_SVG_COMMON_DISPLAY         QString("Common_Display")
#define JSON_TAG_SVG_TRANSFORM_0            QString("Transform_0")
#define JSON_TAG_SVG_TRANSFORM_1            QString("Transform_1")
#define JSON_TAG_SVG_TRANSFORM_2            QString("Transform_2")
#define JSON_TAG_SVG_TRANSFORM_3            QString("Transform_3")
#define JSON_TAG_SVG_TRANSFORM_4            QString("Transform_4")
#define JSON_TAG_SVG_TRANSFORM_5            QString("Transform_5")
#define JSON_TAG_SVG_PATH_ELEM_LIST         QString("PathElementList")
#define JSON_TAG_SVG_PATH_ELEM              QString("PathElement")
#define JSON_TAG_SVG_PATH_ELEM_TYPE         QString("PathElement_Type")
#define JSON_TAG_SVG_PATH_ELEM_POINT        QString("PathElement_Point")
#define JSON_TAG_SVG_PATH_ELEM_QUAD         QString("PathElement_Quadratic")
#define JSON_TAG_SVG_PATH_ELEM_QUAD_1       QString("PathElement_Quadratic_1")
#define JSON_TAG_SVG_PATH_ELEM_QUAD_2       QString("PathElement_Quadratic_2")
#define JSON_TAG_SVG_PATH_ELEM_QUAD_3       QString("PathElement_Quadratic_3")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER       QString("PathElement_Bezier")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER_1     QString("PathElement_Bezier_1")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER_2     QString("PathElement_Bezier_2")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER_3     QString("PathElement_Bezier_3")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER_4     QString("PathElement_Bezier_4")
#define JSON_TAG_SVG_PATH_ELEM_ARC          QString("PathElement_Arc")
#define JSON_TAG_SVG_POINT_X                QString("Point_X")
#define JSON_TAG_SVG_POINT_Y                QString("Point_Y")
#define JSON_TAG_SVG_POINT_NUM              QString("Point_No")

#define JSON_TAG_FILE_TYPE                  QString("FileType")
#define FILE_TYPE_LIBRARY                   QString("Library")
#define FILE_TYPE_MATERIAL                  QString("Materials")
#define JSON_TAG_ATTRIBUTION                QString("AttributionTag")
#define JSON_TAG_MATERIAL_DATA              QString("MaterialData")
#define JSON_TAG_MATERIAL_NAME              QString("MaterialName")
#define JSON_TAG_LASER_TYPE                 QString("LaserType")
#define JSON_TAG_LASER_TYPE_1_5W            QString("1.5W")
#define JSON_TAG_LASER_TYPE_3_5W            QString("3.5W")
#define JSON_TAG_LASER_TYPE_CO2             QString("CO2")
#define JSON_TAG_LASER_TYPE_DS              QString("DS")
#define JSON_TAG_PROCESS_PARAM              QString("ProcessParameter")
#define JSON_TAG_THICK                      QString("Thick")
#define JSON_TAG_PARAMETER                  QString("Parameter")
#define JSON_TAG_USE_TYPE_ENGRAVE           QString("Engrave")
#define JSON_TAG_USE_TYPE_CUT               QString("Cut")
#define JSON_TAG_VERSION                    QString("version")
#define VERSION_NUM                         QString("1.0.2")



//debug_log
#define DEBUG_STRING(text)      qDebug()    << QString(text)
#define INFO_STRING(text)       qInfo()     << QString(text)
#define WARNING_STRING(text)    qWarning()  << QString(text)
#define CRITICAL_STRING(text)   qCritical() << QString(text)
#define FATAL_STRING(text)      qFatal()    << QString(text)

#endif // COMMON_H
