#pragma once
#include <ossia/editor/scenario/time_process.hpp>
#include <wobjectdefs.h>

#include <Engine/Executor/Component.hpp>
#include <Engine/Executor/DocumentPlugin.hpp>
#include <Process/Process.hpp>
#include <Process/ProcessComponent.hpp>
#include <QObject>
#include <memory>
#include <score/model/ComponentFactory.hpp>
#include <score/plugins/customfactory/ModelFactory.hpp>
#include <score_plugin_engine_export.h>

Q_DECLARE_METATYPE(std::shared_ptr<Engine::Execution::ProcessComponent>)
W_REGISTER_ARGTYPE(std::shared_ptr<Engine::Execution::ProcessComponent>)
namespace ossia
{
class scenario;
class time_process;
}

// TODO RENAMEME
namespace Engine
{
namespace Execution
{
struct Context;
template <typename T>
class InvalidProcessException : public std::runtime_error
{
public:
  InvalidProcessException(const QString& s)
      : std::runtime_error{
            (Metadata<PrettyName_k, T>::get() + ": " + s).toStdString()}
  {
  }
};

class SCORE_PLUGIN_ENGINE_EXPORT ProcessComponent
    : public Process::GenericProcessComponent<const Context>
    , public std::enable_shared_from_this<ProcessComponent>
{
  W_OBJECT(ProcessComponent)
  ABSTRACT_COMPONENT_METADATA(
      Engine::Execution::ProcessComponent,
      "d0f714de-c832-42d8-a605-60f5ffd0b7af")

public:
  static constexpr bool is_unique = true;

  ProcessComponent(
      Process::ProcessModel& proc,
      const Context& ctx,
      const Id<score::Component>& id,
      const QString& name,
      QObject* parent);

  virtual ~ProcessComponent();

  virtual void cleanup();
  virtual void stop()
  {
    process().stopExecution();
  }

  const std::shared_ptr<ossia::time_process>& OSSIAProcessPtr()
  {
    return m_ossia_process;
  }
  ossia::time_process& OSSIAProcess() const
  {
    return *m_ossia_process;
  }

  std::shared_ptr<ossia::graph_node> node;
public:
  void nodeChanged(ossia::node_ptr old_node, ossia::node_ptr new_node) W_SIGNAL(nodeChanged, old_node, new_node);

protected:
  std::shared_ptr<ossia::time_process> m_ossia_process;
};

template <typename Process_T, typename OSSIA_Process_T>
struct ProcessComponent_T
    : public Process::GenericProcessComponent_T<ProcessComponent, Process_T>
{
  using Process::GenericProcessComponent_T<ProcessComponent, Process_T>::
      GenericProcessComponent_T;

  OSSIA_Process_T& OSSIAProcess() const
  {
    return static_cast<OSSIA_Process_T&>(ProcessComponent::OSSIAProcess());
  }
};

class SCORE_PLUGIN_ENGINE_EXPORT ProcessComponentFactory
    : public score::GenericComponentFactory<
          Process::ProcessModel,
          Engine::Execution::DocumentPlugin,
          Engine::Execution::ProcessComponentFactory>
{
  SCORE_ABSTRACT_COMPONENT_FACTORY(Engine::Execution::ProcessComponent)
public:
  virtual ~ProcessComponentFactory() override;
  virtual std::shared_ptr<ProcessComponent> make(
      Process::ProcessModel& proc,
      const Context& ctx,
      const Id<score::Component>& id,
      QObject* parent) const = 0;

  //! Reimplement this if the element needs two-phase initialization.
  virtual void init(ProcessComponent* comp) const;
};

template <typename ProcessComponent_T>
class ProcessComponentFactory_T
    : public score::GenericComponentFactoryImpl<
          ProcessComponent_T,
          ProcessComponentFactory>
{
public:
  using model_type = typename ProcessComponent_T::model_type;
  std::shared_ptr<ProcessComponent> make(
      Process::ProcessModel& proc,
      const Context& ctx,
      const Id<score::Component>& id,
      QObject* parent) const final override
  {
    try
    {
      auto comp = std::make_shared<ProcessComponent_T>(
          static_cast<model_type&>(proc), ctx, id, parent);
      this->init(comp.get());
      return comp;
    }
    catch (...)
    {
      return {};
    }
  }
};

class SCORE_PLUGIN_ENGINE_EXPORT ProcessComponentFactoryList final
    : public score::GenericComponentFactoryList<
          Process::ProcessModel,
          Engine::Execution::DocumentPlugin,
          Engine::Execution::ProcessComponentFactory>
{
public:
  ~ProcessComponentFactoryList();
};
}
}
W_REGISTER_ARGTYPE(ossia::node_ptr)
