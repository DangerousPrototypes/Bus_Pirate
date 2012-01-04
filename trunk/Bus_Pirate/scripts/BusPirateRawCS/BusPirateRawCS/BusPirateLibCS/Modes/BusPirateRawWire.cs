using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

namespace BusPirateLibCS.Modes
{
    public class RawWire : Mode
    {

        BusPiratePipe root;

        public RawWire(BusPiratePipe root)
        {
            this.root = root;
        }
        
        public void EnterMode()
        {
            if (root.IsInExclusiveMode())
                throw new InvalidOperationException("Already in another mode");
            root.EnterExclusiveMode();
            root.WriteByte(0x05);
            root.ExpectReadText("RAW1");
        }

        public void ExitMode()
        {
            root.WriteByte(0x00);
            root.ExitExclusiveMode();
        }

        public void I2Cstart()
        {
            root.WriteByte(0x02 | 0);
            root.ExpectReadByte(0x01);
        }

        public void I2Cstop()
        {
            root.WriteByte(0x02 | 1);
            root.ExpectReadByte(0x01);
        }

        private bool cs = false;

        public bool CS
        {
            get
            {
                return cs;
            }
            set
            {
                root.WriteByte((byte)(0x04 | (value ? 1 : 0)));
                root.ExpectReadByte(0x01);
                cs = value;
            }
        }

        public byte ReadByte()
        {
            root.WriteByte(0x06);
            return root.ReadByte();
        }

        public bool ReadBit()
        {
            root.WriteByte(0x07);
            return root.ReadByte() > 0;
        }

        public bool InputPin
        {
            get
            {
                root.WriteByte(0x08);
                return root.ReadByte() > 0;
            }
        }

        public void ClockTick()
        {
            root.WriteByte(0x9);
            root.ExpectReadByte(0x01);
        }

        public bool ClockPin
        {
            set
            {
                root.WriteByte((byte)(0x0A | (value ? 1 : 0)));
                root.ExpectReadByte(0x01);
            }
        }

        public bool OutputPin
        {
            set
            {
                root.WriteByte((byte)(0x0C | (value ? 1 : 0)));
                root.ExpectReadByte(0x01);
            }
        }

        public void WriteBulk(byte[] data) {
            if (data.Length > 16 || data.Length < 1)
                throw new ArgumentOutOfRangeException("data", "Number of bytes must be between 1 and 16");

            root.WriteByte((byte)(0x10 | (data.Length - 1)));
            root.ExpectReadByte(0x01);
            foreach (var b in data)
            {
                root.WriteByte(b);
                root.ExpectReadByte(0x01);
            }
        }

        public bool Power
        {
            get
            {
                return power;
            }
            set
            {
                power = value;
                configPins();
            }
        }

        public bool Pullups
        {
            get
            {
                return pullups;
            }
            set
            {
                pullups = value;
                configPins();
            }
        }

        public bool AUX
        {
            get {
                return aux;
            }
            set {
                aux = value;
                configPins();
            }
        }


        private bool highSpeed = true;
        public bool HighSpeed
        {
            get { return highSpeed; }
            set {
                root.WriteByte((byte)(0x60 | (value ? 1 : 0)));
                root.ExpectReadByte(0x01);
                highSpeed = value;
            }
        }

        public void ConfigPins(bool power, bool pullups, bool aux, bool cs)
        {
            this.power = power;
            this.pullups = pullups;
            this.aux = aux;
            this.cs = cs;
            configPins();
        }

        bool power = false;
        bool pullups = false;
        bool aux = false;

        void configPins()
        {
            byte v = 0x40;
            if (power) v |= 0x08;
            if (pullups) v |= 0x04;
            if (aux) v |= 0x02;
            if (cs) v |= 0x01;
            root.WriteByte(v);
            root.ExpectReadByte(0x01);
        }

        bool lsbFirst = false;

        public void ConfigProtocol(bool activeOutput, bool threeWire, bool LSBfirst)
        {
            this.lsbFirst = LSBfirst;
            byte v = 0x80;
            if (activeOutput) v |= 0x08;
            if (threeWire) v |= 0x04;
            if (LSBfirst) v |= 0x02;
            root.WriteByte(v);
            root.ExpectReadByte(0x01);
        }




        #region IDisposable Members

        public void Dispose()
        {
            ExitMode();
        }

        #endregion

        public void WriteBit(byte b)
        {
            OutputPin = b > 0;
            ClockTick();
        }

        public void WriteBits(int bits, int number)
        {
            for (int i = 0; i < number; i++)
            {
                int j = lsbFirst ? i : number - i - 1;
                WriteBit((byte)((bits >> j) & 0x01));
            }
        }
    }
}
