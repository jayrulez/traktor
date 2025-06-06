/*
 * TRAKTOR
 * Copyright (c) 2022-2025 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Core/Object.h"

#include <string>

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SCRIPT_EDITOR_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor
{

class Guid;

}

namespace traktor::script
{

class IScriptDebugger;
class IScriptProfiler;

/*! Debugger sessions interface.
 * \ingroup Script
 */
class T_DLLCLASS IScriptDebuggerSessions : public Object
{
	T_RTTI_CLASS;

public:
	struct ISessionListener
	{
		virtual void notifyBeginSession(IScriptDebugger* scriptDebugger, IScriptProfiler* scriptProfiler) = 0;

		virtual void notifyEndSession(IScriptDebugger* scriptDebugger, IScriptProfiler* scriptProfiler) = 0;

		virtual void notifySetBreakpoint(const std::wstring& fileName, int32_t lineNumber) = 0;

		virtual void notifyRemoveBreakpoint(const std::wstring& fileName, int32_t lineNumber) = 0;
	};

	/*! Begin new debugging session.
	 *
	 * \param scriptDebugger Script debugger.
	 * \param scriptProfiler Script profiler.
	 */
	virtual void beginSession(script::IScriptDebugger* scriptDebugger, script::IScriptProfiler* scriptProfiler) = 0;

	/*! End debugging session.
	 *
	 * \param scriptDebugger Script debugger.
	 * \param scriptProfiler Script profiler.
	 */
	virtual void endSession(script::IScriptDebugger* scriptDebugger, script::IScriptProfiler* scriptProfiler) = 0;

	/*! Activate new breakpoint.
	 *
	 * \param fileName Script file name.
	 * \param lineNumber Line within script to associate with breakpoint.
	 * \return True if breakpoint was set.
	 */
	virtual bool setBreakpoint(const std::wstring& fileName, int32_t lineNumber) = 0;

	/*! Remove breakpoint.
	 *
	 * \param fileName Script file name.
	 * \param lineNumber Line within script to associate with breakpoint.
	 * \return True if breakpoint was removed.
	 */
	virtual bool removeBreakpoint(const std::wstring& fileName, int32_t lineNumber) = 0;

	/*! Remove all breakpoints from script.
	 *
	 * \param fileName Script file name.
	 * \return True if all breakpoints was removed.
	 */
	virtual bool removeAllBreakpoints(const std::wstring& fileName) = 0;

	/*! Check if breakpoint is set.
	 *
	 * \param fileName Script file name.
	 * \param lineNumber Line within script to check for breakpoint.
	 * \return True if breakpoint is set.
	 */
	virtual bool haveBreakpoint(const std::wstring& fileName, int32_t lineNumber) const = 0;

	/*! Add session listener.
	 *
	 * \param listener Listener implementation.
	 */
	virtual void addListener(ISessionListener* listener) = 0;

	/*! Remove session listener.
	 *
	 * \param listener Listener to remove.
	 */
	virtual void removeListener(ISessionListener* listener) = 0;
};

}
