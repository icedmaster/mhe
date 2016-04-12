using System;
using System.Collections.Generic;

namespace mhe
{
    [System.AttributeUsage(System.AttributeTargets.Field)]
    public class WidgetType : System.Attribute
    {
        public enum Type
        {
            NONE,
            CHECKBOX,
            SLIDER,
            TRANSFORM,
            COMBOBOX
        }

        public Type WrapperType
        {
            get { return type; }
        }

        public WidgetType(Type type)
        {
            this.type = type;
        }

        private Type type = Type.NONE;
    }

    [System.AttributeUsage(System.AttributeTargets.Field)]
    public class FieldProtocolId : System.Attribute
    {
        public int FieldId
        {
            get { return fieldId; }
        }

        public FieldProtocolId(int id)
        {
            this.fieldId = id;
        }

        private int fieldId = -1;
    }
}

