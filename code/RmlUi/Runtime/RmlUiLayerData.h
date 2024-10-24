/*
 * TRAKTOR
 * Copyright (c) 2022-2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Runtime/Engine/LayerData.h"
#include "Resource/Id.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_RMLUI_RUNTIME_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::rmlui
{

	class RmlDocument;

	/*! Stage RmlUi layer persistent data.
	 * \ingroup RmlUi
	 */
	class T_DLLCLASS RmlUiLayerData : public runtime::LayerData
	{
		T_RTTI_CLASS;

	public:
		RmlUiLayerData();

		virtual Ref< runtime::Layer > createInstance(runtime::Stage* stage, runtime::IEnvironment* environment) const override final;

		virtual void serialize(ISerializer& s) override final;

	private:
		friend class RmlUiLayerPipeline;

		resource::Id< RmlDocument > m_rmlDocument;
		bool m_clearBackground = false;
	};

}