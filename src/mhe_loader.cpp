#include "mhe_loader.hpp"

namespace mhe
{
	mhe_loader::mhe_loader(const std::string& filename, TextureManager* tm,
					   FontManager* fm, SoundManager* sm) :
		is_open_(false),
		parsed_(false),
		texture_manager(tm), font_manager(fm), sound_manager(sm)
	{
		pugi::xml_parse_result res = doc.load_file(filename.c_str());
		if (res.status != pugi::status_ok)
		{
			utils::global_log::instance().write("Can't parse mhe file " + filename);
			return;
		}
		utils::global_log::instance().write("File " + filename + " loaded");
		// test - is mhe document
		root = doc.child(L"mhe");
		if (root)
			is_open_ = true;
	}

	mhe_loader::~mhe_loader()
	{
	}

	bool mhe_loader::find_node(pugi::xml_node& node,
                               const std::wstring& type, const std::wstring& name) const
    {
        for (pugi::xml_node lnode = root.child(L"node"); lnode;
			 lnode = lnode.next_sibling(L"node"))
		{
			if ( (lnode.attribute(L"type").value() == type) &&
				 (lnode.child(L"name").child_value() == name) )
			{
				node = lnode;
				return true;
			}
		}
		return false;
    }

    bool mhe_loader::find_asset(pugi::xml_node& node,
                               const std::wstring& type, const std::wstring& name) const
    {
        for (pugi::xml_node lnode = root.child(L"asset"); lnode;
			 lnode = lnode.next_sibling(L"asset"))
		{
			if ( (lnode.attribute(L"type").value() == type) &&
				 (lnode.child(L"name").child_value() == name) )
			{
				node = lnode;
				return true;
			}
		}
		return false;
    }

    boost::shared_ptr<iTexture> mhe_loader::get_texture(const std::wstring& name) const
    {
        pugi::xml_node node;
        if (find_asset(node, L"texture", name))
            return load_texture(node);
        return boost::shared_ptr<iTexture>();
    }

    boost::shared_ptr<iTexture> mhe_loader::load_texture(const pugi::xml_node& node) const
    {
        // get filename
        std::wstring fn(node.child(L"file").child_value());
        // try to load texture
		if (!texture_manager) return boost::shared_ptr<iTexture>();
		return texture_manager->get(utils::from_wstr(fn));
    }

	boost::shared_ptr<gui::iFont> mhe_loader::get_font(const std::wstring& name) const
	{
        pugi::xml_node node;
        if (find_asset(node, L"font", name))
            return load_font(node);
        return boost::shared_ptr<gui::iFont>();
	}

	boost::shared_ptr<gui::iFont> mhe_loader::load_font(const pugi::xml_node& node) const
	{
        // get filename
        std::wstring fn(node.child(L"file").child_value());
        // try to load font
		if (!font_manager) return boost::shared_ptr<gui::iFont>();
		return font_manager->get(utils::from_wstr(fn));
	}

	boost::shared_ptr<iSound> mhe_loader::get_sound(const std::wstring& name) const
	{
        pugi::xml_node node;
        if (find_asset(node, L"sound", name))
            return load_sound(node);
        return boost::shared_ptr<iSound>();
	}

	boost::shared_ptr<iSound> mhe_loader::load_sound(const pugi::xml_node& node) const
	{
        // get filename
        std::wstring fn(node.child(L"file").child_value());
        // try to load font
		if (!sound_manager) return boost::shared_ptr<iSound>();
		return sound_manager->get(utils::from_wstr(fn));
	}

	Sprite* mhe_loader::getSprite(const std::wstring& name) const
	{
		if (!is_open_) return 0;

		pugi::xml_node node;
		if (find_node(node, L"sprite", name))
            return load_sprite(node);
        return 0;
	}

	gui::GUIContainer* mhe_loader::getGUI(const std::wstring& name) const
	{
		if (!is_open_) return 0;

		pugi::xml_node node;
		if (find_node(node, L"gui", name))
            return load_gui(node);
        return 0;
	}

	Sprite* mhe_loader::load_sprite(const pugi::xml_node& node) const
	{
	    // try read sprite position
	    pugi::xml_node pos_node = node.child(L"position");
	    const v3d& pos  = read_v3d(pos_node);
	    const v3d& size = read_v3d(node.child(L"size"));
	    pugi::xml_node al_node = node.child(L"animation_list");  // first

        Sprite* sprite = new Sprite;
	    for (pugi::xml_node it = al_node; it; it = it.next_sibling(L"animation_list"))
	        sprite->addAnimationList(read_animationList(it));

		sprite->setPosition(pos);
		sprite->setSize(size.x(), size.y());
		return sprite;
	}

	gui::GUIContainer* mhe_loader::load_gui(const pugi::xml_node& node) const
	{
		// create new gui container and find all widgets that it hold
		gui::GUIContainer* gui = new gui::GUIContainer();

		for (pugi::xml_node it = node.child(L"widget"); it;
			 				it = it.next_sibling(L"widget"))
			gui->add(read_widget(it));

		return gui;
	}

