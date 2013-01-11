#include "googletestprogram.h"
#include "testcase.h"
#include "testsuite.h"
#include <QtCore>
#include <QtDebug>

// ----------------------------------
// GoogleTestProgram implementation
// ----------------------------------

GoogleTestProgram::GoogleTestProgram(const QString& filepath, QObject* parent)
    : TestProgram(filepath, parent)
{ }

GoogleTestProgram::~GoogleTestProgram(void) { }

QString GoogleTestProgram::filterArg(void) const {

    // TODO: check here for 'enabled' suites & tests

    QString filter;
    QTextStream s(&filter);

    const QLatin1String dot(".");
    const QLatin1String colon(":");

    // for each suite
    const int numSuites = suiteCount();
    for ( int i = 0; i < numSuites; ++i ) {
        TestSuite* suite = suiteAt(i);
        Q_ASSERT_X(suite, Q_FUNC_INFO, "null suite");

        const QString& suiteName = suite->name();

        // for each test in suite
        const int numTests = suite->testCount();
        for ( int j = 0; j < numTests; ++j ) {
            TestCase* test = suite->testAt(j);
            Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");

            s << suiteName << dot << test->name() << colon;
        }
    }

    // if not empty, remove the trailing colon
    if ( !filter.isEmpty() )
        filter.chop(1);

    // return result
    return filter;
}

QStringList GoogleTestProgram::listingArgs(void) const {
    return QStringList() << "--gtest_list_tests";
}


QMap<QString, QStringList> GoogleTestProgram::parseTestListing(QByteArray output, QStringList* errors) {

    Q_ASSERT_X(errors, Q_FUNC_INFO, "null errror list");
    errors->clear();

    QMap<QString, QStringList> listing;

    // parse output
    QString currentSuiteName;
    QString line;
    QBuffer outputBuffer(&output);
    outputBuffer.open(QBuffer::ReadOnly);
    while ( outputBuffer.canReadLine() ) {
        line = outputBuffer.readLine();

        // remove leading & trailing whitespaces
        line = line.trimmed();

        // if TestSuite name
        if ( line.endsWith('.') ) {
            line.chop(1);

             // add suite to listing
            const QString& suiteName = line;
            if ( !listing.contains(suiteName) )
                listing.insert(suiteName, QStringList());

            // store suite name for adding test cases
            currentSuiteName = suiteName;
        }

        // otherwise, should be a TestCase name
        else {

            // sanity check
            // we should have hit a suite name & stored it before reaching a test case name
            // if not, append error & skip out... no sense in proceeding, something got out of whack
            if ( currentSuiteName.isEmpty() || !listing.contains(currentSuiteName) ) {
                errors->append("Malformed listing - found suite name before test case name");
                return listing;
            }

            // add test name to suite
            // operator[] is OK here, since we just checked contains() a few lines back
            const QString& testName = line;
            listing[currentSuiteName].append(testName);
        }
    }

    // return result
    return listing;
}

bool GoogleTestProgram::parseTestResults(QStringList* errors) {

    Q_ASSERT_X(errors, Q_FUNC_INFO, "null string list");
    errors->clear();

    // open XML file
    QFile xmlFile(xmlFilename());
    if ( !xmlFile.open(QFile::ReadOnly) ) {
        errors->append(xmlFile.errorString());
        return false;
    }

    // parse XML file for program results
    m_xml.setDevice(&xmlFile);
    return readProgramResult(errors);
}

