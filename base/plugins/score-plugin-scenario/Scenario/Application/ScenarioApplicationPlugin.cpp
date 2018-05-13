// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <Process/Style/ScenarioStyle.hpp>
#include <Scenario/Document/Interval/FullView/FullViewIntervalPresenter.hpp>
#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentModel.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentPresenter.hpp>
#include <Scenario/Document/ScenarioDocument/ScenarioDocumentView.hpp>
#include <Scenario/Process/ScenarioModel.hpp>
#include <Scenario/Process/Temporal/TemporalScenarioPresenter.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateView.hpp>
#include <score/tools/std/Optional.hpp>
// This part is somewhat similar to what moc does
// with moc_.. stuff generation.
#include "Menus/TransportActions.hpp"
#include "ScenarioApplicationPlugin.hpp"

#include <Process/Dataflow/PortItem.hpp>
#include <Process/LayerPresenter.hpp>
#include <Process/Process.hpp>
#include <Process/Tools/ProcessGraphicsView.hpp>
#include <QAction>
#include <QByteArray>
#include <QIODevice>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QKeySequence>
#include <QList>
#include <QMenu>
#include <QString>
#include <QToolBar>
#include <Scenario/Application/Menus/ObjectMenuActions.hpp>
#include <Scenario/Application/Menus/ScenarioContextMenuManager.hpp>
#include <Scenario/Application/ScenarioActions.hpp>
#include <Scenario/Application/ScenarioEditionSettings.hpp>
#include <Scenario/Document/CommentBlock/CommentBlockModel.hpp>
#include <Scenario/Document/DisplayedElements/DisplayedElementsPresenter.hpp>
#include <Scenario/Document/Event/EventModel.hpp>
#include <Scenario/Document/Interval/Slot.hpp>
#include <Scenario/Document/ScenarioDocument/ProcessFocusManager.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <Scenario/Document/TimeSync/TimeSyncModel.hpp>
#include <Scenario/Palette/Tool.hpp>
#include <algorithm>
#include <core/application/ApplicationSettings.hpp>
#include <core/document/Document.hpp>
#include <core/document/DocumentModel.hpp>
#include <core/document/DocumentPresenter.hpp>
#include <core/document/DocumentView.hpp>
#include <core/presenter/Presenter.hpp>
#include <core/view/Window.hpp>
#include <qnamespace.h>
#include <score/actions/Menu.hpp>
#include <score/document/DocumentInterface.hpp>
#include <score/model/EntityMap.hpp>
#include <score/model/IdentifiedObjectMap.hpp>
#include <score/model/Identifier.hpp>
#include <score/plugins/application/GUIApplicationPlugin.hpp>
#include <score/plugins/documentdelegate/DocumentDelegateModel.hpp>
#include <string.h>

#include <wobjectimpl.h>
W_OBJECT_IMPL(Scenario::ScenarioExecution)
W_OBJECT_IMPL(Scenario::ScenarioApplicationPlugin)
SCORE_DECLARE_ACTION(
    ShowCables, "&Show cables", Dataflow, Qt::ALT + Qt::SHIFT + Qt::Key_G)
