#ifndef _ITIMER_HPP_
#define _ITIMER_HPP_

namespace mhe
{
    class iTimer
    {
        private:
            virtual void set_impl(cmn::uint) = 0;
            virtual void start_impl() = 0;
            virtual void stop_impl() = 0;
            virtual bool is_elapsed() const = 0;
        public:
            virtual ~iTimer() {}
            void set(cmn::uint interval, bool start = true)
            {
                set_impl(interval);
                if (start)
                    start_impl();
            }

            void start()
            {
                start_impl();
            }

            void stop()
            {
                stop_impl();
            }

            bool elapsed() const
            {
                return is_elapsed();
            }
    };
};

#endif
