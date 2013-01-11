#include "mainwindow.h"
#include "resultdetailsview.h"
#include "testlistview.h"
#include "testprogram.h"
#include "testprogressbar.h"
#include "testrunner.h"
#include <QtGui>
#include <QtDebug>

// ---------------------------
// MainWindow implementation
// ---------------------------

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_runner(new TestRunner(this))
    , m_testListView(new TestListView(m_runner))
    , m_resultDetails(new ResultDetailsView)
    , m_progressBar(new TestProgressBar)
    , m_openAction(new QAction(QIcon(":/icons/open"), "Open test directory", this))
    , m_runAction(new QAction(QIcon(":/icons/run"),  "Run all tests", this))
    , m_testCountLabel(new QLabel(""))
    , m_runCountLabel(new QLabel(""))
    , m_passCountLabel(new QLabel(""))
    , m_failCountLabel(new QLabel(""))
    , m_lastDirectoryUsed("")
    , m_lastShouldRecurseChoice(true)
{
    // --------------------------------
    // setup UI
    // --------------------------------

    QToolBar* toolbar = new QToolBar;
    toolbar->setIconSize(QSize(32,32));
    toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolbar->addAction(m_openAction);
    toolbar->addAction(m_runAction);
    addToolBar(toolbar);
    m_runAction->setEnabled(false);

    QLabel* testHeaderLabel = new QLabel("<b>Tests</b>");
    QLabel* runHeaderLabel  = new QLabel("<b>Run</b>");
    QLabel* passHeaderLabel = new QLabel("<b>Passed</b>");
    QLabel* failHeaderLabel = new QLabel("<b>Failed</b>");

    QGridLayout* labelGridLayout = new QGridLayout;
    labelGridLayout->addWidget(testHeaderLabel,  0, 0, 1, 1, Qt::AlignCenter);
    labelGridLayout->addWidget(runHeaderLabel,   0, 1, 1, 1, Qt::AlignCenter);
    labelGridLayout->addWidget(passHeaderLabel,  0, 2, 1, 1, Qt::AlignCenter);
    labelGridLayout->addWidget(failHeaderLabel,  0, 3, 1, 1, Qt::AlignCenter);
    labelGridLayout->addWidget(m_testCountLabel, 1, 0, 1, 1, Qt::AlignCenter);
    labelGridLayout->addWidget(m_runCountLabel,  1, 1, 1, 1, Qt::AlignCenter);
    labelGridLayout->addWidget(m_passCountLabel, 1, 2, 1, 1, Qt::AlignCenter);
    labelGridLayout->addWidget(m_failCountLabel, 1, 3, 1, 1, Qt::AlignCenter);

    QVBoxLayout* resultLayout = new QVBoxLayout;
    resultLayout->addWidget(m_progressBar);
    resultLayout->addLayout(labelGridLayout);

    QSplitter* splitter = new QSplitter(Qt::Horizontal);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(m_testListView);
    splitter->addWidget(m_resultDetails);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(resultLayout);
    layout->addWidget(splitter, 9);

    // initialize grayed-filled progress bar so it doesn't look like an input field
    m_progressBar->grayOut();

    QWidget* w = new QWidget;
    w->setLayout(layout);
    setCentralWidget(w);

    setMinimumSize(900, 700);
    setWindowTitle("Edgecase");

    // --------------------------------
    // make connections
    // --------------------------------

    connect(m_openAction, SIGNAL(triggered()), this,     SLOT(openDirectory()));
    connect(m_runAction,  SIGNAL(triggered()), m_runner, SLOT(runTests()));

    connect(m_runner, SIGNAL(listTestsStarted()),  this, SLOT(onListTestsStarted()));
    connect(m_runner, SIGNAL(listTestsFinished()), this, SLOT(onListTestsFinished()));
    connect(m_runner, SIGNAL(runTestsStarted()),   this, SLOT(onRunTestsStarted()));
    connect(m_runner, SIGNAL(runTestsFinished()),  this, SLOT(onRunTestsFinished()));
    connect(m_runner, SIGNAL(testResultsReady(TestProgram*)), this, SLOT(onTestResultsReady(TestProgram*)));

    connect(m_runner, SIGNAL(listTestsStarted()),  m_testListView, SLOT(onListTestsStarted()));
    connect(m_runner, SIGNAL(listTestsFinished()), m_testListView, SLOT(onListTestsFinished()));
    connect(m_runner, SIGNAL(runTestsStarted()),   m_testListView, SLOT(onRunTestsStarted()));
    connect(m_runner, SIGNAL(runTestsFinished()),  m_testListView, SLOT(onRunTestsFinished()));
    connect(m_runner, SIGNAL(testResultsReady(TestProgram*)), m_testListView, SLOT(onTestResultsReady(TestProgram*)));

    connect(m_runner, SIGNAL(listTestsStarted()),  m_resultDetails, SLOT(onListTestsStarted()));
    connect(m_runner, SIGNAL(listTestsFinished()), m_resultDetails, SLOT(onListTestsFinished()));
    connect(m_runner, SIGNAL(runTestsStarted()),   m_resultDetails, SLOT(onRunTestsStarted()));
    connect(m_runner, SIGNAL(runTestsFinished()),  m_resultDetails, SLOT(onRunTestsFinished()));

    connect(m_runner, SIGNAL(listTestsStarted()),            m_progressBar, SLOT(reset()));
    connect(m_runner, SIGNAL(runTestsStarted()),             m_progressBar, SLOT(reset()));
    connect(m_runner, SIGNAL(listTestsFinished()),           m_progressBar, SLOT(setFinished()));
    connect(m_runner, SIGNAL(runTestsFinished()),            m_progressBar, SLOT(setFinished()));
    connect(m_runner, SIGNAL(progressRangeChanged(int,int)), m_progressBar, SLOT(setRange(int,int)));
    connect(m_runner, SIGNAL(progressValueChanged(int)),     m_progressBar, SLOT(setValue(int)));

    connect(m_testListView,  SIGNAL(testCaseSelected(TestCase*)),
            m_resultDetails, SLOT(onTestCaseSelected(TestCase*)));

    connect(m_testListView,  SIGNAL(testSuiteSelected(TestSuite*)),
            m_resultDetails, SLOT(onTestSuiteSelected(TestSuite*)));

    connect(m_testListView,  SIGNAL(testProgramSelected(TestProgram*)),
            m_resultDetails, SLOT(onTestProgramSelected(TestProgram*)));
}

