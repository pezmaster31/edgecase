#ifndef QTESTLIBPROGRAM_H
#define QTESTLIBPROGRAM_H

#include "testprogram.h"
#include <QXmlStreamReader>

class QTestLibProgram : public TestProgram {

    Q_OBJECT

    // ctor & dtor
    public:
        QTestLibProgram(const QString& filepath, QObject* parent = 0);
        ~QTestLibProgram(void);

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
        bool readSuiteResult(QStringList* errors);
        bool readTestResult(TestSuite* suite, QStringList* errors);

    // data members
    private:
        QXmlStreamReader m_xml;
};

#endif // QTESTLIBPROGRAM_H
