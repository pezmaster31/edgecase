#ifndef TESTPROGRESSBAR_H
#define TESTPROGRESSBAR_H

#include <QWidget>

class TestProgressBar : public QWidget {

    Q_OBJECT

    // ctor & dtor
    public:
        explicit TestProgressBar(QWidget* parent = 0);
        ~TestProgressBar(void);

    // TestProgressBar interface
    public slots:
        void grayOut(void);  // convenience method - grays out a full bar
        void reset(void);    // resets value to range minimum

        void setFinished(bool ok = true);
        void setRange(int min, int max);
        void setValue(int value);
        void setError(bool ok = true);
    public:
        bool hasError(void) const   { return m_hasError; }
        bool isFinished(void) const { return m_isFinished; }
        int maximum(void) const     { return m_maximum; }
        int minimum(void) const     { return m_minimum; }
        int value(void) const       { return m_value; }

    // QWidget-based stuff
    public:
        QSize sizeHint(void) const;
        void paintEvent(QPaintEvent* event);

    // data members
    private:
        int m_maximum;
        int m_minimum;
        int m_value;
        bool m_isFinished;
        bool m_hasError;
};

#endif // TESTPROGRESSBAR_H
