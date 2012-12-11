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
    boost::shared_ptr<mhe::Sprite> sprite(mhe::utils::create_sprite("test_sprite.png", get_engine()->context()));
    scene()->add(sprite);
    sprite->start();
    
    mhe::ColorLinearAnimationList* animation_list = new mhe::ColorLinearAnimationList(1);
    animation_list->set_range(mhe::color_white, mhe::color_black, 100, 2000);
    sprite->add_animation_list(animation_list);
    sprite->execute(1);
    return true;
}