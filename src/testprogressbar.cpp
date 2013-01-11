#include "testprogressbar.h"
#include <QtGui>
#include <QtDebug>

namespace Constants {
    enum { ProgressBarHeightHint = 50
         , ProgressBarWidthHint  = 200
         };
} // namespace Constants

// --------------------------------
// TestProgressBar implementation
// --------------------------------

TestProgressBar::TestProgressBar(QWidget* parent)
    : QWidget(parent)
    , m_maximum(100)
    , m_minimum(1)
    , m_value(1)
    , m_isFinished(false)
    , m_hasError(false)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
}

TestProgressBar::~TestProgressBar(void) { }

void TestProgressBar::grayOut(void) {
    setRange(0,1);
    setValue(1);
    setError(false);
    setFinished(false);
}

void TestProgressBar::paintEvent(QPaintEvent* event) {

    Q_UNUSED(event);

    // calculate progress metrics
    const double range = m_maximum - m_minimum;
    double percent = 0.0;
    if ( range != 0.0 )
        percent = (m_value - m_minimum) / range;
    percent = qBound(0.0, percent, 1.0);
    if ( m_isFinished )
        percent = 1.0;

    QPainter p(this);

    // draw outline
    const QRect outerRect(0,0, size().width(), Constants::ProgressBarHeightHint);
    p.setPen(Qt::black);
    p.setBrush(QBrush(Qt::white));
    p.drawRect(outerRect);

    // draw progress bar
    QRect innerRect( outerRect.adjusted(2,2,-2,-2));
    innerRect.adjust(0,0,qRound((percent-1.0)*innerRect.width()), 0);
    QColor baseColor = Qt::lightGray;
    if ( m_hasError)
        baseColor = Qt::red;
    else if ( m_isFinished )
        baseColor = Qt::green;

    QLinearGradient gradient(innerRect.topLeft(), innerRect.bottomLeft());
    gradient.setColorAt(0,    baseColor.lighter(130));
    gradient.setColorAt(0.5,  baseColor.lighter(106));
    gradient.setColorAt(0.51, baseColor.darker(106));
    gradient.setColorAt(1,    baseColor.darker(130));
    p.setPen(Qt::NoPen);
    p.setBrush(gradient);
    p.drawRect(innerRect);
}

void TestProgressBar::reset(void) {
    m_value = m_minimum;
    m_hasError = false;
    m_isFinished = false;
    update();
}

void TestProgressBar::setError(bool ok) {
    if ( m_hasError == ok )
        return;
    m_hasError = ok;
    update();
}

void TestProgressBar::setFinished(bool ok) {
    if ( m_isFinished == ok )
        return;
    m_isFinished = ok;
    update();
}

void TestProgressBar::setRange(int min, int max) {

    if ( m_minimum == min && m_maximum == max )
        return;

    m_minimum = min;
    m_maximum = max;

    // sanity checks before updating (using same rules as QProgressBar)
    if ( m_value < m_minimum || m_value > m_maximum )
        m_value = m_minimum;
    if ( m_maximum < m_minimum )
        m_maximum = m_minimum;

    update();
}

void TestProgressBar::setValue(int value) {

    // sanity check
    if ( m_value == value || m_value < m_minimum || m_value > m_maximum)
        return;

    // update bar
    m_value = value;
    update();
}

QSize TestProgressBar::sizeHint(void) const {
    return QSize(Constants::ProgressBarWidthHint,
                 Constants::ProgressBarHeightHint);
}
