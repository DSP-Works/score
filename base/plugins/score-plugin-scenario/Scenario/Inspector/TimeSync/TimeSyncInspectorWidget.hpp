#pragma once

#include <Inspector/InspectorWidgetBase.hpp>
#include <Process/TimeValue.hpp>
#include <vector>

namespace Inspector
{
class InspectorSectionWidget;
}
class QLabel;
namespace Scenario
{
class MetadataWidget;
class TriggerInspectorWidget;
class EventModel;
class TimeSyncModel;
/*!
 * \brief The TimeSyncInspectorWidget class
 *      Inherits from InspectorWidgetInterface. Manages an inteface for an
 * TimeSync (Timebox) element.
 */
class TimeSyncInspectorWidget final : public Inspector::InspectorWidgetBase
{
public:
  explicit TimeSyncInspectorWidget(
      const TimeSyncModel& object,
      const score::DocumentContext& context,
      QWidget* parent);

private:
  void updateDisplayedValues();
  void on_dateChanged(const TimeVal&);

  const TimeSyncModel& m_model;

  MetadataWidget* m_metadata{};
  QLabel* m_date{};
  TriggerInspectorWidget* m_trigwidg{};
};
}
