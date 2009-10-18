using System;
using System.Collections.Generic;
using System.Text;
using System.Collections;

namespace canWatcher
{
    class logList : IList<identHistory.dataStamp>
    {
        ArrayList al = new ArrayList();


        #region IList<dataStamp> Members

        public int IndexOf(identHistory.dataStamp item)
        {
            return al.IndexOf(item);
        }

        public void Insert(int index, identHistory.dataStamp item)
        {
            al.Insert(index, item);
        }

        public void RemoveAt(int index)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public identHistory.dataStamp this[int index]
        {
            get
            {
                throw new Exception("The method or operation is not implemented.");
            }
            set
            {
                throw new Exception("The method or operation is not implemented.");
            }
        }

        #endregion

        #region ICollection<dataStamp> Members

        public void Add(identHistory.dataStamp item)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void Clear()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public bool Contains(identHistory.dataStamp item)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public void CopyTo(identHistory.dataStamp[] array, int arrayIndex)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        public int Count
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public bool IsReadOnly
        {
            get { throw new Exception("The method or operation is not implemented."); }
        }

        public bool Remove(identHistory.dataStamp item)
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion

        #region IEnumerable<dataStamp> Members

        public IEnumerator<identHistory.dataStamp> GetEnumerator()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion

        #region IEnumerable Members

        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            throw new Exception("The method or operation is not implemented.");
        }

        #endregion
    }
}
