#pragma once
#include <QLabel>

class QSlantedLabel : public QLabel
{
    Q_OBJECT
    Q_PROPERTY(qreal rotation READ rotation WRITE setRotation)

public:
    explicit QSlantedLabel(QWidget *parent = nullptr);
    qreal rotation() const { return m_rotation; }
    void setRotation(qreal degrees);

    QSize sizeHint() const override;
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    qreal m_rotation = 0.0; // degrees
};
