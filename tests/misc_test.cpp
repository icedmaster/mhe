#include "misc_test.hpp"
#include "camera.hpp"
#include "argparser.hpp"
#include "logutils.hpp"
#include "glwindow.hpp"
#include "irenderable.hpp"
#include "cube.hpp"
#include "light.hpp"
#include "material.hpp"
#include "mhe_gl.hpp"
#include <boost/scoped_ptr.hpp>

namespace
{
    void render();
    mhe::iCamera* m_camera = 0;  // main camera
    mhe::Cube cube(mhe::v3d(-2, 0, 1), mhe::v3d(2, 2, -1));
    mhe::iLight* light = 0;
    mhe::iLight* light1 = 0;
    mhe::iLight* light2 = 0;
};

int misc_test(int argc, char **argv)
{
    mhe::mixlog l;
    boost::shared_ptr<mhe::stdlog> sl(new mhe::stdlog);
    boost::shared_ptr<mhe::filelog> fl(new mhe::filelog);
    l.add(sl);
    fl->open("log.txt");
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
    glEnable(GL_LIGHTING);
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

    mhe::setGlobalAmbient(mhe::colorf(0.3, 0.3, 0.3, 1));

    // create Light
    light = new mhe::Light(1, mhe::LightDirected);
    light->setPosition(mhe::v3d(0.0, 3.0, 0.0));
    light->setDirection(mhe::v3d(0.0, -1.0, 0.0));
    light->setDiffuse(mhe::colorf(0.0, 1.0, 0.0, 1.0));
    light->enable();
    light->update();

    light1 = new mhe::Light();
    light1->setPosition(mhe::v3d(0.0, 0.0, 4));
    light1->setDirection(mhe::v3d(0, 0, 0));
    light1->setDiffuse(mhe::colorf(1.0, 0.0, 1.0, 1.0));
    light1->enable();
    light1->update();

    light2 = new mhe::Light(2, mhe::LightDiffused);
    light2->setPosition(mhe::v3d(4.0, 0.0, 0));
    light2->setDirection(mhe::v3d(-1, 0, 0));
    light2->setDiffuse(mhe::colorf(0.0, 0.0, 1.0, 1.0));
    light2->enable();
    light2->update();

    m_camera = new mhe::Camera(mhe::v3d(0.5, 0.5, 2), mhe::v3d(0, 0, 0));
    m_camera->update();

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
    delete light;
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

        boost::scoped_ptr<mhe::iMaterial> m(new mhe::Material);
        m->setDiffuse(mhe::FRONT_AND_BACK_FACE, mhe::colorf(1.0, 1.0, 1.0, 1));
        m->update();
        cube.draw();
        mhe::poly4d p = cube.top();

        //glColor3f(1, 0, 0);
        //p.draw();

        light->update();
        light1->update();
        light2->update();

        //m_camera->update();

        glFlush();
        SDL_GL_SwapBuffers();
    }
}
