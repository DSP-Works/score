#pragma once
#include <score/model/Identifier.hpp>

namespace Scenario
{
class IntervalModel;
class EventModel;
class EventPresenter;
class StatePresenter;
class TemporalIntervalPresenter;
class TemporalScenarioPresenter;
class TimeSyncPresenter;
class CommentBlockPresenter;
class ScenarioViewInterface
{
public:
  ScenarioViewInterface(const TemporalScenarioPresenter& presenter);

  void on_eventMoved(const EventPresenter& event);
  void on_intervalMoved(const TemporalIntervalPresenter& interval);
  void on_timeSyncMoved(const TimeSyncPresenter& timesync);
  void on_stateMoved(const StatePresenter& state);
  void on_commentMoved(const CommentBlockPresenter& comment);

  void on_hoverOnInterval(const Id<IntervalModel>& intervalId, bool enter);
  void on_hoverOnEvent(const Id<EventModel>& eventId, bool enter);

  void on_graphicalScaleChanged(double scale);

private:
  const TemporalScenarioPresenter& m_presenter;
};
}
