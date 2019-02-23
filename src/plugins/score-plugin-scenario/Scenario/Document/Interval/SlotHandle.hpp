#pragma once
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentViewConstants.hpp>

#include <QGraphicsItem>
#include <QPen>
#include <QRect>
#include <QtGlobal>

#include <score_plugin_scenario_export.h>
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
namespace Process
{
class Port;
}
namespace Scenario
{
class IntervalPresenter;
class TemporalIntervalPresenter;

class SCORE_PLUGIN_SCENARIO_EXPORT SlotHandle final : public QGraphicsItem
{
public:
  SlotHandle(
      const IntervalPresenter& slotView,
      int slotIndex,
      bool isstatic,
      QGraphicsItem* parent);

  const IntervalPresenter& presenter() const { return m_presenter; }
  static constexpr int static_type()
  {
    return QGraphicsItem::UserType + ItemType::SlotHandle;
  }
  int type() const override { return static_type(); }

  int slotIndex() const;
  void setSlotIndex(int);
  static constexpr double handleHeight() { return 4.; }

  QRectF boundingRect() const override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

  void setWidth(qreal width);

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) final override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) final override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) final override;

  const IntervalPresenter& m_presenter;
  qreal m_width{};
  int m_slotIndex{};
  bool m_static{};
};
}
