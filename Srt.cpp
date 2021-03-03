// Author: Torarin Hals Bakke (2012)

// Boost Software License - Version 1.0 - August 17th, 2003

// Permission is hereby granted, free of charge, to any person or organization
// obtaining a copy of the software and accompanying documentation covered by
// this license (the "Software") to use, reproduce, display, distribute,
// execute, and transmit the Software, and to prepare derivative works of the
// Software, and to permit third-parties to whom the Software is furnished to
// do so, all subject to the following:

// The copyright notices in the Software and this entire statement, including
// the above license grant, this restriction and the following disclaimer,
// must be included in all copies of the Software, in whole or in part, and
// all derivative works of the Software, unless such copies or derivative
// works are solely in the form of machine-executable object code generated by
// a source language processor.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
// SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
// FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include "Srt.h"

#include <fstream>
#include <utility>

namespace {
  unsigned int timecode_to_milliseconds(unsigned int h,
                                        unsigned int m,
                                        unsigned int s,
                                        unsigned int f)
  {
    return h*3600000 +
           m*60000 +
           s*1000 +
           f;
  }
}

template <typename T, typename U>
T& getline(T&& input, U&& str) {
  std::getline(std::forward<T>(input), std::forward<U>(str));
  if (!str.empty() && str.back() == '\r') {
    str.resize(str.size()-1);
  }
  return input;
}

bool ReadSrt(const std::string& filename, std::vector<Subtitle>& subtitles) {
  std::ifstream srt(filename);
  if (!srt) return false;

  for (std::string line; getline(srt, line);) {
    unsigned int h, m, s, f, h2, m2, s2, f2;

    if (sscanf(line.c_str(), "%u:%u:%u,%u --> %u:%u:%u,%u",
               &h, &m, &s, &f, &h2, &m2, &s2, &f2)
        != 8)
    {
      continue;
    }

    auto start = (int) timecode_to_milliseconds(h, m, s, f);
    auto stop = (int) timecode_to_milliseconds(h2, m2, s2 ,f2);

    std::vector<std::string> text_lines;
    while (getline(srt, line)) {
      if (line.empty()) break;
      text_lines.push_back(std::move(line));
    }

    if (!subtitles.empty() && subtitles.back().start > start)
      continue;

    subtitles.emplace_back(start, stop, std::move(text_lines));
  }

  return true;
}
