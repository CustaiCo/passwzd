#include <fstream>
#include "config.h"
#include <systemd/sd-journal.h>
#include <wordexp.h>

#define TOO_MANY_LINES 20

const char* DEFAULT_FILE = "~/.pwsafe/psaferc";

// TODO: better configuration options
ConfigFile::ConfigFile() {
    loaded = false;
    std::string filename = GetConfigFilename();

    std::ifstream file(filename);

    if(!file.is_open()) {
        sd_journal_print(LOG_INFO,"Unable to open log file: %s", "~/.pwsafe/psaferc");
        return;
    }
    ParseFile(file);
}

std::string ConfigFile::ExpandPath(const char* path) {
    wordexp_t result;

    if(path == NULL)
        return "";

    switch(wordexp(path,&result,WRDE_NOCMD)) {
        case WRDE_SYNTAX:
        case WRDE_CMDSUB:
        case WRDE_BADVAL:
            wordfree(&result);
            return "";
        default:
            std::string path(result.we_wordv[0]);
            wordfree(&result);
            return path;
    }
}

std::string ConfigFile::GetConfigFilename() {
    const char* unexpanded;

    unexpanded = getenv("PSAFERC");
    if(unexpanded == NULL)
        unexpanded = DEFAULT_FILE;
    return ExpandPath(unexpanded);

}

bool ConfigFile::IsValidConfig() {
    return loaded;
}

/*
 * returns NULL if not initalized
 * returns the value if found
 */
const std::string ConfigFile::GetValue(const std::string& item) {
    if(!loaded)
        return "";
    try {
        return values.at(item);
    } catch (const std::out_of_range e) {
        return "";
    }
}


void ConfigFile::ParseFile(std::ifstream& cfile) {
    std::string line;
    size_t index;

    while(cfile.good()) {
        std::getline(cfile,line);
        if(line.empty())
            continue;
        if(line.at(0) == '#')
            continue;
        index = line.find('=');
        if(index == 0 || index > line.length() ) {
            sd_journal_print(LOG_INFO,"Invalid configuration line: %s", line.c_str());
            continue;
        }
        std::string key = std::string(line,0,index); 
        std::string value = ExpandPath(line.substr(index+1).c_str());
        values.insert(std::make_pair(key,value));
        if(values.size() > TOO_MANY_LINES) {
            sd_journal_print(LOG_INFO,"Configuration file has more than %d lines", TOO_MANY_LINES);
            break;
        }
    }
    loaded = true;
}
