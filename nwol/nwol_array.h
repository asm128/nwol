#include "array_view.h"
#include "nwol_memory.h"
#include "evaluation.h"

#include <memory.h>

#ifndef __ARRAY_H__652434654236655143465__
#define __ARRAY_H__652434654236655143465__

namespace nwol 
{
	template <typename... _Args>	void	clear	(_Args&&... args)						{ const int32_t results[] = {args.clear		()			..., 0}; }
	template <typename... _Args>	void	resize	(uint32_t newSize, _Args&&... args)		{ const int32_t results[] = {args.resize	(newSize)	..., 0}; }

#pragma pack(push, 1)

	template<typename _tBase, size_t _Size>				struct array_static_base				{ protected: _tBase Block[_Size]; };
	template<typename _tBase, size_t _Size>
	struct array_static : public ::nwol::array_view<_tBase>, public ::nwol::array_static_base<_tBase, _Size> {
		typedef ::nwol::array_static_base<_tBase, _Size>	_ArrayStaticBase;
	public:
		using				_ArrayStaticBase::			Block									;
		using				array_view<_tBase>	::			Data									;
		using				array_view<_tBase>	::			Count									;
		typedef				array_view<_tBase>				_TArrayView;

		inline constexpr								array_static							()																							: array_view<_tBase>(Block)													{}
		template<size_t _SizeOther>
		inline											array_static							(const _tBase (&sourceData)[_SizeOther])														: array_view<_tBase>(Block)													{
			Count											= 0;
			for(uint32_t maxCount=::nwol::min(_Size, _SizeOther); Count < maxCount; ++Count)
				Block[Count]									= sourceData[Count];
		}
	};


	// Base for arrays that keeps track of its actual size.
	template<typename _tBase>
	struct vector_base : public array_view<_tBase> {
	protected:
		using				array_view<_tBase>::			Count;
							uint32_t					Size									= 0;

		inline constexpr								vector_base								()																				noexcept	= default;
		inline constexpr								vector_base								(const vector_base<_tBase>&		other)												noexcept	= delete;
		inline constexpr								vector_base								(const vector_base<_tBase>&&	other)												noexcept	= delete;

							vector_base<_tBase>&		operator =								(const vector_base<_tBase>&		other)															= delete;
							vector_base<_tBase>&		operator =								(const vector_base<_tBase>&&	other)															= delete;
		// This helper method is used to prevent redundancies. It returns a safe integer of the same or a higher value than the one passed as argument.
		inline constexpr	uint32_t					calc_reserve_size						(const uint32_t newSize)												const	noexcept	{ return ::nwol::max(newSize, newSize+nwol::max(newSize>>1, 4U));				}
		inline constexpr	uint32_t					calc_malloc_size						(const uint32_t newSize)												const	noexcept	{ return ::nwol::max(newSize*(uint32_t)sizeof(_tBase), Count*(uint32_t)sizeof(_tBase));	}
	}; // vector_base

	// This class is optimized to contain POD instances and won't work for C++ objects that require calling constructors/destructors.
	template<typename _TPOD>
	struct array_pod : public vector_base<_TPOD> {
		typedef				vector_base<_TPOD>			_TVectorBase								;
		typedef				array_view<_TPOD>			_TArrayView									;

		using				_TVectorBase::				Count										;
		using				_TVectorBase::				Data										;
		using				_TVectorBase::				Size										;
		using				_TVectorBase::				calc_reserve_size							;
		using				_TVectorBase::				calc_malloc_size							;
		using				_TVectorBase::				operator[]									;

