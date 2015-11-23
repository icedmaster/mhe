#include "res/animation_loader.hpp"

#include "utils/global_log.hpp"

namespace mhe {

bool AnimationLoader::load(type& res, const std::string& name, const context_type* context)
{
	std::ifstream stream(name.c_str(), std::ios::binary);
	if (!stream.is_open())
	{
		ERROR_LOG("Can't open file:" << name);
		return false;
	}

	char header[3];
	stream.read(header, sizeof(header));
	if (strncmp(header, "mhe", 3))
	{
		DEBUG_LOG("AnimationLoader - header is invalid");
		stream.close();
		return false;
	}
	char version;
	stream.read(&version, 1);
	if (version != 1)
	{
		DEBUG_LOG("AnimationLoader - version is invalid");
		stream.close();
		return false;
	}

	AnimationData& data = res;

	stream.read(reinterpret_cast<char*>(&data.duration), sizeof(float));
	stream.read(reinterpret_cast<char*>(&data.fps), sizeof(float));
	char play_mode;
	stream.read(&play_mode, 1);
	data.play_mode = play_mode;
	uint32_t frames_number;
	stream.read(reinterpret_cast<char*>(&frames_number), sizeof(uint32_t));
	data.frames.resize(frames_number);
	for (size_t i = 0; i < frames_number; ++i)
	{
		NodeAnimationFrame& frame = data.frames[i];
		stream.read(reinterpret_cast<char*>(&frame.node_id), sizeof(uint32_t));
		uint32_t number;
		stream.read(reinterpret_cast<char*>(&number), sizeof(uint32_t));
		frame.positions.resize(number);
		stream.read(reinterpret_cast<char*>(&frame.positions[0]), sizeof(PositionAnimationFrame) * number);
		stream.read(reinterpret_cast<char*>(&number), sizeof(uint32_t));
		frame.rotations.resize(number);
		stream.read(reinterpret_cast<char*>(&frame.rotations[0]), sizeof(RotationAnimationFrame) * number);
		stream.read(reinterpret_cast<char*>(&number), sizeof(uint32_t));
		frame.scalings.resize(number);
		stream.read(reinterpret_cast<char*>(&frame.scalings[0]), sizeof(ScaleAnimationFrame) * number);
	}

	stream.close();

	return true;
}

}
