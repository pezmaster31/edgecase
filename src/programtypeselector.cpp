#include "programtypeselector.h"
#include <QtCore>
#include <QtDebug>

static
bool isGTest(const QString& filename) {

    static const QStringList gtestPrefixes = QStringList() << "gtest_" << "gtst_";

    foreach ( const QString& gtestPrefix, gtestPrefixes ) {
        if ( filename.startsWith(gtestPrefix) )
            return true;
    }
    return false;
}

static
bool isQTest(const QString& filename) {

    // last one is for Qt standard practices
    static const QStringList qtestPrefixes = QStringList() << "qtest_" << "qtst_" << "tst_";

    foreach ( const QString& qtestPrefix, qtestPrefixes ) {
        if ( filename.startsWith(qtestPrefix) )
            return true;
    }
    return false;
}

QList<ProgramInfo> ProgramTypeSelector::getInfo(const QStringList& filepaths) {

    QList<ProgramInfo> programInfoList;
    programInfoList.reserve(filepaths.size());

    foreach ( const QString& filepath, filepaths ) {

        ProgramInfo info(filepath);
        const QString& fn = QFileInfo(filepath).fileName();
        if      ( isGTest(fn) ) info.type = ProgramInfo::GoogleTest;
        else if ( isQTest(fn) ) info.type = ProgramInfo::QtTestLib;
        else
            info.type = ProgramInfo::Unknown;

        programInfoList.append(info);
    }

    return programInfoList;
}
