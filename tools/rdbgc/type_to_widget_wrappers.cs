using System;
using System.Collections.Generic;
using Gtk;

namespace mhe
{
    interface WrapperBase
    {
        Gtk.Widget Widget
        {
            get;
        }

        bool Init(Container parent, string name, object val);
    }

    class WrapperCreator
    {
        static public WrapperBase Create(WidgetType.Type widgetType, Container parent, string name, object val)
        {
            switch (widgetType)
            {
            case WidgetType.Type.CHECKBOX:
                return CreateCheckButton(parent, name, val);
            case WidgetType.Type.TRANSFORM:
                return CreateTransformWidget(parent, name, val);
            default:
                return null;
            }
        }

        static private WrapperBase CreateCheckButton(Container parent, string name, object val)
        {
            CheckBoxWrapper wrapper = new CheckBoxWrapper();
            if (wrapper.Init(parent, name, val) == false)
                return null;
            return wrapper;
        }

        static private WrapperBase CreateTransformWidget(Container parent, string name, object val)
        {
            TransformWrapper wrapper = new TransformWrapper();
            if (wrapper.Init(parent, name, val) == false)
                return null;
            return wrapper;
        }    
    }

    class CheckBoxWrapper : WrapperBase
    {
        public Gtk.Widget Widget
        {
            get { return checkButton; }
        }

        public bool Init(Container parent, string name, object val)
        {
            bool? boolVal = val as bool?; 
            checkButton = new CheckButton(name);
            checkButton.Active = boolVal.Value;
            checkButton.Toggled += (object sender, EventArgs e) =>
            {
                value = checkButton.Active;
            };
            checkButton.Show();
            parent.Add(checkButton);
            value = boolVal;
            return true;
        }

        private CheckButton checkButton;
        private bool? value;
    }

    class TransformWrapper : WrapperBase
    {
        public Gtk.Widget Widget
        {
            get { return transformWidget; }
        }

        public bool Init(Container parent, string name, object val)
        {
            transformWidget = new TransformWidget();
            transformWidget.Show();
            parent.Add(transformWidget);
            return true;
        }

        private TransformWidget transformWidget;
    }
}

