#define SDA 		BP_MOSI //PORTBbits.RB9           //-- The SDA output pin
#define SDA_TRIS 	BP_MOSI_DIR //TRISBbits.TRISB9      //-- The SDA Direction Register Bit

unsigned char OWReset(void);
unsigned char OWReadBit(void);
void OWWriteBit(unsigned char c);
void OWWriteByte(unsigned char Data);
unsigned char OWReadByte(void);
void D_MatchRom(const char address[]);
void D_ReadRom(char *address);

unsigned char PROBE_CHECKSUM (unsigned char *BYTE, int size_of_BYTE);

