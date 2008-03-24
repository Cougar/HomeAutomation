using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace canWatcher
{
   
    public class dataHistory
    {
        private Hashtable allIdents = new Hashtable();

        public dataHistory() { }


        public void createHistory(canMessage cm)
        {
            identHistory ih = (identHistory)allIdents[cm.getIdent()];
            if (ih == null) ih = new identHistory(cm.getIdent());
            
            ih.addData(cm);
            allIdents[cm.getIdent()] = ih;
        }

        public identHistory getHistory(uint ident)
        {
            return (identHistory)allIdents[ident];
        }

        public void clearHistory(uint ident)
        {
            identHistory ih = (identHistory)allIdents[ident];
            if (ih != null) ih.clearHistory();
        }

        public Hashtable getAllHistories()
        {
            return allIdents;
        }

        public void clearAll()
        {
            allIdents = new Hashtable();
        }

    }

    public class identHistory
    {
        private const int MAX_LOG = 50;
        LinkedList<dataStamp> lst = new LinkedList<dataStamp>();


        private uint ident;

        public identHistory(uint ident) { this.ident = ident; }

        public void clearHistory()
        {
            lst = new LinkedList<dataStamp>();
        }

        public void addData(canMessage cm)
        {
           lst.AddLast(new dataStamp(cm.getData(),cm.getDataLength()));
           while(lst.Count>MAX_LOG) lst.RemoveFirst();
        }

        public LinkedList<dataStamp> getStamps()
        {
            return lst;
        }


        public uint getIdent()
        {
            return this.ident;
        }

        public class dataStamp
        {
            DateTime time;
            byte[] data;
            byte data_length;

            public dataStamp(byte[] data, byte data_length) { this.data = data; this.data_length = data_length; time = DateTime.Now; }

            public DateTime getTime() { return time; }
            public byte[] getData() { return data; }
            public byte getDataLength() { return data_length; }

            public override string ToString()
            {
                return "["+time.ToLongTimeString()+"]\t"+data_length.ToString()+"\t"+canMessage.byteArrayToHexString(data,data_length);
            }

        }


    }


}
