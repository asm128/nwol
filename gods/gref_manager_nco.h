#include "gcore_ref_globals.h"

#include "nwol_array.h"
#include "nwol_mutex.h"
#include "nwol_debug.h"

#ifndef NWOL_GREF_MANAGER_NCO_H_287346872
#define NWOL_GREF_MANAGER_NCO_H_287346872

#if defined(ERROR_PRINTF_ENABLED)	// otherwise error_printf() has no effect and these macros resolve into nothing. Removing them reduces warnings from the compiler because of the unused string array.
static const char* __string_table_errors[2]= 
										{	"Reference counter is zero! This is often because of a gptr_pod being cast to a GODS() type and then released with grelease(). As casting to a reference to GODS doesn't mean the pointer is being copied, the reference counter is not incremented."
										,	"Failed to allocate! Out of memory?"
										};		

#define errmsg_refcountnull()		error_printf("%s", __string_table_errors[0])
#define errmsg_alloc()				error_printf("%s", __string_table_errors[1])
#else
#define errmsg_refcountnull()		
#define errmsg_alloc()				
#endif

#if defined(NWOL_DEBUG_ENABLED)

#	define CHECKBUFFEROVERRUNINSTANCE()													\
	if( pNextPage->isBufferOverrunInstance(pNextPage->getInstanceEntry(p0->Column)) ) {	\
		ERROR_PRINTF_ALLOCID(p0);														\
		_CrtDbgBreak();																	\
	}
#	define ERROR_PRINTF_ALLOCID(p)	error_printf("AllocID: %u", uint32_t(p->AllocID))
#	define ACTUALSIZEPADDED		ActualSizePadded
#	define ALIGNINSTANCEARRAY()
#	define GLOBALSOVERRUNCHECK_START	BINFIBO,	// debug check
#	define GLOBALSOVERRUNCHECK_END		, BINFIBO	// debug check
#	define SETREFALLOCID(_newRef)		setRefAllocID(_newRef)
#	define CHECKBUFFEROVERRUNREF(p_)	::nwol::checkOverrun(p_)
#	define COUNTERINCREMENTACQUIREDREFS()	NWOL_INTERLOCKED_INCREMENT(Counters.AcquiredRefs)
#	define COUNTERINCREMENTFREEDREFS()		NWOL_INTERLOCKED_INCREMENT(Counters.FreedRefs)
#else
#	define CHECKPRE_MEMBER 
#	define CHECKPOST_MEMBER

#	define CHECKBUFFEROVERRUNINSTANCE()
#	define ERROR_PRINTF_ALLOCID(p)
#	define ACTUALSIZEPADDED		Globals.DataSizePadded
#	define ALIGNINSTANCEARRAY()	Instances			= &((char*)Instances)[calc_align_address(BASETYPE_ALIGN, Instances)]; // align to 16 byte boundary
#	define GLOBALSOVERRUNCHECK_START
#	define GLOBALSOVERRUNCHECK_END
#	define SETREFALLOCID(_newRef)		
#	define CHECKBUFFEROVERRUNREF(p_)
#	define COUNTERINCREMENTACQUIREDREFS()
#	define COUNTERINCREMENTFREEDREFS()
#endif

namespace nwol
{
	//template<typename baseType>	
	//static				void								__gcustomDestruct						(baseType* p)											{ p->~baseType(); }
	template<typename _tRef>	void							_nwol_internal_release					(_tRef** refToRelease);

	template <typename _tRef, uint32_t _PageSizeInInstances> 
	struct SReferencePage : public CMutex
	{
		typedef				typename _tRef::TBase				_tBase;
	
		struct SInstanceEntry {
																__GPRECHECK();
							_tBase								ActualInstance;
																__GPOSTCHECK();
		};

