#pragma once
#include <ossia/dataflow/node_process.hpp>
#include <ossia/editor/scenario/time_process.hpp>
#include <ossia/editor/scenario/time_value.hpp>

#include <Engine/Executor/ExecutorContext.hpp>
#include <Engine/Executor/ProcessComponent.hpp>
#include <JS/Qml/QmlObjects.hpp>
#include <QEventLoop>
#include <QJSValue>
#include <QQmlEngine>
#include <QString>
#include <memory>
#include <score/document/DocumentContext.hpp>
#include <score/document/DocumentInterface.hpp>

namespace JS
{
class ProcessModel;
namespace Executor
{
class Component final
    : public ::Engine::Execution::
          ProcessComponent_T<JS::ProcessModel, ossia::node_process>
{
  COMPONENT_METADATA("c2737929-231e-4d57-9088-a2a3a8d3c24e")
public:
  Component(
      JS::ProcessModel& element,
      const Engine::Execution::Context& ctx,
      const Id<score::Component>& id,
      QObject* parent);
  ~Component() override;
};

using ComponentFactory
    = ::Engine::Execution::ProcessComponentFactory_T<Component>;
}
}
