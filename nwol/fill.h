#include "evaluation.h"
#include "nwol_coord.h"

#ifndef __FILL_H__9283492364__
#define __FILL_H__9283492364__

namespace nwol
{
	template<typename _T, size_t _Size>	static inline	_T*			fillArray				(_T (&targets)[_Size], _T value)																						{
		for( uint32_t i=0; i < _Size; i++ ) 
			targets[i]														= value; 

		return targets;	
	}
	template<typename _T>				static inline	_T*			fillArray				(_T* targets, _T value, uint32_t count, uint32_t maxCount)																{
		error_if(count > maxCount, "%s", "Attempt to assign a count higher than the allowed size.");
		for( uint32_t i=0, actualCount = count > maxCount ? maxCount : count; i < actualCount; ++i ) 
			targets[i]														= value;	

		return targets; 
	}
	template<typename _T, size_t _Size>	static inline	_T*			fillArray				(_T (&targets)[_Size], _T value, uint32_t count)																		{
		error_if(count > _Size, "%s", "Attempt to assign a count higher than the allowed size.");
		for( uint32_t i=0, actualCount = count > _Size ? _Size : count; i < actualCount; ++i ) 
			targets[i]														= value;	

		return targets; 
	}
	template<typename _T, size_t _Size>	static inline	_T*			fillArray2D				(_T (&targets)[_Size][_Size], _T value)																					{
		for( uint32_t z=0; z < _Size; z++	)									
			for( uint32_t x=0; x < _Size; x++	)									
				targets[z][x]													= value;																

		return &targets[0][0];																		
	}
	template <typename _TCell>							void		fillRectangleBorders	(_TCell* target, int32_t targetWidth, int32_t targetHeight, _TCell color, const ::nwol::SRectangle2D<int32_t>& rect)	{
		const int32_t														ystart					= ::nwol::max(0, nwol::min(targetHeight	, rect.Offset.y				));
		const int32_t														xstart					= ::nwol::max(0, nwol::min(targetWidth	, rect.Offset.x				));
		const int32_t														ystop					= ::nwol::max(0, nwol::min(targetHeight	, rect.Offset.y+rect.Size.y	));
		const int32_t														xstop					= ::nwol::max(0, nwol::min(targetWidth	, rect.Offset.x+rect.Size.x	));

		const int32_t														ylastIndex				= rect.Offset.y+rect.Size.y-1;
		const int32_t														xlastIndex				= rect.Offset.x+rect.Size.x-1;
		if(in_range(rect.Offset.x	, 0, targetWidth	)) { for(int32_t y=rect.Offset.y, ycount=rect.Offset.y+rect.Size.y; y<ycount; ++y) { if(y >= ystop) break; else if(y < ystart) continue; int32_t linearIndex = y*targetWidth+rect.Offset.x;	target[linearIndex] = color; } }
		if(in_range(rect.Offset.y	, 0, targetHeight	)) { for(int32_t x=rect.Offset.x, xcount=rect.Offset.x+rect.Size.x; x<xcount; ++x) { if(x >= xstop) break; else if(x < xstart) continue; int32_t linearIndex = rect.Offset.y*targetWidth+x;	target[linearIndex] = color; } }
		if(in_range(ylastIndex		, 0, targetHeight	)) { for(int32_t x=rect.Offset.x, xcount=rect.Offset.x+rect.Size.x; x<xcount; ++x) { if(x >= xstop) break; else if(x < xstart) continue; int32_t linearIndex = (ylastIndex)*targetWidth+x;	target[linearIndex] = color; } }
		if(in_range(xlastIndex		, 0, targetWidth	)) { for(int32_t y=rect.Offset.y, ycount=rect.Offset.y+rect.Size.y; y<ycount; ++y) { if(y >= ystop) break; else if(y < ystart) continue; int32_t linearIndex = y*targetWidth+(xlastIndex);	target[linearIndex] = color; } }
	}
	template <typename _TCell>							void		fillCircle				(_TCell* target, int32_t targetWidth, int32_t targetHeight, _TCell color, const ::nwol::SCircle2D<int32_t>& circle)		{
		const int32_t														ystart					= (int32_t)::nwol::max(0.0, nwol::min((double)targetHeight	, circle.Center.y - circle.Radius));
		const int32_t														xstart					= (int32_t)::nwol::max(0.0, nwol::min((double)targetWidth	, circle.Center.x - circle.Radius));
		const int32_t														ystop					= (int32_t)::nwol::max(0.0, nwol::min((double)targetHeight	, circle.Center.y + circle.Radius));
		const int32_t														xstop					= (int32_t)::nwol::max(0.0, nwol::min((double)targetWidth	, circle.Center.x + circle.Radius));

		const double														squaredRadius			= circle.Radius*circle.Radius;
		for(int32_t y=ystart; y<ystop; ++y)
			for(int32_t x=xstart; x<xstop; ++x) {	
				::nwol::SCoord2<int32_t>											cellPos					= {x, y};
				double																distanceFromCenter		= (circle.Center - cellPos).LengthSquared();
				if(distanceFromCenter > squaredRadius)
					continue;

				int32_t																linearIndex				= y*targetWidth+x;
				target[linearIndex]										= color;
			}
	}
	template <typename _TCell>							void		fillRectangle			(_TCell* target, int32_t targetWidth, int32_t targetHeight, int32_t targetPitch, _TCell color, const ::nwol::SRectangle2D<int32_t>& rect)	{
		const int32_t														ystart					= ::nwol::max(0, nwol::min(targetHeight	, rect.Offset.y				));
		const int32_t														xstart					= ::nwol::max(0, nwol::min(targetWidth	, rect.Offset.x				));
		const int32_t														ystop					= ::nwol::max(0, nwol::min(targetHeight	, rect.Offset.y+rect.Size.y	));
		const int32_t														xstop					= ::nwol::max(0, nwol::min(targetWidth	, rect.Offset.x+rect.Size.x	));

		for(int32_t y=ystart; y<ystop; ++y)
			for(int32_t x=xstart; x<xstop; ++x) {	
				const int32_t														linearIndex				= y*targetPitch+x;
				target[linearIndex]												= color;
			}
	}
	template <typename _TCell>			static inline	void		fillRectangle			(_TCell* target, int32_t targetWidth, int32_t targetHeight, int32_t targetPitch, _TCell color, const ::nwol::SRectangle2D<int32_t>& rect, ::nwol::ALIGN_SCREEN align)	{
		if(align == (::nwol::SCREEN_TOP_LEFT))
			fillRectangle(target, targetWidth, targetHeight, targetPitch, color, rect);
		else {
			::nwol::SRectangle2D<int32_t>									rectAligned				= rect;
			realignRectangle(targetWidth, targetHeight, rect, rectAligned, align);
			fillRectangle(target, targetWidth, targetHeight, targetPitch, color, rectAligned);
		}
	}
	template <typename _TCell>			static inline	void		fillRectangle			(_TCell* target, int32_t targetWidth, int32_t targetHeight, _TCell color, const ::nwol::SRectangle2D<int32_t>& rect) {
		fillRectangle(target, targetWidth, targetHeight, targetWidth, color, rect);
	}
	template <typename _TCell>			static inline	void		fillRectangle			(_TCell* target, int32_t targetWidth, int32_t targetHeight, _TCell color, const ::nwol::SRectangle2D<int32_t>& rect, ::nwol::ALIGN_SCREEN align) {
		fillRectangle(target, targetWidth, targetHeight, targetWidth, color, rect, align);
	}
}

#endif // __FILL_H__9283492364__
