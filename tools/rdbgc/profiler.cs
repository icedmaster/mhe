using System;
using System.Collections.Generic;

namespace mhe
{
	public class ProfilerNode
	{
		public int id;
		public int parentId;
		public string name;
		public int count;
		public float interval;
	};

	public class Profiler
	{
		public void UpdateData(List<ProfilerNode> nodes)
		{
			this.nodes = nodes;
		}

		public string GetTextData()
		{
			string data = "";
			foreach (var n in nodes)
			{
				data = data + n.name + ":" + n.interval.ToString("0.000") + "\n";
			}

			return data;
		}

		private List<ProfilerNode> nodes = new List<ProfilerNode>();
	}
}

