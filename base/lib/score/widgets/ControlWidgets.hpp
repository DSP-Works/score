#pragma once
#include <QCheckBox>
#include <QPushButton>
#include <array>
#include <score/widgets/DoubleSlider.hpp>
#include <score_lib_base_export.h>
namespace Control
{
struct SCORE_LIB_BASE_EXPORT ToggleButton : public QPushButton
{
public:
  ToggleButton(std::array<const char*, 2> alt, QWidget* parent)
      : QPushButton{parent}
  {
    alternatives[0] = alt[0];
    alternatives[1] = alt[1];
    setCheckable(true);

    connect(this, &QPushButton::toggled, this, [&](bool b) {
      if (b)
      {
        setText(alternatives[1]);
      }
      else
      {
        setText(alternatives[0]);
      }
    });
    if (isChecked())
    {
      setText(alternatives[1]);
    }
    else
    {
      setText(alternatives[0]);
    }
  }
  std::array<QString, 2> alternatives;

protected:
  void paintEvent(QPaintEvent* event) override;
};

struct SCORE_LIB_BASE_EXPORT ValueSlider : public score::Slider
{
public:
  using Slider::Slider;
  bool moving = false;

protected:
  void paintEvent(QPaintEvent* event) override;
};

struct SCORE_LIB_BASE_EXPORT SpeedSlider : public score::Slider
{
public:
  using Slider::Slider;
  bool moving = false;

protected:
  void paintEvent(QPaintEvent* event) override;
};

struct SCORE_LIB_BASE_EXPORT VolumeSlider : public score::DoubleSlider
{
public:
  using DoubleSlider::DoubleSlider;
  bool moving = false;

protected:
  void paintEvent(QPaintEvent* event) override;
};


struct SCORE_LIB_BASE_EXPORT ValueDoubleSlider
    : public score::DoubleSlider
{
public:
  using score::DoubleSlider::DoubleSlider;
  bool moving = false;
  double min{};
  double max{};

protected:
  void paintEvent(QPaintEvent* event) override;
};

struct SCORE_LIB_BASE_EXPORT ValueLogDoubleSlider
    : public score::DoubleSlider
{
public:
  using score::DoubleSlider::DoubleSlider;
  bool moving = false;
  double min{};
  double max{};

protected:
  void paintEvent(QPaintEvent* event) override;
};

struct SCORE_LIB_BASE_EXPORT ComboSlider : public score::Slider
{
  QStringList array;

public:
  template <std::size_t N>
  ComboSlider(const std::array<const char*, N>& arr, QWidget* parent)
      : score::Slider{parent}
  {
    array.reserve(N);
    for (auto str : arr)
      array.push_back(str);
  }

  ComboSlider(const QStringList& arr, QWidget* parent);

  bool moving = false;

protected:
  void paintEvent(QPaintEvent* event) override;
};

SCORE_LIB_BASE_EXPORT const QPalette& transparentPalette();
static inline auto transparentStylesheet()
{
  return QStringLiteral("QWidget { background-color:transparent }");
}

}