		inline											~array_pod									()																							{ safe_nwol_free(Data);	}
		inline constexpr								array_pod									()																				noexcept	= default;
		inline											array_pod									(uint32_t initialSize)																		{ resize(initialSize);		}
		inline											array_pod									(array_pod<_TPOD>&& other)														noexcept	{
			Size											= other.Size									;
			Count											= other.Count									;
			Data											= other.Data									;

			other.Size										= other.Count									= 0;
			other.Data										= 0;
		}
														array_pod									(const array_pod<_TPOD>& other)																{
			if(other.Count) {
				const uint32_t										newSize										= other.Count;
				const uint32_t										reserveSize									= calc_reserve_size(newSize);
				Data											= (_TPOD*)::nwol::nwol_malloc(calc_malloc_size(reserveSize));
				if(Data) {
					throw_if(0 == other.Data, "", "%s", "other.Data is null!")
					else {
						for(uint32_t i = 0, count = newSize; i<count; ++i)
							Data[i]											= other[i];
						Size											= (uint32_t)reserveSize;
						Count											= other.Count;
					}
				}
				else {
					error_printf("Failed to resize array. Requested size: %u. ", (uint32_t)newSize);
				}	// if(Data)
			}	// if(other.Count)
		}
		template <size_t _otherCount>
														array_pod									(const _TPOD (&other)[_otherCount])														{
			if(other.Count) {
				const uint32_t										newSize										= other.Count;
				const uint32_t										reserveSize									= calc_reserve_size(newSize);
				Data											= (_TPOD*)::nwol::nwol_malloc(calc_malloc_size(reserveSize));
				if(Data) {
					throw_if(0 == other.Data, "", "%s", "other.Data is null!")
					else {
						for(uint32_t i = 0, count = newSize; i<count; ++i)
							Data[i]											= other[i];
						Size											= (uint32_t)reserveSize;
						Count											= other.Count;
					}
				}
				else {
					error_printf("Failed to resize array. Requested size: %u. ", (uint32_t)newSize);
				}	// if(Data)
			}	// if(other.Count)
		}
							array_pod<_TPOD>&			operator =									(const array_pod<_TPOD>& other)															{
			resize(other.Count);
			for(uint32_t iElement = 0; iElement < other.Count; ++iElement)
				operator[](iElement)																		= other[iElement];
			return *this;
		}
		// This method doesn't call destructors of the contained PODs.
		inline				int32_t						clear										()																			noexcept	{ return Count = 0; }

