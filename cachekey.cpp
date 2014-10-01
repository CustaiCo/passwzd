#include <stdexcept>
#include <cstdio>
#include "includes/PWSfile.h"
#include "includes/StringX.h"
#include "includes/UTF8Conv.h"
#include "includes/ItemData.h"
#include "includes/keyutils.h"
#include "config.h"
#include <termios.h>
#include <unistd.h>
#include <systemd/sd-journal.h>
#include <string>

#ifndef CACHE_TIMEOUT
#define CACHE_TIMEOUT 120
#endif

void ToggleEcho(bool);

int main(int argc, char *argv[]) {
    std::string filename;
    std::string keyname;
    std::string timeout; 
    std::string input_password; 
    StringX file;
    StringX pass; 
    CUTF8Conv conv; 
    PWSfile::VERSION ver;
    int ret;
    unsigned long int timeout_seconds;
    key_serial_t pkey;
    ConfigFile *config;

    config = ConfigFile::GetConfig();
    filename = config->GetValue("filename");
    keyname = config->GetValue("keyname");
    timeout = config->GetValue("timeout");
    if(filename.empty() || keyname.empty()) {
        std::cout << "Please correctly set up your psaferc file.\n";
        exit(EXIT_FAILURE);
    }
    conv.FromUTF8((unsigned char *)filename.c_str(),filename.length(),file);

    if(timeout.size() != 0) {
        try {
            timeout_seconds = std::stoul(timeout);
        } catch ( std::logic_error e ) {
            std::cout << "Invalid value '" << timeout << "' found for timeout\n";
            exit(EXIT_FAILURE);
        }
    } else {
        timeout_seconds = CACHE_TIMEOUT;
    }

    do {
        std::cout << "Please enter your password: ";
        ToggleEcho(false);
        std::cin >> input_password;
        ToggleEcho(true);
        if(input_password.empty()) {
            std::cout << "\nNo password entered. Goodbye!\n";
            exit(EXIT_SUCCESS);
        }

        conv.FromUTF8((unsigned char *)input_password.c_str(),input_password.length(),pass);
        ret = PWSfile::CheckPasskey( file, pass, ver); 
        if( ret != PWSfile::SUCCESS )
            std::cout << "\nPassword provided does not work. Please try again.\n\n";
        else 
            break;
    } while (true);

    pkey = add_key("user", keyname.c_str(), input_password.c_str(), 
                   input_password.size()+1, KEY_SPEC_SESSION_KEYRING);
    if(pkey == -1) {
        std::perror("\nUnable to write key to keychain");
        exit(EXIT_FAILURE);
    }
    ret = keyctl_set_timeout(pkey,timeout_seconds);
    if(ret == -1) {
        std::perror("\nUnable to update key timeout");
        exit(EXIT_FAILURE);
    }
    std::cout << "\nSuccessfully updated keychain\n";

    exit(EXIT_SUCCESS);
}

// http://falsinsoft.blogspot.com/2014/05/disable-terminal-echo-in-linux.html
void ToggleEcho(bool echoOn) {
    struct termios TermConf;
    tcgetattr(STDIN_FILENO, &TermConf);
    if(echoOn)
       TermConf.c_lflag |= (ICANON | ECHO);
    else
       TermConf.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &TermConf);
}


// vim:et:sw=4:ts=4:ai
