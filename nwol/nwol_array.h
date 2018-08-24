/// Copyright 2016-2017 - asm128
#include "nwol_array_view.h"
#include "nwol_memory.h"

#ifndef ARRAY_H_652434654236655143465
#define ARRAY_H_652434654236655143465

namespace nwol 
{
	template <typename... _Args>	void			clear	(_Args&&... args)						{ const int32_t results[] = {args.clear		()			..., 0}; }
	template <typename... _Args>	void			resize	(uint32_t newSize, _Args&&... args)		{ const int32_t results[] = {args.resize	(newSize)	..., 0}; }

	template<typename _tBase, size_t _arraySize>	struct array_static_base						{ protected: _tBase Block[_arraySize]; };
	template<typename _tBase, size_t _arraySize>
	struct array_static : public ::nwol::array_view<_tBase>, public ::nwol::array_static_base<_tBase, _arraySize> {
		typedef ::nwol::array_static_base<_tBase, _arraySize>	_ArrayStaticBase;
	public:
		using				_ArrayStaticBase	::		Block										;
		using				array_view<_tBase>	::		Data										;
		using				array_view<_tBase>	::		Count										;
		typedef				array_view<_tBase>			_TArrayView;

		inline constexpr								array_static								()																						: array_view<_tBase>(Block)													{}
		template<size_t _arraySizeOther>
		inline											array_static								(const _tBase (&sourceData)[_arraySizeOther])											: array_view<_tBase>(Block)													{
			Count											= 0;
			for(const uint32_t maxCount = (uint32_t)::nwol::min(_arraySize, _arraySizeOther); Count < maxCount; ++Count)
				Block[Count]									= sourceData[Count];
		}
	};

	// Base for arrays that keeps track of its actual size.
	template<typename _tBase>
	struct array_base : public array_view<_tBase> {
	protected:
		using				array_view<_tBase>::		Count;
							uint32_t					Size										= 0;

		inline constexpr								array_base									()																			noexcept	= default;
		inline constexpr								array_base									(const array_base<_tBase>&		other)										noexcept	= delete;
		inline constexpr								array_base									(const array_base<_tBase>&&	other)											noexcept	= delete;

							array_base<_tBase>&			operator =									(const array_base<_tBase>&		other)													= delete;
							array_base<_tBase>&			operator =									(const array_base<_tBase>&&	other)														= delete;
		// This helper method is used to prevent redundancies. It returns a safe integer of the same or a higher value than the one passed as argument.
		inline constexpr	uint32_t					calc_reserve_size							(const uint32_t newSize)											const	noexcept	{ return ::nwol::max(newSize, newSize + ::nwol::max(newSize >> 1, 4U));						}
		inline constexpr	uint32_t					calc_malloc_size							(const uint32_t newSize)											const	noexcept	{ return ::nwol::max(newSize*(uint32_t)sizeof(_tBase), Count*(uint32_t)sizeof(_tBase));	}
	}; // array_base

	// This class is optimized to contain POD instances and won't work for C++ objects that require calling constructors/destructors.
	template<typename _tPOD>
	struct array_pod : public array_base<_tPOD> {
		typedef				array_base<_tPOD>			_TVectorBase								;
		typedef				array_view<_tPOD>			_TArrayView									;

		using				_TVectorBase::				Count										;
		using				_TVectorBase::				Data										;
		using				_TVectorBase::				Size										;
		using				_TVectorBase::				calc_reserve_size							;
		using				_TVectorBase::				calc_malloc_size							;
		using				_TVectorBase::				operator[]									;

