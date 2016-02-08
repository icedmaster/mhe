using System;
using System.Timers;
using System.Collections.Generic;

namespace mhe
{
	public partial class MainWindow : Gtk.Window
	{
		public MainWindow (DataModel dataModel, Protocol protocol, Settings settings) :
			base (Gtk.WindowType.Toplevel)
		{
			this.Build ();
			this.dataModel = dataModel;
			this.protocol = protocol;
			this.settings = settings;
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
			UpdateConnectionState();
			if (!protocol.IsConnected)
			{
				protocol.Connect(settings.connectionHost, settings.connectionPort);
				protocol.SendGetAllCommand((s) =>
				{
					dataModel = new DataModel(s);
				});
			}
			else
			{
				RequestStats();
				RequestProfilerData();
			}
		}

		private void RequestStats()
		{
			protocol.SendGetStatsCommand((answer) =>
			{
				Gtk.Application.Invoke(delegate
				{
					statTextView.Buffer.Text = ProtocolHelper.DecodeStats(answer);
				});
			});
		}

		private void RequestProfilerData()
		{
			protocol.SendCPUProfilerCommand((answer) =>
			{
				Gtk.Application.Invoke(delegate
				{
					var nodes = ProtocolHelper.DecodeProfilerData(answer);
					profiler.UpdateData(nodes);
					profilerTextView.Buffer.Text = profiler.GetTextData();
				});
			});

			protocol.SendGPUProfilerCommand((answer) =>
			{
				Gtk.Application.Invoke(delegate
				{
					var nodes = ProtocolHelper.DecodeProfilerData(answer);
					gpuProfiler.UpdateData(nodes);
					GPUProfilerTextView.Buffer.Text = gpuProfiler.GetTextData();
				});
			});
		}

		private void UpdateConnectionState()
		{
			if (protocol.IsConnected)
				connectionStatusLabel.Text = "Connected";
			else
				connectionStatusLabel.Text = "Disconnected";
		}

		protected void OnImportActivated(object sender, EventArgs e)
		{
			Gtk.FileChooserDialog fileChooser = new Gtk.FileChooserDialog("Select file for import...", this, Gtk.FileChooserAction.Open,
				"Cancel", Gtk.ResponseType.Cancel, "Open", Gtk.ResponseType.Accept);
			fileChooser.Run();
			string fileToImport = fileChooser.Filename;
			fileChooser.Destroy();
		}

		protected void OnExitActivated(object sender, EventArgs e)
		{
			this.Destroy();
		}

		private DataModel dataModel;
		private Protocol protocol;
		private List<string> history = new List<string>();
		private int historyCurrent = 0;
		private Timer pollTimer;
		private Profiler profiler = new Profiler();
		private Profiler gpuProfiler = new Profiler();
		private Settings settings;
	}
}

