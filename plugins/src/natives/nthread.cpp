#include "natives.h"
#include "context.h"
#include "modules/strings.h"
#include <thread>
#include <sstream>

namespace Natives
{
	// native thread_detach(sync_flags:flags);
	AMX_DEFINE_NATIVE(thread_detach, 1)
	{
		amx_RaiseError(amx, AMX_ERR_SLEEP);
		return SleepReturnDetach | (SleepReturnValueMask & params[1]);
	}

	// native thread_attach();
	AMX_DEFINE_NATIVE(thread_attach, 0)
	{
		amx_RaiseError(amx, AMX_ERR_SLEEP);
		return SleepReturnAttach;
	}

	// native thread_sync();
	AMX_DEFINE_NATIVE(thread_sync, 0)
	{
		amx_RaiseError(amx, AMX_ERR_SLEEP);
		return SleepReturnSync;
	}

	// native thread_sleep(ms);
	AMX_DEFINE_NATIVE(thread_sleep, 1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(params[1]));
		return 0;
	}

	// native thread_id(id[], size=sizeof id);
	AMX_DEFINE_NATIVE(thread_id, 2)
	{
		std::ostringstream buf;
		buf << std::this_thread::get_id();
		cell *addr;
		amx_GetAddr(amx, params[1], &addr);
		std::string str = buf.str();
		amx_SetString(addr, str.c_str(), false, false, params[2]);
		return str.size();
	}

	// native String:thread_id_s();
	AMX_DEFINE_NATIVE(thread_id_s, 0)
	{
		std::ostringstream buf;
		buf << std::this_thread::get_id();
		return strings::create(buf.str());
	}
}

static AMX_NATIVE_INFO native_list[] =
{
	AMX_DECLARE_NATIVE(thread_detach),
	AMX_DECLARE_NATIVE(thread_attach),
	AMX_DECLARE_NATIVE(thread_sync),
	AMX_DECLARE_NATIVE(thread_sleep),
	AMX_DECLARE_NATIVE(thread_id),
	AMX_DECLARE_NATIVE(thread_id_s),
};

int RegisterThreadNatives(AMX *amx)
{
	return amx_Register(amx, native_list, sizeof(native_list) / sizeof(*native_list));
}
