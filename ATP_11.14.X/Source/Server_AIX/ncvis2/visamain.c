/*******************************************************************
*
* Copyright (c) 2008, Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      visamain.c
*  
* TITLE:       Main function for the visa network handler      
*  
* DESCRIPTION: This module will handle all incoming messages from
*               the terminal and the host.
*
* APPLICATION: 
*
.
* AUTHOR:      B Gable
*
   REVISION HISTORY
   ----------------
		
	 Rev 1.162   Dec 15 2009   TF-Ajay 
   Changed code to send reversal advice repeat as 0421 if advice reversal gets timeout 
   Updated version 4.4.1.61
   
	 
	  Rev 1.161   Aug  27 2009   TF-Ajay 
   change the code to copy card holder billing currency conversion rate for 0120 sends by the sbatch 
   Updated version 4.4.1.60
   
	 Rev 1.160   Aug  10 2009   TF-Phani 
   ATP should not be doing any PIN TRANSALATION during offline mode. since during STIP,
   ATP will be the one validating the cards.  Directly validates the PIN. 
   Updated version 4.4.1.59
	
	     Rev 1.159   Aug  03 2009   TF-Ajay 
   If the txn originaly originated form DCPISO or voice. no need to send fld 6 and 51. 
   Updated version 4.4.1.58
   
	  Rev 1.158   Jul 23 2009   TF-Ajay 
	CVV2 was seeting null if the txn manually keyed in for STIP
	updated version 4.4.1.57
			
	Rev 1.158   Jul 23 2009   TF-Ajay 
	In some palces handler queue compare with "ncvisa".
	Client make copy of ncvisa as a ncvis3.
	Because of that, the feild 19,32 and 43 was not copy properly 
	Updated version 4.4.1.56


	Rev 1.157   Jul 22 2009   TF-Ajay 
	Updated version 4.4.1.55

	Rev 1.156   Jul 13 2009   TF-Ajay 
	Txn process as a STIP and After process if it gets the reversal
	It should go to host as a 0420 msg
	Updated version 4.4.1.54

	Rev 1.155 Jun 15 2009 Ajay-TF
	Updated version to 4.4.1.53
	Change the code to send 0120 timeout msg as 0420 reversal 

   
	Rev 1.154 Jun 11 2009 Ajay-TF
	Updated version to 4.4.1.52
	Change the code to  copy the filed 104 if 126.13 is I 

	  
	Rev 1.153 May 04 2009 Ajay-TF
	Updated version to 4.4.1.51
	Change the code to not set timer for 0120 msg

	Rev 1.152 Apr 15 2009 Ajay-TF
	Updated version to 4.4.1.50

	Rev 1.151 Apr 17 2009 Ajay   TF
	Updated version to 4.4.1.49

	Rev 1.150 Apr 15 2009 Ajay   TF
	Updated version to 4.4.1.48

	Rev 1.149 Apr 14 2009 Ajay   TF
	Updated version to 4.4.1.47

	Rev 1.148 Mar 31 2009 Girija Y TF
	Updated version to 4.4.1.45

	Rev 1.147 Mar 25, 2009 Girija Y @ThoughtFocus
	Updated version to 4.4.1.44
	Changed  code to support field 95 as Apr09 VISA mandates  
	 
	Rev 1.146 Nov 11, 2008 Girija Y @ThoughtFocus
   Updated version to 4.4.1.42
   Send LONON RESPONSE when host comes ONLINE.(Earlier the event was sent to Txcntl when host
   goes OFFLINE as well).
   Rel 08-11

   Rev 1.145 Aug 17, 2008 Girija Y @ThoughtFocus
   Updated version to 4.4.1.41
   Support Automatic Resend - STIP 
   Rel 08-16

   Rev 1.144 Aug 14, 2008 Girija Y @ThoughtFocus
   Updated version to 4.4.1.40
   Dont insert into TLF01 Table if the transaction is Originating from STIP Manager
   Rel 08-16

   Rev 1.143 July 28, 2008 Ravikumar K N @ThoughtFocus
   Update version to 4.4.1.39 for Support message types
   0120, 0121, 130, 0131 for cadience standin processing
   Rel 08-16
   
   Rev 1.142   Mar 25, 2008  dirby
      Updated version to 4.4.1.34

   Rev 1.141   Mar 12, 2008  dirby
      Updated version to 4.4.1.33
      Merge Visa Compliance into the Visa Cadencie

      Rev 1.139   Jun 14 2006 17:45:20   dirby
   Updated version to 4.4.1.31
   SCR 21323
   
      Rev 1.138   Jun 07 2006 11:30:48   dirby
   Updated version to 4.4.1.30
   SCR 21648
   
      Rev 1.137   Jun 06 2006 13:59:56   dirby
   Updated version to 4.4.1.29
   SCR 21648
   
      Rev 1.136   Jun 01 2006 14:45:20   dirby
   Updated version to 4.4.1.28
   
      Rev 1.135   May 26 2006 08:45:42   dirby
   Updated version to 4.4.1.27
   SCR 20924
   
      Rev 1.134   May 23 2006 16:12:02   dirby
   Updated version to 4.4.1.26
   SCR 20158
   
      Rev 1.133   May 04 2006 16:34:28   dirby
   Updated version to 4.4.1.25
   SCR 21042
   
      Rev 1.132   Apr 24 2006 10:18:44   dirby
   Updated version to 4.4.1.24
   SCr 20917
   
      Rev 1.131   Apr 17 2006 17:10:28   dirby
   Updated version to 4.4.1.23
   SCR 20815
   
      Rev 1.130   Apr 14 2006 07:36:24   dirby
   Updated version to 4.4.1.22
   SCR 20815
   
      Rev 1.129   Apr 06 2006 15:45:48   dirby
   Updated version to 4.4.1.21
   SCR 20719
   
      Rev 1.128   Apr 04 2006 16:54:00   dirby
   Updated version to 4.4.1.20
   SCR 20160
   
      Rev 1.127   Mar 29 2006 11:28:22   dirby
   Updated version to 4.4.1.19
   SCR 20591
   
      Rev 1.126   Mar 09 2006 10:29:28   dirby
   Updated version to 4.4.1.18
   SCR 19670
   
      Rev 1.125   Feb 27 2006 11:12:22   dirby
   Changed some of the Host States used by Visa for Issuer and
   Acquirer states to use the general on/off states used by the other
   network controllers.  The reason is that Monitor does not know how
   to handle the Issuer/Acquirer states, so it was not always displaying
   the correct host state.
   SCR 19665
   
      Rev 1.124   Feb 24 2006 16:26:02   dirby
   Updated version to 4.4.1.17
   SCR 19670
   
      Rev 1.123   Feb 17 2006 09:00:00   dirby
   Updated version to 4.4.1.16
   SCR 19844
   
      Rev 1.122   Feb 07 2006 01:15:22   dirby
   Corrected a buffer overflow problem with field 48 from Visa
   into resp_source when txn is not an 0312.
   SCR 19664
   
      Rev 1.121   Jan 04 2006 17:52:50   lmitchel
   If fld91 = 3 or 5, do not send field 73 to visa
   
      Rev 1.120   Dec 20 2005 14:33:08   dirby
   Added Visa Online File Maintenance handling.  These are
   0302/0312 transactions.
   SCR 18962
   
      Rev 1.119   Dec 14 2005 12:53:34   dirby
   Updated version to 4.4.1.13
   SCR 12418
   
      Rev 1.118   Nov 18 2005 13:03:12   dirby
   Updated version to 4.4.1.12
   SCR 17346
   
      Rev 1.117   Aug 30 2005 10:25:32   dirby
   Updated version to 4.4.1.11
   SCR 16625
   
      Rev 1.116   Aug 26 2005 15:02:28   dirby
   Updated version to 4.4.1.10
   SCR 16693
   
      Rev 1.115   Jul 07 2005 16:29:08   dirby
   Updated version to 4.4.1.9
   SCR 14421
   
      Rev 1.114   Jun 17 2005 14:14:54   dirby
   Updated version to 4.4.1.8
   SCR 16053
   
      Rev 1.113   Jun 09 2005 18:17:58   dirby
   Updated version to 4.4.1.7
   SCR 15799
   
      Rev 1.112   May 17 2005 17:22:10   dirby
   Updated version to 4.4.1.6
   SCR 15420
   
      Rev 1.111   May 11 2005 18:23:02   dirby
   Updated version to 4.4.1.5
   SCR 15019
   
      Rev 1.110   May 06 2005 14:26:38   dirby
   Updated version to 4.4.1.4
   SCR 15019
   
      Rev 1.109   May 05 2005 15:54:22   dirby
   Updated version to 4.4.1.3
   SCR 15019
   
      Rev 1.108   Apr 27 2005 17:31:20   dirby
   Updated version to 4.4.1.2
   SCR 15019
   
      Rev 1.107   Apr 05 2005 15:19:12   dirby
   Updated version to 4.4.1.1
   SCR 12785
   
      Rev 1.106   Mar 17 2005 08:39:14   dirby
   Updated version to 4.4.0.19
   SCR 14238
   
      Rev 1.105   Mar 14 2005 16:45:14   dirby
   Updated version to 4.4.0.18
   SCR 14501
   
      Rev 1.104   Mar 10 2005 17:16:42   dirby
   Updated version to 4.4.0.17
   SCRs 14238 and 14648
   
      Rev 1.103   Mar 02 2005 16:34:16   dirby
   Updated version to 4.4.0.16
   SCR 14460
   
      Rev 1.102   Feb 23 2005 16:04:16   dirby
   Updated version to 4.4.0.15
   SCR 14238
   
      Rev 1.101   Feb 09 2005 16:42:04   dirby
   Updated version to 4.4.0.14
   SCR 13223
   
      Rev 1.100   Jan 12 2005 11:21:40   dirby
   Updated version to 4.4.0.13
   SCR 13513
   
      Rev 1.99   Jan 10 2005 11:22:48   dirby
   Updated version to 4.4.0.12
   SCR 12475
   
      Rev 1.98   Dec 16 2004 17:46:10   dirby
   Updated version to 4.4.0.11
   SCR 13182
   
      Rev 1.97   Dec 10 2004 14:46:54   dirby
   Updated version to 4.4.0.10
   SCR 13182
   
      Rev 1.96   Nov 19 2004 15:13:52   dirby
   Updated version to 4.4.0.9
   SCR 13183
   
      Rev 1.95   Nov 12 2004 17:22:44   dirby
   Updated version to 4.4.0.8
   SCR 12483
   
      Rev 1.94   Nov 05 2004 13:09:44   dirby
   Updated version to 4.4.0.7
   SCR 12724
   
      Rev 1.93   Oct 20 2004 17:50:32   dirby
   Updated version to 4.4.0.6
   SCR 12481
   
      Rev 1.92   Oct 19 2004 09:16:54   dirby
   Updated version to 4.4.0.5
   SCR 12724
   
      Rev 1.91   Oct 18 2004 10:30:52   dirby
   Updated version to 4.4.0.4
   SCR 12724
   
      Rev 1.90   Oct 12 2004 15:24:20   dirby
   Updated version to 4.4.0.3
   SCR 12755
   
      Rev 1.89   Aug 31 2004 17:34:34   dirby
   Updated version to 4.4.0.2
   SCR 1475
   
      Rev 1.88   Jul 23 2004 17:24:32   dirby
   Added support for EMV.
   SCR 1388
   
      Rev 1.87   Jul 08 2004 17:44:50   dirby
   Updated to 4.4.0.1
   SCRs 1287 & 1388
   
      Rev 1.86   May 27 2004 18:01:28   dirby
   Updated version to 4.3.0.1
   SCR 1380
   
      Rev 1.85   Apr 29 2004 15:32:48   dirby
   Updated version to 4.2.0.12
   SCR 1382
   
      Rev 1.84   Apr 22 2004 13:07:58   dirby
   Updated version to 4.2.0.11
   SCR 1117
   
      Rev 1.83   Mar 31 2004 15:24:46   dirby
   Updated version to 4.2.0.10
   SCR 1329
   
      Rev 1.82   Mar 05 2004 11:28:52   dirby
   Updated version to 4.2.0.9
   SCR 1359
   
      Rev 1.81   Feb 26 2004 15:06:54   dirby
   Updated version to 4.2.0.8
   SCR 1117
   
      Rev 1.80   Jan 13 2004 15:43:50   dirby
   Updated version to 4.2.0.7
   SCR 1117
   
      Rev 1.79   Dec 16 2003 13:54:50   dirby
   Updated version to 4.2.0.6
   SCR 1117
   
      Rev 1.78   Dec 10 2003 09:42:16   dirby
   Updated version to 4.2.0.5
   SCR 1117
   
      Rev 1.77   Aug 26 2003 14:01:12   mbysani
   updated as per VbV spec
   
      Rev 1.76   Apr 24 2003 13:57:30   dirby
   Modified logon from monitor to set issuer online without sending
   a request to host.  Acquirer will still send request to host.
   SCR 1065
   
      Rev 1.75   Apr 22 2003 18:46:18   dirby
   Modified to distinguish between Issuer and Acquirer for Log
   ON/OFF/Echoes.    Also modified to use newly defined host
   states; these states (defined in app_info.h) give the status of
   both Issuer and Acquirer states, using a single value.
   Acquirer Online = ATP is ready to receive requests from host.
   Acquirer Offline = Host needs to stand in for us.
   Issuer Online = ATP is ready to send requests to the host.
   Issuer Offline = No requests will be sent to the host.
   Echoes use the Issuer status.
   
      Rev 1.74   Mar 25 2003 12:37:04   dirby
   Updated version to 4.2.0.3
   SCR 1074
   
      Rev 1.73   Mar 13 2003 15:13:08   dirby
   Adding code to handle reject messages from Visa.  If a response
   is rejected, get the original txn from TLF01 and reverse out the
   transaction.  If a request to Visa is rejected, treat it as if Visa
   declined it with a format error response code.
   SCR 1034
   
      Rev 1.72   Feb 19 2003 17:07:14   dirby
   Updated version to 4.2.0.1
   SCR 955
   
      Rev 1.71   Oct 16 2002 11:29:26   dirby
   Updated version to 4.1.1.3
   SCR 820
   
      Rev 1.70   Sep 16 2002 15:14:08   dirby
   Updated version to 4.1.1.2
   SCR 879
   
      Rev 1.69   Aug 22 2002 12:53:06   dirby
   Updated version to 4.1.1.1
   SCR 255
   
      Rev 1.68   Aug 02 2002 15:51:28   dirby
   Updated version to 4.0.3.4
   SCR 594
   
      Rev 1.67   Jul 25 2002 17:23:02   dirby
   Updated version to 4.0.3.3
   SCR 594
   
      Rev 1.66   Jul 07 2002 00:52:32   dirby
   1.  Modified to use precalculated 'sizeof' values instead of recalculating
   structure sizes over and over again.
   2.  Updated version to 4.0.3.2
   
      Rev 1.65   Jul 05 2002 13:31:04   dirby
   Updated version to 4.0.3.1
   SCRs 823 and 621
   
      Rev 1.64   Jun 24 2002 10:50:22   dirby
   Updated version to 4.0.2.21
   SCR 790
   
      Rev 1.63   Jun 21 2002 15:42:24   dirby
   Updated version to 4.0.2.20
   SCR 793
   
      Rev 1.62   Jun 20 2002 15:51:26   dirby
   Updated version to 4.0.2.19
   SCR 794
   
      Rev 1.61   Jun 18 2002 00:07:24   dirby
   Updated version to 4.0.2.18
   
      Rev 1.60   Jun 17 2002 09:50:54   dirby
   Updated version to 4.0.2.17
   SCR 786
   
      Rev 1.59   Jun 13 2002 14:29:58   dirby
   Updated version to 4.0.2.16
   SCR 775
   
      Rev 1.58   Jun 10 2002 15:21:28   dirby
   Updated version to 4.0.2.15
   SCR 770
   
      Rev 1.57   Jun 05 2002 15:54:08   dirby
   Updated version to 4.0.2.14
   SCRs 621, 763, 764, and 765
   
      Rev 1.56   Jun 03 2002 00:27:52   dirby
   Updated version to 4.0.2.13
   SCR 759
   
      Rev 1.55   Jun 02 2002 23:41:30   dirby
   Updated version to 4.0.2.12
   SCR 758
   
      Rev 1.54   May 31 2002 14:05:14   dirby
   Updated version to 4.0.2.11
   SCRs 754 & 755
   
      Rev 1.53   May 30 2002 09:42:34   dirby
   Updated version to 4.0.2.10
   SCR 751
   
      Rev 1.52   May 29 2002 18:42:54   dirby
   Updated version to 4.0.2.9
   SCR 748
   
      Rev 1.51   May 29 2002 13:10:30   dirby
   Updated version to 4.0.2.8
   SCR 748
   
      Rev 1.50   May 24 2002 16:02:30   dirby
   Updated version to 4.0.2.7
   SCRs 745 & 746
   
      Rev 1.49   May 23 2002 16:27:00   dirby
   Updated version to 4.0.2.6
   SCRs 743 & 744
   
      Rev 1.48   May 15 2002 12:01:16   dirby
   Updated version to 4.0.2.5.
   Added code to display version when application is pinged.
   SCR 725
   
      Rev 1.47   Apr 24 2002 11:11:38   dirby
   Removed a useless error message.
   SCR 703
   
      Rev 1.46   Apr 19 2002 15:55:52   dirby
   Updated version to 4.0.2.4
   SCR 702
   
      Rev 1.45   Apr 12 2002 22:11:20   dirby
   Removed some local variables that were not being referenced
   any more.
   SCR - 699
   
      Rev 1.44   Mar 15 2002 15:19:48   dirby
   Modified the code for responses to switched out requests from
   reading the timer then clearing it, to just clearing it.  The read
   has been eliminated, saving an XIPC hop and processing time.
   A read timer reads the timer and sends the data back.  A clear
   timer command reads the timer, sends the data back and 
   clears the timer.
   SCR - There is no SCR associated with this.  It is product improvement.
   
      Rev 1.43   Mar 15 2002 14:28:02   dirby
   Updated version to 4.0.2.3
   SCR 618
   
      Rev 1.42   Mar 05 2002 15:04:16   dirby
   Added startup value to set the MCC validation either on or off.
   Updated version to 4.0.2.2
   SCR 648
   
      Rev 1.41   Feb 01 2002 10:53:00   dirby
   Updated version to 4.0.2.1
   SCR 609
   
      Rev 1.40   Jan 16 2002 16:33:52   dirby
   Modified startup to open trace file prior to issuing an auto logon.
   SCR 546
   
      Rev 1.39   Jan 16 2002 15:37:46   dirby
   Updated version to 4.0.1.1
   
      Rev 1.38   Jan 16 2002 15:32:52   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.37   Jan 14 2002 18:22:08   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.36   Jan 07 2002 13:02:10   dirby
   Updated to version 4.0.0.2
   SCR 569 - Send automatic reversal on time outs
   
      Rev 1.35   Dec 12 2001 11:35:14   dirby
   Modified to send host state to Monitor during start up.
   SCR 488
   
      Rev 1.34   Dec 10 2001 15:22:40   dirby
   Modified to always forward requests to host.
   Modified to ignore max active and max timeout counts if configured to zero, else put out a warning if reached.
   Modified to set state to DOWN when shutting down.
   SCR 488
   
      Rev 1.33   Nov 03 2001 14:20:30   dirby
   Updated version to 4.0.0.1
   
      Rev 1.32   Oct 23 2001 12:58:36   dirby
   Updated version to 3.2.3.5     SCR 509
   
      Rev 1.31   Oct 11 2001 16:07:32   dirby
   Updated version to 3.2.3.4    SCR 496
   
      Rev 1.30   Sep 27 2001 09:45:32   dirby
   Updated version to 3.2.3.3  This is for the change in visautil.c,
   revision 1.138.  SCR 447
   
      Rev 1.29   Aug 03 2001 15:00:12   dirby
   Updated version to 3.2.3.2
   Added some balance inquiry code to 'alter_transaction_data'.
   This is for the sake of services who get this info from ACF01.
   
      Rev 1.28   Jun 13 2001 14:45:44   dirby
   Updated version to 3.2.3.1
   
      Rev 1.27   May 21 2001 13:23:06   dirby
   Updated version to 3.2.2.10
   
      Rev 1.26   Apr 19 2001 13:13:08   SYARLAGA
   updated to version 3.2.2.9
   
      Rev 1.25   Mar 13 2001 13:17:20   dirby
   Updated version to 3.2.2.8
   
      Rev 1.24   Jan 31 2001 09:29:48   dirby
   Updated version to 3.2.2.7
   
      Rev 1.23   Jan 09 2001 13:45:56   dirby
    
   
      Rev 1.22   Jan 08 2001 13:12:24   dirby
   Updated version to 3.2.2.5
   
      Rev 1.21   Oct 18 2000 15:19:48   dirby
   Updated version number to 3.2.2.4
   
      Rev 1.20   Sep 19 2000 16:33:42   dirby
   Updated the version number to 3.2.2.3
   
      Rev 1.19   Aug 30 2000 15:42:24   dirby
   Updated version number to 3.2.2.2
   
      Rev 1.18   Aug 21 2000 16:08:46   dirby
   Added code to support transaction statistics for Monitor.
   
      Rev 1.17   Aug 09 2000 16:10:22   dirby
   Updated version number to 3.2.2.1
   
      Rev 1.16   Jul 30 2000 14:43:06   dirby
   Reworked the Log ON/OFF logic to show accurate status
   on Monitor, and to be a little more efficient.
   
      Rev 1.15   Jul 21 2000 08:59:14   dirby
   Made version number global so it can be printed
   at shut down, as well as startup.
   
      Rev 1.14   Jun 25 2000 02:00:44   dirby
   Removed the functionality that gets NCF01 every request and response.
   It gets NCF01 only at start up now, never during an online transaction.
   
      Rev 1.13   Jan 24 2000 10:36:12   ddabberu
   new echo_logic 
   
      Rev 1.12   Dec 20 1999 15:16:06   ddabberu
   Integration Testested  and the following functions were modified. 
   insertTLf01(), reconUpdate(),
   
   
      Rev 1.11   Nov 16 1999 15:29:04   dirby
   Updated to:
   1.  Remove negative file processing,
   2.  Incorporate a new scheme for handling reversals,
   3.  Use the UpdateDS Data Server.
   
      Rev 1.10   Aug 10 1999 10:29:52   egable
   Remove an unneeded function from the
   the timer reply path.
   
      Rev 1.9   Jul 13 1999 15:56:32   egable
   Verify that appname is a valid length; Output
   messages to syslog and monitor.
   
      Rev 1.8   May 25 1999 16:11:20   egable
   Put the previous source station flag into
   shared memory to allow alternating source
   station ids.
   
      Rev 1.7   19 Apr 1999 10:19:30   dirby
   The result of a Pin Translation printed an error message if it was
   successful.  Changed to display it for pin translation failure.
   
      Rev 1.6   Mar 31 1999 15:37:56   egable
   Update the monitor to show offline when 
   shut down.  Change the response to monitor 
   for status query.
   
   
      Rev 1.5   Mar 30 1999 15:55:18   egable
   Add error processing.
   
      Rev 1.4   Mar 29 1999 16:49:28   egable
   Correct interface to security.
   
      Rev 1.3   Feb 23 1999 09:25:06   egable
   Wait for a reply from the start timer before
   sending message to host.
   
      Rev 1.2   Feb 02 1999 15:59:04   egable
   Changes to message flow, headers and
   'if' statements.  This is the basis for JCB.
   
      Rev 1.1   Jan 11 1999 09:56:48   egable
   Initial changes for equitable, added  issuing
   paths and several message types.
   
      Rev 1.0   17 Dec 1998 16:25:16   jhapitas
   Initial Equitable Revison
   
      Rev 1.0   Dec 04 1998 14:56:30   MSALEH
   initial release
*
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptestats.h"
#include "ptesystm.h"
#include "ntutils.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "visamain.h"
#include "visautil.h"
#include "visatran.h"
#include "memmnger.h"
#include "txutils.h"
#include "diskio.h"
#include "txtrace.h"


extern CHAR          ServiceName[12];
extern INT  volatile EndProcessSignalled;
extern INT  volatile MainProcessDone;

extern CHAR  LOGON_REQUEST[];    
extern CHAR  LOGOFF_REQUEST[];  
extern CHAR  ECHO_REQUEST[]; 

extern CHAR        applnk_que_name[];
extern INT rvrsl_attmpts_value_fr_laterspns_visa;
extern INT rvrsl_attmpts_value_fr_timeout_visa;
WORD  echo_timer_length = 0;


/************************************************
    GLOBAL DECLARATIONS
 ************************************************/

