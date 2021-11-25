/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/


/**************************************************************************/
/**************************************************************************/
/**                                                                       */ 
/** USBX Component                                                        */ 
/**                                                                       */
/**   RX Controller Driver                                                */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/ 
/*                                                                        */ 
/*  COMPONENT DEFINITION                                   RELEASE        */ 
/*                                                                        */ 
/*    ux_dcd_rx.h                                         PORTABLE C      */ 
/*                                                           6.1          */
/*  AUTHOR                                                                */
/*                                                                        */
/*    Chaoqiong Xiao, Microsoft Corporation                               */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */ 
/*    This file defines the equivalences for the RX RENESAS UDC           */ 
/*    controller.                                                         */ 
/*                                                                        */ 
/*  RELEASE HISTORY                                                       */ 
/*                                                                        */ 
/*    DATE              NAME                      DESCRIPTION             */ 
/*                                                                        */ 
/*  05-19-2020     Chaoqiong Xiao           Initial Version 6.0           */
/*  09-30-2020     Chaoqiong Xiao           Modified comment(s),          */
/*                                            resulting in version 6.1    */
/*                                                                        */
/**************************************************************************/

#ifndef UX_DCD_RX_H
#define UX_DCD_RX_H


/* Define RX generic equivalences.  */

#define UX_DCD_RX_SLAVE_CONTROLLER                          0x80
#define UX_DCD_RX_MAX_ED                                    10
#define UX_DCD_RX_ENABLE                                    1
#define UX_DCD_RX_DISABLE                                   0
#define UX_DCD_RX_MAX_BULK_PAYLOAD                          512 
#define UX_DCD_RX_MAX_CONTROL_PAYLOAD                       512 
#define UX_DCD_RX_MAX_BUF_SIZE                              64
#define UX_DCD_RX_MAX_BUF_NUM                               127

/* Define rx HCOR register mapping.   If dynamic address is used, leave the base address as 0. */
#ifdef UX_RX71M
#define UX_RX_USBA_BASE                                     0x000D0400
#endif
#define UX_RX_USB0_BASE                                     0x000A0000
#define UX_RX_USB1_BASE                                     0x000A0200
#define UX_RX_USB_BASE_DYNAMIC                              0x00000000

/* Define base address if not already defined.  Other define locations include the command line or ux_user.h.  */

#ifndef UX_RX_USB_BASE
#define UX_RX_USB_BASE                                      UX_RX_USB0_BASE
#endif


/* Define RX HCOR register mapping.  */

