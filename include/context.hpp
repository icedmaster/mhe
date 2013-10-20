#ifndef __CONTEXT_HPP__
#define __CONTEXT_HPP__

#include "video_driver.hpp"
#include "window_system.hpp"
#include "texture_manager.hpp"
#include "texture_atlas_manager.hpp"
#include "material_manager.hpp"
#include "shader_manager.hpp"

namespace mhe {

class Context
{
public:
	Context()
	{
		texture_manager_.set_helper(&driver_);
		texture_atlas_manager_.set_helper(&texture_manager_);
		material_manager_.set_helper(this);
		shader_manager_.set_helper(this);
	}

	const Driver& driver() const
	{
		return driver_;
	}

	Driver& driver()
	{
		return driver_;
	}

	void set_window_system(WindowSystem* window_system)
	{
		window_system_ = window_system;
	}

	const WindowSystem& window_system() const
	{
		return *window_system_;
	}
    
    WindowSystem& window_system()
    {
        return *window_system_;
    }

	const TextureManager& texture_manager() const
	{
		return texture_manager_;
	}
    
    TextureManager& texture_manager()
    {
        return texture_manager_;
    }

	const TextureAtlasManager& texture_atlas_manager() const
	{
		return texture_atlas_manager_;
	}

	MaterialManager& material_manager()
	{
		return material_manager_;
	}

	const MaterialManager& material_manager() const
	{
		return material_manager_;
	}

	ShaderManager& shader_manager()
	{
		return shader_manager_;
	}

	const ShaderManager& shader_manager() const
	{
		return shader_manager_;
	}

	void reset()
	{
		// TODO: implement full reset
		texture_manager_.free_all();
	}

	void update();
	void flush();
private:
	Driver driver_;
	WindowSystem* window_system_;
	TextureManager texture_manager_;
	TextureAtlasManager texture_atlas_manager_;
	MaterialManager material_manager_;
	ShaderManager shader_manager_;
};

}

#endif
