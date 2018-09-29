#pragma once
#include <Dataflow/UI/PortItem.hpp>
#include <Process/Focus/FocusDispatcher.hpp>
#include <Process/LayerPresenter.hpp>
#include <Process/LayerView.hpp>
#include <Process/Style/ScenarioStyle.hpp>
#include <score/graphics/TextItem.hpp>

#include <score/graphics/RectItem.hpp>

#include <QGraphicsProxyWidget>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>

#include <Effect/EffectLayer.hpp>
#include <Engine/Node/Process.hpp>
#include <Engine/Node/Widgets.hpp>

namespace Control
{

struct UISetup
{
  template <typename Info, typename Model, typename View>
  static void
  init(const Model& object, View& self, const score::DocumentContext& doc)
  {
    if constexpr (ossia::safe_nodes::info_functions<Info>::control_count > 0)
    {
      std::size_t i = 0;
      double pos_y = 0.;
      auto& portFactory
          = score::AppContext().interfaces<Process::PortFactoryList>();
      ossia::for_each_in_tuple(
          ossia::safe_nodes::get_controls<Info>{}(), [&](const auto& ctrl) {
            auto item = new score::EmptyRectItem{&self};
            item->setPos(0, pos_y);
            auto inlet = static_cast<Process::ControlInlet*>(
                object.inlets()
                    [ossia::safe_nodes::info_functions<Info>::control_start
                     + i]);

            Process::PortFactory* fact = portFactory.get(inlet->concreteKey());
            auto port = fact->makeItem(*inlet, doc, item, &self);

            const auto& style = ScenarioStyle::instance();
            auto lab = new score::SimpleTextItem{style.EventWaiting, item};
            lab->setText(ctrl.name);
            lab->setPos(15, 2);

            QGraphicsItem* widg
                = ctrl.make_item(ctrl, *inlet, doc, nullptr, &self);
            widg->setParentItem(item);
            widg->setPos(15, lab->boundingRect().height());

            auto h = std::max(
                20., (qreal)(
                         widg->boundingRect().height()
                         + lab->boundingRect().height() + 2.));
            item->setRect(QRectF{0., 0., 170., h});
            port->setPos(7., h / 2.);

            pos_y += h;

            i++;
          });
    }
  }
};

template <typename Info>
class ControlLayerFactory final : public Process::LayerFactory
{
public:
  virtual ~ControlLayerFactory() = default;

private:
  UuidKey<Process::ProcessModel> concreteKey() const noexcept override
  {
    return Metadata<ConcreteKey_k, ControlProcess<Info>>::get();
  }

  bool matches(const UuidKey<Process::ProcessModel>& p) const override
  {
    return p == Metadata<ConcreteKey_k, ControlProcess<Info>>::get();
  }

  Process::LayerView* makeLayerView(
      const Process::ProcessModel& proc,
      QGraphicsItem* parent) const final override
  {
    return new Process::EffectLayerView{parent};
  }

  Process::LayerPresenter* makeLayerPresenter(
      const Process::ProcessModel& lm, Process::LayerView* v,
      const Process::ProcessPresenterContext& context,
      QObject* parent) const final override
  {
    auto& proc = safe_cast<const ControlProcess<Info>&>(lm);
    auto view = safe_cast<Process::EffectLayerView*>(v);
    auto pres = new Process::EffectLayerPresenter{proc, view, context, parent};

    Control::UISetup::init<Info>(
        static_cast<const ControlProcess<Info>&>(proc), *view, context);

    return pres;
  }

  QGraphicsItem* makeItem(
      const Process::ProcessModel& proc, const score::DocumentContext& ctx,
      score::RectItem* parent) const final override
  {
    auto rootItem = new score::EmptyRectItem{parent};
    Control::UISetup::init<Info>(
        static_cast<const ControlProcess<Info>&>(proc), *rootItem, ctx);
    rootItem->setRect(rootItem->childrenBoundingRect());
    return rootItem;
  }
};
}
