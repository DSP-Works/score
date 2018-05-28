#pragma once
#include <Inspector/InspectorSectionWidget.hpp>
#include <Inspector/InspectorWidgetBase.hpp>
#include <list>
#include <score/plugins/customfactory/UuidKey.hpp>
#include <score/selection/SelectionDispatcher.hpp>

namespace Scenario
{
class StateModel;
class StateInspectorWidget final : public Inspector::InspectorWidgetBase
{
public:
  explicit StateInspectorWidget(
      const StateModel& object,
      const score::DocumentContext& context,
      QWidget* parent);

private:
  void splitFromEvent();
  void splitFromNode();
  void updateDisplayedValues();

  const StateModel& m_model;
  const score::DocumentContext& m_context;
  CommandDispatcher<> m_commandDispatcher;

  std::vector<QWidget*> m_properties;
};
}
