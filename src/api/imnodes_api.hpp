#pragma once
#include <lje_sdk.h>

namespace imnodes_api {

void init();
void shutdown();
void register_all(lua_State *L);

} // namespace imnodes_api
