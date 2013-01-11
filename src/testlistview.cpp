#include "testlistview.h"
#include "testcase.h"
#include "testprogram.h"
#include "testrunner.h"
#include "testsuite.h"
#include <QtGui>
#include <QtDebug>

// -----------------------------
// TestListView implementation
// -----------------------------

TestListView::TestListView(TestRunner* runner, QWidget* parent)
    : QTreeWidget(parent)
    , m_runner(runner)
    , m_passColor("#98fc66")
    , m_failColor("#f44800")
    , m_noResultColor("#aaaaaa")
{
    // hide our header, unused & unecessary visual clutter
    header()->setVisible(false);

    // set our default size policy to fill available space
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    // intercept 'current item changed' signal to translate to specific testcase-type signal
    connect(this, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this, SLOT(onCurrentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)));    
}

TestListView::~TestListView(void) { }

QTreeWidgetItem* TestListView::itemForProgram(TestProgram* program) {

    // sanity check
    if ( program == 0 )
        return 0;

    // iterate over top-level (program-level) tree items
    const int programCount = topLevelItemCount();
    for ( int i = 0; i < programCount; ++i ) {
        QTreeWidgetItem* programItem = topLevelItem(i);
        Q_ASSERT_X(programItem, Q_FUNC_INFO, "null tree item");

        // if tree item's data matches requested program, return it
        const QVariant itemData = programItem->data(0, Qt::UserRole);
        Q_ASSERT_X(itemData.canConvert<TestProgram*>(), Q_FUNC_INFO, "item/data mismatch");
        TestProgram* itemProgram = itemData.value<TestProgram*>();
        if ( itemProgram == program )
            return programItem;
    }

    // not found
    return 0;
}

void TestListView::onCurrentItemChanged(QTreeWidgetItem* current,
                                        QTreeWidgetItem* previous)
{
    Q_UNUSED(previous);
    if ( current == 0 )
        return;

    // fetch item's data (we always use column 0)
    QVariant itemData = current->data(0, Qt::UserRole);

    // TestProgram
    if ( itemData.canConvert<TestProgram*>() ) {
        TestProgram* program = itemData.value<TestProgram*>();
        emit testProgramSelected( program  );
    }

    // TestSuite
    else if ( itemData.canConvert<TestSuite*>() ) {
        TestSuite* suite = itemData.value<TestSuite*>();
        emit testSuiteSelected(suite);
    }

    // TestCase
    else if ( itemData.canConvert<TestCase*>() ) {
        TestCase* test = itemData.value<TestCase*>();
        emit testCaseSelected(test);
    }
}

void TestListView::onListTestsStarted(void) {
    clear();
}

void TestListView::onListTestsFinished(void) {

    // sanity check
    Q_ASSERT_X(m_runner, Q_FUNC_INFO, "null test runner");
    if ( m_runner == 0 )
        return;

    // clear any previous data (necessary here?)
    clear();

    // foreach program
    const int numPrograms = m_runner->programCount();
    for ( int i = 0; i < numPrograms; ++i ) {
        TestProgram* program = m_runner->programAt(i);
        Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");

        // create top-level item for program
        QTreeWidgetItem* programItem = new QTreeWidgetItem;
        programItem->setData(0, Qt::DisplayRole, program->programName());
        programItem->setData(0, Qt::UserRole, QVariant::fromValue(program));
//        programItem->setCheckState(0, Qt::Checked);

        // foreach test suite
        const int numSuites = program->suiteCount();
        for ( int j = 0; j < numSuites; ++j ) {
            TestSuite* suite = program->suiteAt(j);
            Q_ASSERT_X(suite, Q_FUNC_INFO, "null test suite");

            // create item for suite
            QTreeWidgetItem* suiteItem = new QTreeWidgetItem(programItem);
            suiteItem->setData(0, Qt::DisplayRole, suite->name());
            suiteItem->setData(0, Qt::UserRole, QVariant::fromValue(suite));
//            suiteItem->setCheckState(0, Qt::Checked);

            // foreach test case
            const int numTests = suite->testCount();
            for ( int k = 0; k < numTests; ++k ) {
                TestCase* test = suite->testAt(k);
                Q_ASSERT_X(test, Q_FUNC_INFO, "null test case");

                // create item for test
                QTreeWidgetItem* testItem = new QTreeWidgetItem(suiteItem);
                testItem->setData(0, Qt::DisplayRole, test->name());
                testItem->setData(0, Qt::UserRole, QVariant::fromValue(test));
//                testItem->setCheckState(0, Qt::Checked);
            }
        }

        // add program item to table
        addTopLevelItem(programItem);
    }

    // start with all items collapsed
    collapseAll();
}

