// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Process/Style/ScenarioStyle.hpp>

#include <QPainter>
#include <Spline/SplineView.hpp>
#include <score/graphics/ZoomItem.hpp>
#include <ossia/editor/automation/tinyspline_util.hpp>

#include <cmath>
#include <numeric>

// Disclaimer:
// Part of the code comes from splineeditor.cpp from
// the Qt project:
// Copyright (C) 2016 The Qt Company Ltd.
// https://github.com/qt/qtdeclarative/blob/dev/tools/qmleasing/splineeditor.cpp
#include <wobjectimpl.h>
W_OBJECT_IMPL(Spline::View)

namespace Spline
{
class CurveItem : public QGraphicsItem
{
public:
  CurveItem(View& parent)
    : QGraphicsItem{&parent}
    , m_view{parent}
  {
    setFlag(ItemClipsToShape, false);
    setScale(m_zoom);
    updateRect();
  }

  View& m_view;

  QRectF boundingRect() const override
  {
    return QRectF(m_topLeft, m_bottomRight);
  }

  QPointF point(double pos) const noexcept
  {
    auto pt = m_spl.evaluate(pos);
    return mapToCanvas(ossia::nodes::spline_point{pt[0], pt[1]});
  }

  void paint(QPainter* p, const QStyleOptionGraphicsItem*, QWidget*) override
  {
    // TODO optimize painting here
    if (m_spline.points.empty())
      return;

    auto& skin = Process::Style::instance();
    QPainter& painter = *p;

    // Draw the grid
    {
      auto squarePen = skin.IntervalMuted().main.pen3_dashed_flat_miter;
      squarePen.setWidthF(1. / m_zoom);
      squarePen.setStyle(Qt::SolidLine);

      painter.setBrush(skin.NoBrush());
      painter.setPen(squarePen);

      double biggestDim = std::max(boundingRect().width(), boundingRect().height());
      painter.drawLine(-biggestDim * m_zoom, 0, biggestDim * m_zoom, 0);
      painter.drawLine(0, -biggestDim * m_zoom, 0, biggestDim * m_zoom);
    }

    painter.setRenderHint(QPainter::Antialiasing, true);

    // Draw the curve
    {
      QPen segmt = skin.skin.Base4.main.pen2;
      segmt.setWidthF(segmt.widthF() / m_zoom);

      painter.strokePath(m_curveShape, segmt);

      if(m_play > 0)
      {
        segmt.setColor(skin.IntervalPlayFill().color());
        painter.strokePath(m_playShape, segmt);
      }
    }

    // Draw the points
    {
      const auto pts = m_spline.points.size();

      // Handle first point
      auto fp = mapToCanvas(m_spline.points[0]);
      const auto pointSize = 3. / m_zoom;

      painter.setPen(skin.TransparentPen());
      if (m_clicked && 0 != *m_clicked)
        painter.setBrush(QColor(170, 220, 20));
      else
        painter.setBrush(QColor(170, 220, 220));

      painter.drawEllipse(
            QRectF{fp.x() - pointSize, fp.y() - pointSize, pointSize * 2., pointSize * 2.});

      QPen purplePen = skin.skin.Emphasis3.darker.pen2_dotted_square_miter;
      purplePen.setWidthF(purplePen.widthF() / m_zoom);
      // Remaining points
      for (std::size_t i = 1U; i < pts; i++)
      {
        // Draw the purple lines
        painter.setPen(purplePen);
        QPointF p = mapToCanvas(m_spline.points[i]);
        painter.drawLine(fp, p);

        // Draw the points
        if (i != m_clicked)
          painter.setBrush(QColor(170, 220, 20));
        else
          painter.setBrush(QColor(170, 220, 220));

        painter.setPen(skin.TransparentPen());
        painter.drawEllipse(
              QRectF{p.x() - pointSize, p.y() - pointSize, pointSize * 2., pointSize * 2.});
        fp = p;
      }
    }

    painter.setRenderHint(QPainter::Antialiasing, false);
  }