MainWindow::~MainWindow(void) { }

void MainWindow::disableActions(void) {
    m_openAction->setEnabled(false);
    m_runAction->setEnabled(false);
}

void MainWindow::enableActions(void) {
    m_openAction->setEnabled(true);
    m_runAction->setEnabled(true);
}

void MainWindow::onListTestsFinished(void) {

    enableActions();

    // initialize count labels
    const int totalTestCount = m_runner->totalTestCount();
    m_testCountLabel->setText(QString::number(totalTestCount));
    m_runCountLabel->setText("0");
    m_passCountLabel->setText("0");
    m_failCountLabel->setText("0");
}

void MainWindow::onListTestsStarted(void) {

    disableActions();

    // clear all count labels
    m_testCountLabel->clear();
    m_runCountLabel->clear();
    m_passCountLabel->clear();
    m_failCountLabel->clear();
}

void MainWindow::onRunTestsFinished(void) {
    enableActions();
}

void MainWindow::onRunTestsStarted(void) {

    disableActions();

    // clear count labels (leave the total count alone)
    m_runCountLabel->clear();
    m_passCountLabel->clear();
    m_failCountLabel->clear();
}

void MainWindow::onTestResultsReady(TestProgram* program) {

    // check for any errors, update progress bar
    if ( program->hasFailedTests() )
        m_progressBar->setError(true);

    // fetch result summary & update labels
    const int totalTestCount = m_runner->totalTestCount();
    const int totalRunCount  = m_runner->runTestCount();
    const int totalPassCount = m_runner->passedTestCount();
    const int totalFailCount = m_runner->failedTestCount();
    m_testCountLabel->setText(QString::number(totalTestCount));
    m_runCountLabel->setText(QString::number(totalRunCount));
    m_passCountLabel->setText(QString::number(totalPassCount));
    m_failCountLabel->setText(QString::number(totalFailCount));
}

