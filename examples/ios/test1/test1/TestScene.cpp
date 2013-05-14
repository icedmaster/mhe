//
//  TestScene.cpp
//  test1
//
//  Created by skraevsky on 05.12.12.
//  Copyright (c) 2012 mht. All rights reserved.
//

#include "TestScene.h"

bool TestScene::init_impl(const std::string&, void*)
{
    get_engine()->event_manager().add_listener(new MouseListener(this));
    mhe::gui::Widget* widget = new mhe::gui::Widget("main");
    widget->set_sprite(mhe::utils::create_sprite(mhe::color_white, mhe::vector2<float>(32, 32), get_engine()->context()));
    widget->set_geom(mhe::rect<float>(100, 100, 200, 200));
    // label
    mhe::gui::Label* label = new mhe::gui::Label("label");
    label->set_sprite(mhe::utils::create_sprite(mhe::color_green, mhe::vector2<float>(32, 32), get_engine()->context()));
    label->set_geom(mhe::rect<float>(10, 10, 50, 20));
    label->set_caption(mhe::utils::utf8_to_utf32("label"));
    label->set_font(get_engine()->font_manager().get("droid-sans.fnt"));
    label->set_caption_color(mhe::color_blue);
    widget->add_widget(label);
    // button
    mhe::gui::Button* button = new mhe::gui::Button("button");
    button->set_sprite(mhe::utils::create_sprite(mhe::color_green, mhe::vector2<float>(32, 32), get_engine()->context()));
    button->set_pressed_sprite(mhe::utils::create_sprite(mhe::color_yellow, mhe::vector2<float>(32, 32), get_engine()->context()));
    button->set_geom(mhe::rect<float>(50, 50, 50, 30));
    widget->add_widget(button);
    button->add_handler(mhe::gui::mouse_button_press_event, mhe::create_delegate(this, &TestScene::on_button_pressed));
    mhe::gui::GUINode* node = new mhe::gui::GUINode(get_engine()->event_manager());
    node->set_widget(widget);
    gui_node_ = node;
    scene()->add(mhe::nodeptr(node));
    return true;
}

void TestScene::on_button_pressed(mhe::gui::Widget* widget)
{
    mhe::gui::widgetptr label = gui_node_->get_widget("label");
    if (label->visible())
        label->set_visible(false);
    else label->set_visible(true);
}

void TestScene::perform_animation(const mhe::vector2<int> &pos)
{
    boost::shared_ptr<mhe::Sprite> sprite(mhe::utils::create_sprite("test_sprite.png", get_engine()->context()));
    sprite->translate(pos.x(), pos.y(), 0);
    scene()->add(sprite);
    sprite->start();
    
    mhe::ColorLinearAnimationList* animation_list = new mhe::ColorLinearAnimationList(1);
    animation_list->set_range(mhe::color_white, mhe::color_black, 100, 2000);
    sprite->add_animation_list(animation_list);
    sprite->execute(1);
}