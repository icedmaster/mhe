#ifndef _VECTOR2D_HPP_
#define _VECTOR2D_HPP_

namespace mhe
{
    class v2d
    {
        private:
            float x_, y_;
        public:
            v2d()
            {
                // create NULL vector
                x_ = y_ = 0.0;
            }

            v2d(float x, float y)
            {
                x_ = x; y_ = y;
            }

            v2d(const v2d& v)
            {
                x_ = v.x_;
                y_ = v.y_;
            }

            ~v2d() {} // do nothing

            inline void set(float v)
            {
                x_ = y_ = v;
            }

            inline void set(float x, float y)
            {
                x_ = x; y_ = y;
            }

            inline float x() const
            {
                return x_;
            }

            inline float y() const
            {
                return y_;
            }

            void set_x(float x)
            {
                x_ = x;
            }

            void set_y(float y)
            {
                y_ = y;
            }

            inline bool operator== (const v2d& v)
            {
                return ((x_ == v.x_) && (y_ == v.y_)) ? true : false;
            }

            inline bool operator!= (const v2d& v)
            {
                return ((x_ != v.x_) || (y_ != v.y_)) ? true : false;
            }
    };
};

#endif
