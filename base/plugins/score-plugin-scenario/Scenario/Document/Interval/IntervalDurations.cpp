// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "IntervalDurations.hpp"

#include "IntervalModel.hpp"

#include <Process/TimeValue.hpp>
#include <Process/TimeValueSerialization.hpp>
#include <Scenario/Document/ModelConsistency.hpp>

#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONValueVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

#define TIME_TOLERANCE_MSEC 0.5
#include <wobjectimpl.h>
W_OBJECT_IMPL(Scenario::IntervalDurations)
namespace Scenario
{
IntervalDurations::~IntervalDurations()
{
}

IntervalDurations& IntervalDurations::operator=(const IntervalDurations& other)
{
  m_defaultDuration = other.m_defaultDuration;
  m_minDuration = other.m_minDuration;
  m_maxDuration = other.m_maxDuration;
  m_guiDuration = other.m_guiDuration;

  m_playPercentage = other.m_playPercentage;
  m_speed = other.m_speed;
  m_rigidity = other.m_rigidity;

  m_isMinNull = other.m_isMinNull;
  m_isMaxInfinite = other.m_isMaxInfinite;

  return *this;
}

void IntervalDurations::checkConsistency()
{
  m_model.consistency.setWarning(
      minDuration().msec() < 0 - TIME_TOLERANCE_MSEC
      || (isRigid() && minDuration() != maxDuration())); // a voir

  m_model.consistency.setValid(
      minDuration() - TimeVal::fromMsecs(TIME_TOLERANCE_MSEC)
          <= m_defaultDuration
      && maxDuration() + TimeVal::fromMsecs(TIME_TOLERANCE_MSEC)
             >= m_defaultDuration
      && m_defaultDuration.msec() + TIME_TOLERANCE_MSEC > 0);
}

void IntervalDurations::setDefaultDuration(const TimeVal& arg)
{
  if (m_defaultDuration != arg)
  {
    m_defaultDuration = arg;
    defaultDurationChanged(arg);

    if (m_guiDuration < m_defaultDuration)
      setGuiDuration(m_defaultDuration * 1.1);

    checkConsistency();
  }
}

void IntervalDurations::setMinDuration(const TimeVal& arg)
{
  if (m_minDuration != arg && !m_isMinNull)
  {
    m_minDuration = arg;
    minDurationChanged(arg);

    checkConsistency();
  }
}

void IntervalDurations::setMaxDuration(const TimeVal& arg)
{
  if (m_maxDuration != arg)
  {
    m_maxDuration = arg;
    maxDurationChanged(arg);

    if (m_guiDuration < m_maxDuration && !m_maxDuration.isInfinite())
      setGuiDuration(m_maxDuration * 1.1);

    checkConsistency();
  }
}

void IntervalDurations::setGuiDuration(TimeVal guiDuration)
{
  if (m_guiDuration == guiDuration)
    return;

  m_guiDuration = guiDuration;
  guiDurationChanged(guiDuration);
}

void IntervalDurations::setPlayPercentage(double arg)
{
  if (m_playPercentage == arg)
    return;

  auto old = m_playPercentage;

  if (m_defaultDuration * std::abs(arg - old)
      > TimeVal{std::chrono::milliseconds(16)})
  {
    m_playPercentage = arg;
    playPercentageChanged(arg);
  }
}

void IntervalDurations::setRigid(bool arg)
{
  if (m_rigidity == arg)
    return;

  m_rigidity = arg;
  rigidityChanged(arg);
}

void IntervalDurations::setMinNull(bool isMinNull)
{
  if (m_isMinNull == isMinNull)
    return;

  m_isMinNull = isMinNull;
  minNullChanged(isMinNull);
  minDurationChanged(minDuration());
}

void IntervalDurations::setMaxInfinite(bool isMaxInfinite)
{
  if (m_isMaxInfinite == isMaxInfinite)
    return;

  m_isMaxInfinite = isMaxInfinite;
  maxInfiniteChanged(isMaxInfinite);
  maxDurationChanged(maxDuration());
}

SCORE_PLUGIN_SCENARIO_EXPORT void
IntervalDurations::Algorithms::setDurationInBounds(
    IntervalModel& cstr, const TimeVal& time)
{
  if (cstr.duration.defaultDuration() != time)
  {
    // Rigid
    if (cstr.duration.isRigid())
    {
      cstr.duration.setDefaultDuration(time);

      cstr.duration.setMinDuration(time);
      cstr.duration.setMaxDuration(time);
    }
    else // TODO The checking must be done elsewhere if(arg >= m_minDuration &&
         // arg <= m_maxDuration)
         // --> it should be in a command to be undoable
    {
      cstr.duration.setDefaultDuration(time);
    }
  }
}

SCORE_PLUGIN_SCENARIO_EXPORT void
IntervalDurations::Algorithms::changeAllDurations(
    IntervalModel& cstr, const TimeVal& time)
{
  if (cstr.duration.defaultDuration() != time)
  {
    // Note: the OSSIA implementation requires min <= dur <= max at all time
    // and will throw if not the case. Hence this order.
    auto delta = time - cstr.duration.defaultDuration();
    cstr.duration.setDefaultDuration(time);

    cstr.duration.setMinDuration(cstr.duration.minDuration() + delta);
    cstr.duration.setMaxDuration(cstr.duration.maxDuration() + delta);
  }
}

SCORE_PLUGIN_SCENARIO_EXPORT void
IntervalDurations::Algorithms::scaleAllDurations(
    IntervalModel& cstr, const TimeVal& time)
{
  if (cstr.duration.defaultDuration() != time)
  {
    // Note: the OSSIA implementation requires min <= dur <= max at all time
    // and will throw if not the case. Hence this order.
    auto ratio = time / cstr.duration.defaultDuration();
    cstr.duration.setDefaultDuration(time);

    cstr.duration.setMinDuration(cstr.duration.minDuration() * ratio);
    cstr.duration.setMaxDuration(cstr.duration.maxDuration() * ratio);
  }
}
}

