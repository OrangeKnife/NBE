#include "Event.h"


#include "Listener.h"
#include "Input.h"
#include <algorithm>
namespace NBE
{
	Input::Input(InputType t):m_inputType(t)
	{}
	unsigned int Input::addEvent(Event* theEvent)
	{
		m_eventQue.push(theEvent);
		return m_eventQue.size() - 1; //return index
	}
	void Input::handleAllEvents()
	{
		while(m_eventQue.size()>0)
		{
			Event* e = m_eventQue.front();
			for(size_t i = 0; i< m_listener.size(); ++i)
			{
				m_listener[i]->handleEvent(e);
				
			}
			delete e;
			m_eventQue.pop();
		}
	}

	void Input::registerListener(Listener* lsn)
	{
		m_listener.push_back(lsn);
	}

	void Input::unRegisterListener(Listener* _lsn)
	{
		for(auto it = m_listener.begin(); it != m_listener.end(); ++it)
		{
			if(*it == _lsn)
			{
				m_listener.erase(it);
				break;
			}
		}
		
	}
}