#include "StateEvent.h"

//##ModelId=3E5B7A7603DA
void mitk::StateEvent::Set(int id, Event const* event)
{
	m_Id = id;
	m_Event = event;
}

//##ModelId=3E5B7B69025D
void mitk::StateEvent::opname()
{
}

//##ModelId=3E5B7AEC0394
int mitk::StateEvent::GetId() const
{
	return m_Id;
}

//##ModelId=3E5B7B030383
const Event const* mitk::StateEvent::GetEvent() const
{
	return m_Event;
}

//##ModelId=3E5B7B9E0137
mitk::StateEvent::StateEvent()
{}

