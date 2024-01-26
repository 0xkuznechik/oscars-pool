# Description

Fun little library (mostly for Rust and C++ practice) to grab odds and
organizes it for pasting into our Oscars pool Google spreadsheet.

# Steps

1. Output scraped Oscars odds (Rust).
2. process the raw data (C++).
3. Print result to stdout.
4. Copy paste stdout to spreadsheet.
5. Split data by '|' token. The spreadsheet should update accordingly.

# Incorporating leveldb

- Figure out how to include and link in BUILD. See example CMakeLists.txt:

```cmake
include_directories(/home/alberttc/ronin/cpp/leveldb/include)
link_directories(/home/alberttc/ronin/cpp/leveldb/build)
```

- Write to leveldb

Data model:

  K: Category. E.g. "2024 Best Director Betting Odds"
  V: Array (i.e. `std::vector`) of data:
    ```c++
    struct BettingOddsDBEntry {
      std::string timestamp;
      std::string nominee;
      std::string odds;
    };
    ```
    E.g. [
      {"2024-01-22", "Movie X", "+100"},
      {"2024-01-22", "Movie Y", "-120"},
      {"2024-01-23", "Movie X", "+110"},
      {"2024-01-23", "Movie Y", "-110"},
   ]

Data model:

  K: Category. E.g. "2024 Best Director Betting Odds"
  V: KV
    K: Movie
    V: array of data
        ```c++
        struct MovieOdds {
          std::string timestamp;
          std::string odds;
        };
        ```

  E.g.
  {
    "2024 Best Director Betting Odds":
      {
        "Movie X": [
                     {"2024-01-22", "+100"},
                     {"2024-01-23", "+110"},
                   ],
        "Movie Y": [
                     {"2024-01-22", "-120"},
                     {"2024-01-23", "+110"},
                   ],
      }

Maybe consider a local Spanner db for a relational db?