	AnimationList mhe_loader::read_animationList(const pugi::xml_node& node) const
	{
	    cmn::uint num = node.attribute(L"number").as_uint();
	    bool repeat = node.attribute(L"repeat").as_bool();
        AnimationList a_list(repeat);
        a_list.setIndex(num);
        for (pugi::xml_node it = node.child(L"animation"); it;
                            it = it.next_sibling(L"animation"))
        {
            a_list.add(read_animation(it));
        }
        return a_list;
	}

	Animation mhe_loader::read_animation(const pugi::xml_node& node) const
	{
		Animation a(read_number<cmn::uint>(node.child(L"duration")),
					get_texture(node.child(L"texture").child_value()));

        // read transformations
        for (pugi::xml_node it = node.first_child(); it; it = it.next_sibling())
        {
            if (std::wstring(it.name()) == L"translate")
                a.translate(read_v3d(it));
            else if (std::wstring(it.name()) == L"rotate")
            {
                float angle = read_float(it, L"angle");
                const v3d& v = read_v3d(it);
                a.rotate(v, angle);
            }
            else if (std::wstring(it.name()) == L"scale")
                a.scale(read_v3d(it));
        }
        return a;
	}

	boost::shared_ptr<gui::Widget> mhe_loader::read_widget(const pugi::xml_node& node) const
	{
		// load widget by type
		std::wstring type = node.attribute(L"type").value();

		if (type == std::wstring(L"label"))
			return load_label_widget(node);
        else if (type == std::wstring(L"button"))
            return load_button_widget(node);
		else
			return boost::shared_ptr<gui::Widget>();
	}

	boost::shared_ptr<gui::Label> mhe_loader::load_label_widget(const pugi::xml_node& node) const
	{
		// load common widget parameters
		boost::shared_ptr<gui::Label> lbl(new gui::Label());
		load_widget(node, lbl.get());
		pugi::xml_node n = node.child(L"texture");
		if (n)
            lbl->setTexture(get_texture(n.child_value()));

		return lbl;
	}

	boost::shared_ptr<gui::Button> mhe_loader::load_button_widget(const pugi::xml_node& node) const
	{
		// load common widget parameters
		boost::shared_ptr<gui::Button> btn(new gui::Button());
		load_widget(node, btn.get());
		pugi::xml_node n = node.child(L"texture");
		if (n)
            btn->setTexture(get_texture(n.child_value()));
        n = node.child(L"pressed_texture");
		if (n)
            btn->setPressedTexture(get_texture(n.child_value()));

		return btn;
	}

	void mhe_loader::load_widget(const pugi::xml_node& node,
                                 gui::Widget* widget) const
	{
		pugi::xml_node n = node.child(L"rect");
		widget->setGeometry(read_rect(n));
		// optional
		n = node.child(L"color");
		if (n)
			widget->setColor(read_v4d(n));
		n = node.child(L"font");
		if (n)
			widget->setFont(get_font(n.child_value()));
		n = node.child(L"name");
		if (n)
			widget->setName(utils::from_wstr(std::wstring(n.child_value())));
	}

	void mhe_loader::load_all_assets()
	{
		for (pugi::xml_node node = root.child(L"asset"); node;
			 node = node.next_sibling(L"asset"))
		{
			const std::wstring type = node.attribute(L"type").value();
			load_asset_by_type(node, type);
		}
	}

	void mhe_loader::load_asset_by_type(const pugi::xml_node& node,
										const std::wstring& type)
	{
		if (type == L"texture")
			load_texture(node);
		else if (type == L"font")
			load_font(node);
		else if (type == L"sound")
			load_sound(node);
	}

	//------------------ functions ------------------------
	float read_float(const pugi::xml_node& node)
	{
	    std::wstring ws(node.child_value());
	    float f = 0.0;
	    try
	    {
	        //f = boost::lexical_cast<float>(ws);
			f = utils::to_float(ws);
	    }
	    catch (std::exception&) {}

	    return f;
	}

	float read_float(const pugi::xml_node& node,
                     const std::wstring& attr_name)
    {
        std::wstring ws(node.attribute(attr_name.c_str()).value());
        float f = 0.0;
        try
        {
            //f = boost::lexical_cast<float>(ws);
			f = utils::to_float(ws);
        }
        catch (std::exception&) {}

        return f;
    }

	v3d read_v3d(const pugi::xml_node& node)
	{
	    float x = read_float(node, L"x");
	    float y = read_float(node, L"y");
	    float z = read_float(node, L"z");

	    return v3d(x, y, z);
	}

	v4d read_v4d(const pugi::xml_node& node)
	{
	    float x = read_float(node, L"x");
	    float y = read_float(node, L"y");
	    float z = read_float(node, L"z");
	    float w = read_float(node, L"w");

	    return v4d(x, y, z, w);
	}

	rect<float> read_rect(const pugi::xml_node& node)
	{
		float x = read_float(node, L"x");
		float y = read_float(node, L"y");
		float w = read_float(node, L"w");
		float h = read_float(node, L"h");

		return rect<float>(x, y, w, h);
	}
}
