using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO.Ports;
using System.Threading;
using System.IO;
using BusPirateLibCS;
using BusPiratePICProgrammer;

namespace buspirateraw
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }


		
        private void Form1_Load(object sender, EventArgs e)
        {
			pp = new PIC16Programmer(serialPort1, false);
			//pp = new DsPICProgrammer(serialPort1);


            
        }
        PicProgrammer pp;


        private void button1_Click(object sender, EventArgs e)
        {
            pp.Program = !pp.Program;
        }



        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            timer1.Stop();
            if (pp != null)
                pp.close();
            //w.exitMode();
            //b.close();
            
        }


       
        private void btnProgTest_Click(object sender, EventArgs e)
        {
			pp.bulkErase();

			var uids = new byte[] {
				0x01, 0x02,
				0x03, 0x04,
				0x05, 0x06,
				0x07, 0x08
			};
			pp.writeConfig(0x2000, uids, 0, uids.Length);

			var conf = new byte[] {
				0x10, 0x3f
			};
			pp.writeConfig(0x2007, conf, 0, conf.Length);

			var code = new byte[96];

			code[0] = (byte)DateTime.Now.Hour;
			code[2] = (byte)DateTime.Now.Minute;
			code[4] = (byte)DateTime.Now.Second;

			for (int i = 0; i < ('z' - 'a') * 2; i += 2)
			{
				code[6 + i] = (byte)(i + 'a');
			}

			pp.writeCode(0x0, code, 0, code.Length);
			
			var data = new byte[16];

			data[0] = (byte)DateTime.Now.Hour;
			data[1] = (byte)DateTime.Now.Minute;
			data[3] = (byte)DateTime.Now.Second;
			
			for (int i = 0; i < 9; i++) {
				data[i + 4] = (byte)(i * i);
			}

			pp.writeData(0x0, data, 0, data.Length);
			
			var confRead = new byte[16];

			
			pp.readCode(0x2000, confRead, 0, confRead.Length);

			for (int i = 0; i < confRead.Length; i += 2) {
				txtOut.AppendText(String.Format("{0} {1}\n"
					, (i/2 + 0x2000).ToString("X4")
					, (confRead[i] | (confRead[i + 1] << 8)).ToString("X4")));
			}

		
        }



		private void timer1_Tick(object sender, EventArgs e)
        {
            //txtOut.AppendText(b.PinValues.ToString() + "\r\n");
            //txtOut.AppendText(b.ADCProbe + "v"+ "\r\n");


        }

        private void trackBar1_Scroll(object sender, EventArgs e)
        {
            var tb = sender as TrackBar;
            //b.pwmSetup((1 << tb.Value), 0.2);
        }

        private void btnReadHex_Click(object sender, EventArgs e)
        {
			
			var f = File.Open(
			    @"C:\microchip\testStepper\output\testStepper.hex",
			    FileMode.Open, FileAccess.Read);
			var h = new HexParser(f);

			byte[] code = new byte[0x1000];
			byte[] uid = new byte[8];
			byte[] data = new byte[0x80];
			byte[] conf = new byte[2];

			long addr = -1;
			while (h.Address <= code.Length * 2)
			{
				addr = h.Address;
				code[addr] = h.Value;
				h.nextAddress();
			}
			Array.Resize<byte>(ref code, (int)addr + 1);

			addr = -1;
			while (h.Address <= 0x2004 * 2)
			{
				addr = h.Address - 0x2000 * 2;
				uid[addr] =  h.Value;
				h.nextAddress();
			}
			Array.Resize<byte>(ref uid, (int)addr + 1);

			addr = -1;
			while (h.Address <= (0x2007 * 2) + 1 )
			{
				addr = h.Address - 0x2007 * 2;
				conf[addr] = h.Value;
				h.nextAddress();
			}
			Array.Resize<byte>(ref conf, (int)addr + 1);

			addr = -1;
			while (h.Address < (0x2100 + 0x80) * 2)
			{
				addr = h.Address - 0x2100 * 2;
				data[addr] = h.Value;
				h.nextAddress();
			}
			Array.Resize<byte>(ref data, (int)addr + 1);

			pp.bulkErase();
			pp.writeConfig(0x2000, uid, 0, uid.Length);
			if (conf.Length < 2)
			{
				conf = new byte[] { 0x10, 0x3f };
			}
			pp.writeConfig(0x2007, conf, 0, conf.Length);
			pp.writeCode(0x0000, code, 0, code.Length);
			pp.writeData(0x0000, data, 0, data.Length);

			var confRead = new byte[16];
			pp.readCode(0x2000, confRead, 0, confRead.Length);

			txtOut.AppendText("\r\n");
			for (int i = 0; i < confRead.Length; i += 2)
			{
				txtOut.AppendText(String.Format("{0} {1}\r\n"
					, (i / 2 + 0x2000).ToString("X4")
					, (confRead[i] | (confRead[i + 1] << 8)).ToString("X4")));
			}

			f.Close();
            
        }


		private void btnReadProgram_Click(object sender, EventArgs e)
		{
			byte[] data = new byte[320];

			pp.readCode(0x0, data, 0, data.Length);

			for (int i = 0; i < data.Length; i += 2)
			{
				if (i % 16 == 0)
				{
					txtOut.AppendText("\r\n");
				}
				txtOut.AppendText((data[i] | data[i + 1] << 8).ToString("X4") + " ");
			}
		}

		private void btnReadData_Click(object sender, EventArgs e)
		{
			byte[] data = new byte[64];

			pp.readData(0x0, data, 0, data.Length);

			for (int i = 0; i < data.Length; i += 1)
			{
				if (i % 16 == 0)
				{
					txtOut.AppendText("\r\n");
				}
				txtOut.AppendText((data[i]).ToString("X2") + " ");
			}
		}




    }
}
