#include "PortItem.hpp"
#include <Dataflow/UI/CableItem.hpp>
#include <Process/Dataflow/Port.hpp>
#include <QDrag>
#include <QGraphicsSceneMoveEvent>
#include <QMimeData>
#include <QPainter>
#include <QCursor>
#include <QApplication>
#include <Process/Style/ScenarioStyle.hpp>
#include <Device/Node/NodeListMimeSerialization.hpp>
#include <State/MessageListSerialization.hpp>
#include <Dataflow/Commands/EditPort.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/document/DocumentInterface.hpp>
#include <score/document/DocumentContext.hpp>
namespace Dataflow
{
PortItem::port_map PortItem::g_ports;
PortItem::PortItem(Process::Port& p, QGraphicsItem* parent)
  : QGraphicsItem{parent}
  , m_port{p}
{
  this->setCursor(QCursor());
  this->setAcceptDrops(true);
  this->setAcceptHoverEvents(true);
  this->setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);
  this->setToolTip(p.customData());

  g_ports.insert({&p, this});

  Path<Process::Port> path = p;
  for(auto c : CableItem::g_cables)
  {
    if(c.first->source().unsafePath() == path.unsafePath())
    {
      c.second->setSource(this);
      cables.push_back(c.second);
    }
    else if(c.first->sink().unsafePath() == path.unsafePath())
    {
      c.second->setTarget(this);
      cables.push_back(c.second);
    }
  }
}

PortItem::~PortItem()
{
  for(auto cable : cables)
  {
    if(cable->source() == this)
      cable->setSource(nullptr);
    if(cable->target() == this)
      cable->setTarget(nullptr);
  }
  auto it = g_ports.find(&m_port);
  if(it != g_ports.end())
    g_ports.erase(it);
}

QRectF PortItem::boundingRect() const
{
  return {-m_diam/2., -m_diam/2., m_diam, m_diam};
}

void PortItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  painter->setRenderHint(QPainter::Antialiasing, true);

  auto& style = ScenarioStyle::instance();
  switch(m_port.type)
  {
    case Process::PortType::Audio:
      painter->setPen(style.AudioPortPen);
      painter->setBrush(style.AudioPortBrush);
      break;
    case Process::PortType::Message:
      painter->setPen(style.DataPortPen);
      painter->setBrush(style.DataPortBrush);
      break;
    case Process::PortType::Midi:
      painter->setPen(style.MidiPortPen);
      painter->setBrush(style.MidiPortBrush);
      break;
  }

  painter->drawEllipse(boundingRect());
  painter->setRenderHint(QPainter::Antialiasing, false);
}

static PortItem* clickedPort{};
void PortItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  if(this->contains(event->pos()))
  {
    switch(event->button())
    {
      case Qt::RightButton:
        emit contextMenuRequested(event->scenePos(), event->screenPos());
        break;
      default:
        break;
    }
  }
  event->accept();
}

void PortItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  event->accept();
  if(QLineF(pos(), event->pos()).length() > QApplication::startDragDistance())
  {
    QDrag* d{new QDrag{this}};
    QMimeData* m = new QMimeData;
    clickedPort = this;
    m->setText("cable");
    d->setMimeData(m);
    d->exec();
    connect(d, &QDrag::destroyed, this, [] {
      clickedPort = nullptr;
    });
  }
}

void PortItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  if(this->contains(event->pos()))
  {
    switch(event->button())
    {
      case Qt::LeftButton:
        emit showPanel();
        break;
      default:
        break;
    }
  }
  event->accept();
}

void PortItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  prepareGeometryChange();
  m_diam = 8.;
  update();
  event->accept();
}

void PortItem::hoverMoveEvent(QGraphicsSceneHoverEvent* event)
{
  event->accept();
}

void PortItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  prepareGeometryChange();
  m_diam = 6.;
  update();
  event->accept();
}

void PortItem::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
  prepareGeometryChange();
  m_diam = 8.;
  update();
  event->accept();
}

void PortItem::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
  event->accept();
}

void PortItem::dragLeaveEvent(QGraphicsSceneDragDropEvent* event)
{
  prepareGeometryChange();
  m_diam = 6.;
  update();
  event->accept();
}

void PortItem::dropEvent(QGraphicsSceneDragDropEvent* event)
{
  prepareGeometryChange();
  m_diam = 6.;
  update();
  auto& mime = *event->mimeData();

  if(clickedPort && this != clickedPort)
  {
    auto p1 = dynamic_cast<Process::Outlet*>(&m_port);
    auto p2 = dynamic_cast<Process::Outlet*>(&clickedPort->m_port);
    if((p1 && !p2) || (!p1 && p2))
    {
      emit createCable(clickedPort, this);
    }
  }

  clickedPort = nullptr;

  auto& ctx = score::IDocument::documentContext(m_port);
  CommandDispatcher<> disp{ctx.commandStack};
  if (mime.formats().contains(score::mime::addressettings()))
  {
    Mime<Device::FullAddressSettings>::Deserializer des{mime};
    Device::FullAddressSettings as = des.deserialize();

    if (as.address.path.isEmpty())
      return;

    disp.submitCommand(new ChangePortAddress{m_port, State::AddressAccessor{as.address}});
  }
  else if (mime.formats().contains(score::mime::messagelist()))
  {
    Mime<State::MessageList>::Deserializer des{mime};
    State::MessageList ml = des.deserialize();
    if (ml.empty())
      return;
    auto& newAddr = ml[0].address;

    if (newAddr == m_port.address())
      return;

    if (newAddr.address.path.isEmpty())
      return;

    disp.submitCommand(new ChangePortAddress{m_port, std::move(newAddr)});
  }
  event->accept();
}

QVariant PortItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant& value)
{
  switch(change)
  {
    case QGraphicsItem::ItemScenePositionHasChanged:
      for(auto cbl : cables)
      {
        cbl->resize();
      }
      break;
    case QGraphicsItem::ItemVisibleHasChanged:
    case QGraphicsItem::ItemSceneHasChanged:
      for(auto cbl : cables)
      {
        cbl->check();
      }
      break;
    default:
      break;
  }

  return QGraphicsItem::itemChange(change, value);
}

}
