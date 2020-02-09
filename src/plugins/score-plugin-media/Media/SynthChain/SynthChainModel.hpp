#pragma once
#include <Media/SynthChain/SynthChainMetadata.hpp>
#include <Process/Dataflow/Port.hpp>
#include <Media/ChainProcess.hpp>

#include <score_plugin_media_export.h>
namespace Media::SynthChain
{
class ProcessModel;

class SCORE_PLUGIN_MEDIA_EXPORT ProcessModel final
    : public ChainProcess
{
  SCORE_SERIALIZE_FRIENDS
  PROCESS_METADATA_IMPL(Media::SynthChain::ProcessModel)

  W_OBJECT(ProcessModel)

public:
  explicit ProcessModel(
      const TimeVal& duration,
      const Id<Process::ProcessModel>& id,
      QObject* parent);

  ~ProcessModel() override;

  template <typename Impl>
  explicit ProcessModel(Impl& vis, QObject* parent)
      : ChainProcess{vis, parent}
  {
    vis.writeTo(*this);
    init();
  }

  void init()
  {
    m_inlets.push_back(inlet.get());
    m_outlets.push_back(outlet.get());
  }

  std::unique_ptr<Process::MidiInlet> inlet{};
  std::unique_ptr<Process::AudioOutlet> outlet{};

};
}
