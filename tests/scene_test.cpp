#include "scene_test.hpp"
#include "render2d.hpp"
#include "render3d.hpp"
#include "glwindow.hpp"

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
};

int scene_test(int argc, char **argv)
{
    const cmn::uint w = 800;
    const cmn::uint h = 600;
    const cmn::uint bpp = 32;
    mhe::GLWindow window(w, h, bpp);
    boost::shared_ptr<mhe::Viewport> view(new mhe::Viewport(0, 0, w, h));
    mhe::globals::instance().set_window(w, h);

    glClearColor(1, 1, 1, 1);

    // create scene
    mhe::Scene scene;
    scene.setViewport(boost::shared_ptr<mhe::Viewport>(view));
    boost::shared_ptr<mhe::iRenderable> axis(new mhe::Axis);
    scene.add(axis);

    // setup render
    boost::shared_ptr<mhe::WindowSystem> ws(new mhe::WindowSystem);
    boost::shared_ptr<mhe::iRender> render(new mhe::Render3D(ws));
    render->add(&scene);

    boost::shared_ptr<mhe::iCamera> m_camera(new mhe::Camera(mhe::v3d(0.5, 0.5, 2), mhe::v3d(0, 0, 0)));
    m_camera->update();
    scene.addCamera(m_camera, true);

    mhe::InputSystem is;
    is.addListener(new SimpleQuit(mhe::SystemEventType, mhe::QUIT));
    is.addListener(new SimpleQuit(mhe::KeyboardEventType, SDLK_ESCAPE));

    while (running)
    {
        is.check();
        is.handle();
        render->render();
    }

    return 0;
}