		inline											~array_pod									()																						{ safe_nwol_free(Data);		}
		inline constexpr								array_pod									()																			noexcept	= default;
		inline											array_pod									(uint32_t initialSize)																	{ resize(initialSize);		}
		inline											array_pod									(array_pod<_tPOD>&& other)													noexcept	{
			Size											= other.Size									;
			Count											= other.Count									;
			Data											= other.Data									;

			other.Size										= other.Count									= 0;
			other.Data										= 0;
		}
														array_pod									(const array_pod<_tPOD>& other)															{
			if(other.Count) {
				const uint32_t										newSize										= other.Count;
				const uint32_t										reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize);
				throw_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD)), "", "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize)
				else {
					::nwol::auto_nwol_free								safeguard;
					Data											= (_tPOD*)(safeguard.Handle = ::nwol::nwol_malloc(mallocSize));
						 throw_if(0 == Data			, "", "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize)
					else throw_if(0 == other.Data	, "", "%s", "other.Data is null!")
					else {
						for(uint32_t i = 0, count = newSize; i<count; ++i)
							Data[i]											= other[i];
						Size											= (uint32_t)reserveSize;
						Count											= other.Count;
						safeguard.Handle								= 0;
					}
				}
			}
		}
		template <size_t _otherCount>
														array_pod									(const _tPOD (&other)[_otherCount])														{
			if(other.Count) {
				const uint32_t										newSize										= other.Count;
				const uint32_t										reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize);
				throw_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD)), "", "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize)
				else {
					::nwol::auto_nwol_free								safeguard;
					Data											= (_tPOD*)(safeguard.Handle = ::nwol::nwol_malloc(mallocSize));
						 throw_if(0 == Data			, "", "Failed to allocate array. Requested size: %u. ", (uint32_t)newSize) 
					else throw_if(0 == other.Data	, "", "%s", "other.Data is null!") 
					else {
						for(uint32_t i = 0, count = newSize; i < count; ++i)
							Data[i]											= other[i];
						Size											= (uint32_t)reserveSize;
						Count											= other.Count;
						safeguard.Handle								= 0;
					}
				}
			} // 
		}
							array_pod<_tPOD>&			operator =									(const array_pod<_tPOD>& other)															{
			throw_if(resize(other.Count) != (int32_t)other.Count, "", "Failed to assign array.");
			for(uint32_t iElement = 0; iElement < other.Count; ++iElement)
				operator[](iElement)							= other[iElement];
			return *this;
		}
		// This method doesn't call destructors of the contained PODs.
		inline				int32_t						clear										()																			noexcept	{ return Count = 0; }

		// Returns the new size of the array
		inline				int32_t						pop_back									(_tPOD* oldValue)																		{ 
			ree_if(0 == Count, "%s", "Cannot pop elements of an empty array."); 
			--Count;
			safe_podcpy(oldValue, &Data[Count]); 
			return Count; 
		}

		// Returns the index of the pushed value or -1 on failure
		inline				int32_t						push_back									(const _tPOD& newValue)																	{ 
			const int32_t										indexExpected								= Count;
			const int32_t										indexNew									= resize(Count+1, newValue)-1; 
			ree_if(indexNew != indexExpected, "Failed to push value! Array size: %i.", indexExpected);
			return indexNew;
		}

		// Returns the index of the pushed value
		template<size_t _Length>
		inline				int32_t						append										(const _tPOD (&newChain)[_Length])														{ return append(newChain, _Length);		}
		inline				int32_t						append										(const _tPOD* chainToAppend, uint32_t chainLength)										{ 
			const uint32_t										startIndex									= Count;
			const uint32_t										requestedSize								= Count + chainLength; 
			ree_if(requestedSize < Count, "Size overflow. Cannot append chain.");
			const int32_t										newSize										= resize(requestedSize); 
			ree_if(newSize != requestedSize, "Failed to resize array for appending.");

			for(uint32_t i = 0, maxCount = ::nwol::min(chainLength, newSize - startIndex); i < maxCount; ++i)
				Data[startIndex + i]							= chainToAppend[i];
			return startIndex;
		}

		// Returns the new size of the array.
		inline				int32_t						resize										(uint32_t newSize, const _tPOD& newValue)												{ 
			int32_t												oldCount									= (int32_t)Count;
			int32_t												newCount									= resize(newSize);
			ree_if(newCount != (int32_t)newSize, "Failed to resize array. Requested size: %u. Current size: %u (%u).", newCount, Count, Size);
			for( int32_t i = oldCount; i < newCount; ++i )
				::nwol::podcpy(&Data[i], &newValue);
			return newCount;			
		}

		// Returns the new size of the array.
							int32_t						resize										(uint32_t newSize)																		{ 
			const uint32_t										oldCount									= Count;
			if(newSize >= Size) {
				_tPOD												* oldData									= Data;
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize);
				ree_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize)
				::nwol::auto_nwol_free								safeguard;
				_tPOD												* newData									= (_tPOD*)(safeguard.Handle = ::nwol::nwol_malloc(mallocSize));
				ree_if(nullptr == newData, "Failed to resize array. Requested size: %u. Current size: %u.", newSize, (uint32_t)Size);

				_TArrayView											safe_data									= {newData, reserveSize};
				if(oldData) {
					for(uint32_t i = 0, count = ::nwol::min(newSize, oldCount); i<count; ++i)
						safe_data[i]									= operator[](i);
				}
				Size											= (uint32_t)reserveSize;
				Count											= newSize;
				Data											= newData;
				safeguard.Handle								= 0;
				if(oldData)
					::nwol::nwol_free(oldData);
			}
			else
				Count											= newSize;

			return (int32_t)Count;
		}

		// returns the new size of the list or -1 on failure.
							int32_t						insert										(uint32_t index, const _tPOD& newValue)													{ 
			ree_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count+1) {
				_tPOD												* oldData									= Data;
				uint32_t											reserveSize									= calc_reserve_size(Count + 1);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize);
				ree_if(mallocSize != (reserveSize * (uint32_t)sizeof(_tPOD)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize)
				::nwol::auto_nwol_free								safeguard;
				_tPOD												* newData									= (_tPOD*)(safeguard.Handle = ::nwol::nwol_malloc(mallocSize));
				ree_if(nullptr == newData, "Failed to allocate array for inserting new value.");

				_TArrayView											viewSafe									= {newData, Count+1};
				for(uint32_t i = 0, maxCount = ::nwol::min(index, Count); i < maxCount; ++i)
					viewSafe[i]										= oldData[i];
				viewSafe[index]									= newValue;
				for(uint32_t i = index, maxCount = ::nwol::min(index + 1, Count); i < maxCount; ++i) 
					viewSafe[i + 1]									= oldData[i];
				Data											= newData;
				safeguard.Handle								= 0;
			}	
			else {
				for(int32_t i = (int32_t)::nwol::min(index, Count - 1), maxCount = (int32_t)index; i >= maxCount; --i) 
					Data[i + 1]										= Data[i];

				Data[index]										= newValue;
			}			
			return ++Count;
		}

		// returns the new size of the list or -1 on failure.
		template<size_t _chainLength>
		inline				int32_t						insert										(uint32_t index, const _tPOD* (&chainToInsert)[_chainLength])							{ return insert(index, chainToInsert, _chainLength); }
							int32_t						insert										(uint32_t index, const _tPOD* chainToInsert, uint32_t chainLength)						{
			ree_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count + chainLength) {
				_tPOD												* oldData									= Data;
				uint32_t											newSize										= calc_reserve_size(Count + chainLength);
				uint32_t											mallocSize									= calc_malloc_size(newSize);
				ree_if(mallocSize != (newSize * (uint32_t)sizeof(_tPOD)), "Alloc size overflow. Requested size: %u. malloc size: %u.", newSize, mallocSize)
				::nwol::auto_nwol_free								safeguard;
				_tPOD												* newData									= (_tPOD*)(safeguard.Handle = ::nwol::nwol_malloc(mallocSize));
				ree_if(nullptr == newData, "Failed to allocate array for inserting new value.");
				::nwol::array_view<_tPOD>							viewSafe									= {newData, newSize};
				for(uint32_t i = 0					, maxCount = ::nwol::min(index, Count)				; i <	maxCount; ++i)	viewSafe[i			]	= oldData[i];
				for(uint32_t i = 0					, maxCount = ::nwol::min(chainLength, newSize-index); i <	maxCount; ++i)	viewSafe[i + index	]	= chainToInsert[i];
				for(uint32_t i = index + chainLength, maxCount = ::nwol::min(index + 1, Count)			; i <	maxCount; ++i)	viewSafe[i + 1		]	= oldData[i];
				Data											= newData;
				safeguard.Handle								= 0;
			}	
			else {	// no need to reallocate and copy, just shift rightmost elements and insert in-place
				for(int32_t  i = (int32_t)::nwol::min(index, Count - 1), maxCount = (int32_t)index	; i >=	maxCount; --i)	Data[i + chainLength]	= Data[i];
				for(uint32_t i = 0, maxCount = ::nwol::min(chainLength, Count - index)				; i <	maxCount; ++i)	Data[i + index		]	= chainToInsert[i];
			}
			return Count += chainLength;
		}

		// Returns the new size of the list or -1 if the array pointer is not initialized.
		inline				int32_t						remove_unordered							(uint32_t index)																		{ 
			ree_if(0 == Data, "%s", "Uninitialized array pointer!");
			ree_if(index >= Count, "Invalid index: %u.", index);
			Data[index]										= Data[--Count];
			return Count;
		}

		// Returns the index of the argument if found or -1 if not.
		inline				int32_t						find										(const _tPOD& valueToLookFor)										const				{
			for(uint32_t i = 0; i < Count; ++i) 
				if(0 == ::nwol::podcmp(&Data[i], &valueToLookFor))
					return i;
			return -1;
		}

		inline				::nwol::error_t				read										(const byte_t* input, uint32_t* inout_bytesRead)										{
			uint32_t											elementsToRead								= 0;
			if(input) {
				uint32_t											elementsToRead								= *(uint32_t*)input;
				input											+= sizeof(uint32_t);
			}

			*inout_bytesRead								+= sizeof(uint32_t);
			if(0 == elementsToRead) {
				Count											= 0;
				return 0;
			}

			::nwol::array_view<_tPOD>							newStorage									= {(_tPOD*)::nwol::nwol_malloc(sizeof(_tPOD) * elementsToRead), elementsToRead};
			ree_if(0 == newStorage.begin(), "Failed to allocate array for storing read elements.");
			if(0 == input) {
				for(uint32_t i = 0; i < Count; ++i) 
					*inout_bytesRead								+= sizeof(_tPOD);
			}
			else {
				for(uint32_t i = 0; i < Count; ++i) {
					::nwol::podcpy(&newStorage[i], input);
					input											+= sizeof(_tPOD);
					*inout_bytesRead								+= sizeof(_tPOD);
				}
			}
			Count											= elementsToRead;
			Size											= elementsToRead;
			_tPOD											* old											= Data;
			Data											= newStorage.begin();
			::nwol::safe_nwol_free(old);
			return 0;
		}

		inline				::nwol::error_t				write										(byte_t* input, uint32_t* inout_bytesWritten)						const				{
			if(0 == input) {
				inout_bytesWritten							+= sizeof(uint32_t) + sizeof(_tPOD) * Count;	// Just return the size required to store this.
				return 0;
			}

			*(uint32_t*)input								= Count;
			input											+= sizeof(uint32_t);
			*inout_bytesWritten								+= sizeof(uint32_t);

			::nwol::array_view<_tPOD>							newStorage									= {input, Count};
			for(uint32_t i = 0; i < Count; ++i) {
				::nwol::podcpy(&newStorage[i], &Data[i]);
				input											+= sizeof(_tPOD);
				*inout_bytesWritten								+= sizeof(_tPOD);
			}
			return 0;
		}
	}; // array_pod

	// This class is optimized to contain POD instances and won't work for C++ objects that require calling constructors/destructors	.
	template<typename _tPOD>
	struct string : public array_pod<_tPOD> {};

	//------------------------------------------------------------------------------------------------------------
	template<typename _tObj>
	struct array_obj : public array_base<_tObj> {
		typedef				array_base<_tObj>			_TVectorBase;

		using											_TVectorBase::Count;
		using											_TVectorBase::Data;
		using											_TVectorBase::Size;
		using											_TVectorBase::calc_reserve_size;
		using											_TVectorBase::calc_malloc_size;
		using											_TVectorBase::operator[];

		inline											~array_obj									()																						{ for(uint32_t i = 0; i < Count; ++i) Data[i].~_tObj(); safe_nwol_free(Data); }	// dtor
		inline constexpr								array_obj									()																						= default;
		inline											array_obj									(const uint32_t newSize)																{ throw_if(((int32_t)newSize) != resize(newSize), "", "%s", "Failed to resize array."); }
		inline											array_obj									(array_obj<_tObj>&& other)													noexcept	{
			Size											= other.Size									;
			Count											= other.Count									;
			Data											= other.Data									;

			other.Size										= other.Count									= 0;
			other.Data										= 0;
		}	// move ctor

														array_obj									(const array_obj<_tObj>& other)															{
			if(other.Count) {
				uint32_t											newSize										= other.Count;
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize);
				throw_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "", "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize)
				else {
					::nwol::auto_nwol_free								safeguard;
					_tObj												* newData									= (_tObj*)(safeguard.Handle = ::nwol::nwol_malloc(mallocSize));
						 throw_if(0 == newData		, "", "Failed to resize array. Requested size: %u. Current size: %u.", (uint32_t)newSize, (uint32_t)Size)
					else throw_if(0 == other.Data	, "", "%s", "other.Data is null!")
					else {
						for(uint32_t i = 0; i < newSize; ++i) 
							new (&newData[i]) _tObj(other.Data[i]);
						Data											= newData;
						Size											= reserveSize;
						Count											= other.Count;
						safeguard.Handle								= 0;
					}
				}
			}
		}

		inline				array_obj<_tObj>&			operator =									(const array_obj<_tObj>& other)															{
			throw_if(resize(other.Count) != (int32_t)other.Count, "", "Failed to resize array!")
			else {
				for(uint32_t iElement = 0; iElement < other.Count; ++iElement)
					operator[](iElement)							= other[iElement];
			}
			return *this;
		}

		inline				int32_t						clear										()																						{ 
			for(uint32_t i = 0; i < Count; ++i)
				Data[i].~_tObj();
			return Count									= 0; 
		}

		// Returns the new size of the array
		inline				int32_t						pop_back									(_tObj* oldValue)																		{ 
			ree_if(0 == Count, "%s", "Cannot pop elements from an empty array."); 
			--Count;
			safe_assign(oldValue, Data[Count]); 
			Data[Count].~_tObj();
			return Count; 
		}

		// Returns the index of the pushed value or -1 on failure
		inline				int32_t						push_back									(const _tObj& newValue)																	{ 
			int32_t												indexExpected								= Count;
			int32_t												indexNew									= resize(Count+1, newValue)-1; 
			ree_if(indexNew != indexExpected, "Failed to push value! Array size: %i.", indexExpected);
			return indexNew;
		}


		// returns the new array size or -1 if failed.
		template<typename _tObj>
		inline				int32_t						erase										(const _tObj* address)																	{ 
			ree_if(0 == Data, "Uninitialized array pointer!");
			const ptrdiff_t										ptrDiff										= ptrdiff_t(address) - (ptrdiff_t)Data;
			const uint32_t										index										= (uint32_t)(ptrDiff / (ptrdiff_t)sizeof(_tObj));
			ree_if(index >= Count, "Invalid index: %u.", index);
			return remove(index); 
		}

							int32_t						remove										(uint32_t index)																		{ 
			ree_if(0 == Data, "Uninitialized array pointer!");
			ree_if(index >= Count, "Invalid index: %u.", index);
			--Count;
			while(index < Count) {
				Data[index].~_tObj();							// Destroy old
				new (&Data[index]) _tObj(Data[index + 1]);		// Placement new
				++index;
			}
			Data[index].~_tObj();							// Destroy last
			return Count;
		}

		// Returns the new size of the list or -1 if the array pointer is not initialized.
							int32_t						remove_unordered							(uint32_t index)																		{ 
			ree_if(0 == Data, "Uninitialized array pointer!");
			ree_if(index >= Count, "Invalid index: %u.", index);
			Data[index].~_tObj();							// Destroy old
			if(1 == Count) 
				--Count;
			else {
				new (&Data[index]) _tObj(Data[--Count]);	// Placement new
				Data[Count].~_tObj();						// Destroy reordered
			}
			return Count;
		}

		// returns the new size of the list or -1 on failure.
							int32_t						insert										(uint32_t index, const _tObj& newValue)													{ 
			ree_if(index >= Count, "Invalid index: %u.", index);

			if(Size < Count + 1) {
				_tObj												* oldData								= Data;
				uint32_t											reserveSize								= calc_reserve_size(Count+1);
				uint32_t											mallocSize								= calc_malloc_size(reserveSize);
				ree_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize)
				::nwol::auto_nwol_free								safeguard;
				_tObj												* newData								= (_tObj*)(safeguard.Handle = ::nwol::nwol_malloc(mallocSize));
				ree_if(0 == newData, "Failed to allocate for inserting new element into array! current size: %u. new size: %u.", Size, mallocSize);
				::nwol::array_view<_tObj>							viewSafe								= {newData, Count+1};
				for(uint32_t i = 0, maxCount = ::nwol::min(index + 1, Count); i < maxCount; ++i) {
					new (&viewSafe[i]) _tObj(oldData[i]);
					oldData[i].~_tObj();
				}
				new (&viewSafe[index]) _tObj(newValue);
				for(uint32_t i = index, maxCount = ::nwol::min(index + 1, Count); i < maxCount; ++i) {
					new (&viewSafe[i + 1]) _tObj(oldData[i]);
					oldData[i].~_tObj();
				}
				Data											= newData;
				Size											= reserveSize;
				safeguard.Handle								= 0;
			}	
			else {
				for(int32_t i = (int32_t)::nwol::min(index, Count - 1), maxCount = (int32_t)index; i >= maxCount; --i) {
					new (&Data[i + 1]) _tObj(Data[i]);
					Data[i].~_tObj();
				}
				new (&Data[index]) _tObj(newValue);
			}			
			return ++Count;
		}

		// Returns the new size of the array or -1 if failed.
		template <typename... _tArgs>
							int32_t						resize										(uint32_t newSize, _tArgs&&... constructorArgs)											{
			uint32_t											oldCount									= Count;
			if(newSize > Size) {
				_tObj												* oldData									= Data;
				uint32_t											reserveSize									= calc_reserve_size(newSize);
				uint32_t											mallocSize									= calc_malloc_size(reserveSize);
				ree_if(mallocSize != (reserveSize*(uint32_t)sizeof(_tObj)), "Alloc size overflow. Requested size: %u. malloc size: %u.", reserveSize, mallocSize)
				::nwol::auto_nwol_free								safeguard;
				_tObj												* newData									= (_tObj*)(safeguard.Handle = ::nwol::nwol_malloc(mallocSize));
				ree_if(nullptr == newData, "Failed to resize array. Requested size: %u. Current size: %u.", newSize, Size)
				if(oldData) {
					for(uint32_t i = 0, copyCount = ::nwol::min(oldCount, newSize); i < copyCount; ++i)
						new (&newData[i]) _tObj(oldData[i]);
					for(uint32_t i = 0; i < oldCount; ++i)
						oldData[i].~_tObj();
				}
				Data											= newData;
				Size											= reserveSize;
				Count											= (uint32_t)newSize;
				safeguard.Handle								= 0;
				if(oldData) 
					::nwol::nwol_free(oldData);
			}
			else {
				for(int32_t i = ((int32_t)oldCount) - 1, newCount = (int32_t)newSize; i >= newCount; --i)	// we need to cast to int32_t because otherwise --i	will underflow to UINT_MAX and loop forever.
					Data[i].~_tObj();
				Count											= (uint32_t)newSize;
			}
			for(uint32_t i = oldCount; i < Count; ++i)
				new (&Data[i])_tObj(constructorArgs...);

			return (int32_t)Count;
		}	

		//inline				int32_t						find										(const _tObj& valueToLookFor)										const				{
		//	for(uint32_t i=0; i<Count; ++i)
		//		if(Data[i] == valueToLookFor)
		//			return i;
		//	return -1;
		//}
	}; // array_obj

	// returns the index or -1 if not found.
	template <typename _tValue>
	inline				int32_t						find										(const _tValue* begin, const _tValue* end, const _tValue& valueToLookFor)				{
		for(uint32_t i = 0; begin != end; ++i, ++begin)
			if(valueToLookFor == *begin)
				return i;

		return -1;
	}

	// returns the index or -1 if not found.
	template <typename _tValue>
	inline				int32_t						find										(const ::nwol::array_pod<_tValue>& valueContainer, const _tValue& valueToLookFor)		{
		const _tValue										* begin										= valueContainer.begin();
		for(uint32_t i = 0, maxCount = valueContainer.size(); i < maxCount; ++i, ++begin)
			if(valueToLookFor == *begin)
				return i;

		return -1;
	}

} // namespace

#endif // ARRAY_H_652434654236655143465