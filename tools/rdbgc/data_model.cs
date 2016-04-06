using System;
using System.Collections.Generic;

namespace mhe
{
	public enum Type
	{
		None = 0,
		Int = 1,
		Float = 2,
		String = 3,
		Vec3 = 4,
		Vec4 = 5
	};

	public class GlobalVar
	{
		public string Name
		{
			get { return name; }
		}

		public Type VarType
		{
			get { return type; }
		}

		static public bool Check(Type type, string[] arg)
		{
			switch (type)
			{
			case Type.None:
				return false;
			case Type.Int:
				{
					if (arg.Length != 1)
						return false;
					int tmp;
					return int.TryParse(arg[0], out tmp);
				}
			case Type.Float:
				{
					if (arg.Length != 1)
						return false;
					float tmp;
					return float.TryParse(arg[0], out tmp);
				}
			case Type.String:
				return true;
			case Type.Vec3:
			case Type.Vec4:
				return false;
			}
			return false;
		}

		public GlobalVar(string name, int type)
		{
			this.name = name;
			this.type = (Type)type;
		}

		public bool IsCorrect(string[] arg)
		{
			return Check (type, arg);
		}

		private string name;
		private Type type;
	}

	public class DataModel
	{
        public Dictionary<UInt16, Light>.ValueCollection Lights
        {
            get { return lights.Values; }
        }

		public DataModel(byte[] data)
		{
			FillGlobalVars(data);
			Console.Write(string.Format("Received {0} global vars", globalVars.Count)); 
		}

		public GlobalVar GetGlobalVar(string name)
		{
			if (!globalVars.ContainsKey(name))
				return null;
			return globalVars[name];
		}

		public string[] GetGlobalVarsNames()
		{
			string[] res = new string[globalVars.Count];
			globalVars.Keys.CopyTo(res, 0);
			return res;
		}

		public void FillTransformationsData(byte[] data)
		{
			var stream = new ByteStream(data);
			int count = Protocol.ParseInt(stream);
			for (int i = 0; i < count; ++i)
			{
				Transform transform = Transform.Deserialize(stream);
				xforms.Add(transform.id, transform);
			}
		}

		public void FillLightData(byte[] data)
		{
			var stream = new ByteStream(data);
			int count = Protocol.ParseInt(stream);
			for (int i = 0; i < count; ++i)
			{
				var light = Light.Deserialize(this, stream);
				lights.Add(light.id, light);
			}
		}

		public Transform GetTransform(UInt16 id)
		{
			if (!xforms.ContainsKey(id))
				return null;
			return xforms[id];
		}

		private void FillGlobalVars(byte[] data)
		{
			ByteStream stream = new ByteStream(data);
			while (stream.HasData)
			{
				string varName = Protocol.ParseString(stream);
				int varType = Protocol.ParseInt(stream);
				globalVars.Add(varName, new GlobalVar(varName, varType));
			}
		}

		private Dictionary<string, GlobalVar> globalVars = new Dictionary<string, GlobalVar>();

		private Dictionary<UInt16, Transform> xforms = new Dictionary<UInt16, Transform>();
		private Dictionary<UInt16, Light> lights = new Dictionary<UInt16, Light>();
	}
}