NCF01    ncf01_i;
NCF01    ncf01_a;
INT      Tran_Flow;

CHAR  ncvis2_Error_warning_Filename[256] = {0};
CHAR  ncvis2_module_error_warning_file_name[256] = {0};
CHAR  ncvis2_error_warning_file_path[256] = {0};
CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */

CHAR  cur_app_name    [MAX_APP_NAME_SIZE];
CHAR  oprmon_que_name []           = "oprmonI" ;
CHAR  dialog_que_name []           = "dialogA" ;
CHAR  authorizer_que_name[]        = "txcntlA";
CHAR  updateds_que_name[]          = "updatdsA";
CHAR  SHARED_MEMORY_TBL[16];
CHAR  AppName[8] = "";

/* Database structure sizes */
INT    Tlf01_Size;
INT    Ncf01_Size;
INT    Ncf30_Size;
INT    Auth_Tx_Size;

/* Statistics for Monitor */
TXN_COUNTS     Txn_Counts;
MONITOR_STATS  Monitor_Stats;

/* Counters */
INT    MaxActiveTxns;
INT    MaxConsecTimeouts;

/* Txn Tracing Info */
CHAR   DirPath[MAX_APP_NAME_SIZE];
CHAR   TraceFile[MAX_APP_NAME_SIZE];
INT    Tracing;
FILE   TraceFile_fp;
FILE   *pTraceFile_fp;
FILE   **ppTraceFile_fp;

