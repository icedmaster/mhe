using System;
using System.Collections.Generic;

namespace mhe
{
	public static class Log
	{
		public static List<string> AllData
		{
			get { return allData; }
		}

		public static void Write(string s)
		{
			Console.WriteLine(s);
			allData.Add(s);
		}

		private static List<string> allData = new List<string>();
	}
}

