using System;
using System.Timers;
using System.Collections.Generic;

namespace mhe
{
	public partial class MainWindow : Gtk.Window
	{
		public MainWindow (DataModel dataModel, Protocol protocol) :
			base (Gtk.WindowType.Toplevel)
		{
			this.Build ();
			this.dataModel = dataModel;
			this.protocol = protocol;
			pollTimer = new Timer(2000);
			pollTimer.Elapsed += OnTimer;
			pollTimer.Enabled = true;
		}

		protected void OnConsoleKeyPressEvent(object o, Gtk.KeyPressEventArgs args)
		{
			if (args.Event.Key == Gdk.Key.Up)
			{
				++historyCurrent;
				UpdateConsoleFromHistory();
			} else if (args.Event.Key == Gdk.Key.Down)
			{
				--historyCurrent;
				UpdateConsoleFromHistory();
			}
			else if (args.Event.Key == Gdk.Key.Escape)
				AutoCompleteCommand();
		}

		private void UpdateConsoleFromHistory()
		{
			if (historyCurrent < 0)
			{
				historyCurrent = 0;
				entry5.Text = "";
				return;
			}
			else if (historyCurrent > historyCurrent - 1)
				historyCurrent = historyCurrent - 1;
			entry5.Text = history[historyCurrent];
		}

		private void AutoCompleteCommand()
		{
			if (dataModel == null)
				return;
			var vars = dataModel.GetGlobalVarsNames();
			foreach (var s in vars)
			{
				if (s.StartsWith(entry5.Text))
				{
					entry5.Text = s;
					break;
				}
			}
		}

		private void ProcessConsoleCommand(string cmd)
		{
			if (dataModel == null || protocol == null)
				return;
			Protocol.Command command;
			byte[] args;
			if (ConsoleParser.Parse(out command, out args, cmd, protocol, dataModel))
				protocol.SendCommand(command, args, null);
		}

		private void PrintAnswer(string answer)
		{
			label1.Text = answer;
		}

		protected void OnConsoleEntryActivated(object sender, EventArgs e)
		{
			string command = entry5.Text;
			ProcessConsoleCommand(command);
			history.Add(command);
			entry5.Text = "";
			historyCurrent = 0;
		}

		private void OnTimer(Object o, ElapsedEventArgs e)
		{
			RequestStats();
			RequestProfilerData();
		}

		private void RequestStats()
		{
			protocol.SendGetStatsCommand((answer) =>
			{
				Gtk.Application.Invoke(delegate
				{
					//statTextView.Buffer.Text = answer;
				});
			});
		}

		private void RequestProfilerData()
		{
			protocol.SendProfilerCommand((answer) =>
			{
				Gtk.Application.Invoke(delegate
				{
					//profilerTextView.Buffer.Text = protocol.ParseProfilerData(answer);
				});
			});
		}

		private DataModel dataModel;
		private Protocol protocol;
		private List<string> history = new List<string>();
		private int historyCurrent = 0;
		private Timer pollTimer;
	}
}

