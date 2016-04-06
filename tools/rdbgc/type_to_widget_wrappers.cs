using System;
using System.Collections.Generic;
using Gtk;

namespace mhe
{
    class WrapperBase
    {
    }

    class WrapperCreator
    {
        private WrapperBase CreateCheckButton()
        {
            return null;
        }
    }

    class CheckBoxWrapper : WrapperBase
    {
        public CheckBoxWrapper(Container parent, string name, bool? val)
        {
            checkButton = new CheckButton(name);
            checkButton.Active = val.Value;
            checkButton.Toggled += (object sender, EventArgs e) =>
            {
                value = checkButton.Active;
            };
            parent.Add(checkButton);
        }

        private CheckButton checkButton;
        private bool? value;
    }
}

