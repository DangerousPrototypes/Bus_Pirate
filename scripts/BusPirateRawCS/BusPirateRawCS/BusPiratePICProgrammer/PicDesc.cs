using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace BusPiratePICProgrammer
{
    class PicDesc
    {
        
    }

    class Memory
    {
        public String Type { get; set; }
        public Range<long> Range { get; set; }
        
    }


    class Range<T>
    {
        public T Start { get; set; }
        public T End { get; set; }

        public Range() {
        }
        public Range(T start, T end) {
            this.Start = start;
            this.End = end;
        }
    }
}
