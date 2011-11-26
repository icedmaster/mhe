#ifndef _SYSTEM_FACTORY_HPP_
#define _SYSTEM_FACTORY_HPP_

#include "config.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
	class iDriver;
	class iWindowSystem;
    class iInputSystem;
	class iTexture;
	class iAudioDriver;
	class iSound;

	// if we want to use custom factory
	class iSystemFactory
	{
	public:
		virtual ~iSystemFactory() {}
		virtual iWindowSystem* create_window_system() const = 0;
		virtual iInputSystem* create_input_system() const = 0;
	};

    class SystemFactory
    {
        private:
            SystemFactory() {}
            SystemFactory(const SystemFactory&) {}
            ~SystemFactory() {}
            SystemFactory& operator= (const SystemFactory&) {return *this;}

			boost::shared_ptr<iSystemFactory> factory_;
        public:
            static SystemFactory& instance()
            {
                static SystemFactory sf;
                return sf;
            }

			iDriver* createDriver() const;
			iWindowSystem* createWindowSystem() const;
			iInputSystem* createInputSystem() const;
			iTexture* createTexture() const;
			iTexture* create_multitexture() const;
			iAudioDriver* createAudioDriver() const;
			iSound* createSound() const;

			void set_system_factory(iSystemFactory* factory)
			{
				factory_.reset(factory);
			}	

    };

    namespace gui
    {
        /*class iFontManager;
        class LabelImpl;

        class GUIFactory
        {
            private:
                GUIFactory() {}
                GUIFactory(const GUIFactory&) {}
                ~GUIFactory() {}
                void operator= (const GUIFactory&) {}
            public:
                static GUIFactory& instance()
                {
                    static GUIFactory gf;
                    return gf;
                }

                iFontManager* createFontManager();
                LabelImpl* createLabel();
        };*/
    };
};

#endif