/* Miscellaneous */
INT    MCC_Validation_Functionality;
INT    Txn_Is_A_Reject;
INT    Third_Bitmap;

CHAR  Version[] = "ATP_11.13.0";

/*****************************************************************************

  Function:    PinnacleMsgHandler  

  Description: This function will bring in a PTE message and determine where
               it should be routed.
  Author: 
      unknown
  Inputs:
      p_msg_in - the incoming PTE message
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void PinnacleMsgHandler( pPTE_MSG p_msg_in )
{
   pPTE_MSG_DATA  p_msg_data = NULL_PTR;
   pPTE_MSG       p_msg_ins  = NULL_PTR;
   pPTE_MSG       p_cur_msg;
   pPTE_MSG       p_msg_out;
   pBYTE          p_data_data = NULL_PTR;
   pBYTE          pCurrent;
   BYTE           subtype1;
   BYTE           subtype2;
   BYTE           net_consec_tmouts_ctr[4];
   BYTE           active_txns_ctr[4];
   BYTE           current_state[2];
   BYTE           prev_src_station[4];
   BYTE           timer_result;
   CHAR           err_mesg[512] = {0};
   CHAR           Buff[512]={0};
   CHAR           ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   LONG           ret_code;
   XINT           ReturnCode = 0 ;
   INT            nIndex = 0 ;
   INT            ret_val;
   AUTH_TX        auth_tx;
   NCF01          ncf01;


   p_cur_msg = p_msg_in;
   p_msg_out = NULL_PTR;

   ptestats_accumulate_msg_stats(p_msg_in);

   Third_Bitmap = false;

   switch( ptemsg_get_msg_type(p_cur_msg) )
   {
      case MT_AUTH_REQUEST: 

          /* Request coming in via the authorizer handler. */
          if (false == process_incoming_message(p_msg_in)) 
          {
             ncvis2_log_message( 3, 2, "Error processing incoming terminal request" , "PinnacleMsgHandler" , 0 ,"");
             pCurrent = ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_msg_in));
             memset( &auth_tx, 0,        Auth_Tx_Size );
             memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
             perform_error_processing( &auth_tx );
             send_it_back_to_terminal("58", &auth_tx); 
          }
          break;

      case MT_AUTH_RESPONSE: 

          /* Response coming in via the authorizer */
          if (false == process_incoming_message(p_msg_in)) 
          {
             ncvis2_log_message( 3, 2, "Error processing incoming response for host" , "PinnacleMsgHandler" , 0 ,"");
             pCurrent = ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_msg_in));
             memset( &auth_tx, 0,        Auth_Tx_Size );
             memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
             perform_error_processing( &auth_tx );
             strcpy(auth_tx.TLF01_details.response_code,"06");
             auth_tx.TLF01_details.tx_key = match_request_to_response(auth_tx.TLF01_details.tx_key);
             incoming_other_host_response(&auth_tx);
          }
          break;

      case MT_INCOMING: 

          /* Reply from the external host */
          (void)incoming_host_message( p_msg_in );
          break;

      case MT_TIMER_TIMEOUT: 
          /* Reply about timeout of ptemsg */
          if (false == process_timeout_msg(p_msg_in))
          {
             ncvis2_log_message( 3, 2, "Error processing timeout message", "PinnacleMsgHandler" , 0 ,"");
          }
          break;

      case MT_SYS_MONITOR:

         subtype1 = ptemsg_get_msg_subtype1( p_cur_msg );
         subtype2 = ptemsg_get_msg_subtype2( p_cur_msg );

         switch( subtype1 )
         {
            case ST1_LOGON:

               if ( subtype2 == ST2_ISSUER )
               {
                  /* Do not send logoff to host, else they would stand in. */
                  ret_val = set_host_state( vSTAY, vONLINE, "0", "0" );
                  if ( ret_val == false )
                  {
                     ncvis2_log_message( 3, 2, "Unable to change host state", "PinnacleMsgHandler" , 0 ,"");
                  }
                  else
                     (void)send_network_status_to_monitor();
               }
               else
               {
                  memset (&auth_tx, 0, Auth_Tx_Size);
                  (void)perform_host_logon_request( LOGON_REQUEST,
                                                    SOURCE_IS_OPERATOR,
                                                    &auth_tx,
                                                    subtype2 );
               }
               break;

            case ST1_LOGOFF:

               if ( subtype2 == ST2_ISSUER )
               {
                  /* Do not send logoff to host, else they would stand in. */
                  ret_val = set_host_state( vSTAY, vOFFLINE, "0", "0" );
                  if ( ret_val == false )
                  {
                     ncvis2_log_message( 3, 3, "Unable to change host state", "PinnacleMsgHandler" , 0 ,"");
                  }
                  else
                     (void)send_network_status_to_monitor();
               }
               else
               {
                  memset (&auth_tx, 0, Auth_Tx_Size);
                  (void)perform_host_logon_request( LOGOFF_REQUEST,
                                                    SOURCE_IS_OPERATOR,
                                                    &auth_tx,
                                                    subtype2 );
               }
               break;

            case ST1_ECHOTEST:

               memset (&auth_tx, 0, Auth_Tx_Size);
               (void)perform_host_logon_request( ECHO_REQUEST,
                                                 SOURCE_IS_OPERATOR,
                                                 &auth_tx,
                                                 subtype2 );
               break;

            case ST1_NETWORK_QUERY:

               ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                              active_txns_ctr, current_state, prev_src_station );
               pCurrent = ptemsg_get_pte_msg_data_data(ptemsg_get_pte_msg_data(p_msg_in));
               memset( &ncf01, 0,        Ncf01_Size );
               memcpy( &ncf01, pCurrent, Ncf01_Size );
               strcpy( ncf01.status,     current_state );

               p_msg_ins = ptemsg_build_msg( MT_SYS_MONITOR,
                                             ST1_NETWORK_UPDATE, 
                                             ST2_NONE,
                                             applnk_que_name, 
                                             application_que_name,
                                             (pBYTE)&ncf01,
                                             Ncf01_Size, 
                                             0 );

               memset (ConnectionId, 0, sizeof (ConnectionId)) ;
               ReturnCode = ReadMemMonitorTable (ConnectionId) ;
               if (ConnectionId[0] != 0)
               {
                  ptemsg_set_orig_comms_info   (p_msg_ins, ConnectionId) ;
                  ret_code = pteipc_send( p_msg_ins, applnk_que_name );
                  if ( 0 > ret_code )
                  {
                     pteipc_get_errormsg( ret_code, err_mesg );
                     sprintf( Buff,
                             "Failed transmit to que: %s. %s",
                              applnk_que_name,err_mesg );
                     ncvis2_log_message( 3, 3, Buff, "PinnacleMsgHandler" , 0 ,"");
                     free( p_msg_ins );
                  }
               }
               free( p_msg_ins );
               break;
         }
         break;

      case MT_SYSTEM_REQUEST: 

         ptesystm_system_msg_handler(p_msg_in);
		 process_encryption_flag( p_msg_in );
         if ( ST1_SYS_PING == ptemsg_get_msg_subtype1(p_msg_in) )
         {
            if ( Tracing == TRACE_ON )
            {
               /* Flush the trace file buffer when service is pinged. */
               if ( false == flush_file(ppTraceFile_fp) )
               {
                  Tracing = TRACE_OFF;
                  sprintf( Buff,
                    "%s: Unable to flush trace file buffer. Turning Trace off.",
                     ServiceName );
                  ncvis2_log_message( 3, 1, Buff, "PinnacleMsgHandler" , 0 ,"");
                  (void)close_trace_file( ppTraceFile_fp );
               }
            }

            /* When pinged, display application version on Monitor. */
            sprintf( Buff,
                    "Pinged -> Network Controller Service: %s, version %s",
                     ServiceName, Version );
            ncvis2_log_message( 3, 1, Buff, "PinnacleMsgHandler" , 0 ,"");
         }
         else if(ST1_LOAD_TO_REVERSAL_ATTEMPTS == ptemsg_get_msg_subtype1(p_msg_in) ||
        		 ST1_LOAD_LATE_RESP_REVERSAL_ATTEMPTS == ptemsg_get_msg_subtype1(p_msg_in))
         {
        	 ncvis2_read_TO_config_details_tf_ini();
        	 send_trace_response(p_msg_in);
        	 sprintf( Buff,
        	          "rvrsl_attmpts_value_fr_timeout_visa: %d, rvrsl_attmpts_value_fr_laterspns_visa %d",
					  rvrsl_attmpts_value_fr_timeout_visa, rvrsl_attmpts_value_fr_laterspns_visa );
        	 ncvis2_log_message( 1, 1, Buff, "PingMsgHandler" ,0,"");
         }
         break;

      case MT_SYSTEM_REPLY: 
         PRINT("MT_SYSTEM_REPLY  \n" );
         break;

      case MT_DB_REPLY:
            if (PTEMSG_OK == ptemsg_get_result_code(p_msg_in))
            {
               hhutil_parse_db_ok(p_msg_in);
            }
            else
            {
               hhutil_parse_db_error(p_msg_in);
            }
          break;

      case MT_TIMER_REPLY:
         if (PTEMSG_OK == ptemsg_get_result_code(p_msg_in))
         {
            subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
            if (ST1_TIMER_CLEAR == subtype1)
            {
               (void)incoming_host_message_timer_reply(p_msg_in);
            }
         }
         else
         {
            p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
            p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
            memcpy( &auth_tx, p_data_data, Auth_Tx_Size );
            timer_result = ptemsg_get_result_code(p_msg_in);

            memset( err_mesg, 0x00, sizeof(err_mesg) );
            sprintf( err_mesg,
                    "Error from Timerds, result code = %c, stan = %s",
                     timer_result, auth_tx.TLF01_details.sys_trace_audit_num );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            strcat(err_mesg,Log_Bufr);
            ncvis2_log_message( 3, 2, err_mesg , "PinnacleMsgHandler" , 0 ,auth_tx.TLF01_details.card_num);
         }
         break;

      case MT_NSP_TRANSLATE_PIN_RESPONSE:

         /* Response from a translate key */
         p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
         p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);

         memset( &auth_tx, 0x00,     Auth_Tx_Size );
         memcpy( &auth_tx, p_data_data, Auth_Tx_Size );

         if ( false == process_incoming_message_continued( &auth_tx ) )  
         {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Error processing incoming security request Term Id:%s ,Merch Id: %s RRN: %s ",
            		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            ncvis2_log_message( 3, 2, Log_Bufr , "PinnacleMsgHandler" , 0 ,auth_tx.TLF01_details.card_num);
         }
      break;

      case MT_GET_STATS:
         (void)send_transaction_statistics( p_msg_in );
         break;

      case MT_RESET_STATS:
         (void)reset_transaction_statistics( p_msg_in );
         break;

      case MT_INCOMING_GUI:
         /* Dcpiso is forwarding an 0302 message for Visa (Online File Update)*/
	  /* OR ncvisa is sending response for a 0302 message which came from Cadencie earlier. */
         (void)process_0302_request( p_msg_in );
         break;

      default :
         ncvis2_log_message( 3, 2, "Unknown message type received" , "PinnacleMsgHandler" , 0 ,"");
         break;
	}  /* switch msg_type */

} /* PinnacleMsgHandler */

