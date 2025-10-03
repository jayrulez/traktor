/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#pragma once

#include <list>
#include "Ui/RichEdit/RichEdit.h"
#include "Ui/SyntaxRichEdit/SyntaxTypes.h"

// import/export mechanism.
#undef T_DLLCLASS
#if defined(T_UI_EXPORT)
#	define T_DLLCLASS T_DLLEXPORT
#else
#	define T_DLLCLASS T_DLLIMPORT
#endif

namespace traktor::ui
{

class SyntaxLanguage;
class IAutocompleteProvider;
class AutocompletePopup;
class AutocompleteSelectEvent;

/*! RichEdit control with automatic syntax highlighting.
 * \ingroup UI
 */
class T_DLLCLASS SyntaxRichEdit : public RichEdit
{
	T_RTTI_CLASS;

public:
	SyntaxRichEdit();

	bool create(Widget* parent, const std::wstring& text = L"", uint32_t style = WsDoubleBuffer);

	void setLanguage(const SyntaxLanguage* language);

	const SyntaxLanguage* getLanguage() const;

	void setErrorHighlight(int32_t line);

	void getOutline(std::list< SyntaxOutline >& outOutline) const;

	void updateLanguage();

	/*! Set autocomplete provider. */
	void setAutocompleteProvider(IAutocompleteProvider* provider);

	/*! Get autocomplete provider. */
	IAutocompleteProvider* getAutocompleteProvider() const;

	/*! Enable or disable autocomplete. */
	void setAutocompleteEnabled(bool enabled);

	/*! Check if autocomplete is enabled. */
	bool getAutocompleteEnabled() const;

private:
	Ref< const SyntaxLanguage > m_language;
	int32_t m_attributeDefault[2];
	int32_t m_attributeString[2];
	int32_t m_attributeNumber[2];
	int32_t m_attributeSelf[2];
	int32_t m_attributeComment[2];
	int32_t m_attributeFunction[2];
	int32_t m_attributeType[2];
	int32_t m_attributeKeyword[2];
	int32_t m_attributeSpecial[2];
	int32_t m_attributePreprocessor[2];
	int32_t m_attributeError[2];

	Ref< IAutocompleteProvider > m_autocompleteProvider;
	Ref< AutocompletePopup > m_autocompletePopup;
	bool m_autocompleteEnabled = true;
	bool m_processingAutocomplete = false; // Flag to prevent double-processing of Enter
	std::wstring m_lastWord;
	int32_t m_lastWordOffset = -1;

	virtual void contentModified() override;

	void eventChange(ContentChangeEvent* event);

	void eventKeyDown(KeyDownEvent* event);

	void eventKey(KeyEvent* event);

	void eventAutocompleteSelect(AutocompleteSelectEvent* event);

	void triggerAutocomplete();

	void hideAutocomplete();

	std::wstring getCurrentWord(int32_t caretOffset, int32_t& outWordStart) const;

	bool isWordCharacter(wchar_t ch) const;
};

}
