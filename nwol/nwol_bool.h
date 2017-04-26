#include "nwol_size.h"
#ifndef __NWOL_BOOL_H__20987340928374092837__
#define __NWOL_BOOL_H__20987340928374092837__

namespace nwol
{
	template <typename... _Args>	bool	any						(_Args...	args)		{ const bool checks[] = { args..., false}; for(uint32_t i=0; i < ::nwol::size(checks); ++i) if(checks[i]) return true; return false; }
	template <typename... _Args>	bool	all						(_Args...	args)		{ const bool checks[] = {!args..., false}; for(uint32_t i=0; i < ::nwol::size(checks); ++i) if(checks[i]) return false; return true; }
	template <typename... _Args>	bool	equal					(const _Args&&... args)	{ 
		const bool									checks[]				= {(args.first != args.second)..., true}; 
		for(uint32_t i=0; i < ::nwol::size(checks); ++i) 
			if(checks[i]) 
				return false; 

		return true; 
	}
}

#endif // __NWOL_BOOL_H__20987340928374092837__