template <>
void DataStreamReader::read(const Scenario::IntervalDurations& durs)
{
  m_stream << durs.m_defaultDuration << durs.m_minDuration
           << durs.m_maxDuration << durs.m_guiDuration << durs.m_rigidity
           << durs.m_isMinNull << durs.m_isMaxInfinite;
}

template <>
void DataStreamWriter::write(Scenario::IntervalDurations& durs)
{
  m_stream >> durs.m_defaultDuration >> durs.m_minDuration
      >> durs.m_maxDuration >> durs.m_guiDuration >> durs.m_rigidity
      >> durs.m_isMinNull >> durs.m_isMaxInfinite;
}

template <>
void JSONObjectReader::read(const Scenario::IntervalDurations& durs)
{
  obj[strings.DefaultDuration] = toJsonValue(durs.m_defaultDuration);
  obj[strings.MinDuration] = toJsonValue(durs.m_minDuration);
  obj[strings.MaxDuration] = toJsonValue(durs.m_maxDuration);
  obj[strings.GuiDuration] = toJsonValue(durs.m_guiDuration);
  obj[strings.Rigidity] = durs.m_rigidity;
  obj[strings.MinNull] = durs.m_isMinNull;
  obj[strings.MaxInf] = durs.m_isMaxInfinite;
}

template <>
void JSONObjectWriter::write(Scenario::IntervalDurations& durs)
{
  durs.m_defaultDuration
      = fromJsonValue<TimeVal>(obj[strings.DefaultDuration]);
  durs.m_minDuration = fromJsonValue<TimeVal>(obj[strings.MinDuration]);
  durs.m_maxDuration = fromJsonValue<TimeVal>(obj[strings.MaxDuration]);

  durs.m_rigidity = obj[strings.Rigidity].toBool();
  durs.m_isMinNull = obj[strings.MinNull].toBool();
  durs.m_isMaxInfinite = obj[strings.MaxInf].toBool();

  auto guidur = obj.find(strings.GuiDuration);
  if (guidur != obj.end())
    durs.m_guiDuration = fromJsonValue<TimeVal>(*guidur);
  else
  {
    if (durs.m_isMaxInfinite)
      durs.m_guiDuration = durs.m_defaultDuration;
    else
      durs.m_guiDuration = durs.m_maxDuration;
  }
}
