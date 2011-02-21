#include "scene_test.hpp"
#include "render2d.hpp"
#include "render3d.hpp"
#include "glwindow.hpp"
#include "gui/gui.hpp"
#include "math/mhe_math.hpp"
#include "sprite.hpp"
#include "utils/image_loader.hpp"
#include "texture.hpp"
#include "geom.hpp"
#include <iostream>

namespace
{
    bool running = true;
    /*
    boost::shared_ptr<mhe::gui::iFont> fnt;
    boost::shared_ptr<mhe::gui::MainWidget> mw;
    boost::shared_ptr<mhe::gui::Label> lbl;
    */

    class SimpleQuit : public mhe::EventListener
    {
    public:
        SimpleQuit(mhe::EventType et, int type) :
            mhe::EventListener(et, type)
        {
        }
        bool handle(const mhe::Event&)
        {
            running = false;
            return true;
        }
    };

    class SimpleTimer : public mhe::EventListener
    {
        public:
            SimpleTimer(mhe::EventType et, int type) :
                mhe::EventListener(et, type)
            {
            }

            bool handle(const mhe::Event&)
            {
                std::cout << "second elapsed\n";
                return true;
            }
    };
};

int scene_test(int argc, char **argv)
{
    const cmn::uint w = 800;
    const cmn::uint h = 600;
    const cmn::uint bpp = 32;
    mhe::GLWindow window(w, h, bpp);
    boost::shared_ptr<mhe::Viewport> view(new mhe::Viewport(0, 0, w / 2, h));
    boost::shared_ptr<mhe::Viewport> view_2(new mhe::Viewport(w / 2, 0, w, h));
    mhe::globals::instance().set_window(w, h);

    glClearColor(1, 1, 1, 1);

    // create gui
    boost::shared_ptr<mhe::gui::iFont> font(mhe::gui::FontManager::instance().load("/home/master/projects/assets/FreeSans.ttf", 20));
    boost::shared_ptr<mhe::gui::Label> lbl(new mhe::gui::Label);
    lbl->setFont(font);
    lbl->setBackground(mhe::cfGreen);
    lbl->setGeometry(mhe::rect(200, 500, 100, 50));
    lbl->setCaption("nya");

    mhe::InputSystem is;
    is.addListener(new SimpleQuit(mhe::SystemEventType, mhe::QUIT));
    is.addListener(new SimpleQuit(mhe::KeyboardEventType, SDLK_ESCAPE));
    is.addListener(new SimpleTimer(mhe::TimerEventType, 1000));

    // create scene
    mhe::Scene* scene = new mhe::Scene(is);
    scene->setViewport(boost::shared_ptr<mhe::Viewport>(view));
    boost::shared_ptr<mhe::iRenderable> axis(new mhe::Axis);
    scene->add(axis);

    mhe::Scene* scene_2 = new mhe::Scene;
    scene_2->setViewport(boost::shared_ptr<mhe::Viewport>(view_2));
    scene_2->add(axis);

    mhe::Scene* gui_scene = new mhe::Scene;
    gui_scene->setViewport(scene->getViewport());
    gui_scene->add(lbl);

    // try to create sprite
    // 1) load images
    boost::shared_ptr<mhe::Image> im(mhe::load_image("/home/master/projects/assets/sprite1.png"));
    // 2) create texture
    boost::shared_ptr<mhe::iTexture> tex(new mhe::Texture);
    tex->setImage(im);
    // 3) create new animation
    mhe::Animation a(1000, tex);
    // 4) create sprite
    boost::shared_ptr<mhe::Sprite> s(new mhe::Sprite);
    s->setPosition(mhe::v2d(100, 100));
    // 5) add animation to sprite
    s->addAnimation(a);
    // 6) add sprite to scene
    gui_scene->add(s);

    // setup render
    boost::shared_ptr<mhe::WindowSystem> ws(new mhe::WindowSystem);
    boost::shared_ptr<mhe::iRender> render(new mhe::Render3D(ws));
    render->add(scene);
    render->add(scene_2);
    boost::shared_ptr<mhe::iRender> gui_render(new mhe::Render2D(ws));
    gui_render->add(gui_scene);
    render->addSubrender(gui_render);

    boost::shared_ptr<mhe::iCamera> m_camera(new mhe::Camera(mhe::v3d(0.5, 0.5, 2), mhe::v3d(0, 0, 0)));
    m_camera->update();
    boost::shared_ptr<mhe::iCamera> m_camera2(new mhe::Camera(mhe::v3d(0.5, 0.5, 2), mhe::v3d(0, 0, 0)));
    m_camera2->update();
    scene->addCamera(m_camera, true);
    scene_2->addCamera(m_camera2, true);

    while (running)
    {
        is.check();
        is.handle();
        render->render();
    }

    return 0;
}
