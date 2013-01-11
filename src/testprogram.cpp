#include "testprogram.h"
#include "testcase.h"
#include "testsuite.h"
#include <QtCore>
#include <QtDebug>

// ----------------------------
// TestProgram implementation
// ----------------------------

TestProgram::TestProgram(const QString& filename, QObject* parent)
    : QObject(parent)
    , m_filename(filename)
    , m_time(-1.0)
    , m_currentTask(TestProgram::NoTask)
    , m_process(new QProcess(this))
{
    connect(m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            SLOT(onProcessFinished(int,QProcess::ExitStatus)));
}

TestProgram::~TestProgram(void) {
    removeAllSuites();
}

void TestProgram::addSuite(TestSuite* suite) {
    m_suites.append(suite);
}

void TestProgram::clearResults(void) {
    m_time = -1.0;
    foreach ( TestSuite* suite, m_suites ) {
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
        suite->clearResults();
    }
}

int TestProgram::failedTestCount(void) const {
    int result = 0;
    foreach ( TestSuite* suite, m_suites ) {
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
        result += suite->failedTestCount();
    }
    return result;
}

QString TestProgram::fileName(void) const {
    return m_filename;
}

bool TestProgram::hasEnabledTests(void) const {
    foreach ( TestSuite* suite, m_suites ) {
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
        if ( suite->hasEnabledTests() )
            return true;
    }
    return false;
}

bool TestProgram::hasFailedTests(void) const {
    foreach ( TestSuite* suite, m_suites ) {
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
        if ( suite->hasFailedTests() )
            return true;
    }
    return false;
}

bool TestProgram::hasRunTests(void) const {
    foreach ( TestSuite* suite, m_suites ) {
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
        if ( suite->hasRunTests() )
            return true;
    }
    return false;
}

bool TestProgram::hasTime(void) const {
    return m_time >= 0.0;
}

void TestProgram::initializeListing(const QMap<QString, QStringList>& listingMap) {

    // clear prior listing data
    removeAllSuites();

    // iterate over new listing data
    QMap<QString, QStringList>::const_iterator mapIter = listingMap.constBegin();
    QMap<QString, QStringList>::const_iterator mapEnd  = listingMap.constEnd();
    for ( ; mapIter != mapEnd; ++mapIter ) {
        const QString suiteName = mapIter.key();
        const QStringList testNames = mapIter.value();

        // create new suite
        TestSuite* suite = new TestSuite(suiteName);

        // add test cases to suite
        foreach ( const QString& testName, testNames ) {
            TestCase* test = new TestCase(testName);
            suite->addTest(test);
        }

        // add suite to program
        addSuite(suite);
    }
}

QProcess::ProcessChannel TestProgram::listingOutputChannel(void) const {
    return QProcess::StandardOutput;
}

void TestProgram::listTests(void) {

    // skip if our process is still running
    if ( m_process->state() != QProcess::NotRunning )
        return;

    // set our state & start process (w/ args from derived class)
    m_currentTask = TestProgram::ListTests;
    const QStringList args = listingArgs();
    m_process->start(m_filename, args);
}

void TestProgram::onProcessFinished(int exitCode, QProcess::ExitStatus status) {

    // TODO: error handling
    Q_UNUSED(exitCode);
    Q_UNUSED(status);

    // handle process completion depending on our current task
    switch ( m_currentTask ) {

        case TestProgram::ListTests :
        {
            // read listing output from standard out/error
            const QByteArray output = ( listingOutputChannel() == QProcess::StandardOutput
                                          ? m_process->readAllStandardOutput()
                                          : m_process->readAllStandardError()
                                      );

            // parse listing from program output
            QStringList errors;
            QMap<QString, QStringList> listing = parseTestListing(output, &errors);

            // if no errors, set up suite/test case hierachy & emit signal
            if ( errors.isEmpty() ) {
                initializeListing(listing);
                emit listingReady(this);
            }

            // else report error msgs
            else {
                qDebug() << "Could not get test listing: ";
                foreach ( const QString& e, errors )
                    qDebug() << e;
            }

            break;
        }

        case TestProgram::RunTests :
        {
            QStringList errors;
            if ( parseTestResults(&errors) )
                emit resultsReady(this);
            else {
                qDebug() << "Could not parse results: ";
                foreach ( const QString& e, errors )
                    qDebug() << e;
            }

            break;
        }

        default:
            Q_ASSERT_X(false, Q_FUNC_INFO, "unexpected task type");
            break;
    }

    // reset our task state
    m_currentTask = TestProgram::NoTask;
}

int TestProgram::passedTestCount(void) const {
    int result = 0;
    foreach ( TestSuite* suite, m_suites ) {
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
        result += suite->passedTestCount();
    }
    return result;
}

QString TestProgram::programName(void) const {
    return QFileInfo(m_filename).fileName();
}

void TestProgram::removeAllSuites(void) {
    while ( !m_suites.isEmpty() ) {
        TestSuite* suite = m_suites.takeFirst();
        if ( suite )
            delete suite;
    }
}

void TestProgram::removeXmlFile(void) const {
    const QString& xml = xmlFilename();
    if ( QFileInfo(xml).exists() )
        QFile::remove(xml);
}


int TestProgram::runTestCount(void) const {
    int result = 0;
    foreach ( TestSuite* suite, m_suites ) {
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
        result += suite->runTestCount();
    }
    return result;
}

void TestProgram::runTests(void) {

    // skip if our process is still running
    if ( m_process->state() != QProcess::NotRunning )
        return;

    // clear out any prior data
    clearResults();

    // set our state & start process (w/ args from derived class)
    m_currentTask = TestProgram::RunTests;
    const QStringList args = runTestArgs();
    m_process->start(m_filename, args);
}

void TestProgram::setTime(qreal t) {
    m_time = t;
}

TestSuite* TestProgram::suiteAt(int index) const {
    Q_ASSERT_X(index >= 0 && index < suiteCount(), Q_FUNC_INFO, "invalid index");
    return m_suites.at(index);
}

int TestProgram::suiteCount(void) const {
    return m_suites.size();
}

TestSuite* TestProgram::suiteForName(const QString& name) const {
    foreach ( TestSuite* suite, m_suites ) {
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
        if ( suite->name() == name )
            return suite;
    }
    return 0;
}

qreal TestProgram::time(void) const {
    return m_time;
}

int TestProgram::totalTestCount(void) const {
    int result = 0;
    foreach ( TestSuite* suite, m_suites ) {
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
        result += suite->testCount();
    }
    return result;
}

QString TestProgram::xmlFilename(void) const {
    return m_filename + ".xml";
}
