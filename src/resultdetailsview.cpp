#include "resultdetailsview.h"
#include "testcase.h"
#include "testprogram.h"
#include "testsuite.h"
#include <QtGui>
#include <QtDebug>

// ----------------------------------
// ResultDetailsView implementation
// ----------------------------------

ResultDetailsView::ResultDetailsView(QWidget* parent)
    : QTextEdit(parent)
{
    // default attributes
    setReadOnly(true);
}

ResultDetailsView::~ResultDetailsView(void) { }

void ResultDetailsView::onListTestsStarted(void) {
    setPlainText("Listing tests.");
}

void ResultDetailsView::onListTestsFinished(void) {
    setPlainText("Listing complete.\n\nPress run button to start tests.");
}

void ResultDetailsView::onRunTestsStarted(void) {
    setPlainText("Running tests.");
}

void ResultDetailsView::onRunTestsFinished(void) { }

void ResultDetailsView::onTestResultsReady(TestProgram* program) {
    Q_UNUSED(program);
}

void ResultDetailsView::onTestCaseSelected(TestCase* test) {

    // skip invalid test
    if ( test == 0 )
        return;

    // write test header
    writeHeader(test->name(),
                test->wasRun(),
                test->time(),
                !test->passed());

    // skip out if the test wasn't acutally run
    if ( !test->wasRun() )
        return;

    // write any failure messages
    if ( test->hasFailureMessages() ) {

        append("");
        append("Failures:");
        append("");

        const QStringList& messages = test->failureMessages();
        foreach ( const QString& msg, messages ) {
            append(msg);
            append("");
        }
    }

    // write any benchmark messages
    if ( test->hasBenchmarkMessages() ) {

        append("");
        append("Benchmarks: ");
        append("");

        const QStringList& benchmarks = test->benchmarkMessages();
        foreach ( const QString& bm, benchmarks ) {
            append(bm);
            append("");
        }
    }

    // write any other output
    if ( test->hasOtherMessages() ) {

        append("");
        append("Other messages: ");
        append("");

        const QStringList& messages = test->otherMessages();
        foreach ( const QString& msg, messages ) {
            append(msg);
            append("");
        }
    }
}

void ResultDetailsView::onTestSuiteSelected(TestSuite* suite) {
    if ( suite ) {
        writeHeader(suite->name(),
                    suite->hasRunTests(),
                    suite->time(),
                    suite->hasFailedTests());
    }
}

void ResultDetailsView::onTestProgramSelected(TestProgram* program) {
    if ( program ) {
        writeHeader(program->programName(),
                    program->hasRunTests(),
                    program->time(),
                    program->hasFailedTests());
    }
}

void ResultDetailsView::writeHeader(const QString& name,
                                    bool wasRun,
                                    qreal time,
                                    bool hasFailures)
{
    // clear any previous contents
    clear();

    // write name line
    const QString& nameLine = QString("Name: " ) + name;
    append(nameLine);

    // if no results, write message & skip out
    if ( !wasRun ) {
        append("No results available at this time.");
        return;
    }

    // write time line
    QString timeLine("Time: ");
    if ( time < 0.0 )
        timeLine.append("unknown");
    else
        timeLine.append( QString::number(time) + QString(" seconds") );
    append(timeLine);

    // write failures? line
    QString failureLine("Failures? : ");
    failureLine.append( (hasFailures ? QString("yes") : QString("no")) );
    append(failureLine);
}
