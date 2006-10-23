using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace canWatcher
{
    public interface KeeperTracker
    {
        Hashtable getMessages();
        long getPeriod(canMessage cm);
        int getCount(canMessage cm);
        DateTime getTime(canMessage cm);
    }
}
