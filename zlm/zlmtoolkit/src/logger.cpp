#include <iostream>
#include "logger.h"


#define INSTANCE_IMP(class_name, ...) \
class_name &class_name::Instance() { \
    static std::shared_ptr<class_name> s_instance(new class_name(__VA_ARGS__)); \
    static class_name &s_instance_ref = *s_instance; \
    return s_instance_ref; \
}

INSTANCE_IMP(Logger,"ysh_logger");

Logger::Logger(const std::string &loggername)
{
    _logger_name = loggername;
}

