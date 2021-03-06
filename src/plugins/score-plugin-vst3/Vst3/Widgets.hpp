#pragma once
#include <Vst3/EffectModel.hpp>
#include <Process/Style/ScenarioStyle.hpp>

#include <score/graphics/GraphicWidgets.hpp>
#include <score/graphics/TextItem.hpp>

#include <QDialog>

#include <Control/Widgets.hpp>

#include <verdigris>

/*
#if defined(HAS_VST2)
#include <Media/Effect/VST/Model.hpp>
#include <Process/Style/ScenarioStyle.hpp>

#include <score/graphics/GraphicWidgets.hpp>
#include <score/graphics/TextItem.hpp>

#include <QDialog>

#include <Control/Widgets.hpp>

#include <verdigris>

namespace Process
{
struct Context;
}
namespace vst3
{

class VSTWindow final : public QDialog
{
  W_OBJECT(VSTWindow)
public:
  static ERect getRect(AEffect& e);
  static bool hasUI(AEffect& e);

  VSTWindow(const Model& e, const score::DocumentContext& ctx, QWidget* parent);

  ~VSTWindow() override;
  void resize(int w, int h);

public:
  void uiClosing() W_SIGNAL(uiClosing);

private:
  static void setup_rect(QWidget* container, int width, int height);

  VSTWindow(const Model& e, const score::DocumentContext& ctx);

  void resizeEvent(QResizeEvent* event) override;
  void closeEvent(QCloseEvent* event) override;

  std::weak_ptr<AEffectWrapper> effect;
  QWidget* m_defaultWidg{};
  const Model& m_model;
};


}
#endif
*/

namespace vst3
{
class VSTEffectItem final : public score::EmptyRectItem
{
  QGraphicsItem* rootItem{};
  std::vector<std::pair<ControlInlet*, score::EmptyRectItem*>> controlItems;

public:
  VSTEffectItem(const Model& effect, const Process::Context& doc, QGraphicsItem* root);

  void setupInlet(const Model& fx, ControlInlet& inlet, const Process::Context& doc);

private:
  void updateRect();
};

class VSTGraphicsSlider final : public QObject,
                                public score::QGraphicsSliderBase<VSTGraphicsSlider>
{
  W_OBJECT(VSTGraphicsSlider)
  Q_INTERFACES(QGraphicsItem)
  friend struct QGraphicsSliderBase<VSTGraphicsSlider>;

  double m_value{};
  Steinberg::Vst::IEditController* fx{};
  Steinberg::Vst::ParamID num{};

private:
  bool m_grab{};

public:
  static const constexpr double min = 0.;
  static const constexpr double max = 1.;
  friend struct score::DefaultGraphicsSliderImpl;
  VSTGraphicsSlider(Steinberg::Vst::IEditController* fx, Steinberg::Vst::ParamID num, QGraphicsItem* parent);

  static double map(double v) { return v; }
  static double unmap(double v) { return v; }

  void setValue(double v);
  double value() const;

  bool moving = false;

public:
  void valueChanged(double arg_1) W_SIGNAL(valueChanged, arg_1);
  void sliderMoved() W_SIGNAL(sliderMoved);
  void sliderReleased() W_SIGNAL(sliderReleased);

private:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
};

struct VSTFloatSlider : ossia::safe_nodes::control_in
{
  static QWidget* make_widget(
      Steinberg::Vst::IEditController* fx,
      const ControlInlet& inlet,
      const score::DocumentContext& ctx,
      QWidget* parent,
      QObject* context);
  static QGraphicsItem* make_item(
      Steinberg::Vst::IEditController* fx,
      ControlInlet& inlet,
      const score::DocumentContext& ctx,
      QGraphicsItem* parent,
      QObject* context);
};

}
