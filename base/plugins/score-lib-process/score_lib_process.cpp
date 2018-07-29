#include <score_lib_process.hpp>
#include <score_lib_process_commands_files.hpp>
#include <score/plugins/customfactory/FactorySetup.hpp>
#include <score/plugins/application/GUIApplicationPlugin.hpp>
#include <score/plugins/documentdelegate/plugin/DocumentPluginCreator.hpp>
#include <Process/DocumentPlugin.hpp>

score_lib_process::score_lib_process() = default;
score_lib_process::~score_lib_process() = default;

std::vector<std::unique_ptr<score::InterfaceBase>>
score_lib_process::factories(
    const score::ApplicationContext& ctx, const score::InterfaceKey& key) const
{
  using namespace Process;
  return instantiate_factories<
      score::ApplicationContext>(
      ctx, key);
}

std::pair<const CommandGroupKey, CommandGeneratorMap>
score_lib_process::make_commands()
{
  using namespace Process;
  std::pair<const CommandGroupKey, CommandGeneratorMap> cmds{
      CommandFactoryName(), CommandGeneratorMap{}};

  ossia::for_each_type<
    #include <score_lib_process_commands.hpp>
      >(score::commands::FactoryInserter{cmds.second});

  return cmds;
}

score::GUIApplicationPlugin* score_lib_process::make_guiApplicationPlugin(
    const score::GUIApplicationContext& app)
{
  struct app_plug final : public score::GUIApplicationPlugin
  {
    using score::GUIApplicationPlugin::GUIApplicationPlugin;

    void on_initDocument(score::Document& doc)
    {
      score::addDocumentPlugin<Process::DocumentPlugin>(doc);
    }
  };

  return new app_plug{app};
}

#include <score/plugins/PluginInstances.hpp>
SCORE_EXPORT_PLUGIN(score_lib_process)
