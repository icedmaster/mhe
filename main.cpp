#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <boost/shared_ptr.hpp>
#include <cstdlib>
#include <ctime>
#include "gl_extension.hpp"
#include "opengl_driver.hpp"
#include "particle_system.hpp"
#include "test_particle_effect.hpp"
#include "circle_particle_effect.hpp"
#include "texture.hpp"
#include "sprite.hpp"
#include "mhe_loader.hpp"
#include "utils/logutils.hpp"
#include "texture_manager.hpp"
#include "event.hpp"
#include "window_system.hpp"
#include "camera.hpp"
#include "mhe_gui.hpp"
#include "png_image.hpp"

typedef unsigned int uint;

namespace
{
    void render();
    bool running = true;

    boost::shared_ptr<mhe::iDriver> driver(new mhe::OpenGLDriver);
    boost::shared_ptr<mhe::iCamera> camera(new mhe::Camera);

    cmn::uint next_tick;

    mhe::ParticleSystem ps1(100, mhe::v3d(2.0, -3.0, 0.0), true);
    mhe::ParticleSystem ps(48, mhe::v3d(0.0, -3.0, 0.0), true);

    mhe::Sprite sprite1;
    boost::shared_ptr<mhe::Sprite> test_sprite;
    boost::shared_ptr<mhe::Sprite> bgnd_sprite;
    boost::shared_ptr<mhe::Sprite> explode_sprite;
    boost::shared_ptr<mhe::Sprite> explode1_sprite;
    boost::shared_ptr<mhe::Sprite> cursor_sprite;
    boost::shared_ptr<mhe::gui::Cursor> cursor;

	boost::shared_ptr<mhe::gui::GUIContainer> main_gui;

	mhe::gui::BMFont font;

    class QuitListener : public mhe::EventListener
    {
        public:
            QuitListener(int type, int event, int arg = 0) :
                mhe::EventListener(type, event, arg)
            {
            }

            bool handle(const mhe::Event&)
            {
                running = false;
                mhe::utils::global_log::instance().write("quit event");
                return true;
            }
    };

    class ScreenshotListener : public mhe::EventListener
    {
        public:
            ScreenshotListener(int type, int event, int key) :
                mhe::EventListener(type, event, key)
            {
            }

            bool handle(const mhe::Event&)
            {
                std::vector<char> dt;
                driver->get_display_data(dt);

                mhe::png_image png;
                png.set(dt, 800, 600);
                png.save("screen.png");
                return true;
            }
    };

    class MouseListener : public mhe::EventListener
    {
        public:
            MouseListener() :
                EventListener(mhe::MouseEventType, mhe::MOUSE_BUTTON_PRESSED,
                              mhe::BUTTON_LEFT)
            {
            }

            bool handle(const mhe::Event& e)
            {
                const mhe::MouseEvent& me = dynamic_cast<const mhe::MouseEvent&>(e);
                mhe::utils::global_log::instance().printf("x=%d y=%d", me.position().x(),
                                                                       me.position().y());
                //test_sprite->setPosition(tmp);
                test_sprite->execute(0, SDL_GetTicks());
                return true;
            }
    };

	class LabelClickEventHandler : public mhe::gui::GUIEventHandler
	{
		public:
			void handle(const mhe::gui::Widget*)
			{
				mhe::utils::global_log::instance().write("label clicked");
			}
	};
}

