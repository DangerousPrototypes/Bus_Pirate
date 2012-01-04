using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.ComponentModel;
using System.Threading;
using BusPirateLibCS;
using BusPirateLibCS.Modes;

namespace BusPiratePICProgrammer
{
	public abstract class PicProgrammer
	{
		protected BusPirate bp;
		
		protected RawWire hw;
		
		bool lvp = false;
		
		public PicProgrammer(SerialPort sp, bool LVP)
		{
			bp = new BusPirate(sp);
			bp.Open();
			hw = new RawWire(bp);
			hw.EnterMode();

			hw.ConfigProtocol(false, false, true);
			hw.ConfigPins(true, true, false, false);
			hw.HighSpeed = true;
			this.lvp = LVP;
		}

		bool program = false;
		public bool Program
		{
			set
			{
				if (lvp) hw.CS = value;
				hw.AUX = value;
				program = value;
			}
			get
			{
				return program;
			}

		}

		public void DelayMs(int time)
		{
			Thread.Sleep(time);
		}

		public abstract void bulkErase();
		
		public abstract void writeCode(int address, byte[] data, int offset, int length);

		public abstract void writeData(int address, byte[] data, int offset, int length);

		public abstract void writeConfig(int address, byte[] data, int offset, int length);

		public abstract void readData(int address, byte[] data, int offset, int length);

		public abstract void readCode(int address, byte[] data, int offset, int length);



		public void close()
		{
			hw.ExitMode();
			bp.Close();
		}


	}
}