bool GoogleTestProgram::readProgramResult(QStringList* errors) {

    // fetch top-level "testsuites" element
    if ( !m_xml.readNextStartElement() || m_xml.name() != "testsuites") {
        errors->append("File is not readable as GoogleTest result");
        return false;
    }

    // set time elapsed for program
    const QXmlStreamAttributes& attributes = m_xml.attributes();
    const QString& timeString = attributes.value("time").toString();
    if ( !timeString.isEmpty() )
        setTime(timeString.toDouble());

    // read through "testsuite" elements within top-level "testsuites"
    while ( !m_xml.atEnd() && m_xml.readNextStartElement() ) {

        // if "testsuite" element, attempt to parse (ignore other elements)
        if ( m_xml.name() == "testsuite" ) {
            if ( !readSuiteResult(errors) )
                return false;
        } else
            m_xml.skipCurrentElement();

        // catch any XML reader errors
        if ( m_xml.hasError() ) {
            errors->append(m_xml.errorString());
            return false;
        }
    }

    // if we get here, should be OK
    return true;
}

bool GoogleTestProgram::readSuiteResult(QStringList* errors) {

    // sanity check
    Q_ASSERT_X(m_xml.isStartElement() && m_xml.name() == "testsuite",
               Q_FUNC_INFO, "unexpected root element here");

    // fetch test suite for this element
    const QXmlStreamAttributes& attributes = m_xml.attributes();
    const QString& suiteName = attributes.value("name").toString();
    TestSuite* suite = suiteForName(suiteName);
    if ( suite == 0 ) {
        errors->append(QString("Could not find test suite listing for ")+suiteName);
        return false;
    }

    // set time elapsed for test suite
    const QString& timeString = attributes.value("time").toString();
    if ( !timeString.isEmpty() )
        suite->setTime(timeString.toDouble());

    // read through "testcase" elements withing our "testsuite"
    while ( !m_xml.atEnd() && m_xml.readNextStartElement() ) {

        // if "testcase" element, attempt to parse (ignore other elements)
        if ( m_xml.name() == "testcase" ) {
            if ( !readTestResult(suite, errors) )
                return false;
        } else
            m_xml.skipCurrentElement();

        // catch any XML reader errors
        if ( m_xml.hasError() ) {
            errors->append(m_xml.errorString());
            return false;
        }
    }

    // if we get here, should be OK
    return true;
}

bool GoogleTestProgram::readTestResult(TestSuite* suite, QStringList* errors) {

    // sanity check
    Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
    Q_ASSERT_X(m_xml.isStartElement() && m_xml.name() == "testcase",
               Q_FUNC_INFO, "unexpected root element here");

    // fetch test case for this element
    const QXmlStreamAttributes attributes = m_xml.attributes();
    const QString& testName = attributes.value("name").toString();
    TestCase* test = suite->testForName(testName);
    if ( test == 0 ) {
        errors->append(QString("Could not find test case listing for ")+testName);
        return false;
    }

    // set time elapsed for test case
    const QString& timeString = attributes.value("time").toString();
    if ( !timeString.isEmpty() )
        test->setTime(timeString.toDouble());

    // if not run, set status accordingly
    const QString& status = attributes.value("status").toString();
    if ( status == "notrun" )
        test->setWasRun(false);
    // otherwise, check for failure messages
    else {
        test->setPassed(true);
        test->setWasRun(true);

        // read through remaining 'start' elements in testcase
        while ( !m_xml.atEnd() && m_xml.readNextStartElement() ) {

            // parse "failure" element  (ignore other elements)
            if ( m_xml.name() == "failure" ) {
                const QString& msg = m_xml.readElementText();

                // add message to our test & update its status flag
                test->addFailureMessage(msg);
                test->setPassed(false);
            } else
                m_xml.skipCurrentElement();

            // catch any XML reader errors
            if ( m_xml.hasError() ) {
                errors->append(m_xml.errorString());
                return false;
            }
        }
    }

    // if we get here, should be OK
    return true;
}

QStringList GoogleTestProgram::runTestArgs(void) const {

    QStringList args;

    // add our XML output filename (making sure we're starting with a fresh one)
    removeXmlFile();
    args << QString("--gtest_output=xml:%1").arg(xmlFilename());

    // add any test filters that apply
    const QString filter = filterArg();
    if ( !filter.isEmpty() )
        args << QString("--gest_filter=%1").arg(filter);

    // return arg list
    return args;
}
