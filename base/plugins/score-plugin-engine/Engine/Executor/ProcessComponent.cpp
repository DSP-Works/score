// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "ProcessComponent.hpp"

#include <Engine/Executor/ExecutorContext.hpp>
namespace Engine
{
namespace Execution
{
ProcessComponent::~ProcessComponent() = default;
ProcessComponentFactory::~ProcessComponentFactory() = default;
ProcessComponentFactoryList::~ProcessComponentFactoryList() = default;

void ProcessComponentFactory::init(ProcessComponent* comp) const
{
}

ProcessComponent::ProcessComponent(
    Process::ProcessModel& proc,
    const Context& ctx,
    const Id<score::Component>& id,
    const QString& name,
    QObject* parent)
    : Process::GenericProcessComponent<const Context>{proc, ctx, id, name,
                                                      parent}
{
}

void ProcessComponent::cleanup()
{
  if (auto proc = OSSIAProcessPtr())
  {
    this->system().plugin.unregister_node(process(), proc->node);
    in_exec([proc] { proc->node.reset(); });
  }
  node.reset();
  m_ossia_process.reset();
}
}
}
