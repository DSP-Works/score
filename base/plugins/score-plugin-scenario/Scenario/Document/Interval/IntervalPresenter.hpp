#pragma once
#include <Process/TimeValue.hpp>
#include <wobjectdefs.h>
#include <Process/ZoomHelper.hpp>
#include <QPoint>
#include <QString>
#include <QTimer>
#include <Scenario/Document/Interval/SlotPresenter.hpp>
#include <nano_signal_slot.hpp>
#include <score/model/Identifier.hpp>
#include <score_plugin_scenario_export.h>

namespace Process
{
struct ProcessPresenterContext;
class ProcessModel;
class LayerPresenter;
class LayerView;
}

namespace Scenario
{
class IntervalHeader;
class IntervalModel;
class IntervalView;

struct LayerData
{
  LayerData() = default;
  LayerData(const LayerData&) = default;
  LayerData(LayerData&&) = default;
  LayerData& operator=(const LayerData&) = default;
  LayerData& operator=(LayerData&&) = default;
  LayerData(
      const Process::ProcessModel* m,
      Process::LayerPresenter* p,
      Process::LayerView* v)
      : model(m), presenter(p), view(v)
  {
  }

  const Process::ProcessModel* model{};
  Process::LayerPresenter* presenter{};
  Process::LayerView* view{};
};

class SCORE_PLUGIN_SCENARIO_EXPORT IntervalPresenter
    : public QObject
    , public Nano::Observer
{
  W_OBJECT(IntervalPresenter)

public:
  IntervalPresenter(
      const IntervalModel& model,
      IntervalView* view,
      IntervalHeader* header,
      const Process::ProcessPresenterContext& ctx,
      QObject* parent);
  virtual ~IntervalPresenter();
  virtual void updateScaling();

  bool isSelected() const;

  const IntervalModel& model() const;

  IntervalView* view() const;
  IntervalHeader* header() const { return m_header; }

  virtual void on_zoomRatioChanged(ZoomRatio val);
  ZoomRatio zoomRatio() const
  {
    return m_zoomRatio;
  }

  const std::vector<SlotPresenter>& getSlots() const
  {
    return m_slots;
  }

  const Id<IntervalModel>& id() const;

  void on_minDurationChanged(const TimeVal&);
  void on_maxDurationChanged(const TimeVal&);

  double on_playPercentageChanged(double t);

  virtual void startSlotDrag(int slot, QPointF) const = 0;
  virtual void stopSlotDrag() const = 0;

  virtual void selectedSlot(int) const = 0;
  virtual void requestSlotMenu(int slot, QPoint pos, QPointF sp) const = 0;

public:
  void pressed(QPointF arg_1) const E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, pressed, arg_1);
  void moved(QPointF arg_1) const E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, moved, arg_1);
  void released(QPointF arg_1) const E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, released, arg_1);

  void askUpdate() E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, askUpdate);
  void heightChanged() E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, heightChanged);           // The vertical size
  void heightPercentageChanged() E_SIGNAL(SCORE_PLUGIN_SCENARIO_EXPORT, heightPercentageChanged); // The vertical position

protected:
  // Process presenters are in the slot presenters.
  const IntervalModel& m_model;

  ZoomRatio m_zoomRatio{};
  IntervalView* m_view{};
  IntervalHeader* m_header{};

  const Process::ProcessPresenterContext& m_context;

  void updateChildren();
  void updateBraces();
  std::vector<SlotPresenter> m_slots;
};

template <typename T>
const typename T::view_type* view(const T* obj)
{
  return static_cast<const typename T::view_type*>(obj->view());
}

template <typename T>
typename T::view_type* view(T* obj)
{
  return static_cast<typename T::view_type*>(obj->view());
}

template <typename T>
typename T::view_type& view(const T& obj)
{
  return static_cast<typename T::view_type&>(*obj.view());
}

template <typename T>
typename T::view_type& view(T& obj)
{
  return static_cast<typename T::view_type&>(*obj.view());
}
}
