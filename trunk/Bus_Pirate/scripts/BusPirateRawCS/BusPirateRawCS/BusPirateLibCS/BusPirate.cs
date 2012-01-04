using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;
using System.IO;
using System.ComponentModel;
using System.Threading;
using BusPirateLibCS.Util;

namespace BusPirateLibCS
{
    public class BusPirate : BusPiratePipe, IDisposable
    {
        private const byte CMD_RESET = 0x00;

        private const byte CMD_SPI = 0x01;
        private const byte CMD_I2C = 0x02;
        private const byte CMD_UART = 0x03;
        private const byte CMD_1WIRE = 0x04;

        private const byte CMD_TERMINAL = 0x0F;

        private const byte CMD_TEST = 0x10;
        private const byte CMD_TEST_LONG = 0x11;

        private const byte CMD_PWM_START = 0x12;
        private const byte CMD_PWM_STOP = 0x13;

        private const byte CMD_ADC = 0x14;

        private const byte CMD_SET_IO_DIR = 0x40;
        private const byte CMD_SET_PINS = 0x80;

        [Flags]
        public enum Pins : byte
        {
            POWER = 0x40,
            PULLUP = 0x20,
            AUX = 0x10,
            CS = 0x01,
            MISO = 0x02,
            CLK = 0x04,
            MOSI = 0x08,
        }

        public const Pins PINS_IO = Pins.AUX | Pins.CS | Pins.MISO | Pins.CLK | Pins.MOSI;
        public const Pins PINS_OUTPUTS = Pins.POWER | Pins.PULLUP | Pins.AUX | Pins.CS | Pins.MISO | Pins.CLK | Pins.MOSI;
        public const Pins PINS_NONE = 0;

        private ExpectingSerialPort port;

        public BusPirate(SerialPort port)
        {

            port.BaudRate = 115200;
            port.Parity = Parity.None;
            port.DataBits = 8;
            port.StopBits = StopBits.One;
            port.Handshake = Handshake.None;
            port.ReadTimeout = 100;
            
            this.port = new ExpectingSerialPort(port);
        }

        public void Open()
        {
            port.Open();

            var zero = new byte[] { 0 };
            int attempts = 30;

            byte[] modeOnString = Encoding.ASCII.GetBytes("BBIO1");
            byte[] buffer = new byte[modeOnString.Length];
            int oldTimeout = port.ReadTimeout;
            port.ReadTimeout = 5;
			port.Write(zero, 0, zero.Length);
			Wait(500);
			port.ClearQueue();
            while (attempts > 0)
            {

                port.WriteByte(CMD_RESET);
                Wait(5);
                try
                {
                    port.Read(buffer, 0, buffer.Length);
                    for (int i = 0; i < buffer.Length; i++)
                    {
                        if (buffer[i] != modeOnString[i])
                            throw new IOException("Unexpected data when opening Bus Pirate");
                    }
                    break;
                }
                catch (TimeoutException)
                {
                    attempts--;
                }
                
            }

            if (attempts == 0)
                throw new IOException("Can not enter binary mode!");

            port.ReadTimeout = oldTimeout;
			port.ClearQueue();
        }

        public static void Wait(int time)
        {
            System.Threading.Thread.Sleep(time);
        }

        public void Close()
        {
            port.WriteByte(CMD_TERMINAL);
            port.Close();
        }

        public void reset() {
            port.WriteByte(CMD_RESET);
            port.ExpectReadText("BBIO1");
        }

        public void resetToTerminal()
        {
            port.WriteByte(CMD_TERMINAL);
        }

        public void shortTest()
        {
            port.WriteByte(CMD_TEST);
            throw new NotImplementedException();
        }

        public void longTest()
        {
            port.WriteByte(CMD_TEST_LONG);
            throw new NotImplementedException();
        }

        public void pwmSetup(double freq, double duty)
        {
            byte prescale = 0xff;
            ushort bperiod = 0xff;
            ushort bduty;
            int[] periods = new int[4];
            int[] divs = { 1, 8, 64, 256 };
            for (int i = 0; i < divs.Length; i++)
            {
                var n = 1 / (freq * (6.25e-8 * divs[i]));
                if (n < 0x10000)
                {
                    bperiod = (ushort)(n - 1);
                    prescale = (byte)i;
                    break;
                }
            }
            if (prescale == 0xff)
                throw new Exception("Not enough range");
            bduty = (ushort)(bperiod * duty);
            port.WriteByte(CMD_PWM_START);
            port.WriteByte(prescale);
            port.WriteByte((byte)(bduty >> 8));
            port.WriteByte((byte)(bduty & 0xff));
            port.WriteByte((byte)(bperiod >> 8));
            port.WriteByte((byte)(bperiod & 0xff));
            port.ExpectReadByte(0x01);
        }

        public void pwmStop()
        {
            port.WriteByte(CMD_PWM_STOP);
            throw new NotImplementedException();
        }

        public double ADCProbe
        {
            get
            {
                port.WriteByte(CMD_ADC);
                int value = port.ReadByte() << 8;
                value += port.ReadByte();
                return Math.Round(6.6 * value / 1024 , 2);
            }
        }

        private Pins pinsAsInputs = PINS_IO;

        public Pins PinsAsInput {
            get
            {
                return pinsAsInputs;
            }
            set
            {
                pinsAsInputs = value & PINS_IO;
                var x = (byte)(CMD_SET_IO_DIR | (byte)pinsAsInputs);
                port.WriteByte(x);
                pinValues = (Pins)port.ReadByte();
            }
        }

        private Pins pinValues = 0;

        public Pins PinValues
        {
            get
            {
                PinsAsInput = pinsAsInputs;
                return pinValues;
            }
            set
            {
                pinValues = value & PINS_OUTPUTS;
                var x = (byte)(CMD_SET_PINS | (byte)pinValues);
                port.WriteByte(x);
                pinValues = (Pins)port.ReadByte();
            }
        }


        #region BusPiratePipe Members

        public byte ReadByte()
        {
            return port.ReadByte();
        }

        public void ExpectReadByte(byte b)
        {
            port.ExpectReadByte(b);
        }

        public void ExpectReadText(string s)
        {
            port.ExpectReadText(s);
        }

        public void WriteByte(byte b)
        {
            port.WriteByte(b);
        }

        #endregion

        #region IDisposable Members

        void IDisposable.Dispose()
        {
            Close();
        }

        #endregion

        #region BusPiratePipe Members

        private bool exclusiveMode = false;
        
        void BusPiratePipe.EnterExclusiveMode()
        {
            if (exclusiveMode)
                throw new InvalidOperationException("Already in exclusive mode");
            exclusiveMode = true;
        }

        void BusPiratePipe.ExitExclusiveMode()
        {
            if (!exclusiveMode)
                throw new InvalidOperationException("Not in exclusive mode");
            exclusiveMode = false;
        }

        bool BusPiratePipe.IsInExclusiveMode()
        {
            return exclusiveMode;
        }

        #endregion
    }
}
