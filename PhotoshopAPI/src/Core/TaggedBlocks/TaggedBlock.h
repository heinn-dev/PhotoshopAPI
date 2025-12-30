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

/// A generic block to hold data for keys we don't fully support yet (e.g. Curv, Levl)
struct PassthroughTaggedBlock : public TaggedBlock
{
    std::vector<uint8_t> m_Data;
    std::string m_OriginalKey; 

    PassthroughTaggedBlock(const std::string& key, std::vector<uint8_t> data, Enum::TaggedBlockKey enumKey = Enum::TaggedBlockKey::Unknown)
        : m_Data(std::move(data)), m_OriginalKey(key)
    {
        m_Key = enumKey;
    }

	void write(File& document, const FileHeader& header, [[maybe_unused]] ProgressCallback& callback, [[maybe_unused]] const uint16_t padding = 1u) override
    {
        // 1. Signature
        WriteBinaryData<uint32_t>(document, m_Signature.m_Value);

        // 2. Key
        if (m_OriginalKey.size() != 4) {
             PSAPI_LOG_ERROR("PassthroughBlock", "Invalid key length");
             return;
        }
        for (char c : m_OriginalKey) {
            WriteBinaryData<uint8_t>(document, static_cast<uint8_t>(c));
        }

        // 3. Length
        uint64_t dataSize = m_Data.size();
        if (Enum::isTaggedBlockSizeUint64(m_Key) && header.m_Version == Enum::Version::Psb)
        {
            WriteBinaryData<uint64_t>(document, dataSize);
        }
        else
        {
            WriteBinaryData<uint32_t>(document, static_cast<uint32_t>(dataSize));
        }

        // 4. Data
        document.write(m_Data);

        // 5. Padding (Dynamic Fix)
        // Use the 'padding' argument just like the Reader does!
        if (padding > 0)
        {
            uint32_t paddingBytes = (padding - (dataSize % padding)) % padding;
            for (uint32_t i = 0; i < paddingBytes; ++i)
            {
                WriteBinaryData<uint8_t>(document, 0u);
            }
        }
    }
};

PSAPI_NAMESPACE_END