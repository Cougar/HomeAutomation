using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace canWatcher
{
    [Serializable]
    public class outgoingKeeper : KeeperTracker
    {
        Hashtable outgoingMessages = new Hashtable();

        public outgoingKeeper() { }

        public void addMessage(canMessage cm, long period)
        {
            outgoingMessages[cm] = new messageState(period);
        }

        public void deleteMessage(canMessage cm)
        {
            outgoingMessages.Remove(cm);
        }

        public Hashtable getMessages()
        {
            return outgoingMessages;
        }

        public long getPeriod(canMessage cm)
        {
            messageState ms = (messageState)outgoingMessages[cm];
            if (ms == null) return 0;
            return ms.period;
        }

        public int getCount(canMessage cm)
        {
            messageState ms = (messageState)outgoingMessages[cm];
            if (ms == null) return 0;
            return ms.count;
        }

        public DateTime getTime(canMessage cm)
        {
            messageState ms = (messageState)outgoingMessages[cm];

            if (ms == null) return new DateTime();
            return ms.timestamp;
        }

        public bool timeToSend(canMessage cm)
        {
            messageState ms = (messageState)outgoingMessages[cm];
            if (ms == null) return false;
            return (Environment.TickCount > ms.nextTimeToSend);
        }

        public void flagAsSent(canMessage cm)
        {
            messageState ms = (messageState)outgoingMessages[cm];
            if (ms == null) return;

            ms.flagAsSent();
        }


        [Serializable]
        internal class messageState
        {
            public long period;
            public int count;
            public long nextTimeToSend = 0;
            public DateTime timestamp = new DateTime();

            public messageState(long period)
            {
                this.period = period;
                this.nextTimeToSend = Environment.TickCount + period;
            }

            public void flagAsSent()
            {
                this.count++;
                this.nextTimeToSend = Environment.TickCount + this.period;
                this.timestamp = DateTime.Now;
            }
        }
    }
}
