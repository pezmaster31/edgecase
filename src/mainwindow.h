#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDialog>
#include <QMainWindow>
#include <QString>

class ResultDetailsView;
class TestListView;
class TestProgram;
class TestProgressBar;
class TestRunner;

class QAction;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

// our main application window
class MainWindow : public QMainWindow {

    Q_OBJECT
    
    // ctor & dtor
    public:
        explicit MainWindow(QWidget* parent = 0);
        ~MainWindow(void);

    // internal methods
    private slots:

        void onListTestsStarted(void);
        void onListTestsFinished(void);
        void onRunTestsStarted(void);
        void onRunTestsFinished(void);
        void onTestResultsReady(TestProgram* program);
        void openDirectory(void);
    private:
        void disableActions(void);
        void enableActions(void);

    // data members
    private:
        TestRunner*        m_runner;
        TestListView*      m_testListView;
        ResultDetailsView* m_resultDetails;
        TestProgressBar*   m_progressBar;

        QAction* m_openAction;
        QAction* m_runAction;
        QLabel*  m_testCountLabel;
        QLabel*  m_runCountLabel;
        QLabel*  m_passCountLabel;
        QLabel*  m_failCountLabel;

        QString m_lastDirectoryUsed;
        bool    m_lastShouldRecurseChoice;
};

// helper dialog
class DirectoryChooserDialog : public QDialog {

    Q_OBJECT

    // ctor & dtor
    public:
        explicit DirectoryChooserDialog(const QString& startingDir,
                                        bool shouldRecurse,
                                        QWidget* parent = 0);
        ~DirectoryChooserDialog(void);

    // DirectoryChooserDialog interface
    public:
        QString directory(void) const;
        bool shouldRecurse(void) const;

    // internal methods
    private slots:
        void onBrowseButtonClicked(void);

    // data members
    private:
        QLineEdit*   m_lineEdit;
        QPushButton* m_browseButton;
        QCheckBox*   m_recurseCheckBox;
};

#endif // MAINWINDOW_H
