#ifndef RESULTDETAILSVIEW_H
#define RESULTDETAILSVIEW_H

#include <QTextEdit>
class QString;
class TestCase;
class TestSuite;
class TestProgram;

class ResultDetailsView : public QTextEdit {

    Q_OBJECT

    // ctor & dtor
    public:
        explicit ResultDetailsView(QWidget* parent = 0);
        ~ResultDetailsView(void);
    
    // ResultDetailsView interface
    public slots:
        void onListTestsStarted(void);
        void onListTestsFinished(void);
        void onRunTestsStarted(void);
        void onRunTestsFinished(void);
        void onTestResultsReady(TestProgram* program);

        void onTestCaseSelected(TestCase* test);
        void onTestSuiteSelected(TestSuite* suite);
        void onTestProgramSelected(TestProgram* program);

    // internal methods
    private:
        void writeHeader(const QString& name,
                         bool wasRun,
                         qreal time,
                         bool hasFailures);
};

#endif // RESULTDETAILSVIEW_H
