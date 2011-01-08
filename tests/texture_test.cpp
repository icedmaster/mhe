#include "misc_test.hpp"
#include "camera.hpp"
#include "argparser.hpp"
#include "logutils.hpp"
#include "glwindow.hpp"
#include "irenderable.hpp"
#include "cube.hpp"
#include "mhe_gl.hpp"
#include "texture.hpp"
#include "bmp_image.hpp"
#include <boost/scoped_ptr.hpp>

namespace
{
    void render();
    mhe::iCamera* m_camera = 0;  // main camera
    mhe::Cube cube;
    mhe::iTexture *t = 0;
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

    // write cube parameters
    mhe::poly4d f = cube.front();
    mhe::poly4d b = cube.back();
    mhe::v3d n = f.getNormal();
    l.printf("front: %f %f %f", n.x(), n.y(), n.z());
    n = b.getNormal();
    l.printf("back: %f %f %f", n.x(), n.y(), n.z());
    n = cube.bottom().getNormal();
    l.printf("bottom: %f %f %f", n.x(), n.y(), n.z());
    n = cube.top().getNormal();
    l.printf("top: %f %f %f", n.x(), n.y(), n.z());
    n = cube.right().getNormal();
    l.printf("right: %f %f %f", n.x(), n.y(), n.z());
    n = cube.left().getNormal();
    l.printf("left: %f %f %f", n.x(), n.y(), n.z());

    mhe::GLWindow window(w, h, bpp);

    glClearColor(0, 0, 0, 1);

    //cube.invertNormals();
    // initialize OpenGL
    glEnable(GL_DEPTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
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
        l.write("Can't open texture image");
        return 1;
    }
    t->setImage(im);
    l.printf("Texture index: %u", t->id());

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
        render();
    }

    l.write("Misc test completed");
    SDL_Quit();
    delete m_camera;
    //delete light;
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

        //cube.draw();
        mhe::poly4d p = cube.front();

        glColor3f(1, 1, 1);
        //p.draw();
        glBindTexture(GL_TEXTURE_2D, t->id());
        glBegin(GL_QUADS);
        glTexCoord2f(0.0, 0.0); glVertex3f(0.0, 0.0, 0.0);
        glTexCoord2f(0.0, 1.0); glVertex3f(0.0, 1.0, 0.0);
        glTexCoord2f(1.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
        glTexCoord2f(1.0, 0.0); glVertex3f(1.0, 0.0, 0.0);
        glEnd();

        //m_camera->update();

        glFlush();
        SDL_GL_SwapBuffers();
    }
}
