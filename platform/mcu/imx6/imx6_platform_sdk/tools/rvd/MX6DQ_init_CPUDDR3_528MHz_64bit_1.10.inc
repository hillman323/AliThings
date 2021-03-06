//*================================================================================================
//* Copyright (C) 2011, Freescale Semiconductor, Inc. All Rights Reserved
//* THIS SOURCE CODE IS CONFIDENTIAL AND PROPRIETARY AND MAY NOT
//* BE USED OR DISTRIBUTED WITHOUT THE WRITTEN PERMISSION OF
//* Freescale Semiconductor, Inc.
//*================================================================================================


// DDR init script, written in ARM RVDS syntax
// Target the CPUDDR3 board
// Timing optimized to 528MHz. 64-bit data bus 


// Initialization script for i.MX6q CPU Board (DDR3)
// Version 1.0 (07-06-11)
// v 1.01 (June 16, 2011)
// - based on Boaz's orignal script
// - changed write leveling to 0x44
// - updated DDR calibration (DQS gating, read/write) per latest gathered from stress test code
//
// v 1.02 (July 20, 2011)
// - DDR_INPUT bit cleared for: DQM, RAS, CAS, SDCLK, RESET (All output only...)


// v 1.03 (July 20, 2011)
//   - ADOPT, DDRCTL power down timer activated by configuring MMDC0_MAPSR & MMDC0_MDPDC 

//  v 1.04 (Oct 20, 2011)
//  - More order in file
//  - One-time HW ZQ calibration added.
//  - power down timers enable moved to end of config
// 
//  v 1.05 (Mar 25, 2012)
//  - IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET setup corrected to set DDR_SEL='11' (DDR3). This corrects ZQ calibration operation. 
//    Improvment of DDR signals levels is expected.  
//  - DQS drive strength reduced 6->7,device Rtt_nom 120->60, show better overclocking with new DDR_SEL 
//  - tMRD value increased by 1 to match JEDEC

//  v 1.06 (june 12, 2012)
// - SDCLK duty cycle fine tunning changed from default to low.
// - Previous step of setting DRAM_RESET/DDR_SEL='11' is reversed back to '00'
//   , after finding the later to have a better DDR signals integrity. IO design are still investigating this.
// -  i.mx ODT configs changed : 60 -> 120Ohm, to save power. See more in comment bellow.
// - IOMUX configs: Remove redundant (input only...) pull setup from output signals: SDCKE, ODT

//  v 1.07 
// - Silicon version v1.2 (MX6Q/DxxxxxxxC) compatible. SDCLK duty cycle fine tute is back to default (2).
// - MAARCR registers modified to reflect the best simulated ADOPT performence

//  v 1.08 (Sep 2012)
// DQ & DQS input sensing mode changed to CMOS - for power saving.
//(IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL,IOMUXC_SW_PAD_CTL_GRP_DDRMODE )
// CMOS mode means less timing margins than "DDR" mode.
// For i.mx6Q v1.2, CMOS mode seen to operate 10% above the target 528MHz
// , so it is ok to cjange default setup to CMOS 

//  v 1.09 (Sep 2012)
// MDOR register updated RST_to_CKE=0x23, SDE_to_RST=0x10, to meet JEDEC. 
//Apparently there is no actual DDR issue with previous (smaller ) values, as these parameters got spares. 

// v 1.10 (Nov 2012)
// DQ & DQS input sensing mode changed back to differential to align with JEDEC and IO pad designer recommendation.
// Removed "MMDC0_MAARCR ADOPT optimized priorities. Dyn jump disabled" - settings like this belong in an OS configuration
//  as the setting provided previously was causing other bus master issues. The DRAM init is strictly meant to 
//  initialize the DRAM interface for proper read/write transactions and not to perform any AXI/arbitration optimizations
//================================================================================================



wait = on

//*================================================================================================
// Disable WDOG
//*================================================================================================
setmem /16 0x020bc000 = 0x30


//*================================================================================================
// Enable all clocks (they are disabled by ROM code)
//*================================================================================================
setmem /32 0x020c4068 = 0xffffffff
setmem /32 0x020c406c = 0xffffffff
setmem /32 0x020c4070 = 0xffffffff
setmem /32 0x020c4074 = 0xffffffff
setmem /32 0x020c4078 = 0xffffffff
setmem /32 0x020c407c = 0xffffffff
setmem /32 0x020c4080 = 0xffffffff
setmem /32 0x020c4084 = 0xffffffff


//*================================================================================================
// Initialize 64-bit DDR3 
//*================================================================================================

//######################################################
// IOMUX 
//######################################################

//DDR IO TYPE:
setmem /32 0x020e0798 = 0x000c0000 // IOMUXC_SW_PAD_CTL_GRP_DDR_TYPE - DDR_SEL=11 
setmem /32 0x020e0758 = 0x00000000 // IOMUXC_SW_PAD_CTL_GRP_DDRPKE - PKE=0 , Pull disabled for all, except DQS.

//CLOCK:
setmem /32 0x020e0588 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_0 - DSE=101, DDR_INPUT=0, HYS=0
setmem /32 0x020e0594 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDCLK_1 - DSE=101, DDR_INPUT=0, HYS=0