#define UX_RX_DCD_SYSCFG                                    (UX_RX_USB_BASE + 0x00)
#ifdef UX_RX71M
#define UX_RX_DCD_BUSWAIT                                   (UX_RX_USB_BASE + 0x02)
#endif
#define UX_RX_DCD_SYSSTS                                    (UX_RX_USB_BASE + 0x04)
#ifdef UX_RX71M
#define UX_RX_DCD_PLLSTA                                    (UX_RX_USB_BASE + 0x06)
#endif
#define UX_RX_DCD_DVSTCTR                                   (UX_RX_USB_BASE + 0x08)
#define UX_RX_DCD_CFIFO                                     (UX_RX_USB_BASE + 0x14)
#define UX_RX_DCD_D0FIFO                                    (UX_RX_USB_BASE + 0x18)
#define UX_RX_DCD_D1FIFO                                    (UX_RX_USB_BASE + 0x1C)
#define UX_RX_DCD_CFIFOSEL                                  (UX_RX_USB_BASE + 0x20)
#define UX_RX_DCD_CFIFOCTR                                  (UX_RX_USB_BASE + 0x22)
#define UX_RX_DCD_D0FIFOSEL                                 (UX_RX_USB_BASE + 0x28)
#define UX_RX_DCD_D0FIFOCTR                                 (UX_RX_USB_BASE + 0x2A)
#define UX_RX_DCD_D1FIFOSEL                                 (UX_RX_USB_BASE + 0x2C)
#define UX_RX_DCD_D1FIFOCTR                                 (UX_RX_USB_BASE + 0x2E)
#define UX_RX_DCD_INTENB0                                   (UX_RX_USB_BASE + 0x30)
#define UX_RX_DCD_INTENB1                                   (UX_RX_USB_BASE + 0x32)
#define UX_RX_DCD_BRDYENB                                   (UX_RX_USB_BASE + 0x36)
#define UX_RX_DCD_NRDYENB                                   (UX_RX_USB_BASE + 0x38)
#define UX_RX_DCD_BEMPENB                                   (UX_RX_USB_BASE + 0x3A)
#define UX_RX_DCD_SOFCFG                                    (UX_RX_USB_BASE + 0x3C)
#ifdef UX_RX71M
#define UX_RX_DCD_PHYSET                                    (UX_RX_USB_BASE + 0x3E)
#endif
#define UX_RX_DCD_INTSTS0                                   (UX_RX_USB_BASE + 0x40)
#define UX_RX_DCD_INTSTS1                                   (UX_RX_USB_BASE + 0x42)
#define UX_RX_DCD_BRDYSTS                                   (UX_RX_USB_BASE + 0x46)
#define UX_RX_DCD_NRDYSTS                                   (UX_RX_USB_BASE + 0x48)
#define UX_RX_DCD_BEMPSTS                                   (UX_RX_USB_BASE + 0x4A)
#define UX_RX_DCD_FRMNUM                                    (UX_RX_USB_BASE + 0x4C)
#define UX_RX_DCD_DVCHGR                                    (UX_RX_USB_BASE + 0x4E)
#define UX_RX_DCD_USBADDR                                   (UX_RX_USB_BASE + 0x50)
#define UX_RX_DCD_USBREQ                                    (UX_RX_USB_BASE + 0x54)
#define UX_RX_DCD_USBVAL                                    (UX_RX_USB_BASE + 0x56)
#define UX_RX_DCD_USBINDX                                   (UX_RX_USB_BASE + 0x58)
#define UX_RX_DCD_USBLENG                                   (UX_RX_USB_BASE + 0x5A)
#define UX_RX_DCD_DCPCFG                                    (UX_RX_USB_BASE + 0x5C)
#define UX_RX_DCD_DCPMAXP                                   (UX_RX_USB_BASE + 0x5E)
#define UX_RX_DCD_DCPCTR                                    (UX_RX_USB_BASE + 0x60)
#define UX_RX_DCD_PIPESEL                                   (UX_RX_USB_BASE + 0x64)
#define UX_RX_DCD_PIPECFG                                   (UX_RX_USB_BASE + 0x68)
#define UX_RX_DCD_PIPEMAXP                                  (UX_RX_USB_BASE + 0x6C)
#define UX_RX_DCD_PIPEPERI                                  (UX_RX_USB_BASE + 0x6E)
#define UX_RX_DCD_PIPE1CTR                                  (UX_RX_USB_BASE + 0x70)
#define UX_RX_DCD_PIPE2CTR                                  (UX_RX_USB_BASE + 0x72)
#define UX_RX_DCD_PIPE3CTR                                  (UX_RX_USB_BASE + 0x74)
#define UX_RX_DCD_PIPE4CTR                                  (UX_RX_USB_BASE + 0x76)
#define UX_RX_DCD_PIPE5CTR                                  (UX_RX_USB_BASE + 0x78)
#define UX_RX_DCD_PIPE6CTR                                  (UX_RX_USB_BASE + 0x7A)
#define UX_RX_DCD_PIPE7CTR                                  (UX_RX_USB_BASE + 0x7C)
#define UX_RX_DCD_PIPE8CTR                                  (UX_RX_USB_BASE + 0x7E)
#define UX_RX_DCD_PIPE9CTR                                  (UX_RX_USB_BASE + 0x80)
#define UX_RX_DCD_PIPE1TRE                                  (UX_RX_USB_BASE + 0x90)
#define UX_RX_DCD_PIPE1TRN                                  (UX_RX_USB_BASE + 0x92)
#define UX_RX_DCD_PIPE2TRE                                  (UX_RX_USB_BASE + 0x94)
#define UX_RX_DCD_PIPE2TRN                                  (UX_RX_USB_BASE + 0x96)
#define UX_RX_DCD_PIPE3TRE                                  (UX_RX_USB_BASE + 0x98)
#define UX_RX_DCD_PIPE3TRN                                  (UX_RX_USB_BASE + 0x9A)
#define UX_RX_DCD_PIPE4TRE                                  (UX_RX_USB_BASE + 0x9C)
#define UX_RX_DCD_PIPE4TRN                                  (UX_RX_USB_BASE + 0x9E)
#define UX_RX_DCD_PIPE5TRE                                  (UX_RX_USB_BASE + 0xA0)
#define UX_RX_DCD_PIPE5TRN                                  (UX_RX_USB_BASE + 0xA2)
#define UX_RX_DCD_DEVADD0                                   (UX_RX_USB_BASE + 0xD0)
#define UX_RX_DCD_DEVADD1                                   (UX_RX_USB_BASE + 0xD2)
#define UX_RX_DCD_DEVADD2                                   (UX_RX_USB_BASE + 0xD4)
#define UX_RX_DCD_DEVADD3                                   (UX_RX_USB_BASE + 0xD6)
#define UX_RX_DCD_DEVADD4                                   (UX_RX_USB_BASE + 0xD8)
#define UX_RX_DCD_DEVADD5                                   (UX_RX_USB_BASE + 0xDA)
#ifdef UX_RX71M
#define UX_RX_DCD_LPCTRL                                    (UX_RX_USB_BASE + 0x100)
#define UX_RX_DCD_LPSTS                                     (UX_RX_USB_BASE + 0x102)
#endif

