using System;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;

namespace mhe
{
	public class ByteStream
	{
		public ByteStream(byte[] b)
		{
			data = b;
		}

		public bool HasData
		{
			get { return pos < data.Length; }
		}

		public string Read(int num)
		{
			return Encoding.UTF8.GetString(ReadBytes(num));
		}

		public byte[] ReadBytes(int num)
		{
			byte[] res = new byte[num];
			Array.ConstrainedCopy(data, pos, res, 0, num);
			pos += num;
			return res;
		}

		private byte[] data;
		private int pos = 0;
	}

	public class Protocol
	{
		public enum Command
		{
			TEXT,
			SET, GET,
			SET_VAR, GET_VAR,
			GET_ALL_VARS,
			GET_PROFILER_DATA,
			GET_STATS
		};

		public enum Result
		{
			OK,
			ERROR
		};

		private const int CMD_LENGTH = 1;

		public delegate void OnAnswer(byte[] data);

		public static string ParseString(ByteStream stream)
		{
			byte[] len = stream.ReadBytes(4);
			return stream.Read(ConvertToInt(len));
		}

		public static int ParseInt(ByteStream stream)
		{
			return ConvertToInt(stream.ReadBytes(4));
		}

		public static byte[] EncodeString(string s)
		{
			byte[] res = new byte[4 + s.Length];
			Array.Copy(ConvertToBytes(s.Length), res, 4);
			Array.Copy(Encoding.UTF8.GetBytes(s), 0, res, 4, s.Length);
			return res;
		}

		public static byte[] EncodeIntFromString(string s)
		{
			int res;
			if (!int.TryParse(s, out res))
				return null;
			return ConvertToBytes(res);
		}

		public bool IsConnected
		{
			get { return client.Connected; }
		}

		public bool Connect(string host, int port)
		{
			try
			{
				client = new TcpClient(host, port);
			}
			catch (Exception e)
			{
				Log.Write("Cought an exception during the Connect() method:" + e.ToString());
				return false;
			}

			return true;
		}

		public void SendGetAllCommand(OnAnswer onAnswer)
		{
			SendCommand(Command.GET_ALL_VARS, null, onAnswer);
		}

		public void SendGetStatsCommand(OnAnswer onAnswer)
		{
			SendCommand(Command.GET_STATS, null, onAnswer);
		}

		public void SendProfilerCommand(OnAnswer onAnswer)
		{
			//SendCommand(PROFILER_COMMAND, null, onAnswer, false);
		}

		public void SendSetVarCommand(byte[] varData, OnAnswer onAnswer)
		{
			SendCommand(Command.SET_VAR, varData, onAnswer);
		}

		public void SendCommand(Command cmd, byte[] args, OnAnswer onAnswer)
		{
			Log.Write(">> cmd:" + cmd.ToString() + " args:" + args);
			Send(cmd, args);
			Result result = Result.ERROR;
			byte[] answer = null;
			if (Receive(ref result, out answer))
			{
				Log.Write("<< res:" + result.ToString() + " args:" + answer);
				if (onAnswer != null) onAnswer(answer);
			}
		}

		public bool IsGlobalCommand(Command cmd)
		{
			Command[] GLOBAL_COMMANDS = new Command[] {Command.GET_ALL_VARS, Command.GET_PROFILER_DATA, Command.GET_STATS};
			return Array.FindIndex(GLOBAL_COMMANDS, (s) =>
			{
				return cmd == s;
			}) >= 0;
		}

		public string ParseProfilerData(string data)
		{
			var profilerData = ParseProfilerDataString(data);
			string result = "";
			foreach (var pd in profilerData)
			{
				result += pd.name;
				result += " ";
				result += pd.interval;
				result += "\n";
			}
			return result;
		}

		private List<ProfilerData> ParseProfilerDataString(string data)
		{
			List<ProfilerData> profilerData = new List<ProfilerData>();
			var tokens = data.Split(' ');
			for (int i = 0; i < tokens.Length; i += 5)
			{
				ProfilerData newData = new ProfilerData()
				{
					name = tokens[i + 0],
					pointId = tokens[i + 1],
					parentId = tokens[i + 2],
					count = tokens[i + 3],
					interval = tokens[i + 4]
				};
				profilerData.Add(newData);
			}
			return profilerData;
		}

		private bool Send(Command cmd, byte[] data)
		{
			byte[] d = data;
			int len = d != null ? d.Length : 0;
			byte[] lenBuffer = ConvertToBytes(len + CMD_LENGTH);
			byte[] cmdb = new byte[] { (byte)cmd };
			try
			{
				client.GetStream().Write(lenBuffer, 0, 4);
				client.GetStream().Write(cmdb, 0, 1);
				if (len > 0) client.GetStream().Write(d, 0, len);
			}
			catch (Exception e)
			{
				Log.Write("Cought an exception during the Send() method:" + e.ToString());
				return false;
			}
			return true;
		}

		private bool Receive(ref Result result, out byte[] data)
		{
			byte[] lenBuffer = new byte[4];
			byte[] cmdBuffer = new byte[1];
			data = null;
			try
			{
				var stream = client.GetStream();
				int read = stream.Read(lenBuffer, 0, 4);
				if (read < 4) return false;
				int len = ConvertToInt(lenBuffer);
				if (len < 1) return false;
				read = stream.Read(cmdBuffer, 0, 1);
				if (read < 1) return false;
				result = (Result)cmdBuffer[0];
				--len;
				int current = 0;
				data = new byte[len];
				while (current < len)
				{
					read = stream.Read(data, 0, data.Length);
					if (read <= 0) return false;
					current += read;
				}
			}
			catch (Exception e)
			{
				Log.Write("Cought an exception during the Receive() method:" + e.ToString());
				return false;
			}

			return true;
		}

		private static int ConvertToInt(byte[] data)
		{
			return (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
		}

		private static byte[] ConvertToBytes(int n)
		{
			byte[] b = new byte[4];
			b[0] = (byte)(n >> 24);
			b[1] = (byte)(n >> 16);
			b[2] = (byte)(n >> 8);
			b[3] = (byte)(n & 0xff);
			return b;
		}

		private class ProfilerData
		{
			public string name;
			public string pointId;
			public string parentId;
			public string count;
			public string interval;
		};

		private TcpClient client;
	}
}
