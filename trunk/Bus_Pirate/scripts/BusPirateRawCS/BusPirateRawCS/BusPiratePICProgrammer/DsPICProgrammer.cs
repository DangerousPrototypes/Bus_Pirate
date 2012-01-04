using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO.Ports;

namespace BusPiratePICProgrammer
{
	public class DsPICProgrammer : PicProgrammer
	{
		
		public DsPICProgrammer(SerialPort sp) : base(sp, true)
		{

		}
		
		private void dspic_send_24_bits(int p)
		{
			hw.WriteBits(p << 4, 24 + 4);
		}
		
		private int dspic_read_16_bits()
		{
			int result = 0;
			hw.WriteBits(1, 4);
			hw.WriteBits(0, 8);

			result = hw.ReadByte() << 8;
			result |= hw.ReadByte();
			return result;
		}

		public override void bulkErase()
		{
			
			//bulk erase program memory
			//step 1
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x000000);	//NOP
			//step 2
			dspic_send_24_bits(0x24008A);	//MOV #0x4008, W10
			dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
			//step 3
			dspic_send_24_bits(0x200F80);	//MOV #0xF8, W0
			dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
			dspic_send_24_bits(0x200067);	//MOV #0x6, W7
			//step 4
			dspic_send_24_bits(0xEB0300);	//CLR W6
			dspic_send_24_bits(0x000000);	//NOP

