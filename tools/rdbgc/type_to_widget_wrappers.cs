using System;
using System.Collections.Generic;
using Gtk;

namespace mhe
{
    public abstract class WrapperBase
    {
        abstract public Gtk.Widget Widget
        {
            get;
        }

        abstract public bool IsChanged
        {
            get;
        }

        public int ObjectId
        {
            get { return objectId; }
            set { objectId = value; }
        }

        public int FieldId
        {
            get { return fieldId; }
            set { fieldId = value; }
        }

        public string ObjectType
        {
            get { return objectType; }
            set { objectType = value; }
        }

        abstract public bool Init(Container parent, string name, object val);
        abstract public byte[] Serialize();
        abstract public void Apply();

        private int objectId = -1;
        private string objectType;
        private int fieldId = -1;
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
        public override Gtk.Widget Widget
        {
            get { return checkButton; }
        }

        public override bool IsChanged
        {
            get { return checkButton.Active != value.Value; }
        }

        public override bool Init(Container parent, string name, object val)
        {
            bool? boolVal = val as bool?; 
            checkButton = new CheckButton(name);
            checkButton.Active = boolVal.Value;
            checkButton.Show();
            parent.Add(checkButton);
            value = boolVal;
            return true;
        }

        public override byte[] Serialize()
        {
            return Protocol.EncodeBool(value.Value);
        }

        public override void Apply()
        {
            value = checkButton.Active;
        }

        private CheckButton checkButton;
        private bool? value;
    }

    class TransformWrapper : WrapperBase
    {
        public override Gtk.Widget Widget
        {
            get { return transformWidget; }
        }

        public override bool IsChanged
        {
            get { return false; }
        }

        public override bool Init(Container parent, string name, object val)
        {
            transformWidget = new TransformWidget();
            transformWidget.Show();
            parent.Add(transformWidget);
            return true;
        }

        public override byte[] Serialize()
        {
            return null;
        }

        public override void Apply()
        {
        }

        private TransformWidget transformWidget;
    }
}

