using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Linq;
using System.Text;

namespace BusPiratePICProgrammer
{
	public partial class PIC16Programmer : PicProgrammer
	{

		public PIC16Programmer(SerialPort sp, bool LVP)
			: base(sp, LVP)
		{

		}

		public override void bulkErase()
		{
			Program = true;

			if (saveRegisters)
			{
				foreach (var address in savedRegisters.Keys)
				{
					byte[] temp = new byte[2];
					readCode(address, temp, 0, temp.Length);
					savedRegisters[address] = (temp[0] & 0xff) | ((temp[1] << 8) & 0xff);
				}
			}

			if (eraseProgramSeparateFromConfP16F84A)
			{
				loadCodeWord(0x3fff);
				hw.WriteBits(CommandErase, 6);
				hw.WriteBits(0x08, 6);
				DelayMs(Tera);
			}

			if (ConfigAddressSpaceDifferent)
			{
				// Switch to config space
				hw.WriteBits(0, 6);
				hw.WriteBits(0, 16);
				// Some pics need load before erase
				loadCodeWord(0x3fff);
			}

			if (funkyCodeProtectEraseP16F87X)
			{
				throw new NotImplementedException();
			}


			hw.WriteBits(CommandErase, 6);
			DelayMs(Tera);

			if (eeprom)
			{
				hw.WriteBits(CommandEepromErase, 6);
				DelayMs(Tera);
			}



			if (saveRegisters)
			{
				foreach (var address in savedRegisters.Keys)
				{
					byte[] temp = new byte[2];
					temp[0] = (byte)savedRegisters[address];
					temp[1] = (byte)(savedRegisters[address] >> 8);
					writeCode(address, temp, 0, temp.Length);
				}
			}

			Program = false;
		}

		public override void writeCode(int address, byte[] data, int offset, int length)
		{
			Program = true;

			skipAddrSpace(address);
			
			for (int i = 0; i < length; i+=2 * loadsPerWrite)
			{
				
				for (int j = 0; j < loadsPerWrite; j++) {
					int word = data[offset + i + j] 
					        | (data[offset + i + j + 1] << 8);
				
					loadCodeWord(word);
				}

				writeLoadedCode();

				//int read = readCodeWord();
				//if (word != read)
				//{
				//    // retry? fail?
				//}
				IncAddress();
			}

			Program = false;
		}

		public override void writeData(int address, byte[] data, int offset, int length)
		{
			Program = true;

			skipAddrSpace(address);

			for (int i = 0; i < length; i++)
			{
				writeDataWord(data[i]);
				IncAddress();
			}

			Program = false;
		}

		public override void writeConfig(int address, byte[] data, int offset, int length)
		{
			Program = true;

			if (ConfigAddressSpaceDifferent
				&& address >= configAddress && address <= configAddress + configLength)
			{
				hw.WriteBits(0x00, 6);
				hw.WriteBits(0x0000, 16);
				//loadCodeWord(0x3fff);
				//writeLoadedCode();
				address -= configAddress;
			}

			skipAddrSpace(address);

			for (int i = 0; i < length; i += 2)
			{
				int word = data[offset + i] | (data[offset + i + 1] << 8);
				loadCodeWord(word);
				writeLoadedCode();
				IncAddress();
			}

			Program = false;
		}

		public override void readData(int address, byte[] data, int offset, int length)
		{
			Program = true;

			skipAddrSpace(address);

			for (int i = 0; i < length; i++)
			{
				data[offset + i] = readDataByte();
				IncAddress();
			}

			Program = false;
		}

		public override void readCode(int address, byte[] data, int offset, int length)
		{
			Program = true;

			if (ConfigAddressSpaceDifferent
				&& address >= configAddress && address <= configAddress + configLength)
			{
				hw.WriteBits(0x00, 6);
				hw.WriteBits(0x0000, 16);
				address -= configAddress;
			}

			skipAddrSpace(address);

			for (int i = 0; i < length; i += 2)
			{
				int word = readProgramWord();
				data[offset + i] = (byte) (word & 0xff);
				data[offset + i + 1] = (byte) ((word >> 8) & 0xff);
				
				IncAddress();
			}

			Program = false;
		}


	}
}
