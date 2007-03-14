using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.Collections;
using System.IO;
using System.Runtime.Serialization;
using System.Runtime.Serialization.Formatters.Binary;

namespace canConfigurator
{

    public partial class frmTypedefs : Form
    {
        const int MAX_CLASSES = 16;
        const int MAX_IDS = 32768;
        private typeClass currentClass;

        public frmTypedefs()
        {
            InitializeComponent();
        }

        private void frmTypedefs_Load(object sender, EventArgs e)
        {
            loadDefinitions();

        }

        private void saveDefinitions()
        {
            IFormatter formatter = new BinaryFormatter();
            Stream st = File.OpenWrite(Application.StartupPath + "/typedefs.dat");
            Hashtable temphash = new Hashtable();
            List<typeClass> classes = new List<typeClass>();
            foreach (typeClass tc in lstClasses.Items)
            {
                classes.Add(tc);
            }
            temphash["CsharpPath"] = txtCsharpPath.Text;
            temphash["CPath"] = txtCPath.Text;
            temphash["classhash"] = classes;
            formatter.Serialize(st, temphash);
            st.Close();
        }

        private void loadDefinitions()
        {
            IFormatter formatter = new BinaryFormatter();
            if (File.Exists(Application.StartupPath + "/typedefs.dat"))
            {
                Stream st = File.OpenRead(Application.StartupPath + "/typedefs.dat");
                Hashtable temphash = (Hashtable)formatter.Deserialize(st);
                if (temphash != null)
                {
                    List<typeClass> classes = (List<typeClass>)temphash["classhash"];
                    if (classes != null)
                    {
                        foreach (typeClass tc in classes)
                        {
                            lstClasses.Items.Add(tc);
                        }
                    }
                    txtCsharpPath.Text = (temphash["CsharpPath"] == null ? "" : (string)temphash["CsharpPath"]);
                    txtCPath.Text = (temphash["CPath"] == null ? "" : (string)temphash["CPath"]);
                }
                st.Close();
            }
        }

