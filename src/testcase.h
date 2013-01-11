#ifndef TESTCASE_H
#define TESTCASE_H

#include <QMetaType>
#include <QStringList>

class TestCase {

    // ctor & dtor
    public:
        TestCase(const QString& name);
        ~TestCase(void);

    // TestCase interface
    public:

        // name
        QString name(void) const;

        // time
        qreal time(void) const;
        void setTime(qreal t);
        bool hasTime(void) const;

        // wasRun
        bool wasRun(void) const;
        void setWasRun(bool ok = true);

        // passed
        bool passed(void) const;
        void setPassed(bool ok = true);

        // isEnabled
        bool isEnabled(void) const;
        void setEnabled(bool ok = true);

        // failures
        void addFailureMessage(const QString& msg);
        QStringList failureMessages(void) const;
        bool hasFailureMessages(void) const;

        // benchmarks
        void addBenchmarkMessage(const QString& msg);
        QStringList benchmarkMessages(void) const;
        bool hasBenchmarkMessages(void) const;

        // 'other' messages
        void addOtherMessage(const QString& msg);
        QStringList otherMessages(void) const;
        bool hasOtherMessages(void) const;

        // add'l methods
        void clearResults(void);

    // data members
    private:
        QString m_name;
        qreal m_time;
        bool m_wasRun;
        bool m_passed;
        bool m_isEnabled;
        QStringList m_failureMessages;
        QStringList m_benchmarkMessages;
        QStringList m_otherMessages;
};

Q_DECLARE_METATYPE(TestCase*)

#endif // TESTCASE_H
