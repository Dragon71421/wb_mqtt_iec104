#pragma once

#include <string>

/*!
 * @brief Exception for errors of command line parser 
 */
class ConfigParserException : public std::exception
{
    std::string msg;
public:
    ConfigParserException(const std::string &_msg) : msg(_msg) {}
    virtual const char *what() const throw()
    {
        return msg.c_str();
    }
    virtual ~ConfigParserException() throw() {}
};

void parseCommandLineArgs(int argc, char *argv[]);