//ADDRESS:
setmem /32 0x020e056c = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_CAS - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0578 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_RAS - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e074c = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_ADDDS - DSE=110

//CONTROL:
setmem /32 0x020e057c = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_RESET - DSE=110, DDR_INPUT=1, HYS=0, DDR_SEL=00

setmem /32 0x020e058c = 0x00000000 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDBA2
setmem /32 0x020e059c = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT0
setmem /32 0x020e05a0 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDODT1
setmem /32 0x020e078c = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_CTLDS - DSE=110


//DATA STROBE:
setmem /32 0x020e0750 = 0x00020000 // IOMUXC_SW_PAD_CTL_GRP_DDRMODE_CTL - DDR_INPUT=0, CMOS

setmem /32 0x020e05a8 = 0x00000038 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS0 - DSE=110
setmem /32 0x020e05b0 = 0x00000038 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS1 - DSE=110
setmem /32 0x020e0524 = 0x00000038 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS2 - DSE=110
setmem /32 0x020e051c = 0x00000038 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS3 - DSE=110
setmem /32 0x020e0518 = 0x00000038 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS4 - DSE=110
setmem /32 0x020e050c = 0x00000038 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS5 - DSE=110
setmem /32 0x020e05b8 = 0x00000038 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS6 - DSE=110
setmem /32 0x020e05c0 = 0x00000038 // IOMUXC_SW_PAD_CTL_PAD_DRAM_SDQS7 - DSE=110

//DATA:
setmem /32 0x020e0774 = 0x00020000 // IOMUXC_SW_PAD_CTL_GRP_DDRMODE- DDR_INPUT 0,CMOS

setmem /32 0x020e0784 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B0DS - DSE=110
setmem /32 0x020e0788 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B1DS - DSE=110
setmem /32 0x020e0794 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B2DS - DSE=110
setmem /32 0x020e079c = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B3DS - DSE=110
setmem /32 0x020e07a0 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B4DS - DSE=110
setmem /32 0x020e07a4 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B5DS - DSE=110
setmem /32 0x020e07a8 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B6DS - DSE=110
setmem /32 0x020e0748 = 0x00000030 // IOMUXC_SW_PAD_CTL_GRP_B7DS - DSE=110

setmem /32 0x020e05ac = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM0 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e05b4 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM1 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0528 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM2 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0520 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM3 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0514 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM4 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e0510 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM5 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e05bc = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM6 - DSE=110, DDR_INPUT=1, HYS=0
setmem /32 0x020e05c4 = 0x00000030 // IOMUXC_SW_PAD_CTL_PAD_DRAM_DQM7 - DSE=110, DDR_INPUT=1, HYS=0



//######################################################
//Calibrations:
//######################################################
// ZQ:
setmem /32 0x021b0800 = 0xa1390003      // DDR_PHY_P0_MPZQHWCTRL, enable both one-time & periodic HW ZQ calibration.

// write leveling
setmem /32 0x021b080c = 0x001F001F
setmem /32 0x021b0810 = 0x001F001F

setmem /32 0x021b480c = 0x00440044
setmem /32 0x021b4810 = 0x00440044

//DQS gating, read delay, write delay calibration values based on calibration compare of 0x00ffff00:
// It is highly recommended for the user to run calibration code on her/his specific board 
//and replace following delay values accordingly: 



//Read DQS Gating calibration
setmem /32 0x021b083c = 0x4333033F
setmem /32 0x021b0840 = 0x0339033E
setmem /32 0x021b483c = 0x433F0343
setmem /32 0x021b4840 = 0x03490320

//Read calibration
setmem /32 0x021b0848 = 0x42363838
setmem /32 0x021b4848 = 0x3F343242

//Write calibration
setmem /32 0x021b0850 = 0x37424844
setmem /32 0x021b4850 = 0x48264731



//read data bit delay: (3 is the reccommended default value, although out of reset value is 0):
setmem /32 0x021b081c = 0x33333333      // DDR_PHY_P0_MPREDQBY0DL3
setmem /32 0x021b0820 = 0x33333333      // DDR_PHY_P0_MPREDQBY1DL3
setmem /32 0x021b0824 = 0x33333333      // DDR_PHY_P0_MPREDQBY2DL3
setmem /32 0x021b0828 = 0x33333333      // DDR_PHY_P0_MPREDQBY3DL3
setmem /32 0x021b481c = 0x33333333      // DDR_PHY_P1_MPREDQBY0DL3
setmem /32 0x021b4820 = 0x33333333      // DDR_PHY_P1_MPREDQBY1DL3
setmem /32 0x021b4824 = 0x33333333      // DDR_PHY_P1_MPREDQBY2DL3
setmem /32 0x021b4828 = 0x33333333      // DDR_PHY_P1_MPREDQBY3DL3


