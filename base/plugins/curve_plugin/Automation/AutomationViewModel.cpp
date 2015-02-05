#include "AutomationViewModel.hpp"
#include "AutomationModel.hpp"

AutomationViewModel::AutomationViewModel(AutomationModel* model,
										 id_type<ProcessViewModelInterface> id,
										 QObject* parent):
	ProcessViewModelInterface{id, "AutomationViewModel", model, parent},
	m_model{model}
{

}

void AutomationViewModel::serialize(SerializationIdentifier identifier, void* data) const
{
}

AutomationModel* AutomationViewModel::model()
{
	return m_model;
}
