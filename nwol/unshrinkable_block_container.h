#include "nwol_array.h"
#include "multithread.h"
#include "nwol_error.h"

#include <memory>

#ifndef __UNSHRIHNKABLE_BLOCK_CONTAINER_H__65198749648169418__
#define __UNSHRIHNKABLE_BLOCK_CONTAINER_H__65198749648169418__

namespace nwol
{
#pragma pack(push, 1)
	// Absolute coordinates of a stored item 
	struct SBlockCoord {
		uint32_t										BlockIndex;
		uint32_t										DataOffset;
	};

	template<typename _tBase, uint32_t _BlockSize>	struct	SDataBlock	{ _tBase	Data[_BlockSize]	= {}; };	// Base for templated data blocks.
#pragma pack(pop)

	template<typename _tBase, uint32_t _BlockSize, bool _bTrailingNull>
	class block_container_base {
	protected:
		typedef ::nwol::SDataBlock<_tBase, _BlockSize>	TBlock;	// Better to alias this because we're gonna need to reference it later
		typedef nwol::array_pod<TBlock*>				TBlockArray;	// Better to alias this because we're gonna need to reference it later

		TBlockArray										BlockArray;	/// Store here the data blocks.
		nwol::array_pod<uint32_t>						RemainingBlockSpace;

														~block_container_base		()																																	{
			for(uint32_t iBlock=0, count = BlockArray.size(); iBlock < count; iBlock++) {
				TBlock												* p							= BlockArray[iBlock];
				if(p)
					::nwol::nwol_free(p);
			}
		}

		// Return block index and data offset from abstract index.
		nwol::error_t									getDataCoordsFromDataIndex	(uint32_t dataIndex, SBlockCoord& output)																							{
			
			if(0 == BlockArray.size()) {	// if the container is empty, all indices are invalid indices.
				error_printf("Invalid data index!");
				output											= {0, 0};
				return dataIndex;
			}
			
			const SBlockCoord									result						= { dataIndex / _BlockSize, dataIndex % _BlockSize };
			if(result.BlockIndex >= BlockArray.size() || result.DataOffset >= (_BlockSize - RemainingBlockSpace[result.BlockIndex])) {
				error_printf("Index out of boundaries! Index: %i. Block index: %i. Data offset: %i. \n"
					"Max block index: %u. Max data offset: %u."
					, dataIndex
					, result.BlockIndex
					, result.DataOffset
					, (BlockArray.size()-1)
					, (_BlockSize - RemainingBlockSpace[BlockArray.size()-1])
					);
				output											= {0, 0};
				return dataIndex | 0x80000000;
			}

			output = result;
			return 0;
		}

