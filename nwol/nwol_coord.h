/// Copyright 2009-2017 - asm128
#include "nwol_bit.h"
#include "nwol_math.h"

#ifndef __COORD_H__283409263409234698374__
#define __COORD_H__283409263409234698374__

namespace nwol
{
#pragma pack(push, 1)
	template<typename _tBase>
	struct SCoord2 {
		typedef			SCoord2<_tBase>	TCoord2;
							_tBase		x, y;
		//
		inline constexpr	TCoord2		operator+				(const TCoord2& other)										const	noexcept	{ return {x + other.x, y + other.y};																					}
		inline constexpr	TCoord2		operator-				(const TCoord2& other)										const	noexcept	{ return {x - other.x, y - other.y};																					}
		inline constexpr	TCoord2		operator*				(double scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};																	}
		inline constexpr	TCoord2		operator/				(double scalar)												const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};																	}
		inline constexpr	TCoord2		operator*				(int64_t scalar)											const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};																	}
		inline constexpr	TCoord2		operator/				(int64_t scalar)											const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};																	}
		inline constexpr	TCoord2		operator*				(uint64_t scalar)											const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar)};																	}
		inline constexpr	TCoord2		operator/				(uint64_t scalar)											const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar)};																	}
		//
		inline				TCoord2&	operator+=				(const TCoord2& other)												noexcept	{ x += other.x; y += other.y;							return *this;													}
		inline				TCoord2&	operator-=				(const TCoord2& other)												noexcept	{ x -= other.x; y -= other.y;							return *this;													}
		inline				TCoord2&	operator*=				(double scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;													}
		inline				TCoord2&	operator/=				(double scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;													}
		inline				TCoord2&	operator*=				(int64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;													}
		inline				TCoord2&	operator/=				(int64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;													}
		inline				TCoord2&	operator*=				(uint64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar);	return *this;													}
		inline				TCoord2&	operator/=				(uint64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar);	return *this;													}
		//
		inline constexpr	bool		operator==				(const TCoord2& other)										const	noexcept	{ return x == other.x && y == other.y;																					}
		inline constexpr	bool		operator!=				(const TCoord2& other)										const	noexcept	{ return !operator==(other);																							}
		inline constexpr	TCoord2		operator-				()															const	noexcept	{ return {x*-1, y*-1};																									}
		//
		template<typename _t>
		constexpr inline	SCoord2<_t>	Cast					()															const	noexcept	{ return {(_t)x, (_t)y};																								}
		inline				TCoord2&	Scale					(double scalar)														noexcept	{ return *this *= scalar;																								}
		inline				TCoord2&	Normalize				()																				{ _tBase sqLen = LengthSquared(); return (sqLen) ? *this /= ::ftwlib::sqrt_safe(sqLen) : *this;							}
		inline constexpr	double		Dot						(const TCoord2& other)										const	noexcept	{ return x * other.x + y * other.y;																						}
		inline constexpr	_tBase		LengthSquared			()															const	noexcept	{ return x * x + y * y;																									}
		inline constexpr	double		Length					()															const				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? ::sqrt(sqLen) : 0;												}
		inline				double		AngleWith				(const TCoord2& other)										const				{ const double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos( Dot(other) / lengthsProduct ) : 0;	}
		inline				void		AddScaled				(const TCoord2& vectorToScaleAndAdd, float scale)								{
			x								+= vectorToScaleAndAdd.x*scale;
			y								+= vectorToScaleAndAdd.y*scale;
		}
							TCoord2&	Rotate					(double theta)																	{
			const SPairSinCos					pairSinCos				= ::nwol::getSinCos(theta);
			const double						px						= x * pairSinCos.Cos - y * pairSinCos.Sin; 

			y								= (_tBase)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x								= (_tBase)px;
			return *this;
		}
	};	// struct SCoord2

	template<typename _tBase>
	struct SCoord3 {
		typedef			SCoord3<_tBase>	TCoord3;
							_tBase		x, y, z;
		//
		constexpr			TCoord3		operator+				(const TCoord3& other)										const	noexcept	{ return {x + other.x, y + other.y, z + other.z};												}
		constexpr			TCoord3		operator-				(const TCoord3& other)										const	noexcept	{ return {x - other.x, y - other.y, z - other.z};												}
		constexpr			TCoord3		operator*				(double scalar)												const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar)};					}
		constexpr			TCoord3		operator/				(double scalar)												const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar)};					}
		constexpr			TCoord3		operator*				(int64_t scalar)											const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar)};					}
		constexpr			TCoord3		operator/				(int64_t scalar)											const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar)};					}
		constexpr			TCoord3		operator*				(uint64_t scalar)											const	noexcept	{ return {(_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar)};					}
		constexpr			TCoord3		operator/				(uint64_t scalar)											const				{ return {(_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar)};					}
		//
							TCoord3&	operator+=				(const TCoord3& other)												noexcept	{ x += other.x; y += other.y; z += other.z; return *this;										}
							TCoord3&	operator-=				(const TCoord3& other)												noexcept	{ x -= other.x; y -= other.y; z -= other.z; return *this;										}
							TCoord3&	operator*=				(double scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); return *this;	}
							TCoord3&	operator/=				(double scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); return *this;	}
							TCoord3&	operator*=				(int64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); return *this;	}
							TCoord3&	operator/=				(int64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); return *this;	}
							TCoord3&	operator*=				(uint64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); return *this;	}
							TCoord3&	operator/=				(uint64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); return *this;	}
		//
		constexpr inline	bool		operator==				(const TCoord3& other)										const	noexcept	{ return x == other.x && y == other.y && z == other.z;											}
		constexpr inline	bool		operator!=				(const TCoord3& other)										const	noexcept	{ return !operator==(other);																	}
		constexpr inline	TCoord3		operator-				()															const	noexcept	{ return {x*-1, y*-1, z*-1};																	}
		template<typename _t>
		constexpr inline	SCoord3<_t>	Cast					()															const	noexcept	{ return SCoord3<_t>({(_t)x, (_t)y, (_t)z});													}
		inline				TCoord3&	Normalize				()																				{ _tBase sqLen = LengthSquared(); return (sqLen) ? *this /= ::nwol::sqrt_safe(sqLen) : *this;	}
		constexpr			double		Dot						(const TCoord3& other)										const	noexcept	{ return x * other.x + y * other.y + z * other.z;												}
		constexpr inline	_tBase		LengthSquared			()															const	noexcept	{ return x * x + y * y + z * z;																	}
		inline constexpr	double		Length					()															const				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? ::sqrt(sqLen) : 0;						}
		inline				double		AngleWith				(const TCoord3& other)										const				{ const double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos( Dot(other) / lengthsProduct ) : 0;	}
							void		AddScaled				(const TCoord3& vectorToScaleAndAdd, double scale)					noexcept	{
			x								+= (_tBase)(vectorToScaleAndAdd.x*scale);
			y								+= (_tBase)(vectorToScaleAndAdd.y*scale);
			z								+= (_tBase)(vectorToScaleAndAdd.z*scale);
		}
							void		SubstractScaled			(const TCoord3& vectorToScaleAndSubstract, double scale)			noexcept	{
			x								-= (_tBase)(vectorToScaleAndSubstract.x*scale);
			y								-= (_tBase)(vectorToScaleAndSubstract.y*scale);
			z								-= (_tBase)(vectorToScaleAndSubstract.z*scale);
		}
		constexpr			TCoord3		Cross					(const TCoord3& right)										const	noexcept	{ return {y * right.z - z * right.y, z * right.x - x * right.z, x * right.y - y * right.x };	}
							TCoord3&	Cross					(const TCoord3& vector1, const TCoord3& vector2)					noexcept	{ 
			x								= vector1.y * vector2.z - vector1.z * vector2.y;
			y								= vector1.z * vector2.x - vector1.x * vector2.z;
			z								= vector1.x * vector2.y - vector1.y * vector2.x;
			return *this;
		}
							TCoord3&	RotateX					(double theta)																	{
			const SPairSinCos					pairSinCos				= ::nwol::getSinCos(theta);
			const double						pz						= y * pairSinCos.Cos - z * pairSinCos.Sin; 
			y								= (_tBase)(y * pairSinCos.Sin + z * pairSinCos.Cos);
			z								= (_tBase)pz;

			return *this;
		}
							TCoord3&	RotateY					(double theta)																	{
			const SPairSinCos					pairSinCos				= ::nwol::getSinCos(theta);
			const double						px						= x * pairSinCos.Cos - z * pairSinCos.Sin; 
			z								= (_tBase)(x * pairSinCos.Sin + z * pairSinCos.Cos);
			x								= (_tBase)px;	

			return *this;
		}
							TCoord3&	RotateZ					(double theta)																	{
			const SPairSinCos					pairSinCos				= ::nwol::getSinCos(theta);
			const double						px						= x * pairSinCos.Cos - y * pairSinCos.Sin; 
			y								= (_tBase)(x * pairSinCos.Sin + y * pairSinCos.Cos);
			x								= (_tBase)px;

			return *this;
		}
	};	// struct SCoord3

	template<typename _tBase>
	struct SCoord4 {
		typedef			SCoord4<_tBase>	TCoord4;
							_tBase		x, y, z, w;
		//
		constexpr			TCoord4		operator +				(const TCoord4& other)										const	noexcept	{ return { x + other.x, y + other.y, z + other.z, w + other.w }; }
		constexpr			TCoord4		operator -				(const TCoord4& other)										const	noexcept	{ return { x - other.x, y - other.y, z - other.z, w - other.w }; }
		constexpr			TCoord4		operator *				(double scalar)												const	noexcept	{ return { (_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar), (_tBase)(w * scalar) }; }
		constexpr			TCoord4		operator /				(double scalar)												const				{ return { (_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar), (_tBase)(w / scalar) }; }
		constexpr			TCoord4		operator *				(int64_t scalar)											const	noexcept	{ return { (_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar), (_tBase)(w * scalar) }; }
		constexpr			TCoord4		operator /				(int64_t scalar)											const				{ return { (_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar), (_tBase)(w / scalar) }; }
		constexpr			TCoord4		operator *				(uint64_t scalar)											const	noexcept	{ return { (_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar), (_tBase)(w * scalar) }; }
		constexpr			TCoord4		operator /				(uint64_t scalar)											const				{ return { (_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar), (_tBase)(w / scalar) }; }
		//										 
							TCoord4&	operator +=				(const TCoord4& other)												noexcept	{ x += other.x; y += other.y; z += other.z; w += other.w; return *this; }
							TCoord4&	operator -=				(const TCoord4& other)												noexcept	{ x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this; }
							TCoord4&	operator *=				(double scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); w = (_tBase)(w * scalar); return *this; }
							TCoord4&	operator /=				(double scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); w = (_tBase)(w / scalar); return *this; }
							TCoord4&	operator *=				(int64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); w = (_tBase)(w * scalar); return *this; }
							TCoord4&	operator /=				(int64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); w = (_tBase)(w / scalar); return *this; }
							TCoord4&	operator *=				(uint64_t scalar)													noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); w = (_tBase)(w * scalar); return *this; }
							TCoord4&	operator /=				(uint64_t scalar)																{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); w = (_tBase)(w / scalar); return *this; }
		//										 
		constexpr			bool		operator ==				(const TCoord4& other)										const	noexcept	{ return x == other.x && y == other.y && z == other.z && w == other.w;								}
		constexpr inline	bool		operator !=				(const TCoord4& other)										const	noexcept	{ return !operator==(other);																		}
		//										 
		constexpr inline	TCoord4		operator -				()															const	noexcept	{ return {x*-1, y*-1, z*-1, w*-1};																	}
		//
		template<typename _t>
		constexpr inline	SCoord4<_t>	Cast					()															const	noexcept	{ return SCoord4<_t>({(_t)x, (_t)y, (_t)z, (_t)w});													}
		inline				TCoord4&	Normalize				()																				{ _tBase sqLen = LengthSquared(); return (sqLen) ? *this /= ::ftwlib::sqrt_safe(sqLen) : *this;		}
		constexpr			double		Dot						(const TCoord4& other)										const	noexcept	{ return x*other.x + y*other.y + z*other.z + w*other.w;												}
		constexpr inline	_tBase		LengthSquared			()															const	noexcept	{ return x * x + y * y + z * z + w * w;																}
		inline constexpr	double		Length					()															const				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? ::sqrt(sqLen) : 0;							}
		inline				double		AngleWith				(const TCoord4& other)										const				{ const double lengthsProduct = Length() * other.Length(); return lengthsProduct ? ::acos( Dot(other) / lengthsProduct ) : 0;	}
							void		AddScaled				(const TCoord4& vectorToScaleAndAdd, float scale)					noexcept	{
			x								+= vectorToScaleAndAdd.x*scale;
			y								+= vectorToScaleAndAdd.y*scale;
			z								+= vectorToScaleAndAdd.z*scale;
			w								+= vectorToScaleAndAdd.w*scale;
		}
	}; // struct SCoord4

	template<typename _tBase>
	struct SQuat {
		typedef			SQuat  <_tBase>	TQuat;
		typedef			SCoord4<_tBase>	TCoord4;
		typedef			SCoord3<_tBase>	TCoord3;
							_tBase		x, y, z, w;
		// comparison operator
		constexpr			bool		operator ==				(const TQuat& other)										const	noexcept	{ return x == other.x && y == other.y && z == other.z && w == other.w; }
		constexpr inline	bool		operator !=				(const TQuat& other)										const	noexcept	{ return !operator==(other); }
		//
		constexpr			TQuat		operator +				(const TQuat& other)										const	noexcept	{ return { x + other.x, y + other.y, z + other.z, w + other.w }; }
		constexpr			TQuat		operator -				(const TQuat& other)										const	noexcept	{ return { x - other.x, y - other.y, z - other.z, w - other.w }; }
		constexpr			TQuat		operator *				(double scalar)												const	noexcept	{ return { (_tBase)(x * scalar), (_tBase)(y * scalar), (_tBase)(z * scalar), (_tBase)(w * scalar) }; }
		constexpr			TQuat		operator /				(double scalar)												const				{ return { (_tBase)(x / scalar), (_tBase)(y / scalar), (_tBase)(z / scalar), (_tBase)(w / scalar) }; }

							TQuat		operator *				(const TQuat& q)											const	noexcept	{
								TQuat			r;
								r.x			= w*q.x + x*q.w + y*q.z - z*q.y;
								r.y			= w*q.y + y*q.w + z*q.x - x*q.z;
								r.z			= w*q.z + z*q.w + x*q.y - y*q.x;
								r.w			= w*q.w - x*q.x - y*q.y - z*q.z;
		
								return r;
							}
							TQuat		operator *				(const TCoord3& v)											const	noexcept	{
								return 
									{	  w*v.x + y*v.z - z*v.y
									,	  w*v.y + z*v.x - x*v.z
									,	  w*v.z + x*v.y - y*v.x
									,	-(x*v.x + y*v.y + z*v.z) 
									};
							}
							TQuat&		operator+=				(const TQuat& other)												noexcept	{ x += other.x; y += other.y; z += other.z; w += other.w; return *this;													}
							TQuat&		operator-=				(const TQuat& other)												noexcept	{ x -= other.x; y -= other.y; z -= other.z; w -= other.w; return *this;													}
							TQuat&		operator*=				(double scalar)														noexcept	{ x = (_tBase)(x * scalar); y = (_tBase)(y * scalar); z = (_tBase)(z * scalar); w = (_tBase)(w * scalar); return *this;	}
							TQuat&		operator/=				(double scalar)																	{ x = (_tBase)(x / scalar); y = (_tBase)(y / scalar); z = (_tBase)(z / scalar); w = (_tBase)(w / scalar); return *this;	}
		inline				TQuat&		operator*=				(const TQuat& q)													noexcept	{ return *this = operator*(q); }
		// Unary operators
		constexpr inline	TQuat		operator-				()															const	noexcept	{ return {x*-1, y*-1, z*-1, w*-1};																						}
		constexpr inline	TQuat		operator~				()															const	noexcept	{ return {-x, -y, -z, w};																								}
		template<typename _t>
		constexpr inline	SQuat<_t>	Cast					()															const	noexcept	{ return {(_t)x, (_t)y, (_t)z, (_t)w};																					}
		inline				void		Identity				()																	noexcept	{ x = y = z = 0.0f; w = 1.0f;																							}
		inline				TQuat&		Normalize				()																	noexcept	{ _tBase sqLen = LengthSquared(); if(sqLen ) return *this /= ::nwol::sqrt_safe(sqLen); Identity(); return *this;		}
		constexpr			double		Dot						(const TQuat& other)										const	noexcept	{ return x*other.x + y*other.y + z*other.z + w*other.w;																	}
		constexpr inline	_tBase		LengthSquared			()															const	noexcept	{ return x * x + y * y + z * z + w * w;																					}
		inline constexpr	double		Length					()															const				{ const _tBase sqLen = LengthSquared(); return (sqLen) ? ::sqrt(sqLen) : 0;												}
		inline				TQuat&		LinearInterpolate		(const TQuat &p, const TQuat &q, double fTime)						noexcept	{ return *this = ((q-p)*fTime)+p;																						}
							void		AddScaled				(const TCoord4& vector, double scale)								noexcept	{ TQuat q = {(_tBase)(vector.x * scale), (_tBase)(vector.y * scale), (_tBase)(vector.z * scale), (_tBase)0}; q *= *this; w += (_tBase)(q.w * 0.5); x += (_tBase)(q.x * 0.5); y += (_tBase)(q.y * 0.5); z += (_tBase)(q.z * 0.5); }
							void		AddScaled				(const TCoord3& vector, double scale)								noexcept	{ TQuat q = {(_tBase)(vector.x * scale), (_tBase)(vector.y * scale), (_tBase)(vector.z * scale), (_tBase)0}; q *= *this; w += (_tBase)(q.w * 0.5); x += (_tBase)(q.x * 0.5); y += (_tBase)(q.y * 0.5); z += (_tBase)(q.z * 0.5); }
		inline				void		SetRotation				(const TQuat& q1, const TQuat& q2)									noexcept	{ return *this = q1 * q2 * ~q1;																							}
							TCoord3		RotateVector			(const TCoord3 &v)											const	noexcept	{ 
			const TQuat							t						= {x, y, z, w};
			const TQuat							r						= t * v * (~t);
			return {r.x, r.y, r.z}; 
		}
							TQuat&		SLERP					(const TQuat& p, const TQuat& q, double fTime)									{
			//Calculate the dot product 
			double								fDot					= Dot(q);
		
			TQuat								q_;
			if(fDot < 0.0f) { 
				q_								= -q; 
				fDot							= -fDot; 
			} 
 		
			if(fDot < 1.00001f && fDot > 0.99999f) 
				return ::nwol::interpolate_linear(p, q_, fTime); 
 		
			//calculate the angle between the quaternions  
			const	double						fTheta					= ::acos(fDot); 
			return *this					= (p * ::sin(fTheta * (1 - fTime)) + q_ * ::sin(fTheta * fTime)) / ::sin(fTheta); 
		}
		// Convert from Euler Angles	
		inline				TQuat&		MakeFromEulerTaitBryan	(const TCoord3& v)																{ return MakeFromEulerTaitBryan(v.x, v.y, v.z);																			}
							TQuat&		MakeFromEulerTaitBryan	(double fPitch, double fYaw, double fRoll)										{
			 //Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll and multiply those together. the calculation below does the same, just shorter.
			//float p = fPitch * GMATH_PI_180 * .5;
			//float y = fYaw * GMATH_PI_180 * .5;
			//float r = fRoll * GMATH_PI_180 * .5;
			//float sinp = sin(p);
			//float siny = sin(y);
			//float sinr = sin(r);
			//float cosp = cos(p);
			//float cosy = cos(y);
			//float cosr = cos(r);
			//this->w = cosr * cosp * cosy + sinr * sinp * siny;
			//this->x = sinr * cosp * cosy - cosr * sinp * siny;
			//this->y = cosr * sinp * cosy + sinr * cosp * siny;
			//this->z = cosr * cosp * siny - sinr * sinp * cosy;
		
			fPitch							*= 0.5f;//
			fYaw							*= 0.5f;//
			fRoll							*= 0.5f;//

			const SPairSinCos					pairSinCosX				= ::nwol::getSinCos(fPitch	);
			const SPairSinCos					pairSinCosY				= ::nwol::getSinCos(fYaw	);
			const SPairSinCos					pairSinCosZ				= ::nwol::getSinCos(fRoll	);
		
			double								cYcZ					= pairSinCosY.Cos * pairSinCosZ.Cos;
			double								sYsZ					= pairSinCosY.Sin * pairSinCosZ.Sin;
			double								cYsZ					= pairSinCosY.Cos * pairSinCosZ.Sin;
			double								sYcZ					= pairSinCosY.Sin * pairSinCosZ.Cos;
		
			w								= (_tBase)(pairSinCosX.Cos * cYcZ + pairSinCosX.Sin * sYsZ);
			x								= (_tBase)(pairSinCosX.Sin * cYcZ - pairSinCosX.Cos * sYsZ);
			y								= (_tBase)(pairSinCosX.Cos * sYcZ + pairSinCosX.Sin * cYsZ);
			z								= (_tBase)(pairSinCosX.Cos * cYsZ - pairSinCosX.Sin * sYcZ);
		
			return Normalize();
		}
							void		GetEulersTaitBryan		(double* fPitch, double* fYaw, double* fRoll)									{
			double								r11, r21, r31, r32, r33, r12, r13;
			double								q00, q11, q22, q33;
			double								tmp;
		
			q00								= w * w;
			q11								= x * x;
			q22								= y * y;
			q33								= z * z;
		
			r11								= q00 + q11 - q22 - q33;
			r21								= 2 * (x*y + w*z);
			r31								= 2 * (x*z - w*y);
			r32								= 2 * (y*z + w*x);
			r33								= q00 - q11 - q22 + q33;
		
#if defined( __ANDROID__ ) || defined( __LINUX__ )
			tmp								= ::std::abs(r31);
#else
			tmp								= ::abs(r31);
#endif
			if(tmp > 0.999999) {
				r12								= 2 * (x*y - w*z);
				r13								= 2 * (x*z + w*y);
				*fPitch							= 0.0f;
				*fYaw							= -((::nwol::math_pi2) * r31/tmp);
				*fRoll							= ::atan2(-r12, -r31*r13);
			}
			else {
				*fPitch							 = ::atan2(r32, r33);
				*fYaw							 = ::asin(-r31);
				*fRoll							 = ::atan2(r21, r11);
			}
		} //
	}; // struct SQuaternion

	// Geometric figures and other coord-related POD structs.
	template<typename _tCoord> struct	SRange			{	_tCoord Offset, Count;							/**/ inline constexpr bool operator !=(const	SRange			<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SRange			<_tCoord>	& other) const noexcept { return Offset		== other.Offset		&& Count		== other.Count;							} };
	template<typename _tCoord> struct	SLine2D			{	SCoord2<_tCoord> A, B;							/**/ inline constexpr bool operator !=(const	SLine2D			<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SLine2D			<_tCoord>	& other) const noexcept { return A			== other.A			&& B			== other.B;								} };
	template<typename _tCoord> struct	SLine3D			{	SCoord3<_tCoord> A, B;							/**/ inline constexpr bool operator !=(const	SLine3D			<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SLine3D			<_tCoord>	& other) const noexcept { return A			== other.A			&& B			== other.B;								} };
	template<typename _tCoord> struct	STriangle2D		{	SCoord2<_tCoord> A, B, C;						/**/ inline constexpr bool operator !=(const	STriangle2D		<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	STriangle2D		<_tCoord>	& other) const noexcept { return A			== other.A			&& B			== other.B			&& C == other.C;	} };
	template<typename _tCoord> struct	STriangle3D		{	SCoord3<_tCoord> A, B, C;						/**/ inline constexpr bool operator !=(const	STriangle3D		<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	STriangle3D		<_tCoord>	& other) const noexcept { return A			== other.A			&& B			== other.B			&& C == other.C;	} };
	template<typename _tCoord> struct	SRectangle2D	{	SCoord2<_tCoord> Offset, Size;					/**/ inline constexpr bool operator !=(const	SRectangle2D	<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SRectangle2D	<_tCoord>	& other) const noexcept { return Offset		== other.Offset		&& Size			== other.Size;							} };
	template<typename _tCoord> struct	SRectangle3D	{	SCoord3<_tCoord> Offset, Size;					/**/ inline constexpr bool operator !=(const	SRectangle3D	<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SRectangle3D	<_tCoord>	& other) const noexcept { return Offset		== other.Offset		&& Size			== other.Size;							} };
	template<typename _tCoord> struct	SCircle2D		{	double Radius;		SCoord2<_tCoord> Center;	/**/ inline constexpr bool operator !=(const	SCircle2D		<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SCircle2D		<_tCoord>	& other) const noexcept { return Center		== other.Center		&& Radius		== other.Radius;						} };
	template<typename _tCoord> struct	SSphere2D		{	double Radius;		SCoord2<_tCoord> Center;	/**/ inline constexpr bool operator !=(const	SSphere2D		<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SSphere2D		<_tCoord>	& other) const noexcept { return Center		== other.Center		&& Radius		== other.Radius;						} };
	template<typename _tCoord> struct	SCircle3D		{	double Radius;		SCoord3<_tCoord> Center;	/**/ inline constexpr bool operator !=(const	SCircle3D		<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SCircle3D		<_tCoord>	& other) const noexcept { return Center		== other.Center		&& Radius		== other.Radius;						} };
	template<typename _tCoord> struct	SSphere3D		{	double Radius;		SCoord3<_tCoord> Center;	/**/ inline constexpr bool operator !=(const	SSphere3D		<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SSphere3D		<_tCoord>	& other) const noexcept { return Center		== other.Center		&& Radius		== other.Radius;						} };
	template<typename _tCoord> struct	SRay2D			{	SCoord2<_tCoord> Origin, Direction;				/**/ inline constexpr bool operator !=(const	SRay2D			<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SRay2D			<_tCoord>	& other) const noexcept { return Origin		== other.Origin		&& Direction	== other.Direction;						} };
	template<typename _tCoord> struct	SRay3D			{	SCoord3<_tCoord> Origin, Direction;				/**/ inline constexpr bool operator !=(const	SRay3D			<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SRay3D			<_tCoord>	& other) const noexcept { return Origin		== other.Origin		&& Direction	== other.Direction;						} };
	template<typename _tCoord> struct	SPlane2D		{	double Distance;	SCoord2<_tCoord> Direction;	/**/ inline constexpr bool operator !=(const	SPlane2D		<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SPlane2D		<_tCoord>	& other) const noexcept { return Direction	== other.Direction	&& Distance		== other.Distance;						} };
	template<typename _tCoord> struct	SPlane3D		{	double Distance;	SCoord3<_tCoord> Direction;	/**/ inline constexpr bool operator !=(const	SPlane3D		<_tCoord>& other ) const noexcept { return !operator==(other); } /**/ inline constexpr bool operator==(const	SPlane3D		<_tCoord>	& other) const noexcept { return Direction	== other.Direction	&& Distance		== other.Distance;						} };

	enum AXIS_3D : uint8_t
		{	XPOSITIVE				= 0x1
		,	XNEGATIVE				= 0x2
		,	YPOSITIVE				= 0x4
		,	YNEGATIVE				= 0x8
		,	ZPOSITIVE				= 0x10
		,	ZNEGATIVE				= 0x20
		};

	enum ALIGN_SCREEN : uint8_t
		{	SCREEN_RIGHT			= ((uint8_t)XPOSITIVE)
		,	SCREEN_LEFT				= ((uint8_t)XNEGATIVE)
		,	SCREEN_BOTTOM			= ((uint8_t)YPOSITIVE)
		,	SCREEN_TOP				= ((uint8_t)YNEGATIVE)
		,	SCREEN_FAR				= ((uint8_t)ZPOSITIVE)
		,	SCREEN_NEAR				= ((uint8_t)ZNEGATIVE)
		,	SCREEN_CENTER			= SCREEN_LEFT		| SCREEN_RIGHT
		,	SCREEN_VCENTER			= SCREEN_TOP		| SCREEN_BOTTOM
		,	SCREEN_ZCENTER			= SCREEN_FAR		| SCREEN_NEAR
		,	SCREEN_TOP_LEFT			= SCREEN_TOP		| SCREEN_LEFT  
		,	SCREEN_TOP_RIGHT		= SCREEN_TOP		| SCREEN_RIGHT 
		,	SCREEN_BOTTOM_LEFT		= SCREEN_BOTTOM		| SCREEN_LEFT  
		,	SCREEN_BOTTOM_RIGHT		= SCREEN_BOTTOM		| SCREEN_RIGHT 
		,	SCREEN_CENTER_TOP		= SCREEN_CENTER		| SCREEN_TOP	
		,	SCREEN_CENTER_BOTTOM	= SCREEN_CENTER		| SCREEN_BOTTOM	
		,	SCREEN_VCENTER_LEFT		= SCREEN_VCENTER	| SCREEN_LEFT		
		,	SCREEN_VCENTER_RIGHT	= SCREEN_VCENTER	| SCREEN_RIGHT	
		};