			for (int i = 0; i < 2; i++)
			{
				//step 5
				dspic_send_24_bits(0xBB1B86);	//TBLWTL W6, [W7++]
				//step 6
				dspic_send_24_bits(0x200558);	//MOV #0x55, W8
				dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
				dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
				dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
				//step 7
				dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				DelayMs(201);			//Externally time 200ms
				dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
			}//step 8: repeat step 5-7
			//step 9
			dspic_send_24_bits(0x2407FA);	//MOV #0x407F, W10
			dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
			//step 10
			dspic_send_24_bits(0x200558);	//MOV #0x55, W8
			dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
			dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
			dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
			//step 11
			dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			DelayMs(3);			//Externally time 2 msec
			dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP


		}

		
		public override void writeCode(int address, byte[] data, int offset, int length)
		{
			int blocksize = length;
			//if((address%96)==0)
			//{
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 1: Exit the Reset vector.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
				//Step 2: Set the NVMCON to program 32 instruction words.
				dspic_send_24_bits(0x24001A);	//MOV #0x4001, W10
				dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
			//}
			for(int blockcounter=0; blockcounter<blocksize;blockcounter+=12)
			{
				//Step 3: Initialize the write pointer (W7) for TBLWT instruction.
				dspic_send_24_bits(0x200000|(((((blockcounter+address)*2)/3)&0xFF0000)>>12));	//MOV #<DestinationAddress23:16>, W0
				dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
				dspic_send_24_bits(0x200007|(((((blockcounter+address)*2)/3)&0x00FFFF)<<4));	//MOV #<DestinationAddress15:0>, W7
				//Step 4: Initialize the read pointer (W6) and load W0:W5 with the next 4 instruction words to program.
				for(int i=0;i<6;i++)
				{
					dspic_send_24_bits(0x200000|
						(((int)data[blockcounter+(i*2)])<<4)|
						(((int)data[blockcounter+(i*2)+1])<<12)|
						((int)i));
					/**
					MOV #<LSW0>, W0
					MOV #<MSB1:MSB0>, W1
					MOV #<LSW1>, W2
					MOV #<LSW2>, W3
					MOV #<MSB3:MSB2>, W4
					MOV #<LSW3>, W5
					*/
				}
				//Step 5: Set the read pointer (W6) and load the (next set of) write latches.
				dspic_send_24_bits(0xEB0300);	//CLR W6
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBB0BB6);	//TBLWTL [W6++], [W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBBDBB6);	//TBLWTH.B [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBBEBB6);	//TBLWTH.B [W6++], [++W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBB1BB6);	//TBLWTL [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBB0BB6);	//TBLWTL [W6++], [W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBBDBB6);	//TBLWTH.B [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBBEBB6);	//TBLWTH.B [W6++], [++W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBB1BB6);	//TBLWTL [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
			}//Step 6: Repeat steps 3-5 eight times to load the write latches for 32 instructions.	
			//if((address%96)==64)
			//{
				//Step 7: Unlock the NVMCON for writing.
				dspic_send_24_bits(0x200558);	//MOV #0x55, W8
				dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
				dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
				dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
				//Step 8: Initiate the write cycle.
				dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				DelayMs(3);						//Externally time 2 msec
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 9: Reset device internal PC.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
			//}
		}

		public override void writeData(int address, byte[] data, int offset, int length)
		{
			int blocksize = length;

			//Step 1: Exit the Reset vector.
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x000000);	//NOP
			//Step 2: Set the NVMCON to write 16 data words.
			dspic_send_24_bits(0x24005A);	//MOV #0x4005, W10
			dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
			for(int blockcounter=0;blockcounter<blocksize;blockcounter+=8)
			{
				//Step 3: Initialize the write pointer (W7) for TBLWT instruction.
				//dspic_send_24_bits(0x200000|((blockcounter+address&0xFF0000)>>12));	//MOV #0x7F, W0
				dspic_send_24_bits(0x2007F0);//|((blockcounter+address&0xFF0000)>>12));	//MOV #0x7F, W0
				dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
				dspic_send_24_bits(0x2F0007|((((int)blockcounter+address)&0x0FFF)<<4));	//MOV #<DestinationAddress15:0>, W7
				//Step 4: Load W0:W3 with the next 4 data words to program.
				dspic_send_24_bits(0x200000 | (((int)data[blockcounter]) << 4) | (((int)data[blockcounter + 1]) << 12));
				dspic_send_24_bits(0x200001 | (((int)data[blockcounter + 2]) << 4) | (((int)data[blockcounter + 3]) << 12));
				dspic_send_24_bits(0x200002 | (((int)data[blockcounter + 4]) << 4) | (((int)data[blockcounter + 5]) << 12));
				dspic_send_24_bits(0x200003 | (((int)data[blockcounter + 6]) << 4) | (((int)data[blockcounter + 7]) << 12));
				/*for(i=0;i<4;i++)
				{
					dspic_send_24_bits(0x200000|
							(((unsigned long)data[blockcounter+(i*2)])<<4)|
							(((unsigned long)data[blockcounter+(i*2)+1])<<12)|
							((unsigned long) i));
				}*/
				//Step 5: Set the read pointer (W6) and load the (next set of) write latches.
				dspic_send_24_bits(0xEB0300);	//CLR W6
				dspic_send_24_bits(0x000000);	//NOP
				for(int i=0;i<4;i++)
				{
					dspic_send_24_bits(0xBB1BB6);	//TBLWTL [W6++], [W7++]
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x000000);	//NOP
				}
			}//Step 6: Repeat steps 3-4 four times to load the write latches for 16 data words.
			//Step 7: Unlock the NVMCON for writing.
			dspic_send_24_bits(0x200558);	//MOV #0x55, W8
			dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
			dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
			dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
			//Step 8: Initiate the write cycle.
			dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			DelayMs(2);						//Externally time 2 msec
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			//Step 9: Reset device internal PC.
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x000000);	//NOP
		}

		public override void writeConfig(int address, byte[] data, int offset, int length)
		{
			int blocksize = length;
			int payload;
			////if((lastblock&1)==1)
			////{
			//Step 1: Exit the Reset vector.
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x000000);	//NOP
			//Step 2: Initialize the write pointer (W7) for the TBLWT instruction.

			dspic_send_24_bits(0x200007);//|((address&0xFFFF)<<4));	//MOV #0x0000, W7
			////}
			for(int blockcounter=0;blockcounter<blocksize;blockcounter+=2)
			{
				//Step 3: Set the NVMCON to program 1 Configuration register.
				dspic_send_24_bits(0x24008A);	//MOV #0x4008, W10
				dspic_send_24_bits(0x883B0A);	//MOV W10, NVMCON
				//Step 4: Initialize the TBLPAG register.
				//dspic_send_24_bits(0x200000|((blockcounter+address&0xFF0000)>>12));	//MOV #0xF8, W0
				dspic_send_24_bits(0x200F80);//|((blockcounter+address&0xFF0000)>>12));	//MOV #0xF8, W0
				dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
				//Step 5: Load the Configuration register data to W6.
				payload=(((int)data[blockcounter])|(((int)data[blockcounter+1])<<8));
				dspic_send_24_bits(0x200006|((((int)payload)&0xFFFF)<<4));	//MOV #<CONFIG_VALUE>, W6
				//dspic_send_24_bits(0x200036);	//write 0x0003 in the config register
				dspic_send_24_bits(0x000000);	//NOP
				//Step 6: Write the Configuration register data to the write latch and increment the write pointer.
				dspic_send_24_bits(0xBB1B86);	//TBLWTL W6, [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 7: Unlock the NVMCON for programming.
				dspic_send_24_bits(0x200558);	//MOV #0x55, W8
				dspic_send_24_bits(0x883B38);	//MOV W8, NVMKEY
				dspic_send_24_bits(0x200AA9);	//MOV #0xAA, W9
				dspic_send_24_bits(0x883B39);	//MOV W9, NVMKEY
				//Step 8: Initiate the write cycle.
				dspic_send_24_bits(0xA8E761);	//BSET NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				DelayMs(2);						//Externally time 2 msec
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xA9E761);	//BCLR NVMCON, #WR
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 9: Reset device internal PC.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
			}//Step 10: Repeat steps 3-9 until all 7 Configuration registers are cleared.

		}

		public override void readData(int address, byte[] data, int offset, int length)
		{
			int blocksize = length;
			int payload;

			//Step 1: Exit the Reset vector.
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x000000);	//NOP
			dspic_send_24_bits(0x040100);	//GOTO 0x100
			dspic_send_24_bits(0x000000);	//NOP
			//Step 2: Initialize TBLPAG and the read pointer (W6) for TBLRD instruction.
			dspic_send_24_bits(0x2007F0);	//MOV #0x7F, W0
			dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
			dspic_send_24_bits(0x2F0006|((int)((address&0x0FFF)<<4)));	//MOV #<SourceAddress15:0>, W6
			for(int blockcounter=0;blockcounter<blocksize;blockcounter+=8)
			{				
				//Step 3: Initialize the write pointer (W7) and store the next four locations of code memory to W0:W5.
				dspic_send_24_bits(0xEB0380);	//CLR W7
				dspic_send_24_bits(0x000000);	//NOP
				for(int i=0;i<4;i++)
				{
					dspic_send_24_bits(0xBA1BB6);	//TBLRDL [W6++], [W7++]
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x000000);	//NOP
				}
				//Step 4: Output W0:W5 using the VISI register and REGOUT command.
				for(int i=0;i<4;i++)
				{
					dspic_send_24_bits(0x883C20|(int)i);	//MOV W0, VISI
					dspic_send_24_bits(0x000000);	//NOP
					payload=dspic_read_16_bits();	//VISI
					data[blockcounter+(i*2)]=(byte)payload;
					data[blockcounter+((i*2)+1)]=(byte)((payload&0xFF00)>>8);
					dspic_send_24_bits(0x000000);	//NOP
				}
				//Step 5: Reset device internal PC.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
			}
		}

		
		public override void readCode(int address, byte[] data, int offset, int length)
		{
			int blocksize = length / 2;

			int payload;
			if (address >= 0xF80000)
			{

				////if(lastblock&1)
				////{
				//Step 1: Exit the Reset vector.
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
				//Step 2: Initialize TBLPAG, and the read pointer (W6) and the write pointer (W7) for TBLRD instruction.
				dspic_send_24_bits(0x200000 | ((address & 0xFF0000) >> 12));	//MOV #0xF8, W0
				dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
				//dspic_send_24_bits(0x200006|((address&0x00FFFF)<<4));	//MOV #<SourceAddress15:0>, W6
				//ignore the lower 16 bits off the address, they are always 0.
				dspic_send_24_bits(0xEB0300);	//CLR W6
				dspic_send_24_bits(0xEB0380);	//CLR W7
				dspic_send_24_bits(0x000000);	//NOP
				////}

				for (int blockcounter = 0; blockcounter < blocksize; blockcounter += 2)
				{
					//Step 3: Read the Configuration register and write it to the VISI register (located at 0x784).
					dspic_send_24_bits(0xBA0BB6);	//TBLRDL [W6++], [W7]
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x000000);	//NOP
					dspic_send_24_bits(0x883C20);	//MOV W0, VISI
					dspic_send_24_bits(0x000000);	//NOP
					//Step 4: Output the VISI register using the REGOUT command.
					payload = dspic_read_16_bits();	//read <VISI>
					data[blockcounter] = (byte)(payload & 0xff);
					data[blockcounter + 1] = (byte)((payload & 0xFF00) >> 8);
					dspic_send_24_bits(0x000000);	//NOP
					//Step 5: Reset device internal PC.
					dspic_send_24_bits(0x040100);	//GOTO 0x100
					dspic_send_24_bits(0x000000);	//NOP
				}
				//Step 6: Repeat steps 3-5 six times to read all of configuration memory.


			}
			else
			{
				//Step 1: Exit the Reset vector.
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
				//Step 2: Initialize TBLPAG and the read pointer (W6) for TBLRD instruction.
				dspic_send_24_bits(0x200000 | (((((address) * 2) / 3) & 0xFF0000) >> 12));	//MOV #<SourceAddress23:16>, W0
				dspic_send_24_bits(0x880190);	//MOV W0, TBLPAG
				dspic_send_24_bits(0x200006 | (((((address) * 2) / 3) & 0x00FFFF) << 4));	//MOV #<SourceAddress15:0>, W6
				//Step 3: Initialize the write pointer (W7) and store the next four locations of code memory to W0:W5.
				dspic_send_24_bits(0xEB0380);	//CLR W7
				//dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBA1B96);	//TBLRDL [W6], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBADBB6);	//TBLRDH.B [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBADBD6);	//TBLRDH.B [++W6], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBA1BB6);	//TBLRDL [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBA1B96);	//TBLRDL [W6], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBADBB6);	//TBLRDH.B [W6++], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBADBD6);	//TBLRDH.B [++W6], [W7++]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0xBA0BB6);	//TBLRDL [W6++], [W7]
				dspic_send_24_bits(0x000000);	//NOP
				dspic_send_24_bits(0x000000);	//NOP
				//Step 4: Output W0:W5 using the VISI register and REGOUT command.
				int blockcounter = 0;
				for (int i = 0; i < 6; i++)
				{
					dspic_send_24_bits(0x883C20 | (int)i);	//MOV W0, VISI
					dspic_send_24_bits(0x000000);	//NOP
					payload = dspic_read_16_bits();	//Clock out contents of VISI register
					data[blockcounter + i * 2] = (byte)(payload & 0xFF);
					data[blockcounter + i * 2 + 1] = (byte)((payload & 0xFF00) >> 8);
					dspic_send_24_bits(0x000000);	//NOP
				}
				//Step 5: Reset the device internal PC.
				dspic_send_24_bits(0x040100);	//GOTO 0x100
				dspic_send_24_bits(0x000000);	//NOP
			}
		}
	}
}
