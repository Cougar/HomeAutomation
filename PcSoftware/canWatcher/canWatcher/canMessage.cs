using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace canWatcher
{
    [Serializable]
    public class canMessage
    {
        private uint ident;
        private bool extended;
        private byte data_length;
        private byte[] data = new byte[8];

        public canMessage clone()
        {
            return new canMessage(this.ident, this.extended, this.data_length, this.data);
        }

        public canMessage(uint ident, bool extended, byte data_length, byte[] data)
        {
            this.ident = ident;
            this.extended = extended;
            if (this.extended) this.ident &= 0x1FFFFFFF; else this.ident &= 0x000007FF;
            this.data_length = data_length;
            this.data = data;
        }

        public canMessage(byte[] raw, int startIndex)
        {
            this.ident = BitConverter.ToUInt32(raw, startIndex);
            this.extended = (raw[startIndex + 4] != 0);
            if (this.extended) this.ident &= 0x1FFFFFFF; else this.ident &= 0x000007FF;
            this.data_length = raw[startIndex + 5];
            Array.Copy(raw, startIndex + 6, this.data, 0, 8);
        }

        public uint getIdent() { return ident; }
        public bool getExtended() { return extended; }
        public byte getDataLength() { return data_length; }
        public byte[] getData() { return data; }


        public DataGridViewRow getDataGridViewRow(DataGridView dg, KeeperTracker mt)
        {
            DataGridViewRow row = new DataGridViewRow();
            row.CreateCells(dg);

            string dgIdentifier = this.ident.ToString("X").PadLeft(8, '0') + "h";
            string dgLength = this.data_length.ToString();
            string dgData = byteArrayToHexString(this.data, this.data_length);
            string dgPeriod = mt.getPeriod(this).ToString();
            string dgCount = mt.getCount(this).ToString();

            ArrayList a = new ArrayList();
            a.Add(dgIdentifier);
            a.Add(dgLength);
            a.Add(dgData);
            a.Add(dgPeriod);
            a.Add(dgCount);
            a.Add(this.ident);
            a.Add(this);
            row.SetValues(a.ToArray());

            return row;
        }


        private string byteArrayToHexString(byte[] data, byte data_length)
        {
            string str = "";

            for (int i = 0; i < 8; i++)
            {
                if (i < data_length)
                    str += data[data_length - i - 1].ToString("X").PadLeft(2,'0');
                else
                    str += "--";
                if (i != 8) str += " ";
            }

            return str;
        }


        public byte[] getRaw()
        {
            byte[] b = new byte[14];
            byte[] b2 = new byte[8];
            b2 = BitConverter.GetBytes(this.ident);
            Array.Copy(b2, 0, b, 0, 4);
            b[4] = (byte)(this.extended ? 1 : 0);
            b[5] = this.data_length;
            b2 = new byte[8];
            Array.Copy(this.data, 0, b, 6, 8);
            //Array.Copy(this.data, 0, b2, 0, 8);
            //Array.Reverse(b2);
            //Array.Copy(b2, 0, b, 6, 8);
            return b;
        }

    }
}
