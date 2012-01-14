#ifndef _EVENT_HPP_
#define _EVENT_HPP_

#include "types.hpp"
#include "mhe_math.hpp"
#include "window_system.hpp"
#include "impl/system_factory.hpp"
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <map>
#include <vector>

namespace mhe
{
	const int ANY_EVENT = 0;
	// event types
	enum
	{
		NullEventType,
		KeyboardEventType,
		MouseEventType,
		SystemEventType,
		TimerEventType
	};

	// helper functions
	inline cmn::uint create_event_id(int type, int arg, int optarg = 0)
	{
		cmn::uint event = ((optarg << 24) & 0xff000000) |
						  ((type << 16) & 0xff0000)     |
						  (arg & 0xffff);
		return event;
	}

	inline int get_event_type(cmn::uint event)
	{
		return (event >> 16) & 0xff;
	}

	inline int get_event_arg(cmn::uint event)
	{
		return (event & 0xffff);
	}

	inline int get_event_optarg(cmn::uint event)
	{
		return (event >> 24) & 0xff;
	}

	// base event class
	class Event
	{
		private:
			cmn::uint event_id;
			cmn::uint tmstamp;
		private:
			virtual int get_type() const
			{
				return NullEventType;
			}
		protected:
			void set_event_id(cmn::uint id)
			{
				event_id = id;
			}
		public:
			Event() :
				event_id(0)
			{}

			int type() const
			{
				return get_type();
			}

			cmn::uint id() const
			{
				return event_id;
			}

			void set_timestamp(cmn::uint tstamp)
			{
				tmstamp = tstamp;
			}

			cmn::uint timestamp() const
			{
				return tmstamp;
			}
	};

	// keyboard event types
	enum
	{
		KEY_UP = 1,
		KEY_DOWN
	};

	// Keyboard event class
	class KeyboardEvent : public Event
	{
		private:
			int get_type() const
			{
				return KeyboardEventType;
			}
		public:
			KeyboardEvent(int state,	// up or down
						  int sym)
			{
				set_event_id(create_event_id(KeyboardEventType, state, sym));
			}

			int sym() const
			{
				return get_event_arg(id());
			}

			int state() const
			{
				return get_event_optarg(id());
			}
	};

	// mouse event types
	enum
	{
		MOUSE_MOVE = 1,
		MOUSE_BUTTON_PRESSED,
		MOUSE_BUTTON_RELEASED
	};

	// mouse buttons
	enum
	{
		BUTTON_LEFT  = 1,
		BUTTON_RIGHT = (1 << 1)
	};

	// Mouse event class
	class MouseEvent : public Event
	{
		private:
			vector2<int> pos_;
		private:
			int get_type() const
			{
				return MouseEventType;
			}
		public:
			MouseEvent(int state,	// move, press, release...
					   int button,
					   const vector2<int>& pos) :
				pos_(pos)
			{
				set_event_id(create_event_id(MouseEventType, state, button));
			}

			const vector2<int>& position() const
			{
				return pos_;
			}

			int x() const
			{
				return pos_.x();
			}

			int y() const
			{
				return pos_.y();
			}

			int button() const
			{
				return get_event_optarg(id());
			}

			int state() const
			{
				return get_event_arg(id());
			}
	};

	// system events
	enum
	{
		QUIT = 1,
		TICK		
	};	

	// system event class
	class SystemEvent : public Event
	{
		private:
			int get_type() const
			{
				return SystemEventType;
			}
		public:
			SystemEvent(int event, int arg = 0)
			{
				set_event_id(create_event_id(SystemEventType, event, arg));
			}
	};

	class TimerEvent : public Event
	{
		private:
			int get_type() const
			{
				return TimerEventType;
			}
		public:
			enum
			{
				TIMER = 0,
				TIMER_ONCE
			};

			TimerEvent(int event, int arg = 0)
			{
				set_event_id(create_event_id(TimerEventType, event, arg));
			}
	};


	// 
	// If we need to handle all events of concrete type - 
	// need to create listener with type != 0 and event and event_opt = 0
	// 
	class EventListener
	{
		private:
			cmn::uint event_id;
		public:
			EventListener(int type, int event, int event_opt = 0) :
				event_id(create_event_id(type, event, event_opt))
			{}

			cmn::uint id() const
			{
				return event_id;
			}

			virtual bool handle(const Event&/* event*/) {return true;}
	};


	// If you derived from EventListenerEx class, you can
	// override handle() function, or handle_ex() function, that
	// called from handle().
	class EventListenerEx : public EventListener
	{
		private:
			cmn::uint prev_tstamp;
		protected:
			cmn::uint get_tdiff(const Event& e) const
			{
				return e.timestamp() - prev_tstamp;
			}

			virtual bool handle_ex(const Event&) {return true;}
		public:
			EventListenerEx(int type, int event, int event_opt = 0) :
				EventListener(type, event, event_opt),
				prev_tstamp(0)
			{}

			virtual bool handle(const Event& event)
			{
				// save timestamp first
				if (!prev_tstamp) prev_tstamp = event.timestamp();
				bool res = handle_ex(event);
				// update timestamp
				prev_tstamp = event.timestamp();
				return res;
			}
	};

	class iInputSystem
	{
		private:
			struct Listener
			{
				boost::weak_ptr<EventListener> listener;
				boost::shared_ptr<EventListener> once_listener;
			};
			typedef std::multimap< cmn::uint, Listener > mlisteners;
			typedef std::pair< mlisteners::iterator, mlisteners::iterator > mlitpair;

			struct TimerListener
			{
				cmn::uint start;
				Listener listener;
				bool once;
			};

			std::vector<TimerListener> timer_listeners_;

			mlisteners listmap;
			mlisteners arg_listmap;
			mlisteners gl_listmap;
			bool input_enabled_;

			void set_input_state(bool state);

			void add_listener(const Listener& l);
			void add_timer_listener(const Listener& l);
			void add_event_timestamp(Event* e);

			bool filter_event(const Event& e) const;
		private:
			virtual	void check_impl() = 0;
			virtual void handle_impl() = 0;
			virtual void set_window_system_impl(WindowSystem*) {}
		protected:
			void check_timer_events();
			void check_listeners(Event& e);
		public:
			iInputSystem() :
				input_enabled_(true)
			{}

			void check()
			{
				check_impl();
			}

			void handle()
			{
				handle_impl();
			}

			void addListener(boost::shared_ptr<EventListener> el);
			void addListener(EventListener* el);				

			void setWindowSystem(WindowSystem* ws)
			{
				set_window_system_impl(ws);
			}

			void disable_input()
			{
				set_input_state(false);
			}
	
			void enable_input()
			{
				set_input_state(true);
			}
	};

	// helper classes

	// Typical using of PrivateEventListener
	// class SuperClass
	// {
	// 	   typedef PrivateEventListener<SuperClass> rrr;
	//	   friend class rrr;
	//     public:
	//         SuperClass(InputSystem& is)
	//         {
	//             is.addListener(new rrr(EventType, event, event_opt,
    //									  this, &this::handler));
	//         }
	template <class T>
	class PrivateEventListener : public EventListener
	{
		private:
			typedef bool (T::*handler)(const Event&);
		private:
			T* dest;
			handler h;
		public:
			PrivateEventListener(int et, int event, int arg,
							     T* d, handler hnd) :
				EventListener(et, event, arg),
				dest(d), h(hnd)
			{}

			bool handle(const Event& e)
			{
				return (dest->*h)(e);
			}
	};
}

#endif