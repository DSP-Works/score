#pragma once
#include <Process/LayerView.hpp>
#include <wobjectdefs.h>
#include <QString>
#include <score_lib_process_export.h>

class QGraphicsItem;
class QPainter;
class QQuickView;
class QGraphicsProxyWidget;
class QQuickItem;

namespace WidgetLayer
{
class WidgetTextItem;
class SCORE_LIB_PROCESS_EXPORT View final : public Process::LayerView
{
  W_OBJECT(View)
public:
  explicit View(QGraphicsItem* parent);

  void setWidget(QWidget*);

public:
  void contextMenuRequested(QPoint arg_1) W_SIGNAL(contextMenuRequested, arg_1);

private:
  void paint_impl(QPainter*) const override;
  void mousePressEvent(QGraphicsSceneMouseEvent*) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent*) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent*) override;
  void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

  QGraphicsProxyWidget* m_widg{};
};
}
