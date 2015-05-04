using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace WSN_UI
{
    public class ComboItem
    {
        private string _displayText;
        private int _value;

        public ComboItem(string text, int value)
        {
            this._displayText = text;
            this._value = value;
        }

        public string DisplayText
        {
            get { return _displayText; }
        }

        public int Value
        {
            get { return _value; }
        }
    }
}
