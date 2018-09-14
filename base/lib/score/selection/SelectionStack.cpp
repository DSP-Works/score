// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "SelectionStack.hpp"

#include <score/model/IdentifiedObjectAbstract.hpp>
#include <score/selection/FocusManager.hpp>
#include <score/selection/Selectable.hpp>
#include <score/selection/Selection.hpp>
#include <score/tools/Todo.hpp>

#include <QList>
#include <QPointer>
#include <QVector>
#include <qnamespace.h>

#include <wobjectimpl.h>

#include <algorithm>
W_OBJECT_IMPL(score::SelectionStack)
W_OBJECT_IMPL(Selectable)
W_OBJECT_IMPL(score::FocusManager)
namespace score
{
SelectionStack::SelectionStack()
{
  connect(
      this, &SelectionStack::pushNewSelection, this, &SelectionStack::push);
  m_unselectable.push(Selection{});
}

bool SelectionStack::canUnselect() const
{
  return m_unselectable.size() > 1;
}

bool SelectionStack::canReselect() const
{
  return !m_reselectable.empty();
}

void SelectionStack::clear()
{
  m_unselectable.clear();
  m_reselectable.clear();
  m_unselectable.push(Selection{});
  currentSelectionChanged(m_unselectable.top());
}

void SelectionStack::clearAllButLast()
{
  Selection last;
  if (canUnselect())
    last = m_unselectable.top();

  m_unselectable.clear();
  m_reselectable.clear();
  m_unselectable.push(Selection{});
  m_unselectable.push(std::move(last));
}

void SelectionStack::push(const Selection& selection)
{
  // TODO don't push "empty" selections, just add a "deselected" mode.
  if (selection != m_unselectable.top())
  {
    auto s = selection;
    auto it = s.begin();
    while (it != s.end())
    {
      if (*it)
        ++it;
      else
        it = s.erase(it);
    }

    Foreach(s, [&](auto obj) {
      // TODO we should erase connections once the selected objects aren't in
      // the stack anymore.
      connect(
          obj, &IdentifiedObjectAbstract::identified_object_destroyed, this,
          &SelectionStack::prune, Qt::UniqueConnection);
    });

    m_unselectable.push(s);

    if (m_unselectable.size() > 50)
    {
      m_unselectable.removeFirst();
    }
    m_reselectable.clear();

    currentSelectionChanged(s);
  }
}

void SelectionStack::unselect()
{
  m_reselectable.push(m_unselectable.pop());

  if (m_unselectable.empty())
    m_unselectable.push(Selection{});

  currentSelectionChanged(m_unselectable.top());
}

void SelectionStack::reselect()
{
  m_unselectable.push(m_reselectable.pop());

  currentSelectionChanged(m_unselectable.top());
}

void SelectionStack::deselect()
{
  push(Selection{});
}

Selection SelectionStack::currentSelection() const
{
  return canUnselect() ? m_unselectable.top() : Selection{};
}

void SelectionStack::prune(IdentifiedObjectAbstract* p)
{
  {
    int n = m_unselectable.size();
    for (int i = 0; i < n; i++)
    {
      Selection& sel = m_unselectable[i];
      // OPTIMIZEME should be removeOne
      sel.removeAll(p);

      for (auto it = sel.begin(); it != sel.end();)
      {
        // We prune the QPointer that might have been invalidated.
        // This is because if we remove multiple elements at the same time
        // some might still be in the list after the first destroyed() call;
        // they will be refreshed and may lead to crashes.
        if ((*it).isNull())
        {
          it = sel.erase(it);
        }
        else
        {
          ++it;
        }
      }
    }
  }

  {
    int n = m_reselectable.size();
    for (int i = 0; i < n; i++)
    {
      Selection& sel = m_reselectable[i];
      sel.removeAll(p);
      for (auto it = sel.begin(); it != sel.end();)
      {
        if ((*it).isNull())
        {
          it = sel.erase(it);
        }
        else
        {
          ++it;
        }
      }
    }
  }

  m_unselectable.erase(
      std::remove_if(
          m_unselectable.begin(), m_unselectable.end(),
          [](const Selection& s) { return s.empty(); }),
      m_unselectable.end());

  m_reselectable.erase(
      std::remove_if(
          m_reselectable.begin(), m_reselectable.end(),
          [](const Selection& s) { return s.empty(); }),
      m_reselectable.end());

  if (m_unselectable.size() == 0)
    m_unselectable.push(Selection{});

  currentSelectionChanged(m_unselectable.top());
}
}
