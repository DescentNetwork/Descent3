#include "slanted_label.h"

#include <QPainter>
#include <QFontMetrics>
#include <QtMath>

QSlantedLabel::QSlantedLabel(QWidget *parent)
    : QLabel(parent)
{}

void QSlantedLabel::setRotation(qreal degrees)
{
    if (!qFuzzyCompare(m_rotation + 1.0, degrees + 1.0)) {
        m_rotation = degrees;
        updateGeometry();
        update();
    }
}

QSize QSlantedLabel::sizeHint() const
{
    QString txt = text();
    if (txt.isEmpty())
        return QLabel::sizeHint();

    QFontMetrics fm(font());
    QRect r = fm.tightBoundingRect(txt);
    int w = r.width();
    int h = r.height();

    qreal theta = qDegreesToRadians(qAbs(m_rotation));
    qreal cosT = qCos(theta);
    qreal sinT = qSin(theta);
    int rw = qCeil(qAbs(w * cosT) + qAbs(h * sinT));
    int rh = qCeil(qAbs(w * sinT) + qAbs(h * cosT));

    // include margins
    int dx = contentsMargins().left() + contentsMargins().right();
    int dy = contentsMargins().top() + contentsMargins().bottom();
    return QSize(rw + dx, rh + dy);
}

void QSlantedLabel::paintEvent(QPaintEvent * /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    // Translate to center, rotate, translate back
    QPointF center(width() / 2.0, height() / 2.0);
    p.translate(center);
    p.rotate(m_rotation);
    p.translate(-center);

    // Draw using style so palettes, elide, alignment behave normally
    QRect r = rect();
    QStyleOption opt;
    opt.initFrom(this);
    style()->drawItemText(&p, r, alignment(), palette(), isEnabled(), text(), foregroundRole());
}
