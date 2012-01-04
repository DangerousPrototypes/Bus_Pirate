using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Text.RegularExpressions;
using System.Globalization;

namespace BusPiratePICProgrammer
{
    public class HexParser
    {
        private long address = 0;
        StreamReader file;


        public HexParser(Stream file)
        {
            file.Seek(0, SeekOrigin.Begin);
            this.file = new StreamReader(file, Encoding.ASCII);
            nextAddress();
        }

        byte[] buffer = new byte[0];
        int bufferIndex = 0;

        public long Address
        {
            get
            {
                return address;
            }
        }

        public long nextAddress()
        {
            if (bufferIndex + 1 < buffer.Length)
            {
                bufferIndex++;
                address++;
                return address;
            }
            buffer = null;
            var r = new Regex(@":(?<len>[\dA-Z]{2})(?<addr>[\dA-Z]{4})(?<type>[\dA-Z]{2})(?<data>[\dA-Z]+)(?<sum>[\dA-Z]{2})");
            while (buffer == null) {
                string text = file.ReadLine();
				if (text == null)
				{
					address = long.MaxValue;
					return address;
				}
                var m = r.Match(text);
                if (!m.Success)
                    continue;

                int len = parseRe(m, "len");
                int type = parseRe(m, "type");
                int addr = parseRe(m, "addr");
                if (type == 4)
                {
                    int data = parseRe(m, "data");
                    address = data << 16;
                    continue;
                }
                if (type == 1)
                {
                    address = long.MaxValue;
                    break;
                }
                if (type == 0)
                {
                    address = (address & 0xffff0000) + addr;
                    var datas = m.Groups["data"].Value;
                    buffer = new byte[len];
                    for (int i = 0; i < len; i++ )
                    {
                        buffer[i] = byte.Parse(datas.Substring(i * 2, 2), NumberStyles.HexNumber);
                    }
                    bufferIndex = 0;
                }
            }
            return address;
        }

        int parseRe(Match m, string group)
        {
            return int.Parse(m.Groups[group].Value, NumberStyles.HexNumber);
        }

        public byte Value
        {
            get
            {
                return buffer[bufferIndex];
            }
        }

    }
}
