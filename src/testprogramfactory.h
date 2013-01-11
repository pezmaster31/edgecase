#ifndef TESTPROGRAMFACTORY_H
#define TESTPROGRAMFACTORY_H

#include "programinfo.h"
#include <QList>
class TestProgram;

class TestProgramFactory {
    public:
        static QList<TestProgram*> createPrograms(const QList<ProgramInfo>& programInfoList);
};

#endif // TESTPROGRAMFACTORY_H
