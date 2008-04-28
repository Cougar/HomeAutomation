using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;

namespace canWatcher
{
    public class logListBox : ListBox
    {
        private bool doRefresh = true;

        public override void Refresh()
        {
            if (doRefresh) base.Refresh();
        }

        protected override void RefreshItem(int index)
        {
            if (doRefresh) base.RefreshItem(index);
        }

        protected override void RefreshItems()
        {
            if (doRefresh) base.RefreshItems();
        }

        public void setDoRefresh(bool doRefresh)
        {
            this.doRefresh = doRefresh;
        }

    }
}
