#include "text_test.hpp"
#include "camera.hpp"
#include "argparser.hpp"
#include "logutils.hpp"
#include "glwindow.hpp"
#include "mhe_gl.hpp"
#include "timer.hpp"
#include "sdl_input.hpp"
#include "irenderable.hpp"
#include "gui/font.hpp"
#include <boost/scoped_ptr.hpp>

namespace
{
    void render();
    boost::shared_ptr<mhe::iCamera> m_camera;
    bool running = true;
    boost::shared_ptr<mhe::gui::iFont> fnt;
};

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

int text_test(int argc, char **argv)
{
    mhe::mixlog l;
    boost::shared_ptr<mhe::stdlog> sl(new mhe::stdlog);
    boost::shared_ptr<mhe::filelog> fl(new mhe::filelog);
    l.add(sl);
    fl->open("text_log.txt");
    l.add(fl);

    mhe::arg_parser ap(argc, argv);
    ap.add_arg("-w", mhe::arg_parser::int_type);
    ap.add_arg("-h", mhe::arg_parser::int_type);
    ap.add_arg("-b", mhe::arg_parser::int_type);

    // window parameters
    int w = 800;
    int h = 600;
    int bpp = 32;

    if (!ap.process())
        l.write("Using default command line parameters\n");
    else
    {
        ap.get("-w", w);
        ap.get("-h", h);
        ap.get("-b", bpp);
    }
    l.printf("Width: %d Height:%d", w, h);

    mhe::GLWindow window(w, h, bpp);

    glClearColor(0, 0, 0, 1);

    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);

    // temporary code
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w/(float)h;
    float tan_angle = tan(30.0 * 3.14159265358f / 180.0);
    glFrustum(-tan_angle * 0.1 * aspect, tan_angle * 0.1 * aspect,
              -tan_angle * 0.1, tan_angle * 0.1, 0.1, 100.0);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    m_camera.reset(new mhe::Camera(mhe::v3d(0.5, 0.5, 2), mhe::v3d(0, 0, 0)));
    m_camera->update();

    mhe::InputSystem is;
    is.addListener(new SimpleQuit(mhe::SystemEventType, mhe::QUIT));
    is.addListener(new SimpleQuit(mhe::KeyboardEventType, SDLK_ESCAPE));

    // create font
    fnt.reset(mhe::gui::FontManager::instance().load("/home/master/projects/assets/FreeSans.ttf", 10));

    while (running)
    {
        is.check();
        is.handle();
        render();
    }

    return 0;
}

namespace
{
    void render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_camera->update();

        mhe::Axis axis;
        axis.draw();

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glDisable(GL_DEPTH);
        fnt->print(20, 20, "MHE");
        glEnable(GL_DEPTH);
        glPopMatrix();

        glFlush();
        SDL_GL_SwapBuffers();
    }
};
