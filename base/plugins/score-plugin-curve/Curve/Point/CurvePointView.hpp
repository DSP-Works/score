#pragma once
#include <score/model/Identifier.hpp>
#include <wobjectdefs.h>
#include <score/model/IdentifiedObjectMap.hpp>

#include <QGraphicsItem>
#include <QPoint>
#include <QRect>

class QGraphicsSceneContextMenuEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
namespace Curve
{
class PointModel;
struct Style;
class PointView final
    : public QObject
    , public QGraphicsItem
{
  W_OBJECT(PointView)
  Q_INTERFACES(QGraphicsItem)
public:
  PointView(
      const PointModel* model,
      const Curve::Style& style,
      QGraphicsItem* parent);

  const PointModel& model() const;
  const Id<PointModel>& id() const;

  static constexpr int static_type()
  {
    return QGraphicsItem::UserType + 100;
  }
  int type() const override
  {
    return static_type();
  }

  QRectF boundingRect() const override;
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

  void setSelected(bool selected);

  void enable();
  void disable();

  void setModel(const PointModel* model);

public:
  void contextMenuRequested(const QPoint& arg_1, const QPointF& arg_2) E_SIGNAL(, contextMenuRequested, arg_1, arg_2);

protected:
  void contextMenuEvent(QGraphicsSceneContextMenuEvent*) override;

private:
  const PointModel* m_model;
  const Curve::Style& m_style;
  bool m_selected{};
};
}

extern template class IdContainer<Curve::PointView, Curve::PointModel>;
