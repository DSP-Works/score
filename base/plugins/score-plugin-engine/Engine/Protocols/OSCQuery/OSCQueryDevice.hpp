#pragma once
#include <ossia/network/base/device.hpp>

#include <Engine/Protocols/OSSIADevice.hpp>

namespace Engine
{
namespace Network
{
class OSCQueryDevice final : public OwningOSSIADevice
{
  Q_OBJECT
public:
  OSCQueryDevice(const Device::DeviceSettings& settings);

  bool reconnect() override;
  void disconnect() override;
  void recreate(const Device::Node& n) override;

Q_SIGNALS:
  void sig_command();
  void sig_disconnect();
private Q_SLOTS:
  void slot_command();
};
}
}
