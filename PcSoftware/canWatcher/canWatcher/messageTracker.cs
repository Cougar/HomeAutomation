using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace canWatcher
{
    [Serializable]
    public class messageTracker : KeeperTracker
    {
        Hashtable incommingMessages = new Hashtable();
        Hashtable allMessages = new Hashtable();

        public messageTracker() { }

        public void addMessage(canMessage cm)
        {
            allMessages[cm.getIdent()] = cm;
            stamp(cm);
        }

        public Hashtable getMessages()
        {
            return allMessages;
        }

        private void stamp(canMessage cm)
        {
            messageState ms = (messageState)incommingMessages[cm.getIdent()];
            if (ms == null)
            {
                ms = new messageState();
            }
            ms.stamp();
            incommingMessages[cm.getIdent()] = ms;

        }

        public long getPeriod(canMessage cm)
        {
            messageState ms = (messageState)incommingMessages[cm.getIdent()];
            if (ms == null) return 0;
            return ms.period;
        }
        public int getCount(canMessage cm)
        {

            messageState ms = (messageState)incommingMessages[cm.getIdent()];

            if (ms == null) return 0;
            return ms.count;
        }

        [Serializable]
        internal class messageState
        {
            public int count;
            public long period;

            private long lastStamp;

            public messageState()
            {
                lastStamp = Environment.TickCount;
                this.count = 0;
                this.period = 0;
            }

            public void stamp()
            {
                this.count++;
                this.period = Environment.TickCount - lastStamp;
                lastStamp = Environment.TickCount;
            }

        }

    }
}