/* Define RX control register values.  */
#define UX_RX_DCD_SYSCFG_SCKE                               (1ul<<10)
#ifdef UX_RX71M
#define UX_RX_DCD_SYSCFG_CNEN                               (1ul<<8)
#define UX_RX_DCD_SYSCFG_HSE                                (1ul<<7)
#endif
#define UX_RX_DCD_SYSCFG_DCFM                               (1ul<<6)
#define UX_RX_DCD_SYSCFG_DRPD                               (1ul<<5)
#define UX_RX_DCD_SYSCFG_DPRPU                              (1ul<<4)
#define UX_RX_DCD_SYSCFG_USBE                               1ul

#define UX_RX_DCD_SYSSTS_LNST                               3ul
#define UX_RX_DCD_SYSSTS_SOFEN                              0x20ul

#ifdef UX_RX71M
#define UX_RX_DCD_PLLSTA_PLLLOCK                            1ul
#endif

#define UX_RX_DCD_DVSTCTR_UACKEY0                           (1ul<<15)
#define UX_RX_DCD_DVSTCTR_UACKEY1                           (1ul<<12)
#define UX_RX_DCD_DVSTCTR_WKUP                              (1ul<<8)
#define UX_RX_DCD_DVSTCTR_RWUPE                             (1ul<<7)
#define UX_RX_DCD_DVSTCTR_USBRST                            (1ul<<6)
#define UX_RX_DCD_DVSTCTR_RESUME                            (1ul<<5)
#define UX_RX_DCD_DVSTCTR_UACT                              (1ul<<4)
#define UX_RX_DCD_DVSTCTR_RHST                              (0x7ul)

#define UX_RX_DCD_DVSTCTR_SPEED_LOW                         (1ul)
#define UX_RX_DCD_DVSTCTR_SPEED_FULL                        (2ul)
#define UX_RX_DCD_DVSTCTR_SPEED_HIGH                        (3ul)
#define UX_RX_DCD_DVSTCTR_RESET_IN_PROGRESS                 (4ul)

#define UX_RX_DCD_TESTMODE_HOSTPCC                          (1ul<<15)

#define UX_RX_DCD_DXFBCFG_DFACC                             (0x00ul<<12)

#define UX_RX_DCD_CFIFOSEL_RCNT                             (1ul<<15)
#define UX_RX_DCD_CFIFOSEL_REW                              (1ul<<14)
#define UX_RX_DCD_CFIFOSEL_MBW_8                            (0ul<<10)
#define UX_RX_DCD_CFIFOSEL_MBW_MASK                         (1ul<<10)
#define UX_RX_DCD_CFIFOSEL_BIGEND                           (1ul<<8)
#define UX_RX_DCD_CFIFOSEL_ISEL                             (1ul<<5)
#define UX_RX_DCD_CFIFOSEL_CURPIPE_MASK                     (0xFul)

