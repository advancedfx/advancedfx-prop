#ifndef _HLAE_CONFIG_H_
#define _HLAE_CONFIG_H_

#include <hlae/core/debug.h>

class hlaeConfig : public wxObject
{

public:

	hlaeConfig();
	~hlaeConfig();
	
};

extern hlaeConfig g_config;

#endif // _HLAE_CONFIG_H_