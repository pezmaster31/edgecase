#include "qtestlibprogram.h"
#include "testcase.h"
#include "testsuite.h"
#include <QtCore>
#include <QtDebug>

// ----------------------------------
// QTestLibProgram implementation
// ----------------------------------

QTestLibProgram::QTestLibProgram(const QString& filepath, QObject* parent)
    : TestProgram(filepath, parent)
{ }

QTestLibProgram::~QTestLibProgram(void) { }

QStringList QTestLibProgram::listingArgs(void) const {
    return QStringList() << "-datatags";
}

QMap<QString, QStringList> QTestLibProgram::parseTestListing(QByteArray output, QStringList* errors) {

    Q_ASSERT_X(errors, Q_FUNC_INFO, "null errror list");
    errors->clear();

    QMap<QString, QStringList> listing;

    // parse output
    QString line;
    QStringList fields;
    QString suiteName;
    QString testName;
    QBuffer outputBuffer(&output);
    outputBuffer.open(QBuffer::ReadOnly);
    while ( outputBuffer.canReadLine() ) {
        line = outputBuffer.readLine();

        // remove leading & trailing whitespace
        line = line.trimmed();

        // get names from line:
        // TestSuiteName TestCaseName
        fields = line.split(' ');
        if ( fields.size() != 2 ) {
            errors->append("Malformed listing");
            return listing;
        }
        suiteName = fields.at(0);
        testName  = fields.at(1);

        // add to listing
        // (if new suite, initialize its test list with default Qt fxn)
        if ( !listing.contains(suiteName) ) {
            QStringList intialList = QStringList() << "initTestCase";
            listing.insert(suiteName, intialList);
        }
        listing[suiteName].append(testName);
    }

    // foreach suite, append the other default Qt fxn
    QMap<QString, QStringList>::iterator mapIter = listing.begin();
    QMap<QString, QStringList>::iterator mapEnd  = listing.end();
    for ( ; mapIter != mapEnd; ++mapIter ) {
        QStringList& testList = mapIter.value();
        testList.append("cleanupTestCase");
    }

    // return result
    return listing;
}

bool QTestLibProgram::parseTestResults(QStringList* errors) {

    Q_ASSERT_X(errors, Q_FUNC_INFO, "null string list");
    errors->clear();

    // open XML file
    QFile xmlFile(xmlFilename());
    if ( !xmlFile.open(QFile::ReadOnly) ) {
        errors->append(xmlFile.errorString());
        return false;
    }

    // parse XML file for program results
    // N.B. - QTestLib is really set up for 1 test suite per program.
    //        So I'm calling the readSuiteResult() method to stay consistent w/
    //        the equivalent logic in GoogleTestProgram XML parsing
    m_xml.setDevice(&xmlFile);
    return readSuiteResult(errors);
}

bool QTestLibProgram::readSuiteResult(QStringList* errors) {

    // fetch top-level "TestCase" element
    if ( !m_xml.readNextStartElement() || m_xml.name().toString() != "TestCase") {
        errors->append("File is not readable as QTestLib result");
        return false;
    }

    // fetch test suite for this element
    const QXmlStreamAttributes& attributes = m_xml.attributes();
    const QString& suiteName = attributes.value("name").toString();
    TestSuite* suite = suiteForName(suiteName);
    if ( suite == 0 ) {
        errors->append(QString("Could not find test suite listing for ")+suiteName);
        return false;
    }

    // read through "TestFunction" elements within top-level "TestCase"  (testcases in our testsuite)
    while ( !m_xml.atEnd() && m_xml.readNextStartElement() ) {

        // if "TestFunction" element, attempt to parse (ignore other elements)
        if ( m_xml.name().toString() == "TestFunction" ) {
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

bool QTestLibProgram::readTestResult(TestSuite* suite, QStringList* errors) {

    // sanity check
    Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");
    Q_ASSERT_X(m_xml.isStartElement() && m_xml.name().toString() == "TestFunction",
               Q_FUNC_INFO, "unexpected root element here");

    // fetch test case for this element
    const QXmlStreamAttributes attributes = m_xml.attributes();
    const QString& testName = attributes.value("name").toString();
    TestCase* test = suite->testForName(testName);
    if ( test == 0 ) {
        errors->append(QString("Could not find test case listing for ")+testName);
        return false;
    }

    // set wasRun flag
    test->setWasRun(true);

    // parse child elements (Incidents, Messages, & Benchmarks) to set other attributes
    while ( !m_xml.atEnd() && m_xml.readNextStartElement() ) {

        // BenchmarkResult
        if ( m_xml.name().toString() == "BenchmarkResult" ) {
            const QXmlStreamAttributes bmAttr = m_xml.attributes();
            const QString& valueString = bmAttr.value("value").toString();
            const QString& iterString  = bmAttr.value("iterations").toString();
            if ( !valueString.isEmpty() && !iterString.isEmpty()  ) {
                const qreal  total      = valueString.toDouble();
                const qint32 iterations = iterString.toInt();
                const QString msg = QString("Benchmark : %1 msec per iteration (total: %2, iterations: %3)")
                        .arg( total / iterations )
                        .arg( total )
                        .arg( iterations );
                test->addBenchmarkMessage(msg);
            }
            m_xml.skipCurrentElement();
        }

        // Incident
        else if ( m_xml.name().toString() == "Incident" ) {
            const QXmlStreamAttributes incidentAttr = m_xml.attributes();
            const QString& resultType = incidentAttr.value("type").toString();

            // pass
            if ( resultType == "pass" ) {
                test->setPassed(true);
            }

            // fail
            else if ( resultType == "fail" ) {
                test->setPassed(false);

                // read failure messages from "Description" element
                while ( !m_xml.atEnd() && m_xml.readNextStartElement() ) {
                    if ( m_xml.name().toString() == "Description" ) {
                        if ( m_xml.readNext() && m_xml.isCDATA() ) {
                            const QString& msg = m_xml.text().toString();
                            test->addFailureMessage(msg);
                        }
                    } else
                        m_xml.skipCurrentElement();
                }
            }

            // unknown type
            else {
                errors->append(QString("Unknown incident type ")+resultType);
                return false;
            }

            m_xml.skipCurrentElement();
        }

        // Message
        else if ( m_xml.name().toString() == "Message" ) {
            const QXmlStreamAttributes messageAttr = m_xml.attributes();
            const QString& msgType = messageAttr.value("type").toString();

            // read 'other' messages from "Description" element
            while ( !m_xml.atEnd() && m_xml.readNextStartElement() ) {
                if ( m_xml.name().toString() == "Description" ) {
                    if ( m_xml.readNext() && m_xml.isCDATA() ) {
                        const QString& contents = m_xml.text().toString();
                        const QString& msg = QString("%1 : %2").arg(msgType).arg(contents);
                        test->addOtherMessage(msg);
                    }
                } else
                    m_xml.skipCurrentElement();
            }

            m_xml.skipCurrentElement();
        }

        // otherwise, unknown/unused element
        else
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

QStringList QTestLibProgram::runTestArgs(void) const {
    QStringList args;
    args << "-xml";
    args << "-o" << xmlFilename();
    return args;
}