#define UX_RX_DCD_DFIFOSEL_RCNT                             (1ul<<15)
#define UX_RX_DCD_DFIFOSEL_REW                              (1ul<<14)
#define UX_RX_DCD_DFIFOSEL_DCLRM                            (1ul<<13)
#define UX_RX_DCD_DFIFOSEL_DREQE                            (1ul<<12)
#define UX_RX_DCD_DFIFOSEL_MBW_8                            (0ul<<10)
#define UX_RX_DCD_DFIFOSEL_BIGEND                           (1ul<<8)

#define UX_RX_DCD_FIFOCTR_BVAL                              (1ul<<15)
#define UX_RX_DCD_FIFOCTR_BCLR                              (1ul<<14)
#define UX_RX_DCD_FIFOCTR_FRDY                              (1ul<<13)
#define UX_RX_DCD_FIFOCTR_DTLN                              (0xFFFul)
                                                        
#define UX_RX_DCD_INTENB0_VBSE                              (1ul<<15)
#define UX_RX_DCD_INTENB0_RSME                              (1ul<<14)
#define UX_RX_DCD_INTENB0_SOFE                              (1ul<<13)
#define UX_RX_DCD_INTENB0_DVSE                              (1ul<<12)
#define UX_RX_DCD_INTENB0_CTRE                              (1ul<<11)
#define UX_RX_DCD_INTENB0_BEMPE                             (1ul<<10)
#define UX_RX_DCD_INTENB0_NRDYE                             (1ul<<9)
#define UX_RX_DCD_INTENB0_BRDYE                             (1ul<<8)

#define UX_RX_DCD_INTENB1_BCHGE                             (1ul<<14)
#define UX_RX_DCD_INTENB1_DTCHE                             (1ul<<12)
#define UX_RX_DCD_INTENB1_ATTCHE                            (1ul<<11)
#define UX_RX_DCD_INTENB1_EOFERRE                           (1ul<<6)
#define UX_RX_DCD_INTENB1_SIGNE                             (1ul<<5)
#define UX_RX_DCD_INTENB1_SACKE                             (1ul<<4)

#define UX_RX_DCD_PIPE0                                     (1ul<<0)
#define UX_RX_DCD_PIPE1                                     (1ul<<1)
#define UX_RX_DCD_PIPE2                                     (1ul<<2)
#define UX_RX_DCD_PIPE3                                     (1ul<<3)
#define UX_RX_DCD_PIPE4                                     (1ul<<4)
#define UX_RX_DCD_PIPE5                                     (1ul<<5)
#define UX_RX_DCD_PIPE6                                     (1ul<<6)
#define UX_RX_DCD_PIPE7                                     (1ul<<7)
#define UX_RX_DCD_PIPE8                                     (1ul<<8)
#define UX_RX_DCD_PIPE9                                     (1ul<<9)
#define UX_RX_DCD_PIPE_ALL                                  (0x3FFul)

#define UX_RX_DCD_SOFCFG_TRNENSEL                           (1ul<<8)
#define UX_RX_DCD_SOFCFG_BRDYM                              (1ul<<6)
#define UX_RX_DCD_SOFCFG_INIT                               (1ul<<5)

#ifdef UX_RX71M
#define UX_RX_DCD_PHYSET_HSEB                               (1ul<<15)
#define UX_RX_DCD_PHYSET_REPSEL1                            (1ul<<9)
#define UX_RX_DCD_PHYSET_REPSEL0                            (1ul<<8)
#define UX_RX_DCD_PHYSET_CLKSEL1                            (1ul<<5)
#define UX_RX_DCD_PHYSET_CLKSEL0                            (1ul<<4)
#define UX_RX_DCD_PHYSET_CDPEN                              (1ul<<3)
#define UX_RX_DCD_PHYSET_PLLRESET                           (1ul<<1)
#define UX_RX_DCD_PHYSET_DIRPD                              (1ul<<0)
#endif

#define UX_RX_DCD_INTSTS0_VBINT                             (1ul<<15)
#define UX_RX_DCD_INTSTS0_RESM                              (1ul<<14)
#define UX_RX_DCD_INTSTS0_SOFR                              (1ul<<13)
#define UX_RX_DCD_INTSTS0_DVST                              (1ul<<12)
#define UX_RX_DCD_INTSTS0_CTRT                              (1ul<<11)
#define UX_RX_DCD_INTSTS0_BEMP                              (1ul<<10)
#define UX_RX_DCD_INTSTS0_NRDY                              (1ul<<9)
#define UX_RX_DCD_INTSTS0_BRDY                              (1ul<<8)
#define UX_RX_DCD_INTSTS0_VBSTS                             (1ul<<7)
#define UX_RX_DCD_INTSTS0_DVSQ_MASK                         (7ul<<4)
#define UX_RX_DCD_INTSTS0_VALID                             (1ul<<3)
#define UX_RX_DCD_INTSTS0_CTSQ_MASK                         (7ul)

