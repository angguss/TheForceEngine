#include "vocAsset.h"
#include <DXL2_System/system.h>
#include <DXL2_Asset/assetSystem.h>
#include <DXL2_Archive/archive.h>
#include <DXL2_System/parser.h>
#include <DXL2_Audio/audioSystem.h>
#include <assert.h>
#include <map>
#include <algorithm>

namespace DXL2_VocAsset
{
	typedef std::map<std::string, SoundBuffer*> VocMap;
	static VocMap s_vocAssets;
	static std::vector<u8> s_buffer;
	static const char* c_defaultGob = "SOUNDS.GOB";

	bool parseVoc(SoundBuffer* voc);
	
	SoundBuffer* get(const char* name)
	{
		VocMap::iterator iVoc = s_vocAssets.find(name);
		if (iVoc != s_vocAssets.end())
		{
			return iVoc->second;
		}

		// It doesn't exist yet, try to load the font.
		if (!DXL2_AssetSystem::readAssetFromGob(c_defaultGob, name, s_buffer))
		{
			return nullptr;
		}

		SoundBuffer* voc = new SoundBuffer;
		if (!parseVoc(voc))
		{
			delete voc;
			return nullptr;
		}

		s_vocAssets[name] = voc;
		return voc;
	}

	void freeAll()
	{
		VocMap::iterator iVoc = s_vocAssets.begin();
		for (; iVoc != s_vocAssets.end(); ++iVoc)
		{
			SoundBuffer* voc = iVoc->second;
			delete[] voc->data;
			delete voc;
		}
		s_vocAssets.clear();
	}

	////////////////////////////////////////
	//////////// Internal //////////////////
	////////////////////////////////////////
	#pragma pack(push)
	#pragma pack(1)
	struct VocHeader
	{
		u8  desc[20];
		u16 datablockOffset;
		u16 version;
		u16 id;
	};

	struct VocBlockHeader
	{
		u8 blocktype;
		u8 size[3];
		u8 sr;
		u8 pack;
	};
	#pragma pack(pop)

	enum BlockType
	{
		VOC_TERMINATOR = 0,
		VOC_SOUND_DATA,
		VOC_SOUND_CONTINUE,
		VOC_SILENCE,
		VOC_MARKER,
		VOC_ASCII,
		VOC_REPEAT,
		VOC_END_REPEAT
	};

	enum VocCodec
	{
		CODEC_8BITS   = 0,
		CODEC_4BITS   = 1,
		CODEC_2_6BITS = 2,
		CODEC_2_BITS  = 3,
	};

	void continueSoundData(SoundBuffer* voc, const u8* soundData, u32 size)
	{
		voc->data = (u8*)realloc(voc->data, voc->size + size);
		memcpy(voc->data + voc->size, soundData, size);
		voc->size += size;
	}
	
	// Each sound block can have a different sampleRate but Dark Forces does not use this capability.
	// Dark Forces requires the 8 bit codec to be used.
	void addSoundData(SoundBuffer* voc, s32 sampleRate, u8 codec, const u8* soundData, u32 size)
	{
		assert(codec == CODEC_8BITS);
		voc->sampleRate = sampleRate;
		continueSoundData(voc, soundData, size);
	}
		
	void addSilence(SoundBuffer* voc, s32 sampleRate, u32 silenceLen)
	{
		// copy silence into the sound data.
		voc->data = (u8*)realloc(voc->data, voc->size + silenceLen);
		memset(voc->data + voc->size, 128, silenceLen);
		voc->size += silenceLen;
	}

	void addMarker(SoundBuffer* voc, u16 markerId)
	{
		// Ignored by Dark Forces.
	}

	// Dark Forces VOC files only have zero or one looping section.
	void loopStart(SoundBuffer* voc, u16 repeatCount)
	{
		voc->loopStart = voc->size;
		voc->flags |= SBUFFER_FLAG_LOOPING;
		// repeatCount is ignored in Dark Forces.
	}

	void loopEnd(SoundBuffer* voc)
	{
		voc->loopEnd = voc->size;
	}

	bool parseVoc(SoundBuffer* voc)
	{
		if (s_buffer.empty() || !voc) { return false; }

		const size_t len = s_buffer.size();
		const u8* buffer = s_buffer.data();
		const u8* end = buffer + len;
		memset(voc, 0, sizeof(SoundBuffer));

		// Read the header.
		VocHeader* header = (VocHeader*)buffer;
		buffer += sizeof(VocHeader);

		// Parse blocks.
		buffer = s_buffer.data() + header->datablockOffset;
		while (buffer < end)
		{
			const BlockType type = BlockType(*buffer); buffer++;
			// Break if this is the final block.
			if (type == VOC_TERMINATOR) { break; }
			// All other blocks have a 3 byte size (up to 16MB).
			const u32 len = buffer[0] | (buffer[1] << 8u) | (buffer[2] << 16u);
			buffer += 3;
			// Block parsing.
			switch (type)
			{
				case VOC_SOUND_DATA:
				{
					const s32 sampleRate = 1000000 / (256 - (s32)buffer[0]);
					const u8  codec = buffer[1];
					const u8* soundData = &buffer[2];
					addSoundData(voc, sampleRate, codec, soundData, len - 2);
				} break;
				case VOC_SOUND_CONTINUE:
				{
					const u8* soundData = &buffer[2];
					continueSoundData(voc, soundData, len);
				} break;
				case VOC_SILENCE:
				{
					const u16 silenceLen = *((u16*)buffer);
					const s32 sampleRate = 1000000 / (256 - (s32)buffer[2]);
					addSilence(voc, sampleRate, silenceLen);
				} break;
				case VOC_MARKER:
				{
					const u16 markerId = *((u16*)buffer);
					addMarker(voc, markerId);
				} break;
				case VOC_ASCII:
				{
					// The string is ignored.
				} break;
				case VOC_REPEAT:
				{
					const u16 repeatCount = *((u16*)buffer);
					loopStart(voc, repeatCount);
				} break;
				case VOC_END_REPEAT:
				{
					loopEnd(voc);
				} break;
				default:
					assert(0);
			};
			// Move to the next block.
			buffer += len;
		}

		if (!(voc->flags & SBUFFER_FLAG_LOOPING))
		{
			voc->loopStart = 0;
			voc->loopEnd = voc->size;
		}

		return true;
	}
}