#pragma once

#include "TaggedBlock.h"
#include "Macros.h"
#include "Util/Enum.h"

#include <vector>

PSAPI_NAMESPACE_BEGIN

/// \brief The Layer ID tagged block, key: 'lyid'.
///
/// This block contains a unique ID for the layer.
struct LayerIdTaggedBlock : public TaggedBlock
{
	/// The ID of the layer
	uint32_t m_LayerId = 0u;

	LayerIdTaggedBlock() = default;
	LayerIdTaggedBlock(uint32_t layerId);

	void read(File& document, const uint64_t offset, const Signature signature);
	void write(File& document, const FileHeader& header, ProgressCallback& callback, const uint16_t padding = 1u) override;
};

PSAPI_NAMESPACE_END
