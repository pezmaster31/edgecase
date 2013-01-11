#include "testsuite.h"
#include "testcase.h"
#include <QtCore>
#include <QtDebug>

// --------------------------
// TestSuite implementation
// --------------------------

TestSuite::TestSuite(const QString& name)
    : m_name(name)
    , m_time(-1.0)
{ }

TestSuite::~TestSuite(void) {
    while ( !m_tests.isEmpty() ) {
        TestCase* test = m_tests.takeFirst();
        if ( test )
            delete test;
    }
}

void TestSuite::addTest(TestCase* test) {
    m_tests.append(test);
}

void TestSuite::clearResults(void) {

    m_time = -1.0;

    foreach ( TestCase* test, m_tests ) {
        Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");
        test->clearResults();
    }
}

QString TestSuite::name(void) const {
    return m_name;
}

int TestSuite::failedTestCount(void) const {
    int count = 0;
    foreach ( TestCase* test, m_tests ) {
        Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");
        if ( test->wasRun() && !test->passed() )
            ++count;
    }
    return count;
}

bool TestSuite::hasEnabledTests(void) const {
    foreach ( TestCase* test, m_tests ) {
        Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");
        if ( test->isEnabled() )
            return true;
    }
    return false;
}

bool TestSuite::hasFailedTests(void) const {
    foreach ( TestCase* test, m_tests ) {
        Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");
        if ( test->wasRun() && !test->passed() )
            return true;
    }
    return false;
}

bool TestSuite::hasRunTests(void) const {
    foreach ( TestCase* test, m_tests ) {
        Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");
        if ( test->wasRun() )
            return true;
    }
    return false;
}

bool TestSuite::hasTime(void) const {
    return m_time >= 0.0;
}

int TestSuite::passedTestCount(void) const {
    int count = 0;
    foreach ( TestCase* test, m_tests ) {
        Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");
        if ( test->wasRun() && test->passed() )
            ++count;
    }
    return count;
}

int TestSuite::runTestCount(void) const {
    int count = 0;
    foreach ( TestCase* test, m_tests ) {
        Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");
        if ( test->wasRun() )
            ++count;
    }
    return count;
}

void TestSuite::setTime(qreal t) {
    m_time = t;
}

TestCase* TestSuite::testAt(int index) const {
    Q_ASSERT_X(index >= 0 && index < testCount(), Q_FUNC_INFO, "invalid index");
    return m_tests.at(index);
}

int TestSuite::testCount(void) const {
    return m_tests.size();
}

TestCase* TestSuite::testForName(const QString& name) const {
    foreach ( TestCase* test, m_tests ) {
        Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");
        if ( test->name() == name )
            return test;
    }
    return 0;
}

qreal TestSuite::time(void) const {
    return m_time;
}
