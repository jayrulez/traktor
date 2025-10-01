/*
 * TRAKTOR
 * Copyright (c) 2022 Anders Pistol.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 */
#include "Ui/SyntaxRichEdit/SyntaxRichEdit.h"

#include "Core/Log/Log.h"
#include "Core/Misc/String.h"
#include "Ui/Application.h"
#include "Ui/StyleSheet.h"
#include "Ui/SyntaxRichEdit/SyntaxLanguage.h"
#include "Ui/Autocomplete/IAutocompleteProvider.h"
#include "Ui/Autocomplete/AutocompletePopup.h"
#include "Ui/Events/KeyDownEvent.h"
#include "Ui/Events/KeyEvent.h"

namespace traktor::ui
{

T_IMPLEMENT_RTTI_CLASS(L"traktor.ui.SyntaxRichEdit", SyntaxRichEdit, RichEdit)

SyntaxRichEdit::SyntaxRichEdit()
{
	m_attributeDefault[0] = m_attributeDefault[1] = 0;
	m_attributeString[0] = m_attributeString[1] = 0;
	m_attributeNumber[0] = m_attributeNumber[1] = 0;
	m_attributeSelf[0] = m_attributeSelf[1] = 0;
	m_attributeComment[0] = m_attributeComment[1] = 0;
	m_attributeFunction[0] = m_attributeFunction[1] = 0;
	m_attributeType[0] = m_attributeType[1] = 0;
	m_attributeKeyword[0] = m_attributeKeyword[1] = 0;
	m_attributeSpecial[0] = m_attributeSpecial[1] = 0;
	m_attributePreprocessor[0] = m_attributePreprocessor[1] = 0;
	m_attributeError[0] = m_attributeError[1] = 0;
}

bool SyntaxRichEdit::create(Widget* parent, const std::wstring& text, uint32_t style)
{
	if (!RichEdit::create(parent, text, style))
		return false;

	m_attributeDefault[0] = addTextAttribute(ColorReference(this, L"color-default"), false, false, false);
	m_attributeDefault[1] = addBackgroundAttribute(ColorReference(this, L"background-color-default"));

	m_attributeString[0] = addTextAttribute(ColorReference(this, L"color-string"), false, false, false);
	m_attributeString[1] = addBackgroundAttribute(ColorReference(this, L"background-color-string"));

	m_attributeNumber[0] = addTextAttribute(ColorReference(this, L"color-number"), false, false, false);
	m_attributeNumber[1] = addBackgroundAttribute(ColorReference(this, L"background-color-number"));

	m_attributeSelf[0] = addTextAttribute(ColorReference(this, L"color-self"), false, true, false);
	m_attributeSelf[1] = addBackgroundAttribute(ColorReference(this, L"background-color-self"));

	m_attributeComment[0] = addTextAttribute(ColorReference(this, L"color-comment"), false, true, false);
	m_attributeComment[1] = addBackgroundAttribute(ColorReference(this, L"background-color-comment"));

	m_attributeFunction[0] = addTextAttribute(ColorReference(this, L"color-function"), false, false, false);
	m_attributeFunction[1] = addBackgroundAttribute(ColorReference(this, L"background-color-function"));

	m_attributeType[0] = addTextAttribute(ColorReference(this, L"color-type"), false, false, false);
	m_attributeType[1] = addBackgroundAttribute(ColorReference(this, L"background-color-type"));

	m_attributeKeyword[0] = addTextAttribute(ColorReference(this, L"color-keyword"), false, false, false);
	m_attributeKeyword[1] = addBackgroundAttribute(ColorReference(this, L"background-color-keyword"));

	m_attributeSpecial[0] = addTextAttribute(ColorReference(this, L"color-special"), false, false, false);
	m_attributeSpecial[1] = addBackgroundAttribute(ColorReference(this, L"background-color-special"));

	m_attributePreprocessor[0] = addTextAttribute(ColorReference(this, L"color-preprocessor"), false, false, false);
	m_attributePreprocessor[1] = addBackgroundAttribute(ColorReference(this, L"background-color-preprocessor"));

	m_attributeError[0] = addTextAttribute(ColorReference(this, L"color-error"), false, false, false);
	m_attributeError[1] = addBackgroundAttribute(ColorReference(this, L"background-color-error"));

	addEventHandler< ContentChangeEvent >(this, &SyntaxRichEdit::eventChange);
	addEventHandler< KeyDownEvent >(this, &SyntaxRichEdit::eventKeyDown);
	addEventHandler< KeyEvent >(this, &SyntaxRichEdit::eventKey);

	// Create autocomplete popup
	m_autocompletePopup = new AutocompletePopup();

	return true;
}

void SyntaxRichEdit::setLanguage(const SyntaxLanguage* language)
{
	m_language = language;
	updateLanguage();
}

const SyntaxLanguage* SyntaxRichEdit::getLanguage() const
{
	return m_language;
}

void SyntaxRichEdit::setErrorHighlight(int32_t line)
{
	updateLanguage();
	if (line >= 0)
	{
		const int32_t offset = getLineOffset(line);
		const int32_t length = getLineLength(line);
		setAttributes(offset, length, m_attributeError[0], m_attributeError[1]);
	}
}

void SyntaxRichEdit::getOutline(std::list< SyntaxOutline >& outOutline) const
{
	if (!m_language)
		return;

	const int32_t fromLine = 0;
	const int32_t toLine = getLineCount();

	for (int32_t line = fromLine; line < toLine; ++line)
	{
		const std::wstring text = getLine(line);
		m_language->outline(line, text, outOutline);
	}
}

void SyntaxRichEdit::updateLanguage()
{
	if (!m_language)
		return;

	const int32_t fromLine = 0;
	const int32_t toLine = getLineCount() - 1;

	SyntaxLanguage::State currentState = SyntaxLanguage::StInvalid;
	int32_t startOffset = getLineOffset(fromLine);
	int32_t endOffset = startOffset;

	Ref< SyntaxLanguage::IContext > context = m_language->createContext();

	for (int32_t line = fromLine; line <= toLine; ++line)
	{
		const std::wstring text = getLine(line);
		for (int32_t i = 0; i < int32_t(text.length());)
		{
			SyntaxLanguage::State state = currentState;
			int32_t consumedChars = 0;

			if (!m_language->consume(context, text.substr(i), state, consumedChars))
				break;

			if (state != currentState)
			{
				if (endOffset > startOffset)
				{
					switch (currentState)
					{
					case SyntaxLanguage::StDefault:
						setAttributes(startOffset, endOffset - startOffset, m_attributeDefault[0], m_attributeDefault[1]);
						break;

					case SyntaxLanguage::StString:
						setAttributes(startOffset, endOffset - startOffset, m_attributeString[0], m_attributeString[1]);
						break;

					case SyntaxLanguage::StNumber:
						setAttributes(startOffset, endOffset - startOffset, m_attributeNumber[0], m_attributeNumber[1]);
						break;

					case SyntaxLanguage::StSelf:
						setAttributes(startOffset, endOffset - startOffset, m_attributeSelf[0], m_attributeSelf[1]);
						break;

					case SyntaxLanguage::StLineComment:
						setAttributes(startOffset, endOffset - startOffset, m_attributeComment[0], m_attributeComment[1]);
						break;

					case SyntaxLanguage::StBlockComment:
						setAttributes(startOffset, endOffset - startOffset, m_attributeComment[0], m_attributeComment[1]);
						break;

					case SyntaxLanguage::StFunction:
						setAttributes(startOffset, endOffset - startOffset, m_attributeFunction[0], m_attributeFunction[1]);
						break;

					case SyntaxLanguage::StType:
						setAttributes(startOffset, endOffset - startOffset, m_attributeType[0], m_attributeType[1]);
						break;

					case SyntaxLanguage::StKeyword:
						setAttributes(startOffset, endOffset - startOffset, m_attributeKeyword[0], m_attributeKeyword[1]);
						break;

					case SyntaxLanguage::StSpecial:
						setAttributes(startOffset, endOffset - startOffset, m_attributeSpecial[0], m_attributeSpecial[1]);
						break;

					case SyntaxLanguage::StPreprocessor:
						setAttributes(startOffset, endOffset - startOffset, m_attributePreprocessor[0], m_attributePreprocessor[1]);
						break;

					default:
						break;
					}

					currentState = state;
					startOffset = endOffset;
					endOffset += consumedChars;
				}
				else
				{
					currentState = state;
					endOffset += consumedChars;
				}
			}
			else
			{
				endOffset += consumedChars;
			}

			i += consumedChars;
		}
	}

	if (endOffset > startOffset)
	{
		switch (currentState)
		{
		case SyntaxLanguage::StDefault:
			setAttributes(startOffset, endOffset - startOffset, m_attributeDefault[0], m_attributeDefault[1]);
			break;

		case SyntaxLanguage::StString:
			setAttributes(startOffset, endOffset - startOffset, m_attributeString[0], m_attributeString[1]);
			break;

		case SyntaxLanguage::StNumber:
			setAttributes(startOffset, endOffset - startOffset, m_attributeNumber[0], m_attributeNumber[1]);
			break;

		case SyntaxLanguage::StSelf:
			setAttributes(startOffset, endOffset - startOffset, m_attributeSelf[0], m_attributeSelf[1]);
			break;

		case SyntaxLanguage::StLineComment:
			setAttributes(startOffset, endOffset - startOffset, m_attributeComment[0], m_attributeComment[1]);
			break;

		case SyntaxLanguage::StBlockComment:
			setAttributes(startOffset, endOffset - startOffset, m_attributeComment[0], m_attributeComment[1]);
			break;

		case SyntaxLanguage::StFunction:
			setAttributes(startOffset, endOffset - startOffset, m_attributeFunction[0], m_attributeFunction[1]);
			break;

		case SyntaxLanguage::StType:
			setAttributes(startOffset, endOffset - startOffset, m_attributeType[0], m_attributeType[1]);
			break;

		case SyntaxLanguage::StKeyword:
			setAttributes(startOffset, endOffset - startOffset, m_attributeKeyword[0], m_attributeKeyword[1]);
			break;

		case SyntaxLanguage::StSpecial:
			setAttributes(startOffset, endOffset - startOffset, m_attributeSpecial[0], m_attributeSpecial[1]);
			break;

		case SyntaxLanguage::StPreprocessor:
			setAttributes(startOffset, endOffset - startOffset, m_attributePreprocessor[0], m_attributePreprocessor[1]);
			break;

		default:
			break;
		}
	}

	setAttributes(endOffset, 0, m_attributeDefault[0], m_attributeDefault[1]);
}

void SyntaxRichEdit::contentModified()
{
	updateLanguage();
}

void SyntaxRichEdit::eventChange(ContentChangeEvent* event)
{
	updateLanguage();

	// Update autocomplete provider with new content
	if (m_autocompleteProvider)
	{
		m_autocompleteProvider->updateContent(getText());
	}
}

void SyntaxRichEdit::setAutocompleteProvider(IAutocompleteProvider* provider)
{
	m_autocompleteProvider = provider;
	if (m_autocompletePopup && provider)
	{
		if (!m_autocompletePopup->create(this, provider))
		{
			log::warning << L"Failed to create autocomplete popup" << Endl;
		}
		else
		{
			m_autocompletePopup->addEventHandler< AutocompleteSelectEvent >(this, &SyntaxRichEdit::eventAutocompleteSelect);
		}
	}
}

IAutocompleteProvider* SyntaxRichEdit::getAutocompleteProvider() const
{
	return m_autocompleteProvider;
}

void SyntaxRichEdit::setAutocompleteEnabled(bool enabled)
{
	m_autocompleteEnabled = enabled;
	if (!enabled && m_autocompletePopup)
		m_autocompletePopup->hide();
}

bool SyntaxRichEdit::getAutocompleteEnabled() const
{
	return m_autocompleteEnabled;
}

void SyntaxRichEdit::eventKeyDown(KeyDownEvent* event)
{
	// Only handle autocomplete if enabled and provider is set
	if (m_autocompleteEnabled && m_autocompleteProvider)
	{

	const VirtualKey key = event->getVirtualKey();

	// Forward certain keys to autocomplete popup if it's visible
	if (m_autocompletePopup && m_autocompletePopup->isVisible(false))
	{
		switch (key)
		{
		case VkUp:
			m_autocompletePopup->selectPrevious();
			event->consume();
			return;
		case VkDown:
			m_autocompletePopup->selectNext();
			event->consume();
			return;
		case VkReturn:
		case VkTab:
			m_processingAutocomplete = true; // Set flag to prevent base class processing
			m_autocompletePopup->acceptSelection();
			m_processingAutocomplete = false; // Clear flag after processing
			event->consume();
			return;
		case VkEscape:
			m_autocompletePopup->hide();
			event->consume();
			return;
		}
	}

	// Trigger autocomplete on certain conditions
	bool shouldTrigger = false;

	switch (key)
	{
	case VkPeriod:
		shouldTrigger = true;
		break;

	default:
		// Check if it's a letter or number that extends current word
		if ((key >= VkA && key <= VkZ) || (key >= Vk0 && key <= Vk9) || key == '_')
		{
			// Trigger after a small delay to let the character be inserted
			Application::getInstance()->defer([this]() {
				triggerAutocomplete();
			});
		}
		else if (key == VkSpace || key == VkBackSpace || key == VkDelete)
		{
			// Hide autocomplete on space or deletion
			hideAutocomplete();
		}
		break;
	}

	if (shouldTrigger)
	{
		// Trigger immediately for dot notation
		Application::getInstance()->defer([this]() {
			triggerAutocomplete();
		});
	}

	} // End autocomplete handling

	// Let the base class handle the event unless it was consumed by autocomplete
	// This is important for normal editor functionality like Enter key processing
}

void SyntaxRichEdit::eventKey(KeyEvent* event)
{
	// Prevent character insertion when autocomplete is processing Enter/Tab
	if (m_processingAutocomplete)
	{
		// Block character insertion during autocomplete operations
		event->consume();
		return;
	}

	// For all other cases, let the base class handle character input normally
}

void SyntaxRichEdit::eventAutocompleteSelect(AutocompleteSelectEvent* event)
{
	const AutocompleteSuggestion& suggestion = event->getSuggestion();

	// Find current word boundaries
	const int32_t caretOffset = getCaretOffset();
	int32_t wordStart;
	const std::wstring currentWord = getCurrentWord(caretOffset, wordStart);

	// Replace current word with suggestion
	const int32_t wordLength = caretOffset - wordStart;

	// Select the partial word by placing caret at end and programmatically creating selection
	placeCaret(caretOffset, false);

	// Simple approach: select backwards to word start and replace
	// We'll select by moving caret to start, then treating next insert as replacement
	std::wstring textBefore = getText().substr(0, wordStart);
	std::wstring textAfter = getText().substr(caretOffset);
	std::wstring newText = textBefore + suggestion.name + textAfter;

	// Set new text and place caret after suggestion
	setText(newText);
	placeCaret(wordStart + (int32_t)suggestion.name.length(), false);

	hideAutocomplete();
}

void SyntaxRichEdit::triggerAutocomplete()
{
	if (!m_autocompleteEnabled || !m_autocompleteProvider || !m_autocompletePopup)
		return;

	const int32_t caretOffset = getCaretOffset();
	int32_t wordStart;
	const std::wstring currentWord = getCurrentWord(caretOffset, wordStart);

	// Don't show autocomplete for very short words (unless after dot)
	if (currentWord.length() < 1)
		return;

	// Check if we're after a dot
	bool afterDot = false;
	if (wordStart > 0)
	{
		const std::wstring text = getText();
		if (wordStart - 1 < (int32_t)text.length() && text[wordStart - 1] == L'.')
			afterDot = true;
	}

	// Build autocomplete context
	AutocompleteContext context;
	context.text = getText();
	context.caretOffset = caretOffset;
	context.currentWord = currentWord;
	context.afterDot = afterDot;
	context.line = getLineFromOffset(caretOffset);
	context.column = caretOffset - getLineOffset(context.line);

	// Get suggestions
	std::vector< AutocompleteSuggestion > suggestions;
	if (m_autocompleteProvider->getSuggestions(context, suggestions))
	{
		// Calculate popup position - get actual caret position from RichEdit
		const int32_t caretLine = getLineFromOffset(caretOffset);
		const int32_t lineOffset = getLineOffset(caretLine);
		const int32_t columnOffset = caretOffset - lineOffset;

		// Get font metrics for better positioning
		const FontMetric fontMetric = getFontMetric();
		const int32_t charWidth = fontMetric.getAdvance(L'M', L'M');
		const int32_t lineHeight = fontMetric.getHeight();

		// Account for editor margins and current scroll position
		const int32_t scrollLine = getScrollLine();
		const int32_t marginLeft = 4; // Minimal left margin
		const int32_t marginTop = 2;  // Minimal top margin

		// Calculate caret position relative to editor client area
		const Point caretPos(
			marginLeft + columnOffset * charWidth,
			marginTop + (caretLine - scrollLine) * lineHeight
		);

		// Convert to screen coordinates and position popup right below caret
		const Point screenPos = clientToScreen(caretPos);
		// Position popup immediately below the caret line with minimal gap
		const Point popupPos(screenPos.x, screenPos.y + lineHeight);

		m_autocompletePopup->showSuggestions(popupPos, suggestions);
	}
	else
	{
		hideAutocomplete();
	}
}

void SyntaxRichEdit::hideAutocomplete()
{
	if (m_autocompletePopup)
		m_autocompletePopup->hide();
}

std::wstring SyntaxRichEdit::getCurrentWord(int32_t caretOffset, int32_t& outWordStart) const
{
	const std::wstring text = getText();

	// Find start of current word
	int32_t wordStart = caretOffset;
	while (wordStart > 0 && isWordCharacter(text[wordStart - 1]))
		wordStart--;

	outWordStart = wordStart;
	return text.substr(wordStart, caretOffset - wordStart);
}

bool SyntaxRichEdit::isWordCharacter(wchar_t ch) const
{
	return (ch >= L'a' && ch <= L'z') ||
		   (ch >= L'A' && ch <= L'Z') ||
		   (ch >= L'0' && ch <= L'9') ||
		   ch == L'_';
}

}
