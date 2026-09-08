// MdAnchors.h : invisible scroll-sync anchors for the markdown preview.
// Insert() places an empty inline anchor <a id=mdN></a> at the start of each
// safe block (headings, list items, quotes, paragraph starts) so the editor
// and the preview pane can be mapped onto each other. Fenced code, tables,
// raw HTML blocks and indented code are left untouched - inserting there
// would change how the markdown renders.

#pragma once
#include <string>
#include <vector>

namespace MdAnchors
{
// out receives src with anchors inserted; offsets[N] is the position of
// anchor N in src (wchar_t units, i.e. the same units as RichEdit character
// indices, which count a CRLF pair as one character).
void Insert(const std::wstring& src, std::wstring& out,
            std::vector<size_t>& offsets);

// RichEdit controls collapse "\r\n" to a single '\r', shifting every
// following index. Converts offsets from src coordinates to RichEdit
// character indices.
std::vector<long> ToRichEditOffsets(const std::wstring& src,
                                    const std::vector<size_t>& offsets);
}
