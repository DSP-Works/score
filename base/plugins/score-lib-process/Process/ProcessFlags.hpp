#pragma once
#include <ossia/detail/config.hpp>
#include <cinttypes>
namespace Process
{
class ProcessModel;
class ProcessModelFactory;
class LayerFactory;
class EffectFactory;

/**
 * @brief Various settings for processes.
 */
enum ProcessFlags : int64_t
{
  //! Can be loaded as a process of an interval
  SupportsTemporal = 0x1,

  //! Can be loaded in an effect chain
  SupportsEffectChain = 0x2,

  //! Can be loaded in a state
  SupportsState = 0x4,

  //! Action from the user required upon creation
  RequiresCustomData = 0x8,

  //! When created in an interval, go on the top slot or in a new slot
  PutInNewSlot = 0x16,

  //! The process won't change if the parent duration changes (eg it's a filter)
  TimeIndependent = 0x32,

  SupportsLasting = SupportsTemporal | SupportsEffectChain,
  ExternalEffect = SupportsLasting | RequiresCustomData | TimeIndependent,
  SupportsAll = SupportsTemporal | SupportsEffectChain | SupportsState
};

/**
 * \class ProcessFlags_k
 * \brief Metadata to retrieve the ProcessFlags of a process
 */
class ProcessFlags_k;
}
