#ifndef _INPUT_HPP_
#define _INPUT_HPP_

#include "types.hpp"
#include "impl/system_factory.hpp"
#include <boost/shared_ptr.hpp>

namespace mhe
{
    class SystemFactory;

	enum EventType
	{
		NullEventType,
		KeyboardEventType,
		MouseEventType,
		SystemEventType,
		TimerEventType
	};

    // Keyboard events
	enum
	{
		KEY_DOWN,
		KEY_UP
	};

	// System events
	enum
	{
        QUIT
	};

	// Mouse events
	enum
	{
	    MOUSE_MOVE,
	    MOUSE_BUTTON_PRESSED,
	    MOUSE_BUTTON_RELEASED
	};

	// timer events
	enum
	{
	    ELAPSED
	};

	struct Point
	{
	    cmn::uint x;
	    cmn::uint y;
	};

	struct Mouse
	{
	    Point pos;
	    cmn::uint button;
	};

	struct Keyboard
	{
	    cmn::uint key;
	};

	class Event
	{
	    private:
            Mouse mouse;
            Keyboard keyboard;
		private:
			virtual EventType get_type() const
			{
				return NullEventType;
			}

			virtual cmn::uint get_id() const
			{
				return 0;
			}
        protected:
            void set_mouse(const Mouse& m)
            {
                mouse = m;
            }

            void set_keyboard(const Keyboard& k)
            {
                keyboard = k;
            }
		public:
            Event()
            {
            }

			EventType eventType() const
			{
				return get_type();
			}

			cmn::uint id() const
			{
				return get_id();
			}

			const Mouse& getMouse() const
			{
			    if (get_type() != MouseEventType)
                    throw mhe::exception("Not mouse type emitted!");
			    return mouse;
			}

			const Keyboard& getKeyboard() const
			{
			    if (get_type() != KeyboardEventType)
                    throw mhe::exception("Not keyboard event emitted!");
                return keyboard;
			}
	};

	class KeyboardEvent : public Event
	{
		private:
			EventType get_type() const
			{
				return KeyboardEventType;
			}

			cmn::uint get_id() const
			{
				return (KeyboardEventType << 24) | getKeyboard().key;
			}
		private:
			int type_;
		public:
			KeyboardEvent(int type, cmn::uint sym) :
				type_(type)
			{
			    Keyboard kb;
			    kb.key = sym;
			    set_keyboard(kb);
			}

			int type() const
			{
				return type_;
			}

			cmn::uint sym() const
			{
				return getKeyboard().key;
			}
	};

	class SystemEvent : public Event
	{
		private:
			EventType get_type() const
			{
				return SystemEventType;
			}

			cmn::uint get_id() const
			{
				return (SystemEventType << 24) | event_;
			}
        private:
            int event_;
        public:
            SystemEvent(int event) :
                event_(event)
            {
            }
	};

	enum
	{
	    BUTTON_LEFT  = 1,
	    BUTTON_RIGHT = (1 << 1)
	};

	class MouseEvent : public Event
	{
        private:
			EventType get_type() const
			{
				return MouseEventType;
			}

			cmn::uint get_id() const
			{
				return (MouseEventType << 24) | event_;
			}
        private:
            int event_;
        public:
            MouseEvent(int event, const Point& pos, cmn::uint but = 0) :
                event_(event)
            {
                Mouse m;
                m.button = but;
                m.pos = pos;
                set_mouse(m);
            }

            int type() const
            {
                return event_;
            }

            const Point& position() const
            {
                return getMouse().pos;
            }

            cmn::uint buttons() const
            {
                return getMouse().button;
            }
	};

	class EventListener
	{
		private:
			cmn::uint event_id;

			void create_event_id(EventType et, int event)
			{
				event_id = ((et << 24) & 0xff000000) | event;
			}
		public:
			EventListener(EventType et, int event)
			{
				create_event_id(et, event);
			}

			cmn::uint id() const
			{
				return event_id;
			}

			virtual bool handle(const Event&) {return true;}
	};

	class KeyboardEventHandler
	{
		public:
			virtual bool handle(const KeyboardEvent&) {return true;}
	};

	class MouseEventHandler
	{
	    public:
            virtual bool handle(const MouseEvent&) {return true;}
	};

	class iInputSystem
	{
		private:
			virtual void check_impl() = 0;
			virtual void handle_impl() = 0;
			virtual void add_listener(EventListener* el) = 0;
			virtual void set_keyboard_event_handler(KeyboardEventHandler *ke) = 0;
		public:
			void check()
			{
				check_impl();
			}

			void handle()
			{
			    handle_impl();
			}

			void addListener(EventListener* el)
			{
				add_listener(el);
			}

			void setKeyboardEventHandler(KeyboardEventHandler* ke)
			{
			    set_keyboard_event_handler(ke);
			}
	};

	class InputSystem
	{
	    private:
            boost::shared_ptr<iInputSystem> impl_;
	    public:
            InputSystem() :
                impl_(SystemFactory::instance().createInputSystem())
            {
            }

            void check()
            {
                impl_->check();
            }

            void handle()
            {
                impl_->handle();
            }

            void addListener(EventListener* el)
			{
				impl_->addListener(el);
			}

			void setKeyboardEventHandler(KeyboardEventHandler* ke)
			{
			    impl_->setKeyboardEventHandler(ke);
			}
	};
};

#endif
