#pragma once
#include <Curve/CurveStyle.hpp>
#include <wobjectdefs.h>
#include <Curve/Process/CurveProcessPresenter.hpp>
#include <Mapping/MappingModel.hpp>
#include <Mapping/MappingView.hpp>
#include <Process/ProcessContext.hpp>

namespace Mapping
{
class LayerPresenter final
    : public Curve::CurveProcessPresenter<ProcessModel, LayerView>
{
  W_OBJECT(LayerPresenter)
public:
  LayerPresenter(
      const Curve::Style& style,
      const ProcessModel& layer,
      LayerView* view,
      const Process::ProcessPresenterContext& context,
      QObject* parent)
      : CurveProcessPresenter{style, layer, view, context, parent}
  {
  }
};
}
