#pragma once
#include <Engine/Executor/ClockManager/ClockManagerFactory.hpp>
#include <Engine/Executor/ClockManager/DefaultClockManager.hpp>
#include <Engine/Executor/DocumentPlugin.hpp>
#include <score_plugin_engine_export.h>
namespace Process
{
class Cable;
}
namespace Dataflow
{
class DocumentPlugin;
class Clock final
    : public Engine::Execution::ClockManager
    , public Nano::Observer
{
public:
  Clock(const Engine::Execution::Context& ctx);

  ~Clock() override;

private:
  // Clock interface
  void play_impl(
      const TimeVal& t, Engine::Execution::BaseScenarioElement&) override;
  void pause_impl(Engine::Execution::BaseScenarioElement&) override;
  void resume_impl(Engine::Execution::BaseScenarioElement&) override;
  void stop_impl(Engine::Execution::BaseScenarioElement&) override;
  bool paused() const override;

  Engine::Execution::DefaultClockManager m_default;
  Engine::Execution::DocumentPlugin& m_plug;
  Engine::Execution::BaseScenarioElement* m_cur{};
  bool m_paused{};
};

class ClockFactory final : public Engine::Execution::ClockManagerFactory
{
  SCORE_CONCRETE("e9ae6dec-a10f-414f-9060-b21d15b5d58d")

  QString prettyName() const override;
  std::unique_ptr<Engine::Execution::ClockManager>
  make(const Engine::Execution::Context& ctx) override;

  Engine::Execution::time_function
  makeTimeFunction(const score::DocumentContext& ctx) const override;
  Engine::Execution::reverse_time_function
  makeReverseTimeFunction(const score::DocumentContext& ctx) const override;
};
}
