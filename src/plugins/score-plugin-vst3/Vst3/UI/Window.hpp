#pragma once
#include <Vst3/EffectModel.hpp>

#include <score/application/ApplicationContext.hpp>
#include <Media/Effect/Settings/Model.hpp>

#include <pluginterfaces/gui/iplugview.h>

#include <QWindow>
#include <QDialog>

namespace vst3
{

inline const char* currentPlatform()
{
#if defined (__APPLE__)
  return Steinberg::kPlatformTypeNSView;
#elif defined(__linux__)
  return Steinberg::kPlatformTypeX11EmbedWindowID;
#elif defined(_WIN32)
  return Steinberg::kPlatformTypeHWND;
#endif
  return "";
}
struct WindowContainer
{
  WId nativeId;
  QWindow* qwindow{};
  QWidget* container{};
};

class Window;
WindowContainer createVstWindowContainer(Window& parentWindow, const Model& e, const score::DocumentContext& ctx);

class Window : public QDialog
{
  const Model& m_model;
  WindowContainer container;
public:
  Window(const Model& e, const score::DocumentContext& ctx, QWidget* parent)
    : QDialog{parent}
    , m_model{e}
  {
    Steinberg::IPlugView& view = *e.fx.view;

    setAttribute(Qt::WA_DeleteOnClose, true);

    container = createVstWindowContainer(*this, e, ctx);

    bool ontop = score::AppContext().settings<Media::Settings::Model>().getVstAlwaysOnTop();
    if (ontop)
    {
      setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint);
    }
    else
    {
      setWindowFlags(windowFlags() | Qt::WindowCloseButtonHint);
    }
    show();

    e.externalUIVisible(true);
  }

  void resizeEvent(QResizeEvent* event)
  {
    QDialog::resizeEvent(event);

    Steinberg::IPlugView& view = *m_model.fx.view;

    auto& g = this->geometry();
    Steinberg::ViewRect r;
    r.top = 0;
    r.bottom = g.height();
    r.left = 0;
    r.bottom = g.width();

    container.qwindow->resize(r.getWidth(), r.getHeight());
    container.container->resize(r.getWidth(), r.getHeight());

    if(view.canResize() == Steinberg::kResultTrue)
      view.onSize(&r);
  }


  void closeEvent(QCloseEvent* event)
  {
    QPointer<Window> p(this);
    if(auto view = m_model.fx.view)
      view->removed();

    const_cast<QWidget*&>(m_model.externalUI) = nullptr;
    m_model.externalUIVisible(false);
    if (p)
      QDialog::closeEvent(event);
  }

};
}
