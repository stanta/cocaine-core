/*
    Copyright (c) 2011-2012 Andrey Sibiryov <me@kobology.ru>
    Copyright (c) 2011-2012 Other contributors as noted in the AUTHORS file.

    This file is part of Cocaine.

    Cocaine is free software; you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    Cocaine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>. 
*/

#include <boost/format.hpp>
#include <ctime>
#include <cstdio>
#include <sys/uio.h>

#include "cocaine/sinks/file.hpp"

using namespace cocaine;
using namespace cocaine::sink;

file_t::file_t(const std::string& name,
               const Json::Value& args):
    category_type(name, args),
    m_file(NULL)
{
    std::string path = args["path"].asString();

    m_file = std::fopen(path.c_str(), "a");
    
    if(m_file == NULL) {
        throw system_error_t("unable to open '%s' log file", path);
    }
}

file_t::~file_t() {
    if(m_file) {
        std::fclose(m_file);
    }
}

namespace {
    static const char * describe[] = {
        NULL,
        "ERROR",
        "WARNING",
        "INFO",
        "DEBUG"
    };
}

void
file_t::emit(logging::priorities priority,
             const std::string& source,
             const std::string& message)
{
    time_t time = 0;
    tm timeinfo;

    // XXX: Not sure if it's needed.
    std::memset(&timeinfo, 0, sizeof(timeinfo));

    std::time(&time);
    ::localtime_r(&time, &timeinfo);

    char timestamp[128];

    size_t result = std::strftime(timestamp, 128, "%c", &timeinfo);

    BOOST_ASSERT(result != 0);

    boost::format format("[%s] [%s] %s: %s\n");

    format % timestamp 
           % describe[priority] 
           % source 
           % message;

    std::string out(format.str());

    char * buffer = new char[out.size()];

    std::memcpy(
        buffer,
        out.data(),
        out.size()
    );

    iovec io = {
        buffer,
        out.size()
    };

    ssize_t written = ::writev(::fileno(m_file), &io, 1);

    BOOST_ASSERT(written == out.size());
}