#define UX_RX_DCD_INTSTS0_DVSQ_POWERED                      0x0000ul
#define UX_RX_DCD_INTSTS0_DVSQ_DEFAULT                      0x0010ul
#define UX_RX_DCD_INTSTS0_DVSQ_ADDRESS                      0x0020ul
#define UX_RX_DCD_INTSTS0_DVSQ_CONFIGURED                   0x0030ul
#define UX_RX_DCD_INTSTS0_DVSQ_SUSPENDED_POWERED            0x0040ul
#define UX_RX_DCD_INTSTS0_DVSQ_SUSPENDED_DEFAULT            0x0050ul
#define UX_RX_DCD_INTSTS0_DVSQ_SUSPENDED_ADDRESS            0x0060ul
#define UX_RX_DCD_INTSTS0_DVSQ_SUSPENDED_CONFIGURED         0x0070ul

#define UX_RX_DCD_INTSTS0_CTSQ_SETUP                        0x0000ul
#define UX_RX_DCD_INTSTS0_CTSQ_CRDS                         0x0001ul
#define UX_RX_DCD_INTSTS0_CTSQ_CRSS                         0x0002ul
#define UX_RX_DCD_INTSTS0_CTSQ_CWDS                         0x0003ul
#define UX_RX_DCD_INTSTS0_CTSQ_CWSS                         0x0004ul
#define UX_RX_DCD_INTSTS0_CTSQ_CWNDSS                       0x0005ul
#define UX_RX_DCD_INTSTS0_CTSQ_CTSE                         0x0006ul

#define UX_RX_DCD_INTSTS1_BCHG                              (1ul<<14)
#define UX_RX_DCD_INTSTS1_DTCH                              (1ul<<12)
#define UX_RX_DCD_INTSTS1_ATTCH                             (1ul<<11)
#define UX_RX_DCD_INTSTS1_EOFERR                            (1ul<<6)
#define UX_RX_DCD_INTSTS1_SIGN                              (1ul<<5)
#define UX_RX_DCD_INTSTS1_SACK                              (1ul<<4)

#define UX_RX_DCD_FRMNUM_OVRN                               (1ul<<15)
#define UX_RX_DCD_FRMNUM_CRCE                               (1ul<<14)
#define UX_RX_DCD_FRMNUM_FRNM_MASK                          (0x7FFul)
 
#define UX_RX_DCD_DCPCFG_DIR                                (1ul<<4)
#define UX_RX_DCD_DCPCFG_SHTNAK                             (1ul<<7)
#define UX_RX_DCD_DCPCFG_CNTMD                              (1ul<<8)

#define UX_RX_DCD_DCPMAXP_DEVADDR_SHIFT                     (12ul)
#define UX_RX_DCD_DCPMAXP_DEVADDR_MASK                      0xf000ul

#define UX_RX_DCD_DCPCTR_BSTS                               (1ul<<15)
#define UX_RX_DCD_DCPCTR_INBUFM                             (1ul<<14)
#define UX_RX_DCD_DCPCTR_CSCLR                              (1ul<<13)
#define UX_RX_DCD_DCPCTR_CSSTS                              (1ul<<12)
#define UX_RX_DCD_DCPCTR_SUREQCLR                           (1ul<<11)
#define UX_RX_DCD_DCPCTR_SQCLR                              (1ul<<8)
#define UX_RX_DCD_DCPCTR_SQSET                              (1ul<<7)
#define UX_RX_DCD_DCPCTR_SQMON                              (1ul<<6)
#define UX_RX_DCD_DCPCTR_PBUSY                              (1ul<<5)
#define UX_RX_DCD_DCPCTR_PINGE                              (1ul<<4)
#define UX_RX_DCD_DCPCTR_CCPL                               (1ul<<2)
#define UX_RX_DCD_DCPCTR_PID_MASK                           (3ul)
#define UX_RX_DCD_DCPCTR_PIDNAK                             (0ul)
#define UX_RX_DCD_DCPCTR_PIDBUF                             (1ul)
#define UX_RX_DCD_DCPCTR_PIDSTALL                           (2ul)
#define UX_RX_DCD_DCPCTR_PIDSTALL2                          (3ul)