		static constexpr	uint32_t							getPageBytes							()														{
			return (uint32_t)
				(	(sizeof(_tRef*)*_PageSizeInInstances+sizeof(_tRef)*_PageSizeInInstances)	// Stack of unused pointers and reference instance areas.
				+	::nwol::get_type_size_padded<SInstanceEntry>(BASETYPE_ALIGN) * _PageSizeInInstances
				+	BASETYPE_ALIGN*BASETYPE_ALIGN // this last one may be redundant as it may be considered already by the one_if() evaluation
				);
		}
		static				bool								isBufferOverrunInstance					(const SInstanceEntry* p)								{																								
			bool													r											= false;																				
			if (BINFIBO != p->NWOL_DEBUG_CHECK_NAME_PRE) {																							
				error_printf("Memory was overwritten before the object data. Value: %u. Expected: %u.", (uint32_t)p->NWOL_DEBUG_CHECK_NAME_PRE, (uint32_t)BINFIBO);						
				r													= true;																				
			}																							

			if (BINFIBO != p->NWOL_DEBUG_CHECK_NAME_POST) {																							
				error_printf("Memory was overwritten after the object data. Value: %u. Expected: %u.", (uint32_t)p->NWOL_DEBUG_CHECK_NAME_POST, (uint32_t)BINFIBO);						
				r													= true;																				
			}

			if( r ) 			{																							
				//printInfoString(&p->ActualInstance);													
				PLATFORM_CRT_BREAKPOINT(); 																		
			}																							
			return r;																					
		}

							::nwol::SReferenceGlobals			Globals;
							_tRef**								lstUnusedInstances						= 0;
							_tRef*								lstReferences							= 0;
							char*								Instances								= 0;
							uint32_t							UnusedInstances							= 0;
							uint32_t							UsedItems								= 0;
							char								PageBytes[getPageBytes()]				= {};
#if defined(NWOL_DEBUG_ENABLED)
		static constexpr	const uint32_t						ActualEntrySize							= sizeof(SInstanceEntry);
		static constexpr	const uint32_t						ActualSizePadded						= ::nwol::get_type_size_padded<SInstanceEntry>(BASETYPE_ALIGN);
		static constexpr	const uint32_t						ActualAlign								= ::nwol::get_type_align<SInstanceEntry>();
#endif
		// Initializes the reference and sets the reference counter to 1.
		// This function doesn't require to be called inside a critical section.
							void								initRef									(_tRef* newRef, uint32_t dataIndex)						{
			SInstanceEntry												* entry									= getInstanceEntry(dataIndex);
			newRef->Instance										= &entry->ActualInstance; 
			newRef->Globals											= &Globals;
			newRef->ReferenceCount									= 1;
#if defined(NWOL_DEBUG_ENABLED)
			newRef->Column											= dataIndex;
			entry->NWOL_DEBUG_CHECK_NAME_PRE	= entry->NWOL_DEBUG_CHECK_NAME_POST = newRef->NWOL_DEBUG_CHECK_NAME_PRE = newRef->NWOL_DEBUG_CHECK_NAME_POST = BINFIBO;
#endif
		}

