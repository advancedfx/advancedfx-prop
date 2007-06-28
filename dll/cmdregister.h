
#ifndef CMDREGISTER_H
#define CMDREGISTER_H

#include "register.h"

#define PREFIX "mirv_"
#define DEBUG_PREFIX "__mirv_"

typedef void (__cdecl *Void_func_t)(void);
extern void CvarRegister(Void_func_t func);
extern void CmdRegister(Void_func_t func);

#define _REGISTER_CVAR_INT(var, cvar, def, flags) \
	pEngfuncs->pfnRegisterVariable(cvar, def, flags); \
	var = pEngfuncs->pfnGetCvarPointer(cvar);

#define _REGISTER_CVAR(var, cvar, def, flags) \
	cvar_t *var; \
	void register_cvar_ ## var ## () { _REGISTER_CVAR_INT(var, cvar, def, flags) } \
	RegisterWithFunction<Void_func_t> register_cvar_ ## var ## _(register_cvar_ ## var, CvarRegister); 

#define REGISTER_CVAR(var, def, flags) _REGISTER_CVAR(var, PREFIX # var, def, flags)
#define REGISTER_DEBUGCVAR(var, def, flags) _REGISTER_CVAR(var, DEBUG_PREFIX # var, def, flags)

#define _REGISTER_CMD_INT(cmd, func) \
	pEngfuncs->pfnAddCommand(cmd, func);

#define _REGISTER_CMD(cmd, func) \
	void func(); \
	void register_cmd_ ## func ## () { _REGISTER_CMD_INT(cmd, func) } \
	RegisterWithFunction<Void_func_t> register_cmd_ ## func ## _(register_cmd_ ## func, CmdRegister);

#define REGISTER_CMD(cmd) _REGISTER_CMD(PREFIX # cmd, cmd ## _cmd)
#define REGISTER_CMD_BEGIN(cmd) _REGISTER_CMD("+" ## PREFIX # cmd, cmd ## _begincmd)
#define REGISTER_CMD_END(cmd) _REGISTER_CMD("-" ## PREFIX # cmd, cmd ## _endcmd)

#define REGISTER_CMD_FUNC(cmd) \
	_REGISTER_CMD(PREFIX # cmd, cmd ## _cmd) \
	void cmd ## _cmd()

#define REGISTER_CMD_FUNC_BEGIN(cmd) \
	_REGISTER_CMD("+" ## PREFIX # cmd, cmd ## _begincmd) \
	void cmd ## _begincmd()

#define REGISTER_CMD_FUNC_END(cmd) \
	_REGISTER_CMD("-" ## PREFIX # cmd, cmd ## _endcmd) \
	void cmd ## _endcmd()

#define CALL_CMD(cmd) cmd ## _cmd();
#define CALL_CMD_BEGIN(cmd) cmd ## _begincmd();
#define CALL_CMD_END(cmd) cmd ## _endcmd();

#define REGISTER_DEBUGCMD_FUNC(cmd) \
	_REGISTER_CMD(DEBUG_PREFIX # cmd, cmd ## _cmd) \
	void cmd ## _cmd()

#endif