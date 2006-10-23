using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.IO;

namespace canWatcher
{
    public class serialCanConnection
    {
        const int readBufferSize = 256;
        SerialPort serial_conn;

        public const byte UART_START_BYTE = 253;
        public const byte UART_END_BYTE = 250;

        byte[] iBuff = new byte[256];
        int iBuffPointer = 0;

        Queue<canMessage> cmQueue = new Queue<canMessage>();

        public serialCanConnection(int baudRate,Parity parity,string portName,StopBits stopbits,int databits,bool rtsEnable)
        {
            try
            {
                serial_conn = new SerialPort();
                serial_conn.ReadBufferSize = readBufferSize;
                serial_conn.BaudRate = baudRate;
                serial_conn.Parity = parity;
                serial_conn.PortName = portName;
                serial_conn.StopBits = stopbits;
                serial_conn.DataBits = databits;
                serial_conn.RtsEnable = rtsEnable;
                this.serial_conn.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serial_conn_DataReceived);
            }
            catch (Exception e)
            {
                
                throw e;
            }
        }

        ~serialCanConnection()
        {
            if (serial_conn != null)
            {
                serial_conn.Close();
                serial_conn = null;
            }
        }

        public bool isConnected()
        {
            return (serial_conn.IsOpen);
        }

        public bool connect(out string err)
        {
            err = "";
            if (serial_conn.IsOpen) { err = "Port alredy open."; return false; }
            try
            {
                serial_conn.Open();
            }
            catch (Exception e)
            {

                err = e.Message;
                return false;
            }

            return true;
        }

        public bool disconnect(out string err)
        {
            err = "";
            try
            {
                serial_conn.Close();
            }
            catch (Exception e)
            {

                err = e.Message;
                return false;
            }

            return true;
        }

        public canMessage getMessage()
        {
            if (cmQueue.Count > 0)
            {
                return cmQueue.Dequeue();
            }
            return null;
        }

        public bool sendMessage(canMessage cm)
        {
            if (serial_conn.IsOpen)
            {
                byte[] b = new byte[16];
                b[0] = UART_START_BYTE;
                Array.Copy(cm.getRaw(), 0, b, 1, 14);
                b[15] = UART_END_BYTE;
                serial_conn.Write(b, 0, 16);
                return true;
            }
            return false;
        }

        public bool writeRaw(byte[] b, int length)
        {
            if (serial_conn.IsOpen)
            {
                serial_conn.Write(b, 0, length);
                return true;
            }
            return false;
        }


        private void serial_conn_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {

            System.IO.Ports.SerialPort port = (System.IO.Ports.SerialPort)sender;
            byte[] data = new byte[port.BytesToRead];

            int bytesToRead = port.BytesToRead;

            // Fyll på array hela tiden i slutet.
            port.Read(data, 0, data.Length);
            Array.Copy(data, 0, iBuff, iBuffPointer, data.Length);
            iBuffPointer += bytesToRead;


            // När array större eller lika med 16...
            while (iBuffPointer >= 16)
            {
                int startIndex = 0;

                // Sök igenom efter start byte från början.
                for (int i = 0; i < iBuffPointer; i++)
                {
                    // Poppa alla bytes som inte är startbyte.
                    if (iBuff[i] != UART_START_BYTE) startIndex++;
                    else
                    {
                        // När startbyte hittas, kolla om återstående längd är större eller lika med 16 (inkl startbyte)
                        if ((iBuffPointer - startIndex) >= 16)
                        {
                            //om så, kolla 15 byte fram.
                            if (iBuff[startIndex + 15] == UART_END_BYTE)
                            {
                                // Om byte 15 är slutbyte så extraktas startIndex till slutbyteindex.
                                canMessage cm = new canMessage(iBuff, startIndex + 1);
                                cmQueue.Enqueue(cm);

                                newIncommingCanMessage.Invoke(this, EventArgs.Empty);

                                // Sätt ny startindex och avsluta loop.
                                startIndex += 16;
                                break;
                            }
                        }
                    }
                }
                // och i slutet göra en array copy.
                // Flytta ner allt efter slutbyte till index 0 i array.
                Array.Copy(iBuff, startIndex, iBuff, 0, iBuffPointer - 16);
                iBuffPointer -= startIndex;
            }

        }

        public event EventHandler newIncommingCanMessage;




    }
}
