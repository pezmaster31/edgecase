#ifndef TESTRUNNER_H
#define TESTRUNNER_H

#include <QList>
#include <QMap>
#include <QMetaType>
#include <QObject>
#include <QString>
class TestProgram;

class TestRunner : public QObject {

    Q_OBJECT

    // ctor & dtor
    public:
        explicit TestRunner(QObject* parent = 0);
        ~TestRunner(void);

    // signals
    signals:
        void listTestsStarted(void);
        void listTestsFinished(void);

        void runTestsStarted(void);
        void runTestsFinished(void);

        void testResultsReady(TestProgram* program);

        void progressRangeChanged(int min, int max);
        void progressValueChanged(int value);

    // TestRunner interface
    public slots:
        void listTests(QString directory, bool shouldRecurse);
        void runTests(void);
    public:

        // TestProgram access
        int programCount(void) const;
        TestProgram* programAt(int index) const;
        TestProgram* programForName(const QString& name) const;

        // convenience counts (total numbers across all of our test programs)
        int failedTestCount(void) const;
        int runTestCount(void) const;
        int passedTestCount(void) const;
        int totalTestCount(void) const;

    // internal methods
    private slots:
        void onProgramListingReady(TestProgram* program);
        void onProgramResultsReady(TestProgram* program);
    private:
        bool allProgramsFinished(void) const;
        int finishedProgramCount(void) const;
        void removeAllTests(void);
        void updateProgress(TestProgram* program);

    // data members
    private:
        enum TaskType { NotRunning = 0
                      , ListTests
                      , RunTests
                      };
        TaskType m_currentTask;

        QList<TestProgram*> m_programs;
        QMap<TestProgram*, bool> m_isRunningMap;

};

Q_DECLARE_METATYPE(TestRunner*)

#endif // TESTRUNNER_H
