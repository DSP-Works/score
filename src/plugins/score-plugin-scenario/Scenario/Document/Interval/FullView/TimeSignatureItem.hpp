#pragma once
#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/Interval/IntervalPresenter.hpp>
#include <QGraphicsSceneMouseEvent>
#include <QMenu>
#include <QTextLayout>
#include <QApplication>
#include <score/tools/Bind.hpp>
#include <Scenario/Commands/Signature/SignatureCommands.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <Process/Style/Pixmaps.hpp>
#include <Process/Style/ScenarioStyle.hpp>
#include <Magnetism/MagnetismAdjuster.hpp>
namespace Scenario
{

class TimeSignatureHandle
    : public QObject
    , public QGraphicsItem
{
  W_OBJECT(TimeSignatureHandle)
public:
  TimeSignatureHandle(const IntervalModel& itv, QGraphicsItem* parent)
    : QGraphicsItem{parent}
  {

  }

  ~TimeSignatureHandle()
  {
  }

  QRectF boundingRect() const final override
  {
    return {
      std::min(-m_rect.width() / 2., -6.5),
          -8.,
          std::max(13., m_rect.width()),
          std::max(20., m_rect.height())};
  }

  void paint(
      QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget) override
  {
    painter->drawPixmap(QPointF{-6.5, 2.}, Process::Pixmaps::instance().metricHandle);
    painter->drawPixmap(QPointF{-m_rect.width() / 2. - 1., -8.}, m_signature);
  }

  void setSignature(TimeVal time, Control::time_signature sig)
  {
    m_time = time;
    if(sig != m_sig)
    {
      m_sig = sig;
      updateImpl();
    }
    update();
  }

  const TimeVal& time() const noexcept { return m_time; }
  const Control::time_signature& signature() const noexcept { return m_sig; }

  void move(double originalPos, double delta) W_SIGNAL(move, originalPos, delta);
  void press() W_SIGNAL(press);
  void release() W_SIGNAL(release);
  void remove() W_SIGNAL(remove);

  bool pressed{};

private:
  void updateImpl()
  {
    prepareGeometryChange();

    auto& skin = score::Skin::instance();
    auto& m_font = skin.MonoFontSmall;

    {
      const auto str = QString{"%1/%2"}.arg(m_sig.upper).arg(m_sig.lower);
      QTextLayout layout(str, m_font);
      layout.beginLayout();
      auto line = layout.createLine();
      layout.endLayout();

      m_rect = line.naturalTextRect();
      auto r = line.glyphRuns();

      if (r.size() > 0)
      {
        double ratio = qApp->devicePixelRatio();
        auto m_line = QImage(
            m_rect.width() * ratio,
            m_rect.height() * ratio,
            QImage::Format_ARGB32_Premultiplied);
        m_line.setDevicePixelRatio(ratio);
        m_line.fill(Qt::transparent);

        {
          QPainter p{&m_line};
          p.setRenderHint(QPainter::Antialiasing, false);
          p.setRenderHint(QPainter::TextAntialiasing, false);

          p.setPen(skin.Light.main.pen0);
          p.setBrush(skin.NoBrush);
          p.drawGlyphRun(QPointF{0, 0}, r[0]);
        }
        m_signature = QPixmap::fromImage(m_line);
      }
    }

    update();
  }

  TimeVal m_time{};
  Control::time_signature m_sig{0,0};
  QPixmap m_signature;
  QRectF m_rect;
};

class FixedHandle final : public TimeSignatureHandle
{
public:
  using TimeSignatureHandle::TimeSignatureHandle;
};

class MovableHandle final : public TimeSignatureHandle
{
public:
  using TimeSignatureHandle::TimeSignatureHandle;
private:
  double m_origItemX{};
  double m_pressX{};

  void mousePressEvent(QGraphicsSceneMouseEvent* mv) override
  {
    mv->accept();
    if(mv->button() != Qt::LeftButton)
    {
      remove();
    }
    else
    {
      pressed = true;
      m_origItemX = this->x();
      m_pressX = mv->scenePos().x();
      press();
    }
  }

