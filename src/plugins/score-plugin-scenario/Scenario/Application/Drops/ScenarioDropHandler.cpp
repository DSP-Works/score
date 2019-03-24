// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ScenarioDropHandler.hpp"

#include <Scenario/Process/ScenarioPresenter.hpp>
#include <Scenario/Process/ScenarioView.hpp>
#include <QFileInfo>
namespace Scenario
{

MagneticStates
closestLeftState(
    MagneticStates cur, Scenario::Point pt, const Scenario::ScenarioPresenter& pres)
{
  constexpr int magnetic = 10;
  const auto& scenario = pres.model();

  // Check if we keep the current magnetism
  if(cur.horizontal)
  {
    const auto state_msec = scenario.events.at(cur.horizontal->eventId()).date().msec();

    const double rel_y_distance = std::abs(cur.horizontal->heightPercentage() - pt.y);
    const double abs_y_distance = rel_y_distance * pres.view().height();

    if(abs_y_distance < magnetic && state_msec < pt.date.msec())
    {
      return {cur.horizontal, cur.vertical, true};
    }
  }
  else if(cur.vertical)
  {
    auto cur_date = Scenario::parentEvent(*cur.vertical, scenario).date().msec();
    const double abs_x_distance = std::abs((cur_date - pt.date.msec()) / pres.zoomRatio());
    if(abs_x_distance < magnetic)
    {
      return {cur.horizontal, cur.vertical, true};
    }
  }

  EventModel& start_ev = scenario.startEvent();
  SCORE_ASSERT(!start_ev.states().empty());

  ossia::fast_hash_map<Id<EventModel>, double> eventDates;
  for(auto& ev : scenario.events)
    eventDates[ev.id()] = ev.date().msec();

  Scenario::StateModel* start_st = &scenario.states.at(start_ev.states().front());

  const double pt_msec = pt.date.msec();
  StateModel* min_x_state = start_st;
  double min_x_distance = std::numeric_limits<double>::max();

  StateModel* min_y_state = start_st;
  double min_y_distance = std::numeric_limits<double>::max();
  for(auto& state : scenario.states)
  {
    const auto state_msec = eventDates[state.eventId()];

    if(state_msec >= pt_msec)
      continue;

    const double rel_x_distance = std::abs(state_msec - pt.date.msec());
    const double abs_x_distance = rel_x_distance / pres.zoomRatio();

    const double rel_y_distance = std::abs(state.heightPercentage() - pt.y);
    const double abs_y_distance = rel_y_distance * pres.view().height();

    if(abs_x_distance < min_x_distance)
    {
      min_x_state = &state;
      min_x_distance = abs_x_distance;
    }

    if(abs_y_distance < min_y_distance)
    {
      min_y_state = &state;
      min_y_distance = abs_y_distance;
    }
  }

  if(min_x_distance < min_y_distance)
  {
    return {nullptr, min_x_state, min_x_distance < magnetic};
  }
  else
  {
    return {min_y_state, nullptr, min_y_distance < magnetic};
  }
}

DropHandler::~DropHandler() {}
GhostIntervalDropHandler::~GhostIntervalDropHandler() {}

bool GhostIntervalDropHandler::dragEnter(
    const Scenario::ScenarioPresenter& pres,
    QPointF pos,
    const QMimeData& mime)
{
  return dragMove(pres, pos, mime);
}

bool GhostIntervalDropHandler::dragMove(
    const Scenario::ScenarioPresenter& pres,
    QPointF pos,
    const QMimeData& mime)
{
  bool mimeTypes = ossia::any_of(m_acceptableMimeTypes, [&] (const auto& mimeType){
    return mime.formats().contains(mimeType);
  });

  bool suffixes = false;
  for(auto& url : mime.urls())
  {
    if(url.isLocalFile())
    {
      const auto ext = QFileInfo{url.toLocalFile()}.suffix();
      suffixes |= ossia::any_of(m_acceptableSuffixes, [&] (const auto& suffix) {
        return ext == suffix;
      });
      if(suffixes)
        break;
    }
  }
  if (!(mimeTypes || suffixes))
    return false;

  auto pt = pres.toScenarioPoint(pos);
  m_magnetic = closestLeftState(m_magnetic, pt, pres);
  auto [x_state, y_state, magnetic] = m_magnetic;

  if(y_state)
  {
    if(magnetic)
    {
      // TODO in the drop, handle the case where rel_t < 0 - else, negative date + crash
      pres.drawDragLine(*y_state, {Scenario::parentEvent(*y_state, pres.model()).date(), pt.y});
    }
    else
    {
      pres.drawDragLine(*y_state, pt);
    }
  }
  else if (x_state)
  {
    if (x_state->nextInterval() || x_state->eventId() == pres.model().startEvent().id())
    {
      pres.drawDragLine(*x_state, pt);
    }
    else
    {
      if(magnetic)
        pres.drawDragLine(*x_state, {pt.date, x_state->heightPercentage()});
      else
        pres.drawDragLine(*x_state, pt);
    }
  }
  return true;
}

bool GhostIntervalDropHandler::dragLeave(
    const Scenario::ScenarioPresenter& pres,
    QPointF pos,
    const QMimeData& mime)
{
  pres.stopDrawDragLine();
  return false;
}

DropHandlerList::~DropHandlerList() {}

bool DropHandlerList::dragEnter(
    const ScenarioPresenter& scen,
    QPointF drop,
    const QMimeData& mime) const
{
  for (auto& fact : *this)
  {
    if (fact.dragEnter(scen, drop, mime))
      return true;
  }

  return false;
}

bool DropHandlerList::dragMove(
    const ScenarioPresenter& scen,
    QPointF drop,
    const QMimeData& mime) const
{
  for (auto& fact : *this)
  {
    if (fact.dragMove(scen, drop, mime))
      return true;
  }

  return false;
}

bool DropHandlerList::dragLeave(
    const ScenarioPresenter& scen,
    QPointF drop,
    const QMimeData& mime) const
{
  for (auto& fact : *this)
  {
    if (fact.dragLeave(scen, drop, mime))
      return true;
  }

  return false;
}

bool DropHandlerList::drop(
    const ScenarioPresenter& scen,
    QPointF drop,
    const QMimeData& mime) const
{
  for (auto& fact : *this)
  {
    if (fact.drop(scen, drop, mime))
      return true;
  }

  return false;
}

IntervalDropHandler::~IntervalDropHandler() {}

IntervalDropHandlerList::~IntervalDropHandlerList() {}

bool IntervalDropHandlerList::drop(
    const score::DocumentContext& ctx,
    const Scenario::IntervalModel& cst,
    const QMimeData& mime) const
{
  for (auto& fact : *this)
  {
    if (fact.drop(ctx, cst, mime))
      return true;
  }

  return false;
}
}
