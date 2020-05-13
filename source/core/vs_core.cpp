#include "core/vs_core.h"

#include <thread>

std::thread::id g_debug_mainThreadID;
bool g_debug_isMainThreadIDSet = false;

bool debug_isMainThread() {
    return g_debug_mainThreadID == std::this_thread::get_id();
}

void debug_setMainThread() {
    assert(!g_debug_isMainThreadIDSet);
    g_debug_mainThreadID = std::this_thread::get_id();
    g_debug_isMainThreadIDSet = true;
}