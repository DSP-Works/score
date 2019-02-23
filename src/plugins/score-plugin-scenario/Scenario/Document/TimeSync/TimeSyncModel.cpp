// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "TimeSyncModel.hpp"

#include <Process/Style/ScenarioStyle.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/VerticalExtent.hpp>
#include <Scenario/Process/ScenarioInterface.hpp>

#include <score/document/DocumentInterface.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <score/model/Identifier.hpp>
#include <score/model/ModelMetadata.hpp>

#include <QtGlobal>

#include <wobjectimpl.h>
W_OBJECT_IMPL(Scenario::TimeSyncModel)
namespace Scenario
{
TimeSyncModel::TimeSyncModel(
    const Id<TimeSyncModel>& id,
    const VerticalExtent& extent,
    const TimeVal& date,
    QObject* parent)
    : Entity{id, Metadata<ObjectKey_k, TimeSyncModel>::get(), parent}
    , m_date{date}
    , m_extent{extent}
{
  m_expression = State::defaultFalseExpression();
  metadata().setInstanceName(*this);
  metadata().setColor(Process::Style::instance().TimenodeDefault);
}

void TimeSyncModel::addEvent(const Id<EventModel>& eventId)
{
  m_events.push_back(eventId);

  auto scenar = dynamic_cast<ScenarioInterface*>(parent());
  if (scenar)
  {
    // There may be no scenario when we are cloning without a parent.
    // TODO this addEvent should be in an outside algorithm.
    auto& theEvent = scenar->event(eventId);
    theEvent.changeTimeSync(this->id());
  }

  newEvent(eventId);
}

bool TimeSyncModel::removeEvent(const Id<EventModel>& eventId)
{
  auto it = ossia::find(m_events, eventId);
  if (it != m_events.end())
  {
    m_events.erase(it);
    eventRemoved(eventId);
    return true;
  }

  return false;
}

void TimeSyncModel::clearEvents()
{
  auto ev = m_events;
  m_events.clear();
  for (const auto& e : ev)
    eventRemoved(e);
}

const TimeVal& TimeSyncModel::date() const
{
  return m_date;
}

void TimeSyncModel::setDate(const TimeVal& date)
{
  m_date = date;
  dateChanged(m_date);
}

const TimeSyncModel::EventIdVec& TimeSyncModel::events() const
{
  return m_events;
}

void TimeSyncModel::setEvents(const TimeSyncModel::EventIdVec& events)
{
  m_events = events;
}

const VerticalExtent& TimeSyncModel::extent() const
{
  return m_extent;
}

void TimeSyncModel::setExtent(const VerticalExtent& extent)
{
  if (extent != m_extent)
  {
    m_extent = extent;
    extentChanged(m_extent);
  }
}

void TimeSyncModel::setExpression(const State::Expression& expression)
{
  if (m_expression == expression)
    return;
  m_expression = expression;
  triggerChanged(m_expression);
}

bool TimeSyncModel::active() const
{
  return m_active;
}

void TimeSyncModel::setActive(bool active)
{
  if (active == m_active)
    return;
  m_active = active;
  activeChanged();
}
}
