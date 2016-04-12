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

            prorertyChangesTimer = new Timer(500);
            prorertyChangesTimer.Elapsed += OnPropertyChangesTimer;
            prorertyChangesTimer.Enabled = true;

            renderObjectsTreeView.AppendColumn("Objects", new Gtk.CellRendererText(), "text", 0);
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
					RequestTypesInfo();
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

		private void RequestTypesInfo()
		{
			protocol.SendGetAllTypesDataCommand("transform", (answer) =>
			{
				dataModel.FillTransformationsData(answer);
			});

			protocol.SendGetAllTypesDataCommand("light", (answer) =>
			{
				dataModel.FillLightData(answer);
                Gtk.Application.Invoke(delegate
                {
                    RebuildRenderObjectsTreeView();     
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

        private enum RenderObjectType
        {
            LIGHT = 0
        }

        private void RebuildRenderObjectsTreeView()
        {
            renderObjectsTreeStore = new Gtk.TreeStore(typeof(object));
            // lights
            Gtk.TreeIter lightsRoot = renderObjectsTreeStore.AppendValues();
            var lights = dataModel.Lights;
            foreach (var light in lights)
            {
                renderObjectsTreeStore.AppendValues(lightsRoot, light);
            }

            Gtk.TreeViewColumn column = renderObjectsTreeView.GetColumn(0);

            column.Title = "Objects";
            var renderer = new Gtk.CellRendererText();
            column.PackStart(renderer, true);
            column.SetCellDataFunc(renderer, RenderTreeViewObject);

            renderObjectsTreeView.Model = renderObjectsTreeStore;
            renderObjectsTreeView.Selection.Changed += OnRenderTreeViewSelectionChanged;
        }

        private void RenderTreeViewObject(Gtk.TreeViewColumn column, Gtk.CellRenderer cell, Gtk.TreeModel model, Gtk.TreeIter iter)
        {
            Gtk.CellRendererText cellRenderer = cell as Gtk.CellRendererText;
            var path = model.GetPath(iter);
            if (path.Depth == 1)
            {
                if (path.Indices[0] == (int)RenderObjectType.LIGHT)
                    cellRenderer.Text = "lights";
            }
            else
            {
                if (path.Indices[0] == (int)RenderObjectType.LIGHT)
                {
                    Light light = (Light)model.GetValue(iter, 0);
                    cellRenderer.Text = light.id.ToString();
                }
            }
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

        private void OnRenderTreeViewSelectionChanged(object sender, EventArgs e)
        {
            Gtk.TreeIter iter;
            if (renderObjectsTreeView.Selection.GetSelected(out iter) == false)
                return;
            var indices = renderObjectsTreeView.Selection.GetSelectedRows()[0].Indices;
            if (indices.Length != 2)
                return;
            if (indices[0] == (int)RenderObjectType.LIGHT)
            {
                var obj = (Light)renderObjectsTreeView.Model.GetValue(iter, 0);
                BuildTreeElementGUI(obj, Light.TypeName, obj.id);
            }
        }

        private void BuildTreeElementGUI<T>(T obj, string type, int objectId)
        {
            foreach (var wrapper in currentProperties)
                propertiesContainer.Remove(wrapper.Widget);
            currentProperties.Clear();
            var fields = typeof(T).GetFields();
            foreach (var field in fields)
            {
                var attributes = field.GetCustomAttributes(false);
                foreach (var attr in attributes)
                {
                    if (attr is WidgetType)
                    {
                        WidgetType widgetType = attr as WidgetType;
                        var wrapperType = widgetType.WrapperType;
                        var val = field.GetValue(obj);
                        var wrapper = WrapperCreator.Create(wrapperType, propertiesContainer, field.Name, val);
                        if (wrapper != null)
                        {
                            wrapper.ObjectId = objectId;
                            wrapper.ObjectType = type;
                            foreach (var fieldAttr in attributes)
                            {
                                FieldProtocolId fieldId = fieldAttr as FieldProtocolId;
                                if (fieldId != null)
                                {
                                    wrapper.FieldId = fieldId.FieldId;
                                    break;
                                }
                            }
                            currentProperties.Add(wrapper);
                        }
                        break;
                    }
                }
            }               
        }

        private void OnPropertyChangesTimer(Object o, ElapsedEventArgs e)
        {
            foreach (var wrapper in currentProperties)
            {
                if (wrapper.IsChanged)
                {
                    protocol.SendSetFieldCommand(wrapper.ObjectType, wrapper.ObjectId, wrapper.FieldId, wrapper.Serialize(), null);
                    wrapper.Apply();
                }
            }
        }

		private DataModel dataModel;
		private Protocol protocol;
		private List<string> history = new List<string>();
		private int historyCurrent = 0;
		private Timer pollTimer;
		private Profiler profiler = new Profiler();
		private Profiler gpuProfiler = new Profiler();
		private Settings settings;

        private Gtk.TreeStore renderObjectsTreeStore;
        private List<WrapperBase> currentProperties = new List<WrapperBase>();
        private Timer prorertyChangesTimer;
	}
}

