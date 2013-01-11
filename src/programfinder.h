#ifndef PROGRAMFINDER_H
#define PROGRAMFINDER_H

#include <QStringList>

class ProgramFinder {

    public:
        static QStringList lookupExecutables(const QString& startingDirectory,
                                             bool shouldRecurse = true);
        static bool isProbablyLibrary(const QString& filepath);
};

#endif // PROGRAMFINDER_H
