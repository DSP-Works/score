#pragma once
#include <score/plugins/qt_interfaces/CommandFactory_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryFamily_QtInterface.hpp>
#include <score/plugins/qt_interfaces/FactoryInterface_QtInterface.hpp>
#include <score/plugins/qt_interfaces/GUIApplicationPlugin_QtInterface.hpp>
#include <score/plugins/qt_interfaces/PluginRequirements_QtInterface.hpp>

#include <QObject>

#include <verdigris>

class score_plugin_media final : public score::Plugin_QtInterface,
                                 public score::FactoryInterface_QtInterface,
                                 public score::ApplicationPlugin_QtInterface,
                                 public score::CommandFactory_QtInterface
{
  SCORE_PLUGIN_METADATA(1, "142f926e-b2d9-41ce-aff3-a1dab33d3de2")

public:
  score_plugin_media();
  ~score_plugin_media() override;

  std::vector<std::unique_ptr<score::InterfaceBase>> factories(
      const score::ApplicationContext& ctx,
      const score::InterfaceKey& factoryName) const override;

  std::pair<const CommandGroupKey, CommandGeneratorMap>
  make_commands() override;

  score::ApplicationPlugin*
  make_applicationPlugin(const score::ApplicationContext& app) override;
  score::GUIApplicationPlugin*
  make_guiApplicationPlugin(const score::GUIApplicationContext& app) override;
};
