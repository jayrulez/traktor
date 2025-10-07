/*
 * TRAKTOR
 * Copyright (c) 2024 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include "Shine/Layout.h"
#include "Core/Ref.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_SHINE_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::shine
{

class RenderContext;
class IRenderGraph;
class Interactable;
class FontManager;

// Forward declare MouseButton enum
enum class MouseButton;

/*!
 * \brief Root container for UI elements - the canvas.
 *
 * Canvas is the root of the UI hierarchy and manages:
 * - Rendering (owns RenderContext)
 * - Input routing (mouse, keyboard)
 * - Focus management
 * - Update loop
 *
 * Create a Canvas to host your UI, then add UI elements as children.
 */
class T_DLLCLASS Canvas : public Layout
{
	T_RTTI_CLASS;

public:
	/*!
	 * \brief Constructor
	 * \param renderGraph Traktor render graph for drawing
	 * \param width Canvas width in pixels
	 * \param height Canvas height in pixels
	 * \param name Optional name for debugging
	 */
	Canvas(IRenderGraph* renderGraph, float width, float height, const std::wstring& name = L"Canvas");

	/*!
	 * \brief Destructor
	 */
	virtual ~Canvas();

	// === Lifecycle ===

	/*!
	 * \brief Update canvas and all children
	 * \param deltaTime Time since last frame in seconds
	 */
	virtual void update(float deltaTime) override;

	/*!
	 * \brief Render canvas and all children
	 */
	void render();

	// === Rendering ===

	/*!
	 * \brief Get render context
	 */
	RenderContext* getRenderContext() const { return m_renderContext; }

	/*!
	 * \brief Set viewport size (updates canvas size)
	 */
	void setViewportSize(float width, float height);

	// === Input Routing ===

	/*!
	 * \brief Route mouse button down event to elements
	 * \param button Mouse button
	 * \param position Mouse position in canvas space
	 * \return true if event was handled
	 */
	bool handleMouseButtonDown(MouseButton button, const Vector2& position);

	/*!
	 * \brief Route mouse button up event to elements
	 * \param button Mouse button
	 * \param position Mouse position in canvas space
	 * \return true if event was handled
	 */
	bool handleMouseButtonUp(MouseButton button, const Vector2& position);

	/*!
	 * \brief Route mouse move event to elements
	 * \param position Mouse position in canvas space
	 * \return true if event was handled
	 */
	bool handleMouseMove(const Vector2& position);

	/*!
	 * \brief Route mouse wheel event to elements
	 * \param delta Scroll delta
	 * \return true if event was handled
	 */
	bool handleMouseWheel(float delta);

	/*!
	 * \brief Route key down event to focused element
	 * \param key Key code
	 * \return true if event was handled
	 */
	bool handleKeyDown(int key);

	/*!
	 * \brief Route key up event to focused element
	 * \param key Key code
	 * \return true if event was handled
	 */
	bool handleKeyUp(int key);

	/*!
	 * \brief Route text input event to focused element
	 * \param character Unicode character
	 * \return true if event was handled
	 */
	bool handleTextInput(wchar_t character);

	// === Focus Management ===

	/*!
	 * \brief Get currently focused element
	 */
	Interactable* getFocusedElement() const { return m_focusedElement; }

	/*!
	 * \brief Set focused element
	 * \param element Element to focus (or nullptr to clear focus)
	 */
	void setFocusedElement(Interactable* element);

	/*!
	 * \brief Clear focus from all elements
	 */
	void clearFocus();

	// === Hit Testing ===

	/*!
	 * \brief Find topmost interactable element at position
	 * \param position Position in canvas space
	 * \return Topmost interactable element, or nullptr if none
	 */
	Interactable* findInteractableAt(const Vector2& position);

	// === Properties ===

	/*!
	 * \brief Get whether canvas is interactive (receives input)
	 */
	bool isInteractive() const { return m_interactive; }

	/*!
	 * \brief Set whether canvas is interactive
	 */
	void setInteractive(bool interactive) { m_interactive = interactive; }

	// === Font System ===

	/*!
	 * \brief Set font manager for this canvas
	 */
	void setFontManager(FontManager* manager);

	/*!
	 * \brief Get font manager
	 */
	FontManager* getFontManager() const { return m_fontManager; }

protected:
	/*!
	 * \brief Recursively find interactable at position
	 */
	Interactable* findInteractableAtRecursive(UIElement* element, const Vector2& position);

private:
	Ref<RenderContext> m_renderContext;
	IRenderGraph* m_renderGraph;
	Ref<FontManager> m_fontManager;
	Interactable* m_focusedElement = nullptr;
	Interactable* m_hoveredElement = nullptr;
	bool m_interactive = true;
};

}
