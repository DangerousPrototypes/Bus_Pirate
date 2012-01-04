using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BusPirateLibCS
{
    public interface BusPiratePipe
    {
        byte ReadByte();
        void ExpectReadByte(byte b);
        void ExpectReadText(string s);
        void WriteByte(byte b);

        void EnterExclusiveMode();
        void ExitExclusiveMode();
        bool IsInExclusiveMode();
    }
}
