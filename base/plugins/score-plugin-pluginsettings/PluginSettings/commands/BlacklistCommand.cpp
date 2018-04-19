// This is an open source non-commercial project. Dear PVS-Studio, please check
// it. PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/*
#include "BlacklistCommand.hpp"

#include <QDebug>
#include <score/command/Command.hpp>

struct DataStreamInput;
struct DataStreamOutput;

namespace PluginSettings
{
BlacklistCommand::BlacklistCommand(QString name, bool value)
    : m_blacklistedState{{name, value}}
{
}

void BlacklistCommand::undo(const score::DocumentContext& ctx) const
{
}

void BlacklistCommand::redo(const score::DocumentContext& ctx) const
{
  SCORE_TODO;
  QSettings s;

  auto currentList = s.value("PluginSettings/Blacklist", QStringList
  {}).toStringList();

  for(auto& elt : currentList)
  {
      if(!m_blacklistedState.contains(elt))
      {
          m_blacklistedState[elt] = true;
      }
  }

  QStringList newList;

  for(auto& key : m_blacklistedState.keys())
  {
      if(m_blacklistedState[key] == true)
      {
          newList.push_back(key);
      }
  }

  s.setValue("PluginSettings/Blacklist", newList);

}

void BlacklistCommand::serializeImpl(DataStreamInput&) const
{
}

void BlacklistCommand::deserializeImpl(DataStreamOutput&)
{
}


bool BlacklistCommand::mergeWith(const Command* other)
{
    // TODO

    //if(other->uid() != uid())   // make sure other is also an AppendText
command
    //{
    //    return false;
    //}

    auto cmd = static_cast<const BlacklistCommand*>(other);

    for(auto& key : cmd->m_blacklistedState.keys())
    {
        m_blacklistedState[key] = cmd->m_blacklistedState[key];
    }

    return true;
}


const CommandParentFactoryKey& BlacklistCommand::parentKey() const
{
  static CommandParentFactoryKey p;
  SCORE_TODO;
  return p;
}

const CommandFactoryKey& BlacklistCommand::key() const
{
  static CommandFactoryKey p;
  SCORE_TODO;
  return p;
}

QString BlacklistCommand::description() const
{
  SCORE_TODO;
  return {};
}
}
*/
