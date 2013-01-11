#ifndef TESTLISTVIEW_H
#define TESTLISTVIEW_H

#include <QColor>
#include <QTreeWidget>
class TestCase;
class TestSuite;
class TestProgram;
class TestRunner;

class TestListView : public QTreeWidget {

    Q_OBJECT

    // ctor & dtor
    public:
        TestListView(TestRunner* runner, QWidget* parent = 0);
        ~TestListView(void);
    
    // signals
    signals:
        void testCaseSelected(TestCase* test);
        void testSuiteSelected(TestSuite* suite);
        void testProgramSelected(TestProgram* program);

    // TestListView interface
    public slots:
        void onListTestsStarted(void);
        void onListTestsFinished(void);
        void onRunTestsStarted(void);
        void onRunTestsFinished(void);
        void onTestResultsReady(TestProgram* program);

    // internal methods
    private slots:
        void onCurrentItemChanged(QTreeWidgetItem* current, QTreeWidgetItem* previous);
    private:
        QTreeWidgetItem* itemForProgram(TestProgram* program);
        void updateItemForResults(QTreeWidgetItem* item);

    // data members
    private:
        TestRunner* m_runner; // copy, not owned

        QColor m_passColor;
        QColor m_failColor;
        QColor m_noResultColor;
};

#endif // TESTLISTVIEW_H
