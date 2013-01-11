#include "testprogramfactory.h"
#include "googletestprogram.h"
#include "qtestlibprogram.h"
#include <QtCore>
#include <QtDebug>

QList<TestProgram*>
TestProgramFactory::createPrograms(const QList<ProgramInfo>& programInfoList) {

    QList<TestProgram*> programs;
    foreach ( const ProgramInfo& info, programInfoList ) {

        switch ( info.type ) {
            case ProgramInfo::GoogleTest :
                programs.append( new GoogleTestProgram(info.filename) );
                break;
            case ProgramInfo::QtTestLib :
                programs.append( new QTestLibProgram(info.filename) );
                break;
            default:
                ; // ignore any others
        }
    }

    return programs;
}
