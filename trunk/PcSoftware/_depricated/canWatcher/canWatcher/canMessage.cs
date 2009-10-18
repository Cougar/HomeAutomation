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
        private bool remote_request;
        private byte data_length;
        private byte[] data = new byte[8];


        public override string ToString()
        {
            return "[" + DateTime.Now.ToLongTimeString() + "]\t" + ident.ToString("X").PadLeft(8, '0') + "h\t" + (remote_request ? "Yes" : "No") + "\t" + data_length.ToString() + "\t" + byteArrayToHexString(data, data_length);
        }

        public canMessage clone()
        {
            return new canMessage(this.ident, this.extended, this.remote_request,this.data_length, this.data);
        }

        public canMessage(uint ident, bool extended, bool remote_request,byte data_length, byte[] data)
        {
            this.ident = ident;
            this.extended = extended;
            if (this.extended) this.ident &= 0x1FFFFFFF; else this.ident &= 0x000007FF;
            this.remote_request = remote_request;
            this.data_length = data_length;
            this.data = data;
        }

        public canMessage(byte[] raw, int startIndex)
        {
            this.ident = BitConverter.ToUInt32(raw, startIndex);
            this.extended = (raw[startIndex + 4] != 0);
            if (this.extended) this.ident &= 0x1FFFFFFF; else this.ident &= 0x000007FF;
            this.remote_request = (raw[startIndex + 5] != 0);
            this.data_length = raw[startIndex + 6];
            Array.Copy(raw, startIndex + 7, this.data, 0, 8);
        }

        public uint getIdent() { return ident; }
        public bool getExtended() { return extended; }
        public bool getRemoteRequest() { return remote_request; }
        public byte getDataLength() { return data_length; }
        public byte[] getData() { return data; }


        public DataGridViewRow getDataGridViewRow(DataGridView dg, KeeperTracker mt)
        {
            DataGridViewRow row = new DataGridViewRow();
            row.CreateCells(dg);

            string dgIdentifier = this.ident.ToString("X").PadLeft(8, '0') + "h";
            string dgRemoteRequest = (this.remote_request?"Yes":"No");
            string dgLength = this.data_length.ToString();
            string dgData = byteArrayToHexString(this.data, this.data_length);
            string dgPeriod = mt.getPeriod(this).ToString();
            string dgCount = mt.getCount(this).ToString();
            string dgTime = mt.getTime(this).ToLongTimeString();

            ArrayList a = new ArrayList();
            a.Add(dgIdentifier);
            a.Add(dgRemoteRequest);
            a.Add(dgLength);
            a.Add(dgData);
            a.Add(dgPeriod);
            a.Add(dgCount);
            a.Add(dgTime);
            a.Add(this.ident);
            a.Add(this);
            row.SetValues(a.ToArray());

            return row;
        }


        public static string byteArrayToHexString(byte[] data, byte data_length)
        {
            string str = "";
            /*
            for (int i = 0; i < 8; i++)
            {
                if (i < data_length)
                    str += data[data_length - i - 1].ToString("X").PadLeft(2,'0');
                else
                    str += "--";
                if (i != 8) str += " ";
            }
            */
            for (int i = 7; i >= 0; i--)
            {
                if (i > data_length-1)
                    str += "--";
                else
                    str += data[i].ToString("X").PadLeft(2, '0');
                if (i != 8) str += " ";
            }
            return str;
        }


        public byte[] getRaw()
        {
            byte[] b = new byte[serialCanConnection.PACKET_LENGTH-2];
            byte[] b2 = new byte[8];
            b2 = BitConverter.GetBytes(this.ident);
            Array.Copy(b2, 0, b, 0, 4);
            b[4] = (byte)(this.extended ? 1 : 0);
            b[5] = (byte)(this.remote_request ? 1 : 0);
            b[6] = this.data_length;
            b2 = new byte[8];
            Array.Copy(this.data, 0, b, 7, 8);
            return b;
        }

    }
}