#define UX_RX_DCD_PIPECFG_TYPE_MASK                         (0xC000ul)
#define UX_RX_DCD_PIPECFG_TYPE_BIT_USED                     (0ul)
#define UX_RX_DCD_PIPECFG_TYPE_BULK                         (1ul<<14)
#define UX_RX_DCD_PIPECFG_TYPE_INTERRUPT                    (2ul<<14)
#define UX_RX_DCD_PIPECFG_TYPE_ISOCHRONOUS                  (3ul<<14)
#define UX_RX_DCD_PIPECFG_BFRE                              (1ul<<10)
#define UX_RX_DCD_PIPECFG_DBLB                              (1ul<<9)
#define UX_RX_DCD_PIPECFG_CNTMD                             (1ul<<8)
#define UX_RX_DCD_PIPECFG_SHTNAK                            (1ul<<7)
#define UX_RX_DCD_PIPECFG_DIR                               (1ul<<4)
#define UX_RX_DCD_PIPECFG_EPNUM_MASK                        (0xFul)

#define UX_RX_DCD_PIPEBUF_SIZEMASK                          (0x1Ful<<10)
#define UX_RX_DCD_PIPEBUF_BUFNMBMASK                        (0xFFul<<10)
#define UX_RX_DCD_PIPEBUF_SHIFT                             (10)

#define UX_RX_DCD_PIPEMAXP_DEVSELMASK                       (0xFul<<12)
#define UX_RX_DCD_PIPEMAXP_DEVSEL_SHIFT                     (12)
#define UX_RX_DCD_PIPEMAXP_MXPSMASK                         (0x7FFul)

#define UX_RX_DCD_PIPE1TRE_TRCLR                            (1ul<<8)
#define UX_RX_DCD_PIPE1TRE_TRENB                            (1ul<<9)

#define UX_RX_DCD_FIFO_D0                                   0
#define UX_RX_DCD_FIFO_D1                                   1
#define UX_RX_DCD_FIFO_C                                    2

#define UX_RX_DCD_DEVADDX_SPEED_LOW                         (1ul<<6)
#define UX_RX_DCD_DEVADDX_SPEED_FULL                        (2ul<<6)
#define UX_RX_DCD_DEVADDX_SPEED_HIGH                        (3ul<<6)
#define UX_RX_DCD_DEVADDX_UPPHUB_SHIFT                      11
#define UX_RX_DCD_DEVADDX_HUBPORT_SHIFT                     8

#ifdef UX_RX71M
#define UX_RX_DCD_LPCTRL_HWUPM                              (1ul<<7)
#define UX_RX_DCD_LPSTS_SUSPENDM                            (1ul<<14)
#endif

/* Define rx command/status bitmaps.  */

#define UX_RX_DCD_DCP                                       0

#define UX_RX_DCD_DCPCTR_DATA1                              (1ul<<7)
#define UX_RX_DCD_DCPCTR_DATA0                              (1ul<<8)

/* Define rx PIPE selection definitions.  */
#define UX_RX_DCD_PIPESEL_NO_PIPE                           0x000f

/* Define rx fifo definition.  */
#define UX_RX_DCD_PIPE0_SIZE                                256
#define UX_RX_DCD_PIPE_NB_BUFFERS                           64

#define UX_RX_DCD_PIPECTR_INBUFM                            (1ul<<14)
#define UX_RX_DCD_PIPECTR_BSTS                              (1ul<<15)
#define UX_RX_DCD_PIPECTR_CSCLR                             (1ul<<13)
#define UX_RX_DCD_PIPECTR_CSSTS                             (1ul<<12)
#define UX_RX_DCD_PIPECTR_ATREPM                            (1ul<<11)
#define UX_RX_DCD_PIPECTR_SQCLR                             (1ul<<8)
#define UX_RX_DCD_PIPECTR_SQSET                             (1ul<<7)
#define UX_RX_DCD_PIPECTR_SQMON                             (1ul<<6)
#define UX_RX_DCD_PIPECTR_PBUSY                             (1ul<<5)
#define UX_RX_DCD_PIPECTR_PID_MASK                          (3)
#define UX_RX_DCD_PIPECTR_PIDNAK                            (0)
#define UX_RX_DCD_PIPECTR_PIDBUF                            (1)
#define UX_RX_DCD_PIPECTR_PIDSTALL                          (2)
#define UX_RX_DCD_PIPECTR_PIDSTALL2                         (3)
#define UX_RX_DCD_PIPECTR_DATA1                             ( 1ul<< 7)
#define UX_RX_DCD_PIPECTR_DATA0                             ( 1ul<< 8)