																SReferencePage							(const ::nwol::SReferenceGlobals& globals )				: Globals(globals)
		{
#if defined(NWOL_DEBUG_ENABLED)
			const ::nwol::glabel										& typeName								= _tRef::get_type_name();
				
			info_printf("Loading %s table.", typeName.begin());
				 if( Globals.DataAlign == 32 )				{ info_printf("Detected align to 32 bytes: %llu."		, (uint64_t)Globals.DataAlign);	}	
			else if( Globals.DataAlign == 16 )				{ info_printf("Detected align to 16 bytes: %llu."		, (uint64_t)Globals.DataAlign);	}	
			else if( !(Globals.DataAlign % sizeof(void*)) ) { info_printf("Detected align to native size: %llu."	, (uint64_t)sizeof(void*));		}	
			else { info_printf("Type is not of an aligned size. %llu padding bytes will be added.", (uint64_t)ActualSizePadded-ActualEntrySize);	}
#else
			UNUSED const uint32_t										ActualEntrySize							= sizeof(SInstanceEntry);
			UNUSED const uint32_t										ActualSizePadded						= Globals.DataSizePadded;
			UNUSED const uint32_t										ActualAlign								= Globals.DataAlign;
#endif
			info_printf("Size of base type: %u. Entry size: %u. Size of entry padded to %u bytes: %u", (uint32_t)sizeof(_tBase), (uint32_t)ActualEntrySize, (uint32_t)BASETYPE_ALIGN, (uint32_t)ActualSizePadded);
			lstUnusedInstances										= (_tRef**)&PageBytes[0];
			lstReferences											= (_tRef*)&PageBytes[sizeof(_tRef*)*_PageSizeInInstances];
			Instances												= &PageBytes[sizeof(_tRef*)*_PageSizeInInstances+sizeof(_tRef)*_PageSizeInInstances];
#if defined(NWOL_DEBUG_ENABLED)
			Instances												= &Instances[sizeof(SInstanceEntry::NWOL_DEBUG_CHECK_NAME_PRE)];
#else
#endif
			Instances												= &Instances[calc_align_address(BASETYPE_ALIGN, &Instances)]; // = &Instances[calc_align_address(BASETYPE_ALIGN, &Instances[sizeof(SInstanceEntry::NWOL_DEBUG_CHECK_NAME_PRE)])]; // align to 16 byte boundary (skip safety check)
			//for(uint32_t i=0, count = ::nwol::size(PageBytes)/8; i< count; ++i) {
			//	((uint32_t*)Instances)[i] = ((i % 3) == 0) ? 0xFFFF00FF : (i % 2) ? 0xFF00FFFF : 0xFFFFFF00;
			//}
		}

							uint32_t							getAvailableInstanceCountNoLock			()								const noexcept			{ return UnusedInstances+(_PageSizeInInstances-UsedItems);	}
#if defined(NWOL_DEBUG_ENABLED)
							SInstanceEntry*						getInstanceEntry						(int32_t index)											{ return (SInstanceEntry*)&Instances[index*::nwol::get_type_size_padded<SInstanceEntry>(BASETYPE_ALIGN)-sizeof(SInstanceEntry::NWOL_DEBUG_CHECK_NAME_PRE)];	}
							const SInstanceEntry*				getInstanceEntry						(int32_t index)					const					{ return (SInstanceEntry*)&Instances[index*::nwol::get_type_size_padded<SInstanceEntry>(BASETYPE_ALIGN)-sizeof(SInstanceEntry::NWOL_DEBUG_CHECK_NAME_PRE)];	}
#else
							SInstanceEntry*						getInstanceEntry						(int32_t index)											{ return (SInstanceEntry*)&Instances[index*::nwol::get_type_size_padded<SInstanceEntry>(BASETYPE_ALIGN)];	}
							const SInstanceEntry*				getInstanceEntry						(int32_t index)					const					{ return (SInstanceEntry*)&Instances[index*::nwol::get_type_size_padded<SInstanceEntry>(BASETYPE_ALIGN)];	}
#endif
		// Retrieves an unused reference or initializes a new one if there is enough capacity.
							_tRef*								getUnusedInstance						()														{
			_tRef														* newRef								= 0;
			Lock();
			if(UnusedInstances > 0) {
				newRef													= lstUnusedInstances[--UnusedInstances];
				Unlock();
				//newRef->ReferenceCount++;
			}
			else if(UsedItems < _PageSizeInInstances) {
				int32_t														dataIndex								= UsedItems;
				newRef													= &lstReferences[UsedItems++];
				Unlock(); // no need for safety anymore
				initRef(newRef, dataIndex);
			}
			else
				Unlock();
			return newRef;
		}
		//	
							_tRef*								getUnusedInstanceUnsafe					()														{
			_tRef														* newRef								= 0;
			if(UnusedInstances > 0)
				newRef													= lstUnusedInstances[--UnusedInstances];
			else if(UsedItems < _PageSizeInInstances) {
				int32_t														dataIndex								= UsedItems;
				newRef													= &lstReferences[UsedItems++];
				initRef(newRef, dataIndex);
			}
			return newRef;
		}
		//
							uint32_t							getAvailableInstanceCount				()														{	
			uint32_t													r; 
			Lock();
			r														= UnusedInstances+(_PageSizeInInstances-UsedItems);
			Unlock();
			return r;
		}

	}; // struct

