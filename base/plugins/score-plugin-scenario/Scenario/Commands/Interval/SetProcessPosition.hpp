#pragma once
#include <Scenario/Commands/ScenarioCommandFactory.hpp>
#include <score/command/Command.hpp>
#include <score/model/path/Path.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace Process
{
class ProcessModel;
}
namespace Scenario
{
class IntervalModel;
namespace Command
{
class PutProcessBefore final : public score::Command
{
  SCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(), PutProcessBefore, "Set process position")

public:
  // Put proc2 before proc
  PutProcessBefore(
      const IntervalModel& cst,
      Id<Process::ProcessModel> proc,
      Id<Process::ProcessModel> proc2);

  void undo(const score::DocumentContext& ctx) const override;
  void redo(const score::DocumentContext& ctx) const override;

protected:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

private:
  Path<Scenario::IntervalModel> m_path;
  Id<Process::ProcessModel> m_proc, m_proc2;
};

class PutProcessToEnd final : public score::Command
{
  SCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(), PutProcessToEnd, "Set process position")

public:
  // Put proc2 before proc
  PutProcessToEnd(const IntervalModel& cst, Id<Process::ProcessModel> proc);

  void undo(const score::DocumentContext& ctx) const override;
  void redo(const score::DocumentContext& ctx) const override;

protected:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

private:
  Path<Scenario::IntervalModel> m_path;
  Id<Process::ProcessModel> m_proc, m_proc_after;
};

class SwapProcessPosition final : public score::Command
{
  SCORE_COMMAND_DECL(
      ScenarioCommandFactoryName(),
      SwapProcessPosition,
      "Set process position")

public:
  SwapProcessPosition(
      const IntervalModel& cst,
      Id<Process::ProcessModel> proc,
      Id<Process::ProcessModel> proc2);

  void undo(const score::DocumentContext& ctx) const override;
  void redo(const score::DocumentContext& ctx) const override;

protected:
  void serializeImpl(DataStreamInput& s) const override;
  void deserializeImpl(DataStreamOutput& s) override;

private:
  Path<Scenario::IntervalModel> m_path;
  Id<Process::ProcessModel> m_proc, m_proc2;
};
}
}
