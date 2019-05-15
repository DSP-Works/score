#pragma once
#include <Process/Style/ScenarioStyle.hpp>
#include <Scenario/Document/State/StateView.hpp>
#include <Scenario/Document/State/StatePresenter.hpp>
#include <Scenario/Document/State/StateModel.hpp>

#include <QBrush>
#include <QGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QPen>

namespace Scenario
{

class StatePlusOverlay final : public QGraphicsItem
{
public:
  StatePlusOverlay(StateView* parent) : QGraphicsItem{parent}
  {
    this->setAcceptHoverEvents(true);
  }

  static constexpr int static_type()
  {
    return QGraphicsItem::UserType + ItemType::StateOverlay;
  }

  int type() const override { return static_type(); }

  QRectF boundingRect() const override
  {
    return {-1, -1, 16, 16};
  }

  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override
  {
    auto& skin = Process::Style::instance();

    painter->setRenderHint(QPainter::Antialiasing, true);

    const auto& pending_brush = skin.EventPending.getBrush();
    painter->setBrush(pending_brush);
    const auto bright = pending_brush.color();
    QPen p{bright.darker(300)};
    p.setWidth(2);
    painter->setPen(p);

    // TODO instead of a cross, make an arrow that looks like |->
    const auto small_rad = m_big ? 3. : 2.;

    const auto l1 =
        m_big
        ? QLineF{QPointF{15 - small_rad, 0}, QPointF{15 - small_rad, 2 * small_rad}}.translated(-3, 1)
        : QLineF{QPointF{15 - small_rad, 0}, QPointF{15 - small_rad, 2 * small_rad}}.translated(-4, 2);
    const auto l2 =
        m_big
        ? QLineF{QPointF{15 - 2 * small_rad, small_rad}, QPointF{15, small_rad}}.translated(-3, 1)
        : QLineF{QPointF{15 - 2 * small_rad, small_rad}, QPointF{15, small_rad}}.translated(-4, 2);

    painter->drawLine(l1.translated(1, 1));
    painter->drawLine(l2.translated(1, 1));
    p.setColor(bright);
    painter->setPen(p);
    painter->drawLine(l1);
    painter->drawLine(l2);

    painter->setRenderHint(QPainter::Antialiasing, false);
  }

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override
  {
    auto st = static_cast<StateView*>(parentItem());
    st->presenter().select();
    if(auto p = event->pos(); p.x() > 4 && p.y() < 10)
    {
      st->startCreateMode();
    }

    event->ignore();
  }

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override
  {
    auto p = event->pos();
    m_big = p.x() > 4 && p.y() < 10;
    update();
  }

  void hoverMoveEvent(QGraphicsSceneHoverEvent* event) override
  {
    auto p = event->pos();
    m_big = p.x() > 4 && p.y() < 10;
    update();
  }
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override
  {
    m_big = false;
    update();
  }

  bool m_big{false};
};
}