#pragma pack(pop)

	template <typename _tCoord> static inline constexpr		bool			in_range				(const	SCoord2<_tCoord>& value, const SCoord2<_tCoord>& rangeStart, const SCoord2<_tCoord>& rangeStop)		noexcept	{ return (value.x >= rangeStart.x) && (value.x < rangeStop.x) && (value.y >= rangeStart.y) && (value.y < rangeStop.y);	}
	template <typename _tCoord> static inline constexpr		bool			in_range				(const	SCoord3<_tCoord>& value, const SCoord3<_tCoord>& rangeStart, const SCoord3<_tCoord>& rangeStop)		noexcept	{ return (value.x >= rangeStart.x) && (value.x < rangeStop.x) && (value.y >= rangeStart.y) && (value.y < rangeStop.y) && (value.z >= rangeStart.z) && (value.z < rangeStop.z);	}

	template <typename _tCoord>
				void														coord2D_rotateInPlace	(SCoord2<_tCoord>& coord, const SPairSinCos& pairSinCos)													noexcept	{
		const double																px						= coord.x * pairSinCos.Cos - coord.y * pairSinCos.Sin; 
		coord.y																	= (_tCoord)(coord.x * pairSinCos.Sin + coord.y * pairSinCos.Cos);
		coord.x																	= (_tCoord)px;
		return coord;
	}

	template <typename _tCoord>
	constexpr	SCoord2<_tCoord>											coord2D_rotate			(const SCoord2<_tCoord>& coord, const SPairSinCos& pairSinCos)												noexcept	{
		return {(_tCoord)(coord.x * pairSinCos.Cos - coord.y * pairSinCos.Sin), (_tCoord)(coord.x * pairSinCos.Sin + coord.y * pairSinCos.Cos)};
	}

	template <typename _tCoord>	inline void									coord2D_rotateInPlace	(		SCoord2<_tCoord>& coord, double theta)																noexcept	{ coord2D_rotateInPlace(coord, ::nwol::getSinCos(theta)); }
	template <typename _tCoord>	inline SCoord2<_tCoord>						coord2D_rotate			(const	SCoord2<_tCoord>& coord, double theta)																noexcept	{ return coord2D_rotate(coord, ::nwol::getSinCos(theta)); }

	//------------------------------------------------------------------------------------------------------------
	template <typename _tCoord>
				SRectangle2D<_tCoord>&										realignRectangle		
					(	int32_t							targetWidth
					,	int32_t							targetHeight
					,	const SRectangle2D<_tCoord>		& rectangle
					,	SRectangle2D<_tCoord>			& targetRectangle
					,	ALIGN_SCREEN					align
					)																																					noexcept	{
		targetRectangle															= rectangle;
 			 if gbit_true(align, SCREEN_CENTER	)	{ targetRectangle.Offset.x		= (targetWidth	>> 1) - (rectangle.Size.x >> 1) + rectangle.Offset.x;	}
		else if gbit_true(align, SCREEN_RIGHT	)	{ targetRectangle.Offset.x		= targetWidth  - rectangle.Offset.x - rectangle.Size.x;					}

			 if gbit_true(align, SCREEN_VCENTER	)	{ targetRectangle.Offset.y		= (targetHeight >> 1) - (rectangle.Size.y >> 1) + rectangle.Offset.y;	}
		else if gbit_true(align, SCREEN_BOTTOM	)	{ targetRectangle.Offset.y		= targetHeight - rectangle.Offset.y - rectangle.Size.y;					}
		return targetRectangle;
	}

	typedef SCoord2<int8_t		>											SCoord2i8	;
	typedef SCoord2<int16_t		>											SCoord2i16	;
	typedef SCoord2<int32_t		>											SCoord2i32	;
	typedef SCoord2<uint8_t		>											SCoord2u8	;
	typedef SCoord2<uint16_t	>											SCoord2u16	;
	typedef SCoord2<uint32_t	>											SCoord2u32	;
	typedef SCoord2<float32_t	>											SCoord2f32	;
	typedef SCoord2<float64_t	>											SCoord2f64	;

	typedef SCoord3<int8_t		>											SCoord3i8	;
	typedef SCoord3<int16_t		>											SCoord3i16	;
	typedef SCoord3<int32_t		>											SCoord3i32	;
	typedef SCoord3<uint8_t		>											SCoord3u8	;
	typedef SCoord3<uint16_t	>											SCoord3u16	;
	typedef SCoord3<uint32_t	>											SCoord3u32	;
	typedef SCoord3<float32_t	>											SCoord3f32	;
	typedef SCoord3<float64_t	>											SCoord3f64	;

	typedef SCoord4<int8_t		>											SCoord4i8	;
	typedef SCoord4<int16_t		>											SCoord4i16	;
	typedef SCoord4<int32_t		>											SCoord4i32	;
	typedef SCoord4<uint8_t		>											SCoord4u8	;
	typedef SCoord4<uint16_t	>											SCoord4u16	;
	typedef SCoord4<uint32_t	>											SCoord4u32	;
	typedef SCoord4<float32_t	>											SCoord4f32	;
	typedef SCoord4<float64_t	>											SCoord4f64	;
}

#endif // __COORD_H__283409263409234698374__