#pragma once
#include <ossia/editor/scenario/time_value.hpp>
#include <wobjectdefs.h>

#include <Process/TimeValue.hpp>
#include <QCache>
#include <QColor>
#include <QDateTime>
#include <QGlyphRun>
#include <QGraphicsItem>
#include <QMap>
#include <QPainterPath>
#include <QPoint>
#include <QString>
#include <QTextLayout>
#include <QtGlobal>
#include <Scenario/Document/TimeRuler/TimeRuler.hpp>
#include <chrono>
#include <deque>
#include <score/model/ColorReference.hpp>
class QGraphicsSceneMouseEvent;
class QPainter;
class QStyleOptionGraphicsItem;
class QWidget;
class QGraphicsView;

namespace Scenario
{
class TimeRuler final
    : public QObject
    , public QGraphicsItem
{
  W_OBJECT(TimeRuler)
  Q_INTERFACES(QGraphicsItem)

public:
  enum class Format
  {
    Seconds,
    Milliseconds
  };

  TimeRuler(QGraphicsView*);
  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override;

  void setWidth(qreal newWidth);

  qreal graduationSpacing() const
  {
    return m_intervalsBetweenMark * m_graduationsSpacing;
  }

  qreal width() const
  {
    return m_width;
  }

  void setStartPoint(TimeVal dur);
  void setPixelPerMillis(double factor);

public:
  void drag(QPointF arg_1, QPointF  arg_2) W_SIGNAL(drag, arg_1, arg_2);
  void rescale() W_SIGNAL(rescale);

private:
  void computeGraduationSpacing();

  QRectF boundingRect() const final override;
  void mousePressEvent(QGraphicsSceneMouseEvent*) final override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent*) final override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent*) final override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent*) final override;
  void createRulerPath();

  struct Mark
  {
    double pos;
    std::chrono::microseconds time;
    QGlyphRun text;
  };

  ossia::time_value m_startPoint{};

  double m_pixelPerMillis{0.01};

  std::vector<Mark> m_marks;

  qreal m_width{};

  qreal m_graduationsSpacing{};
  qreal m_graduationDelta{};
  qreal m_intervalsBetweenMark{};
  Format m_timeFormat{};

  QPainterPath m_path;

  QWidget* m_viewport{};

  QGlyphRun getGlyphs(std::chrono::microseconds);
  QTextLayout m_layout;
  std::deque<std::pair<std::chrono::microseconds, QGlyphRun>> m_stringCache;

  static const constexpr qreal graduationHeight = -15.;
  static const constexpr qreal timeRulerHeight = -2. * graduationHeight;
  static const constexpr qreal textPosition = 1.65 * graduationHeight;
};
}
