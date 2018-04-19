// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "LocalProtocolSettingsWidget.hpp"

#include "LocalProtocolFactory.hpp"
#include "LocalSpecificSettings.hpp"

#include <Device/Protocol/ProtocolSettingsWidget.hpp>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QVBoxLayout>
#include <score/widgets/TextLabel.hpp>

class QWidget;

namespace Engine
{
namespace Network
{
LocalProtocolSettingsWidget::LocalProtocolSettingsWidget(QWidget* parent)
    : ProtocolSettingsWidget(parent)
{
  auto lay = new QFormLayout;
  QLabel* deviceNameLabel = new TextLabel(tr("Local device"), this);
  lay->addWidget(deviceNameLabel);
  m_oscPort = new QSpinBox;
  m_wsPort = new QSpinBox;
  m_oscPort->setRange(0, 65535);
  m_wsPort->setRange(0, 65535);
  lay->addRow(tr("OSC port"), m_oscPort);
  lay->addRow(tr("WebSocket port"), m_wsPort);

  setLayout(lay);

  setSettings(LocalProtocolFactory::static_defaultSettings());
}

Device::DeviceSettings LocalProtocolSettingsWidget::getSettings() const
{
  Device::DeviceSettings s = LocalProtocolFactory::static_defaultSettings();
  // TODO *** protocol was never being set here. Check everywhere.! ***
  s.name = "score";
  Network::LocalSpecificSettings local;
  local.oscPort = m_oscPort->value();
  local.wsPort = m_wsPort->value();
  s.deviceSpecificSettings = QVariant::fromValue(local);
  return s;
}

void LocalProtocolSettingsWidget::setSettings(
    const Device::DeviceSettings& settings)
{
  if (settings.deviceSpecificSettings
          .canConvert<Network::LocalSpecificSettings>())
  {
    auto set = settings.deviceSpecificSettings
                   .value<Network::LocalSpecificSettings>();
    m_oscPort->setValue(set.oscPort);
    m_wsPort->setValue(set.wsPort);
  }
}
}
}
