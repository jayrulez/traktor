/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include <algorithm>
#include "Core/Log/Log.h"
#include "Ui/Event.h"
#include "Ui/EventSubject.h"

namespace traktor::ui
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.ui.EventSubject", EventSubject, Object)

void EventSubject::raiseEvent(Event* event)
{
	T_ANONYMOUS_VAR(Ref< EventSubject >)(this);

	if (!event)
		return;

	const TypeInfo& eventType = type_of(event);
	for (auto i = m_eventHandlers.begin(); i != m_eventHandlers.end(); ++i)
	{
		if (i->second.handlers.empty() || !is_type_of(*i->first, eventType))
			continue;
		if (i->second.disableCounter != 0)
			continue;

		// Invoke event handlers reversed as the most prioritized are at the end and they should
		// be able to "consume" the event so it wont reach other, less prioritized, handlers.
		const RefArray< IEventHandler >& eventHandlers = i->second.handlers;
		for (Ref< IEventHandler > eventHandler : eventHandlers)
		{
			eventHandler->notify(event);

			// In case this subject has been destroyed in any handler we leave quickly.
			if (m_eventHandlers.empty())
				return;

			if (event->consumed())
				break;
		}
	}
}

void EventSubject::removeAllEventHandlers()
{
	for (auto i = m_eventHandlers.begin(); i != m_eventHandlers.end(); ++i)
		i->second.handlers.clear();
	m_eventHandlers.clear();
}

void EventSubject::addEventHandler(const TypeInfo& eventType, IEventHandler* eventHandler)
{
	auto& eventHandlers = m_eventHandlers[&eventType];
	eventHandlers.handlers.push_front(eventHandler);
}

void EventSubject::removeEventHandler(IEventHandler* eventHandler)
{
	for (auto& it : m_eventHandlers)
		it.second.handlers.remove(eventHandler);
}

bool EventSubject::hasEventHandler(const TypeInfo& eventType)
{
	const auto it = m_eventHandlers.find(&eventType);
	if (it != m_eventHandlers.end())
		return !it->second.handlers.empty();
	else
		return false;
}

void EventSubject::enableEventHandlers(const TypeInfo& eventType)
{
	auto& eventHandlers = m_eventHandlers[&eventType];
	eventHandlers.disableCounter--;
}

void EventSubject::disableEventHandlers(const TypeInfo& eventType)
{
	auto& eventHandlers = m_eventHandlers[&eventType];
	eventHandlers.disableCounter++;
}

}
