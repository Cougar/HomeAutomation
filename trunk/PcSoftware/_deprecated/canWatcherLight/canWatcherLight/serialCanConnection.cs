using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using System.IO;

namespace canWatcherLight
{
    public class serialCanConnection
    {
        const int readBufferSize = 1024;
        SerialPort serial_conn;

        public const byte UART_START_BYTE = 253;
        public const byte UART_END_BYTE = 250;

        public const int PACKET_LENGTH = 17;

        byte[] iBuff = new byte[8192];
        int iBuffPointer = 0;

        Queue<CanPacket> cmQueue = new Queue<CanPacket>();

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

        public bool getMessage(out CanPacket cm)
        {
            cm = null;
            if (cmQueue.Count > 0)
            {
                cm = cmQueue.Dequeue();
                return true;
            }
            return false;
        }

        public bool sendMessage(CanPacket cm)
        {
            if (serial_conn.IsOpen)
            {
                byte[] b = new byte[PACKET_LENGTH];
                b[0] = UART_START_BYTE;
                Array.Copy(cm.getBytes(), 0, b, 1, PACKET_LENGTH-2);
                b[PACKET_LENGTH-1] = UART_END_BYTE;
                serial_conn.Write(b, 0, PACKET_LENGTH);
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


            // När array större eller lika med PACKET_LENGTH...
            while (iBuffPointer >= PACKET_LENGTH)
            {
                int startIndex = 0;

                // Sök igenom efter start byte från början.
                for (int i = 0; i < iBuffPointer; i++)
                {
                    // Poppa alla bytes som inte är startbyte.
                    if (iBuff[i] != UART_START_BYTE) startIndex++;
                    else
                    {
                        // När startbyte hittas, kolla om återstående längd är större eller lika med PACKET_LENGTH (inkl startbyte)
                        if ((iBuffPointer - startIndex) >= PACKET_LENGTH)
                        {
                            //om så, kolla PACKET_LENGTH-1 byte fram.
                            if (iBuff[startIndex + PACKET_LENGTH-1] == UART_END_BYTE)
                            {
                                // Om byte PACKET_LENGTH-1 är slutbyte så extraktas startIndex till slutbyteindex.
                                CanPacket cm = new CanPacket(iBuff, (uint)startIndex + 1);
                                cmQueue.Enqueue(cm);

                                newIncommingCanMessage.Invoke(this, EventArgs.Empty);

                                // Sätt ny startindex och avsluta loop.
                                startIndex += PACKET_LENGTH;
                                break;
                            }
                        }
                    }
                }
                // och i slutet göra en array copy.
                // Flytta ner allt efter slutbyte till index 0 i array.
                Array.Copy(iBuff, startIndex, iBuff, 0, iBuffPointer - PACKET_LENGTH);
                iBuffPointer -= startIndex;
            }

        }

        public event EventHandler newIncommingCanMessage;




    }
}
