#pragma once

#include "Macros.h"
#include "Layer.h"
#include "PhotoshopFile/LayerAndMaskInformation.h"
#include "Core/TaggedBlocks/TaggedBlock.h"
#include "Core/TaggedBlocks/TaggedBlockStorage.h"
#include "LayeredFile/concepts.h"

#include <vector>
#include <memory>

PSAPI_NAMESPACE_BEGIN

/// Represents an Adjustment Layer (Curves, Levels, etc.)
/// The adjustment data is preserved via PassthroughTaggedBlocks stored in the base Layer class.
template <typename T>
	requires concepts::bit_depth<T>
struct AdjustmentLayer : Layer<T>
{

	AdjustmentLayer() = default;

	AdjustmentLayer(const LayerRecord& layerRecord, ChannelImageData& channelImageData, const FileHeader& header)
        : Layer<T>(layerRecord, channelImageData, header)
    {
        // No specific parsing logic needed here yet,
        // as we are relying on the base class to hold the data (including passthrough blocks).
    }

	/// Generate the PhotoshopFile-compatible layer data for writing.
	///
	/// This override is necessary because the base Layer::to_photoshop() doesn't properly handle:
	/// 1. Layer masks (passes std::nullopt)
	/// 2. Empty placeholder channels required by some applications
	///
	/// AdjustmentLayers are similar to GroupLayers in structure - they don't have image data
	/// but may have masks and need placeholder channels for compatibility.
	std::tuple<LayerRecord, ChannelImageData> to_photoshop() override
	{
		PascalString lrName = Layer<T>::generate_name();
		ChannelExtents extents = generate_extents(ChannelCoordinates(Layer<T>::m_Width, Layer<T>::m_Height, Layer<T>::m_CenterX, Layer<T>::m_CenterY));
		LayerRecords::BitFlags bitFlags = LayerRecords::BitFlags(Layer<T>::m_IsLocked, !Layer<T>::m_IsVisible, false);
		std::optional<LayerRecords::LayerMaskData> lrMaskData = Layer<T>::internal_generate_mask_data();
		LayerRecords::LayerBlendingRanges blendingRanges = Layer<T>::generate_blending_ranges();

		// Initialize channel vectors
		std::vector<LayerRecords::ChannelInformation> channelInfoVec;
		std::vector<std::unique_ptr<channel_wrapper>> channelDataVec;

		// Extract mask data if present (this was being lost in the base implementation)
		auto maskData = Layer<T>::internal_extract_mask();
		if (maskData.has_value())
		{
			channelInfoVec.push_back(std::get<0>(maskData.value()));
			channelDataVec.push_back(std::move(std::get<1>(maskData.value())));
		}

		// Generate tagged blocks (includes the passthrough adjustment data like Curves, Levels, etc.)
		auto blockVec = this->generate_tagged_blocks();
		std::optional<AdditionalLayerInfo> taggedBlocks = std::nullopt;
		if (blockVec.size() > 0)
		{
			TaggedBlockStorage blockStorage = { blockVec };
			taggedBlocks.emplace(blockStorage);
		}

		// Generate empty placeholder channels for compatibility with applications like Krita
		Layer<T>::generate_empty_channels(channelInfoVec, channelDataVec, Layer<T>::m_ColorMode);

		LayerRecord lrRecord = LayerRecord(
			lrName,
			extents.top,
			extents.left,
			extents.bottom,
			extents.right,
			static_cast<uint16_t>(channelInfoVec.size()),
			channelInfoVec,
			Layer<T>::m_BlendMode,
			Layer<T>::m_Opacity,
			static_cast<uint8_t>(Layer<T>::m_IsClippingMask),
			bitFlags,
			lrMaskData,
			blendingRanges,
			std::move(taggedBlocks)
		);
		return std::make_tuple(std::move(lrRecord), ChannelImageData(std::move(channelDataVec)));
	}
};

extern template struct AdjustmentLayer<bpp8_t>;
extern template struct AdjustmentLayer<bpp16_t>;
extern template struct AdjustmentLayer<bpp32_t>;

PSAPI_NAMESPACE_END