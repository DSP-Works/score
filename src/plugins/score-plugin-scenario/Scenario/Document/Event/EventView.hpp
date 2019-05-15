#pragma once
#include "ExecutionStatus.hpp"

#include <Scenario/Document/Event/ConditionView.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentViewConstants.hpp>
#include <Scenario/Document/VerticalExtent.hpp>

#include <score/model/ColorReference.hpp>
#include <score/widgets/MimeData.hpp>

#include <QGraphicsItem>
#include <QPoint>
#include <QRect>
#include <QString>
#include <QtGlobal>

#include <score_plugin_scenario_export.h>
#include <wobjectdefs.h>
class QGraphicsSceneDragDropEvent;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QMimeData;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;

namespace Scenario
{
class ConditionView;
class EventPresenter;
class SCORE_PLUGIN_SCENARIO_EXPORT EventView final : public QObject,
                                                     public QGraphicsItem
{
  W_OBJECT(EventView)
  Q_INTERFACES(QGraphicsItem)

public:
  EventView(EventPresenter& presenter, QGraphicsItem* parent);
  ~EventView() override;

  static constexpr int static_type()
  {
    return QGraphicsItem::UserType + ItemType::Event;
  }
  int type() const override { return static_type(); }

  const EventPresenter& presenter() const { return m_presenter; }

  QRectF boundingRect() const override
  {
    return {-1, 0., 6, m_height};
  }

  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

  void setSelected(bool selected);
  bool isSelected() const;

  void setCondition(const QString& cond);
  bool hasCondition() const;
  ConditionView& conditionItem() noexcept { return m_conditionItem; }

  void setExtent(const VerticalExtent& extent);
  void setExtent(VerticalExtent&& extent);

  void setWidthScale(double);
  void changeToolTip(const QString&);

public:
  void eventHoverEnter()
      E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, eventHoverEnter)
  void eventHoverLeave()
      E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, eventHoverLeave)

  void dropReceived(const QPointF& pos, const QMimeData& arg_2)
      E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, dropReceived, pos, arg_2)

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* h) override;
  void hoverLeaveEvent(QGraphicsSceneHoverEvent* h) override;

  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

private:
  EventPresenter& m_presenter;
  ConditionView m_conditionItem;
  QString m_condition;
  score::ColorRef m_color;
  double m_height{};

  bool m_selected{};
};
}