  void mouseMoveEvent(QGraphicsSceneMouseEvent* mv) override
  {
    double delta = mv->scenePos().x() - m_pressX;
    if(delta != 0)
    {
      move(m_origItemX, delta);
    }
    mv->accept();
  }

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* mv) override
  {
    mouseMoveEvent(mv);
    pressed = false;
    release();
  }
};

  class TimeSignatureItem
      : public QObject
      , public QGraphicsItem
  {
  public:
    TimeSignatureItem(const IntervalPresenter& itv, QGraphicsItem* parent)
      : QGraphicsItem{parent}
      , m_itv{itv}
      , m_magnetic{(Process::MagnetismAdjuster&)m_itv.context().app.interfaces<Process::MagnetismAdjuster>()}
    {
      setZValue(200);
      setFlag(ItemHasNoContents, true);
      setFlag(ItemClipsChildrenToShape, false);

      con(itv.model(), &IntervalModel::timeSignaturesChanged,
          this, &TimeSignatureItem::handlesChanged);
      handlesChanged();
    }

    void createHandle(TimeVal time, Control::time_signature sig)
    {
      TimeSignatureHandle* handle{};

      if(time == TimeVal::zero())
      {
        // The first time handle cannot move or change
        handle = new FixedHandle{m_itv.model(), this};
      }
      else
      {
        // Other handles are free
        handle = new MovableHandle{m_itv.model(), this};

        con(*handle, &TimeSignatureHandle::press,
            this, [=] {
          m_origHandles = this->m_itv.model().timeSignatureMap();
          m_origTime = handle->time();
          m_origSig = handle->signature();
        });
        con(*handle, &TimeSignatureHandle::move,
            this, [=] (double originalPos, double delta) {
          moveHandle(*handle, originalPos, delta);
        });
        con(*handle, &TimeSignatureHandle::release,
            this, [=] {
          m_origHandles.clear();
          m_itv.context().dispatcher.commit();
        });
        con(*handle, &TimeSignatureHandle::remove,
            this, [=] {
          removeHandle(*handle);
        }, Qt::QueuedConnection);
      }

      handle->setPos(time.toPixels(m_ratio), 0.);
      handle->setSignature(time, sig);

      m_handles.push_back(handle);
    }

    void setZoomRatio(ZoomRatio r)
    {
      if(m_ratio != r)
      {
        m_ratio = r;

        auto it = m_handles.begin();
        auto handle_it = m_itv.model().timeSignatureMap().begin();
        while(it != m_handles.end())
        {
          auto& [time, sig] = *handle_it;

          (*it)->setPos(time.toPixels(m_ratio), 0.);
          (*it)->setSignature(time, sig);

          ++it;
          ++handle_it;
        }
      }
    }

    void setWidth(double w)
    {
      prepareGeometryChange();
      m_width = w;
    }

  private:
    void handlesChanged()
    {
      const auto& signatures = m_itv.model().timeSignatureMap();
      if(m_handles.size() > signatures.size())
      {
        // Removed handles
        for(auto it = m_handles.begin(); it != m_handles.end(); )
        {
          // TODO what if we undo creation while pressing
          // we should prevent undo / redo while doing an action...
          if(signatures.find((*it)->time()) == signatures.end())
          {
            SCORE_ASSERT(!((*it)->pressed));
            delete *it;
            it = m_handles.erase(it);
          }
          else
          {
            ++it;
          }
        }
      }
      else if(m_handles.size() < signatures.size())
      {
        // Created handles
        for(auto h : m_handles)
          delete h;
        m_handles.clear();

        for(auto& [time, sig] : signatures)
        {
          createHandle(time, sig);
        }
      }
      else
      {
        for(auto h : m_handles)
          if(h->pressed)
            return;

        auto it = m_handles.begin();
        auto handle_it = signatures.begin();
        while(it != m_handles.end())
        {
          (*it)->setPos(handle_it->first.toPixels(m_ratio), 0.);
          (*it)->setSignature(handle_it->first, handle_it->second);

          ++it;
          ++handle_it;
        }
      }
    }

    void moveHandle(TimeSignatureHandle& handle, double originalPos, double delta)
    {
      const double x = originalPos + delta;
      // TODO what if we pass on top of another :|

      // Find leftmost signature
      const auto msecs = TimeVal::fromMsecs(x * m_ratio);
      const auto new_time = m_magnetic.getPosition(msecs);

      // Replace it in the signatures
      TimeSignatureMap signatures = m_origHandles;
      auto it = signatures.find(m_origTime);
      signatures.erase(it);
      signatures[new_time] = m_origSig;

      // Set new position for the handle
      handle.setX(new_time.msec() / m_ratio);
      handle.setSignature(new_time, handle.signature());

      m_itv.context().dispatcher.submit<Scenario::Command::SetTimeSignatures>(m_itv.model(), signatures);
    }

    void removeHandle(TimeSignatureHandle& handle)
    {
      TimeSignatureMap signatures = m_itv.model().timeSignatureMap();
      auto it = signatures.find(handle.time());
      signatures.erase(it);

      CommandDispatcher<> disp{m_itv.context().commandStack};
      disp.submit<Scenario::Command::SetTimeSignatures>(m_itv.model(), signatures);
    }

    QRectF boundingRect() const final override
    {
      return {0., 0., m_width, 17.};
    }

    void paint(
        QPainter* painter,
        const QStyleOptionGraphicsItem* option,
        QWidget* widget) override
    {
    }

    void requestNewHandle(QPointF pos)
    {
      auto signatures = m_itv.model().timeSignatureMap();
      signatures[TimeVal::fromMsecs(pos.x() * m_ratio)] = Control::time_signature{4, 4};
      CommandDispatcher<> disp{m_itv.context().commandStack};
      disp.submit<Scenario::Command::SetTimeSignatures>(m_itv.model(), signatures);
    }

    void contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override
    {
      QMenu menu;
      auto act = menu.addAction("Add signature change");
      connect(act, &QAction::triggered, this, [this, pos=event->pos()] {
        requestNewHandle(pos);
      });

      menu.exec(event->screenPos());
    }

    double m_width{100.};
    ZoomRatio m_ratio{1.};

    const IntervalPresenter& m_itv;
    Process::MagnetismAdjuster& m_magnetic;

    std::vector<TimeSignatureHandle*> m_handles;
    TimeSignatureMap m_origHandles{};

    TimeVal m_origTime{};
    Control::time_signature m_origSig{};

};
}


