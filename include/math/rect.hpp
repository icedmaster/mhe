#ifndef _RECT_HPP_
#define _RECT_HPP_

#include "vector2d.hpp"
#include <cmath>

namespace mhe
{
	template <class T>
    class rect
    {
        private:
            vector2<T> p1, p2;
            T w_, h_;

            void do_cache()
            {
                w_ = fabs(p2.x() - p1.x());
                h_ = fabs(p2.y() - p1.y());
            }
        public:
            rect() {}
            rect(const vector2<T>& ll, const vector2<T>& rh) :
                p1(ll),
                p2(rh)
            {
                do_cache();
            }

            rect(T x, T y, T w, T h) :
                p1(x, y),
                p2(x + w, y + h),
				w_(w), h_(h)
            {
                //do_cache();
            }

			void set(T x, T y, T w, T h)
			{
				p1.set(x, y);
				p2.set(x + w, y + h);
				w_ = w;
				h_ = h;
			}

            T width() const
            {
                return w_;
            }

            T height() const
            {
                return h_;
            }

            template <class Y>
            bool in(Y x, Y y) const
            {
                if ( (x >= p1.x()) && (x <= p2.x()) &&
                     (y >= p1.y()) && (y <= p2.y()) )
                    return true;
                return false;
            }

            template <class Y>
            bool in(const vector2<Y>& v) const
            {
                return in(v.x(), v.y());
            }

			template <class Y>
			bool in_without_border(Y x, Y y) const
			{
                if ( (x > p1.x()) && (x < p2.x()) &&
                     (y > p1.y()) && (y < p2.y()) )
                    return true;
                return false;
            }

            template <class Y>
            bool in_without_border(const vector2<Y>& v) const
            {
                return in_without_border(v.x(), v.y());
            }


            rect& operator= (const rect& r)
            {
                p1 = r.p1;
                p2 = r.p2;
                w_ = r.w_;
                h_ = r.h_;
                return *this;
            }

            const vector2<T>& ll() const
            {
                return p1;
            }

            const vector2<T>& rh() const
            {
                return p2;
            }
    };
};

#endif