#include "misc_test.hpp"
#include "camera.hpp"
#include "argparser.hpp"
#include "logutils.hpp"
#include "glwindow.hpp"
#include "irenderable.hpp"
#include "cube.hpp"
#include "mhe_gl.hpp"
#include "multitexture.hpp"
#include "bmp_image.hpp"
#include "timer.hpp"
#include "sdl_input.hpp"
#include <boost/scoped_ptr.hpp>

namespace
{
    void render();
    bool running = true;
    mhe::iCamera* m_camera = 0;  // main camera
    mhe::Cube cube;
    mhe::iTexture *t = 0;
    cmn::uint secs = 0, total_frames = 0;

    class SimpleKeyboardHandler : public mhe::KeyboardEventHandler
    {
        public:
            bool handle(const mhe::KeyboardEvent& ke)
            {
                if (ke.type() == mhe::KEY_UP) return true; // ignore this kind of messages

                if (ke.type() == mhe::KEY_DOWN)
                {
                    // move right
                    if (ke.sym() == SDLK_q)
                        m_camera->move(mhe::x_axis, 0.05);
                    else if (ke.sym() == SDLK_a)
                        m_camera->move(mhe::x_axis, -0.05);
                }
                return true;
            }
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

    class ClickListener : public mhe::EventListener
    {
        public:
            ClickListener(mhe::EventType et, int type) :
                mhe::EventListener(et, type)
            {
            }

            bool handle(const mhe::Event& e)
            {
                // we listen only left button click
                mhe::eMouse m = e.getMouse();
                if (m.button == mhe::BUTTON_RIGHT) return true;
                std::cout << "left button clicked at \n" << m.pos.x << " " << m.pos.y << "\n";
                return true;
            }
    };
};

int texture_test(int argc, char **argv)
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

    //cube.invertNormals();
    // initialize OpenGL
    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_LIGHTING);
    glDepthFunc(GL_LEQUAL);
    //glEnable(GL_AUTO_NORMAL);
    //glEnable(GL_NORMALIZE);
    //glEnable(GL_COLOR_MATERIAL);

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

    m_camera = new mhe::Camera(mhe::v3d(0.5, 0.5, 2), mhe::v3d(0, 0, 0));
    m_camera->update();

    t = new mhe::Texture();
    boost::shared_ptr<mhe::Image> im(new mhe::bmp_image);
    if (!im->load("halo.bmp"))
    {
        l.write("Can't open texture image halo.bmp");
        return 1;
    }
    t->setImage(im);

    boost::shared_ptr<mhe::Image> im1(new mhe::bmp_image);
    if (!im1->load("nya.bmp"))
    {
        l.write("Can't open texture image nya.bmp");
        return 1;
    }
    t->setImage(im1);
    t->setCoord(0, mhe::v2d(0.0, 0.0));
    t->setCoord(1, mhe::v2d(0.0, 1.0));
    t->setCoord(2, mhe::v2d(1.0, 1.0));
    t->setCoord(3, mhe::v2d(1.0, 0.0));

    float tc_[8] = {0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 0.0};
    std::vector<float> tc;
    tc.assign(tc_, tc_ + 8);
    t->setCoord(tc);

    // init timer for stats
    SDL_InitSubSystem(SDL_INIT_TIMER);  // must be removed
    boost::shared_ptr<mhe::iTimer> tmr(new mhe::Timer);
    tmr->set(1000);

    mhe::InputSystem is;
    is.setKeyboardEventHandler(new SimpleKeyboardHandler);
    is.addListener(new SimpleQuit(mhe::SystemEventType, mhe::QUIT));
    is.addListener(new SimpleQuit(mhe::KeyboardEventType, SDLK_ESCAPE));
    is.addListener(new ClickListener(mhe::MouseEventType, mhe::MOUSE_BUTTON_PRESSED));

    //SDL_Event event;
    while  (running)
    {
        /*
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) break;
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE) break;
            if (event.key.keysym.sym == SDLK_RIGHT)
                m_camera->rotate(mhe::y_axis, 0.5);
            else if (event.key.keysym.sym == SDLK_LEFT)
                m_camera->rotate(mhe::y_axis, -0.5);
            else if (event.key.keysym.sym == SDLK_q)
                m_camera->move(mhe::x_axis, 0.05);
            else if (event.key.keysym.sym == SDLK_a)
                m_camera->move(mhe::x_axis, -0.05);
            else if (event.key.keysym.sym == SDLK_w)
                m_camera->move(mhe::y_axis, 0.05);
            else if (event.key.keysym.sym == SDLK_s)
                m_camera->move(mhe::y_axis, -0.05);
            else if (event.key.keysym.sym == SDLK_e)
                m_camera->move(mhe::z_axis, 0.05);
            else if (event.key.keysym.sym == SDLK_d)
                m_camera->move(mhe::z_axis, -0.05);

            else if (event.key.keysym.sym == SDLK_p)    // write current camera parameters
                     l.printf("x: %f y: %f z: %f", m_camera->getPosition().x(),
                                              m_camera->getPosition().y(),
                                              m_camera->getPosition().z());
        }
        else if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_p)    // write current camera parameters
                l.printf("x: %f y: %f z: %f", m_camera->getPosition().x(),
                                              m_camera->getPosition().y(),
                                              m_camera->getPosition().z());
        }
        */
        is.check();
        is.handle();
        render();

        if (tmr->elapsed())
        {
            ++secs;
            tmr->start();
        }
    }

    l.write("Misc test completed");
    l.printf("frames: %u seconds: %u FPS: %f", total_frames, secs, (float)total_frames/(float)secs);

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

        glColor3f(1, 1, 1);
        //p.draw();

        t->prepare();

        float coord[] = {0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0};
        cmn::uint ind[] = {0, 1, 2, 2, 3, 0};
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, coord);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, ind);
        /*glBegin(GL_QUADS);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 1.0, 0.0);
        glVertex3f(1.0, 1.0, 0.0);
        glVertex3f(1.0, 0.0, 0.0);
        glEnd();*/
        t->clean();
        glDisableClientState(GL_VERTEX_ARRAY);

        //m_camera->update();

        glFlush();
        SDL_GL_SwapBuffers();

        ++total_frames;
    }
}
