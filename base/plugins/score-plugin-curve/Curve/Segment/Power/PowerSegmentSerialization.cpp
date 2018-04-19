// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "PowerSegment.hpp"

#include <QJsonObject>
#include <QJsonValue>
#include <score/serialization/DataStreamVisitor.hpp>
#include <score/serialization/JSONVisitor.hpp>

template <>
void DataStreamReader::read(const Curve::PowerSegment& segmt)
{
  m_stream << segmt.gamma;
}

template <>
void DataStreamWriter::write(Curve::PowerSegment& segmt)
{
  m_stream >> segmt.gamma;
}

template <>
void JSONObjectReader::read(const Curve::PowerSegment& segmt)
{
  obj[strings.Power] = segmt.gamma;
}

template <>
void JSONObjectWriter::write(Curve::PowerSegment& segmt)
{
  segmt.gamma = obj[strings.Power].toDouble();
}

template <>
void DataStreamReader::read(const Curve::PowerSegmentData& segmt)
{
  m_stream << segmt.gamma;
}

template <>
void DataStreamWriter::write(Curve::PowerSegmentData& segmt)
{
  m_stream >> segmt.gamma;
}

template <>
void JSONObjectReader::read(const Curve::PowerSegmentData& segmt)
{
  obj[strings.Power] = segmt.gamma;
}

template <>
void JSONObjectWriter::write(Curve::PowerSegmentData& segmt)
{
  segmt.gamma = obj[strings.Power].toDouble();
}
