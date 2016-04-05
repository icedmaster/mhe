using System;

namespace mhe
{
    public class Transform
    {
        public UInt16 id;

        public UInt16 parentId;

        public Vector3 position;

        public Quat rotation;

        public Vector3 scale;

        public static Transform Deserialize(ByteStream stream)
        {
            Transform transform = new Transform();
            transform.id = Protocol.ParseUint16(stream);
            transform.parentId = Protocol.ParseUint16(stream);
            transform.position = Protocol.ParseVector3(stream);
            transform.rotation = Protocol.ParseQuat(stream);
            transform.scale = Protocol.ParseVector3(stream);

            return transform;
        }
    }

    public class AABB
    {
        public UInt16 id;

        public Vector3 center;
        public Vector3 extents;
    }

    public class TransformInstance
    {
        public Transform transform
        {
            get { return dataModel.GetTransform(id); }
        }

        public TransformInstance(DataModel dataModel, UInt16 id)
        {
            this.dataModel = dataModel;
            this.id = id;
        }

        private DataModel dataModel;
        private UInt16 id;
    }

    public class AABBInstance
    {
    }
}

