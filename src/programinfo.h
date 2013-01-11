#ifndef PROGRAMINFO_H
#define PROGRAMINFO_H

#include <QString>

struct ProgramInfo {

    // enums
    enum ProgramType { Unknown = 0
                     , GoogleTest
                     , QtTestLib
                     };

    // data members
    QString filename;
    ProgramType type;

    // ctors & dtor
    ProgramInfo(const QString& fn = QString(),
                ProgramType t = ProgramInfo::Unknown)
        : filename(fn)
        , type(t)
    { }

    ProgramInfo(const ProgramInfo& other)
        : filename(other.filename)
        , type(other.type)
    { }

    ~ProgramInfo(void) { }
};


#endif // PROGRAMINFO_H
