#include "MdAnchors.h"
#include <algorithm>
#include <cctype>

namespace {

bool IsBlank(const std::wstring& s)
{
    for (wchar_t c : s)
        if (!iswspace((wint_t)c))
            return false;
    return true;
}

// only -, *, _ characters (hr / setext underline)
bool IsRuleLine(const std::wstring& s)
{
    if (s.empty())
        return false;
    for (wchar_t c : s)
        if (c != L'-' && c != L'*' && c != L'_')
            return false;
    return true;
}

bool IsFence(const std::wstring& s)
{
    return (s.compare(0, 3, L"```") == 0) || (s.compare(0, 3, L"~~~") == 0);
}

// list bullet "- ", "* ", "+ ", ordered "1. "/"2) ", quote "> "
// returns number of leading marker chars, 0 if none
size_t MarkerLength(const std::wstring& s)
{
    if (s.size() >= 2 && (s[0] == L'-' || s[0] == L'*' || s[0] == L'+') && s[1] == L' ')
        return 2;
    if (s.size() >= 2 && s[0] == L'>')
        return (s[1] == L' ') ? 2 : 1;
    size_t d = 0;
    while (d < s.size() && iswdigit((wint_t)s[d]) && d < 10)
        d++;
    if (d > 0 && d < s.size() && (s[d] == L'.' || s[d] == L')') && d + 1 < s.size() && s[d + 1] == L' ')
        return d + 2;
    return 0;
}

} // namespace

namespace MdAnchors {

std::vector<long> ToRichEditOffsets(const std::wstring& src,
                                    const std::vector<size_t>& offsets)
{
    std::vector<long> rich(offsets.size(), 0);
    if (offsets.empty())
        return rich;
    // walk src once: every "\r\n" pair counts as one RichEdit character
    size_t i = 0;          // index into src
    long r = 0;            // richedit index
    size_t k = 0;          // index into offsets
    while (i < src.size() && k < offsets.size()) {
        while (k < offsets.size() && offsets[k] <= i) {
            rich[k] = r;
            k++;
        }
        if (i < src.size()) {
            if (src[i] == L'\r' && i + 1 < src.size() && src[i + 1] == L'\n')
                i++;
            i++;
            r++;
        }
    }
    while (k < offsets.size())
        rich[k++] = r;
    return rich;
}

void Insert(const std::wstring& src, std::wstring& out,
            std::vector<size_t>& offsets)
{
    out.clear();
    offsets.clear();
    out.reserve(src.size() + 64);
    offsets.reserve(32);

    bool inFence = false;
    bool prevBlank = true;   // start of document counts as a blank line

    size_t lineStart = 0;
    while (lineStart <= src.size()) {
        if (lineStart == src.size() && lineStart != 0)
            break;
        size_t nl = src.find(L'\n', lineStart);
        const bool hasNl = (nl != std::wstring::npos);
        size_t lineEnd = hasNl ? nl : src.size();      // exclusive, at '\n'
        size_t contentEnd = lineEnd;
        if (contentEnd > lineStart && src[contentEnd - 1] == L'\r')
            contentEnd--;
        std::wstring line = src.substr(lineStart, contentEnd - lineStart);
        std::wstring trimmed;
        size_t firstNonSpace = line.find_first_not_of(L" \t");
        if (firstNonSpace != std::wstring::npos)
            trimmed = line.substr(firstNonSpace);

        const wchar_t* anchor = NULL;
        size_t insertAt = std::wstring::npos; // position within src of anchor

        if (!inFence && IsFence(trimmed)) {
            inFence = true;
        } else if (inFence) {
            if (IsFence(trimmed))
                inFence = false;
        } else if (!trimmed.empty()) {
            const bool indentedCode = (firstNonSpace >= 4);
            if (!indentedCode && !IsRuleLine(trimmed)
                && trimmed[0] != L'|' && trimmed[0] != L'<')
            {
                size_t marker = MarkerLength(trimmed);
                if (marker == 0 && trimmed[0] == L'#') {
                    // ATX heading: consume the '#' run plus following spaces
                    size_t h = 0;
                    while (h < trimmed.size() && h < 6 && trimmed[h] == L'#')
                        h++;
                    while (h < trimmed.size() && trimmed[h] == L' ')
                        h++;
                    marker = h;
                }
                if (marker > 0 || prevBlank) {
                    anchor = L"";
                    insertAt = lineStart + firstNonSpace + marker;
                }
            }
        }

        // append original line, inserting the anchor just after its marker
        if (anchor != NULL) {
            wchar_t buf[32];
            swprintf(buf, 32, L"<a id=md%d></a>", (int)offsets.size());
            offsets.push_back(insertAt);
            out.append(src, lineStart, insertAt - lineStart);
            out.append(buf);
            out.append(src, insertAt, lineEnd - insertAt);
        } else {
            out.append(src, lineStart, lineEnd - lineStart);
        }
        if (hasNl) {
            out.push_back(L'\n');
            lineStart = nl + 1;
        } else {
            lineStart = src.size() + 1;
        }

        prevBlank = trimmed.empty();
        if (inFence == false && IsFence(trimmed))
            prevBlank = true; // content after a closing fence starts fresh
        if (lineStart > src.size())
            break;
    }
}

} // namespace MdAnchors
