using System;
using System.Collections.Generic;
using System.Text;
using System.IO.Ports;
using canConfigurator;

namespace canBootloader
{
    public class SerialConnection
    {
        public const byte UART_START_BYTE = 253;
        public const byte UART_END_BYTE = 250;
        public const int PACKET_LENGTH = 17;
        private const int readBufferSize = 8192;

        private Queue<CanPacket> cpq = new Queue<CanPacket>();
        private System.Threading.Mutex queueMutex = new System.Threading.Mutex();
        private SerialPort serial_conn = null;
        private byte[] iBuff = new byte[8192];
        private int iBuffPointer = 0;

        public SerialConnection() {  }
        ~SerialConnection() { serial_conn.Close();  }
        public bool open() 
        { 
            if (serial_conn==null) return false;
            try
            {
                if (!serial_conn.IsOpen) serial_conn.Open();
            }
            catch (UnauthorizedAccessException)
            {
                return false;
            }
            return true; 
        }
        public bool close() 
        {
            if (serial_conn == null) return false;
            if (serial_conn.IsOpen) serial_conn.Close();
            return true; 
        }
        public bool isOpen() { return serial_conn.IsOpen; }
        public void setConfiguration(int baudRate, Parity parity, string portName, StopBits stopbits, int databits, bool rtsEnable) 
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
       
        public bool writePacket(CanPacket cp) 
        {
            if (serial_conn.IsOpen)
            {
                byte[] bytes = new byte[PACKET_LENGTH];
                Array.Copy(cp.getBytes(), 0, bytes, 1, PACKET_LENGTH - 2);
                bytes[0] = UART_START_BYTE;
                bytes[PACKET_LENGTH - 1] = UART_END_BYTE;
                serial_conn.Write(bytes, 0, PACKET_LENGTH);
                return true;
            }
            return false;
        }

        public bool getPacket(out CanPacket cp) 
        {
            queueMutex.WaitOne();
            cp = null;
            bool had = false;
            if (cpq.Count > 0)
            {
                cp = cpq.Dequeue();
                had = true;
            }
            queueMutex.ReleaseMutex();
            return had;
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
                            if (iBuff[startIndex + PACKET_LENGTH - 1] == UART_END_BYTE)
                            {
                                // Om byte PACKET_LENGTH-1 är slutbyte så extraktas startIndex till slutbyteindex.
                                CanPacket cm = new CanPacket(iBuff, (uint)startIndex + 1);
                                queueMutex.WaitOne();
                                cpq.Enqueue(cm);
                                queueMutex.ReleaseMutex();
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

    }
}
