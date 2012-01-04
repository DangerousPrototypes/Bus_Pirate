RECOMMEND: Use the latest ISE iMPACT software to directly generate an XSVF file
           for your target device in the JTAG scan chain.


OVERVIEW:
When necessary to translate SVF to XSVF, this archive contains the 
stand-alone SVF-to-XSVF translators.


INSTRUCTIONS:
Instructions for translating SVF to XSVF using the stand-alone SVF-to-XSVF
(svf2xsvf) translators:

  SVF2XSVF Basic Syntax:

    svf2xsvf501 [options] -i input.svf -o output.xsvf -a output.txt

      -i input.svf   = any input file name for the input SVF file.
      -o output.xsvf = any output file name for output XSVF file.
      -a output.txt  = any output file name for output text version
                       of the XSVF file.  (For debugging/analysis.)

      [options] may be zero or more of the following:
      -d            = delete pre-existing output files.
      -w            = delete pre-existing output files.
      -r N          = max "retry" count = N times.  (XC9500/XL/XV only)
      -fpga         = Special translation for FPGA bitstreams.  (FPGA only)
      -extensions   = add special TAP command extensions (CoolRunner/II only)
      -xwait        = Use the inline XWAIT XSVF command (CoolRunner/II only)
      -minxstate    = Minimize XSTATE XSVF commands
      -v2           = backward compatible XSVF with XSVF v2 player.
      -xprintsrc N  = print various levels of comments into XSVF (N=0-3)


  SVF2XSVF Options Requirements for Each Family:

    XC9500/XL/XV:                      // No options required

    CoolRunner/II: -r 0 -extensions -xwait

    XC18V00/XCF00S: -r 0               // 0 = zero
        (See readme_xc18v00_xcf00s.txt for more XC18V00/XCF00S information!!!)

    XCF00P: -r 0 -extensions           // 0 = zero

    Spartan-3AN (not Spartan-3A) and indirect SPI flash programming:
        -fpga -rlen 10000

    All Spartan-II (or later) and Virtex (or later) FPGAs:
                  -fpga -extensions    // automatically sets -r 0

    All XC4000+/Spartan/Spartan-XL FPGAs:
                  -fpga -rlen 1024     // automatically sets -r 0

    System ACE MPM/SC: -r 0 -xwait

  Command-line Translation Examples:

    XC9500/XL/XV:
    svf2xsvf -d -i xc9536.svf -o xc9536.xsvf -a xc9536.txt

    CoolRunner/II:
    svf2xsvf -d -r 0 -extensions -xwait -i xcr3064xl.svf
             -o xcr3064xl.xsvf -a xcr3064xl.txt

    XC18V00/XCF00S:
    svf2xsvf -d -r 0 -i prom.svf -o prom.xsvf -a prom.txt
    (See readme_xc18v00_xcf00s.txt for more XC18V00/XCF00S information!!!)

    XCF00P:
    svf2xsvf -d -r 0 -extensions -i prom.svf -o prom.xsvf -a prom.txt

    Spartan-II/Spartan-3/Virtex+ Platform FPGAs:
    svf2xsvf -d -fpga -extensions -i xcv300.svf -o xcv300.xsvf -a xcv300.txt

    XC4000+/Spartan/Spartan-XL FPGAs:
    svf2xsvf -d -fpga -rlen 1024 -i fpga.svf -o fpga.xsvf -a fpga.txt

    System ACE MPM/SC:
    svf2xsvf -d -r 0 -xwait -i mpm.svf -o mpm.xsvf -a mpm.txt