		// Copy the data into the block, caulculate the item range and return the item index.
		// Warning! Members need to be locked for this to be ok.
		nwol::error_t									pushSanitizedData			(const _tBase* data, uint32_t dataElementCount, ::nwol::array_view<const _tBase>& arrayView, SBlockCoord& dataCoords)		{

			bool												blockFound					= false;		// Look for a suitable block.
			const uint32_t										dataFinalElementCount		= dataElementCount + one_if(_bTrailingNull);

			for( uint32_t iBlock = 0, blockCount = BlockArray.size(); iBlock < blockCount; iBlock++ ) {
				if( RemainingBlockSpace[iBlock] >= dataFinalElementCount) {
					dataCoords.DataOffset							= _BlockSize - RemainingBlockSpace[iBlock];
					dataCoords.BlockIndex							= (uint32_t)iBlock; 
					blockFound										= true;
					break;
				}
			}

			if(!blockFound) {	// we need to create a new block
				// We assume the block array has enough size now, so create the new block and set its members.
				TBlock												* newBlock					= (TBlock*)::nwol::nwol_malloc(sizeof(TBlock));
				reterr_error_if(0 == newBlock, "Failed to allocate new data block. Out of memory?");
				//memset(newBlock, 0, sizeof(SBlock));

				const uint32_t										expectedBlockIndex			= RemainingBlockSpace.size();
				const int32_t										remainingSpaceCountIndex	= RemainingBlockSpace.push_back(_BlockSize);

				::nwol::error_t										failed						= expectedBlockIndex != (uint32_t)remainingSpaceCountIndex;
				if(0 == failed) {
					dataCoords.BlockIndex							= (uint32_t)BlockArray.push_back(newBlock);
					if(expectedBlockIndex != (uint32_t)dataCoords.BlockIndex) 
						failed											= -1;
				}
				if(failed) {
					::nwol::nwol_free(newBlock);
					error_printf("Failed to push new data block. Out of memory?");
					return -1;
				}

				// if(expectedBlockIndex != remainingSpaceCountIndex) {
				// 	::nwol::nwol_free(newBlock);
				// 	error_printf("Failed to push new counter for data block. Out of memory?");
				// 	return -1;
				// }
				// 
				// dataCoords.BlockIndex				= (uint32_t)BlockArray.push_back(newBlock);
				// if(expectedBlockIndex != dataCoords.BlockIndex) {
				// 	::nwol::nwol_free(newBlock);
				// 	error_printf("Failed to push new data block. Out of memory?");
				// 	return -1;
				// }

				dataCoords.DataOffset								= 0;
			}

			RemainingBlockSpace[dataCoords.BlockIndex]			-= dataFinalElementCount;
			_tBase													* newPlace					= &BlockArray[dataCoords.BlockIndex]->Data[dataCoords.DataOffset];

			// Copy the user data into the container pointed by newPlace.
			memcpy(newPlace, data, dataElementCount*sizeof(_tBase));
			if(_bTrailingNull)
				memset(&newPlace[dataElementCount], 0, sizeof(_tBase));

			arrayView											= nwol::array_view<const _tBase>(newPlace, dataElementCount);
			return 0;
		}
	};	// class

	template<typename _tBase, uint32_t _BlockInstanceCount>
	class unordered_pod_set : public block_container_base<_tBase, _BlockInstanceCount, false> {
		typedef block_container_base<_tBase, _BlockInstanceCount, false>	container_base;
		typedef ::nwol::SDataBlock<_tBase, _BlockInstanceCount>				TBlock;			// Better to alias this because we're gonna need to reference it later
		typedef ::nwol::array_pod<TBlock*>									TBlockArray;	// Better to alias this because we're gonna need to reference it later

	public:
														unordered_pod_set			()																																	{ (void)INIT_CRITICAL_SECTION(Lock); }

		using	container_base::						BlockArray					;
		using	container_base::						getDataCoordsFromDataIndex	;
		// Returns an array view of a given item.
		const _tBase*									get							(const int32_t itemIndex)																							const			{
			// return nullptr in case of failure
			ENTER_CRITICAL_SECTION(Lock);
			SBlockCoord											dataCoords;
			::nwol::error_t										result						= getDataCoordsFromDataIndex(itemIndex, dataCoords);
			if errored(result) {
				LEAVE_CRITICAL_SECTION(Lock);
				error_printf("Failed to get data coordinates! Invalid index?");
				return result;
			}
			result											= &BlockArray.pData[dataCoords.BlockIndex]->Data[dataCoords.DataOffset];
			LEAVE_CRITICAL_SECTION(Lock);
			return result;
		}

		_tBase*											get							(const int32_t itemIndex)																											{
			// return nullptr in case of failure		
			ENTER_CRITICAL_SECTION(Lock);	
			SBlockCoord											dataCoords;
			::nwol::error_t										result						= getDataCoordsFromDataIndex(itemIndex, dataCoords);
			if errored(result) {
				LEAVE_CRITICAL_SECTION(Lock);
				error_printf("Failed to get data coordinates! Invalid index?");
				return result;
			}
			result											= &BlockArray.pData[dataCoords.BlockIndex]->Data[dataCoords.DataOffset];
			LEAVE_CRITICAL_SECTION(Lock);
			return result;
		}

	};	// class

	template<typename _tBase, uint32_t _BlockSize>
	// the actual name for this would be unshrinkable block container for null-terminated data.
	class unordered_string_set : public block_container_base<_tBase, _BlockSize, true> {

