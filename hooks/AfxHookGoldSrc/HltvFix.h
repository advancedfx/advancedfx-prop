#pragma once

#include "cmd_tools.h"

typedef int (* CL_IsThirdPerson_t)( void );

extern CL_IsThirdPerson_t OldClientCL_IsThirdPerson;
int NewClientCL_IsThirdPerson(void);

extern xcommand_t OldClientCmdDemForceHltv;
void NewClientCmdDemForceHltv(void);