#include "getDirStats.h"
#include "digester.h"

#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>

#include <cstdio>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <sstream>
#include <iostream>

constexpr int MIN_WORD_SIZE = 3;
constexpr int MAX_WORD_SIZE = 1024;

static bool is_dir(const std::string & path)
{
    struct stat buff;
    if (0 != stat(path.c_str(), & buff)) return false;
    return S_ISDIR(buff.st_mode);
}

// getDirStats() computes stats about directory a directory
//   dir_name = name of the directory to examine
//   n = how many top words/filet types/groups to report
Results getDirStats(const std::string & dir_name, int n)
{
    Results res;
    res.largest_file_path = "";
    res.largest_file_size = -1;
    res.n_files = 0;
    res.n_dirs = -1;
    res.all_files_size = 0;
    res.most_common_types.clear();
    res.most_common_words.clear();
    res.duplicate_files.clear();

    if(!is_dir(dir_name)){
        res.n_dirs++;
        return res;
    }

    std::unordered_map<std::string,int> file_types;
    std::unordered_map<std::string,int> words;
    std::unordered_map<std::string,std::vector<std::string>> duplicate_files;
    std::vector<std::string> stack;
    FILE * fp;

    /*
    * Information from https://gitlab.com/cpsc457/public/find-empty-directories,
    * was used for assistance in the implementation of this code.
    * Iterate through all sub-directories/files in the specified main-directory.
    */
    stack.push_back(dir_name);
    while(!stack.empty()) {
        auto dirname = stack.back();
        stack.pop_back();

        //Initialize stat struct for current directory/file path.
        struct stat sstruct;
        if(stat(dirname.c_str(), &sstruct) < 0){
            exit(-1);
        }

        //Process all sub-directories of current directory path.
        if(is_dir(dirname)){
            DIR * dir = opendir(dirname.c_str());
            if(dir) {
                while(1) {
	                dirent * de = readdir(dir);
	                if(!de) {
                        break;
                    }
	                std::string name = de->d_name;
	                if( name == "." || name == "..") continue;
	                std::string path = dirname + "/" + de->d_name;
	                stack.push_back(path);
                }
            closedir(dir);
            }
            res.n_dirs++;
        }
        //Process all files of current directory path.
        else{
            if(res.largest_file_size < sstruct.st_size){
                res.largest_file_path = dirname.c_str();
                res.largest_file_size = sstruct.st_size;
            }
            res.n_files++;
            res.all_files_size += sstruct.st_size;

            /*
            * Information from https://gitlab.com/cpsc457/public/popen-example,
            * was used for assistance in the implementation of this code.
            * Get each file type, and save its occurance in the file_types unordered_map<>.
            */
            //Prepare the command to execute in popen()
            std::string cmd = "file -b " + dirname;
            //Run the command in popen()
            fp = popen(cmd.c_str(), "r");
            if(fp == nullptr) {
                printf("popen() failed, quitting\n");
                exit(-1);
            }
            //Get the output of popen() using fgets(3) into raw C-style char buffer
            std::string ftype;
            char buff[4096];
            char * res = fgets(buff, sizeof(buff), fp);
            pclose(fp);
            //Try to parse the buffer
            if(res != nullptr) {
                //Find the end of the first field ('\0', or '\n' or ',')
                int eol = 0;
                while(buff[eol] != ',' && buff[eol] != '\n' && buff[eol] != 0) eol ++;
                //Terminate the string
                buff[eol] = 0;
                //Remember the type
                ftype = buff;
                file_types[ftype]++;
            } 
            else {
                //file(1) did not produce any output... no idea why, so let's
                //Just skip the file, but make the file type something bizare
                printf("file(1) failed, not sure why");
            }
            
            /*
            * Information from https://gitlab.com/cpsc457/public/word-histogram,
            * was used for assistance in the implementation of this code.
            * Get each word in every file, and save its occurance in the words unordered_map<>.
            */
            fp = fopen(dirname.c_str(), "r");
            if(fp == nullptr) {
                printf("fopen() failed, quitting\n");
                exit(-1);
            }
            while(1) {
                std::string result;
                while(1) {
                    int c = fgetc(fp);
                    if(c == EOF) break;
                    c = tolower(c);
                    if(!isalpha(c)) {
                        if(result.size() < MIN_WORD_SIZE) {
                            result.clear();
                            continue;
                        }
                        else {
                            break;
                        }
                    }
                    else {
                        result.push_back(c);
                        if(result.size() >= MAX_WORD_SIZE) {
                            printf("input exceeded %d word size, aborting...\n", MAX_WORD_SIZE);
                            exit(-1);
                        }
                    }
                }

                if(result.size() == 0) break;
                words[result]++;
            }
            fclose(fp);

            //Create a hash ID based on file contents for each file, and save its occurance in the duplicate_files unordered_map<>.
            std::string sha256_path = sha256_from_file(dirname.c_str());
            duplicate_files[sha256_path].emplace_back(dirname.c_str());
        }
    }

    //Comparator function using the number of occurances.
    auto compare = [](auto& a, auto& b) {
        return a.second > b.second;
    };

    //Sort the occurance of file_types in descending order.
    for(auto & f : file_types)
        res.most_common_types.emplace_back(f.first, f.second);
        //If we have more than N entries, we'll sort partially, since
        //We only need the first N to be sorted
    if(res.most_common_types.size() > size_t(n)) {
        std::partial_sort(res.most_common_types.begin(), res.most_common_types.begin() + n, res.most_common_types.end(), compare);
        //Drop all entries after the first n
        res.most_common_types.resize(n);
    }
    else {
        std::sort(res.most_common_types.begin(), res.most_common_types.end(), compare);
    }

    //Sort the occurance of words in descending order.
    for(auto & w : words)
        res.most_common_words.emplace_back(w.first, w.second);
        //If we have more than N entries, we'll sort partially, since
        //We only need the first N to be sorted
    if(res.most_common_words.size() > size_t(n)) {
        std::partial_sort(res.most_common_words.begin(), res.most_common_words.begin() + n, res.most_common_words.end(), compare);
        //Drop all entries after the first n
        res.most_common_words.resize(n);
    }
    else {
        std::sort(res.most_common_words.begin(), res.most_common_words.end(), compare);
    }

    // Comparator function using the size of the groups of duplicate files.
    auto compare_duplicate_files = [](auto& a, auto& b) {
        return a.size() > b.size();
    };

    //Sort the size of the groups of duplicate_files in descending order.
    for(auto & d : duplicate_files){
        if(d.second.size() == 0 || d.second.size() == 1)
            continue;
        res.duplicate_files.emplace_back(d.second);
    }
        //If we have more than N entries, we'll sort partially, since
        //We only need the first N to be sorted
    if(res.duplicate_files.size() > size_t(n)) {
        std::partial_sort(res.duplicate_files.begin(), res.duplicate_files.begin() + n, res.duplicate_files.end(), compare_duplicate_files);
        //Drop all entries after the first n
        res.duplicate_files.resize(n);
    }
    else {
        std::sort(res.duplicate_files.begin(), res.duplicate_files.end(), compare_duplicate_files);
    }

    return res;
}