#pragma once
#include <ossia/editor/automation/tinysplinecpp.h>

#include <Automation/Spline/SplineAutomModel.hpp>
#include <Process/LayerView.hpp>
namespace Spline
{
class View final : public Process::LayerView
{
  Q_OBJECT
public:
  View(QGraphicsItem* parent);

  void setSpline(ossia::nodes::spline_data d)
  {
    if (d != m_spline)
      m_spline = std::move(d);
    updateSpline();
    update();
  }

  const ossia::nodes::spline_data& spline() const
  {
    return m_spline;
  }

Q_SIGNALS:
  void changed();

private:
  void paint_impl(QPainter*) const override;
  void updateSpline();

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

  optional<std::size_t> findControlPoint(QPointF point);
  void addPoint(const QPointF point);
  template <typename T>
  QPointF mapToCanvas(const T& point) const
  {
    return QPointF(point.x() * width(), height() - point.y() * height());
  }
  ossia::nodes::spline_point mapFromCanvas(const QPointF& point) const;

  ossia::nodes::spline_data m_spline;
  tinyspline::BSpline m_spl;
  optional<std::size_t> m_clicked;
};
}