  void updateRect()
  {
    prepareGeometryChange();
    m_topLeft = QPointF{INT_MAX, INT_MAX};
    m_bottomRight = QPointF{INT_MIN, INT_MIN};
    for(auto& pt : m_spline.points)
    {
      m_topLeft.rx() = std::min(pt.x(), m_topLeft.rx());
      m_topLeft.ry() = std::min(1. - pt.y(), m_topLeft.ry());
      m_bottomRight.rx() = std::max(pt.x(), m_bottomRight.rx());
      m_bottomRight.ry() = std::max(1. - pt.y(), m_bottomRight.ry());
    }

    const double w = m_bottomRight.x() - m_topLeft.x();
    const double h = m_bottomRight.y() - m_topLeft.y();
    m_topLeft -= QPointF{0.1 * w, 0.1 * h};
    m_bottomRight += QPointF{0.1 * w, 0.1 * h};
    update();
  }

  void setZoomToFitRect(QRectF parentRect)
  {
    // We want to find the zoom ratio that makes the largest dimension
    // of the child to fit in the parent.
    // e.g. if the child fits in a [10; 30] rect,
    // and the parent is a [400, 500] rect,
    // 400 / 10 = 40
    // 500 / 30 = 16.6
    // we take 16.6 as zoom ratio
    const double w = m_bottomRight.x() - m_topLeft.x();
    const double h = m_bottomRight.y() - m_topLeft.y();
    const double parent_w = parentRect.width();
    const double parent_h = parentRect.height();
    const double w_ratio = parent_w / w;
    const double h_ratio = parent_h / h;

    m_zoom = std::min(w_ratio, h_ratio) * 0.9;
    setScale(m_zoom);
  }

  ossia::nodes::spline_point evaluate(double pos) const noexcept
  {
    auto pt = m_spl.evaluate(pos);
    return ossia::nodes::spline_point{pt[0], pt[1]};
  }

  static const constexpr auto N = 500;
  void updateSpline()
  {
    m_spl.set_points(
          reinterpret_cast<const tsReal*>(m_spline.points.data()),
          m_spline.points.size());

    // Recompute the curve
    {
      m_points.clear();

      auto& path = m_curveShape;
      path.clear();

      auto pt = mapToCanvas(evaluate(0));
      m_points.push_back(pt);
      path.moveTo(pt);

      for (std::size_t i = 1U; i < N; i++)
      {
        pt = mapToCanvas(evaluate(double(i) / N));
        path.lineTo(pt);
        m_points.push_back(pt);
      }

      updatePlayPath();
    }

    m_view.changed();
  }

  void updatePlayPath()
  {
    if(m_play > 0)
    {
      auto& path = m_playShape;
      path.clear();

      std::size_t max = std::min(std::size_t(qBound(0.f, m_play, 1.f) * N), m_points.size());
      if(max == 0)
        return;

      path.moveTo(m_points[0]);
      for (std::size_t i = 1U; i < max; i++)
      {
        path.lineTo(m_points[i]);
      }
    }
    else
    {
      m_playShape.clear();
    }
  }

  void setSpline(ossia::nodes::spline_data d)
  {
    if (d != m_spline)
    {
      m_spline = std::move(d);
      updateSpline();
      updateRect();
    }
    update();
  }

  void setPlayPercentage(float f)
  {
    m_play = f;
    updatePlayPath();
    update();
  }

  const ossia::nodes::spline_data& spline() const { return m_spline; }

  template <typename T>
  QPointF mapToCanvas(const T& point) const
  {
    return QPointF(point.x(), 1. - point.y());
  }

  ossia::nodes::spline_point mapFromCanvas(const QPointF& point) const
  {
    return ossia::nodes::spline_point{(double)point.x(), 1. - point.y()};
  }

  std::optional<std::size_t> findControlPoint(QPointF point) const
  {
    int pointIndex = -1;
    qreal distance = -1;

    const auto N = m_spline.points.size();
    for (std::size_t i = 0; i < N; ++i)
    {
      qreal d = QLineF{point, mapToCanvas(m_spline.points.at(i))}.length();
      if ((distance < 0 && d < 10 / m_zoom) || d < distance)
      {
        distance = d;
        pointIndex = i;
      }
    }

    if (pointIndex != -1)
      return pointIndex;
    return {};
  }

