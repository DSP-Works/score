// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "StatePresenter.hpp"

#include "StateModel.hpp"

#include <QApplication>
#include <QMimeData>
#include <QStringList>
#include <Scenario/Application/ScenarioApplicationPlugin.hpp>
#include <Scenario/Commands/State/AddMessagesToState.hpp>
#include <Scenario/Document/Event/ExecutionStatus.hpp>
#include <Scenario/Document/State/ItemModel/MessageItemModel.hpp>
#include <Scenario/Document/State/StateModel.hpp>
#include <State/Message.hpp>
#include <State/MessageListSerialization.hpp>
#include <score/command/Dispatchers/CommandDispatcher.hpp>
#include <score/document/DocumentContext.hpp>
#include <score/document/DocumentInterface.hpp>
#include <score/model/IdentifiedObject.hpp>
#include <score/model/ModelMetadata.hpp>
#include <score/selection/Selectable.hpp>
#include <score/serialization/MimeVisitor.hpp>
#include <score/tools/Todo.hpp>
#include <score/widgets/GraphicsItem.hpp>

class QObject;
#include <score/model/Identifier.hpp>

namespace Scenario
{
StatePresenter::StatePresenter(
    const StateModel& model, QGraphicsItem* parentview, QObject* parent)
    : QObject{parent}
    , m_model{model}
    , m_view{new StateView{*this, parentview}}
    , m_dispatcher{score::IDocument::documentContext(m_model).commandStack}
{
  // The scenario catches this :
  con(m_model.selection, &Selectable::changed, m_view,
      &StateView::setSelected);

  con(m_model, &StateModel::sig_statesUpdated, this,
      &StatePresenter::updateStateView);

  con(m_model, &StateModel::statusChanged, m_view, &StateView::setStatus);
  m_view->setStatus(m_model.status());

  connect(m_view, &StateView::startCreateMode, this, [=] {
    auto& plug
        = score::GUIAppContext()
              .guiApplicationPlugin<Scenario::ScenarioApplicationPlugin>();
    plug.editionSettings().setTool(Scenario::Tool::Create);
  });
  connect(m_view, &StateView::dropReceived, this, &StatePresenter::handleDrop);

  updateStateView();
}

StatePresenter::~StatePresenter()
{
}

const Id<StateModel>& StatePresenter::id() const
{
  return m_model.id();
}

StateView* StatePresenter::view() const
{
  return m_view;
}

const StateModel& StatePresenter::model() const
{
  return m_model;
}

bool StatePresenter::isSelected() const
{
  return m_model.selection.get();
}

void StatePresenter::handleDrop(const QMimeData* mime)
{
  // If the mime data has states in it we can handle it.
  if (mime->formats().contains(score::mime::messagelist()))
  {
    Mime<State::MessageList>::Deserializer des{*mime};
    State::MessageList ml = des.deserialize();

    auto cmd = new Command::AddMessagesToState{m_model, ml};

    m_dispatcher.submitCommand(cmd);
  }
}

void StatePresenter::updateStateView()
{
  m_view->setContainMessage(m_model.messages().rootNode().hasChildren());
}
}
