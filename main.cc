// Output scraped Oscars odds (Rust), process the raw data here (C++).
// Then print result to stdout.
#include <time.h>
#include <unistd.h>

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/strings/match.h"
#include "absl/strings/str_cat.h"
#include "absl/strings/str_join.h"
#include "absl/strings/str_replace.h"
#include "absl/strings/str_split.h"
#include "absl/strings/string_view.h"
#include "absl/types/span.h"

namespace helpers {

std::vector<std::string> ReadFileAndReturn(const std::string &filename) {
  std::string line;
  std::ifstream f(filename);

  std::vector<std::string> toks;
  if (f.is_open()) {
    while (getline(f, line))
      // TODO: Is std::move necessary?
      // https://abseil.io/tips/77
      toks.push_back(std::move(line));
  } else {
    fprintf(stderr, "could not open file\n");
  }

  return toks;
}

}  // namespace helpers

namespace {

struct MovieOdds {
  std::string movie_title;
  std::string odds;
};

absl::flat_hash_map<std::string, int> NOMINATION_COUNT_;

std::string GetTime() {
  time_t current_time;
  time(&current_time);

  struct tm *time_info;
  time_info = localtime(&current_time);

  char time_string[80];
  strftime(time_string, sizeof(time_string), "%Y-%m-%d", time_info);

  return time_string;
}

static std::string kCurrentDate = GetTime();

// Search line for begin_delim, save all chars between that and end_delim
std::string GetBlobBetweenDelims(absl::string_view line,
                                 absl::string_view begin_delim,
                                 absl::string_view end_delim) {
  if (!absl::StrContains(line, begin_delim) ||
      !absl::StrContains(line, end_delim)) {
    // std::cout << absl::StrCat(begin_delim, ", ", end_delim, "delims not
    // found" );
    return "";
  }

  std::vector<std::string> split_off_begin = absl::StrSplit(line, begin_delim);
  if (split_off_begin.size() != 2) {
    std::cout << "Split ! 2\n  " << absl::StrJoin(split_off_begin, "|");
    return "";
  }

  std::vector<std::string> split_off_end =
      absl::StrSplit(split_off_begin[1], end_delim);
  if (split_off_end.size() != 2) {
    std::cout << "Split > 2\n  " << absl::StrJoin(split_off_end, "|");
    return "";
  }

  return split_off_end[0];
}

std::string StripHTML(absl::string_view line) {
  return absl::StrReplaceAll(
      line,
      {{"2024Best", "2024 Best"},  // Not html, but let's do it here anyways.
       {"<h2>", ""},
       {"</h2>", ""},
       {"<li>", ""},
       {"</li>", ""}});
}

// TODO: Make getting a block better w/ a helper function
// std::vector<std::string> ParseBlock(absl::string_view delim_begin,
// absl::string_view delim_end)

// Invariant: Call at beginning of "2024Best" token
void ProcessIter(const std::string *s) {
  if (*s != "2024Best") {
    std::cout << "ProcessIter: error" << std::endl;
    return;
  }

  // Process header
  std::vector<std::string> header;
  while (!absl::EndsWith(*s, "</h2>")) {
    header.push_back(StripHTML(*s));
    ++s;
  }
  header.push_back(StripHTML(*s));  // Attached token '.*<\h2> '
  const std::string category = absl::StrJoin(header, " ");

  absl::flat_hash_map</*category=*/std::string, std::vector<MovieOdds> >
      category_odds;

  // Advance to <ul>, which dictates the odds section
  while (*s != "<ul>") {
    ++s;
  }

  std::vector<std::string> current_movie;
  // Iterate until end of odds section
  while (!absl::StrContains(*s, "</ul>")) {
    // Begin current movie block. Example:
    //
    //   <li>Cillian
    //   Murphy
    //   (Oppenheimer)
    //   -250</li>
    if (absl::StrContains(*s, "<li>")) {
      while (!absl::StrContains(*s, "</li>")) {
        current_movie.push_back(StripHTML(*s));
        ++s;
      }
      current_movie.push_back(StripHTML(*s));  // Push back odds
      std::string current_movie_str =
          // Remove trailing odds
          absl::StrJoin(current_movie.begin(), current_movie.end() - 1, " ");

      if (auto s = GetBlobBetweenDelims(current_movie_str, "(", ")");
          s.empty()) {
        NOMINATION_COUNT_[current_movie_str]++;
      } else {
        NOMINATION_COUNT_[s]++;
      }

      std::string current_movie_odds = current_movie[current_movie.size() - 1];

      current_movie.clear();

      MovieOdds odds{.movie_title = std::move(current_movie_str),
                     .odds = std::move(current_movie_odds)};

      category_odds[category].push_back(odds);
    }
    ++s;
  }

  for (const auto &[k, v] : category_odds) {
    std::cout << "Timestamp|Category|Nominee|Odds" << std::endl;
    for (const auto &m : v) {
      std::cout << kCurrentDate << "|" << k << "|" << m.movie_title << "|'"
                << m.odds << std::endl;
    }
  }
}

void Process(absl::Span<const std::string> tokens) {
  auto it = tokens.begin();
  while (it != tokens.end()) {
    // std::cout << *it << std::endl;
    if (*it == "2024Best") {
      ProcessIter(it);
    }
    ++it;
  }
}

}  // namespace

int main(int argc, char *argv[]) {
  std::string f = "./debug-tokenized.log";
  auto v = helpers::ReadFileAndReturn(f);
  Process(v);
  std::cout << "---" << std::endl;
  std::cout << "Movie|Num Noms" << std::endl;
  for (const auto &[k, v] : NOMINATION_COUNT_) {
    std::cout << k << "| " << v << std::endl;
  }
  // TODO: Also add a timestamp header with date
  auto current_time = GetTime();
  return 0;
}
