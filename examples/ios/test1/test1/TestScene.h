//
//  TestScene.h
//  test1
//
//  Created by skraevsky on 05.12.12.
//  Copyright (c) 2012 mht. All rights reserved.
//

#ifndef __test1__TestScene__
#define __test1__TestScene__

#include <mhe.hpp>

class TestScene : public mhe::game::GameScene
{
private:
    class MouseListener : public mhe::EventListener
    {
    public:
        MouseListener(TestScene* scene) :
            scene_(scene)
        {}
        
        mhe::EventType type() const {return mhe::mouse_event_type;}
        int arg() const {return mhe::MouseEvent::button_pressed;}
        
        bool handle(const mhe::Event* event)
        {
            const mhe::MouseEvent* mouse_event = static_cast<const mhe::MouseEvent*>(event);
            scene_->mouse_click(mouse_event->pos());
            return false;
        }
    private:
        TestScene* scene_;
    };
    friend class MouseListener;
public:
    TestScene(mhe::game::Engine* engine):
        mhe::game::GameScene(engine)
    {}
    
    bool init_impl(const std::string&, void*);

    void on_button_pressed(mhe::gui::Widget* widget);
private:
    void mouse_click(const mhe::vector2<int>& pos)
    {
        perform_animation(pos);
    }
    
    void perform_animation(const mhe::vector2<int>& pos);

    mhe::gui::GUINode* gui_node_;
};

#endif /* defined(__test1__TestScene__) */
