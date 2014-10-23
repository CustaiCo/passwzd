#include "c_bindings.h"
#include "config.h"
#include "main.h"

/* 
 * Get option or NULL if not found in psaferc 
 */
const char* pwsafe_get_config(const char* option)
{
    std::string value;
    ConfigFile *config;
    config = ConfigFile::GetConfig();
    value = config->GetValue(std::string(option));
    if(value.empty())
        return NULL;
    else
        return value.c_str();
}

/*
 * Gets password or NULL if no password exists with that title
 * or an error occured getting the passsword. 
 *
 * If masterpass is NULL, password be attempted to be retrieved
 * from session keyring
 */
const char* pwsafe_get_password(const char* masterpass, const char* title)
{
    std::string value;
    
    if(title == NULL)
        return NULL;

    try
    {
        if(masterpass == NULL)
            value = get_password(std::string(title));
        else
            value = get_password(std::string(masterpass), std::string(title));

        /* assume empty passwords are not valid */
        return value.empty() ? NULL : value.c_str();
    }
    /* don't expose any c++ errors to C code */
    catch(...)
    {
        return NULL;
    }
}
