using System;
using System.Collections.Generic;

namespace mhe
{
	public static class ConsoleParser
	{
		public static bool Parse(out Protocol.Command command, out byte[] args, string cmdline, Protocol protocol, DataModel dataModel)
		{
			var splitted = cmdline.Split('=');
			for (int i = 0; i < splitted.Length; ++i)
				splitted[i] = splitted[i].Trim();

			if (splitted.Length == 1)
			{
				return ParseGetCommand(out command, out args, splitted, protocol, dataModel);
			}
			else
				return ParseSetCommand (out command, out args, splitted, protocol, dataModel);
		}

		static bool ParseGetCommand(out Protocol.Command command, out byte[] args, string[] cmdline, Protocol protocol, DataModel dataModel)
		{
			command = Protocol.Command.GET_VAR;
			args = null;
			var globalVar = GetGlobalVar(cmdline, dataModel);
			if (globalVar == null)
				return false; // not implemented yet
			return true;
		}

		static bool ParseSetCommand(out Protocol.Command command, out byte[] args, string[] cmdline, Protocol protocol, DataModel dataModel)
		{
			command = Protocol.Command.SET_VAR;
			args = null;
			var globalVar = GetGlobalVar(cmdline, dataModel);
			if (globalVar == null)
				return false; // not implemented yet
			string[] varvalue = new string[cmdline.Length - 1];
			for (int i = 1; i < cmdline.Length; ++i)
				varvalue[i - 1] = cmdline[i];
			if (!globalVar.IsCorrect(varvalue))
				return false;
			byte[] varName = Protocol.EncodeString(globalVar.Name);
			byte[] varValue = EncodeVarValue(globalVar, varvalue);
			if (varValue == null)
				return false;
			args = new byte[varName.Length + varValue.Length];
			Array.Copy(varName, args, varName.Length);
			Array.Copy(varValue, 0, args, varName.Length, varValue.Length); 
			return true;
		}

		static byte[] EncodeVarValue(GlobalVar globalVar, string[] cmdline)
		{
			switch (globalVar.VarType)
			{
			case Type.Int:
				return Protocol.EncodeIntFromString(cmdline[0]);
			case Type.String:
				return Protocol.EncodeString(cmdline[0]);
			default:
				return null;
			}
		}

		static GlobalVar GetGlobalVar(string[] cmdline, DataModel dataModel)
		{
			return dataModel.GetGlobalVar(cmdline[0]);
		}
	}
}