        private void cmdClassAdd_Click(object sender, EventArgs e)
        {
            string name = txtClassName.Text.Trim().ToUpper();
            string comment = txtClassComment.Text.Trim();

            if (!checkClassField(name, comment)) return;

            if (lstClasses.Items.Count >= MAX_CLASSES)
            {
                MessageBox.Show("You cannot add more classes. Maximum of " + MAX_CLASSES.ToString() + " classes reached.", "Limit", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            typeClass typeclass = new typeClass(name, comment);

            // Get first free value
            bool[] takenvalues = new bool[MAX_CLASSES];
            foreach (typeClass tp in lstClasses.Items)
            {
                takenvalues[tp.value] = true;
            }
            for (int i = 0; i < MAX_CLASSES; i++)
            {
                if (!takenvalues[i])
                {
                    typeclass.value = i;
                    break;
                }
            }

            lstClasses.Items.Add(typeclass);
        }

        private void cmdClassUpdate_Click(object sender, EventArgs e)
        {
            typeClass typeclass = (typeClass)lstClasses.SelectedItem;
            if (typeclass == null)
            {
                MessageBox.Show("You need to select a class to update.", "Missing class", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            string name = txtClassName.Text.Trim().ToUpper();
            string comment = txtClassComment.Text.Trim();

            if (!checkClassField(name, comment, typeclass.name)) return;

            typeclass.name = name;
            typeclass.comment = comment;

        }

        private void cmdClassDelete_Click(object sender, EventArgs e)
        {
            typeClass typeclass = (typeClass)lstClasses.SelectedItem;
            if (typeclass == null)
            {
                MessageBox.Show("You need to select a class to delete.", "Missing class", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            currentClass = null;
            lstClasses.Items.Remove(typeclass);
            txtClassName.Text = "";
            txtClassComment.Text = "";
        }


        private bool checkClassField(string name, string comment)
        {
            return checkClassField(name, comment, "");
        }

        private bool checkClassField(string name, string comment, string oldName)
        {
            Regex rgxIsValidChars = new Regex("[^A-Za-z0-9_\\-]");

            if (name.Length == 0)
            {
                MessageBox.Show("Name field is empty.", "Emtpy field", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            if (rgxIsValidChars.IsMatch(name))
            {
                MessageBox.Show("Name has invalid characters.", "Invalid chars", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            typeClass typeclass = new typeClass(name, comment);

            if (oldName != name && lstClasses.Items.Contains(typeclass))
            {
                MessageBox.Show("A class with that name alredy exists.", "Dupes", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            return true;
        }

        private bool checkIdField(string name, string comment)
        {
            return checkIdField(name, comment, "");
        }

        private bool checkIdField(string name, string comment, string oldName)
        {
            Regex rgxIsValidChars = new Regex("[^A-Za-z0-9_\\-]");

            if (name.Length == 0)
            {
                MessageBox.Show("Name field is empty.", "Emtpy field", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            if (rgxIsValidChars.IsMatch(name))
            {
                MessageBox.Show("Name has invalid characters.", "Invalid chars", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            typeId typeid = new typeId(name, comment);

            if (oldName != name && currentClass.getIds().Contains(typeid))
            {
                MessageBox.Show("A id for this class with that name alredy exists.", "Dupes", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return false;
            }

            return true;
        }


        private void lstClasses_SelectedIndexChanged(object sender, EventArgs e)
        {
            typeClass typeclass = (typeClass)lstClasses.SelectedItem;
            if (typeclass != null)
            {
                txtClassName.Text = typeclass.name;
                txtClassComment.Text = typeclass.comment;
                txtIdComment.Text = "";
                txtIdName.Text = "";
                currentClass = typeclass;
                lstIds.Items.Clear();
                foreach (typeId ti in typeclass.getIds())
                {
                    lstIds.Items.Add(ti);
                }
            }
        }

        private void lstIds_SelectedIndexChanged(object sender, EventArgs e)
        {
            typeId typeid = (typeId)lstIds.SelectedItem;
            if (typeid != null)
            {
                txtIdName.Text = typeid.name;
                txtIdComment.Text = typeid.comment;
            }
        }

        private void cmdIdAdd_Click(object sender, EventArgs e)
        {
            if (currentClass == null)
            {
                MessageBox.Show("You need to select a class first.", "Missing class", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            string name = txtIdName.Text.Trim().ToUpper();
            string comment = txtIdComment.Text.Trim();

            if (!checkIdField(name, comment)) return;

            if (currentClass.getIds().Count >= MAX_IDS)
            {
                MessageBox.Show("You cannot add more ids. Maximum of " + MAX_IDS.ToString() + " classes reached.", "Limit", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            typeId typeid = new typeId(name, comment);

            // Get first free value
            bool[] takenvalues = new bool[MAX_IDS];
            foreach (typeId ti in currentClass.getIds())
            {
                takenvalues[ti.value] = true;
            }
            for (int i = 0; i < MAX_IDS; i++)
            {
                if (!takenvalues[i])
                {
                    typeid.value = i;
                    break;
                }
            }

            currentClass.getIds().Add(typeid);
            lstIds.Items.Add(typeid);
        }

        private void cmdIdUpdate_Click(object sender, EventArgs e)
        {
            if (currentClass == null)
            {
                MessageBox.Show("You need to select a class first.", "Missing class", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            typeId typeid = (typeId)lstIds.SelectedItem;
            if (typeid == null)
            {
                MessageBox.Show("You need to select a Id to update.", "Missing id", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            string name = txtIdName.Text.Trim().ToUpper();
            string comment = txtIdComment.Text.Trim();

            if (!checkIdField(name, comment, typeid.name)) return;

            typeid.name = name;
            typeid.comment = comment;
        }

        private void cmdIdDelete_Click(object sender, EventArgs e)
        {
            if (currentClass == null)
            {
                MessageBox.Show("You need to select a class first.", "Missing class", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            typeId typeid = (typeId)lstIds.SelectedItem;
            if (typeid == null)
            {
                MessageBox.Show("You need to select a id to delete.", "Missing id", MessageBoxButtons.OK, MessageBoxIcon.Information);
                return;
            }

            lstIds.Items.Remove(typeid);
            currentClass.getIds().Remove(typeid);
            txtIdName.Text = "";
            txtIdComment.Text = "";
        }

        private void cmdBrowseCsharp_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                txtCsharpPath.Text = sfd.FileName;
            }
        }

        private void cmdBrowseC_Click(object sender, EventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            if (sfd.ShowDialog() == DialogResult.OK)
            {
                txtCPath.Text = sfd.FileName;
            }
        }

        private void cmdGenerate_Click(object sender, EventArgs e)
        {
            generate();
            saveDefinitions();

            MessageBox.Show("Generation completed!", "Done", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void generate()
        {
            string PACKET_TYPE = "ptBOOT=0x0, ptPGM=0x1";
            string BOOT_TYPE = "btADDR = 0x0, btPGM = 0x1, btDONE = 0x2, btACK = 0x4, btNACK = 0x5, btCRC = 0x6, btCHANGEID = 0x7";

            string CSharp = "";
            string C = "";

            string PGM_CLASS = "";

            string classTypeC = "";
            string classTypeCSharp = "";

            int classCount = lstClasses.Items.Count;
            foreach (typeClass tc in lstClasses.Items)
            {
                PGM_CLASS += tc.getNameValuePair();
                if (--classCount > 0) PGM_CLASS += ", ";

                string ids = "";
                string comments = "";
                int idCount = tc.getIds().Count;
                foreach (typeId ti in tc.getIds())
                {
                    ids += ti.getNameValuePair(tc.name);
                    comments += ti.getComment(tc.name);
                    if (--idCount > 0) ids += ", ";
                }
                classTypeC += tc.getHeader(typeClass.syntax.C) + ids + tc.getFooter(typeClass.syntax.C) + "\r\n" + comments + "\r\n\r\n";
                classTypeCSharp += tc.getHeader(typeClass.syntax.CSHARP) + ids + tc.getFooter(typeClass.syntax.CSHARP) + "\r\n" + comments + "\r\n\r\n";
            }


            CSharp = "namespace canConfigurator{\r\n\tpublic class td{\r\n";
            CSharp += "public enum PACKET_TYPE : byte { " + PACKET_TYPE + " };\r\n";
            CSharp += "public enum BOOT_TYPE : byte { " + BOOT_TYPE + " };\r\n";
            CSharp += "public enum PGM_CLASS : byte { " + PGM_CLASS + " };\r\n\r\n";
            CSharp += classTypeCSharp;
            CSharp += "\t}\r\n}";
            C = "typedef enum _PACKET_TYPE { " + PACKET_TYPE + " } PACKET_TYPE;\r\n";
            C += "typedef enum _BOOT_TYPE { " + BOOT_TYPE + " } BOOT_TYPE;\r\n";
            C += "typedef enum _PGM_CLASS {" + PGM_CLASS + "} PGM_CLASS;\r\n\r\n";
            C += classTypeC;

            writeFile(txtCPath.Text, C);
            writeFile(txtCsharpPath.Text, CSharp);
        }

        private void writeFile(string path, string content)
        {
            if (File.Exists(path))
            {
                File.Copy(path, path + ".bck", true);
            }
            TextWriter tw = new StreamWriter(path,false);
            tw.Write(content);
            tw.Close();
        }


    }

    [Serializable]
    internal class typeClass
    {
        public enum syntax { C, CSHARP };

        private List<typeId> typeIds;
        public string name;
        public string comment;
        public int value = -1;

        public typeClass(string name, string comment)
        {
            this.name = name.Trim().ToUpper();
            this.comment = comment;
            this.typeIds = new List<typeId>();
        }

        public typeClass(string name) : this(name, "") { }

        public override string ToString()
        {
            return this.name;
        }

        public override bool Equals(object obj)
        {
            if (obj == null || obj == System.DBNull.Value) return false;

            return this.name.Equals(((typeClass)obj).name);
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }

        public bool addId(typeId id)
        {
            typeIds.Add(id);

            return true;
        }

        public List<typeId> getIds()
        {
            return typeIds;
        }


        internal string getNameValuePair()
        {
            return "pc" + this.name + "=0x" + this.value.ToString("X");
        }

        internal string getHeader(syntax s)
        {
            switch (s)
            {
                case syntax.C: return "/*" + this.comment + "*/\r\ntypedef enum _PGM_" + this.name + "_TYPE {";
                case syntax.CSHARP: return "/*" + this.comment + "*/\r\npublic enum PGM_" + this.name + "_TYPE : ushort {";
            }
            return "";
        }

        internal string getFooter(syntax s)
        {
            switch (s)
            {
                case syntax.C: return "} PGM_" + this.name + "_TYPE;";
                case syntax.CSHARP: return "};";
            }
            return "";
        }
    }

    [Serializable]
    internal class typeId
    {
        public int value = -1;
        public string name;
        public string comment;

        public typeId(string name, string comment)
        {
            this.name = name.Trim().ToUpper();
            this.comment = comment;
        }

        public typeId(string name) : this(name, "") { }

        public override string ToString()
        {
            return this.name;
        }

        public override bool Equals(object obj)
        {
            if (obj == null || obj == System.DBNull.Value) return false;

            return this.name.Equals(((typeId)obj).name);
        }

        public override int GetHashCode()
        {
            return base.GetHashCode();
        }



        internal string getNameValuePair(string className)
        {
            return "p" + className.Substring(0, 1).ToLower() + "t" + this.name + "=0x" + this.value.ToString("X").PadLeft(4, '0');
        }

        internal string getComment(string className)
        {
            return "/*\r\n" + "p" + className.Substring(0, 1).ToLower() + "t" + this.name + "\r\n" + this.comment + "\r\n*/\r\n";
        }
    }

}