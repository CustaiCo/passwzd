#ifndef __CONFIGFILE_H
#define __CONFIGFILE_H
#include <string>
#include <unordered_map>

class ConfigFile
{
    public:
        bool IsValidConfig();
        static ConfigFile* GetConfig()
        {
            static ConfigFile *instance = NULL;
            if(instance == NULL)
                instance = new ConfigFile();
            return instance;
        }
        const std::string GetValue(const std::string&);

    private:
        ConfigFile();
        bool loaded;
        void ParseFile(std::ifstream&);
        std::unordered_map<std::string, std::string> values;
        static std::string GetConfigFilename();
        static std::string ExpandPath(const char*);
};
#endif
