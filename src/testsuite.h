#ifndef TESTSUITE_H
#define TESTSUITE_H

#include <QList>
#include <QMetaType>
#include <QString>
class TestCase;

class TestSuite {

    // ctor & dtor
    public:
        TestSuite(const QString& name);
        ~TestSuite(void);

    // TestSuite interface
    public:
        // name
        QString name(void) const;

        // status
        bool hasEnabledTests(void) const;
        bool hasFailedTests(void) const;
        bool hasRunTests(void) const;

        // time
        bool hasTime(void) const;
        qreal time(void) const;
        void setTime(qreal t);

        // test accesss
        void addTest(TestCase* test);
        int testCount(void) const;
        TestCase* testAt(int index) const;
        TestCase* testForName(const QString& name) const;

        // convenience methods
        void clearResults(void);
        int runTestCount(void) const;
        int passedTestCount(void) const;
        int failedTestCount(void) const;

    // data members
    private:
        QString m_name;
        qreal   m_time;
        QList<TestCase*> m_tests;
};

Q_DECLARE_METATYPE(TestSuite*)

#endif // TESTSUITE_H
