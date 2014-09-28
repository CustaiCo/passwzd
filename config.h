#ifndef __CONFIGFILE_H
#define __CONFIGFILE_H
#include <string>
#include <map>

class ConfigFile {
    public:
        bool IsValidConfig();
        static ConfigFile* GetConfig() {
            static ConfigFile *instance = NULL;
            if( instance == NULL )
                instance = new ConfigFile();
            return instance;
        }
        const std::string GetValue(const std::string&);

    private:
        ConfigFile();
        void ParseFile(std::ifstream&);
        std::map<std::string, std::string> values;
        bool loaded;
        static std::string GetConfigFilename();
        static std::string ExpandPath(const char*);
};
#endif