namespace Scenario
{
void test_parse_expr_full();

ScenarioApplicationPlugin::ScenarioApplicationPlugin(
    const score::GUIApplicationContext& ctx)
    : GUIApplicationPlugin{ctx}
{
  auto app = QCoreApplication::instance();
  if (auto guiapp = qobject_cast<QGuiApplication*>(app))
  {
    connect(
        guiapp, &QGuiApplication::applicationStateChanged, this,
        [&](Qt::ApplicationState st) { editionSettings().setDefault(); });
  }

  // Register conditions for the actions enablement
  using namespace score;
  using namespace Process;
  ctx.actions.onFocusChange(
      std::make_shared<EnableWhenFocusedProcessIs<ProcessModel>>());
  ctx.actions.onFocusChange(
      std::make_shared<EnableWhenFocusedProcessIs<ScenarioInterface>>());
  ctx.actions.onDocumentChange(
      std::make_shared<EnableWhenDocumentIs<ScenarioDocumentModel>>());

  ctx.actions.onSelectionChange(
      std::make_shared<EnableWhenSelectionContains<IntervalModel>>());
  ctx.actions.onSelectionChange(
      std::make_shared<EnableWhenSelectionContains<EventModel>>());
  ctx.actions.onSelectionChange(
      std::make_shared<EnableWhenSelectionContains<StateModel>>());
  ctx.actions.onSelectionChange(
      std::make_shared<EnableWhenSelectionContains<TimeSyncModel>>());

  auto on_sm = std::make_shared<EnableWhenScenarioModelObject>();
  ctx.actions.onSelectionChange(on_sm);
  ctx.actions.onFocusChange(on_sm);
  auto on_instant_si
      = std::make_shared<EnableWhenScenarioInterfaceInstantObject>();
  ctx.actions.onSelectionChange(on_instant_si);
  ctx.actions.onFocusChange(on_instant_si);
  auto on_si = std::make_shared<EnableWhenScenarioInterfaceObject>();
  ctx.actions.onSelectionChange(on_si);
  ctx.actions.onFocusChange(on_si);

  if (context.applicationSettings.gui)
  {
    m_objectActions.setupContextMenu(m_layerCtxMenuManager);
  }
  // Dataflow
  m_showCables = new QAction{this};
  m_showCables->setCheckable(true);
  m_showCables->setChecked(true);
  connect(m_showCables, &QAction::toggled, this, [this](bool c) {
    auto doc = this->currentDocument();
    if (doc)
    {
      Dataflow::CableItem::g_cables_enabled = c;
      ScenarioDocumentPresenter* plug = score::IDocument::try_get<ScenarioDocumentPresenter>(*doc);

      for(const auto& port : Dataflow::PortItem::g_ports())
      {
        Dataflow::PortItem& item = *port.second;
        item.resetPortVisible();
      }

      for (Dataflow::CableItem& cable : plug->cableItems)
      {
        cable.check();
      }

    }
  });
}

auto ScenarioApplicationPlugin::makeGUIElements() -> GUIElements
{
  using namespace score;
  GUIElements e;

  m_objectActions.makeGUIElements(e);
  m_toolActions.makeGUIElements(e);
  m_transportActions.makeGUIElements(e);

  // Dataflow
  auto& actions = e.actions;
  actions.add<Actions::ShowCables>(m_showCables);

  score::Menu& menu = context.menus.get().at(score::Menus::View());
  menu.menu()->addAction(m_showCables);

  return e;
}

ScenarioApplicationPlugin::~ScenarioApplicationPlugin() = default;

void ScenarioApplicationPlugin::on_presenterDefocused(
    Process::LayerPresenter* pres)
{
  // We set the currently focused view model to a "select" state
  // to prevent problems.
  editionSettings().setDefault();

  disconnect(m_contextMenuConnection);
}

void ScenarioApplicationPlugin::on_presenterFocused(
    Process::LayerPresenter* pres)
{
  // Generic stuff
  if (focusedPresenter())
  {
    disconnect(m_contextMenuConnection);
  }
  if (pres)
  {
    // If a layer is right-clicked,
    // this is called and will create a context menu with slot & process
    // information.
    m_contextMenuConnection = QObject::connect(
        pres, &Process::LayerPresenter::contextMenuRequested, this,
        [=](const QPoint& pos, const QPointF& pt2) {
          QMenu menu(qApp->activeWindow());
          ScenarioContextMenuManager::createLayerContextMenu(
              menu, pos, pt2, m_layerCtxMenuManager, *pres);
          menu.exec(pos);
          menu.close();
        });
  }
  else
  {
    return;
  }

  auto s_pres = dynamic_cast<TemporalScenarioPresenter*>(pres);
  if (s_pres)
  {
    connect(
        s_pres, &TemporalScenarioPresenter::keyPressed, this,
        &ScenarioApplicationPlugin::keyPressed);

    connect(
        s_pres, &TemporalScenarioPresenter::keyReleased, this,
        &ScenarioApplicationPlugin::keyReleased);

    auto& select_act = context.actions.action<Actions::SelectTool>();
    select_act.action()->trigger();
  }
}

void ScenarioApplicationPlugin::on_documentChanged(
    score::Document* olddoc, score::Document* newdoc)
{
  using namespace score;
  // TODO the context menu connection should be reviewed, too.
  this->disconnect(m_focusConnection);
  this->disconnect(m_defocusConnection);

  m_editionSettings.setDefault();
  m_editionSettings.setExecution(false);

  if (!newdoc)
    return;

  // Load cables
  auto& model
      = score::IDocument::modelDelegate<Scenario::ScenarioDocumentModel>(
          *newdoc);
  model.finishLoading();
  // TODO do htis on restore

  // Setup ui
  if (!newdoc->context().app.mainWindow)
    return;

  auto focusManager = processFocusManager();

  if (!focusManager)
    return;

  m_focusConnection = connect(
      focusManager, &Process::ProcessFocusManager::sig_focusedPresenter, this,
      &ScenarioApplicationPlugin::on_presenterFocused);
  m_defocusConnection = connect(
      focusManager, &Process::ProcessFocusManager::sig_defocusedPresenter,
      this, &ScenarioApplicationPlugin::on_presenterDefocused);

  if (focusManager->focusedPresenter())
  {
    // Used when switching between documents
    on_presenterFocused(focusManager->focusedPresenter());
  }
  else
  {
    // We focus by default the first process of the interval in full view
    // we're in
    // TODO this snippet is useful, put it somewhere in some Toolkit file.
    ScenarioDocumentPresenter* pres
        = IDocument::presenterDelegate<ScenarioDocumentPresenter>(*newdoc);
    if (pres)
    {
      FullViewIntervalPresenter* cst_pres
          = pres->presenters().intervalPresenter();

      if (!cst_pres->getSlots().empty())
      {
        focusManager->focus(
            cst_pres->getSlots().front().processes.front().presenter);
      }
    }
  }

  // Finally we focus the View widget.
  if (auto v = newdoc->view())
  {
    auto bev = dynamic_cast<ScenarioDocumentView*>(&v->viewDelegate());
    if (bev)
      bev->view().setFocus();
  }
}

void ScenarioApplicationPlugin::on_activeWindowChanged()
{
  editionSettings().setDefault(); // NOTE maybe useless now ?
}

TemporalScenarioPresenter* ScenarioApplicationPlugin::focusedPresenter() const
{
  if (auto focusManager = processFocusManager())
  {
    return dynamic_cast<TemporalScenarioPresenter*>(
        focusManager->focusedPresenter());
  }
  return nullptr;
}

void ScenarioApplicationPlugin::on_initDocument(score::Document& doc)
{
}

void ScenarioApplicationPlugin::on_createdDocument(score::Document& doc)
{
}

void ScenarioApplicationPlugin::prepareNewDocument()
{
  if (context.applicationSettings.gui)
  {
    auto& stop_action = context.actions.action<Actions::Stop>();
    stop_action.action()->trigger();
  }
}

Process::ProcessFocusManager*
ScenarioApplicationPlugin::processFocusManager() const
{
  if (auto doc = currentDocument())
  {
    if (auto pres = doc->presenter())
    {
      auto bem = dynamic_cast<ScenarioDocumentPresenter*>(
          pres->presenterDelegate());
      if (bem)
      {
        return &bem->focusManager();
      }
    }
  }

  return nullptr;
}
}
