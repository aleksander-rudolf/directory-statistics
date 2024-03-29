#pragma once

#include <vector>
#include <string>
#include <utility>

struct Results
{
  // path of the largest file in the directory
  std::string largest_file_path;
  // size of the largest file
  long largest_file_size;
  // total number of files in the directory (recursive)
  long n_files;
  // total number of directories in the directory (recursive)
  long n_dirs;
  // cumulative size of all files
  long all_files_size;
  // most common file types, as reported by the file utility,
  // sorted by frequency, and reported with their counts
  std::vector<std::pair<std::string, int>> most_common_types;
  // most common words
  // word = sequence of alphabetic characters, converted to lower case
  // sorted by frequency, reported with their counts
  std::vector<std::pair<std::string, int>> most_common_words;
  // list of groups of duplicate files (top 5)
  //   duplicate_files.size() == number of groups
  //   duplicate_files[0].size() == size of first group
  //   duplicate_files[0][0] == path to the first file in first group
  //   duplicate_files[0][1] == path to the second file in first group
  std::vector<std::vector<std::string>> duplicate_files;
};

Results getDirStats(const std::string & dir_name, int n);
