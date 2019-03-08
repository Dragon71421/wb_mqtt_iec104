#include "config_parser.h"

#include <string>
#include <iostream>
#include <unistd.h>

#include <jsoncpp/json/json.h>


void parseCommandLineArgs(int argc, char *argv[])
{
    std::string config_file;
    int verbose_level = 0;
    int opt;
    
    while ((opt = getopt(argc, argv, "hc:v")) != -1) {
        switch (opt) {
        case 'c':
            config_file = optarg;
            break;
        case 'v':
            verbose_level++;
            break;
        case 'h':
        default:
            throw ConfigParserException(std::string("Usage: ") + argv[0] + " -c \"config_file_name\" [-v [-v ...]]");
        }
    }

}