/* Define rx initialization values.  */
#ifdef UX_RX71M
#ifndef UX_RX_DCD_BUSWAIT_VALUE
#define UX_RX_DCD_BUSWAIT_VALUE                             0x6
#endif
#endif
#define UX_RX_DCD_COMMAND_STATUS_RESET                      0
#define UX_RX_DCD_INIT_RESET_DELAY                          10
#define UX_RX_DCD_MAX_BUF_SIZE                              64
#define UX_RX_DCD_MAX_BUF_NUM                               127

/* Define rx FIFO write completion code.  */
#define UX_RX_DCD_FIFO_WRITING                              2
#define UX_RX_DCD_FIFO_WRITE_END                            3
#define UX_RX_DCD_FIFO_WRITE_SHORT                          4
#define UX_RX_DCD_FIFO_WRITE_DMA                            5
#define UX_RX_DCD_FIFO_WRITE_ERROR                          6

/* Define rx FIFO read completion code.  */
#define UX_RX_DCD_FIFO_READING                              2
#define UX_RX_DCD_FIFO_READ_END                             3
#define UX_RX_DCD_FIFO_READ_SHORT                           4
#define UX_RX_DCD_FIFO_READ_DMA                             5
#define UX_RX_DCD_FIFO_READ_ERROR                           6
#define UX_RX_DCD_FIFO_READ_OVER                            7

/* Define rx physical endpoint definitions.  */
#define UX_RX_DCD_ED_BRDY                                   0x00000001
#define UX_RX_DCD_ED_NRDY                                   0x00000002
#define UX_RX_DCD_ED_BEMP                                   0x00000004
#define UX_RX_DCD_ED_EOFERR                                 0x00000010
#define UX_RX_DCD_ED_SIGN                                   0x00000020
#define UX_RX_DCD_ED_SACK                                   0x00000040

/* Define USB RX physical endpoint status definition.  */

#define UX_DCD_RX_ED_STATUS_UNUSED                          0ul
#define UX_DCD_RX_ED_STATUS_USED                            1ul
#define UX_DCD_RX_ED_STATUS_TRANSFER                        2ul
#define UX_DCD_RX_ED_STATUS_STALLED                         4ul

/* Define USB RX physical endpoint state machine definition.  */

#define UX_DCD_RX_ED_STATE_IDLE                             0ul
#define UX_DCD_RX_ED_STATE_DATA_TX                          1ul
#define UX_DCD_RX_ED_STATE_DATA_RX                          2ul
#define UX_DCD_RX_ED_STATE_STATUS_TX                        3ul
#define UX_DCD_RX_ED_STATE_STATUS_RX                        4ul

/* Define USB RX physical endpoint structure.  */

typedef struct UX_DCD_RX_ED_STRUCT 
{

    ULONG           ux_dcd_rx_ed_status;
    ULONG           ux_dcd_rx_ed_state;
    ULONG           ux_dcd_rx_ed_index;
    ULONG           ux_dcd_rx_ed_fifo_index;
    ULONG           ux_dcd_rx_ed_payload_length;
    ULONG           ux_dcd_rx_ed_packet_length;
    ULONG           ux_dcd_rx_ed_actual_length;
    UCHAR           *ux_dcd_rx_ed_payload_address;
    ULONG           ux_dcd_rx_ed_buffer_number;
    ULONG           ux_dcd_rx_ed_buffer_size;
    struct UX_SLAVE_ENDPOINT_STRUCT             
                    *ux_dcd_rx_ed_endpoint;
} UX_DCD_RX_ED;


/* Define USB RX DCD structure definition.  */

