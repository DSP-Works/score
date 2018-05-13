#pragma once
#include <QObject>
#include <wobjectdefs.h>
#include <score_plugin_scenario_export.h>

namespace Scenario
{
class SCORE_PLUGIN_SCENARIO_EXPORT ModelConsistency final : public QObject
{
  W_OBJECT(ModelConsistency)

  
  

  bool m_valid{true};
  bool m_warning{false};

public:
  explicit ModelConsistency(QObject* parent);
  ModelConsistency(const ModelConsistency& other);
  ModelConsistency& operator=(const ModelConsistency& other);

  bool isValid() const;
  bool warning() const;

  void setValid(bool arg);
  void setWarning(bool warning);

public:
  void validChanged(bool arg) W_SIGNAL(validChanged, arg);
  void warningChanged(bool warning) W_SIGNAL(warningChanged, warning);

W_PROPERTY(bool, warning READ warning WRITE setWarning NOTIFY warningChanged)

W_PROPERTY(bool, valid READ isValid WRITE setValid NOTIFY validChanged)
};
}
