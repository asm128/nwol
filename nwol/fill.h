#include "evaluation.h"
#include "nwol_coord.h"

#ifndef __FILL_H__9283492364__
#define __FILL_H__9283492364__

namespace nwol
{
	template<typename _tValue, size_t _arraySize>	
	static inline								_tValue*	fillArray				(_tValue (&targets)[_arraySize], _tValue value)																			{
		for( uint32_t i=0; i < _arraySize; i++ ) 
			targets[i]												= value; 

		return targets;	
	}
	template<typename _tValue>						
	static inline								_tValue*	fillArray				(_tValue* targets, _tValue value, uint32_t count, uint32_t maxCount)													{
		error_if(count > maxCount, "%s", "Attempt to assign a count higher than the allowed size.");
		for( uint32_t i=0, actualCount = count > maxCount ? maxCount : count; i < actualCount; ++i ) 
			targets[i]												= value;	

		return targets; 
	}
	template<typename _tValue, size_t _arraySize>	
	static inline								_tValue*	fillArray				(_tValue (&targets)[_arraySize], _tValue value, uint32_t count)															{
		error_if(count > _arraySize, "%s", "Attempt to assign a count higher than the allowed size.");
		for( uint32_t i=0, actualCount = count > _arraySize ? _arraySize : count; i < actualCount; ++i ) 
			targets[i]												= value;	

		return targets; 
	}
	template<typename _tValue, size_t _arrayWidth, size_t _arrayHeight>	
	static inline										_tValue*	fillArray2D				(_tValue (&targets)[_arrayHeight][_arrayWidth], _tValue value)													{
		for( uint32_t z=0; z < _arrayHeight; z++	)									
			for( uint32_t x=0; x < _arrayWidth; x++	)									
				targets[z][x]											= value;																

		return &targets[0][0];																		
	}
	template <typename _tCell>					void		fillRectangleBorders	(_tCell* target, int32_t targetWidth, int32_t targetHeight, _tCell color, const ::nwol::SRectangle2D<int32_t>& rect)	{
		const int32_t												ystart					= ::nwol::max(0, nwol::min(targetHeight	, rect.Offset.y					));
		const int32_t												xstart					= ::nwol::max(0, nwol::min(targetWidth	, rect.Offset.x					));
		const int32_t												ystop					= ::nwol::max(0, nwol::min(targetHeight	, rect.Offset.y + rect.Size.y	));
		const int32_t												xstop					= ::nwol::max(0, nwol::min(targetWidth	, rect.Offset.x + rect.Size.x	));

		const int32_t												ylastIndex				= rect.Offset.y + rect.Size.y - 1;
		const int32_t												xlastIndex				= rect.Offset.x + rect.Size.x - 1;
		if(in_range(rect.Offset.x	, 0, targetWidth	)) { for(int32_t y = rect.Offset.y, ycount = rect.Offset.y + rect.Size.y; y < ycount; ++y) { if(y >= ystop) break; else if(y >= ystart) { const int32_t linearIndex = y * targetWidth + rect.Offset.x	; target[linearIndex] = color; } } }
		if(in_range(rect.Offset.y	, 0, targetHeight	)) { for(int32_t x = rect.Offset.x, xcount = rect.Offset.x + rect.Size.x; x < xcount; ++x) { if(x >= xstop) break; else if(x >= xstart) { const int32_t linearIndex = rect.Offset.y * targetWidth + x	; target[linearIndex] = color; } } }
		if(in_range(ylastIndex		, 0, targetHeight	)) { for(int32_t x = rect.Offset.x, xcount = rect.Offset.x + rect.Size.x; x < xcount; ++x) { if(x >= xstop) break; else if(x >= xstart) { const int32_t linearIndex = ylastIndex * targetWidth + x		; target[linearIndex] = color; } } }
		if(in_range(xlastIndex		, 0, targetWidth	)) { for(int32_t y = rect.Offset.y, ycount = rect.Offset.y + rect.Size.y; y < ycount; ++y) { if(y >= ystop) break; else if(y >= ystart) { const int32_t linearIndex = y * targetWidth + xlastIndex		; target[linearIndex] = color; } } }
	}
	template <typename _tCell>					void		fillCircle				(_tCell* target, int32_t targetWidth, int32_t targetHeight, _tCell color, const ::nwol::SCircle2D<int32_t>& circle)		{
		const int32_t												ystart					= (int32_t)::nwol::max(0.0, nwol::min((double)targetHeight	, circle.Center.y - circle.Radius));
		const int32_t												xstart					= (int32_t)::nwol::max(0.0, nwol::min((double)targetWidth	, circle.Center.x - circle.Radius));
		const int32_t												ystop					= (int32_t)::nwol::max(0.0, nwol::min((double)targetHeight	, circle.Center.y + circle.Radius));
		const int32_t												xstop					= (int32_t)::nwol::max(0.0, nwol::min((double)targetWidth	, circle.Center.x + circle.Radius));

		const double												squaredRadius			= circle.Radius*circle.Radius;
		for(int32_t y=ystart; y<ystop; ++y)
			for(int32_t x=xstart; x<xstop; ++x) {					
				::nwol::SCoord2<int32_t>									cellPos					= {x, y};
				double														distanceFromCenter		= (circle.Center - cellPos).LengthSquared();
				if(distanceFromCenter > squaredRadius)
					continue;

				int32_t														linearIndex				= y*targetWidth+x;
				target[linearIndex]										= color;
			}
	}
	template <typename _tCell>					void		fillRectangle			(_tCell* target, int32_t targetWidth, int32_t targetHeight, int32_t targetPitch, _tCell color, const ::nwol::SRectangle2D<int32_t>& rect)	{
		const int32_t												ystart					= ::nwol::max(0, nwol::min(targetHeight	, rect.Offset.y				));
		const int32_t												xstart					= ::nwol::max(0, nwol::min(targetWidth	, rect.Offset.x				));
		const int32_t												ystop					= ::nwol::max(0, nwol::min(targetHeight	, rect.Offset.y+rect.Size.y	));
		const int32_t												xstop					= ::nwol::max(0, nwol::min(targetWidth	, rect.Offset.x+rect.Size.x	));

		for(int32_t y=ystart; y<ystop; ++y)
			for(int32_t x=xstart; x<xstop; ++x) {					
				const int32_t												linearIndex				= y*targetPitch+x;
				target[linearIndex]										= color;
			}
	}
	template <typename _tCell>	
	static inline								void		fillRectangle			(_tCell* target, int32_t targetWidth, int32_t targetHeight, int32_t targetPitch, _tCell color, const ::nwol::SRectangle2D<int32_t>& rect, ::nwol::ALIGN_SCREEN align)	{
		if(align == (::nwol::SCREEN_tValueOP_LEFT))
			fillRectangle(target, targetWidth, targetHeight, targetPitch, color, rect);
		else {
			::nwol::SRectangle2D<int32_t>							rectAligned				= rect;
			realignRectangle(targetWidth, targetHeight, rect, rectAligned, align);
			fillRectangle(target, targetWidth, targetHeight, targetPitch, color, rectAligned);
		}
	}
	template <typename _tCell>	static inline	void		fillRectangle			(_tCell* target, int32_t targetWidth, int32_t targetHeight, _tCell color, const ::nwol::SRectangle2D<int32_t>& rect)								{ fillRectangle(target, targetWidth, targetHeight, targetWidth, color, rect);			}
	template <typename _tCell>	static inline	void		fillRectangle			(_tCell* target, int32_t targetWidth, int32_t targetHeight, _tCell color, const ::nwol::SRectangle2D<int32_t>& rect, ::nwol::ALIGN_SCREEN align)	{ fillRectangle(target, targetWidth, targetHeight, targetWidth, color, rect, align);	}
}

#endif // __FILL_H__9283492364__