void TestListView::onRunTestsStarted(void) {

    // gray out all tests
    const int programCount = topLevelItemCount();
    for ( int i = 0; i < programCount; ++i ) {
        QTreeWidgetItem* programItem = topLevelItem(i);
        Q_ASSERT_X(programItem, Q_FUNC_INFO, "null tree item");
        programItem->setBackgroundColor(0, m_noResultColor);

        const int suiteCount = programItem->childCount();
        for ( int j = 0; j < suiteCount; ++j ) {
            QTreeWidgetItem* suiteItem = programItem->child(j);
            Q_ASSERT_X(suiteItem, Q_FUNC_INFO, "null tree item");
            suiteItem->setBackgroundColor(0, m_noResultColor);

            const int testCount = suiteItem->childCount();
            for ( int k = 0; k < testCount; ++k ) {
                QTreeWidgetItem* testItem = suiteItem->child(k);
                Q_ASSERT_X(testItem, Q_FUNC_INFO, "null tree item");
                testItem->setBackgroundColor(0, m_noResultColor);
            }
        }
    }
}

void TestListView::onRunTestsFinished(void) { }

void TestListView::onTestResultsReady(TestProgram* program) {

    // sanity check
    Q_ASSERT_X(program, Q_FUNC_INFO, "null test program");
    if ( program == 0 )
        return;

    // fetch tree item for requested program
    QTreeWidgetItem* programItem = itemForProgram(program);
    if ( programItem == 0 )
        return;
    updateItemForResults(programItem);

    // update program's children (suites & test cases)
    const int suiteCount = programItem->childCount();
    for ( int j = 0; j < suiteCount; ++j ) {
        QTreeWidgetItem* suiteItem = programItem->child(j);
        if ( suiteItem )
            updateItemForResults(suiteItem);

        // foreach test case
        const int testCount = suiteItem->childCount();
        for ( int k = 0; k < testCount; ++k ) {
            QTreeWidgetItem* testItem = suiteItem->child(k);
            if ( testItem )
                updateItemForResults(testItem);
        }
    }

    // select program item if none selected yet
    QTreeWidgetItem* item = currentItem();
    if ( item == 0 )
        setCurrentItem(programItem);
}

void TestListView::updateItemForResults(QTreeWidgetItem*item) {

    Q_ASSERT_X(item, Q_FUNC_INFO, "null item");
    const QVariant itemData = item->data(0, Qt::UserRole);

    // TestProgram
    if ( itemData.canConvert<TestProgram*>() ) {
        TestProgram* program = itemData.value<TestProgram*>();
        if ( !program->hasRunTests() )
            item->setBackgroundColor(0, m_noResultColor);
        else {
            if ( program->hasFailedTests() )
                item->setBackgroundColor(0, m_failColor);
            else
                item->setBackgroundColor(0, m_passColor);
        }
    }

    // TestSuite
    else if ( itemData.canConvert<TestSuite*>() ) {
        TestSuite* suite = itemData.value<TestSuite*>();
        if ( !suite->hasRunTests() )
            item->setBackgroundColor(0, m_noResultColor);
        else {
            if ( suite->hasFailedTests() )
                item->setBackgroundColor(0, m_failColor);
            else
                item->setBackgroundColor(0, m_passColor);
        }
    }

    // TestCase
    else if ( itemData.canConvert<TestCase*>() ) {
        TestCase* test = itemData.value<TestCase*>();
        if ( !test->wasRun() )
            item->setBackgroundColor(0, m_noResultColor);
        else {
            if ( !test->passed() )
                item->setBackgroundColor(0, m_failColor);
            else
                item->setBackgroundColor(0, m_passColor);
        }
    }
}