		typedef	block_container_base<_tBase, _BlockSize, true>			container_base;
		typedef	::nwol::SDataBlock<_tBase, _BlockSize>					TBlock;	// Better to alias this because we're gonna need to reference it later

		using	container_base::						pushSanitizedData;
		using	container_base::						BlockArray;
		using	container_base::						RemainingBlockSpace;
		using	container_base::						getDataCoordsFromDataIndex;

		DECLARE_CRITICAL_SECTION						(Lock);
		::nwol::array_pod<uint32_t>						ItemOffsets					;
		::nwol::array_pod<uint32_t>						ItemSizes					;
		bool											bInitialized				= false;
		

		// Push the item range into the items array. Warning! Items array needs to be locked for this to be ok.
		::nwol::error_t									pushItemRange				(uint32_t itemOffset, uint32_t itemSize, int32_t* itemIndex)																		{
			if(itemIndex)
				*itemIndex										= -1;
			const int32_t										newItemIndex				= ItemOffsets	.push_back(itemOffset);	// This is for the user.
			const int32_t										newSizesIndex				= ItemSizes		.push_back(itemSize); 
			reterr_error_if(newItemIndex != newSizesIndex, "%s", "Indices don't match. newItemIndex: %u, newSizesIndex: %u.", newItemIndex, newSizesIndex);
			if(itemIndex)
				*itemIndex										= (int32_t)newItemIndex;
			return newItemIndex;
		}

		::nwol::error_t									init						()																																	{
			if( bInitialized )
				return 0;
			const int32_t										newItemIndex				= pushItemRange(0, 0, 0);						reterr_error_if_errored(newItemIndex, "%s", "Failed to allocate item array!");
			TBlock												*firstBlock					= (TBlock*)::nwol::nwol_malloc(sizeof(TBlock));	reterr_error_if(0 == firstBlock, "%s", "Failed to allocate initial block! Out of memory?");
			firstBlock->Data[0]								= (_tBase)0;
			const int32_t										blockIndex					= this->BlockArray			.push_back(firstBlock);
			const int32_t										blockSpaceIndex				= this->RemainingBlockSpace	.push_back(_BlockSize - 1);
			if errored(blockIndex		) { error_printf("%s", "Failed to allocate array!"); this->RemainingBlockSpace	.clear(); return -1; }
			if errored(blockSpaceIndex	) { error_printf("%s", "Failed to allocate array!"); this->BlockArray			.clear(); return -1; }
			bInitialized						= true;
			return 0;
		}
	public:
														unordered_string_set		()																																	: bInitialized(false)	{ (void)INIT_CRITICAL_SECTION(Lock); }
														~unordered_string_set		()																																	{
#if defined(NWOL_DEBUG_ENABLED)
			uint64_t											totalChars					= 0;
			for( uint32_t iItem=1, count=ItemOffsets.size(); iItem<count; iItem++) {
				const ::nwol::array_view<const _tBase>	arrayView					= this->get_array_view((int32_t)iItem);
				info_printf("Data string found. Data: '%s'. Offset: %u. Size: %u.", arrayView.begin(), ItemOffsets[iItem], (uint32_t)arrayView.size());
				totalChars										+= arrayView.size();
			}
			if(ItemOffsets.size()) {
				info_printf("Total strings: %u. Total characters: %llu. Average string size: %u.", (uint32_t)ItemOffsets.size(), totalChars, ItemOffsets.size() ? (uint32_t)(totalChars/ItemOffsets.size()) : 0);
			}
#endif
			DELETE_CRITICAL_SECTION(Lock);
		}

