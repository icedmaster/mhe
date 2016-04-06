using System;

namespace mhe
{
    [System.ComponentModel.ToolboxItem(true)]
    public partial class Vec3Widget : Gtk.Bin
    {
        public Vec3Widget()
        {
            this.Build();

            X_eventBox.ModifyBg(Gtk.StateType.Normal, new Gdk.Color(255, 0, 0));
            Y_eventBox.ModifyBg(Gtk.StateType.Normal, new Gdk.Color(0, 255, 0));
            Z_eventBox.ModifyBg(Gtk.StateType.Normal, new Gdk.Color(0, 0, 255));
            X_label.ModifyFg(Gtk.StateType.Normal, new Gdk.Color(255, 255, 255));
            Y_label.ModifyFg(Gtk.StateType.Normal, new Gdk.Color(255, 255, 255));
            Z_label.ModifyFg(Gtk.StateType.Normal, new Gdk.Color(255, 255, 255));
        }

        protected void OnZValueChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }

        protected void OnYValueChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }


        protected void OnXValueChanged(object sender, EventArgs e)
        {
            throw new NotImplementedException();
        }
    }
}

