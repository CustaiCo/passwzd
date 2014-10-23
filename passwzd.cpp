#include <string>
#include <iostream>
#include <stdexcept>
#include <system_error>
#include <systemd/sd-journal.h>
#include "arg.h"
#include "main.h"

int main(int argc, char *argv[])
{
    std::string id_wanted;
    std::string value;
    char *argv0;
    bool newline = false;

    ARGBEGIN
    {
        case 'n':
            newline = true;
            break;
    }
    ARGEND

    // this prevents us from going on if we don't know what password
    if(argv == NULL || *argv == NULL)
        exit(EXIT_FAILURE);

    id_wanted = std::string(*argv);

    try
    {
        value = get_password(id_wanted);
    }
    catch(std::system_error& e)
    {
        errno = e.code().value();
        sd_journal_perror("Unable to read key");
        exit(EXIT_FAILURE);
    }
    catch(std::runtime_error& e)
    {
        sd_journal_print(LOG_INFO,e.what());
        exit(EXIT_FAILURE);
    }
    std::cout << value;
    if(newline)
        std::cout << "\n";
    exit(EXIT_SUCCESS);
}

// vim:et:sw=4:ts=4:ai
