#include "programfinder.h"
#include <QtCore>

QStringList ProgramFinder::lookupExecutables(const QString& startingDirectory,
                                             bool shouldRecurse)
{
    // for some reason, QFileInfo and friends don't like directory paths
    // that don't end with a separator
    QString lookupDirectory = startingDirectory;
    if ( !lookupDirectory.endsWith(QDir::separator()) )
        lookupDirectory.append(QDir::separator());

    // skip out early if our path is not a directory
    const QFileInfo fi(lookupDirectory);
    if ( !fi.exists() || !fi.isDir() )
        return QStringList();

    QDir dir = fi.dir();
    dir.setFilter(QDir::Files | QDir::Executable);
    QStringList executableFilenames;

    // if we should recurse through all subdirectories
    if ( shouldRecurse ) {
        QDirIterator dirIter(dir, QDirIterator::Subdirectories);
        while ( dirIter.hasNext() ) {
            dirIter.next();
            const QFileInfo exe = dirIter.fileInfo();
            executableFilenames.append( exe.canonicalFilePath() );
        }
    }

    // otherwise, look in top level only
    else {
        const QFileInfoList exes = dir.entryInfoList();
        foreach ( const QFileInfo& exe, exes )
            executableFilenames.append( exe.canonicalFilePath() );
    }

    // remove all duplicates
    executableFilenames.removeDuplicates();

    // attempt to remove libraries
    // these are picked up as 'executable' by directory scan, but we can eliminate common lib types
    QStringList result;
    foreach ( const QString& exeFn, executableFilenames ) {
        if ( !isProbablyLibrary(exeFn) )
            result.append(exeFn);
    }

    // return our resulting filepaths
    return result;
}

bool ProgramFinder::isProbablyLibrary(const QString& filepath) {

    // fetch filename (only) from filepath
    const QFileInfo fi(filepath);
    const QString filename = fi.fileName();

    // check common, known library naming conventions
    if ( filename.endsWith(".so",    Qt::CaseInsensitive) ) return true;
    if ( filename.contains(".so.",   Qt::CaseInsensitive) ) return true;
    if ( filename.endsWith(".a",     Qt::CaseInsensitive) ) return true;
    if ( filename.endsWith(".dylib", Qt::CaseInsensitive) ) return true;
    if ( filename.endsWith(".dll",   Qt::CaseInsensitive) ) return true;
    if ( filename.endsWith(".lib",   Qt::CaseInsensitive) ) return true;
    if ( filename.endsWith(".ocx",   Qt::CaseInsensitive) ) return true;

    // otherwise probably not a library (though still may not be a test executable)
    // but we can at least eliminate showing these to the user
    return false;
}