typedef struct UX_DCD_RX_STRUCT
{                                            
    
    struct UX_SLAVE_DCD_STRUCT
                    *ux_dcd_rx_dcd_owner;
    struct UX_DCD_RX_ED_STRUCT                    
                    ux_dcd_rx_ed[UX_DCD_RX_MAX_ED];
    ULONG           ux_dcd_rx_pipe[UX_DCD_RX_MAX_ED];
    ULONG           ux_dcd_rx_base;
    ULONG           ux_dcd_rx_next_available_bufnum;
    ULONG           ux_dcd_rx_debug;
} UX_DCD_RX;


/* Define USB RX DCD prototypes.  */

UINT    _ux_dcd_rx_address_set(UX_DCD_RX *dcd_rx,ULONG address);
UINT    _ux_dcd_rx_endpoint_create(UX_DCD_RX *dcd_rx, UX_SLAVE_ENDPOINT *endpoint);
UINT    _ux_dcd_rx_endpoint_destroy(UX_DCD_RX *dcd_rx, UX_SLAVE_ENDPOINT *endpoint);
UINT    _ux_dcd_rx_endpoint_reset(UX_DCD_RX *dcd_rx, UX_SLAVE_ENDPOINT *endpoint);
UINT    _ux_dcd_rx_endpoint_stall(UX_DCD_RX *dcd_rx, UX_SLAVE_ENDPOINT *endpoint);
UINT    _ux_dcd_rx_endpoint_status(UX_DCD_RX *dcd_rx, ULONG endpoint_index);
UINT    _ux_dcd_rx_frame_number_get(UX_DCD_RX *dcd_rx, ULONG *frame_number);
UINT    _ux_dcd_rx_function(UX_SLAVE_DCD *dcd, UINT function, VOID *parameter);
UINT    _ux_dcd_rx_initialize(ULONG dcd_io);
UINT    _ux_dcd_rx_initialize_complete(VOID);
VOID    _ux_dcd_rx_interrupt_handler(VOID);
VOID    _ux_dcd_rx_register_clear(UX_DCD_RX *dcd_rx, ULONG rx_register, ULONG value);
ULONG   _ux_dcd_rx_register_read(UX_DCD_RX *dcd_rx, ULONG rx_register);
VOID    _ux_dcd_rx_register_set(UX_DCD_RX *dcd_rx, ULONG rx_register, ULONG value);
VOID    _ux_dcd_rx_register_write(UX_DCD_RX *dcd_rx, ULONG rx_register, ULONG value);
UINT    _ux_dcd_rx_state_change(UX_DCD_RX *dcd_rx, ULONG state);
UINT    _ux_dcd_rx_transfer_callback(UX_DCD_RX *dcd_rx, UX_SLAVE_TRANSFER *transfer_request, 
                                        ULONG interrupt_status, ULONG ctsq_mask);
UINT    _ux_dcd_rx_transfer_request(UX_DCD_RX *dcd_rx, UX_SLAVE_TRANSFER *transfer_request);
UINT    _ux_dcd_rx_transfer_abort(UX_DCD_RX *dcd_rx, UX_SLAVE_TRANSFER *transfer_request);

UINT    _ux_dcd_rx_buffer_read(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed);
UINT    _ux_dcd_rx_fifo_read(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed);
VOID    _ux_dcd_rx_buffer_notready_interrupt(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed, ULONG flag);
VOID    _ux_dcd_rx_buffer_empty_interrupt(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed, ULONG flag);
VOID    _ux_dcd_rx_buffer_ready_interrupt(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed, ULONG flag);
ULONG   _ux_dcd_rx_fifo_port_change(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed, ULONG direction);
UINT    _ux_dcd_rx_fifod_write(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed);
UINT    _ux_dcd_rx_fifoc_write(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed);
UINT    _ux_dcd_rx_buffer_write(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed);
ULONG   _ux_dcd_rx_data_buffer_size(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed);
VOID    _ux_dcd_rx_endpoint_nak_set(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed);
VOID    _ux_dcd_rx_current_endpoint_change(UX_DCD_RX *dcd_rx, UX_DCD_RX_ED *ed, ULONG direction);

#define ux_dcd_rx_initialize                      _ux_dcd_rx_initialize
#define ux_dcd_rx_interrupt_handler               _ux_dcd_rx_interrupt_handler

#endif

