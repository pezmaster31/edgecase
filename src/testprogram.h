#ifndef TESTPROGRAM_H
#define TESTPROGRAM_H

#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QProcess>
#include <QStringList>
class TestSuite;

class TestProgram : public QObject {

    Q_OBJECT

    // ctor & dtor
    protected:
        TestProgram(const QString& filename, QObject* parent = 0);
    public:
        virtual ~TestProgram(void);

    // signals
    signals:
        void listingReady(TestProgram* program);
        void resultsReady(TestProgram* program);

    // TestProgram interface
    public slots:
        void listTests(void);
        void runTests(void);
    public:
        QString fileName(void) const;     // '/full/path/to/test_exe'
        QString programName(void) const;  // 'test_exe'

        // status
        bool hasEnabledTests(void) const;
        bool hasFailedTests(void) const;
        bool hasRunTests(void) const;

        // time
        bool hasTime(void) const;
        qreal time(void) const;

        // TestSuite access
        int suiteCount(void) const;
        TestSuite* suiteAt(int index) const;
        TestSuite* suiteForName(const QString& name) const;

        // convenience counts (total numbers across all of our test suites)
        int failedTestCount(void) const;
        int runTestCount(void) const;
        int passedTestCount(void) const;
        int totalTestCount(void) const;

    // TestProgram 'shared' internals
    protected:

        enum TaskType { NoTask = 0
                      , ListTests
                      , RunTests
                      };

        // called by derived classes during parseTestResults()
        void setTime(qreal t);

        // used to read the QByteArray that gets passed to parseTestListing() - default: STDOUT
        virtual QProcess::ProcessChannel listingOutputChannel(void) const;

        // provide command line args for each run type
        virtual QStringList listingArgs(void) const =0;
        virtual QStringList runTestArgs(void) const =0;

        // derived classes should output
        virtual QMap<QString, QStringList> parseTestListing(QByteArray output, QStringList* errors) =0;
        virtual bool parseTestResults(QStringList* errors) =0;

        // XmlFile-related convience methods
        QString xmlFilename(void) const;
        void removeXmlFile(void) const;

    // TestProgram private internals
    private slots:
        void onProcessFinished(int exitCode, QProcess::ExitStatus status);
    private:
        void addSuite(TestSuite* suite);
        void clearResults(void);
        void initializeListing(const QMap<QString, QStringList>& listingMap);
        void removeAllSuites(void);

    // data members
    private:
        QString  m_filename;
        qreal    m_time;
        TaskType m_currentTask;
        QList<TestSuite*> m_suites;
        QProcess* m_process;
};

Q_DECLARE_METATYPE(TestProgram*)

#endif // TESTPROGRAM_H
