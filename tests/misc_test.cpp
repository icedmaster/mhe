#include "misc_test.hpp"
#include "camera.hpp"
#include "argparser.hpp"
#include "logutils.hpp"
#include "glwindow.hpp"
#include <GL/gl.h>

namespace
{
    void render();
    mhe::iCamera* m_camera = 0;  // main camera
};

int misc_test(int argc, char **argv)
{
    mhe::mixlog l;
    boost::shared_ptr<mhe::stdlog> sl(new mhe::stdlog);
    boost::shared_ptr<mhe::filelog> fl(new mhe::filelog);
    l.add(sl);
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

    // initialize OpenGL
    glEnable(GL_DEPTH);

    // temporary code
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float aspect = (float)w/(float)h;
    float tan_angle = tan(45.0 * 3.14159265358f / 180.0);
    glFrustum(-tan_angle * 0.1 * aspect, tan_angle * 0.1 * aspect,
              -tan_angle * 0.1, tan_angle * 0.1, 0.1, 100.0);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    m_camera = new mhe::Camera(mhe::v3d(0.5, 0.5, 2), mhe::v3d(0, 0, 0));

    SDL_Event event;
    for  (;;)
    {
        SDL_PollEvent(&event);
        if (event.type == SDL_QUIT) break;
        if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_ESCAPE) break;
            if (event.key.keysym.sym == SDLK_RIGHT)
                m_camera->rotate(mhe::y_axis, 0.5);
            else if (event.key.keysym.sym == SDLK_LEFT)
                m_camera->rotate(mhe::y_axis, -0.5);
        }
        else if (event.type == SDL_KEYDOWN)
        {
            if (event.key.keysym.sym == SDLK_p)    // write current camera parameters
                l.printf("x: %f y: %f z: %f", m_camera->getPosition().x(),
                                              m_camera->getPosition().y(),
                                              m_camera->getPosition().z());
        }
        render();
    }

    l.write("Misc test completed");
    SDL_Quit();
    delete m_camera;
    return 0;
}


namespace
{
    void render()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_camera->update();

        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(0,0,0);
        glVertex3f(10, 0, 0);
        glEnd();

        glColor3f(0, 1, 0);
        glBegin(GL_LINES);
        glVertex3f(0,0,0);
        glVertex3f(0, 10, 0);
        glEnd();

        glColor3f(0, 0, 1);
        glBegin(GL_LINES);
        glVertex3f(0,0,0);
        glVertex3f(0, 0, 10);
        glEnd();

        glFlush();
        SDL_GL_SwapBuffers();
    }
}