	template<typename _tRef>	void							_nwol_ref_release						(_tRef* refToRelease);

	// ----------------------------------------------------- gref_manager_nco
	template <typename _tRef, GREF_CATEGORY _managerType=GREF_CATEGORY_NCO> class gref_manager_nco
	{
	protected:
		typedef				typename _tRef::TBase							_tBase;
		typedef SReferencePage<_tRef, ::nwol::get_page_size<_tBase>()>		_TPage;

							const ::nwol::SReferenceGlobals					Globals;
		_tRef*																createRef_noinit						()														{																			
			_tRef																	* newRef								= getNewInstanceFromLastPageUsed	();						
			if(0 == newRef)	newRef													= getNewInstanceFromQuickAlloc	();							
			if(0 == newRef)	newRef													= getNewInstanceFromPage		();									
			if(0 == newRef)	newRef													= getNewInstanceFromNewPage		();								
			
			if(newRef) { 																		
				newRef->ReferenceCount												= 1;											
				SETREFALLOCID(newRef);													
			}																		
			else {																		
				errmsg_alloc();														
				PLATFORM_CRT_BREAKPOINT();														
			}																		
			PLATFORM_CRT_CHECK_MEMORY();
			return newRef;															
		}

	private:
		//typedef				void											(*customDestructType)					(_tBase*);
		static				const uint32_t									PageSizeInInstances						= ::nwol::get_page_size<_tBase>();

							::nwol::array_pod<_TPage*>						lstReferencePages						= {};
							CMutex											ManagerLock								= {};
							CMutex											QuickAllocLock							= {};		
							uint32_t										LastPageUsed							= INVALID_ROW;						
							uint32_t										QuickAllocCount							= 0;					
							_tRef*											QuickAllocActual[PageSizeInInstances]	= {};				
							array_view<_tRef*>								QuickAllocView							= {QuickAllocActual, PageSizeInInstances};				

		struct SCounters {					
#if defined(__WINDOWS__)
			refcount_t															CreatedRefs								= 0;		
			refcount_t															AcquiredRefs							= 0;		
			refcount_t															FreedRefs								= 0;			
#else
			refcount_t															CreatedRefs								;		
			refcount_t															AcquiredRefs							;		
			refcount_t															FreedRefs								;			
#endif
		}																	Counters;

							void											setLastPageUsed							(uint32_t pageIndex)									{
			ManagerLock.Lock();
			LastPageUsed														= pageIndex;
			ManagerLock.Unlock();
		}

		// Locks the array for getting the pointer of the page.
							_TPage*											getPage									(uint32_t nIndex)										{
			ManagerLock.Lock();
			_TPage																	* result								= lstReferencePages[nIndex];
			ManagerLock.Unlock();
			return result;
		}

		// Tries to get a pointer from the quick alloc list.
							_tRef*											getNewInstanceFromQuickAlloc			()														{
			QuickAllocLock.Lock();
			_tRef																	* result								= QuickAllocCount ? QuickAllocView[--QuickAllocCount] : 0;
			QuickAllocLock.Unlock();
			return result;
		}

		// Creates new page and retrieves an instance from it.
							_tRef*											getNewInstanceFromNewPage				()														{
			_tRef																	* newRef								= 0;
			::nwol::SReferenceGlobals												globals									= Globals;
			ManagerLock.Lock();
			uint32_t																expectedIndex = globals.Row				= (uint32_t)lstReferencePages.size();
			_TPage																	* pNewPage								= new _TPage(globals);
			if( 0 == pNewPage ) {
				errmsg_alloc();
			}
			else {
				newRef																= pNewPage->getUnusedInstanceUnsafe();
				uint32_t																actualIndex							= lstReferencePages.push_back(pNewPage);
				if (expectedIndex != actualIndex) {
					error_printf("%s", "Failed to resize reference page array.");
					newRef																= 0;
					delete(pNewPage);
				}

				LastPageUsed														= globals.Row;
			}
			ManagerLock.Unlock();
			return newRef;
		}

		// Tries to get new instance from last page used.
							_tRef*											getNewInstanceFromLastPageUsed			()														{
			_tRef																	* newRef								= 0;
			ManagerLock.Lock();
			if ((INVALID_ROW) != LastPageUsed) {
				_TPage																	* pPage									= lstReferencePages[LastPageUsed];
				ManagerLock.Unlock();
				newRef																= pPage->getUnusedInstance();
				if( 0 == newRef )
					LastPageUsed														= INVALID_ROW;
			}
			else
				ManagerLock.Unlock();
			return newRef;
		}

		// Looks for an already-allocated page with an available instance.
							_tRef*											getNewInstanceFromPage					()														{
			_tRef																	* newRef								= 0;
			for (uint32_t iPage = 0, pageCount = (uint32_t)lstReferencePages.size(); iPage < pageCount; iPage++) {
				ManagerLock.Lock();
				uint32_t																pageIndex								= (uint32_t)lstReferencePages.size() - iPage - 1;
				_TPage																	* pPage									= lstReferencePages[pageIndex];
				ManagerLock.Unlock();
				newRef																= pPage->getUnusedInstance();
				if(newRef) {
					setLastPageUsed(pPage->getAvailableInstanceCount() ? pageIndex : 0);
					break;
				}
			}
			return newRef;
		}

		// Retrieves pages with full capacity, setting the capacity and unused count to 0 in order to prevent being used by another thread.
							uint32_t										getFullCapacityPages					(_TPage** outPages, uint32_t nMaxCount)					{
			ManagerLock.Lock();
			uint32_t																mainPageCount							= (uint32_t)lstReferencePages.size();	// We don't really need more than 0xFFFFFFFF pages. If you do, then your program is mismanaging memory.
			uint32_t																iOutPage								= 0;
			for( uint32_t iMainList=0; iOutPage < nMaxCount && iMainList < mainPageCount; iMainList++ ) {
				_TPage																	* pPage									= lstReferencePages[iMainList];
				pPage->Lock();
				if( PageSizeInInstances == pPage->getAvailableInstanceCountNoLock() ) {
					uint32_t																usedItems								= (uint32_t)pPage->UsedItems;
					uint32_t																unusedCount								= (uint32_t)(pPage->UnusedInstances);
					pPage->UsedItems													= PageSizeInInstances;					// both capacities exhausted so we can unlock the page and nobody will have a reason to use it.
					pPage->UnusedInstances												= 0;									// both capacities exhausted so we can unlock the page and nobody will have a reason to use it.
					pPage->Unlock();
					_tRef																	* newRef								= nullptr;
					// drain all reference capacity
					while(usedItems < PageSizeInInstances) {
						uint32_t																dataIndex								= usedItems;
						newRef																= &pPage->lstReferences[usedItems++];
						pPage->initRef(newRef, dataIndex);
						SETREFALLOCID(newRef);
					}
					// set up already existing unused instances 
					for( uint32_t iRef=0; iRef < unusedCount; iRef++ ) {
						++(newRef = pPage->lstUnusedInstances[iRef])->ReferenceCount;
						SETREFALLOCID(newRef);
					}
					outPages[iOutPage++]												= pPage;
				}
				else
					pPage->Unlock();
			}
			ManagerLock.Unlock();
			return iOutPage;
		}

		// Push a bunch of page pointers into the page list.
							void											pushNewPages							(_TPage** lstPages, uint32_t pageCount)					{
			// Insert new pages into main list so we can finish working with it outside the risky area
			ManagerLock.Lock();
			uint32_t																pageOffset								= (uint32_t) lstReferencePages.size(); 
			lstReferencePages.resize(pageOffset+pageCount);
			for( uint32_t iMainList=pageOffset, iNewPage=0; iNewPage<pageCount; iMainList++, iNewPage++ )
				(lstReferencePages[iMainList] = lstPages[iNewPage])->Globals.Row	= iMainList;
			ManagerLock.Unlock();
		}

		// Create [pageCount] pages and initialize the references.
		// Capacity is set to 0 as it is assumed all references will be delivered to the user.
							void											createNewExhaustedPages					(_TPage** lstPages, uint32_t pageCount)					{
			for( uint32_t iPage=0; iPage<pageCount; iPage++ ) {
				_TPage																	* pPage									= new _TPage(Globals);
				pPage->UsedItems													= PageSizeInInstances;	// Capacity exhausted
				pPage->UnusedInstances												= 0;					// References exhausted
				for( uint32_t iNewRef=0; iNewRef < PageSizeInInstances; iNewRef++ )	{ // Initialize all references
					_tRef																	* newRef								= &pPage->lstReferences[iNewRef];
					pPage->initRef(newRef, iNewRef);
					SETREFALLOCID(newRef);
				}
				lstPages[iPage]														= pPage;
			}
			pushNewPages(lstPages, pageCount);
		}

		// Allocate pages of references and output references to array. 
							void											allocRefFullPage						(_tRef** lstRef, uint32_t nFullPageCount)				{
			::nwol::array_pod<_TPage*>												lstPages								(nFullPageCount);	// store here the pages we get.
			memset(&lstPages[0], 0, sizeof(_TPage*)*nFullPageCount);
		
			uint32_t																recycledPages							= getFullCapacityPages(lstPages.begin(), nFullPageCount);	// Fill list with existing unused pages.
			
			if( nFullPageCount != recycledPages )	// Create missing pages.
				createNewExhaustedPages( &lstPages.begin()[recycledPages], nFullPageCount-recycledPages);

			// finish page initialization
			for( uint32_t iNewPage=0; iNewPage<nFullPageCount; iNewPage++ ){
				_TPage																	* pPage									= lstPages.begin()[iNewPage];
				uint32_t																refOffset								= iNewPage*PageSizeInInstances;
				for( uint32_t iNewRef=0; iNewRef < PageSizeInInstances; iNewRef++ ) {
					int32_t																	outIndex								= refOffset+iNewRef;
					_tRef*																	oldObject								= lstRef[outIndex];
					lstRef[outIndex]													= &pPage->lstReferences[iNewRef];
					::nwol::release(&oldObject);
				}
			}
		}

		// Stores an unused reference into the QuickAlloc array.
							void											releaseReferenceToQuickAlloc			(_tRef* p1)												{
			QuickAllocLock.Lock();
			if( QuickAllocCount == PageSizeInInstances )
				emptyQuickAlloc();
			uint32_t																prevPos;
			if( QuickAllocCount && 
				QuickAllocView[(prevPos = QuickAllocCount-1)]->Instance <
				p1->Instance
				)
			{
				QuickAllocView[QuickAllocCount++]									= QuickAllocView[prevPos];
				QuickAllocView[prevPos]												= p1;
			}
			else
				QuickAllocView[QuickAllocCount++]									= p1;
			QuickAllocLock.Unlock();
			COUNTERINCREMENTFREEDREFS();
		}

		// Distribute contents of QuickAlloc array into pages owning the references.
							void											emptyQuickAlloc							()														{
			releaseReferenceListToPage(QuickAllocView.begin(), QuickAllocCount);
			QuickAllocCount														= 0;
		}

		// Return a bunch of unused references to their pages of origin.
							void											releaseReferenceListToPage				( _tRef** lstRef, uint32_t nReferenceCount)				{
			ManagerLock.Lock();
			_TPage																	* pPage									= 0;
			for( uint32_t iRef=0; iRef<nReferenceCount; iRef++ ) {
				_tRef																	* p0									= lstRef[iRef];
				_TPage																	* pNextPage								= lstReferencePages[p0->Globals->Row];
				if(0 == pNextPage) {
					error_printf("Pages should never be null. Reasons for it to be null can be a missing lock causing this function to access while the list is being overwritten or overwriting the vectors memory.");
					continue;
				}
				CHECKBUFFEROVERRUNINSTANCE();
				if( pPage != pNextPage ) {
					if( pPage )
						pPage->Unlock();
					pPage																= pNextPage;
					pPage->Lock();	// needs to unlock at end of loop
				}
				uint32_t																prevPos;
				if( pPage->UnusedInstances > 0 && 
					pPage->lstUnusedInstances[prevPos = pPage->UnusedInstances-1]->Instance <
					p0->Instance
					)
				{
					pPage->lstUnusedInstances[pPage->UnusedInstances++]					= pPage->lstUnusedInstances[prevPos];
					pPage->lstUnusedInstances[prevPos]									= p0;
				}
				else
					pPage->lstUnusedInstances[pPage->UnusedInstances++]					= p0;
			}
			if( pPage )
				pPage->Unlock();
			ManagerLock.Unlock();
		}

							void											setRefAllocID							(_tRef* newRef)										{																									
			newRef->AllocID														= NWOL_INTERLOCKED_INCREMENT(Counters.CreatedRefs)-1;								
#if defined(NWOL_DEBUG_ENABLED)
			if (newRef->__breakAllocID != (INVALID_ALLOC_ID) && newRef->__breakAllocID == newRef->AllocID)	 {																								
				warning_printf("Allocation break triggered.");												
				PLATFORM_CRT_BREAKPOINT();																				
			}						
#endif
		}

#if defined(NWOL_DEBUG_ENABLED)
		// Output an error message in case of detecting an invalid reference count.
		static				void											_printRefCountZero						(const _tRef* p)									{
			errmsg_refcountnull();
			ERROR_PRINTF_ALLOCID(p);
			//printInfoString(p);
		}
#endif

	public:
		static inline		gref_manager_nco<_tRef, _managerType>&			get										()													{
			static gref_manager_nco<_tRef, _managerType> managerInstance;
			return managerInstance;
		}
		inline constexpr													gref_manager_nco						(void(*_funcFreeR)(_tRef**))
			: Globals
				(	_tRef::get_type_name().begin()									// Cue
				,	_managerType													// object type (0 nco, 1 = c++ object, 2 = plain old data)
				,	_tRef::get_type_name				().size()					//
				,	::nwol::get_type_size				<_tBase>()					//
				,	::nwol::get_type_align				<_tBase>()					//
				,	::nwol::get_type_align_multiplier	<_tBase>()					
				,	::nwol::get_type_size_padded		<_tBase>(BASETYPE_ALIGN)	
				,	(uint32_t)INVALID_ROW											// row (invalid row)
				,	PageSizeInInstances										
				,	(void*)_funcFreeR												// deallocator function
				,	this
				)
		{}
																			~gref_manager_nco						()													{
			if (Counters.CreatedRefs) {	
#if defined(NWOL_DEBUG_ENABLED)
				static const ::nwol::glabel												& typeName								= _tRef::get_type_name();																									
				info_printf("Instance manager(GREF(%s)) shutting down", typeName.begin());																					
																																											
				info_printf("GREF(%s) instances created:	%llu (%llu bytes).", typeName.begin(), (uint64_t)Counters.CreatedRefs,	(uint64_t)(sizeof(_tRef)+sizeof(_TPage::SInstanceEntry))*Counters.CreatedRefs	);
				info_printf("GREF(%s) instances acquired:	%llu (%llu bytes).", typeName.begin(), (uint64_t)Counters.AcquiredRefs,	(uint64_t)(sizeof(_tRef)+sizeof(_TPage::SInstanceEntry))*Counters.AcquiredRefs	);
				info_printf("GREF(%s) instances freed:		%llu (%llu bytes).", typeName.begin(), (uint64_t)Counters.FreedRefs,	(uint64_t)(sizeof(_tRef)+sizeof(_TPage::SInstanceEntry))*Counters.FreedRefs		);
#endif
				if (Counters.CreatedRefs > Counters.FreedRefs) { 																								
					error_printf("Number of instances not released properly: %llu.", (uint64_t)Counters.CreatedRefs - Counters.FreedRefs);						
				}																																				
				else if (Counters.CreatedRefs < Counters.FreedRefs) {																							
					error_printf("Number of instances deleted but not created by this manager: %llu.", (uint64_t)Counters.FreedRefs - Counters.CreatedRefs);	
				}																																				
				else {																																			
					info_printf("No count mismatch detected.");																								
				}
			}																																					
			else if (0 != Counters.FreedRefs) {																													
				error_printf("Number of instances deleted but not created by this manager: %llu.", (uint64_t)Counters.FreedRefs - Counters.CreatedRefs);		
			}
			// Delete all pages on shutdown.
			uint32_t																pageCount								= (uint32_t)lstReferencePages.size();
			if (pageCount) {
				static const ::nwol::glabel												& typeName								= _tRef::get_type_name();
				info_printf("Preparing to deallocate %llu gcore_ref<%s> pages. (%llu bytes)", (uint64_t)pageCount, typeName.begin(), ((uint64_t)sizeof(_TPage))*pageCount );
				for (uint32_t i = 0; i < pageCount; i++) {
					if (lstReferencePages[i])
						delete(lstReferencePages[i]);
				};
				info_printf("Deallocation finished");
			}
			lstReferencePages.clear();
		}
		// Allocate a new reference.
							void											allocRef								(_tRef** p1)										{
			_tRef																	* oldObject								= (*p1);
			(*p1)																= createRef_noinit();
			::nwol::release(&oldObject);	// we do not use releaseRef() method because grelease acts as proxy for instances managed by other modules.
		}
		// Allocates a list of references at once.
							void											allocRefs								(_tRef** lstRef, uint32_t nCount)					{
			uint32_t																nRandAllocInstances						= nCount % PageSizeInInstances;	// These are the remaining instances that aren't enough to fill a page. 
			uint32_t																nFullPageCount							= nCount / PageSizeInInstances;		// The amount of pages needed for nCount allocations.
			
			// Get the remaining instances that aren't enough to fill a page. 
			for(uint32_t iRandAlloc = 0; iRandAlloc < nRandAllocInstances; iRandAlloc++)
				allocRef(&lstRef[iRandAlloc]);

			if( nFullPageCount )
				allocRefFullPage(&lstRef[nRandAllocInstances], nFullPageCount);
		}
		// Acquire a reference.
							_tRef*											acquireRef								(_tRef* p)											{
			if (p) {
#if defined(NWOL_DEBUG_ENABLED)
				if (0 == p->ReferenceCount)
					_printRefCountZero(p);
#endif

				NWOL_INTERLOCKED_INCREMENT(p->ReferenceCount);
				CHECKBUFFEROVERRUNREF(p);
				COUNTERINCREMENTACQUIREDREFS();
			}
			return p;
		}

		// Release a reference and set the pointer to null.
							void											releaseRef								(_tRef** _p)										{
			_tRef																	* p0									= *_p;
			(*_p)																= 0;

			CHECKBUFFEROVERRUNREF(p0);
			switch (NWOL_INTERLOCKED_DECREMENT(p0->ReferenceCount)) {
			default:
				break;
			case ((REFCOUNT_T)0) - 1:
#if defined(NWOL_DEBUG_ENABLED)
				_printRefCountZero(p0);
#endif
				p0->ReferenceCount													= 0;
			case 0:
				switch(_managerType) {
				case GREF_CATEGORY_NCO	: 
				case GREF_CATEGORY_OBJ	: p0->get()->~_tBase();
				default						: break;
				}
				//if(_ManagerType != GREF_CATEGORY_POD)
				//	p0->get()->~_tBase();
				releaseReferenceToQuickAlloc(p0);
			}
			PLATFORM_CRT_CHECK_MEMORY();
		}
	}; // class
}// namespace

#endif // NWOL_GREF_MANAGER_NCO_H_287346872
