#pragma once
#include <Midi/MidiNote.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Application/Drops/ScenarioDropHandler.hpp>
namespace Midi
{

class DropMidiInSenario final : public Scenario::DropHandler
{
  SCORE_CONCRETE("8F162598-9E4E-4865-A861-81DF01D2CDF0")

  bool drop(
      const Scenario::TemporalScenarioPresenter&, QPointF pos,
      const QMimeData& mime) override;
};

struct MidiTrack
{
  std::vector<Midi::NoteData> notes;
  int min{127}, max{0};

  struct MidiSong
  {
    std::vector<MidiTrack> tracks;
    double duration{};
    float tempo{};
    float tickPerBeat{};

    double durationInMs{};
  };
  static MidiSong
  parse(const QMimeData& dat, const score::DocumentContext& ctx);
};
}