  void mousePressEvent(QGraphicsSceneMouseEvent* e) override
  {
    auto btn = e->button();
    if (btn == Qt::LeftButton)
    {
      if ((m_clicked = findControlPoint(e->pos())))
      {
        moveControlPoint(e->pos());
        e->accept();
      }
      else if(m_curveShape.contains(e->pos()))
      {
        qDebug() << "click in curve";
      }
      else
      {
        e->ignore();
      }
    }
    else if (btn == Qt::RightButton)
    {
      // Delete
      updateSpline();
    }
  }

  void mouseMoveEvent(QGraphicsSceneMouseEvent* e) override
  {
    if(m_clicked)
    {
      moveControlPoint(e->pos());
      e->accept();
    }
    else
    {
      e->ignore();
    }
  }

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* e) override
  {
    if (m_clicked)
    {
      moveControlPoint(e->pos());
      updateRect();
      m_view.changed();
      m_clicked = std::nullopt;
      e->accept();
      m_view.released(e->pos());
    }
    else
    {
      e->ignore();
    }
  }

  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override
  {
    const auto newPos = mapFromCanvas(event->pos());
    std::size_t splitIndex = 0;
    const std::size_t N = m_spline.points.size();
    for (std::size_t i = 0; i < N - 1; ++i)
    {
      if (m_spline.points[i].x() <= newPos.x())
      {
        splitIndex = i;
      }
      else
      {
        break;
      }
    }

    m_spline.points.insert(m_spline.points.begin() + splitIndex + 1, newPos);

    updateSpline();
    update();
  }

  void moveControlPoint(QPointF mouse)
  {
    SCORE_ASSERT(m_clicked);

    auto p = mapFromCanvas(mouse);
    const auto mp = *m_clicked;
    const auto N = m_spline.points.size();
    if (mp < N)
    {
      m_spline.points[mp] = p;

      updateSpline();
      update();
    }
  }

  std::vector<QPointF> m_points;
  QPainterPath m_curveShape, m_playShape;
  std::optional<std::size_t> m_clicked;
  ossia::nodes::spline_data m_spline;
  ts::spline<2> m_spl;

  double m_zoom{10.};
  QPointF m_topLeft, m_bottomRight;
  float m_play{0.};

};

static_assert(std::is_same<tsReal, qreal>::value, "");
View::View(QGraphicsItem* parent) : LayerView{parent}
{
  m_impl = new CurveItem{*this};

  auto item = new score::ZoomItem{this};
  item->setPos(10, 10);

  // TODO proper zooming is done in log space
  connect(item, &score::ZoomItem::zoom, this, [this] {
    auto& zoom = m_impl->m_zoom;
    zoom = qBound(0.001, zoom * 1.5, 1000.);
    m_impl->setScale(zoom);
  });
  connect(item, &score::ZoomItem::dezoom,
          this, [this] {
    auto& zoom = m_impl->m_zoom;
    zoom = qBound(0.001, zoom / 1.5, 1000.);
    m_impl->setScale(zoom);
  });

  connect(item, &score::ZoomItem::recenter,
          this, &View::recenter);
  recenter();

  this->setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemClipsToShape);
}


void View::setSpline(ossia::nodes::spline_data d)
{
  m_impl->setSpline(std::move(d));
}

const ossia::nodes::spline_data& View::spline() const noexcept
{
  return m_impl->spline();
}

void View::setPlayPercentage(float p)
{
  m_impl->setPlayPercentage(p);
}

void View::recenter()
{
  m_impl->updateRect();
  m_impl->setZoomToFitRect(boundingRect());
  auto childCenter = m_impl->mapRectToParent(m_impl->boundingRect()).center() - m_impl->pos();
  auto ourCenter = boundingRect().center();
  auto delta = ourCenter - childCenter;

  m_impl->setPos(delta);
}

void View::paint_impl(QPainter* p) const
{
}

void View::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
  m_pressedPos = e->scenePos();
  e->accept();
}

void View::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
  const auto delta = e->scenePos() - m_pressedPos;
  m_impl->setPos(m_impl->pos() + delta);
  m_pressedPos = e->scenePos();
  e->accept();
}

void View::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
  mouseMoveEvent(e);
}

void View::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* e)
{

}

}