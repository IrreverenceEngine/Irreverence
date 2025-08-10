#include <IR_Audio.hpp>

namespace IR::Audio {

	constexpr unsigned int WAVE_HEADER_RIFF = 0x52494646; // "RIFF"
	constexpr unsigned int WAVE_HEADER_WAVE = 0x57415645; // "WAVE"

	struct WAVHeader {
		char id[4];
		unsigned int size;
		void* data;
	};

	void* LoadWAV(const void* data, Int64 size, SoundFormat& soundData)
	{
		// TODO: Implement.
		return nullptr;
	}

}
