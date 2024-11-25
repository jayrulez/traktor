#include "TurboBadgerUi/Backend/TBClipboardInterface.h"

#include "Core/Misc/TString.h"

namespace traktor::turbobadgerui
{
    void TBClipboardInterface::Empty()
    {
        m_clipboard.Clear();
    }

    bool TBClipboardInterface::HasText()
    {
        return !m_clipboard.IsEmpty();
    }

    bool TBClipboardInterface::SetText(const char* text)
    {
        return m_clipboard.Set(text);
    }

    bool TBClipboardInterface::GetText(tb::TBStr& text)
    {
        return text.Set(m_clipboard);
    }
}
