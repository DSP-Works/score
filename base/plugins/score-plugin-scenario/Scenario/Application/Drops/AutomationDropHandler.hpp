#pragma once
#include <Scenario/Application/Drops/ScenarioDropHandler.hpp>

namespace Scenario
{

class DropProcessInScenario final : public DropHandler
{
  SCORE_CONCRETE("9a094988-b05f-4e10-8e0d-56e8d46e084d")

  bool drop(
      const Scenario::TemporalScenarioPresenter&,
      QPointF pos,
      const QMimeData& mime) override;
};

class DropPortInScenario final : public DropHandler
{
  SCORE_CONCRETE("b71dd84e-e242-4451-bab5-970215c6b120")

  bool drop(
      const Scenario::TemporalScenarioPresenter&,
      QPointF pos,
      const QMimeData& mime) override;
};

class DropLayerInScenario final : public DropHandler
{
  SCORE_CONCRETE("0eb96d95-3f5f-4e7a-b806-d03d0ac88b48")

  bool drop(
      const Scenario::TemporalScenarioPresenter&,
      QPointF pos,
      const QMimeData& mime) override;
};

class DropLayerInInterval final : public IntervalDropHandler
{
  SCORE_CONCRETE("9df2eac6-6680-43cc-9634-60324416ba04")

  bool drop(
      const Scenario::IntervalModel&,
      const QMimeData& mime) override;
};
/**
 * @brief The ProcessDropHandler class
 * Will create a blank process.
 */
class DropProcessInInterval final : public IntervalDropHandler
{
  SCORE_CONCRETE("08f5aec5-3a42-45c8-b3db-aa45a851dd09")

  bool drop(const Scenario::IntervalModel&, const QMimeData& mime) override;
};

/**
 * @brief The AutomationDropHandler class
 * Will create an automation where the addresses are dropped.
 */
class AutomationDropHandler final : public IntervalDropHandler
{
  SCORE_CONCRETE("851c98e1-4bcb-407b-9a72-8288d83c9f38")

  bool drop(const Scenario::IntervalModel&, const QMimeData& mime) override;
};
}
