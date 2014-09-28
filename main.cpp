#include <string>
#include "includes/PWSfile.h"
#include "includes/StringX.h"
#include "includes/UTF8Conv.h"
#include "includes/ItemData.h"
#include "includes/keyutils.h"
#include "config.h"
#include <systemd/sd-journal.h>

int main(int argc, char *argv[]) {
    std::string filename;
    std::string keyname;
    std::string uuid_wanted; 
    char * password;
    StringX file;
    StringX pass; 
    CUTF8Conv conv; 
    PWSfile::VERSION ver;
    PWSfile* pfile;
    int ret;
    int status;
    CItemData item;
    const unsigned char *itemdata;
    size_t len;
    key_serial_t pkey;
    ConfigFile *config;

    // this prevents us from going on if we don't know what password
    if( argc < 2 )
        exit(EXIT_FAILURE);

    config = ConfigFile::GetConfig();
    filename = config->GetValue("filename");
    if(filename.empty()) {
        sd_journal_print(LOG_INFO,"could not find filename in config file");
        exit(EXIT_FAILURE);
    }

    keyname = config->GetValue("keyname");
    if(keyname.empty()) {
        sd_journal_print(LOG_INFO,"could not find keyname in config file");
        exit(EXIT_FAILURE);
    }
    pkey = request_key("user", keyname.c_str(), "xxxx", KEY_SPEC_SESSION_KEYRING);
    if(pkey == -1) 
    {
        sd_journal_print(LOG_INFO,"Unable to get key %s for password unlock", keyname.c_str());
        exit(EXIT_FAILURE); 
    }
    ret = keyctl_read_alloc(pkey,(void **)&password);
    if(ret == -1)
    {
        sd_journal_perror("Unable to read key");
        exit(EXIT_FAILURE); 
    }
    
    conv.FromUTF8((unsigned char *)filename.c_str(),filename.length(),file);
    conv.FromUTF8((unsigned char *)password,strlen(password),pass);
    uuid_wanted = std::string(argv[1]);

    ret = PWSfile::CheckPasskey( file, pass, ver); 
    if( ret != PWSfile::SUCCESS ) {
        sd_journal_print(LOG_INFO,"password provided by key %s did not work.", keyname.c_str() );
        exit(EXIT_FAILURE); 
    }

    pfile = PWSfile::MakePWSfile(file, ver, PWSfile::Read ,status);
    ret = pfile->Open(pass);
    if( ret != PWSfile::SUCCESS ) {
        sd_journal_print(LOG_INFO,"open of file did not work: %d", ret );
        exit(EXIT_FAILURE); 
    }

    while((ret = pfile->ReadRecord(item)) == PWSfile::SUCCESS )
    {
        conv.ToUTF8(item.GetUUID(), itemdata, len);
        if(uuid_wanted != (const char*)itemdata)
            continue;
        conv.ToUTF8(item.GetPassword(), itemdata, len);
        std::cout << itemdata;
        exit(EXIT_SUCCESS);
    }

    sd_journal_print(LOG_INFO,"unable to find password with uuid: %s", argv[1] );
    exit(EXIT_FAILURE);
}


// vim:et:sw=4:ts=4:ai