int main(int, char**)
{
    mhe::utils::createStandartLog();
    mhe::utils::init_randomizer();
	mhe::WindowSystem ws;
	if (!ws.init(800, 600, 32))
		return 1;
    ws.showCursor(false);

    driver->set_window_system(&ws);

    mhe::load_default_extensions();
    mhe::utils::global_log::instance().write(mhe::get_availible_extensions());

    driver->set_clear_color(mhe::cfBlack);
    //driver->set_clear_color(mhe::cfWhite);
    driver->enable_depth((mhe::DepthFunc)0);
    driver->enable_lighting();
    //driver->enable_blending(mhe::ALPHA_ONE_MINUS_ALPHA);
    glEnable(GL_LIGHT0);

    float diff[] = {0.0, 0.8, 0.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diff);

    camera->setPosition(mhe::v3d(0.0, 2.0, 7.0));

    boost::shared_ptr<mhe::CircleParticleEffect> pe(new mhe::CircleParticleEffect(/*mhe::cfRed*/));
    pe->setAlphaMode(0.5, 0.005, 0.1, 0.9);
    pe->setSpeed(50.0);
    ps1.addEffect(new mhe::test::TestParticleEffect(mhe::cfYellow));
    ps.addEffect(pe);
    ps1.start(SDL_GetTicks());
    ps.start(SDL_GetTicks());

    next_tick = SDL_GetTicks() + 10;

    boost::shared_ptr<mhe::TextureManager> tm(new mhe::TextureManager());

    // try to init texture
    boost::shared_ptr<mhe::iTexture> tex = tm->get("assets/nya3.siwa");
    ps.setTexture(tex);

    // test mhe_loader
    mhe::mhe_loader loader("assets/level.mhe");
    loader.setTextureManager(tm);
    test_sprite.reset(loader.getSprite(L"maho"));
    bgnd_sprite.reset(loader.getSprite(L"background"));
    bgnd_sprite->setSize(800, 600);
    bgnd_sprite->setPriority(1);
    explode_sprite.reset(loader.getSprite(L"explode"));
    explode1_sprite.reset(loader.getSprite(L"explode1"));
    cursor_sprite.reset(loader.getSprite(L"cursor"));

    // create sprite
    sprite1.setPosition(mhe::v3d(2.0, 2.0, 0.0));

    const boost::shared_ptr<mhe::iTexture>& s_tex = tm->get("assets/blend.png");
    //s_tex->setImage(s_im);
    mhe::AnimationList s_al(true);
    mhe::Animation s_a(1000, s_tex);
    s_a.scale(mhe::v3d(0.5, 0.5, 1.0));
    mhe::Animation s_a1(1000, s_tex);
    s_a1.scale(mhe::v3d(2.0, 2.0, 1.0));
    s_al.add(s_a);
    s_al.add(s_a1);
    sprite1.addAnimationList(s_al);
    sprite1.setSize(1);

    explode_sprite->execute(0, SDL_GetTicks());
    explode1_sprite->execute(0, SDL_GetTicks());
    sprite1.execute(0, SDL_GetTicks());
    test_sprite->execute(0, SDL_GetTicks());
    bgnd_sprite->execute(0, SDL_GetTicks());

    mhe::InputSystem is;
    is.setWindowSystem(&ws);
    is.addListener(new QuitListener(mhe::SystemEventType, mhe::QUIT));
    is.addListener(new QuitListener(mhe::KeyboardEventType, mhe::KEY_DOWN, SDLK_ESCAPE));
    is.addListener(new ScreenshotListener(mhe::KeyboardEventType, mhe::KEY_DOWN, SDLK_s));
    is.addListener(new MouseListener);

    cursor.reset(new mhe::gui::Cursor(is));
    cursor_sprite->execute(0, SDL_GetTicks());
    cursor->setSprite(cursor_sprite);

	// setup font
	font.load("assets/145.fnt");
    font.setColor(mhe::cfRed);

    // load GUI from file
    main_gui.reset(loader.getGUI(L"main"));
    main_gui->setupEvents(is);

    bool asc = true;
    int ind = 1;
    while (running)
    {
        //process_events();
        is.check();
        render();

        cmn::uint tick = SDL_GetTicks();
        ps.update(tick);
        ps1.update(tick);
        explode_sprite->update(tick);
        explode1_sprite->update(tick);
        sprite1.update(tick);
        test_sprite->update(tick);
        bgnd_sprite->update(tick);
        cursor->update(tick);

        if (tick >= next_tick + 1000)
        {
            /*if (asc)
            {
                pe->setRadius(1.5);
                asc = false;
            }
            else
            {
                pe->setRadius(1.0);
                asc = true;
            }*/
            next_tick = tick;
            explode_sprite->execute(ind, tick, false);
            if (ind == 1) ind = 0;
            else ind = 1;
        }
		//driver->set_projection_matrix(fp);
    }
    return 0;
}

namespace
{
    void render()
    {
        driver->clear_depth();
        driver->clear_color();

        // draw background
        mhe::matrixf om;
		om.set_ortho(0, 800, 0, 600, -1, 1);
		driver->set_projection_matrix(om);
		driver->set_modelview_matrix(mhe::matrixf::identity());
		driver->disable_lighting();
		bgnd_sprite->draw(driver);
		driver->enable_lighting();

        // draw 3d things
        camera->update(driver);

        glColor3f(1.0, 0.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(10.0, 0.0, 0.0);
        glEnd();
        glColor3f(0.0, 1.0, 0.0);
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 10.0, 0.0);
        glEnd();
        glColor3f(0.0, 0.0, 1.0);
        glBegin(GL_LINES);
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 10.0);
        glEnd();
        glColor3f(1.0, 1.0, 1.0);

        //draw_cube();
        driver->disable_lighting();
        // draw sprites
        ps1.draw(driver);
        ps.draw(driver);
        explode_sprite->draw(driver);
        explode1_sprite->draw(driver);
        sprite1.draw(driver);

		driver->set_projection_matrix(om);
		driver->set_modelview_matrix(mhe::matrixf::identity());
		// draw background first
        test_sprite->draw(driver);
		main_gui->draw(driver);
		cursor->draw(driver);
		font.print(driver, L"aabb РУССкий текст", mhe::v2d(30, 30));
        driver->enable_lighting();

        //glFlush();
        SDL_GL_SwapBuffers();
    }
}
