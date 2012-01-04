using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BusPiratePICProgrammer
{
	public partial class PIC16Programmer
	{
		private void IncAddress()
		{
			hw.WriteBits(0x06, 6);
		}
		
		private void skipAddrSpace(int address)
		{
			int skip = 0;
			while (skip < address)
			{
				IncAddress();
				skip++;
			}
		}
		
		bool ConfigAddressSpaceDifferent = true;
		
		private byte readDataByte()
		{
			hw.WriteBits(0x05, 6);
			hw.OutputPin = false;

			int word = 0;
			int b1 = hw.ReadByte();
			int b2 = hw.ReadByte();
			word = (b1 >> 1);
			word |= (b2 << 7);
			return (byte)(word & 0x00ff);
		}


		private int CommandBeginDataProg = 0x08;
		private bool EndDataProgCommand = false;
		private int CommandEndDataProg = 0x17;

		private void writeDataWord(int word)
		{
			hw.WriteBits(0x03, 6);
			hw.WriteBits((word & 0xff) << 1, 16);
			hw.WriteBits(CommandBeginDataProg, 6);
			hw.OutputPin = false;
			DelayMs(Tdprog);
			if (EndDataProgCommand)
				hw.WriteBits(CommandEndDataProg, 6);

		}

		private void loadCodeWord(int word)
		{
			hw.WriteBits(0x02, 6);
			hw.WriteBits((word & 0x3fff) << 1, 16);

			hw.OutputPin = false;
		}

		private void writeLoadedCode()
		{
			hw.WriteBits(CommandBeginCodeProg, 6);
			hw.OutputPin = false;
			
			DelayMs(Tdprog);

			if (EndCodeProgCommand)
				hw.WriteBits(CommandEndCodeProg, 6);

			hw.OutputPin = false;
		}

		private int loadsPerWrite = 1;

		private int CommandBeginCodeProg = 0x08;

		private bool EndCodeProgCommand = false;
		private int CommandEndCodeProg = 0x0A;

		private int eepromOffset = 0x4000;
		private int Tdprog = 50;

		private int configAddress = 0x2000;
		private int configLength = 8;

		private bool saveRegisters = false;
		
		private bool funkyCodeProtectEraseP16F87X = false;

		private int CommandErase = 0x09;

		private bool eraseProgramSeparateFromConfP16F84A = false;

		private int Tera = 50;

		private bool eeprom = true;

		private int CommandEepromErase = 0x0B;

		Dictionary<int, int> savedRegisters = new Dictionary<int, int>();

		private int readProgramWord()
		{
			hw.WriteBits(0x04, 6);
			int word = 0;
			int b1 = hw.ReadByte();
			int b2 = hw.ReadByte();
			word |= b1 >> 1;
			word |= b2 << 7;
			word = word & 0x3fff;
			return word;
		}

	}
}
