#include "includes/PWSfile.h"
#include "includes/StringX.h"
#include "includes/UTF8Conv.h"
#include "includes/ItemData.h"
#include <getopt.h>
#include <wchar.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "includes/keyutils.h"


int main(int argc, char *argv[]) {
    int opt;
    char * filename;
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

    filename = NULL;
    password = NULL;

    while((opt = getopt(argc, argv, "p:f:")) != -1 ) {
        switch(opt) {
        case 'p':
            password = optarg;
            break;
        case 'f':
            filename = optarg;
            break;
        default: 
            puts( "nope\n" );
        }
    }

    if( password == NULL ) {
        pkey = request_key("user", "demopass", "i need a password", KEY_SPEC_SESSION_KEYRING );
        if(pkey == -1) {
            perror("I could not get my key");
            return -1;
        }
        if(keyctl_read_alloc(pkey,(void **)&password) < 0){
            perror("I could not read key");
            return -1;
        }
    }
    
    if( filename == NULL )
    {
        std::cout << "not enuf\n";
        return -1;
    }
    conv.FromUTF8((unsigned char *)filename,strlen(filename),file);
    conv.FromUTF8((unsigned char *)password,strlen(password),pass);
    ret = PWSfile::CheckPasskey( file, pass, ver); 
    if( ret != PWSfile::SUCCESS ) {
        std::cout << "doesn't work\n";
        std::cout << "return value: " << ret << "\n";
        return -1;
    }

    pfile = PWSfile::MakePWSfile(file, ver, PWSfile::Read ,status);
    ret = pfile->Open(pass);
    if( ret != PWSfile::SUCCESS ) {
        std::cout << "open fail\n";
        std::cout << "return value: " << ret << "\n";
        return -1;
    }
    std::cout << "open success\n";

    while( (ret = pfile->ReadRecord(item)) == PWSfile::SUCCESS )
    {
        std::cout << "Size: " << item.GetSize() << "\n";
        conv.ToUTF8(item.GetUUID(), itemdata, len);
        std::cout << "UUID: " << itemdata << "\n";
        conv.ToUTF8(item.GetTitle(), itemdata, len);
        std::cout << "Title: " << itemdata << "\n";
        conv.ToUTF8(item.GetPassword(), itemdata, len);
        std::cout << "Password: " << itemdata << "\n";
    }

    std::cout << "final return: " << ret << "\n";

    return 0;

}


// vim:et:sw=4:ts=4:ai
