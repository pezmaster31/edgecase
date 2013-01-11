#include "testcase.h"
#include <QtCore>
#include <QtDebug>

// -------------------------
// TestCase implementation
// -------------------------

TestCase::TestCase(const QString& name)
    : m_name(name)
    , m_time(-1.0)
    , m_wasRun(false)
    , m_passed(false)
    , m_isEnabled(true)
{ }

TestCase::~TestCase(void) { }

void TestCase::addBenchmarkMessage(const QString& msg) {
    m_benchmarkMessages.append(msg);
}

void TestCase::addFailureMessage(const QString& msg) {
    m_failureMessages.append(msg);
}

void TestCase::addOtherMessage(const QString& msg) {
    m_otherMessages.append(msg);
}

QStringList TestCase::benchmarkMessages(void) const {
    return m_benchmarkMessages;
}

void TestCase::clearResults(void) {
    m_wasRun = false;
    m_passed = false;
    m_time = -1.0;
    m_benchmarkMessages.clear();
    m_failureMessages.clear();
    m_otherMessages.clear();
}

QStringList TestCase::failureMessages(void) const {
    return m_failureMessages;
}

bool TestCase::hasBenchmarkMessages(void) const {
    return !m_benchmarkMessages.isEmpty();
}

bool TestCase::hasFailureMessages(void) const {
    return !m_failureMessages.isEmpty();
}

bool TestCase::hasOtherMessages(void) const {
    return !m_otherMessages.isEmpty();
}

bool TestCase::hasTime(void) const {
    return m_time >= 0.0;
}

bool TestCase::isEnabled(void) const {
    return m_isEnabled;
}

QString TestCase::name(void) const {
    return m_name;
}

QStringList TestCase::otherMessages(void) const {
    return m_otherMessages;
}

bool TestCase::passed(void) const {
    return m_passed;
}

void TestCase::setEnabled(bool ok) {
    m_isEnabled = ok;
}

void TestCase::setPassed(bool ok) {
    m_passed = ok;
}

void TestCase::setTime(qreal t) {
    m_time = t;
}

void TestCase::setWasRun(bool ok) {
    m_wasRun = ok;
}

qreal TestCase::time(void) const {
    return m_time;
}

bool TestCase::wasRun(void) const {
    return m_wasRun;
}
