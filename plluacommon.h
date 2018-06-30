#ifndef PLLUACOMMON_H
#define PLLUACOMMON_H

/* PostgreSQL */
#include <postgres.h>
#include <fmgr.h>
#include <funcapi.h>
#include <access/heapam.h>
#include <access/xact.h>
#if PG_VERSION_NUM >= 90300
#include <access/htup_details.h>
#endif
#include <catalog/namespace.h>
#include <catalog/pg_proc.h>
#include <catalog/pg_type.h>
#include <commands/trigger.h>
#include <executor/spi.h>
#include <nodes/makefuncs.h>
#include <parser/parse_type.h>
#include <utils/array.h>
#include <utils/builtins.h>
#include <utils/datum.h>
#include <utils/lsyscache.h>
#include <utils/memutils.h>
#include <utils/rel.h>
#include <utils/syscache.h>
#include <utils/typcache.h>
/* Lua */
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#if LUA_VERSION_NUM <= 501
#define lua_pushglobaltable(L) lua_pushvalue(L, LUA_GLOBALSINDEX)
#define lua_setuservalue lua_setfenv
#define lua_getuservalue lua_getfenv
#define lua_rawlen lua_objlen
#define luaP_register(L,l) luaL_register(L, NULL, (l))
#else
#define luaP_register(L,l) luaL_setfuncs(L, (l), 0)
#endif

#if !defined LUA_VERSION_NUM || LUA_VERSION_NUM==501
/*
** Adapted from Lua 5.2.0
*/
void luaL_setfuncs (lua_State *L, const luaL_Reg *l, int nup);
#endif

#define PLLUA_VERSION "PL/Lua dev"

#if defined(PLLUA_DEBUG)
#include "pllua_debug.h"
#define BEGINLUA int ____stk=lua_gettop(L)
#define ENDLUA ____stk =lua_gettop(L)-____stk; if(0!=____stk) ereport(INFO, (errmsg("stk %s>%i",AT,____stk)))
#define ENDLUAV(v) ____stk =(lua_gettop(L)-____stk-v); if(0!=____stk) ereport(INFO, (errmsg("stk %s>%i",AT,____stk)))
#else
#define BEGINLUA
#define ENDLUA
#define ENDLUAV(v)
#endif


#define lua_push_oidstring(L, oid) do\
{\
    luaL_Buffer b;\
    luaL_buffinit(L, &b);\
    lua_pushinteger(L, oid);\
    luaL_addstring(&b, "oid_");\
    luaL_addvalue(&b);\
    luaL_pushresult(&b);\
}while(0)

typedef struct {
    const char* name;
    bool hasTraceback;
} LVMInfo;

/* get MemoryContext for state L */
MemoryContext luaP_getmemctxt (lua_State *L);

lua_State *pllua_getmaster (lua_State *L);
int pllua_getmaster_index(lua_State *L);

#if PG_VERSION_NUM < 110000
    #define TupleDescAttr(tupdesc, i) ((tupdesc)->attrs[(i)])

    #define pg_create_context(name) \
            AllocSetContextCreate(TopMemoryContext, \
                name, ALLOCSET_DEFAULT_MINSIZE, ALLOCSET_DEFAULT_INITSIZE, \
                ALLOCSET_DEFAULT_MAXSIZE);
#else
    #define pg_create_context(name) \
            AllocSetContextCreate(TopMemoryContext, name, ALLOCSET_DEFAULT_SIZES);
#endif

#define lua_swap(L) lua_insert(L, -2)

#define luaP_getfield(L, s) \
    lua_pushlightuserdata((L), (void *)(s)); \
    lua_rawget((L), LUA_REGISTRYINDEX)

#define MTOLUA(state) {MemoryContext ___mcxt,___m;\
    ___mcxt = luaP_getmemctxt(state); \
    ___m  = MemoryContextSwitchTo(___mcxt)

#define MTOPG MemoryContextSwitchTo(___m);}
Oid pg_to_regtype(const char *typ_name);

#endif // PLLUACOMMON_H
