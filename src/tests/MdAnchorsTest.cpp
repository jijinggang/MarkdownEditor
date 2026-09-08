// MdAnchorsTest.cpp : standalone unit tests for MdAnchors (no MFC).
// Build & run:
//   cl /nologo /EHsc /W3 /D_UNICODE /DUNICODE MdAnchorsTest.cpp ..\MdAnchors.cpp /Fe:MdAnchorsTest.exe && MdAnchorsTest.exe
#include <string>
#include <vector>
#include <cassert>
#include <cstdio>
#include "../MdAnchors.h"

using namespace std;

static int g_fail = 0;

#define CHECK(cond) do { if (!(cond)) { printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); g_fail++; } } while (0)

static bool HasAnchor(const wstring& s, int n) {
    wchar_t buf[32];
    swprintf(buf, 32, L"<a id=md%d></a>", n);
    return s.find(buf) != wstring::npos;
}

int main() {
    // 1. heading: anchor after "# "
    {
        wstring in = L"# Title", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(out == L"# <a id=md0></a>Title");
        CHECK(off.size() == 1 && off[0] == 2);
    }
    // 2. paragraph start: anchor at line start
    {
        wstring in = L"para", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(out == L"<a id=md0></a>para");
        CHECK(off.size() == 1 && off[0] == 0);
    }
    // 3. paragraph continuation not anchored
    {
        wstring in = L"para\ncont\n\nsecond", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.size() == 2);
        CHECK(HasAnchor(out, 0) && HasAnchor(out, 1));
    }
    // 4. fenced code content skipped; blocks after fence anchored
    {
        wstring in = L"# H\n```\ncode\n```\nafter", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.size() == 2); // "# H" and "after"
    }
    // 5. table lines skipped
    {
        wstring in = L"| a | b |\n|---|---|\n| 1 | 2 |", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.empty());
    }
    // 6. list / ordered list / quote markers consumed
    {
        wstring in = L"- item\n1. num\n> quote", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.size() == 3);
        CHECK(off[0] == 2 && off[1] == 10 && off[2] == 16);
        // all three lines are "block starts" (no blank line needed for
        // list/quote after list/quote); line 2 "1. num" follows a list item
        CHECK(out.find(L"- <a id=md0></a>item") != wstring::npos);
        CHECK(out.find(L"1. <a id=md1></a>num") != wstring::npos);
        CHECK(out.find(L"> <a id=md2></a>quote") != wstring::npos);
    }
    // 7. hr / setext underline skipped
    {
        wstring in = L"---\n\n***\n\nTitle\n===", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.size() == 1 && off[0] == 10); // only "Title" (paragraph start)
    }
    // 8. indented code skipped
    {
        wstring in = L"    code line", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.empty());
    }
    // 9. raw html block skipped
    {
        wstring in = L"<div>\nhtml\n</div>", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.empty());
    }
    // 10. CRLF preserved, offsets refer to original text
    {
        wstring in = L"a\r\n\r\n# T\r\n", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(out.find(L"\r\n") != wstring::npos);
        CHECK(off.size() == 2);
        CHECK(off[0] == 0);
        CHECK(off[1] == 7); // after "# " on the third line
        CHECK(in.compare(5, 3, L"# T") == 0);
    }
    // 11. ToRichEditOffsets: CRLF collapses to one char
    {
        wstring src = L"ab\r\ncd";
        vector<size_t> off;
        off.push_back(0);
        off.push_back(5);
        vector<long> rich = MdAnchors::ToRichEditOffsets(src, off);
        CHECK(rich.size() == 2 && rich[0] == 0 && rich[1] == 4);
    }
    // 12. sequential ids
    {
        wstring in = L"# a\n\n# b\n\n# c", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.size() == 3);
        CHECK(HasAnchor(out, 0) && HasAnchor(out, 1) && HasAnchor(out, 2));
    }
    // 13. empty input
    {
        wstring in = L"", out = L"x";
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(out.empty() && off.empty());
    }
    // 14. unicode content: wchar offsets, multibyte untouched
    {
        wstring in = L"# 标题\n\n正文段落", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.size() == 2);
        CHECK(off[0] == 2);
        CHECK(in.compare(off[1], 4, L"正文段落") == 0);
        CHECK(out.find(L"# <a id=md0></a>标题") != wstring::npos);
    }
    // 15. blank lines / whitespace-only lines skipped
    {
        wstring in = L"\n\n   \n\nword", out;
        vector<size_t> off;
        MdAnchors::Insert(in, out, off);
        CHECK(off.size() == 1 && in.compare(off[0], 4, L"word") == 0);
    }

    if (g_fail) { printf("%d FAILURES\n", g_fail); return 1; }
    printf("all tests passed\n");
    return 0;
}