//setmem /32	0x021b082c =	0xf3333333 	// wr bit delay, byte 0
//setmem /32	0x021b0830 =	0xf3333333 	// wr bit delay, byte 1
//setmem /32	0x021b0834 =	0xf3333333 	// wr bit delay, byte 2
//setmem /32	0x021b0838 =	0xf3333333 	// wr bit delay, byte 3
//setmem /32	0x021b482c =	0xf3333333 	// wr bit delay, byte 4
//setmem /32	0x021b4830 =	0xf3333333 	// wr bit delay, byte 5
//setmem /32	0x021b4834 =	0xf3333333 	// wr bit delay, byte 6
//setmem /32	0x021b4838 =	0xf3333333 	// wr bit delay, byte 7


//For i.mx6qd parts of versions A & B (v1.0, v1.1), uncomment the following lines. For version C (v1.2), keep commented
//setmem /32	0x021b08c0 =	0x24911492	// fine tune SDCLK duty cyc to low - seen to improve measured duty cycle of i.mx6
//setmem /32	0x021b48c0 =	0x24911492	

//######################################################
// Complete calibration by forced measurment:
//######################################################
setmem /32 0x021b08b8 = 0x00000800      // DDR_PHY_P0_MPMUR0, frc_msr
setmem /32 0x021b48b8 = 0x00000800      // DDR_PHY_P0_MPMUR0, frc_msr

//######################################################
//MMDC init:


//528MHz
//in DDR3, 64-bit mode, only MMDC0 is initiated:
setmem /32 0x021b0004 = 0x00020036		// MMDC0_MDPDC see spread sheet for timings
setmem /32 0x021b0008 = 0x09444040		// MMDC0_MDOTC see spread sheet for timings
setmem /32 0x021b000c = 0x555A7975		// MMDC0_MDCFG0 see spread sheet for timings. CL=8
setmem /32 0x021b0010 = 0xFF538F64		// MMDC0_MDCFG1 see spread sheet for timings
setmem /32 0x021b0014 = 0x01ff00db      // MMDC0_MDCFG2 - tRRD - 4ck; tWTR - 4ck; tRTP - 4ck; tDLLK - 512ck
setmem /32 0x021b0018 = 0x00081740      // MMDC0_MDMISC, RALAT=0x5
//MDMISC: RALAT kept to the high level of 5. 
//MDMISC: consider reducing RALAT if your 528MHz board design allow that. Lower RALAT benefits: 
//a. better operation at low frequency
//b. Small performence improvment

setmem /32 0x021b001c = 0x00008000      // MMDC0_MDSCR

setmem /32 0x021b002c = 0x000026d2      // MMDC0_MDRWD
setmem /32 0x021b0030 = 0x005a1023      // MMDC0_MDOR - tXPR - 91ck; SDE_to_RST - 13ck; RST_to_CKE - 33ck
setmem /32 0x021b0040 = 0x00000027      // CS0_END - 0x4fffffff 

setmem /32 0x021b0000 = 0xc31a0000      // MMDC0_MDCTL - row - 14bits; col = 10bits; burst length 8; 64-bit data bus

//######################################################
// Initialize 2GB DDR3 - Micron MT41J128M , but fit wide range of other DDR3 devices
//MR2:
setmem /32 0x021b001c = 0x04088032      // MMDC0_MDSCR
setmem /32 0x021b001c = 0x0408803a      // MMDC0_MDSCR


//MR3:
setmem /32 0x021b001c = 0x00008033      // MMDC0_MDSCR
setmem /32 0x021b001c = 0x0000803b      // MMDC0_MDSCR
//MR1:
setmem /32 0x021b001c = 0x00048031      // MMDC0_MDSCR
setmem /32 0x021b001c = 0x00048039      // MMDC0_MDSCR
//MR0:

setmem /32 0x021b001c = 0x09408030      // MMDC0_MDSCR, 
setmem /32 0x021b001c = 0x09408038      // MMDC0_MDSCR, 

//DDR device ZQ calibration:
setmem /32 0x021b001c = 0x04008040      // MMDC0_MDSCR, 
setmem /32 0x021b001c = 0x04008048      // MMDC0_MDSCR
//######################################################
//final DDR setup, before operation start:

setmem /32 0x021b0020 = 0x00005800      // MMDC0_MDREF, enable auto refresh, set refresh rate.

//Following ODT setup (0x11117) represents(along with obove DDR device configs) : i.mx_ODT=DDR_device_ODT=120OHm.
//User might to also interested in trying the value of 0x00000007,which represents: i.mx_ODT disabled, DDR_device_ODT=120Ohm.
//0x00000007 saves more power, and seen to run very well with Freescale RDKs. Still, running with no ODT has it's implications 
// of signal integrity and should be carefully simulated during board design.

setmem /32 0x021b0818 = 0x00011117      // DDR_PHY_P0_MPODTCTRL, ODT enable
setmem /32 0x021b4818 = 0x00011117      // DDR_PHY_P1_MPODTCTRL

setmem /32 0x021b0004 = 0x00025576		// MMDC0_MDPDC see spread sheet for timings, SDCTL power down enabled

setmem /32 0x021b0404 = 0x00011006      //MMDC0_MAPSR ADOPT power down enabled

setmem /32 0x021b001c = 0x00000000      // MMDC0_MDSCR

