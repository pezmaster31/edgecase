#ifndef PROGRAMTYPESELECTOR_H
#define PROGRAMTYPESELECTOR_H

#include "programinfo.h"
#include <QStringList>

class ProgramTypeSelector {
    public:
        static QList<ProgramInfo> getInfo(const QStringList& filepaths);
};

#endif // PROGRAMTYPESELECTOR_H
