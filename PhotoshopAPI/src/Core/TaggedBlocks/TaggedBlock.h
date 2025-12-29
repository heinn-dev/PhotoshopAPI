#pragma once

#include "Macros.h"
#include "Util/Enum.h"
#include "PhotoshopFile/FileHeader.h"
#include "PhotoshopFile/LayerAndMaskInformation.h"
#include "Core/Struct/File.h"
#include "Core/Struct/Signature.h"
#include "Util/Logger.h"

#include "Core/FileIO/Write.h"
#include "Core/FileIO/Util.h"

#include <memory>
#include <variant>

PSAPI_NAMESPACE_BEGIN


// Generic Tagged Block which does not hold any data. If you wish to parse further tagged blocks extend this struct and add an implementation
struct TaggedBlock
{
	Signature m_Signature;
	uint64_t m_Offset = 0u;	// Demarkates the start of the taggedblock, not the start of the data
	std::variant<uint32_t, uint64_t> m_Length;

	Enum::TaggedBlockKey getKey() const noexcept{ return m_Key; };

	virtual ~TaggedBlock() = default;
	TaggedBlock() = default;

	// Read a TaggedBlock from a file
	void read(File& document, const FileHeader& header, const uint64_t offset, const Signature signature, const Enum::TaggedBlockKey key, const uint16_t padding = 1u);
	virtual void write(File& document, [[maybe_unused]] const FileHeader& header, [[maybe_unused]] ProgressCallback& callback, [[maybe_unused]] const uint16_t padding = 1u);
protected:
	Enum::TaggedBlockKey m_Key = Enum::TaggedBlockKey::Unknown;
};

struct PassthroughTaggedBlock : public TaggedBlock
{
    std::vector<uint8_t> m_Data;
    std::string m_OriginalKey; // Store "Curv" explicitly since the Enum doesn't know it

    PassthroughTaggedBlock(const std::string& key, std::vector<uint8_t> data) 
        : m_Data(std::move(data)), m_OriginalKey(key)
    {
        m_Key = Enum::TaggedBlockKey::Unknown;
        // Calculate the total size (Key + Length + Data + Padding) can be complex, 
        // but for writing raw data we mainly care about the payload size.
    }

    void write(File& document, [[maybe_unused]] const FileHeader& header, [[maybe_unused]] ProgressCallback& callback, [[maybe_unused]] const uint16_t padding = 1u) override
    {
        // 1. Write Signature (8BIM)
        document.write(m_Signature.m_Value);

        // 2. Write the Original Key (e.g. "Curv")
        // We bypass the Enum conversion that was causing the crash
        if (m_OriginalKey.size() != 4) {
             PSAPI_LOG_ERROR("PassthroughBlock", "Invalid key length for passthrough block");
             return; 
        }
        document.write(m_OriginalKey);

        // 3. Write Length
        // PSD requires padding the data to a multiple of 4 bytes
        uint64_t dataSize = m_Data.size();
        WriteBinaryData::write(document, static_cast<uint32_t>(dataSize)); // Write as 32-bit int

        // 4. Write Data
        document.write(m_Data);

        // 5. Write Padding
        // The standard padding in TaggedBlock::write is often just 1 byte to even, 
        // but 'Additional Layer Info' blocks specifically align to 4 bytes.
        // We calculate needed padding to reach 4-byte alignment.
        uint32_t paddingBytes = (4 - (dataSize % 4)) % 4;
        for (uint32_t i = 0; i < paddingBytes; ++i)
        {
            uint8_t pad = 0;
            document.write(pad);
        }
    }
};

PSAPI_NAMESPACE_END