#pragma once
#include <Curve/CurveView.hpp>
#include <wobjectdefs.h>
#include <Process/LayerView.hpp>
#include <QTextLayout>

namespace InterpState
{
class View final : public Process::LayerView
{
  W_OBJECT(View)
public:
  explicit View(QGraphicsItem* parent);
  ~View() override;
  void setCurveView(Curve::View* view)
  {
    m_curveView = view;
  }

private:
  QPixmap pixmap() noexcept override;
  void paint_impl(QPainter* painter) const override;
  void dropEvent(QGraphicsSceneDragDropEvent* event) override;

  Curve::View* m_curveView;
};
}
