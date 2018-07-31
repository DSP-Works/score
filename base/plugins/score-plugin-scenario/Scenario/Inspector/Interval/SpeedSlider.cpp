#include <Scenario/Inspector/Interval/SpeedSlider.hpp>

#include <Scenario/Document/Interval/IntervalModel.hpp>
#include <score/widgets/MarginLess.hpp>
#include <score/widgets/TextLabel.hpp>
#include <score/widgets/ControlWidgets.hpp>
#include <QWidget>
#include <QPushButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QGridLayout>

namespace Scenario
{

SpeedSlider::SpeedSlider(
    const Scenario::IntervalModel& model
    , const score::DocumentContext&
    , bool withButtons
    , QWidget* parent)
  : QWidget{parent}
  , m_model{model}
{
  setObjectName("SpeedSlider");
  auto lay = new score::MarginLess<QGridLayout>{this};
  auto setSpeedFun = [=](double val) {
    auto& dur = ((IntervalModel&)(m_model)).duration;
    auto s = double(val) / 100.0;
    if (dur.speed() != s)
    {
      dur.setSpeed(s);
    }
  };

  if(withButtons)
  {
    // Buttons
    int btn_col = 0;
    for (double factor : {0., 50., 100., 200., 500.})
    {
      auto pb
          = new QPushButton{"x " + QString::number(factor * 0.01), this};
      pb->setMinimumWidth(35);
      pb->setMaximumWidth(45);
      pb->setFlat(true);
      pb->setContentsMargins(0, 0, 0, 0);
      pb->setStyleSheet(
            QStringLiteral("QPushButton { margin: 0px; padding: 0px; border: none; }"));

      connect(pb, &QPushButton::clicked, this, [=] { setSpeedFun(factor); });
      lay->addWidget(pb, 1, btn_col++, 1, 1);
    }
  }

  // Slider
  auto speedSlider = new Control::SpeedSlider{Qt::Horizontal};
  speedSlider->setTickInterval(100);
  speedSlider->setMinimum(-100);
  speedSlider->setMaximum(500);
  speedSlider->setValue(m_model.duration.speed() * 100.);
  con(model.duration, &IntervalDurations::speedChanged, this,
      [=](double s) {
    double r = s * 100;
    if (r != speedSlider->value())
      speedSlider->setValue(r);
  });

  lay->addWidget(speedSlider, 0, 0, 1, 5);

  for (int i = 0; i < 5; i++)
    lay->setColumnStretch(i, 0);
  lay->setColumnStretch(5, 10);
  connect(speedSlider, &QSlider::valueChanged, this, setSpeedFun);
}

}