		// Returns an array view of a given item.
		array_view<const _tBase>						get_array_view				(const int32_t itemIndex)																											{
			ENTER_CRITICAL_SECTION(Lock);
			init();
			// return empty string in case of failure
			::nwol::array_view<const _tBase>					result						= ::nwol::array_view<const _tBase>(&this->BlockArray[0]->Data[0], 0);

			if( itemIndex >= (int32_t)ItemOffsets.size() ) {
				LEAVE_CRITICAL_SECTION(Lock);
				error_printf("Item index out of boundaries! Index: %i.", itemIndex);
				return result;
			}

			if( 0 > itemIndex ) {
				LEAVE_CRITICAL_SECTION(Lock);
				error_printf("Invalid item index! Index: %i.", itemIndex);
				return result;
			}
			const int32_t										dataIndex					= ItemOffsets	[itemIndex];
			const int32_t										dataLength					= ItemSizes		[itemIndex];
			SBlockCoord											dataCoords;
			if( 0 > this->getDataCoordsFromDataIndex(dataIndex, dataCoords) ) {
				LEAVE_CRITICAL_SECTION(Lock);
				error_printf("Failed to get data coordinates!");
				return result;
			}
			result											= ::nwol::array_view<const _tBase>(&this->BlockArray[dataCoords.BlockIndex]->Data[dataCoords.DataOffset], dataLength);
			LEAVE_CRITICAL_SECTION(Lock);
			return result;
		}

		// Pushes an item into the container.
		::nwol::error_t									push_back					(const _tBase* data, uint32_t readCount, int32_t* itemIndex, ::nwol::array_view<const _tBase>& arrayView)									{
			reterr_error_if(0 == data, "Cannot read from null address.");

			if(itemIndex)
				*itemIndex										= 0;	// return item 0 for the next two if statements.

			ENTER_CRITICAL_SECTION(Lock);
			init();
			arrayView										= ::nwol::array_view<const _tBase>(&this->BlockArray[0]->Data[0], 0);	// return empty string in case of failure

			// return success on storing empty string.
			if(0 == readCount || 0 == data[0]) {
				LEAVE_CRITICAL_SECTION(Lock);
				return 0;
			}

			if(itemIndex)
				*itemIndex										= -1;	// return invalid index from now on so in case of failure we can detect it.

			uint32_t											textLen						= 0;	// Count string characters
			while(textLen < readCount && 0 != data[textLen]) 
				++textLen;

			if(textLen >= _BlockSize) {
				error_printf("Data too long! Size requested: %u. Storage block size: %u. Truncating...", textLen, (uint32_t)_BlockSize);
				textLen											= _BlockSize-1;
			}

			SBlockCoord											dataCoords					= {};
			for(uint32_t iItem=0, itemCount = ItemSizes.size(); iItem<itemCount; ++iItem) {
				if(ItemSizes[iItem] != textLen)
					continue;

				this->getDataCoordsFromDataIndex(ItemOffsets[iItem], dataCoords);
				if(0 == memcmp(&this->BlockArray[dataCoords.BlockIndex]->Data[dataCoords.DataOffset], data, textLen*sizeof(_tBase))) {
					arrayView										= ::nwol::array_view<const _tBase>(&this->BlockArray[dataCoords.BlockIndex]->Data[dataCoords.DataOffset], textLen);
					verbose_printf("Duplicated data string found. Data: %s. Size: %u. Recycling..."
						, arrayView.begin()
						, arrayView.size()
					);
					LEAVE_CRITICAL_SECTION(Lock);
					if(itemIndex)
						*itemIndex										= (int32_t)iItem;
					return iItem;
				}
			}
			::nwol::error_t										errMy						= this->pushSanitizedData(data, textLen, arrayView, dataCoords);
			int32_t												newItemIndex				= -1;
			if(0 > errMy) {
				error_printf("Failed to push data to container!");
			}
			else {
				verbose_printf("Registered new data string: %s. Character count: %u.", arrayView.begin(), arrayView.size());
				// Calculate the virtual range and push the range to the item list.
				const int32_t										newItemOffset				= dataCoords.BlockIndex * _BlockSize + dataCoords.DataOffset;
				newItemIndex									= pushItemRange(newItemOffset, textLen, itemIndex);
				if( 0 > newItemIndex ) {
					error_printf("%s", "Failed to push new item range!");
					errMy											= -1;
				}

			}
			LEAVE_CRITICAL_SECTION(Lock);

			return (0 > errMy) ? -1 : newItemIndex;
		}
	};	// class
}	// namespace

#endif // __GUNSHRIHNKABLE_BLOCK_CONTAINER_H__65198749648169418__
