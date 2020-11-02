#define ASIO_STANDALONE

#ifdef _WIN32
#define _WIN32_WINNT 0X0A00
#endif

#include "asio.hpp"
#include "asio/ts/buffer.hpp"
#include "asio/ts/internet.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <memory>
#include <mutex>
#include <map>