		// Returns the index of the pushed value
		inline				int32_t						push_back									(const _TPOD& newValue)																	{ 
			const int32_t										indexExpected								= Count;
			const int32_t										indexNew									= resize(Count+1, newValue)-1; 
			if(indexNew != indexExpected)
				error_printf("Failed to push value! Array size: %i.", indexExpected);
			return indexNew;
		}
		// Returns the index of the pushed value
		template<size_t _Length>
		inline				int32_t						append										(const _TPOD* chainToAppend, uint32_t chainLength)										{ 
			const int32_t										startIndex									= Count;
			resize(Count+chainLength); 
			for(uint32_t i=0, maxCount = ::nwol::min(chainLength, Count-startIndex); i<maxCount; ++i)
				Data[startIndex+i]								= chainToAppend[i];
			return startIndex;
		}
		template<size_t _Length>
		inline				int32_t						append										(const _TPOD (&newChain)[_Length])														{ 
			return append(newChain, _Length);
		}
		// Returns the new size of the array.
							int32_t						resize										(uint32_t newSize)																		{ 
			const uint32_t										oldCount									= Count;
			if(newSize >= Size) {
				_TPOD												* oldData									= Data;
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				_TPOD												* newData									= (_TPOD*)::nwol::nwol_malloc(calc_malloc_size(reserveSize));
				_TArrayView											safe_data									= {newData, reserveSize};

				if(newData) {
					if(oldData)
						for(uint32_t i=0, count = ::nwol::min(newSize, oldCount); i<count; ++i)
							safe_data[i]									= operator[](i);

					Size											= (uint32_t)reserveSize;
					Count											= newSize;
					Data											= newData;
					if(oldData)
						::nwol::nwol_free(oldData);
				}
				else {
					error_printf("Failed to resize array. Requested size: %u. Current size: %u.", newSize, (uint32_t)Size);
				}
			}
			else
				Count									= newSize;

			return (int32_t)Count;
		}
		// returns the new size of the list or -1 on failure.
							int32_t						insert										(uint32_t index, const _TPOD& newValue)													{ 
			reterr_error_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count+1) {
				_TPOD												* oldData									= Data;
				uint32_t											reserveSize									= calc_reserve_size(Count+1);
				_TPOD												* newData									= (_TPOD*)::nwol::nwol_malloc(calc_malloc_size(reserveSize));
				_TArrayView											viewSafe									= {newData, Count+1};
				for(uint32_t i=0, maxCount = ::nwol::min(index, Count); i<maxCount; ++i)
					viewSafe[i]										= oldData[i];
				viewSafe[index]									= newValue;
				for(uint32_t i=index, maxCount = ::nwol::min(index+1, Count); i<maxCount; ++i) 
					viewSafe[i+1]									= oldData[i];
				Data											= newData;
			}	
			else {
				for(int32_t i = (int32_t)::nwol::min(index, Count-1), maxCount = (int32_t)index; i >= maxCount; --i) 
					Data[i+1]										= Data[i];

				Data[index]										= newValue;
			}			
			return ++Count;
		}
		// returns the new size of the list or -1 on failure.
							int32_t						insert										(uint32_t index, const _TPOD* chainToInsert, uint32_t chainLength)						{
			reterr_error_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count+chainLength) {
				_TPOD												* oldData									= Data;
				uint32_t											newSize										= calc_reserve_size(Count+chainLength);
				_TPOD												* newData									= (_TPOD*)::nwol::nwol_malloc(calc_malloc_size(newSize));
				::nwol::array_view<_TPOD>							viewSafe									= {newData, newSize};
				
				for(uint32_t i=0, maxCount = ::nwol::min(index, Count); i<maxCount; ++i)
					viewSafe[i]										= oldData[i];

				for(uint32_t i=0, maxCount = ::nwol::min(chainLength, newSize-index); i<maxCount; ++i)
					viewSafe[i+index]								= chainToInsert[i];

				for(uint32_t i=index+chainLength, maxCount = ::nwol::min(index+1, Count); i<maxCount; ++i)
					viewSafe[i+1]									= oldData[i];

				Data											= newData;
			}	
			else {
				for(int32_t i = (int32_t)::nwol::min(index, Count-1), maxCount = (int32_t)index; i >= maxCount; --i)
					Data[i+chainLength]								= Data[i];
				for(uint32_t i=0, maxCount = ::nwol::min(chainLength, Count-index); i<maxCount; ++i)
					Data[i+index]									= chainToInsert[i];
			}			
			return Count += chainLength;
		}

		template<size_t _Length>
		inline				int32_t						insert										(uint32_t index, const _TPOD* (&chainToInsert)[_Length])								{ return insert(index, chainToInsert, _Length); }
		// Returns the new size of the array.
		inline				int32_t						resize										(uint32_t newSize, const _TPOD& newValue)												{ 
			int32_t												oldCount									= (int32_t)Count;
			int32_t												newCount									= resize(newSize);
			for( int32_t i=oldCount; i < newCount; ++i )
				::nwol::podcpy(&Data[i], &newValue);
			return newCount;			
		}	
		// Returns the new size of the list or -1 if the array pointer is not initialized.
		inline				int32_t						remove_unordered							(uint32_t index)																		{ 
			reterr_error_if(0 == Data, "%s", "Uninitialized array pointer!");

			if(index >= Count) {
				error_printf("Invalid index: %u.", index);
			}
			else 
				Data[index]										= Data[--Count];

			return Count;
		}
		// Returns the index of the argument if found or -1 if not.
		inline				int32_t						find										(const _TPOD& valueToLookFor)										const				{
			for(uint32_t i=0; i<Count; ++i) 
				if(0 == ::nwol::podcmp(&Data[i], &valueToLookFor))
					return i;
			return -1;
		}
	}; // array_pod

	// This class is optimized to contain POD instances and won't work for C++ objects that require calling constructors/destructors	.
	template<typename _TPOD>
	struct string : public array_pod<_TPOD> {};

	//------------------------------------------------------------------------------------------------------------
	template<typename _TObj>
	struct array_obj : public vector_base<_TObj> {
		typedef				vector_base<_TObj>			_TVectorBase;

		using											_TVectorBase::Count;
		using											_TVectorBase::Data;
		using											_TVectorBase::Size;
		using											_TVectorBase::calc_reserve_size;
		using											_TVectorBase::calc_malloc_size;
		using											_TVectorBase::operator[];

		inline											~array_obj									()																						{ for(uint32_t i=0; i<Count; ++i) Data[i].~_TObj(); safe_nwol_free(Data); }	// dtor
		inline constexpr								array_obj									()																						= default;
		inline											array_obj									(const uint32_t newSize)																{ throw_if(((int32_t)newSize) != resize(newSize), "", "%s", "Failed to resize array."); }
		inline											array_obj									(array_obj<_TObj>&& other)													noexcept	{
			Size											= other.Size									;
			Count											= other.Count									;
			Data											= other.Data									;

			other.Size										= other.Count									= 0;
			other.Data										= 0;
		}	// move ctor
														array_obj									(const array_obj<_TObj>& other)															{
			if(other.Count) {
				uint32_t											newSize										= other.Count;
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				_TObj												* newData									= (_TObj*)::nwol::nwol_malloc(calc_malloc_size(reserveSize));
				throw_if(0 == newData, "", "Failed to resize array. Requested size: %u. Current size: %u.", (uint32_t)newSize, (uint32_t)Size)
				else {
					if(other.Data)
						for(uint32_t i=0; i<newSize; ++i)
							new (&newData[i]) _TObj(other.Data[i]);
					else {
						error_printf("%s", "other.Data is null!");
						for(uint32_t i=0; i<newSize; ++i)
							new (&newData[i]) _TObj();
					}
					Data											= newData;
					Size											= reserveSize;
					Count											= other.Count;
				}
			}
		}
		inline				array_obj<_TObj>&			operator =									(const array_obj<_TObj>& other)															{
			resize(other.Count);
			for(uint32_t iElement=0; iElement < other.Count; ++iElement)
				operator[](iElement)							= other[iElement];
			return *this;
		}
		// Returns the index of the pushed value
		inline				int32_t						push_back									(const _TObj& newValue)																	{ 
			int32_t												indexExpected								= Count;
			int32_t												indexNew									= resize(Count+1, newValue)-1; 
			if(indexNew != indexExpected)
				error_printf("Failed to push value! Array size: %i.", indexExpected);
			return indexNew;
		}
		// returns the new size of the list or -1 on failure.
							int32_t						insert										(uint32_t index, const _TObj& newValue)													{ 
			reterr_error_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count+1) {
				_TObj												* oldData								= Data;
				uint32_t											reserveSize								= calc_reserve_size(Count+1);
				_TObj												* newData								= (_TObj*)::nwol::nwol_malloc(calc_malloc_size(reserveSize));
				reterr_error_if(0 == newData, "Failed to allocate for inserting new element into array! current size: %u. new size: %u.", Size, calc_malloc_size(reserveSize));
				::nwol::array_view<_TObj>							viewSafe								= {newData, Count+1};
				for(uint32_t i=0, maxCount = ::nwol::min(index+1, Count); i<maxCount; ++i) {
					new (&viewSafe[i]) _TObj(oldData[i]);
					oldData[i].~_TObj();
				}
				new (&viewSafe[index]) _TObj(newValue);
				for(uint32_t i=index, maxCount = ::nwol::min(index+1, Count); i<maxCount; ++i) {
					new (&viewSafe[i+1]) _TObj(oldData[i]);
					oldData[i].~_TObj();
				}
				Data											= newData;
				Size											= reserveSize;
			}	
			else {
				for(int32_t i = (int32_t)::nwol::min(index, Count-1), maxCount = (int32_t)index; i >= maxCount; --i) {
					new (&Data[i+1]) _TObj(Data[i]);
					Data[i].~_TObj();
				}
				new (&Data[index]) _TObj(newValue);
			}			
			return ++Count;
		}
		inline				int32_t						clear										()																						{ 
			for(uint32_t i=0; i<Count; ++i)
				Data[i].~_TObj();
			return Count									= 0; 
		}
		// Returns the new size of the array.
		template <typename... _TArgs>
							int32_t						resize										(uint32_t newSize, _TArgs&&... constructorArgs)											{
			uint32_t											oldCount									= Count;
			bool												failed										= false;
			if(newSize > Size) {
				_TObj												* oldData									= Data;
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				_TObj												* newData									= (_TObj*)::nwol::nwol_malloc(calc_malloc_size(reserveSize));
				if(newData) {
					if(oldData) {
						for(uint32_t i=0, copyCount = ::nwol::min(oldCount, newSize); i<copyCount; ++i)
							new (&newData[i]) _TObj(oldData[i]);
						for(uint32_t i=0; i<oldCount; ++i)
							oldData[i].~_TObj();
					}
					Data											= newData;
					Size											= reserveSize;
					Count											= (uint32_t)newSize;
					if(oldData) 
						::nwol::nwol_free(oldData);
				}
				else {
					error_printf("Failed to resize array. Requested size: %u. Current size: %u.", (uint32_t)newSize, (uint32_t)Size);
					failed											= true;
				}
			}
			else {
				for(int32_t i=((int32_t)oldCount)-1, newCount=(int32_t)newSize; i >= newCount; --i)	// we need to cast to int32_t because otherwise --i	will underflow to UINT_MAX and loop forever.
					Data[i].~_TObj();
				Count											= (uint32_t)newSize;
			}
			if(!failed) {
				for(uint32_t i=oldCount; i<Count; ++i)
					new (&Data[i])_TObj(constructorArgs...);
			}

			return (int32_t)Count;
		}	
		// Returns the new size of the list or -1 if the array pointer is not initialized.
							int32_t						remove_unordered							(uint32_t index)																		{ 
			reterr_error_if(0 == Data, "Uninitialized array pointer!");

			if(index >= Count) {
				error_printf("Invalid index: %u.", index);
			}
			else {
				Data[index].~_TObj();							// Destroy old
				if(1 == Count) 
					--Count;
				else {
					new (&Data[index]) _TObj(Data[--Count]);	// Placement new
					Data[Count].~_TObj();						// Destroy reordered
				}
			}

			return Count;
		}
		inline				int32_t						find										(const _TObj& valueToLookFor)										const				{
			for(uint32_t i=0; i<Count; ++i)
				if(Data[i] == valueToLookFor)
					return i;
			return -1;
		}
	}; // array_obj
#pragma pack(pop)
} // namespace

#endif // __ARRAY_H__652434654236655143465__