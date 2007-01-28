using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Globalization;

namespace canBootloader
{
	public class HexFile
	{
		private const ulong USER_RESET_ADDR = 0; //0x1000;	//0x1000 för PIC??
		private const ulong BUFFER_SIZE = 5200000;
		private const ulong LINE_MAX_SIZE = 1024;
		private const ulong ERROR_BUFFER_SIZE = 256;
		private const ulong FILE_PATH_SIZE = 256;

		private ulong length = 0;
		private ulong addrLower = 0;
		private ulong addrUpper = 0;
		private string filepath = "";
		private bool valid = false;
		private byte[] buff = new byte[BUFFER_SIZE];

		public HexFile() { }

		public bool isValid() { return valid; }

		public bool loadHex(string filepath)
		{
			// Populate buffer

			byte lineDataCount = 0;
			ulong lineAddrHigh = 0;
			ulong lineAddr = 0;
			byte lineCode = 0;
			ulong fullAddr = 0;

			this.filepath = filepath;
			this.length = 0;
			this.addrLower = 0x7FFFFFFF;
			this.addrUpper = 0;
			this.valid = false;

			// Prime the buffer
			for (ulong i = 0; i < BUFFER_SIZE; i++) buff[i] = 0xFF;


			// Try opening file
			if (!File.Exists(this.filepath)) 
			{
				// The file could not be opened
				Console.WriteLine("Error in HexFile: Unable to load file.");
				return false;
			}
			else 
			{
				FileStream file = new FileStream(this.filepath, FileMode.Open, FileAccess.Read);
				StreamReader sr = new StreamReader(file);
				
				// Safely use the file stream

				// For each line, read until eof.
				while (!sr.EndOfStream)
				{
					// read one line
					string oneLine = sr.ReadLine();
					
					// If valid line
					if (!oneLine.Equals("") && oneLine[0]==':')
					{
						lineDataCount = byte.Parse(oneLine.Substring(1,2), NumberStyles.HexNumber);
						lineAddr =  ulong.Parse(oneLine.Substring(3,4),NumberStyles.HexNumber) & 65535;	 
						lineCode = byte.Parse(oneLine.Substring(7,2), NumberStyles.HexNumber);   
				
						fullAddr = (lineAddrHigh * 65536) + lineAddr;
					
						switch(lineCode)
						{
								case 0:
									this.length +=lineDataCount;
									if ((fullAddr + lineDataCount - 1) > this.addrUpper) this.addrUpper = fullAddr + lineDataCount - 1;
									if (lineCode == 0 && fullAddr < this.addrLower) this.addrLower = fullAddr;
									for (int j = 0; j < lineDataCount; j++)
									{
										buff[fullAddr] = byte.Parse(oneLine.Substring(j * 2 + 9, 2), System.Globalization.NumberStyles.HexNumber);
										buff[fullAddr] &= (byte)255;
										fullAddr++;
									}
									break;
								case 4:
									lineAddrHigh = ulong.Parse(oneLine.Substring(9, 4), System.Globalization.NumberStyles.HexNumber) & 65535;
									break;
								case 1: break; 
							
						}

					}
				
				}

				this.valid = true;
				sr.Close();
			}

			// Force lower addr to start
			this.addrLower = USER_RESET_ADDR;

			return true;
		}


		public ulong getAddrUpper() { return addrUpper; }

		public ulong getAddrLower() { return addrLower; }

		public ulong getLength() { return length; }

		public byte getByte(ulong addr)
		{
			if (addr>=BUFFER_SIZE) return 0xFF;
			return buff[addr];
		}

	}
}
