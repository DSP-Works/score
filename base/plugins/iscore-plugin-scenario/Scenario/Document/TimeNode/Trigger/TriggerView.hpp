#pragma once
#include <QGraphicsItem>
class QGraphicsSvgItem;

class TriggerView final : public QGraphicsObject
{
        Q_OBJECT
    public:
        TriggerView(QGraphicsItem* parent);
        QRectF boundingRect() const override;
        void paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget) override;

        static constexpr int static_type()
        { return QGraphicsItem::UserType + 7; }
        int type() const override
        { return static_type(); }

    signals:
        void pressed();

    protected:
        void mousePressEvent(QGraphicsSceneMouseEvent*) override;

    private:
        QGraphicsSvgItem* m_item{};
};
