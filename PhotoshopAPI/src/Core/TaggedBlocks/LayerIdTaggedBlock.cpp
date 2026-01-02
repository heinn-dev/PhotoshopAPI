#include "LayerIdTaggedBlock.h"

#include "Core/FileIO/Read.h"
#include "Core/FileIO/Write.h"
#include "Core/FileIO/Util.h"
#include "Core/FileIO/LengthMarkers.h"

PSAPI_NAMESPACE_BEGIN

LayerIdTaggedBlock::LayerIdTaggedBlock(uint32_t layerId) : m_LayerId(layerId)
{
	m_Key = Enum::TaggedBlockKey::lrId;
}

void LayerIdTaggedBlock::read(File& document, const uint64_t offset, const Signature signature)
{
	m_Key = Enum::TaggedBlockKey::lrId;
	m_Offset = offset;
	m_Signature = signature;

	uint32_t length = ReadBinaryData<uint32_t>(document);
	// The length should always be 4 for this block
	if (length != 4u)
	{
		PSAPI_LOG_ERROR("LayerIdTaggedBlock", "Expected length of 4 for Layer ID block, got %u", length);
	}
	m_Length = length;

	m_LayerId = ReadBinaryData<uint32_t>(document);
}

void LayerIdTaggedBlock::write(File& document, const FileHeader& header, ProgressCallback& callback, const uint16_t padding)
{
	WriteBinaryData<uint32_t>(document, Signature("8BIM").m_Value);
	WriteBinaryData<uint32_t>(document, Signature("lyid").m_Value);

	// This will write the length marker (should end up being 4) and handle padding if necessary
	Impl::ScopedLengthBlock<uint32_t> len_block(document, padding);

	WriteBinaryData<uint32_t>(document, m_LayerId);
}

PSAPI_NAMESPACE_END
