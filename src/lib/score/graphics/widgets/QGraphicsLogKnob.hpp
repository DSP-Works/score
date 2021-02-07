#pragma once
#include <score/graphics/widgets/Constants.hpp>

#include <QObject>
#include <QGraphicsItem>
#include <verdigris>

#include <score_lib_base_export.h>

namespace score
{
class SCORE_LIB_BASE_EXPORT QGraphicsLogKnob final : public QObject, public QGraphicsItem
{
    W_OBJECT(QGraphicsLogKnob)
    Q_INTERFACES(QGraphicsItem)
    friend struct DefaultGraphicsKnobImpl;

    double m_value{};
    QRectF m_rect{defaultKnobSize};

public:
    double min{}, max{};

private:
    bool m_grab{};

public:
    QGraphicsLogKnob(QGraphicsItem* parent);

    double map(double v) const noexcept;
    double unmap(double v) const noexcept;

    void setRect(const QRectF& r);
    void setRange(double min, double max);
    void setValue(double v);
    double value() const;
    QRectF boundingRect() const override;

    bool moving = false;

public:
    void valueChanged(double arg_1) E_SIGNAL(SCORE_LIB_BASE_EXPORT, valueChanged, arg_1)
    void sliderMoved() E_SIGNAL(SCORE_LIB_BASE_EXPORT, sliderMoved)
    void sliderReleased() E_SIGNAL(SCORE_LIB_BASE_EXPORT, sliderReleased)

    private:
        void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};
}
