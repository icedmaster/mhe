#include "gui_test.hpp"
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
#include "gui/gui.hpp"
#include "globals.hpp"
#include <boost/scoped_ptr.hpp>

namespace
{
    void render();
    boost::shared_ptr<mhe::iCamera> m_camera;
    bool running = true;
    boost::shared_ptr<mhe::gui::iFont> fnt;
    boost::shared_ptr<mhe::gui::MainWidget> mw;
    boost::shared_ptr<mhe::gui::Label> lbl;
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

class GUIEventHandler : public mhe::MouseEventHandler
{
    public:
        bool handle(const mhe::MouseEvent& me)
        {
            mw->handleMouseEvent(me);
            return true;
        }
};

class LabelListener : public mhe::EventListener
{
    public:
        bool handle(const mhe::Event&)
        {
            fnt->setForegroundColor(mhe::cfBlack);
            return true;
        }
};

class LabelLeftListener : public mhe::EventListener
{
    public:
        bool handle(const mhe::Event&)
        {
            fnt->setForegroundColor(mhe::cfRed);
            return true;
        }
};

class LabelClickListener : public mhe::EventListener
{
    public:
        bool handle(const mhe::Event& e)
        {
            lbl->setCaption("Nya ^_^");
            return true;
        }
};

int gui_test(int argc, char **argv)
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

    glClearColor(1, 1, 1, 1);

    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    //glDepthFunc(GL_LEQUAL);

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
    is.setMouseEventHandler(new GUIEventHandler);

    // create font
    fnt.reset(mhe::gui::FontManager::instance().load("/home/master/projects/assets/FreeMono.ttf", 20));
    fnt->setForegroundColor(mhe::cfRed);
    // create widgets
    mw.reset(new mhe::gui::MainWidget);
    lbl.reset(new mhe::gui::Label);
    lbl->setFont(fnt);
    lbl->setBackground(mhe::cfGreen);
    lbl->setGeometry(mhe::rect(200, 500, 100, 50));
    lbl->setCaption("nya");
    lbl->setOnMouseMove(new LabelListener);
    lbl->setOnMouseLeft(new LabelLeftListener);
    lbl->setOnMouseClick(new LabelClickListener);
    lbl->setImage("/home/master/projects/assets/label.bmp");
    mw->add(lbl);

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

        // draw other part of scene
        m_camera->update();

        mhe::Axis axis;
        axis.draw();

        // set ortogonal projection for text rendering
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();     // save previous projection
        glLoadIdentity();
        glOrtho( 0, mhe::globals::instance().window_width(),
                 0, mhe::globals::instance().window_height(), -1, 1 );
        glMatrixMode(GL_MODELVIEW);
        // disable depth test for 2d rendering
        glDisable(GL_DEPTH);
        glDisable(GL_DEPTH_TEST);
        glLoadIdentity();
        mw->draw();
        // restore parameters
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_DEPTH);
        // restore projection
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();

        glFlush();
        SDL_GL_SwapBuffers();
    }
}
