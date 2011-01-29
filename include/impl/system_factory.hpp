#ifndef _SYSTEM_FACTORY_HPP_
#define _SYSTEM_FACTORY_HPP_

#include "config.hpp"

namespace mhe
{
    class iInputSystem;

    class SystemFactory
    {
        private:
            SystemFactory() {}
            SystemFactory(const SystemFactory&) {}
            ~SystemFactory() {}
            SystemFactory& operator= (const SystemFactory&) {return *this;}
        public:
            static SystemFactory& instance()
            {
                static SystemFactory sf;
                return sf;
            }

            iInputSystem* createInputSystem();

    };

    namespace gui
    {
        class iFontManager;
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
        };
    };
};

#endif