void MainWindow::openDirectory(void) {

    // run 'chooser' dialog
    DirectoryChooserDialog dialog(m_lastDirectoryUsed, m_lastShouldRecurseChoice);

    // if 'OK'
    if ( dialog.exec() == QDialog::Accepted ) {

        // get input from dialog
        m_lastDirectoryUsed       = dialog.directory();
        m_lastShouldRecurseChoice = dialog.shouldRecurse();

        // initialize with test listing
        m_runner->listTests(m_lastDirectoryUsed, m_lastShouldRecurseChoice);
    }
}

// ---------------------------------------
// DirectoryChooserDialog implementation
// ---------------------------------------

DirectoryChooserDialog::DirectoryChooserDialog(const QString& startingDir,
                                               bool shouldRecurse,
                                               QWidget* parent)
    : QDialog(parent)
    , m_lineEdit(new QLineEdit)
    , m_browseButton(new QPushButton("..."))
    , m_recurseCheckBox(new QCheckBox("&Recurse subdirectories"))
{
    // ----------
    // setup UI
    // ----------

    QLabel* note = new QLabel("<b>Directory containing executable(s)</b>");
    QLabel* desc = new QLabel("Type a path (or use the browse button).\n");
    QHBoxLayout* inputLayout = new QHBoxLayout;
    inputLayout->addWidget(m_lineEdit,     8);
    inputLayout->addWidget(m_browseButton, 1);
    QVBoxLayout* chooserLayout = new QVBoxLayout;
    chooserLayout->addWidget(note, 0, Qt::AlignLeft);
    chooserLayout->addWidget(desc, 0, Qt::AlignLeft);
    chooserLayout->addLayout(inputLayout);
    chooserLayout->addWidget(m_recurseCheckBox);

    QDialogButtonBox* buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, SIGNAL(accepted()), SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), SLOT(reject()));

    QVBoxLayout* dialogLayout = new QVBoxLayout;
    dialogLayout->addLayout(chooserLayout);
    dialogLayout->addWidget(buttonBox/*, 0, Qt::AlignRight | Qt::AlignBottom*/);
    setLayout(dialogLayout);

    m_lineEdit->setText(startingDir);
    m_recurseCheckBox->setChecked(shouldRecurse);

    setMinimumWidth(600);

    // ------------------
    // make connections
    // ------------------

    connect(m_browseButton, SIGNAL(clicked()), SLOT(onBrowseButtonClicked()));
}

DirectoryChooserDialog::~DirectoryChooserDialog(void) { }

QString DirectoryChooserDialog::directory(void) const {
    return m_lineEdit->text();
}

void DirectoryChooserDialog::onBrowseButtonClicked(void) {

    const QString& currentDirectory = directory();

    // let user select a directory
    const QString title = "Select Directory";
    const QString startingPath = ( currentDirectory.isEmpty() ? QDir::homePath() : currentDirectory );
    const QString dir = QFileDialog::getExistingDirectory(this, title, startingPath);

    // if canceled or no change, return
    if ( dir.isEmpty() || dir == currentDirectory )
        return;

    // otherwise update line edit
    m_lineEdit->setText(dir);
}

bool DirectoryChooserDialog::shouldRecurse(void) const {
    return m_recurseCheckBox->isChecked();
}
