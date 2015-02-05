#include "RemovePoint.hpp"
#include <Automation/AutomationModel.hpp>
using namespace iscore;
#define CMD_UID 2000
#define CMD_NAME "RemovePoint"
#define CMD_DESC QObject::tr("Remove point from curve")

RemovePoint::RemovePoint():
	SerializableCommand{"CurveControl",
						CMD_NAME,
						CMD_DESC}
{
}

RemovePoint::RemovePoint(ObjectPath&& path,
						 double x):
	SerializableCommand{"CurveControl",
						CMD_NAME,
						CMD_DESC},
	m_path{path},
	m_x{x}
{
	auto autom = m_path.find<AutomationModel>();
	m_oldY = autom->points()[x];
}

void RemovePoint::undo()
{
	auto autom = m_path.find<AutomationModel>();
	autom->addPoint(m_x, m_oldY);
}

void RemovePoint::redo()
{
	auto autom = m_path.find<AutomationModel>();
	autom->removePoint(m_x);
}

int RemovePoint::id() const
{
	return CMD_UID;
}

bool RemovePoint::mergeWith(const QUndoCommand* other)
{
	return false;
}

void RemovePoint::serializeImpl(QDataStream& s)
{
	s << m_x << m_oldY;
}

void RemovePoint::deserializeImpl(QDataStream& s)
{
	s >> m_x >> m_oldY;
}
