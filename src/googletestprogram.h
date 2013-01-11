#ifndef GOOGLETESTPROGRAM_H
#define GOOGLETESTPROGRAM_H

#include "testprogram.h"
#include <QXmlStreamReader>

class GoogleTestProgram : public TestProgram {

    Q_OBJECT

    // ctor & dtor
    public:
        GoogleTestProgram(const QString& filepath, QObject* parent = 0);
        ~GoogleTestProgram(void);

    // TestProgram implementation
    protected:

        // provide command line args for each run type
        QStringList listingArgs(void) const;
        QStringList runTestArgs(void) const;

        // derived classes should output
        QMap<QString, QStringList> parseTestListing(QByteArray output, QStringList* errors);
        bool parseTestResults(QStringList* errors);

    // internal methods
    private:
        QString filterArg(void) const;
        bool readProgramResult(QStringList* errors);
        bool readSuiteResult(QStringList* errors);
        bool readTestResult(TestSuite* suite, QStringList* errors);

    // data members
    private:
        QXmlStreamReader m_xml;
};

#endif // GOOGLETESTPROGRAM_H
