/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "TurboBadgerUi/Runtime/Types.h"

#include "Resource/Id.h"
#include "Runtime/Engine/LayerData.h"

 // import/export mechanism.
#undef T_DLLCLASS
#if defined(T_TURBOBADGERUI_RUNTIME_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::turbobadgerui
{
	class TurboBadgerUiResource;

	/*! Stage TurboBadgerUi layer persistent data.
	 * \ingroup TurboBadgerUi
	 */
	class T_DLLCLASS TurboBadgerUiLayerData : public runtime::LayerData
	{
		T_RTTI_CLASS;

	public:
		TurboBadgerUiLayerData();

		virtual Ref< runtime::Layer > createInstance(runtime::Stage* stage, runtime::IEnvironment* environment) const override final;

		virtual void serialize(ISerializer& s) override final;

	private:
		friend class TurboBadgerUiLayerPipeline;

		resource::Id< TurboBadgerUiResource > m_uiResource;
	};
}