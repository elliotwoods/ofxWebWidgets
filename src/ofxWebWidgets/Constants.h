#pragma once

#include <cstdarg>
#include <set>
#include <list>
#include <map>

#include "ofMain.h"

#include "json.hpp"
using json = nlohmann::json;

#if defined( __WIN32__ ) || defined( _WIN32 )
#define MHD_PLATFORM_H
#include <ws2tcpip.h>
#include <stdint.h>
#else
#include <sys/socket.h>
#endif

typedef SSIZE_T ssize_t;
using namespace std;

extern "C" {
#include "microhttpd.h"
}