/*****************************************************************************

  Function:    MainProcessor

  Description: This function will start up and shutdown ncvisa, as well as
               check for and bring in new PTE messages.
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void MainProcessor()
{
   pPTE_MSG p_msg;
   LONG     ret_code;
   INT      rc;
   CHAR     ErrorMsg[512] = {0};
   CHAR     Buffer[512]	= {0};
   CHAR     err_msg[512] = {0};
   BYTE     net_consec_tmouts_ctr[4];
   BYTE     active_txns_ctr[4];
   BYTE     current_state[2];
   BYTE     prev_src_station[4];
   AUTH_TX  auth_tx;

   GetAppName( AppName );

   GetAppName (cur_app_name);
   GetXipcInstanceName( Buffer );

   sprintf( Buffer,
           "Starting the Network Controller Service: %s, version %s",
            ServiceName, Version );
   ncvis2_get_error_warning_file_name_path();
   ncvis2_log_message( 2, 1, Buffer , "MainProcessor" , 0 ,"");
   strcat( Buffer, "\n" );
   PRINT( Buffer );

#ifdef _DEBUG
	 if( !pteipc_init_single_instance_app( AppName, "pte_ipc" ) )
#else
	if( !pteipc_init_multiple_instance_app( cur_app_name, ServiceName, Buffer) )
#endif
	{
      ncvis2_log_message( 3, 3, "Failed to create XIPC queues" , "MainProcessor" , 0 ,"");
	
#ifdef _DEBUG
	 pteipc_shutdown_single_instance_app(); 
#else
	pteipc_shutdown_multiple_instance_app();
#endif
		MainProcessDone = 1;
		return;
	}
   ncvis2_log_message( 3, 1, "Started the Visa Host Handler Service" , "MainProcessor" , 0 ,"");

   if (strlen(AppName) > (sizeof(ncf01_i.primary_key.network_id)-2) )
	{
		sprintf(Buffer, "Error - the name of the application must be %d characters or less", (sizeof(ncf01_i.primary_key.network_id)-2));
		ncvis2_log_message( 3, 2, Buffer, "MainProcessor" , 0 ,"");
#ifdef _DEBUG
		pteipc_shutdown_single_instance_app();
#else
		pteipc_shutdown_multiple_instance_app();
#endif
		MainProcessDone = 1;
		return;
	}

   /* Calculate Data Structure sizes to save time during processing. */
   Tlf01_Size   = sizeof(TLF01);
   Ncf01_Size   = sizeof(NCF01);
   Ncf30_Size   = sizeof(NCF30);
   Auth_Tx_Size = sizeof(AUTH_TX);

   /*  Create a table to be used for shared memory between multiple
       instances of this Network Control Module.  It contains three 
       fields:  # of consecutive timerouts, # of active transactions, 
       and the current state of the host.
   */
   memset(SHARED_MEMORY_TBL, 0, sizeof(SHARED_MEMORY_TBL));
   strcpy(SHARED_MEMORY_TBL,cur_app_name);
   strcat(SHARED_MEMORY_TBL,"Table");
   ret_code = create_generic_table( SHARED_MEMORY_TBL);
   if ( MEMACCESSERROR == ret_code )
   {
      /* Table already exists. */
      PRINT ("Shared memory table already exists. \n");
   }
   else if ( 0 == ret_code )
   {
      /* Table was successfully created.  Initialize host counters and state. */
      ret_code = WriteVisaTable(SHARED_MEMORY_TBL, "0", "0", BOTH_OFFLINE, "1");
      if ( 0 != ret_code)
      {
         /* Error creating the table.  It must already exist. */
         ncvis2_log_message( 3, 2, "Unable to write to shared memory", "MainProcessor" , 0 ,"");
      }
   }
   else
   {
         /* Error creating the table.   */
         ncvis2_log_message( 3, 2,"Unable to create to shared memory", "MainProcessor" , 0 ,"");
   }
  
   /* Initialize transaction statistics */
   (void)reset_stats( &Txn_Counts );

   /* Determine if transaction tracing should be turned on. */
   rc = get_trace_ini_info( AppName, DirPath, ErrorMsg );
   if ( rc == 0 )
   {
      Tracing = TRACE_ON;

      /* Open the transaction trace file. */
      memset( TraceFile, 0x00, sizeof(TraceFile) );

      pTraceFile_fp  = &TraceFile_fp;
      ppTraceFile_fp = &pTraceFile_fp;

      if (false == open_trace_file(AppName,DirPath,ppTraceFile_fp,TraceFile))
      {
         Tracing = TRACE_OFF;
         sprintf( ErrorMsg,
                 "Unable to open trace file %s. Tracing is off",
                  TraceFile );
         ncvis2_log_message( 3,1,ErrorMsg, "MainProcessor" , 0 ,"");
      }
      else
      {
    	  ncvis2_log_message( 3,1,"Tracing is turned ON", "MainProcessor" , 0 ,"");
      }
   }
   else
   {
      Tracing = TRACE_OFF;
      if ( rc == -1 )
      {
         ncvis2_log_message( 3,1,ErrorMsg, "MainProcessor" , 0 ,"");
      }
      ncvis2_log_message( 3,1,"Tracing is turned off", "MainProcessor" , 0 ,"");

   }

   /* Get Network's Issuing record from NCF01. */
   memset(&ncf01_i, 0, Ncf01_Size);
   strcpy(ncf01_i.primary_key.network_id, AppName);
   ncf01_i.primary_key.network_type[0] = 'I';

   if (hhutil_get_ncf01_cfg( &ncf01_i, err_msg) != PTEMSG_OK)
   {
      sprintf(Buffer, "Error on select of NCF01 Issuing: %s", err_msg);
      ncvis2_log_message( 3,2,Buffer, "MainProcessor" , 0 ,"");
   }
   else
   {
      /* Get Network's Acquiring record from NCF01. */
      memset(&ncf01_a, 0, Ncf01_Size);
      strcpy(ncf01_a.primary_key.network_id, AppName);
      ncf01_a.primary_key.network_type[0] = 'A';

      if (hhutil_get_ncf01_cfg( &ncf01_a, err_msg) != PTEMSG_OK)
      {
         sprintf(Buffer, "Error on select of NCF01 Acquiring: %s", err_msg);
         ncvis2_log_message( 3,2,Buffer, "MainProcessor" , 0 ,"");
      }
      else
      {
         //copy the echo timer length
         echo_timer_length = atoi(ncf01_i.echo_interval);

         /* Initialize configurable counters. */
         MaxActiveTxns     = atoi(ncf01_i.max_active_txns);
         MaxConsecTimeouts = atoi(ncf01_i.max_consecutive_timeouts);

         /* Perform auto logon? */
         if (0 == strcmp(ncf01_i.auto_logon,"1"))
         {
            /* Yes, set up a logon request to the host in auth_tx  */
            memset(&auth_tx, 0, Auth_Tx_Size);
            perform_host_logon_request(LOGON_REQUEST, SOURCE_IS_OPERATOR,
                                       &auth_tx, 0);

            /* Set Acquirer Online, but keep Issuer Offline until response. */
            //ret_code=WriteVisaTable(SHARED_MEMORY_TBL,"0","0",ISSOFF_ACQON,"1");
            ret_code=WriteVisaTable(SHARED_MEMORY_TBL,"0","0",BOTH_OFFLINE,"1");
         }
         (void)send_network_status_to_monitor();
      }
   }
   ncvis2_read_TO_config_details_tf_ini();
   /* Set MCC Validation either On or Off. */
   MCC_Validation_Functionality = MCC_VALIDATION_OFF;

   while( !EndProcessSignalled )
	{

		/* You are blocked here waiting for a message on either app queue or control que  */
		/* if there is no message on either que for 5 seconds, the blocking call returns   */

		/* The following line will be used to get messages from the queue. For now the
		   message is automatically created here, but this will be removed */
	    p_msg = pteipc_receive( application_que_name, control_que_name, 5, &ret_code );

			                 
		if( p_msg != NULL_PTR )
		{
			PinnacleMsgHandler( p_msg );
			free (p_msg); 
		}
		else if( ret_code != QUE_ER_TIMEOUT ) 
		{
			pteipc_get_errormsg( ret_code, ErrorMsg );
			sprintf( Buffer, "Error on pteipc_receive %s", ErrorMsg );
			ncvis2_log_message( 3,3,Buffer, "MainProcessor" , 0 ,"");
		}
	}

   /* Send logoff request to host prior to shutting down; ignore response. */
   ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                  active_txns_ctr, current_state, prev_src_station );

   if ((current_state[0] == cBOTH_ONLINE) || (current_state[0] == cISSOFF_ACQON))
   {
      /* Only send logoff if Acquirer state is online. */
      memset(&auth_tx, 0, sizeof(AUTH_TX));
      perform_host_logon_request( LOGOFF_REQUEST,
                                  SOURCE_IS_OPERATOR,
                                  &auth_tx,
                                  ST2_ACQUIRER );
   }

   /* Shutdown */
   WriteVisaTable( SHARED_MEMORY_TBL, "0", "0", DOWN, "1" );
   (void)send_network_status_to_monitor();
#ifdef _DEBUG
	 pteipc_shutdown_single_instance_app(); 
#else
	pteipc_shutdown_multiple_instance_app();
#endif
	MainProcessDone = 1;
}



/*****************************************************************************

  Function:    EndProcess   

  Description: This function will output a message to notify the user that
               ncvisa is being shut down.
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void EndProcess()
{
   CHAR Buffer[256] = {0};

   if ( Tracing == TRACE_ON )
   {
      if ( false == close_trace_file( ppTraceFile_fp ))
      {
         sprintf( Buffer, "Unable to close trace file %s", TraceFile );
         ncvis2_log_message( 3,1,Buffer, "EndProcess" , 0 ,"");
      }
   }

   sprintf( Buffer, "Shutting down the %s Service, version %s",
            ServiceName, Version );
   ncvis2_log_message( 2,1,Buffer, "EndProcess" , 0 ,"");
   strcat( Buffer, "\n" );
   PRINT( Buffer );
}


