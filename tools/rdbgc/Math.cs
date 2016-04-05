using System;

namespace mhe
{
    public class Vector3
    {
        public float X
        {
            get { return x; }
            set { x = value; }
        }

        public float Y
        {
            get { return y; }
            set { y = value; }
        }

        public float Z
        {
            get { return z; }
            set { z = value; }
        }

        private float x;
        private float y;
        private float z;
    }

    public class Quat
    {
        static Quat FromEuler(float yaw, float pitch, float roll)
        {
            return Identity();
        }

        public float Yaw
        {
            get { return x; }
        }

        public float Pitch
        {
            get { return y; }
        }

        public float Roll
        {
            get { return z; }
        }

        public Quat(float x, float y, float z, float w)
        {
            this.x = x;
            this.y = y;
            this.z = z;
            this.w = w;
        }

        public static Quat Identity()
        {
            Quat q = new Quat(0, 0, 0, 1);
            return q;
        }

        private float x;
        private float y;
        private float z;
        private float w;
    }

    public class Color
    {
        public Color(float r, float g, float b, float a)
        {
            this.r = r;
            this.g = g;
            this.b = b;
            this.a = a;
        }

        private float r;
        private float g;
        private float b;
        private float a;
    }
}

