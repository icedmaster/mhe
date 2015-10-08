using System;
using System.Collections.Generic;

namespace mhe
{
	public static class ProtocolHelper
	{
		public static string DecodeStats(byte[] data)
		{
			ByteStream stream = new ByteStream(data);
			int viewsNumber = Protocol.ParseInt(stream);
			string res = "";
			for (int i = 0; i < viewsNumber; ++i)
			{
				string viewName = Protocol.ParseString(stream);
				res += viewName;
				res += "\n";
				int fieldsNumber = Protocol.ParseInt(stream);
				for (int j = 0; j < fieldsNumber; ++j)
				{
					string fieldName = Protocol.ParseString(stream);
					Type fieldType = (Type)Protocol.ParseInt(stream);
					int valueLength = Protocol.ParseInt(stream);
					string fieldValue = ValueToString(fieldType, valueLength, stream);
					res += string.Format("\t" + fieldName + ":" + fieldValue + "\n");
				}
				res += "\n";
			}

			return res;
		}

		public static List<ProfilerNode> DecodeProfilerData(byte[] data)
		{
			List<ProfilerNode> nodes = new List<ProfilerNode>();

			ByteStream stream = new ByteStream(data);
			Protocol.ProfileDataType profilerType = (Protocol.ProfileDataType)stream.ReadBytes(1)[0];
			int fieldsNumber = Protocol.ParseInt(stream);
			for (int i = 0; i < fieldsNumber; ++i)
			{
				int nodeId = Protocol.ParseInt(stream);
				int nodeParentId = Protocol.ParseInt(stream);
				string nodeName = Protocol.ParseString(stream);
				int nodeCount = Protocol.ParseInt(stream);
				float nodeInterval = Protocol.ParseFloat(stream); // in seconds

				ProfilerNode node = new ProfilerNode {
					id = nodeId,
					parentId = nodeParentId,
					name = nodeName,
					count = nodeCount,
					interval = nodeInterval
				};

				if (profilerType == Protocol.ProfileDataType.CPU)
					node.interval *= 1000.0f;

				nodes.Add(node);
			}

			return nodes;
		}

		public static string ValueToString(Type type, int valueLength, ByteStream stream)
		{
			switch (type)
			{
			case Type.Int: 
				{
					if (valueLength != 4)
						return "";
					int val = Protocol.ParseInt(stream);
					return val.ToString();
				}
			default:
				return "";
			}
		}
	}
}

