#pragma once
#include <QMetaType>
#include <memory>
#include <score/serialization/VisitorInterface.hpp>
#include <score_lib_state_export.h>

namespace ossia
{
struct unit_t;
}
namespace State
{
struct SCORE_LIB_STATE_EXPORT Unit
{
  Q_GADGET
public:
  Unit() noexcept;
  Unit(const Unit& other) noexcept;
  Unit(Unit&& other) noexcept;
  Unit& operator=(const Unit& other) noexcept;
  Unit& operator=(Unit&& other) noexcept;
  ~Unit();

  Unit(const ossia::unit_t&) noexcept;
  Unit& operator=(const ossia::unit_t&) noexcept;

  operator const ossia::unit_t&() const noexcept;
  operator ossia::unit_t&() noexcept;

  bool operator==(const State::Unit& other) const noexcept;
  bool operator!=(const State::Unit& other) const noexcept;

  const ossia::unit_t& get() const noexcept;
  ossia::unit_t& get() noexcept;

private:
  std::unique_ptr<ossia::unit_t> unit;
};
}

template <>
struct SCORE_LIB_STATE_EXPORT TSerializer<DataStream, ossia::unit_t>
{
  static void readFrom(DataStreamReader& s, const ossia::unit_t& var);
  static void writeTo(DataStreamWriter& s, ossia::unit_t& var);
};

template <>
struct SCORE_LIB_STATE_EXPORT TSerializer<DataStream, State::Unit>
{
  static void readFrom(DataStreamReader& s, const State::Unit& var);
  static void writeTo(DataStreamWriter& s, State::Unit& var);
};

template <>
struct is_custom_serialized<ossia::unit_t> : public std::true_type
{
};

Q_DECLARE_METATYPE(State::Unit)
