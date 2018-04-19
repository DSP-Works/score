// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "EventShortcut.hpp"

#include <QBoxLayout>
#include <QCheckBox>
#include <QPushButton>
#include <score/widgets/MarginLess.hpp>

namespace Scenario
{
EventShortCut::EventShortCut(QString eventId, QWidget* parent)
    : QWidget{parent}
{
  auto groupLay = new score::MarginLess<QHBoxLayout>{this};

  // browser button
  m_eventBtn = new QPushButton{this};
  m_eventBtn->setText(eventId);
  m_eventBtn->setFlat(true);

  m_box = new QCheckBox{};

  groupLay->addWidget(m_eventBtn);
  groupLay->addWidget(m_box);

  connect(m_eventBtn, &QPushButton::clicked, [=]() { eventSelected(); });
}

bool EventShortCut::isChecked()
{
  return m_box->isChecked();
}

QString EventShortCut::eventName()
{
  return m_eventBtn->text();
}
}
