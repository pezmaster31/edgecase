#include "testrunner.h"
#include "programfinder.h"
#include "programinfo.h"
#include "programtypeselector.h"
#include "testprogram.h"
#include "testprogramfactory.h"
#include <QtCore>
#include <QtDebug>

// ---------------------------
// TestRunner implementation
// ---------------------------

TestRunner::TestRunner(QObject *parent)
    : QObject(parent)
    , m_currentTask(TestRunner::NotRunning)
{ }

TestRunner::~TestRunner(void) {
    removeAllTests();
}

bool TestRunner::allProgramsFinished(void) const {
    return finishedProgramCount() == m_isRunningMap.size();
}

int TestRunner::failedTestCount(void) const {
    int result = 0;
    foreach ( TestProgram* program, m_programs ) {
        Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");
        result += program->failedTestCount();
    }
    return result;
}

int TestRunner::finishedProgramCount(void) const {

    int numProgramsFinished = 0;

    // for each program that was to be run
    QMap<TestProgram*, bool>::const_iterator progIter = m_isRunningMap.constBegin();
    QMap<TestProgram*, bool>::const_iterator progEnd  = m_isRunningMap.constEnd();
    for ( ; progIter != progEnd; ++progIter ) {

        // if finished, update counter
        const bool isRunning = progIter.value();
        if ( !isRunning )
            ++numProgramsFinished;
    }

    return numProgramsFinished;
}

void TestRunner::listTests(QString directory, bool shouldRecurse) {

    // don't do anything if we're currently running
    if ( m_currentTask != TestRunner::NotRunning )
        return;

    // clear out any previous test programs
    removeAllTests();

    // set our current state
    m_currentTask = TestRunner::ListTests;

    // use helper utilites to create program objects from filenames
    const QStringList& filepaths              = ProgramFinder::lookupExecutables(directory, shouldRecurse);
    const QList<ProgramInfo>& programInfoList = ProgramTypeSelector::getInfo(filepaths);
    const QList<TestProgram*>& programList    = TestProgramFactory::createPrograms(programInfoList);

    // fire off initial progress notifications
    emit listTestsStarted();
    emit progressRangeChanged(0, programList.size());
    emit progressValueChanged(0);

    // set up each program created
    foreach ( TestProgram* p, programList ) {

        // store program
        m_programs.append(p);
        m_isRunningMap.insert(p, true);

        // make connections
        connect(p, SIGNAL(listingReady(TestProgram*)), SLOT(onProgramListingReady(TestProgram*)));
        connect(p, SIGNAL(resultsReady(TestProgram*)), SLOT(onProgramResultsReady(TestProgram*)));

        // produce listing
        p->listTests();
    }
}

void TestRunner::onProgramListingReady(TestProgram* program) {

    // sanity check
    Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");
    if ( program == 0 )
        return;

    // update progress tracking & emit signals
    updateProgress(program);

    // check for completion
    if ( allProgramsFinished() ) {

        // reset our state flag & progress tracking
        m_currentTask = TestRunner::NotRunning;
        m_isRunningMap.clear();

        // signal runner finished
        emit listTestsFinished();
    }
}

void TestRunner::onProgramResultsReady(TestProgram* program) {

    // sanity check
    Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");
    if ( program == 0 )
        return;

    // update progress tracking & emit signals
    updateProgress(program);

    // signal that results are ready for this program
    emit testResultsReady(program);

    // check for completion
    if ( allProgramsFinished() ) {

        // reset our state flag & progress tracking
        m_currentTask = TestRunner::NotRunning;
        m_isRunningMap.clear();

        // signal runner finished
        emit runTestsFinished();
    }
}

int TestRunner::passedTestCount(void) const {
    int result = 0;
    foreach ( TestProgram* program, m_programs ) {
        Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");
        result += program->passedTestCount();
    }
    return result;
}

int TestRunner::programCount(void) const {
    return m_programs.size();
}

TestProgram* TestRunner::programAt(int index) const {
    Q_ASSERT_X(index >= 0 && index < programCount(), Q_FUNC_INFO, "invalid index");
    return m_programs.at(index);
}

TestProgram* TestRunner::programForName(const QString& name) const {
    foreach ( TestProgram* program, m_programs ) {
        Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");
        if ( program->programName() == name )
            return program;
    }
    return 0;
}

void TestRunner::removeAllTests(void) {
    while ( !m_programs.isEmpty() ) {
        TestProgram* p = m_programs.takeFirst();
        Q_ASSERT_X(p, Q_FUNC_INFO, "null test program");
        if ( p )
            delete p;
    }
}

int TestRunner::runTestCount(void) const {
    int result = 0;
    foreach ( TestProgram* program, m_programs ) {
        Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");
        result += program->runTestCount();
    }
    return result;
}

void TestRunner::runTests(void) {

    // don't do anything if we're currently running
    if ( m_currentTask != TestRunner::NotRunning )
        return;

    // set our current state
    m_currentTask = TestRunner::RunTests;

    // determine our list of programs to run
    foreach ( TestProgram* program, m_programs ) {
        Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");
        if ( program->hasEnabledTests() )
            m_isRunningMap.insert(program, false);
    }

    // fire off initial progress notifications
    emit runTestsStarted();
    emit progressRangeChanged(0, m_isRunningMap.size());
    emit progressValueChanged(0);

    // for each program that has any enabled tests
    QMap<TestProgram*, bool>::iterator progIter = m_isRunningMap.begin();
    QMap<TestProgram*, bool>::iterator progEnd  = m_isRunningMap.end();
    for ( ; progIter != progEnd; ++progIter ) {

        // set progress tracking flag
        progIter.value() = true;

        // run tests
        TestProgram* p = progIter.key();
        p->runTests();
    }
}

int TestRunner::totalTestCount(void) const {
    int result = 0;
    foreach ( TestProgram* program, m_programs ) {
        Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");
        result += program->totalTestCount();
    }
    return result;
}

void TestRunner::updateProgress(TestProgram* program) {

    // update progress tracking structure
    Q_ASSERT_X(m_isRunningMap.contains(program), Q_FUNC_INFO, "unknown test program");
    m_isRunningMap[program] = false;

    // emit signals
    emit progressValueChanged( finishedProgramCount() );
}
