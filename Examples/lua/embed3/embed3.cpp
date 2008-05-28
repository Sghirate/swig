/* embed3.cpp A C++ embeded interpreter
 
This will register a C++ class with Lua, and then call a Lua function
passing C++ objects to this function.
 
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "swigluarun.h"   // the SWIG external runtime

/* the SWIG wrappered library */
extern "C" int luaopen_example(lua_State*L);

// the code itself
#include "example.h"

// this code pushes a C++ pointer as well as the SWIG type onto the Lua stack
bool push_pointer(lua_State*L, void* ptr, const char* type_name, int owned = 0) {
  // task 1: get the object 'type' which is registered with SWIG
  // you need to call SWIG_TypeQuery() with the class name
  // (normally, just look in the wrapper file to get this)
  swig_type_info * pTypeInfo = SWIG_TypeQuery(L, type_name);
  if (pTypeInfo == 0)
    return false;   // error
  // task 2: push the pointer to the Lua stack
  // this requires a pointer & the type
  // the last param specifies if Lua is responsible for deleting the object
  SWIG_NewPointerObj(L, ptr, pTypeInfo, owned);
  return true;
}

/* This is an example of how to call the Lua function
    void onEvent(Event e) 
  its very tedious, but gives you an idea of the issues involed.
*/
int call_onEvent(lua_State *L, Event e) {
  int top;
  /* ok, here we go:
  push a, push b, call 'add' check & return res
  */
  top = lua_gettop(L);  /* for later */
  lua_pushstring(L, "onEvent");                                  /* function name */
  lua_gettable(L, LUA_GLOBALSINDEX);               /* function to be called */
  if (!lua_isfunction(L, -1)) {
    printf("[C++] error: cannot find function 'OnEvent'\n");
    lua_settop(L, top);  // reset
    return 0;
  }
  // push the event object
  push_pointer(L, &e, "Event *", 0);
  if (lua_pcall(L, 1, 0, 0) != 0)  /* call function with 1 arguments and no result */
  {
    printf("[C++] error running function `OnEvent': %s\n", lua_tostring(L, -1));
    lua_settop(L, top);  // reset
    return 0;
  }
  lua_settop(L, top);  /* reset stack */
  return 1;   // ok
}


int main(int argc, char* argv[]) {
  int ok;
  int res;
  char str[80];
  printf("[C++] Welcome to the simple embedded Lua example v3\n");
  printf("[C++] We are in C++\n");
  printf("[C++] opening a Lua state & loading the libraries\n");
  lua_State *L = lua_open();
  luaopen_base(L);
  luaopen_string(L);
  luaopen_math(L);
  printf("[C++] now loading the SWIG wrappered library\n");
  luaopen_example(L);
  printf("[C++] all looks ok\n");
  printf("\n");
  printf("[C++] lets create an Engine and pass a pointer to Lua\n");
  Engine engine;
  /* this code will pass a pointer into lua, but C++ still owns the object
  this is a little tedious, to do, but lets do it
  we need to pass the pointer (obviously), the type name 
  and a flag which states if Lua should delete the pointer once its finished with it
  The type name is a class name string which is registered with SWIG
  (normally, just look in the wrapper file to get this)
  in this case we don't want Lua to delete the pointer so the ownership flag is 0
  */
  push_pointer(L,&engine,"Engine *",0);
  lua_setglobal(L, "pEngine");  // set as global variable

  printf("[C++] now lets load the file 'runme.lua'\n");
  printf("[C++] any lua code in this file will be executed\n");
  if (luaL_loadfile(L, "runme.lua") || lua_pcall(L, 0, 0, 0)) {
    printf("[C++] ERROR: cannot run lua file: %s", lua_tostring(L, -1));
    exit(3);
  }
  printf("[C++] We are now back in C++, all looks ok\n");
  printf("\n");

  printf("[C++] Lets call the Lua function onEvent(e)\n");
  printf("[C++] We will give it different events, as we wish\n");
  printf("[C++] Starting with STARTUP\n");
  Event ev;
  ev.mType = Event::STARTUP;
  call_onEvent(L, ev);
  printf("[C++] ok\n");
  printf("[C++] now we will try MOUSEPRESS,KEYPRESS,MOUSEPRESS\n");
  ev.mType = Event::MOUSEPRESS;
  call_onEvent(L, ev);
  ev.mType = Event::KEYPRESS;
  call_onEvent(L, ev);
  ev.mType = Event::MOUSEPRESS;
  call_onEvent(L, ev);
  printf("[C++] ok\n");
  printf("[C++] Finally we will SHUTDOWN\n");
  ev.mType = Event::SHUTDOWN;
  call_onEvent(L, ev);
  printf("[C++] ok\n");

  printf("\n");
  printf("[C++] all finished, closing the lua state\n");
  lua_close(L);
  return 0;
}