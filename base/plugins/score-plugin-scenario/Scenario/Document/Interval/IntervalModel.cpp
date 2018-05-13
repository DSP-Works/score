// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "IntervalModel.hpp"

#include <Process/Process.hpp>
#include <Process/ProcessList.hpp>
#include <Process/Style/ScenarioStyle.hpp>
#include <Process/TimeValue.hpp>
#include <Scenario/Document/Interval/IntervalDurations.hpp>
#include <Scenario/Document/Interval/Slot.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentPresenter.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentPresenter.hpp>
#include <map>
#include <score/application/ApplicationContext.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/document/DocumentInterface.hpp>
#include <score/model/ModelMetadata.hpp>
#include <score/tools/IdentifierGeneration.hpp>
#include <score/tools/Todo.hpp>
#include <utility>
#include <wobjectimpl.h>
W_OBJECT_IMPL(Scenario::IntervalModel)

namespace Scenario
{

class StateModel;
class TimeSyncModel;
IntervalModel::IntervalModel(
    const Id<IntervalModel>& id, double yPos, QObject* parent)
    : Entity{id, Metadata<ObjectKey_k, IntervalModel>::get(), parent}
    , inlet{Process::make_inlet(Id<Process::Port>(0), this)}
    , outlet{Process::make_outlet(Id<Process::Port>(0), this)}
{
  initConnections();
  metadata().setInstanceName(*this);
  metadata().setColor(ScenarioStyle::instance().IntervalDefaultBackground);
  setHeightPercentage(yPos);

  inlet->type = Process::PortType::Audio;
  outlet->type = Process::PortType::Audio;
  outlet->setPropagate(true);
}

IntervalModel::~IntervalModel()
{
  static_assert(
      std::is_same<
          serialization_tag<IntervalModel>::type, visitor_entity_tag>::value,
      "");
  processes.clear();
}
void IntervalModel::initConnections()
{
  processes.added.connect<IntervalModel, &IntervalModel::on_addProcess>(this);
  processes.removing
      .connect<IntervalModel, &IntervalModel::on_removingProcess>(this);
}

IntervalModel::IntervalModel(DataStream::Deserializer& vis, QObject* parent)
    : Entity{vis, parent}
{
  initConnections();
  vis.writeTo(*this);
}

IntervalModel::IntervalModel(JSONObject::Deserializer& vis, QObject* parent)
    : Entity{vis, parent}
{
  initConnections();
  vis.writeTo(*this);
}

IntervalModel::IntervalModel(DataStream::Deserializer&& vis, QObject* parent)
    : Entity{vis, parent}
{
  initConnections();
  vis.writeTo(*this);
}

IntervalModel::IntervalModel(JSONObject::Deserializer&& vis, QObject* parent)
    : Entity{vis, parent}
{
  initConnections();
  vis.writeTo(*this);
}

const Id<StateModel>& IntervalModel::startState() const
{
  return m_startState;
}

void IntervalModel::setStartState(const Id<StateModel>& e)
{
  m_startState = e;
}

const Id<StateModel>& IntervalModel::endState() const
{
  return m_endState;
}

void IntervalModel::setEndState(const Id<StateModel>& endState)
{
  m_endState = endState;
}

const TimeVal& IntervalModel::date() const
{
  return m_date;
}

void IntervalModel::setStartDate(const TimeVal& start)
{
  m_date = start;
  dateChanged(start);
}

void IntervalModel::translate(const TimeVal& deltaTime)
{
  setStartDate(m_date + deltaTime);
}

// Simple getters and setters

double IntervalModel::heightPercentage() const
{
  return m_heightPercentage;
}

// Should go in an "execution" object.
void IntervalModel::startExecution()
{
  for (Process::ProcessModel& proc : processes)
  {
    proc.startExecution(); // prevents editing
  }
}
void IntervalModel::stopExecution()
{
  // duration.setPlayPercentage(0);
  duration.setExecutionSpeed(1.0);
  for (Process::ProcessModel& proc : processes)
  {
    proc.stopExecution();
  }
}

void IntervalModel::reset()
{
  duration.setPlayPercentage(0);
  duration.setExecutionSpeed(1.0);

  for (Process::ProcessModel& proc : processes)
  {
    proc.reset();
    proc.stopExecution();
  }

  setExecutionState(IntervalExecutionState::Enabled);
}

void IntervalModel::setHeightPercentage(double arg)
{
  if (m_heightPercentage != arg)
  {
    m_heightPercentage = arg;
    heightPercentageChanged(arg);
  }
}

void IntervalModel::setExecutionState(IntervalExecutionState s)
{
  if (s != m_executionState)
  {
    m_executionState = s;
    executionStateChanged(s);
  }
}

ZoomRatio IntervalModel::zoom() const
{
  return m_zoom;
}

void IntervalModel::setZoom(const ZoomRatio& zoom)
{
  m_zoom = zoom;
}

TimeVal IntervalModel::midTime() const
{
  return m_center;
}

void IntervalModel::setMidTime(const TimeVal& value)
{
  m_center = value;
}

void IntervalModel::setSmallViewVisible(bool v)
{
  m_smallViewShown = v;
  smallViewVisibleChanged(v);
}

bool IntervalModel::smallViewVisible() const
{
  return m_smallViewShown;
}

void IntervalModel::clearSmallView()
{
  m_smallView.clear();
  rackChanged(Slot::SmallView);
}

void IntervalModel::clearFullView()
{
  m_fullView.clear();
  rackChanged(Slot::FullView);
}

void IntervalModel::replaceSmallView(const Rack& other)
{
  m_smallView = other;
  rackChanged(Slot::SmallView);
}

void IntervalModel::replaceFullView(const FullRack& other)
{
  m_fullView = other;
  rackChanged(Slot::FullView);
}

void IntervalModel::addLayer(int slot, Id<Process::ProcessModel> id)
{
  auto& procs = m_smallView.at(slot).processes;
  SCORE_ASSERT(ossia::find(procs, id) == procs.end());
  procs.push_back(id);

  layerAdded({slot, Slot::SmallView}, id);

  putLayerToFront(slot, id);
}

void IntervalModel::removeLayer(int slot, Id<Process::ProcessModel> id)
{
  auto& procs = m_smallView.at(slot).processes;
  const auto N = procs.size();
  boost::range::remove_erase(procs, id);

  if (procs.size() < N)
  {
    layerRemoved({slot, Slot::SmallView}, id);

    if (!procs.empty())
      putLayerToFront(slot, procs.front());
    else
      putLayerToFront(slot, ossia::none);
  }
}

void IntervalModel::putLayerToFront(int slot, Id<Process::ProcessModel> id)
{
  m_smallView.at(slot).frontProcess = id;
  frontLayerChanged(slot, id);
}

void IntervalModel::putLayerToFront(int slot, ossia::none_t)
{
  m_smallView.at(slot).frontProcess = ossia::none;
  frontLayerChanged(slot, ossia::none);
}

void IntervalModel::addSlot(Slot s, int pos)
{
  SCORE_ASSERT((int)m_smallView.size() >= pos);
  m_smallView.insert(m_smallView.begin() + pos, std::move(s));
  slotAdded({pos, Slot::SmallView});

  if (m_smallView.size() == 1)
    setSmallViewVisible(true);
}

void IntervalModel::addSlot(Slot s)
{
  addSlot(std::move(s), m_smallView.size());
}

void IntervalModel::removeSlot(int pos)
{
  SCORE_ASSERT((int)m_smallView.size() >= pos);
  m_smallView.erase(m_smallView.begin() + pos);
  slotRemoved({pos, Slot::SmallView});

  if (m_smallView.empty())
    setSmallViewVisible(false);
}

const Slot* IntervalModel::findSmallViewSlot(int slot) const
{
  if (slot < (int)m_smallView.size())
    return &m_smallView[slot];

  return nullptr;
}

const Slot& IntervalModel::getSmallViewSlot(int slot) const
{
  return m_smallView.at(slot);
}

Slot& IntervalModel::getSmallViewSlot(int slot)
{
  return m_smallView.at(slot);
}

const FullSlot* IntervalModel::findFullViewSlot(int slot) const
{
  if (slot < (int)m_fullView.size())
    return &m_fullView[slot];

  return nullptr;
}

const FullSlot& IntervalModel::getFullViewSlot(int slot) const
{
  return m_fullView.at(slot);
}

FullSlot& IntervalModel::getFullViewSlot(int slot)
{
  return m_fullView.at(slot);
}

void IntervalModel::setMuted(bool m)
{
  if (m != m_muted)
  {
    m_muted = m;
    mutedChanged(m);
  }
}

double IntervalModel::getSlotHeight(const SlotId& slot) const
{
  if (slot.fullView())
    return processes.at(m_fullView.at(slot.index).process).getSlotHeight();
  else
    return m_smallView.at(slot.index).height;
}

void IntervalModel::setSlotHeight(const SlotId& slot, double height)
{
  height = std::max(height, 20.);
  if (slot.fullView())
    processes.at(m_fullView.at(slot.index).process).setSlotHeight(height);
  else
    getSmallViewSlot(slot.index).height = height;
  slotResized(slot);
}

void swap(Scenario::Slot& lhs, Scenario::Slot& rhs)
{
  Scenario::Slot tmp = std::move(lhs);
  lhs = std::move(rhs);
  rhs = std::move(tmp);
}

void IntervalModel::swapSlots(int pos1, int pos2, Slot::RackView v)
{
  if (v == Slot::FullView)
  {
    auto& vec = m_fullView;
    SCORE_ASSERT((int)vec.size() > pos1);
    SCORE_ASSERT((int)vec.size() > pos2);
    std::iter_swap(vec.begin() + pos1, vec.begin() + pos2);
  }
  else
  {
    auto& vec = m_smallView;
    SCORE_ASSERT((int)vec.size() > pos1);
    SCORE_ASSERT((int)vec.size() > pos2);
    std::iter_swap(vec.begin() + pos1, vec.begin() + pos2);
  }
  slotsSwapped(pos1, pos2, v);
}

void IntervalModel::on_addProcess(const Process::ProcessModel& p)
{
  m_fullView.push_back(FullSlot{p.id()});
  slotAdded({m_fullView.size() - 1, Slot::FullView});
}

void IntervalModel::on_removingProcess(const Process::ProcessModel& p)
{
  const auto& pid = p.id();
  for (int i = 0; i < (int)m_smallView.size(); i++)
  {
    removeLayer(i, pid);
  }

  auto it = ossia::find_if(
      m_fullView, [&](const FullSlot& slot) { return slot.process == pid; });
  if (it != m_fullView.end())
  {
    int N = std::distance(m_fullView.begin(), it);
    m_fullView.erase(it);
    slotRemoved(SlotId{N, Slot::FullView});
  }
}

bool isInFullView(const IntervalModel& cstr)
{
  // TODO just check if parent() == basescenario
  auto& doc = score::IDocument::documentContext(cstr);
  if (auto pres = doc.document.presenter())
  {
    auto sub = qobject_cast<Scenario::ScenarioDocumentPresenter*>(
        pres->presenterDelegate());
    if (sub)
      return &sub->displayedElements.interval() == &cstr;
    return false;
  }
  return false;
}

bool isInFullView(const Process::ProcessModel& cstr)
{
  return isInFullView(*static_cast<IntervalModel*>(cstr.parent()));
}

const Scenario::Slot& SlotPath::find(const score::DocumentContext& ctx) const
{
  return interval.find(ctx).getSmallViewSlot(index);
}

const Scenario::Slot*
SlotPath::try_find(const score::DocumentContext& ctx) const
{
  if (auto cst = interval.try_find(ctx))
    return cst->findSmallViewSlot(index);
  else
    return nullptr;
}
}
