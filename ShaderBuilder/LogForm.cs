using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace ShaderBuilder
{
    public partial class LogForm : Form
    {
        public LogForm()
        {
            InitializeComponent();
        }

        internal void Clear()
        {
            label1.Text = "";
        }

        internal void AddText(string text)
        {
            label1.Text = label1.Text + "\n" + text;
        }

        private void buttonToClip_Click(object sender, EventArgs e)
        {
            Clipboard.SetText(label1.Text);
        }
    }
}
