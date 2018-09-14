// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Process/Dataflow/Port.hpp>

#include <ossia/editor/state/destination_qualifiers.hpp>

#include <QColor>

#include <Color/GradientModel.hpp>
#include <Color/GradientPresenter.hpp>
#include <wobjectimpl.h>
W_OBJECT_IMPL(Gradient::ProcessModel)
namespace Gradient
{
ProcessModel::ProcessModel(
    const TimeVal& duration, const Id<Process::ProcessModel>& id,
    QObject* parent)
    : Process::ProcessModel{duration, id,
                            Metadata<ObjectKey_k, ProcessModel>::get(), parent}
    , outlet{Process::make_outlet(Id<Process::Port>(0), this)}

{
  outlet->type = Process::PortType::Message;
  m_colors.insert(std::make_pair(0.2, QColor(Qt::black)));
  m_colors.insert(std::make_pair(0.8, QColor(Qt::white)));

  metadata().setInstanceName(*this);
  init();
}

ProcessModel::~ProcessModel()
{
}

void ProcessModel::init()
{
  outlet->setCustomData("Out");
  m_outlets.push_back(outlet.get());
}

QString ProcessModel::prettyName() const
{
  auto res = address().toString();
  if (!res.isEmpty())
    return res;
  return "Gradient";
}

const ProcessModel::gradient_colors& ProcessModel::gradient() const
{
  return m_colors;
}

void ProcessModel::setGradient(const ProcessModel::gradient_colors& c)
{
  if (m_colors != c)
  {
    m_colors = c;
    gradientChanged();
  }
}

const ::State::AddressAccessor& ProcessModel::address() const
{
  return outlet->address();
}

bool ProcessModel::tween() const
{
  return m_tween;
}

void ProcessModel::setTween(bool tween)
{
  if (m_tween == tween)
    return;

  m_tween = tween;
  tweenChanged(tween);
}

void ProcessModel::setDurationAndScale(const TimeVal& newDuration)
{
  // We only need to change the duration.
  setDuration(newDuration);
}

void ProcessModel::setDurationAndGrow(const TimeVal& newDuration)
{
  setDuration(newDuration);
}

void ProcessModel::setDurationAndShrink(const TimeVal& newDuration)
{
  setDuration(newDuration);
}

bool ProcessModel::contentHasDuration() const
{
  return true;
}

TimeVal ProcessModel::contentDuration() const
{
  auto lastPoint = 1.;
  if (!m_colors.empty())
  {
    auto back = m_colors.rbegin()->first;
    lastPoint = std::max(1., back);
  }

  return duration() * lastPoint;
}
}

template <>
void DataStreamReader::read(const Gradient::ProcessModel& autom)
{
  m_stream << *autom.outlet;
  m_stream << autom.m_colors << autom.m_tween;
  insertDelimiter();
}

template <>
void DataStreamWriter::write(Gradient::ProcessModel& autom)
{
  autom.outlet = Process::make_outlet(*this, &autom);
  m_stream >> autom.m_colors >> autom.m_tween;

  checkDelimiter();
}

template <>
void JSONObjectReader::read(const Gradient::ProcessModel& autom)
{
  obj["Outlet"] = toJsonObject(*autom.outlet);
  JSONValueReader v{};
  v.readFrom(autom.m_colors);
  obj["Gradient"] = v.val;
  obj["Tween"] = autom.tween();
}

template <>
void JSONObjectWriter::write(Gradient::ProcessModel& autom)
{
  JSONObjectWriter writer{obj["Outlet"].toObject()};
  autom.outlet = Process::make_outlet(writer, &autom);
  if (!autom.outlet)
  {
    autom.outlet = Process::make_outlet(Id<Process::Port>(0), &autom);
    autom.outlet->type = Process::PortType::Message;
  }

  autom.setTween(obj["Tween"].toBool());
  JSONValueWriter v{};
  v.val = obj["Gradient"];
  v.writeTo(autom.m_colors);
}
