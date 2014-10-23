#include <string>
#include <stdexcept>
#include <system_error>
#include "includes/PWSfile.h"
#include "includes/StringX.h"
#include "includes/UTF8Conv.h"
#include "includes/ItemData.h"
#include "includes/keyutils.h"
#include "config.h"

std::string get_password(const std::string& master, const std::string& id_wanted)
{
    std::string filename;
    StringX file;
    StringX pass;
    CUTF8Conv conv;
    PWSfile::VERSION ver;
    PWSfile* pfile;
    ConfigFile *config;
    int ret;
    int status;
    CItemData item;
    const unsigned char *itemdata;
    size_t len;

    config = ConfigFile::GetConfig();
    filename = config->GetValue("filename");
    if(filename.empty())
        throw std::runtime_error("could not find filename in config file");
   
    conv.FromUTF8((unsigned char *)filename.c_str(),filename.length(),file);
    conv.FromUTF8((unsigned char *)master.c_str(),master.length(),pass);

    ret = PWSfile::CheckPasskey( file, pass, ver);
    if( ret != PWSfile::SUCCESS )
        throw std::runtime_error("password provided by key did not work.");

    pfile = PWSfile::MakePWSfile(file, ver, PWSfile::Read ,status);
    ret = pfile->Open(pass);
    if( ret != PWSfile::SUCCESS )
        throw std::runtime_error("open of file did not work: " + std::to_string(ret));

    while((ret = pfile->ReadRecord(item)) == PWSfile::SUCCESS )
    {
        conv.ToUTF8(item.GetUUID(), itemdata, len);
        if(id_wanted != (const char*)itemdata) {
            conv.ToUTF8(item.GetTitle(), itemdata, len);
            if(id_wanted != (const char*)itemdata)
                continue;
        }
        conv.ToUTF8(item.GetPassword(), itemdata, len);
        return std::string((const char *)itemdata);
    }
    throw std::runtime_error("Unable to find password with id: " + id_wanted);
}

std::string get_password(const std::string& id_wanted)
{
    char* password;
    std::string keyname;
    ConfigFile *config;
    key_serial_t pkey;
    int ret;

    config = ConfigFile::GetConfig();

    keyname = config->GetValue("keyname");
    if(keyname.empty())
        throw std::runtime_error("could not find keyname in config file");

    pkey = request_key("user", keyname.c_str(), "", KEY_SPEC_SESSION_KEYRING);
    if(pkey == -1)
        throw std::runtime_error("Unable to get key " + keyname + " for password unlock");

    ret = keyctl_read_alloc(pkey,(void **)&password);
    if(ret == -1)
        throw std::system_error(errno, std::system_category(), "Cannot read key");

    return get_password(std::string(password), id_wanted);
}

// vim:et:sw=4:ts=4:ai
