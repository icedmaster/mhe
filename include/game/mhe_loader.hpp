#ifndef _MHE_LOADER_HPP_
#define _MHE_LOADER_HPP_

#include "lib/pugixml/pugixml.hpp"
#include "mhe_math.hpp"
#include "sprite.hpp"
#include "texture_manager.hpp"
#include "font_manager.hpp"
#include "sound_manager.hpp"
#include "utils/strutils.hpp"
#include "utils/logutils.hpp"
#include "mhe_gui.hpp"
#include "engine.hpp"

namespace mhe {
namespace game {
	class mhe_loader
	{
		private:
            pugi::xml_document doc;
			pugi::xml_node root;
			bool is_open_;
			bool parsed_;

			Engine* engine_;

			bool find_node(pugi::xml_node& node,
                           const std::wstring& type, const std::wstring& name) const;
            bool find_asset(pugi::xml_node& node,
                            const std::wstring& type, const std::wstring& name) const;

            boost::shared_ptr<iTexture> get_texture(const std::wstring& name) const;
            boost::shared_ptr<iTexture> load_texture(const pugi::xml_node& node) const;

			boost::shared_ptr<gui::iFont> get_font(const std::wstring& name) const;
			boost::shared_ptr<gui::iFont> load_font(const pugi::xml_node& node) const;

			boost::shared_ptr<iSound> get_sound(const std::wstring& name) const;
			boost::shared_ptr<iSound> load_sound(const pugi::xml_node& node) const;

			void load_asset_by_type(const pugi::xml_node& node,
									const std::wstring& type);

			Sprite* load_sprite(const pugi::xml_node& node) const;
			gui::GUIContainer* load_gui(const pugi::xml_node& node) const;

			AnimationList read_animationList(const pugi::xml_node& node) const;
			Animation read_animation(const pugi::xml_node& node) const;

			boost::shared_ptr<gui::Widget> read_widget(const pugi::xml_node& node) const;
			boost::shared_ptr<gui::Label> load_label_widget(const pugi::xml_node& node) const;
			boost::shared_ptr<gui::Button> load_button_widget(const pugi::xml_node& node) const;
			void load_widget(const pugi::xml_node& node,
                             gui::Widget* widget) const;
		public:
			mhe_loader(const std::string& filename, Engine* engine);
			~mhe_loader();

			Sprite* getSprite(const std::wstring& name) const;
			gui::GUIContainer* getGUI(const std::wstring& name) const;

			void load_all_assets();

			const Engine* get_engine() const
			{
				return engine_;
			}
	};

	// helper functions
	float read_float(const pugi::xml_node& node);
    float read_float(const pugi::xml_node& node,
                     const std::wstring& attr_name);

    template <class T>
    T read_number(const pugi::xml_node& node)
    {
        std::wstring ws(node.child_value());
        T num = 0;
        try
        {
            num = boost::lexical_cast<T>(ws);
        }
        catch (std::exception&) {}
        return num;
    }

	v3d read_v3d(const pugi::xml_node& node);
	v4d read_v4d(const pugi::xml_node& node);
	rect<float> read_rect(const pugi::xml_node& node);
}}	// namespaces

#endif
