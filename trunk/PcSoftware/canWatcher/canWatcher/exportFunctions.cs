using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;

namespace canWatcher
{
    static public class exportFunctions
    {
        public enum exportFormat{TXT,XML};

        public static void exportToFile(string filerstr,string titlestr,ListBox lst,exportFormat xf)
        {

            SaveFileDialog sf = new SaveFileDialog();
            sf.AddExtension = true;
            sf.Filter = filerstr;
            sf.Title = titlestr;
            sf.CheckPathExists = true;
            if (sf.ShowDialog() == DialogResult.OK)
            {
                FileStream file;

                try
                {
                    file = new FileStream(sf.FileName, FileMode.Create, FileAccess.Write);
                }
                catch (Exception e)
                {

                    MessageBox.Show("Error occured during export. Error message: " + e.Message, "Exporting error", MessageBoxButtons.OK, MessageBoxIcon.Exclamation);
                    return;
                }

                StreamWriter sw = new StreamWriter(file);

                foreach (Object cm in lst.Items) sw.WriteLine(cm.ToString());

                sw.Close();
            }
        }

    }
}
