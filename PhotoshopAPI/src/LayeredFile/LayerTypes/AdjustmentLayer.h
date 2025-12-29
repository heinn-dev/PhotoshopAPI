#pragma once

#include "Macros.h"
#include "Layer.h"
#include "LayeredFile/concepts.h"

PSAPI_NAMESPACE_BEGIN

/// This struct holds no data, we just use it to identify its type
/// This will probably be split into multiple files later on
template <typename T>
	requires concepts::bit_depth<T>
struct AdjustmentLayer : Layer<T>
{

	AdjustmentLayer() = default;

	AdjustmentLayer(const LayerRecord& layerRecord, ChannelImageData& channelImageData, const FileHeader& header) 
        : Layer<T>(layerRecord, channelImageData, header)
    {
        // No specific parsing logic needed here yet, 
        // as we are relying on the base class to hold the data.
    }
};

extern template struct AdjustmentLayer<bpp8_t>;
extern template struct AdjustmentLayer<bpp16_t>;
extern template struct AdjustmentLayer<bpp32_t>;

PSAPI_NAMESPACE_END