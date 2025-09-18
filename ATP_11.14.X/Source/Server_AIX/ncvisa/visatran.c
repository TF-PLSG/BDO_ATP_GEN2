/*******************************************************************************
* Copyright (c) 2008 Hypercom, Inc.
* All Rights Reserved.
*
* MODULE:      visatran.c
*  
* TITLE:       
*  
* DESCRIPTION: This module will parse and manipulate all incoming 
*               messages from the terminal and the host for the 
*               MasterCard host.
*
* APPLICATION: 
*
* AUTHOR:      B. Gable
*
   REVISION HISTORY
   ----------------

    Rev 1.153   Jul 13 2009   TF-Ajay 
    Added code to copy the STIP txn status from ncf30 


	 Rev 1.152 Apr 09 2009 Girija Y TF
   Changed the code to solve issue for SIGNON and SIGNOFF.
   The host state was changing when SIGNON and SIGNOFF request is sent.

   Rev 1.151 March 31 2009 Girija Y TF
   Changed the code to not to send "0430" response back to VISA host.

   Rev 1.150   March 30 2009 17:12:20 Girija Y TF
   Change Request for sending SIGN ON and OFF to Host with DE70 = "078" & "079".

   Rev 1.149   Mar 12, 2008  dirby
      Merge Visa Compliance into the Visa Cadencie

   Rev 1.148   Feb 4, 2008   dirby
      Handling of 0810 messages now includes field 39.  Visa sends
      an approval code; we also must send an approval code in DE39.
      ATP will always send 00 = approved.
   SCR 25045

      Rev 1.147   May 04 2006 16:34:02   dirby
   Modified to store emv response info into the emv data block.
   This was done so emv data can be obtained with an original txn
   for voids and reversals.
   SCR 21042
   
      Rev 1.146   Mar 29 2006 11:29:52   dirby
   Modified to always echo Station IDs for txns from Visa.  Previously,
   they were not echoed if there was an error or decline when moving
   data from Host Message into Auth_Tx.
   SCR 20591
   
      Rev 1.145   Feb 27 2006 11:57:42   dirby
   Changed some of the Host States used by Visa for Issuer and
   Acquirer states to use the general on/off states used by the other
   network controllers.  The reason is that Monitor does not know how
   to handle the Issuer/Acquirer states, so it was not always displaying
   the correct host state.
   SCR 19665
   
      Rev 1.144   Dec 20 2005 14:33:08   dirby
   Added Visa Online File Maintenance handling.  These are
   0302/0312 transactions.
   SCR 18962
   
      Rev 1.143   Aug 26 2005 15:04:18   dirby
   Modified to not copy invoice number from host buffer into Auth Tx
   buffer unless there is data in it.  There never should be except for
   switched in transactions.
   SCR 16693
   
      Rev 1.142   Jun 17 2005 14:13:04   dirby
   Modified the format of the RRN.  Removed the use of the stan and
   GMT hour.  Replaced them with time in the format of: hmmssjjj
   SCR 16053
   
      Rev 1.141   May 17 2005 17:20:56   dirby
   Modified to stop standing in for Auth and Cash Overrides.
   SCR 15420
   
      Rev 1.140   Apr 14 2005 16:31:00   dirby
   Modified to not copy the stan from the host into auth_tx upon a
   response for voids.  This is because that is the original stan.
   SCR 12190
   
      Rev 1.139   Apr 12 2005 17:27:50   dirby
   Changed to send out Voids as if they were reversals.
   SCR 12190
   
      Rev 1.138   Mar 10 2005 17:17:36   dirby
   Modified to transfer host response EMV fields into original auth_tx.
   SCR 14238
   
      Rev 1.137   Jul 23 2004 17:24:34   dirby
   Added support for EMV.
   SCR 1388
   
      Rev 1.136   Jun 08 2004 13:22:12   dirby
   Copy text into response_text when host inactive - for Voice GUI.
   Release 4.3.0-1
   
      Rev 1.135   Apr 29 2004 15:38:30   dirby
   Modified to make sure RRN of voids is the same as the RRN of
   the original txn.  Also removed a function that is no longer used.
   SCR 1382
   
      Rev 1.134   Apr 22 2004 13:11:08   dirby
   Removed references to settlement_date.  This field was not being
   used.  Now, with this change, it is used to hold the ECI for ecommerce
   reversals.  See file visautil.c.
   SCR 1117
   
      Rev 1.133   Apr 22 2003 18:44:50   dirby
   Modified to distinguish between Issuer and Acquirer for Log
   ON/OFF/Echoes.    Also modified to use newly defined host
   states; these states (defined in app_info.h) give the status of
   both Issuer and Acquirer states, using a single value.
   Acquirer Online = ATP is ready to receive requests from host.
   Acquirer Offline = Host needs to stand in for us.
   Issuer Online = ATP is ready to send requests to the host.
   Issuer Offline = No requests will be sent to the host.
   Echoes use the Issuer status.
   
      Rev 1.132   Mar 13 2003 15:12:50   dirby
   Adding code to handle reject messages from Visa.  If a response
   is rejected, get the original txn from TLF01 and reverse out the
   transaction.  If a request to Visa is rejected, treat it as if Visa
   declined it with a format error response code.
   SCR 1034
   
      Rev 1.131   Feb 20 2003 15:43:10   dirby
   Added code to not only use track 2 for VE txns, but also use track1.
   Removed the parsing of track 2 when validating VE; it was redundant.
   SCR 1033
   
      Rev 1.130   Nov 08 2002 11:37:48   dirby
   Added code to not insert record into TLF01 if it has already
   been inserted by the update_all process.
   SCR 845
   
      Rev 1.129   Oct 16 2002 12:04:48   dirby
   Modified processing of echo timer when it expires to immediately
   set the echo timer again.  This is to prevent warning messages
   in the syslog that occur when the 0800 echo msg is sent.  Any
   time a msg is sent to the host, the echo timer is cleared then
   reset.  The 'clear' command caused the warning because the
   timer has already been cleared when it timed out.
   SCR 874
   
      Rev 1.128   Sep 16 2002 15:19:38   dirby
   Added code to process auth_repeats separately.  They need to
   look in NCF30 for the original auth txn in order to know how to
   process them.  If orig txn is found, do not process the repeat;
   just respond.  If orig txn is not found, process the repeat as a
   normal authorization.
   SCR 879
   
      Rev 1.127   Aug 02 2002 15:50:00   dirby
   Removed code so that timeout of echoes do not increment the
   timeout counter.  Only do this for financial transactions.  Do not
   want to go offline just because too many echoes time out.
   SCR 594
   
      Rev 1.126   Jul 25 2002 17:29:10   dirby
   Modified to allow echoes to continue to go out when offline and
   set host state online if a response is received.
   SCR 594
   
      Rev 1.125   Jul 07 2002 03:17:20   dirby
   Fixed bug - return false if host is offline when checking status.
   SCR 594
   
      Rev 1.124   Jul 07 2002 00:51:06   dirby
   1.  Modified to use precalculated 'sizeof' values instead of recalculating
   structure sizes over and over again.
   2.  Reimplemented going offline when max timeouts is reached.
   SCR 594
   
      Rev 1.123   Jul 05 2002 13:29:12   dirby
   Added code to display a message for reversals.
   SCR 621
   
      Rev 1.122   Jun 25 2002 11:22:30   dirby
   Modified function 'alter_transaction_data' to allow balances to
   be moved from response auth_tx into the request auth_tx for
   all transactions.  This is because cash advance txns have
   balances.
   SCR 798
   
      Rev 1.121   Jun 24 2002 10:57:48   dirby
   For switched in txns, change message type of Auth Repeats
   into a regular Auth:  from 0101 to 0100.  Otherwise, txcntl might
   think this is an advice and force an approval. In this case, approved
   responses were going out without approval codes. This caused
   a reject of 0293 from Visa.
   SCR 790
   
      Rev 1.120   Jun 20 2002 15:58:54   dirby
   Modified to pass the Visa CPS data from DE62 through from a
   response into TLF01.
   SCR 794
   
      Rev 1.119   Jun 05 2002 16:07:30   dirby
   1.  Modified to display a message when a txn times out.
        SCR 621
   2.  Modified to not send a response to Visa when a txn comes
        back from txcntl declined.
        SCR 763
   
      Rev 1.118   May 30 2002 09:45:14   dirby
   Changed response code from 05 to 57 for Visa Electron transactions
   that are declined due to being manual entry.  Response code
   of 05 was considered too harsh.
   SCR 751
   
      Rev 1.117   May 29 2002 18:45:20   dirby
   Added check to not validate Visa Electron for reversals or 
   ATM Confirms, since they do not have track data.
   SCR 748
   
      Rev 1.116   May 29 2002 13:21:48   dirby
   Changed function 'is_txn_visa_electron' to determine if card is
   VE based on bin range.  Previously it was based on the service
   code.  Removed all other processing from this function.  Moved
   all the VE verification processing into 'validate_visa_electron'.
   SCR 748
   
      Rev 1.115   May 24 2002 16:09:42   dirby
   Removed line of code that copied currency code from NCF30
   into TLF01.  We already have it, and it could be different.
   SCR 746
   
      Rev 1.114   May 15 2002 15:57:32   dirby
   Modified to make sure there is a response code for ATM Confirm transactions.
   SCR 731
   
      Rev 1.113   May 15 2002 13:30:34   dirby
   Modified to set auth number to all zeroes for reversals caused
   by time outs.
   SCR 737
   
      Rev 1.112   May 15 2002 11:59:08   dirby
   Added invoice_number to alter_transaction_data function.
   It may contain part of the Visa Transaction Id from DE62.  It is
   needed for reversals.
   SCR 725
   
      Rev 1.111   Apr 24 2002 15:22:38   dirby
   Modified process_timeout_msg to populate voice_auth_text
   prior to sending response.
   SCR 701
   
      Rev 1.110   Apr 24 2002 11:15:56   dirby
   Added function 'txn_is_blocked'.  This determines if a txn from
   Visa is not permitted.
   SCR 703
   
      Rev 1.109   Apr 23 2002 14:41:50   dirby
   Added code to not set the request timer for ATM Confirms.
   Visa does not respond to them.
   SCR 718
   
      Rev 1.108   Apr 21 2002 15:14:24   dirby
   Modified to support reversal repeats on the Acquirer side.
   SCR 701
   
      Rev 1.107   Apr 12 2002 22:40:28   dirby
   Modified the time out functionality, where code sets up to issue
   an automatic reversal, to use the Acquiring ID from NCF01 for
   the original Acquiring ID.  Cannot use the one from TLF01.  If
   transaction came from EB Host (nceqit), it contains the Acquiring
   ID for EB Host.
   SCR 699
   
      Rev 1.106   Apr 12 2002 16:40:00   dirby
   Modified to not process the response from a command that
   clears the echo timer.  Found this bug while working on SCR 699
   
      Rev 1.105   Mar 15 2002 15:19:56   dirby
   Modified the code for responses to switched out requests from
   reading the timer then clearing it, to just clearing it.  The read
   has been eliminated, saving an XIPC hop and processing time.
   A read timer reads the timer and sends the data back.  A clear
   timer command reads the timer, sends the data back and 
   clears the timer.
   SCR - There is no SCR associated with this.  It is product improvement.
   
      Rev 1.104   Mar 15 2002 14:26:06   dirby
   Modified to log reversals that are caused by time outs into TLF01.
   And to not send a response for these back to the originator.
   SCR 618
   
      Rev 1.103   Mar 06 2002 14:27:34   dirby
   Added code to copy original DE7 into auth_tx for a reversal
   from host.  This is so that it will be there for nceqit to send out
   in its DE90.  Cannot use value from Visa's DE90; it might be zero.
   SCR 616
   
      Rev 1.102   Jan 23 2002 14:44:58   dirby
   Initialized buffers in transaction statistic functions.
   
      Rev 1.101   Jan 16 2002 15:33:02   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.100   Jan 14 2002 18:22:24   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.99   Jan 07 2002 13:05:50   dirby
   Modified to automatically issue a reversal request on time outs.
   SCR 569 - Send automatic reversal on time outs
   
      Rev 1.98   Dec 11 2001 11:44:54   dirby
   Active txn count was being incremented, but the new value was
   not being stored into shared memory.  Fixed it.     SCR 488
   
      Rev 1.97   Dec 10 2001 15:22:48   dirby
   Modified to always forward requests to host.
   Modified to ignore max active and max timeout counts if configured to zero, else put out a warning if reached.
   Modified to set state to DOWN when shutting down.
   SCR 488
   
      Rev 1.96   Oct 24 2001 15:09:58   dirby
   Removed a line of code so that update_database is called for
   balance inquiries.  This is so ccf03p (velocity) can be updated.
   Need to keep track of the failed pin attempts.
   SCR 515
   
      Rev 1.95   Oct 11 2001 16:11:14   dirby
   Modified to get dispensed amount from DE61 for ATM confirms.   SCR 496
   
      Rev 1.94   Oct 01 2001 11:06:10   dirby
   Modified response from function 'move_iso_to . . ' so that if it
   failed, only send response to host if txn is a request from the
   host.  Else if this is a response from the host, decline it.
   
      Rev 1.93   Aug 03 2001 15:00:04   dirby
   Updated version to 3.2.3.2
   Added some balance inquiry code to 'alter_transaction_data'.
   This is for the sake of services who get this info from ACF01.
   
      Rev 1.92   May 31 2001 14:26:58   SYARLAGA
   Modifications done in incoming host message
   
      Rev 1.91   May 23 2001 15:34:46   SYARLAGA
   Modified AUTH_ADMIN_ADVICE.Prints the message to the monitor without responding.
   
      Rev 1.90   May 01 2001 17:19:24   dirby
   Modified to not insert Auth_Advice_Responses into TLF01 after
   returning from txcntl.  They are logged to TLF01 prior to going to txcntl.
   
      Rev 1.89   Apr 30 2001 10:56:28   SYARLAGA
   Modified visa source and destination header fields.Storing the source and destination values of the original message before sending out to visa in 0110 response.
   
      Rev 1.88   Jan 17 2001 11:50:52   dirby
   Removed alot of the Echo timer code.  There was redundancy that caused error
   messages to be logged for every echo request sent.
   
      Rev 1.87   Oct 18 2000 15:28:30   dirby
   1.  Consolidated initialization code for switched in transactions into 'init_txn'.
   2.  Removed commented out code.
   3.  Relocated host_finish_time to be more accurate.
   
      Rev 1.86   Aug 28 2000 10:03:50   dirby
   Type cast transaction statistics to unsigned long.
   
      Rev 1.85   Aug 21 2000 16:08:48   dirby
   Added code to support transaction statistics for Monitor.
   
      Rev 1.84   Aug 11 2000 16:45:08   dirby
   Modified to send response code of "01" when host is unavailable - per EBs request.
   Also corrected some indentation problems.
   
      Rev 1.83   Aug 09 2000 16:55:02   dirby
   In validate_visa_electron function, changed compare against MAGNETIC_STRIPE
   to skip over the first byte, which is filler in the Hypercom Standard.
   
      Rev 1.82   Aug 09 2000 16:11:04   dirby
   Added a memset of auth_tx for network management requests.
   
      Rev 1.81   Jul 31 2000 15:41:30   dirby
   Modified SQLs into NCF30 to use card number as part of the key.
   
      Rev 1.80   Jul 30 2000 14:43:08   dirby
   Reworked the Log ON/OFF logic to show accurate status
   on Monitor, and to be a little more efficient.
   
      Rev 1.79   Jul 19 2000 17:00:08   dirby
   Changed code for ATM Confirmations (0102 partial reversals) to reverse
   out the amount in field 61 subfield 2.  This is the undispensed amount,
   positions 13-24 in field 61.  It was reversing out the amount in field 61,
   subfield 1, which is the total amount in US Dollars.
   
      Rev 1.78   Jul 16 2000 00:45:14   dirby
   Added available balance and current balance to the response message
   for switched out balance inquiries.
   
      Rev 1.77   Jul 14 2000 09:05:50   dirby
   Modified to not translate PINs  for reversals.
   
      Rev 1.76   Jul 13 2000 16:18:34   dirby
   Modified the standin processing function to not
   overwrite the auth_number with the time if the auth
   number already exists.
   Also removed the use of a temporary variable.  It added no value to the code.
   
      Rev 1.75   Jun 30 2000 14:16:20   dirby
   1.  Removed the code that was forwarding txns to ASK.
   2.  Added code for Visa Electron.
   3.  Added code to support Balance Inquiry.
   
      Rev 1.74   Jun 25 2000 02:00:44   dirby
   Removed the functionality that gets NCF01 every request and response.
   It gets NCF01 only at start up now, never during an online transaction.
   
      Rev 1.73   Jun 24 2000 23:39:10   dirby
   Added ATM CONFIRM functionality.
   
      Rev 1.72   Jun 21 2000 11:30:58   dirby
   Modified so Monitor's status is not updated for every transaction.
   Modified so Monitor's status is not updated for every time out.
   Reworked the logic of function 'verify_host_app_status' to make it
   more efficient and to update shared memory when there is a change.
   
      Rev 1.71   May 12 2000 14:46:20   ddabberu
   If we see an atm confirm message, drop the message, this is a temporary fix,
   Gabriel will talk to equitable guys to see what todo with this request in txcntl.
    then the following piece of code will be moved to the process_incoming_message() function
    with whatever checks to be made. DURGA 5/10/00
       
   
      Rev 1.70   Apr 18 2000 17:08:46   svelaga
   Intialized the echo_auth_tx to avoid 
   garbage filling into auth_tx.
   
      Rev 1.69   Mar 28 2000 14:48:46   ddabberu
   1. get_visa_data() changed.
   2. build_host_Structure() modified to send fld95 only if it has some value in it.
   
      Rev 1.68   Mar 21 2000 17:10:12   ddabberu
   update database if resp_code is 00/55
   
      Rev 1.67   Mar 20 2000 17:25:36   ddabberu
   1.Echo fld90 if sent for reversals
   2.fld44.5 should be null if trk1/trk2 not present.
   3.Send fld38 in responses only when fld39 is 00.
   
      Rev 1.66   Feb 16 2000 15:34:12   svelaga
   Logging into TLF01, even the send transaction 
   to host failed.
   
      Rev 1.65   Feb 10 2000 17:23:28   svelaga
   Implemented the design for SCR #374.
   
      Rev 1.64   Feb 01 2000 15:56:22   svelaga
   Fixed SCR #253 and #256.
   
   Removed the clear echo_timer functionality when
   echo timer times out. 
   
   
      Rev 1.63   Jan 27 2000 13:53:54   svelaga
   Fixed SCR #310.
   
   Small modification for echo-logic in 
   incoming_host_message.
   
      Rev 1.62   Jan 25 2000 13:01:02   svelaga
   removed the trace statements which are kept 
   for debugging purpose.
   
      Rev 1.61   Jan 25 2000 09:47:20   dirby
   Added some standin processing for some void transactions.
   
      Rev 1.60   Jan 25 2000 08:40:24   svelaga
   modified code to 'compare two strings' from
   'equating two strings'.
   
      Rev 1.59   Jan 24 2000 17:15:00   dirby
   Added a clear and set echo timer when echo test is issued.
   
      Rev 1.58   Jan 24 2000 16:04:12   ddabberu
   restored the clear timer request
   in proces_timeout ()
   commented currency code related portion
   in alter_transaction_data
   
      Rev 1.57   Jan 24 2000 10:36:16   ddabberu
   new echo_logic 
   
      Rev 1.54   Jan 18 2000 11:03:58   ddabberu
   1.  Removed the code to clear a timer when there is no response from the host.
       Did this because the timer gets cleared when it times out.
   2.  Modified the function that converts request tx_keys to responses so that the
       default will add 100 to the request, making it a response.
   
   
      Rev 1.53   Jan 14 2000 15:07:26   dirby
   Set default value to the original tx_key in the function
   that translates a request into a response tx_key.
   
      Rev 1.52   Jan 08 2000 23:23:30   mbaker
   Modified 08 Jan 2000 by MDB to address if logic to ensure data updates.
   
      Rev 1.51   Jan 04 2000 13:35:56   ddabberu
   updated to process authorization, auth_advice, and reversals.
   changes made in the following functions:
   process_auth_advices(), incoming_other_host_response(), hhutil_db_ok()
   
      Rev 1.50   Dec 30 1999 16:39:06   dirby
   Modified to update transaction control database tables
   upon successful completion of a switched in authorization.
   
      Rev 1.49   Dec 22 1999 11:02:58   dirby
   Modified to separate advice processing from reversals.
   
      Rev 1.48   Dec 20 1999 15:16:08   ddabberu
   Integration Testested  and the following functions were modified. 
   insertTLf01(), reconUpdate(),
   
   
      Rev 1.47   Nov 18 1999 08:56:20   dirby
   Fixed some compile bugs.
   
      Rev 1.46   Nov 16 1999 15:29:06   dirby
   Updated to:
   1.  Remove negative file processing,
   2.  Incorporate a new scheme for handling reversals,
   3.  Use the UpdateDS Data Server.
   
      Rev 1.45   Aug 25 1999 17:05:56   egable
   Implement the new reversal design.
   
      Rev 1.44   Aug 23 1999 10:35:02   egable
   Use timerds instead of netds for timer.
   
      Rev 1.43   Aug 13 1999 14:56:28   egable
   Insert NCF30 in the switched out path.
   
      Rev 1.42   Aug 12 1999 16:49:16   egable
   Change recon_update
   
      Rev 1.41   Jul 28 1999 14:48:28   egable
   Use 'ncvisa" instead of AppName for the
   acquirer_id.  This is to guarantee that racal
   knows this transaction is from visa.
   
      Rev 1.40   Jul 28 1999 14:03:06   egable
   Set the orig and dest queues for ASK routing.
   This does not change the function of the routing,
   it just makes the .ipc traces easier to read.
   
      Rev 1.39   Jul 27 1999 15:32:48   egable
   Correct the outgoing ASK path.
   
      Rev 1.38   Jul 21 1999 15:44:40   egable
   Change field 37 per Equitable.
   
      Rev 1.37   Jul 16 1999 15:38:02   egable
   Add the ASK routing code.
   
      Rev 1.36   Jul 13 1999 15:58:12   egable
   Output messages to syslog as well as monitor;
   correct a couple of frees.
   
      Rev 1.35   Jul 12 1999 14:39:32   egable
   Set the tx_key for the incoming response
   message to be a response to the original
   message.
   
      Rev 1.34   Jun 24 1999 14:47:14   egable
   Change the response code from "TO" to "19 
   for timeouts.
   
      Rev 1.33   Jun 18 1999 16:43:24   egable
   Use voice_auth_text instead of response_text.
   
      Rev 1.32   Jun 16 1999 16:20:10   egable
   For responses from the host, keep the
   original message type in TLF01.  This is
   for the HOST/2 interface.
   
      Rev 1.31   Jun 03 1999 18:55:52   egable
   Change create_rrn to use the date from 
   field 7.
   
      Rev 1.30   Jun 01 1999 11:51:20   egable
   Correct the message type in field 90.
   
      Rev 1.29   May 27 1999 13:48:28   egable
   Correct compiler error.
   
      Rev 1.28   May 27 1999 13:27:40   egable
   Update the response text for voice auth
   transactions.
   
      Rev 1.27   May 25 1999 16:11:54   egable
   Put the previous source station flag into
   shared memory to allow alternating source
   station ids.
   
   
      Rev 1.26   May 23 1999 14:48:10   egable
   Add the cash_advance_authorization to
   the tx_key match response function.
   
      Rev 1.25   May 18 1999 10:12:30   egable
   Correct lookup key
   
      Rev 1.24   May 14 1999 15:54:34   egable
   Create the transaction id in process_error
   
      Rev 1.23   May 07 1999 14:41:50   egable
   Update original data from TLF01 for reversals.
   
      Rev 1.22   Apr 29 1999 13:19:54   egable
   Correct check for max consec timeouts.
   
      Rev 1.21   Apr 23 1999 14:51:22   egable
   Only populate field 93 for logon, not logoff or
   echo test.
   
      Rev 1.20   Apr 21 1999 01:16:16   egable
   Correct retrieval reference number.
   
      Rev 1.19   Apr 20 1999 16:03:00   egable
   Fix echo test response.
   
      Rev 1.18   19 Apr 1999 10:21:06   dirby
   1.  Added override transaction processing.
   2.  Added function 'create_rrn'.
   
      Rev 1.17   Apr 12 1999 14:46:24   egable
   Change the sleep command for non-windows
   platforms.  Remove message when the pin
   translation fails.
   
      Rev 1.16   Apr 09 1999 15:35:02   egable
   Correct RRN...again.
   
      Rev 1.15   Apr 09 1999 15:10:58   egable
   Correct RRN.
   
      Rev 1.14   Apr 09 1999 11:48:00   egable
   Create RRN differently.
   
      Rev 1.13   Apr 07 1999 08:30:34   egable
   Delay between messages to monitor.
   
      Rev 1.12   Mar 31 1999 15:39:06   egable
   Change send_status_to_network to update the
   'A' and 'I' items.
   
   
      Rev 1.11   Mar 30 1999 15:55:22   egable
    
   
      Rev 1.10   Mar 30 1999 14:07:24   egable
   Only set the timer if the host message is
   built successfully.
   
      Rev 1.9   Mar 26 1999 15:23:00   egable
   Correct update of NCF30 and logons.
   
      Rev 1.8   Mar 25 1999 13:55:16   egable
   Update host_start_time and host_finish_time and
   correct data going to NCF30.
   
      Rev 1.7   Mar 15 1999 13:37:54   egable
   Make corrections from unit test of acquiring 
   path.
   
      Rev 1.6   Mar 12 1999 08:33:06   egable
   Make changes from unit testing.
   
      Rev 1.5   Mar 09 1999 14:51:50   egable
   Correct errors found in unit testing.
   
      Rev 1.4   Mar 04 1999 16:14:34   egable
   Remove the check for ncf01.status = A
   
      Rev 1.3   Feb 23 1999 09:26:56   egable
   Correct errors and change the dataserver
   que names.
   
      Rev 1.2   Feb 02 1999 15:59:36   egable
   Changes to message flow, headers and
   'if' statements.  This is the basis for JCB.
   
   
      Rev 1.1   Jan 11 1999 09:56:48   egable
   Initial changes for equitable, added  issuing
   paths and several message types.
   
      Rev 1.0   17 Dec 1998 16:25:16   jhapitas
   Initial Equitable Revison
   
      Rev 1.0   Dec 04 1998 14:56:32   MSALEH
   initial release
*
*******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "basictyp.h"
#include "pte.h"
#include "ptemsg.h"
#include "pteipc.h"
#include "ptetime.h"
#include "ptetimer.h"
#include "ntutils.h"
#include "visautil.h"
#include "visatran.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "visamain.h"
#include "memmnger.h"
#include "txutils.h"
#include "genutil.h"
#include "echo_timer.h"


/**********************************
      FUNCTION PROTOTYPES
**********************************/

/* functions local */
INT   send_request_to_host();
INT   alter_transaction_data(pAUTH_TX, pAUTH_TX);
INT   send_response_to_term_app(pAUTH_TX);
void  assign_next_stan();
INT   get_term_status(BYTE[]);
INT   verify_host_app_status();
INT   perform_op_net_mngmnt_response(CHAR[],pAUTH_TX);
BYTE  match_request_to_response(BYTE);
void  perform_error_processing(pAUTH_TX);
INT   get_visa_data (pAUTH_TX);
void  calc_julian_day(CHAR[],CHAR[]); 
void  perform_standin_processing(CHAR[], BYTE, pAUTH_TX);

extern BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type );
extern char  A2E[];    /*  ASCII to EBCDIC conversion */
extern char  E2A[];    /*  EBCDIC to ASCII conversion */

extern void bcd_to_asc (BYTE  bstring[],INT blen,pCHAR astr,INT astr_len); // nirbhay: ncvis2 cadencie changes.

/* global variables */
extern INT      Tran_Flow;
extern NCF01    ncf01_i;
extern NCF01    ncf01_a;
extern TLF01    tlf01;
extern struct   iso_msg_rec HOST_MESSAGE;
extern INT      buflen;                  /* total size of buffer */
extern BYTE     ReqRsp_Buffer [COMM_BUF_SZE];
extern CHAR     SHARED_MEMORY_TBL[];
CHAR            ServiceCode[SERVICE_CODE_SIZE_PLUS_1];
extern CHAR          ServiceName[12];
/* Database structure sizes */
extern INT    Ncf01_Size;
extern INT    Ncf30_Size;
extern INT    Tlf01_Size;
extern INT    Auth_Tx_Size;

BYTE    orig_host_start_time[17];
CHAR   *orig_is_ask;
CHAR    trands_que_name[]  = "trandsA";
CHAR    devds_que_name[]   = "devdsA";
CHAR    netds_que_name[]   = "netdsA";
CHAR    timerds_que_name[] = "timerdsA";

extern CHAR        updateds_que_name[];
extern CHAR        authorizer_que_name[];
extern CHAR        cur_app_name[];
extern CHAR        text_txds_queue [];
extern CHAR        oprmon_que_name[];
extern CHAR        AppName[];

extern CHAR        Dest_Stat_Id[];
extern CHAR        applnk_que_name[];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;

/* Counters */
extern INT    MaxActiveTxns;
extern INT    MaxConsecTimeouts;

/* Constants & variables used for processing of LOGONs */ 
CHAR vLogon_NMI_Code[5];
CHAR vLogon_Resp_Indicator[6];

CHAR  LOGON_NMI_START[4] =          "001";
CHAR  LOGON_NMI_ECHO_TEST[5] =      "0301";
CHAR  LOGON_NMI_LOGON[5] =          "0071";
CHAR  LOGON_NMI_LOGOFF[5] =         "0072";

CHAR  LOGON_REQUEST[6] =            "LOGON";
CHAR  LOGON_RESPONSE[16] =          "LOGON RESPONSE";
CHAR  ECHO_REQUEST[11] =            "ECHO";
CHAR  ECHO_RESPONSE[20] =           "LOGON ECHO RESPONSE";
CHAR  ECHO_TIMER_KEY[21] =          "LOGON ECHO TIMER KEY";
CHAR  LOGOFF_REQUEST[7] =           "LOGOFF";
CHAR  LOGOFF_RESPONSE[16] =         "LOGOFF RESPONSE";
CHAR  LOGON_RESP_INDICATOR[6] =     "00002";

/* CR to send SIGN ON and OFF request to Host
Girija Y TF, March 30 2009 */
CHAR  SIGNON_ADVICE_RETRIEVAL[7] =  "SIGNON";
CHAR  SIGNOFF_ADVICE_RETRIEVAL[8] = "SIGNOFF";
CHAR  LOGON_NMI_SIGNON[5] =			"0078";
CHAR  LOGON_NMI_SIGNOFF[5] =		"0079";

CHAR  echo_lookup_key[15]        = "ECHO_VISA";
CHAR  echo_request_timer_key[15] = "ECHO_REQ_VISA";

extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */

/* Miscellaneous */
#define SEND_TO_TIMERDS  1
#define SEND_RETURN_MSG  0 

extern INT   Txn_Is_A_Reject;
extern WORD  echo_timer_length;
extern INT   Third_Bitmap;

#define CWK_KEK       1
#define CWK_MFK       2



/*****************************************************************************

  Function:    send_it_back_to_terminal

  Description: This function will send the auth_tx buffer back to the authorizer.
  Author: 
      unknown
  Inputs:
      resp_code - The response code to be sent to the terminal
      p_auth_tx - A pointer to the auth_tx buffer to be sent to the authorizer.
  Outputs:
      None
  Return values:
      true  - indicates that the message was sent successfully.
      false - indicates that the message could not be sent successfully.
  Modified by:
      BGable      6/30/98
******************************************************************************/
BOOLEAN send_it_back_to_terminal(BYTE resp_code[3], pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg_term_handler_out = NULL_PTR;
   LONG       ret_code;
   CHAR	      Buffer[512] = {0};
   CHAR	      ErrorMsg[512] = {0};

   strcpy(p_auth_tx->TLF01_details.response_code, resp_code);
   p_auth_tx->TLF01_details.tx_key =
                match_request_to_response(p_auth_tx->TLF01_details.tx_key);
   p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              authorizer_que_name,
                                              application_que_name,
                                              (pBYTE)p_auth_tx,
                                              Auth_Tx_Size,
                                              0 );
   if ( NULL_PTR == p_msg_term_handler_out )
   {
	  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build response to txcntl Term Id:%s ,Merch Id: %s RRN: %s ",
    		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncvisa_log_message( 3, 3, Log_Bufr , "send_it_back_to_terminal" , 0 ,p_auth_tx->TLF01_details.card_num);

      return(false);
   }
   ret_code = pteipc_send (p_msg_term_handler_out, authorizer_que_name );
   if ( ret_code < 0 )
   {
      pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "Error on pteipc_send to que %s: %s",
               authorizer_que_name, ErrorMsg );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
    		  p_auth_tx->TLF01_details.terminal_id,
			  p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvisa_log_message( 3, 3, Buffer , "send_it_back_to_terminal" , 0 ,p_auth_tx->TLF01_details.card_num);
      free(p_msg_term_handler_out);
      return(false);
   }
   free(p_msg_term_handler_out);
   return(true);
}


/*****************************************************************************

  Function:    incoming_terminal_request

  Description: This function will process an incoming message from the terminal 
               (via the authorizer)
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the incoming auth_tx
  Outputs:
      None
  Return values:
      true  - indicates that the message processed successfully.
      false - indicates that the message could not be processed successfully.
  Modified by:
      BGable      6/30/98
      DI - 04/14/99 - Added offline cases for override transactions.
******************************************************************************/
INT incoming_terminal_request(pAUTH_TX p_auth_tx)
{
   pPTE_MSG_DATA  p_msg_data = NULL_PTR;
   BYTE           temp_tx_key;
   pPTE_MSG       p_msg_remove_auth = NULL_PTR;


   if (( p_auth_tx->TLF01_details.general_status > GS_NSP_KEYCHANGE_FAILED) &&
       ( p_auth_tx->TLF01_details.general_status <= GS_NSP_OUTOFSERVICE))
   {
      if(send_it_back_to_terminal("58", p_auth_tx ))
         return(true);
      return(false);
   }
	
   /* Populate some Visa fields in auth_tx. */
   if (false == get_visa_data (p_auth_tx) )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));

      sprintf(Log_Bufr,"  Error trying to update Visa information Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncvisa_log_message( 3, 3, Log_Bufr , "incoming_terminal_request" , 0 ,p_auth_tx->TLF01_details.card_num );

      return(false);
   }
   strcpy(p_auth_tx->TLF01_details.acquirer_id, "ncvisa");
   p_auth_tx->TLF01_details.general_status = GS_TXN_NEW;

   /***************************************************************************
    *
    * Check if we do standin processing or if we need to go online
    * to the host for this transaction 
    *
    ***************************************************************************/
   temp_tx_key = p_auth_tx->TLF01_details.tx_key;
   switch ( temp_tx_key)
   {
      case AUTH_OFFLINE_SALE: 
		{	/* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_SALE_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true); 
		}
      case AUTH_REFUND: 
		{	/* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      case AUTH_OFFLINE_REFUND: 
		{	/* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_REFUND_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      case AUTH_VOID_SALE:
		{     /* Void Sales will be sent to Visa as Reversals. */
            break;
		}
      case AUTH_VOID_REFUND:
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_VOID_REFUND_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);
		}
      case AUTH_VOID_CASH_ADVANCE: 
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_VOID_CASH_ADVANCE_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      case AUTH_SALE_ADJUSTMENT: 
		{	/* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_SALE_ADJUSTMENT_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);   
		}
      case AUTH_REVERSAL: 
		{	/* Create tran type string and store it in the auth_tx_block */
         if (0 == atoi(p_auth_tx->TLF01_details.actual_amount))
            p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL;
         else
         {
            /* Partial Dispense */
            p_auth_tx->TLF01_details.tx_key = AUTH_ATM_CONFIRM;

            /* Required field for ATM Confirms */
            if ( p_auth_tx->TLF01_details.response_code[0] == 0x00 )
               strcpy( p_auth_tx->TLF01_details.response_code, APPROVED );
         }

         /* Continue processing as ONLINE transaction. */
         break;
		}
      case AUTH_REFUND_ADJUSTMENT: 
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_REFUND_ADJUSTMENT_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      case AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE: 
		{
            p_auth_tx->TLF01_details.tx_key =
                     AUTH_DEFERRED_PURCHASE_AUTHORIZATION_OVERRIDE_RESPONSE;
            perform_standin_processing( "00", GS_TXN_COMPLETED_OFF, p_auth_tx );
            return( true );
		}
      case AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE: 
		{
            p_auth_tx->TLF01_details.tx_key = AUTH_CASH_ADVANCE_AUTHORIZATION;
			/*TF Praneeth Modified to support tx_key revert back while stip transaction */
			p_auth_tx->TLF01_details.voice_auth_text[0] = AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE;
            break;
		}
      case AUTH_AUTHORIZATION_OVERRIDE: 
		{
            p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION;
			/*TF Praneeth Modified to support tx_key revert back while stip transaction */
			p_auth_tx->TLF01_details.voice_auth_text[0] = AUTH_AUTHORIZATION_OVERRIDE;
            break;
		}
      case AUTH_SALES_COMPLETION: 
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_SALES_COMPLETION_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      case AUTH_OFFLINE_VOID_SALE: 
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_VOID_SALE_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
      case AUTH_OFFLINE_VOID_REFUND: 
		{   /* create tran type string and store it in the auth_tx_block */
            p_auth_tx->TLF01_details.tx_key = AUTH_OFFLINE_VOID_REFUND_RESPONSE;
            perform_standin_processing("00", GS_TXN_COMPLETED_OFF, p_auth_tx);
            return(true);    
		}
		default:  /* online transaction to host */
		{
		}
	} /* end switch statement */ 
   
   /***************************************************************************/
   
   /* confirm host is available */
   if (false == verify_host_app_status(p_auth_tx))
   {
      /* Build response for terminal */
      strcpy( p_auth_tx->TLF01_details.response_text, "Host Inactive" );
      ncvisa_log_message( 3, 2, "Host Inactive" , "incoming_terminal_request" , 0 ,"");

	  /*TF Praneeth revert back tx_key  while stip transaction */
	  if ((p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
	  {
		  p_auth_tx->TLF01_details.tx_key = p_auth_tx->TLF01_details.voice_auth_text[0];
		  memset(p_auth_tx->TLF01_details.voice_auth_text,0x00,sizeof(p_auth_tx->TLF01_details.voice_auth_text));
	  }

      p_auth_tx->TLF01_details.tx_key =
                match_request_to_response(p_auth_tx->TLF01_details.tx_key);

      perform_standin_processing("01", GS_TXN_DROPPED, p_auth_tx);
      return(true);
   }

   /* Only set for a logon */
   memset(vLogon_NMI_Code, 0x00, sizeof(vLogon_NMI_Code));
   /*TF Praneeth clearing buffer variable used to store tx_key  while stip transaction */
   if ((p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_AUTHORIZATION_OVERRIDE) || (p_auth_tx->TLF01_details.voice_auth_text[0] == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE))
   {
	  memset(p_auth_tx->TLF01_details.voice_auth_text,0x00,sizeof(p_auth_tx->TLF01_details.voice_auth_text));
   }
   /* Build request for host. */
   return( send_request_to_host( p_auth_tx ) );

} /* incoming_terminal_request */

/*****************************************************************************

  Function:    process_logon_response

  Description: This function will zero out all counters used in the host handler
               for determining the host's status.  (These counters are in shared
               memory).
  Author: 
      	  	  unknown
  Inputs:
      	  	  None
  Outputs:
      	  	  None
  Return values:
	true  - always returns true
  Modified by:
      BGable      6/30/98
******************************************************************************/
BOOLEAN process_logon_response(CHAR nmi_code[5], CHAR response_code[3])
{
   pPTE_MSG  p_msg_host_app = NULL_PTR;
   CHAR      ErrorMsg[512] = {0};
   BYTE      net_consec_tmouts_ctr[4];
   BYTE      active_txns_ctr[4];
   BYTE      current_state[2];
   BYTE      prev_src_station[4];
   INT       host_status_updated = false;
   INT       ret_val;

   /* Get host state and counters */
   ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                  active_txns_ctr, current_state, prev_src_station );

   if (strcmp(LOGON_NMI_LOGON,nmi_code) == 0)
   {
      if ( (0 == strcmp(response_code,APPROVED)) ||
           (response_code[0] == 0x00)  )
      {
         /* Update host state if it changed.
          * Do not set echo timer here.  It
          * was already set when the response
          * came in from the network.
          */
         ret_val = set_host_state( vONLINE, vONLINE,
                                   net_consec_tmouts_ctr,
                                   active_txns_ctr );
         if ( ret_val == false )
         {
            ncvisa_log_message( 3, 3,  "Unable to change host state" , "process_logon_response" , 0 ,"");
         }
         else
         {
            host_status_updated = true;
            ncvisa_log_message( 3, 1,  "LOGON Request successful" , "process_logon_response" , 0 ,"");
         }
      }
      else
      {
         sprintf( ErrorMsg,
                 "Logon request declined. Response Code: %s",
                  response_code );
         ncvisa_log_message( 3, 2,  ErrorMsg , "process_logon_response" , 0 ,"");
      }
   }
   else if (strcmp(LOGON_NMI_LOGOFF , nmi_code ) == 0)
   {
      if ( (0 != strcmp(response_code,APPROVED)) &&
           (response_code[0] != 0x00)  )
      {
         /* Even though Visa declines the logoff request, still logoff. */
         sprintf( ErrorMsg,
                 "Logoff request response code from host: %s",
                  response_code );
         ncvisa_log_message( 3, 2,  ErrorMsg , "process_logon_response" , 0 ,"");
      }

      /* We only get here for Acquiring Logoffs. Leave Issuing status as is. */
      ret_val = set_host_state( vOFFLINE, vSTAY,
                                net_consec_tmouts_ctr,
                                active_txns_ctr );
      if ( ret_val == false )
      {
         ncvisa_log_message( 3, 3,  "Unable to change host state" , "process_logon_response" , 0 ,"");
      }
      else
      {
         host_status_updated = true;
         ncvisa_log_message( 3, 1, "LOGOFF Request successful" , "process_logon_response" , 0 ,"");

         /* Going offline; clear the echo timer. */
         if ( echo_timer_length > 0 )
         {
            if(false == clear_echo_timer(echo_lookup_key,(pPTE_MSG *)NULL_PTR,
                                         application_que_name,SEND_TO_TIMERDS))
            {
               ncvisa_log_message( 3, 3, "Error clearing the echo_timer" , "process_logon_response" , 0 ,"");
            }
         }
      }
   }

   else if ( strcmp(LOGON_NMI_ECHO_TEST , nmi_code) == 0 )
   {
      if ( (0 != strcmp(response_code,APPROVED)) &&
           (response_code[0] != 0x00)  )
      {
         /* Even though Visa declines the echo, continue. */
         sprintf( ErrorMsg,
                 "Network Management Echo not approved. Response code from host: %s",
                  response_code );
         ncvisa_log_message( 3, 2, ErrorMsg , "process_logon_response" , 0 ,"");
      }

      /* For ECHOs, if offline, set state Online since we just got a response.*/
      if ( (current_state[0] == cBOTH_OFFLINE) ||
           (current_state[0] == cISSOFF_ACQON)  )
      {
         /* Echoes only affect Issuing; set Issuing online, leave Acq as is. */
         ret_val = set_host_state( vSTAY, vONLINE,
                                   net_consec_tmouts_ctr,
                                   active_txns_ctr );
         if ( ret_val == true )
         {
            host_status_updated = true;
            ncvisa_log_message( 3, 1, "Received response to echo. Going back Online" , "process_logon_response" , 0 ,"");

         }
      }

      (void)clear_echo_timer(echo_request_timer_key,(pPTE_MSG *)NULL_PTR,
                             application_que_name, SEND_TO_TIMERDS );
   }

   /* Update host state and counters */
   if ( host_status_updated == true )
      (void)send_network_status_to_monitor();

   return(true);
}


/*
 *	ThoughtFocus Nirbhay : Cadencie changes
 *	This function clears network timer for 0312 msg from VISA to Cadencie (ncvis2A).
 *  and also logs 0312 msg in TLF01 with response code "00".
 */
INT hhutil_clear_timer_for_312_msg_to_Cadencie( pAUTH_TX p_auth_tx, LONG incmg_msg_data_len )
{
	  CHAR     lookup_key[22];
	  pPTE_MSG          p_msg_timer_out = NULL_PTR;
	  pPTE_MSG          p_msg_chain = NULL_PTR;
	  LONG              ret_code;
	  CHAR              ErrorMsg[512]={0};
	  CHAR              Buffer[512]={0};
	  BYTE              buffer[sizeof(AUTH_TX)+sizeof(TLF01)];

	  memset(lookup_key, 0, sizeof(lookup_key));
      strcpy(lookup_key, cur_app_name);
      strcpy(p_auth_tx->TLF01_details.terminal_id, "TID_0302");	// terminal ID for 302/312 msg.
      strcat(lookup_key, p_auth_tx->TLF01_details.terminal_id);
      strcat(lookup_key, p_auth_tx->TLF01_details.sys_trace_audit_num);

      // Build PTEMSG to get Timer Data from the Host Data Server
      p_msg_timer_out = ptetimer_bld_clear_timer_req( lookup_key,
                                                      timerds_que_name,
                                                      application_que_name );
      if ( NULL_PTR == p_msg_timer_out )
      { 
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr,"  Insufficient Memory to build clear timer message Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,
         					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncvisa_log_message( 3, 2, Log_Bufr , "hhutil_clear_timer_for_312_msg_to_Cadencie" , 0 ,p_auth_tx->TLF01_details.card_num);
         return(false);
      }

      memset(buffer, 0, sizeof(buffer));
      //memcpy(buffer,ReqRsp_Buffer, incmg_msg_data_len);
      memcpy(buffer,p_auth_tx, Auth_Tx_Size);
      p_msg_chain = ptemsg_chain_new_message (p_msg_timer_out,
                                              MT_DB_REQUEST,
                                              ST1_DB_ECHO,
                                              ST2_NONE,
                                              timerds_que_name,
                                              application_que_name,
                                              buffer,
                                              Auth_Tx_Size,
                                              0);

      // get Timer message from Host Data Server now
      ret_code = pteipc_send( p_msg_chain, timerds_que_name);
      free( p_msg_chain);
      if(ret_code < 0)
      {  // failed getting timer data
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buffer, "pteipc error sending to que %s: %s",
                  timerds_que_name, ErrorMsg );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,
         					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvisa_log_message( 3, 3, Buffer , "hhutil_clear_timer_for_312_msg_to_Cadencie" , 0 ,p_auth_tx->TLF01_details.card_num);
         return(false);
      }
	  return(true);
}  /* hhutil_clear_timer_for_312_msg_to_Cadencie */


/*****
 *	ThoughtFocus Nirbhay : Cadencie changes
 *	This function sends 0312 msg from VISA to Cadencie (ncvis2A)
 *
 ******/
BYTE hhutil_transfer_312_msg_to_Cadencie( pPTE_MSG p_msg )
{
   BYTE      retval              = true;
   pPTE_MSG  p_msg_out           = NULL_PTR;
   LONG      retCode             = 0L;
   CHAR      strError[512]       = {0};
   BYTE      originator_queue[17]= {0};
   BYTE      originator_info[33] = {0};
   CHAR cadencie_host_que_name  [MAX_QUE_NAME_SIZE+1] = "ncvis2A" ;

   strcpy( originator_queue, ptemsg_get_msg_orig_queue(p_msg) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info(p_msg) );

   /*
   // Append originator Q to end of ReqRsp_Buffer: Tag, Length, Data.
   len = strlen( ORIG_QUEUE );
   memcpy( ReqRsp_Buffer+buflen, ORIG_QUEUE, len );
   buflen += len;

   len = strlen( originator_queue );
   ReqRsp_Buffer[buflen++] = len;
   memcpy( ReqRsp_Buffer+buflen, originator_queue, len );
   buflen += len;

   // Append originator Info to end of ReqRsp_Buffer: Tag, Length, Data.
   len = strlen( ORIG_INFO );
   memcpy( ReqRsp_Buffer+buflen, ORIG_INFO, len );
   buflen += len;

   len = strlen( originator_info );
   ReqRsp_Buffer[buflen++] = len;
   memcpy( ReqRsp_Buffer+buflen, originator_info, len );
   buflen += len;
	*/


   p_msg_out = ptemsg_build_msg( MT_INCOMING_GUI,
                                 ST1_NONE,
                                 ST2_NONE,
                                 cadencie_host_que_name,
                                 application_que_name,
                                 ReqRsp_Buffer,
                                 buflen,
                                 0 );

   if ( p_msg_out == NULL_PTR )
   {
      sprintf( strError,
              "Insufficient Memory to build ncvisa request message. Size: %d",
               buflen );
      ncvisa_log_message( 3, 3, strError , "hhutil_transfer_312_msg_to_Cadencie" , 0 ,"");
      retval = false;
   }
   else
   {
      ptemsg_set_orig_comms_info( p_msg_out, originator_info );
      retCode = pteipc_send( p_msg_out, cadencie_host_que_name );
      free (p_msg_out);

      if ( retCode < 0 )
      {
         pteipc_get_errormsg (retCode, strError);
         sprintf( strError,
                 "Error on pteipc_send to que %s: %s",
                  originator_queue, strError );
         ncvisa_log_message( 3, 3, strError , "hhutil_transfer_312_msg_to_Cadencie" , 0 ,"");
         retval = false;
      }
   }
   return( retval );
} /* hhutil_transfer_312_msg_to_Cadencie */



/*****************************************************************************

  Function:    incoming_host_message

  Description: This function will process a message received from the host.

  Author: 
      unknown
  Inputs:
      p_msg  - incomin host message
  Outputs:
      None
  Return values:
      true  - indicates that the message processed successfully.
      false - indicates that the message could not be processed successfully.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT incoming_host_message(pPTE_MSG p_msg)
{
   pPTE_MSG_DATA     p_msg_data = NULL_PTR;
   pPTE_MSG          p_msg_timer_out = NULL_PTR;
   pPTE_MSG          p_msg_chain = NULL_PTR;
   AUTH_TX           auth_tx;
   LONG              ret_code = 0;
   INT               data_len=0;
   CHAR              Buffer[512]={0};
   CHAR              ErrorMsg[512]={0};
   CHAR              unique_tran_str_id[17] = "";
   CHAR              nmi_code[5];
   CHAR              lookup_key[22];
   pBYTE             p_data_data = NULL_PTR;
   BYTE              temp_originator_queue  [17] = {0};
   BYTE              buffer[sizeof(AUTH_TX)+sizeof(TLF01)] = {0};
   BYTE              tran_type = 0;
   BYTE              net_consec_tmouts_ctr[4] = {0};
   BYTE              active_txns_ctr[4] = {0};
   BYTE              current_state[2] = {0};
   BYTE              prev_src_station[4] = {0};
   CHAR              temp_src_station_id[7] = {0};
   CHAR              temp_dest_station_id[7] = {0};
   INT               i=0,tmp_trace_len=0;
   BYTE 			 temp_msg_trace[150]={0};
   FILE 			*lastmsgptr=NULL_PTR;
   /* Update transaction statistics. */
   update_txn_stats( &Txn_Counts, (pAUTH_TX)NULL_PTR, RECEIVED );

   /* clear and set the echo timer  */
   if ( echo_timer_length > 0 )
   {
      (void)clear_echo_timer(echo_lookup_key,(pPTE_MSG *)NULL_PTR,
                             application_que_name, SEND_TO_TIMERDS );

      /* Set the echo timer. */
      if ( false == set_echo_timer(echo_lookup_key, echo_timer_length,
                                   (pPTE_MSG *)NULL_PTR,application_que_name,
                                   SEND_TO_TIMERDS) )
      {
         ncvisa_log_message( 3, 3, "Error setting the echo_timer" , "incoming_host_message" , 0 ,"");
      }
   }

   /* Get the p_msg data from p_msg */
   p_msg_data = ptemsg_get_pte_msg_data(p_msg);  

   /* copy data to ReqRsp_Buffer */
   p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
   data_len = ptemsg_get_pte_msg_data_data_length(p_msg_data);
   copy_to_req_resp_buffer( p_data_data, data_len);

 /*  if(strcmp(ServiceName,"ncvis3")==0)
   	{	lastmsgptr = fopen("ncvis3_last_msg_bin_trace.log","w");
   		if(lastmsgptr != NULL_PTR)
   		{
   			memcpy(temp_msg_trace,ReqRsp_Buffer,data_len);
   			fwrite(temp_msg_trace,1,data_len,lastmsgptr);
   			fclose(lastmsgptr);
   		}
   	} */
   /* Log transaction to trace file. */
   write_to_txn_file();

   /* Parse the message from the host into HOST_MESSAGE. */
   if (false == hhutil_parse_host_response_msg(&auth_tx))
   {
	   return(true);
   }

   /* Save Station Ids until after clearing Auth_Tx. */
   strcpy( temp_src_station_id,  auth_tx.incoming_src_station_id  );
   strcpy( temp_dest_station_id, auth_tx.incoming_dest_station_id );

   /* Put iso data into auth_tx struct */
   memset(ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
   memset(&auth_tx, 0, Auth_Tx_Size);
	/*Copy orginal msg in req buffer*/
	
	if(data_len < COMM_BUF_SZE && data_len > 0)
	{
		sprintf(auth_tx.req_data_len,"%04d", data_len);
		memcpy(auth_tx.req_buffer,ReqRsp_Buffer,data_len);
	}
   /* Determine transaction type. */
   tran_type = get_tran_type_2(auth_tx.TLF01_details.tx_key);
   auth_tx.TLF01_details.tx_key = tran_type;

   strcpy( auth_tx.incoming_src_station_id,  temp_src_station_id  );
   strcpy( auth_tx.incoming_dest_station_id, temp_dest_station_id );

   if ( false == move_iso_response_to_auth_struct(nmi_code, &auth_tx) )
   {
      /* Something is wrong with the data. */
      tran_type = get_tran_type_2(auth_tx.TLF01_details.tx_key);
      auth_tx.TLF01_details.tx_key = tran_type;
      if ( tran_type > 100 )
      {
         /* This is a response.  Decline txn, but continue processing. */
         if ( 0 == strcmp(auth_tx.TLF01_details.response_code, "00") )
         {
            strcpy( auth_tx.TLF01_details.response_code, INVALID_TXN );
            memset( auth_tx.TLF01_details.auth_number, 0x00, 6 );
         }
      }
      else
      {
         /* Update transaction statistics. */
         update_txn_stats( &Txn_Counts, &auth_tx, NA );

         /* This is a request.  Send back declined response. */
         auth_tx.TLF01_details.tx_key =
                     match_request_to_response(auth_tx.TLF01_details.tx_key);
         strcpy( auth_tx.TLF01_details.response_code, INVALID_TXN );

         /* Get host state and counters */
         ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                        active_txns_ctr, current_state, prev_src_station );

         /* Acquiring state must be online to process requests from host. */
         if ( (current_state[0] == cBOTH_ONLINE) ||
              (current_state[0] == cISSOFF_ACQON) )
         {
            /* It is online; send response. */
            (void)incoming_other_host_response(&auth_tx);
         }
         return( true );
      }
   }
   /* Save the TPDU ID so we know where to send the request / response. */
   memcpy(auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id,
          sizeof(ncf01_i.tpdu_id));
	// For Logging last msg -- remove after 
	if(strcmp(ServiceName,"ncvis3")==0)
	{
		memset(temp_msg_trace,0x00,sizeof(temp_msg_trace));
		lastmsgptr = fopen("ncvis3_last_msg_trace1.log","w");
		if(lastmsgptr != NULL_PTR)
		{
			sprintf(temp_msg_trace,"Card_no: %s , STAN : %s ",auth_tx.TLF01_details.card_num,auth_tx.TLF01_details.sys_trace_audit_num);
			tmp_trace_len = strlen(temp_msg_trace);
			fwrite(temp_msg_trace,1,tmp_trace_len,lastmsgptr);
			fclose(lastmsgptr);
		}
	}

   /* Determine transaction type. */
   tran_type = get_tran_type_2(auth_tx.TLF01_details.tx_key);
   auth_tx.TLF01_details.tx_key = tran_type;
	/*  Just log the message - TF Phani */
	if(AUTH_ADMINISTRATIVE_TEXT== auth_tx.TLF01_details.tx_key)
	{
		strcpy( Buffer, "ADMINSTRATIVE MSG from Visa. Message type = ");
		strcat( Buffer,  auth_tx.TLF01_details.message_type );
		ncvisa_log_message( 3, 1, Buffer , "incoming_host_message" , 0 ,"");
		(void)insert_tlf01( &auth_tx );
		return(true);
	}

   /* If unknown transaction type, report an error and drop it. */
   if(AUTH_INVALID == auth_tx.TLF01_details.tx_key)
   {
      /*+-------------------------+
        | Send out error message. |
        +-------------------------+*/
      strcpy( Buffer, "Unsupported transaction from Visa. Message type = ");
      strcat( Buffer,  auth_tx.TLF01_details.message_type );
      strcat( Buffer, ", Processing code = " );
      strcat( Buffer,  auth_tx.TLF01_details.processing_code );
      strcat( Buffer, auth_tx.TLF01_details.terminal_id );
      strcat( Buffer, auth_tx.TLF01_details.retrieval_ref_num );
      ncvisa_log_message( 3, 3, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
      /* Prevents sending response to host.
       * Can't send a response to a request we don't understand.
       */
      return(true);
   }

   /* If txn is not a reject, process as normal,
    * or if it is a reject of a request we sent to Visa,
    * then set response code to 'format error', clear timer, and send back
    * to txcntl where txn will be treated as a normal declined txn.
    * If txn is a reject of a response we sent to Visa, then we need to
    * get orig txn from TLF01 and reverse it out.
    */
   if ( (Txn_Is_A_Reject == false) ||
        ((Txn_Is_A_Reject == true) && (tran_type < 100)) )
   {
      /* Txn is either not a reject, or a reject of a request. */
      if ( Txn_Is_A_Reject == true )
      {
         strcpy( auth_tx.TLF01_details.response_code, FORMAT_ERROR );

         /* Need to convert this request to a response. */
         if ( auth_tx.TLF01_details.message_type[1] == '0' )
            auth_tx.TLF01_details.message_type[1] = '1';

         else if ( auth_tx.TLF01_details.message_type[1] == '2' )
            auth_tx.TLF01_details.message_type[1] = '3';

         tran_type = match_request_to_response( tran_type );
         auth_tx.TLF01_details.tx_key = tran_type;

         strcpy( auth_tx.TLF01_details.response_text,
                "Received Visa Reject Msg on the Request" );
      }

      /* Handle logons here. */
      if(0 == strcmp(auth_tx.TLF01_details.message_type, "0810"))
      {
         /* Update transaction statistics. */
         update_txn_stats( &Txn_Counts, &auth_tx, NA );

         process_logon_response(nmi_code, auth_tx.TLF01_details.response_code);
         return(true);
      }
      else if(0 == strcmp(auth_tx.TLF01_details.message_type, "0800"))
      {  
         perform_host_logon_request(nmi_code, SOURCE_IS_HOST, &auth_tx, 0);
         return(true);
      }

      /* Update transaction statistics. */
      update_txn_stats( &Txn_Counts, &auth_tx, NA );

      if((tran_type >= AUTH_SALE) && (tran_type <= AUTH_INVALID))
      {
         Tran_Flow = SWITCHED_IN;

         /* Get host state and counters */
         ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                        active_txns_ctr, current_state, prev_src_station );

         /* Acquiring state must be online to process requests from host. */
         if ( (current_state[0] == cBOTH_ONLINE) ||
              (current_state[0] == cISSOFF_ACQON) )
         {
            /* Initialize the TLF01 buffer. */
            init_txn( &auth_tx );

            strcpy(auth_tx.TLF01_details.originator_queue,
                   ptemsg_get_msg_orig_queue ( p_msg ));
            strcpy(auth_tx.TLF01_details.originator_info,
                   ptemsg_get_msg_orig_comms_info (p_msg));

            if (AUTH_ADMIN_ADVICE == auth_tx.TLF01_details.tx_key)
            {
               sprintf( Buffer,
                       " CRIS Alert: %s",
                        auth_tx.TLF01_details.voice_auth_text );
				memset(Log_Bufr,0x00,sizeof(Log_Bufr));
				sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
						auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
				strcat(Buffer,Log_Bufr);
               ncvisa_log_message( 3, 1, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
               return(true);
            }

            /*+------------------------------+
              | Process Switched In messages |
              +------------------------------+*/

            /* Are we blocking this transaction? */
            /* Commented as per the april mandated BR 2020.
             * */
           /* if ( true == txn_is_blocked(&auth_tx,ErrorMsg) )
            {
               // Txn is blocked.  Log message and respond to host.
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
               					auth_tx.TLF01_details.terminal_id,
               					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               strcat(ErrorMsg,Log_Bufr);
               ncvisa_log_message( 3, 2, ErrorMsg , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);

               auth_tx.TLF01_details.tx_key =
                        match_request_to_response(auth_tx.TLF01_details.tx_key);

               strcpy( auth_tx.TLF01_details.response_code, NOT_PERMITTED );
               (void)incoming_other_host_response(&auth_tx);
               return( false );
            }*/

            /* Check for Visa Electron request. */
            if ( true == is_txn_visa_electron( &auth_tx ) )
            {
               if ( false == validate_visa_electron( &auth_tx, ErrorMsg ) )
               {
                  /* Txn is Visa Electron, but validation checks failed. */
                  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
                  sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
                  					auth_tx.TLF01_details.terminal_id,
                  					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
                  strcat(ErrorMsg,Log_Bufr);
                  ncvisa_log_message( 3, 2, ErrorMsg , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);

                  auth_tx.TLF01_details.tx_key =
                        match_request_to_response(auth_tx.TLF01_details.tx_key);
                  (void)incoming_other_host_response(&auth_tx);
                  return( false );
               }
            }

            if ( 0 == strcmp(auth_tx.TLF01_details.message_type, "0101") )
            {
               /* Process an Auth Repeat. */
               process_auth_repeat_part1( &auth_tx );
               return(true);
            }

            return( hhutil_process_host_request(&auth_tx) );
         }
         return( true );
      }

      /* Process Switched Out transactions. */
      Tran_Flow = SWITCHED_OUT;
      ptetime_get_strtime(auth_tx.TLF01_details.host_finish_time);
      strcpy(temp_originator_queue, ptemsg_get_msg_orig_queue ( p_msg ));
      orig_is_ask = strstr( temp_originator_queue, "ask" );   
      if( orig_is_ask != NULL )
      {
         ptemsg_set_msg_type (p_msg, MT_OUTGOING);
         ptemsg_set_orig_comms_info(p_msg, "VISA"); 
         ptemsg_set_orig_queue(p_msg, application_que_name);
         ptemsg_set_dest_queue(p_msg, "hstvisaA");
         ret_code = pteipc_send(p_msg, "dialogA");
         if( ret_code < 0 )
         {
            pteipc_get_errormsg( ret_code, ErrorMsg );
            sprintf( Buffer, "error on pteipc_send to que %s: %s",
                    "dialogA", ErrorMsg );

            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            					auth_tx.TLF01_details.terminal_id,
            					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            strcat(Buffer,Log_Bufr);
            ncvisa_log_message( 3, 3, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
            return(false);
         }
         else
         {
            /* Update transaction statistics */
            update_txn_stats( &Txn_Counts, &auth_tx, SENT );

            /* Log transaction to trace file. */
            write_to_txn_file();
         }
         return(true);
      }

      /* Prepare to clear the timer. */
      if ( auth_tx.TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
         strcpy( auth_tx.TLF01_details.terminal_id, TID_0302 );

      memset(lookup_key, 0, sizeof(lookup_key));
      strcpy(lookup_key, cur_app_name);
      strcat(lookup_key, auth_tx.TLF01_details.terminal_id);
      strcat(lookup_key, auth_tx.TLF01_details.sys_trace_audit_num);

      /* Build PTEMSG to get Timer Data from the Host Data Server */ 
      p_msg_timer_out = ptetimer_bld_clear_timer_req( lookup_key,
                                                      timerds_que_name,
                                                      application_que_name );
      if ( NULL_PTR == p_msg_timer_out )
      { 
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Insufficient Memory to build clear timer message Term Id:%s ,Merch Id: %s RRN: %s ",
         					auth_tx.TLF01_details.terminal_id,
         					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         ncvisa_log_message( 3, 2, Log_Bufr , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
         return(false);
      }

      memset(buffer, 0, sizeof(buffer));
      memcpy(buffer,&auth_tx, Auth_Tx_Size);
      p_msg_chain = ptemsg_chain_new_message (p_msg_timer_out,
                                              MT_DB_REQUEST,
                                              ST1_DB_ECHO,
                                              ST2_NONE,
                                              timerds_que_name,
                                              application_que_name,
                                              buffer,
                                              Auth_Tx_Size,
                                              0);

      /* get Timer message from Host Data Server now  */
      ret_code = pteipc_send( p_msg_chain, timerds_que_name);
      free( p_msg_chain);
      if(ret_code < 0)
      {  /* failed getting timer data */
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( Buffer, "pteipc error sending to que %s: %s",
                  timerds_que_name, ErrorMsg );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
         					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvisa_log_message( 3, 3, Buffer , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
         return(false);
      }
   }
   else
   {
      /* A response we sent to Visa got rejected. */
      memset( buffer,              0x00,                   sizeof(buffer) );
      memcpy( buffer,              &auth_tx,               Auth_Tx_Size   );
      memcpy( buffer+Auth_Tx_Size, &auth_tx.TLF01_details, Tlf01_Size     );

      /* Get orig txn from TLF01. */
      ret_code = build_and_send_request_message( MT_DB_REQUEST, 
                                                 ST1_DB_SELECT,
                                                 ST2_DB_SELECT_TLF01_FOR_VISA_REJECTS,
                                                 devds_que_name, 
                                                 (pBYTE)buffer,
                                                 sizeof(buffer), 
                                                 TLF01_DATA,
                                                 NULL_STR,
                                                 ErrorMsg );
      if ( false == ret_code  )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",auth_tx.TLF01_details.terminal_id,
        		 auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(ErrorMsg,Log_Bufr);
         ncvisa_log_message( 3, 2, ErrorMsg , "incoming_host_message" , 0 ,auth_tx.TLF01_details.card_num);
      }
   }
   return(true);
}

/*****************************************************************************

  Function:    incoming_host_message_timer_reply

  Description: This function will receive a reply from timerds in 
               response to a clear timer in the response to a switched
               out request.
  Author: 
      unknown
  Inputs:
      p_msg_in -  a pointer to the incoming timer message
  Outputs:
      None
  Return values:
      true  - indicates that the message processed successfully.
      false - indicates that the message could not be processed successfully.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT incoming_host_message_timer_reply(pPTE_MSG p_msg_in)
{
   pPTE_MSG       p_msg_timer_out = NULL_PTR;
   pPTE_MSG_DATA  p_msg_data = NULL_PTR;
   pPTE_MSG       p_echo_msg = NULL_PTR;
   pBYTE          p_data;
   LONG           timer_reply_size = 0;
   CHAR           app_data1[32];
   CHAR           app_data2[32];
   CHAR           lookup_key[22];
   BYTE           buffer[sizeof(AUTH_TX)];
   LONG           data_len;
   AUTH_TX        auth_tx;
   AUTH_TX        temp_auth_tx;
   INT            ret_val = true;
   INT            len;
   CHAR			  err_msg[256] = {0};

   /* Get the data from the buffer. */
   timer_reply_size = ptetimer_get_timer_reply(p_msg_in, lookup_key,
                                               app_data1, app_data2,
                                               buffer, sizeof(buffer));
   if ( (0 == strcmp(lookup_key,echo_lookup_key))       || 
        (0 == strcmp(lookup_key,echo_request_timer_key)) )
   {
      /* No need to process response to a command to clear the echo timer. */
      ret_val = true;
   }
   else if (timer_reply_size != sizeof(buffer))
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr,"  No data returned with the get timer message Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncvisa_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
      ret_val = false;
   }
   else
   {
      /* Get original auth_tx structure; it does not include info from host. */
      memset(&temp_auth_tx, 0, Auth_Tx_Size);
      memcpy(&temp_auth_tx, buffer, Auth_Tx_Size);

      /* Get the info from the host response message.
       * It was chained to the clear-timer message.
       */
      p_echo_msg  = ptemsg_get_chained_pte_msg (p_msg_in,2);
      p_msg_data  = ptemsg_get_pte_msg_data (p_echo_msg);
      p_data      = ptemsg_get_pte_msg_data_data (p_msg_data);
      data_len    = ptemsg_get_pte_msg_data_data_length (p_msg_data);

      if (data_len != sizeof(buffer))
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr,"  No data returned with the get timer message Term Id:%s ,Merch Id: %s RRN: %s ",
         					auth_tx.TLF01_details.terminal_id,
         					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         ncvisa_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);

         ret_val = false;
      }
      else
      {
         memset(&auth_tx, 0, Auth_Tx_Size);
         memcpy(&auth_tx, p_data, Auth_Tx_Size);

         /* Set tx_key to a response. */
         temp_auth_tx.TLF01_details.tx_key =
                   match_request_to_response(temp_auth_tx.TLF01_details.tx_key);

         /* Copy the host response info into temp_auth_tx. */
         if (false == alter_transaction_data(&auth_tx, &temp_auth_tx) )
         {
            ret_val = false;
         }
#if BDOR_62_PREAUTH_VOID
         else if(strncmp(temp_auth_tx.TLF01_details.handler_queue, parvbh_que_name, 7)==0)
         {
			 if ( true != send_response_to_parvbh( &temp_auth_tx ))
			 {
				 memset(Log_Bufr, 0x00, sizeof(Log_Bufr));
				 sprintf(Log_Bufr, "Unable to send response to tool Term Id:%s ,Merch Id: %s RRN: %s ",
						auth_tx.TLF01_details.terminal_id,
						auth_tx.TLF01_details.merchant_id,
						auth_tx.TLF01_details.retrieval_ref_num);
				 ncvisa_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
				 ret_val = false;
			 }
			 ret_val = true;
         }
#endif
         else
         {
            /* Set flag indicating txn was authorized at a remote host. */
            strcpy(temp_auth_tx.TLF01_details.dispensation.auth_2,"RA");

            /* For Voice Auth txns, populate response text field. */
            if ( temp_auth_tx.TLF01_details.voice_auth[0] == 'Y' )
               hhutil_get_response_text(&temp_auth_tx);

            /* Is this a repeat or a reversal caused by a time out? */
            if ( (temp_auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT_RESPONSE) ||
                 ((temp_auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE) &&
                  (temp_auth_tx.advice_repeat_count[0] != 0x00) ) )
            {
               /* Yes it is.  Log the reversal to TLF01.
                * No need to send response to originator; a time
                * out response should already have been sent back.
                * Need to change transaction id to avoid unique
                * constraint violation with the timed out transaction.
                * Increase the last digit by 1.
                */
               len = strlen( temp_auth_tx.TLF01_details.primary_key.transaction_id );

               if ( temp_auth_tx.TLF01_details.primary_key.transaction_id[len-1] == '9' )
                  temp_auth_tx.TLF01_details.primary_key.transaction_id[len-1] = '0';
               else
                  temp_auth_tx.TLF01_details.primary_key.transaction_id[len-1] += 1;

               (void)insert_tlf01( &temp_auth_tx );
            }
            else
            {
               /* No it is not. */
               if ( temp_auth_tx.TLF01_details.tx_key == AUTH_CARD_UPDATE_RESPONSE )
               {
                  process_0312_response( &temp_auth_tx );
               }
               else if(true == ncvisa_check_for_acq_refund_transaction(&temp_auth_tx)||
            		   true ==  ncvisa_check_for_cadencie_acq_transaction(&temp_auth_tx))
			   { /*cadeince sent refund transaction*/
            	   memcpy(temp_auth_tx.req_data_len,auth_tx.req_data_len,sizeof(auth_tx.req_data_len));
            	   memcpy(temp_auth_tx.req_buffer,auth_tx.req_buffer,sizeof(auth_tx.req_buffer));
            	   ret_val= ncvisa_transfer_refund_response_msg_to_VISA2(&temp_auth_tx );
			   }
               /* Send response back to originator via txcntl. */
               else if (false == (ret_val = send_response_to_term_app(&temp_auth_tx)) )
               {
                  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
                  sprintf(Log_Bufr,"  Unable to send response to authorizer Term Id:%s ,Merch Id: %s RRN: %s ",
                  					auth_tx.TLF01_details.terminal_id,
                  					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
                  ncvisa_log_message( 3, 2, Log_Bufr , "incoming_host_message_timer_reply" , 0 ,auth_tx.TLF01_details.card_num);
               }
            }
         }
      }
   }
   return( ret_val );

} /* incoming_host_message_timer_reply */

BOOLEAN send_response_to_parvbh	( pAUTH_TX p_auth_tx )
{
	pPTE_MSG    p_msg_term_handler_out = NULL_PTR;
	LONG        ret_code;
	CHAR	      Buffer[512]={0};
	CHAR	      ErrorMsg[512]={0};
	BYTE        buffer1[sizeof(AUTH_TX)];


	decrement_active_txn_ctr( true );

	/* build the message to be sent to the terminal handler */
	memset(buffer1, 0, sizeof(buffer1));
	memcpy(buffer1, p_auth_tx, Auth_Tx_Size);
	p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE,
											  ST1_NONE,
											  ST2_NONE,
											  parvbh_que_name,
											  application_que_name,
											  (pBYTE)&buffer1,
											  sizeof(buffer1),
											  0 );

	ret_code = pteipc_send (p_msg_term_handler_out, parvbh_que_name);
	if ( ret_code < 0 )
	{
	  pteipc_get_errormsg( ret_code, ErrorMsg );
	  sprintf( Buffer, "pteipc error sending to que %s: %s", parvbh_que_name, ErrorMsg );
	  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	  sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
			  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,
			  p_auth_tx->TLF01_details.retrieval_ref_num);
	  strcat(Buffer,Log_Bufr);
	  ncvisa_log_message( 3, 3, Buffer, "send_response_to_parvbh", 0, p_auth_tx->TLF01_details.card_num );

	  free(p_msg_term_handler_out);
	  return(false);
	}
	else
	{
	  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	  sprintf(Log_Bufr," Response sent successfully to tool Term Id:%s ,Merch Id: %s RRN: %s ",
			  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,
			  p_auth_tx->TLF01_details.retrieval_ref_num);
	  ncvisa_log_message( 2, 1, Log_Bufr, "send_response_to_parvbh", 0, p_auth_tx->TLF01_details.card_num);
	}
	free(p_msg_term_handler_out);
	return(true);
}
/*****************************************************************************

  Function:    process_timeout_msg

  Description: This function will process a timeout message by getting the auth_tx
               from the timer timeout message and creating a timeout response to the
               terminal.
  Author: 
      unknown
  Inputs:
      p_msg - the incoming timeout message from the timer
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT process_timeout_msg(pPTE_MSG p_msg)
{
   AUTH_TX     auth_tx;
   BYTE        buffer2[sizeof(AUTH_TX)];
   BYTE        net_consec_tmouts_ctr[4];
   BYTE        active_txns_ctr[4];
   BYTE        current_state[2];
   BYTE        prev_src_station[4];
   BYTE        temp_tx_key;
   CHAR        lookup_key[MAX_TIMER_KEY_SIZE];
   CHAR        app_data1[MAX_APP_DATA_SIZE];
   CHAR        app_data2[MAX_APP_DATA_SIZE];
   CHAR	      	Buffer[512]={0};
   CHAR		   ErrorMsg[512] = {0};
   CHAR        tempstr[10];
   LONG        timer_reply_size = 0;
   INT         tempint;
   INT         repeat_ctr;
   INT         ret_val;
   INT         update_flag = true;

   /* get timeout values */
   timer_reply_size = ptetimer_get_timer_reply(p_msg, lookup_key, app_data1,
                                             app_data2,buffer2,sizeof(buffer2));

   /* Handle Echo Timer */
   /* ----------------- */
 	if(strcmp(lookup_key,echo_lookup_key) == 0)
   {
      /* 1. Echo timer is automatically cleared 
       * 2. Set Echo timer
       * 3. Send ECHO request to host
       */
 
     /* Reset echo timer. */
      if(false == set_echo_timer(echo_lookup_key,echo_timer_length,
                                 (pPTE_MSG *) NULL_PTR,application_que_name,
                                 SEND_TO_TIMERDS))
      {
         ncvisa_log_message( 3, 3, "Error setting the echo_timer" , "process_timeout_msg" , 0 ,"");
         ret_val = false;
      }
      else
         ret_val = true;

      memset (&auth_tx, 0, Auth_Tx_Size);
      if (false == perform_host_logon_request(ECHO_REQUEST, SOURCE_IS_OPERATOR,
                                             &auth_tx, ST2_ISSUER))
      {
         ncvisa_log_message( 3, 1, "Error executing host Echo Test" , "process_timeout_msg" , 0 ,"");
      }
   }

 	else if(strcmp(lookup_key,echo_request_timer_key) == 0)
   {
      /* An Echo request has timed out. Take no action here  */
      /* Allow the echoes to continue going out as normal.   */
      /* --------------------------------------------------- */

      ret_val = true;
   }
   else
   {
      /* A transaction request to a host has timed out. */
      /* ---------------------------------------------- */
      if (timer_reply_size != sizeof(buffer2))
      {
         ncvisa_log_message( 3, 2, "Error retrieving auth_tx from timer message" , "process_timeout_msg" , 0 ,"");
         ret_val = false;
      }
      else
      {
         ret_val = true;
         memset( &auth_tx, 0,       Auth_Tx_Size );
         memcpy( &auth_tx, buffer2, Auth_Tx_Size );

         /* update the transaction in the auth_tx table to TIMED OUT */
         auth_tx.TLF01_details.general_status = GS_TXN_TIMEDOUT;

         /* Update Shared Memory Counters */
         /* ----------------------------- */

         /* Get host state and counters */
         ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                        active_txns_ctr, current_state, prev_src_station );

         /* Decrement the active transactions counter field */
         tempint = atoi(active_txns_ctr) -1;
         if (tempint < 0)
         { /* if less than zero set it to zero */
            strcpy(active_txns_ctr,"0"); 
         }
         else
         {
            itoa(tempint, tempstr, 10);
            strcpy(active_txns_ctr,tempstr); 
         }

         /* Increment the number of times the host has failed */
         if ( MaxConsecTimeouts > 0 )
         {
            if (atoi(net_consec_tmouts_ctr) >= MaxConsecTimeouts)
            {
               /* Set Issuing state offline; leave Acquiring as is. */
               (void)set_host_state( vSTAY, vOFFLINE,
                                     net_consec_tmouts_ctr,
                                     active_txns_ctr );

               (void)send_network_status_to_monitor();
               update_flag = false;
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Maximum consecutive timeouts reached. Term Id:%s ,Merch Id: %s RRN: %s ",
               					auth_tx.TLF01_details.terminal_id,
               					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               ncvisa_log_message( 3, 3, Log_Bufr , "process_timeout_msg" , 0 ,auth_tx.TLF01_details.card_num);

            }
            else
            {
               tempint = atoi(net_consec_tmouts_ctr) + 1;
               itoa(tempint, tempstr, 10);
               strcpy(net_consec_tmouts_ctr,tempstr);
            }
         }

         /* Update shared memory and Monitor's display. */
         if ( update_flag == true )
         {
            WriteVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                            active_txns_ctr, current_state, prev_src_station );
         }


         /* Standin for Timed Out Transaction */
         /* --------------------------------- */

         /* Update transaction statistics. */
         strcpy( auth_tx.TLF01_details.response_code, TIMED_OUT );
         update_txn_stats( &Txn_Counts, &auth_tx, NA );

         /* Display time out message on Monitor.
          * This message is used to isolate problems to selected bins
          * or to certain merchants.
          */
        if(true == ncvisa_check_for_acq_refund_transaction(&auth_tx)||
           true == ncvisa_check_for_cadencie_acq_transaction(&auth_tx))
        {
        	/* Dont print for refund txn received by cadencie*/
		}
		else
		{
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Txn to Visa timed out.  Term Id:%s ,Merch Id: %s RRN: %s ",
         					auth_tx.TLF01_details.terminal_id,
         					auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         ncvisa_log_message( 3, 1, Log_Bufr , "process_timeout_msg" , 0 ,auth_tx.TLF01_details.card_num);
		}

         if ( auth_tx.TLF01_details.tx_key == AUTH_CARD_UPDATE )
         {
            issue_0302_response( TIMED_OUT, &auth_tx );
         }
#if BDOR_62_PREAUTH_VOID
		 else if(strncmp(auth_tx.TLF01_details.handler_queue, parvbh_que_name, 7) == 0)
         {
        	 return (true);
         }
#endif
         /* Is this the first time this txn has timed out? */
         else if ( auth_tx.advice_repeat_count[0] == 0x00 )
         {
            /* YES
             * ---
             * Reverse out the txn if it is an authorization.
             * Issue reversal repeat if it is a reversal.
             * Do nothing for network management transactions.
             */
            if ( AUTH_LOGON != auth_tx.TLF01_details.tx_key )
            {

         	   if(true == ncvisa_check_for_acq_refund_transaction(&auth_tx) ||
         		  true == ncvisa_check_for_cadencie_acq_transaction(&auth_tx))
         	   {
         		   auth_tx.TLF01_details.tx_key =
         		             match_request_to_response( auth_tx.TLF01_details.tx_key );

         		  ret_val = ncvisa_transfer_refund_response_msg_to_VISA2(&auth_tx);
         		   return ret_val;
         	   }
               /* Send time out message to terminal handler. */
               temp_tx_key = auth_tx.TLF01_details.tx_key;
               if ( 0 == strcmp(auth_tx.TLF01_details.voice_auth, "Y"))
               {
                  strcpy( auth_tx.TLF01_details.voice_auth_text,
                         "Trnsxn Timed Out");
               }
               perform_standin_processing(TIMED_OUT, GS_TXN_TIMEDOUT, &auth_tx);

               /* No need to reverse balance inquiries. */
               if ( AUTH_BALANCE_INQUIRY != temp_tx_key )
               {
                  /* Set repeat counter to 1 so next time, txn gets repeated. */
                  auth_tx.advice_repeat_count[0] = '1';

                  /* Set tx_key to indicate if we are repeating a reversal,
                   * or is this an authorization that needs to be reversed.
                   */
                  if ( AUTH_REVERSAL == temp_tx_key )
                  {
                     auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_REPEAT;
                     strcpy( auth_tx.TLF01_details.message_type, "0401" );
                  }
                  else
                  {
                     auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;

                     /* Set up the 'original' information for field 90. */
                     strcpy( auth_tx.TLF01_details.orig_sys_trace_num,
                             auth_tx.TLF01_details.sys_trace_audit_num);

                     strcpy( auth_tx.orig_acq_inst_id,
                             ncf01_i.acquirer_inst_id_code );

                     strcpy( auth_tx.TLF01_details.orig_message, "0100" );
                     strcpy( auth_tx.TLF01_details.message_type, "0400" );

                     /* Auth ID should be all zeroes for reversals caused
                      * by time outs.
                      */
                     memset( auth_tx.TLF01_details.auth_number, '0', 6 );
                  }

                  /* Send out the reversal request to the host. */
                  auth_tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
                  send_request_to_host( &auth_tx );
               }
            }
         }
         else
         {
            /* NO
             * --
             * We have tried to reverse out this transaction, but it timed out.
             * Keep trying, by sending reversal repeats, until we have tried
             * the maximum allowed times (configured in NCF01).
             */
            repeat_ctr = atoi( auth_tx.advice_repeat_count );
            if ( repeat_ctr <= atoi(ncf01_i.advice_repeat_counter) )
            {
               /* We have not reached max repeats yet.
                * Increment the counter and
                * send another reversal repeat.
                */
               repeat_ctr++;
               if ( repeat_ctr < 99 )
               {
                  sprintf( auth_tx.advice_repeat_count, "%02d", repeat_ctr );

                  /* Send a reversal repeat message to the host */
                  auth_tx.TLF01_details.tx_key = AUTH_REVERSAL_REPEAT;
                  strcpy(auth_tx.TLF01_details.message_type, "0401");
                  auth_tx.TLF01_details.general_status = GS_TXN_PERF_REQ;
                  send_request_to_host( &auth_tx );
               }
            }
         }
      }
   }
   return( ret_val );
 }  /* process_timeout_msg */

/*****************************************************************************

  Function:    send_response_to_term_app

  Description: This function will update the transaction counters (number of
               active transactions and number of timed out transactions) and 
               sends the message to the authorizer.
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure to be sent to the authorizer
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT send_response_to_term_app(pAUTH_TX p_auth_tx)
{
   pPTE_MSG    p_msg_term_handler_out = NULL_PTR;
   LONG        ret_code;
   CHAR	      Buffer[512]={0};
   CHAR	      ErrorMsg[512]={0};
   BYTE        buffer1[sizeof(AUTH_TX)];


   decrement_active_txn_ctr( true );

   /* build the message to be sent to the terminal handler */
   memset(buffer1, 0, sizeof(buffer1));
   memcpy(buffer1, p_auth_tx, Auth_Tx_Size);
   p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              authorizer_que_name,
                                              application_que_name,
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1),
                                              0 );

   ret_code = pteipc_send (p_msg_term_handler_out, authorizer_que_name);  
   if ( ret_code < 0 )
   {
	  pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "pteipc error sending to que %s: %s", authorizer_que_name, ErrorMsg );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
    		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvisa_log_message( 3, 3, Buffer , "send_response_to_term_app" , 0,p_auth_tx->TLF01_details.card_num );

      free(p_msg_term_handler_out);
      return(false);
   }
   else
   {
	  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
	  sprintf(Log_Bufr," Response sent successfully to authorizer Term Id:%s ,Merch Id: %s RRN: %s ",
			  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncvisa_log_message( 2, 1, Log_Bufr , "send_response_to_term_app" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   free(p_msg_term_handler_out);
   return(true);
} /* send_response_to_term_app */

/*****************************************************************************

  Function:    alter_transaction_data

  Description: This function will update the original message's auth_tx 
               structure with the data returned by the host.
  Author: 
      unknown
  Inputs:
      p_auth_tx  - the auth_tx structure from the host
      p_temp_auth_tx - the auth_tx structure from the original message.
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT alter_transaction_data(pAUTH_TX p_auth_tx, pAUTH_TX p_temp_auth_tx)
{
   INT    ret_value = true;
   INT    emv_request_len  = 0;
   INT    emv_response_len = 0,actuval_EMV_resp_len =0,temp_int;
   EMV    emv;
   BYTE 	temp_len[5]="";
   CHAR   tempstr[6] = "";

   if ( AUTH_INVALID == p_temp_auth_tx->TLF01_details.tx_key )
   {
      /* This transaction is not understood by the server and will be dropped */
      return( false );
   }

   if ( AUTH_BALANCE_INQUIRY_RESPONSE == p_temp_auth_tx->TLF01_details.tx_key )
   {
      /* Replace the amt in the auth_tx table with the amt in the response */
      strcpy(p_temp_auth_tx->TLF01_details.total_amount, p_auth_tx->TLF01_details.total_amount);
   }

   /* Copy balance info for Balance Inquiries and Cash Txns. */
   strcpy(p_temp_auth_tx->current_balance,   p_auth_tx->current_balance  );
   strcpy(p_temp_auth_tx->available_balance, p_auth_tx->available_balance);
   p_temp_auth_tx->current_balance_sign   =  p_auth_tx->current_balance_sign;
   p_temp_auth_tx->available_balance_sign =  p_auth_tx->available_balance_sign;

   /* Copy balances into these fields; nceqit looks for them here. */
   strcpy(p_temp_auth_tx->ACF01_details.cash_available, p_auth_tx->ACF01_details.cash_available);
   strcpy(p_temp_auth_tx->ACF01_details.cash_limit,     p_auth_tx->ACF01_details.cash_limit);

   p_temp_auth_tx->TLF01_details.general_status = GS_TXN_COMPLETED;

   strcpy(p_temp_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx->TLF01_details.retrieval_ref_num);

   if ( p_temp_auth_tx->TLF01_details.tx_key != AUTH_VOID_SALE_RESPONSE )
      strcpy(p_temp_auth_tx->TLF01_details.sys_trace_audit_num, p_auth_tx->TLF01_details.sys_trace_audit_num);

   /* Modified 08 Jan 2000 by MDB to address if logic */
   if (p_auth_tx->TLF01_details.auth_number [0] != 0x00)
       strcpy(p_temp_auth_tx->TLF01_details.auth_number, p_auth_tx->TLF01_details.auth_number );

   strcpy(p_temp_auth_tx->TLF01_details.response_code, p_auth_tx->TLF01_details.response_code);

   if (p_auth_tx->TLF01_details.date_yyyymmdd [0] != 0x00)
      strcpy(p_temp_auth_tx->TLF01_details.date_yyyymmdd, p_auth_tx->TLF01_details.date_yyyymmdd);

   if (p_auth_tx->TLF01_details.time_hhmmss [0] != 0x00)
      strcpy(p_temp_auth_tx->TLF01_details.time_hhmmss, p_auth_tx->TLF01_details.time_hhmmss);

   strcpy(p_temp_auth_tx->TLF01_details.resp_source_len, p_auth_tx->TLF01_details.resp_source_len);
   strcpy(p_temp_auth_tx->TLF01_details.resp_source, p_auth_tx->TLF01_details.resp_source);
   p_temp_auth_tx->TLF01_details.cvc = p_auth_tx->TLF01_details.cvc;

   // TF Phani .. Req.. (if test)
   if( strncmp(p_auth_tx->TLF01_details.handler_queue, "ncvis2", 6) != 0) // TF phani ..Field 44
   p_temp_auth_tx->TLF01_details.product_codes[17].quantity[0]=p_auth_tx->TLF01_details.product_codes[17].quantity[0];
   strcpy(p_temp_auth_tx->TLF01_details.mcard_banknet_len, p_auth_tx->TLF01_details.mcard_banknet_len);
   strcpy(p_temp_auth_tx->TLF01_details.mcard_banknet, p_auth_tx->TLF01_details.mcard_banknet);

   // Dont copy for cadencie 302 response. nirbhay
   if(!(strcmp(p_temp_auth_tx->TLF01_details.handler_queue, "ncvis2A") == 0 &&
		strcmp(p_temp_auth_tx->TLF01_details.message_type, "0302") == 0))
   {
	   strcpy(p_temp_auth_tx->TLF01_details.agent_id_len, p_auth_tx->TLF01_details.agent_id_len);
	   strcpy(p_temp_auth_tx->TLF01_details.agent_id, p_auth_tx->TLF01_details.agent_id);
   }

   strcpy(p_temp_auth_tx->TLF01_details.host_finish_time, p_auth_tx->TLF01_details.host_finish_time);

   if(0 != strncmp(p_temp_auth_tx->TLF01_details.handler_queue,"dcpiso",6))
   {
	   if ( p_auth_tx->TLF01_details.invoice_number[0] != 0x00 )
	      strcpy(p_temp_auth_tx->TLF01_details.invoice_number, p_auth_tx->TLF01_details.invoice_number);
   }

   /* From DE62 for logging into TLF01 and also for DE102 in nceqit. */
   p_temp_auth_tx->TLF01_details.visa_char_indicator = p_auth_tx->TLF01_details.visa_char_indicator;
   strcpy(p_temp_auth_tx->TLF01_details.visa_tran_id,         p_auth_tx->TLF01_details.visa_tran_id);
   strcpy(p_temp_auth_tx->TLF01_details.visa_validation_code, p_auth_tx->TLF01_details.visa_validation_code);

   if (0 == strncmp(p_temp_auth_tx->TLF01_details.handler_queue,"dcpiso",6) ||
   	   0 == strncmp(p_temp_auth_tx->TLF01_details.handler_queue,"VOICE",6))
   {
	   strcpy(p_temp_auth_tx->TLF01_details.product_codes[1].amount, p_auth_tx->TLF01_details.product_codes[1].amount);
	   strcpy(p_temp_auth_tx->TLF01_details.product_codes[5].quantity, p_auth_tx->TLF01_details.product_codes[5].quantity);
	   strcpy(p_temp_auth_tx->TLF01_details.product_codes[7].code, p_auth_tx->TLF01_details.product_codes[7].code);
	   strcpy(p_temp_auth_tx->TLF01_details.add_amounts, p_auth_tx->TLF01_details.add_amounts);
   }

   /* Get EMV response fields */
   strcpy( p_temp_auth_tx->EMV_details.issuer_auth_data, p_auth_tx->EMV_details.issuer_auth_data );
   strcpy( p_temp_auth_tx->EMV_details.issuer_script_1,  p_auth_tx->EMV_details.issuer_script_1  );
   strcpy( p_temp_auth_tx->EMV_details.issuer_script_2,  p_auth_tx->EMV_details.issuer_script_2  );

   /* Combine emv response data with emv request data. */
   memset( &emv, 0x00, sizeof(EMV) );

   emv_request_len  = genutil_bcd_to_int( p_temp_auth_tx->EMV_details.emv_block, 2 );
   //emv_response_len = genutil_bcd_to_int( p_auth_tx->EMV_details.emv_block, 2 );
   emv_response_len = p_auth_tx->EMV_details.emv_block[0];
   if(emv_response_len >255)
	emv_response_len = 255;
   else if(emv_response_len > 0 && emv_response_len < 255)
   {	
		   	  actuval_EMV_resp_len= emv_response_len -3;
			  if (actuval_EMV_resp_len < 80)
			  {
				   sprintf(temp_len,"%04d",actuval_EMV_resp_len);
				   //p_temp_auth_tx->EMV_details.future_field1[0] = 0x00;
				   //memset(&p_temp_auth_tx->EMV_details.future_field1[1],actuval_EMV_resp_len,1);
				   genutil_asc_to_bin(temp_len,p_temp_auth_tx->EMV_details.future_field1,&temp_int);
				   memcpy( p_temp_auth_tx->EMV_details.future_field1+2,
					 p_auth_tx->EMV_details.emv_block+4,
					actuval_EMV_resp_len );
	   
			  }else if (actuval_EMV_resp_len > 80 && actuval_EMV_resp_len < 240)
			  {
					sprintf(temp_len,"%04d",actuval_EMV_resp_len);
				   genutil_asc_to_bin(temp_len,p_temp_auth_tx->EMV_details.future_field1,&temp_int);

				   memcpy( p_temp_auth_tx->EMV_details.future_field1+2,
					 p_auth_tx->EMV_details.emv_block+4,
					sizeof(p_temp_auth_tx->EMV_details.future_field1)-3 );
				   memcpy( p_temp_auth_tx->EMV_details.future_field2,
					 p_auth_tx->EMV_details.emv_block+81,
					actuval_EMV_resp_len-(sizeof(p_temp_auth_tx->EMV_details.future_field1)-3));	
			  }else if(actuval_EMV_resp_len >240)
			  {
				 ncvisa_log_message( 3, 1, "EMV response is > 240 " , "alter_transaction_data" , 0 ,"");
			  }
	   
			  //copy response fileds from response
			 // if(emv_request_len > emv_response_len)
		   //  memcpy(p_temp_auth_tx->EMV_details.emv_block+emv_request_len+2,p_auth_tx->EMV_details.emv_block+2,emv_response_len-1);
   
   }
#if 0
   /* Store response EMV data ahead of the request EMV data. */
   memcpy( emv.emv_block+2,
           p_auth_tx->EMV_details.emv_block+2,
           emv_response_len );

   memcpy( emv.emv_block+2+emv_response_len,
           p_temp_auth_tx->EMV_details.emv_block+2,
           emv_request_len );

   emv_request_len += emv_response_len;
   emv_request_len = (emv_request_len > 255) ? 255 : emv_request_len;

   sprintf( tempstr, "%04d", emv_request_len );
   genutil_asc_to_bcd( tempstr, 4, emv.emv_block );

   /* Store combined EMV data into request Auth Tx structure. */
   memcpy(p_temp_auth_tx->EMV_details.emv_block,emv.emv_block,emv_request_len+2); 
#endif

   if ( AUTH_CARD_UPDATE_RESPONSE == p_temp_auth_tx->TLF01_details.tx_key )
   {
      /* Error Reason Code */
      strcpy( p_temp_auth_tx->TLF01_details.resp_source,
              p_auth_tx->TLF01_details.resp_source );

      /* Action Code */
      strcpy( p_temp_auth_tx->TLF01_details.type_of_data,
              p_auth_tx->TLF01_details.type_of_data );

      /* Region Coding */
      strcpy( p_temp_auth_tx->TLF01_details.product_codes[1].amount,
              p_auth_tx->TLF01_details.product_codes[1].amount );

      /* Date, Action */
      strcpy( p_temp_auth_tx->TLF01_details.product_codes[0].amount,
              p_auth_tx->TLF01_details.product_codes[0].amount );

      /* File Update Code */
      strcpy( p_temp_auth_tx->TLF01_details.processing_code,
              p_auth_tx->TLF01_details.processing_code );

      /* File Name */
      strcpy( p_temp_auth_tx->TLF01_details.kek, p_auth_tx->TLF01_details.kek );
   }

   if (0 == strncmp(p_temp_auth_tx->TLF01_details.handler_queue,"dcpiso",6) ||
	   0 == strncmp(p_temp_auth_tx->TLF01_details.handler_queue,"VOICE",6)   )
   {
	   /* OCT MANDT 2024: Visa 2, 3 Req */
	   strcpy( p_temp_auth_tx->TLF01_details.kek, p_auth_tx->TLF01_details.kek );
	   strcpy( p_temp_auth_tx->TLF01_details.product_codes[14].code, p_auth_tx->TLF01_details.product_codes[14].code );
   }


   /* Field 63 */
   // Dont copy for cadencie 302 response. nirbhay
   if(!(strcmp(p_temp_auth_tx->TLF01_details.handler_queue, "ncvis2A") == 0 &&
		strcmp(p_temp_auth_tx->TLF01_details.message_type, "0302") == 0))
   {
   strcpy(p_temp_auth_tx->TLF01_details.product_codes[10].code, p_auth_tx->TLF01_details.product_codes[10].code);
   strcpy(p_temp_auth_tx->TLF01_details.product_codes[11].code, p_auth_tx->TLF01_details.product_codes[11].code);
   strcpy(p_temp_auth_tx->TLF01_details.product_codes[12].code, p_auth_tx->TLF01_details.product_codes[12].code);
   }

   /* Field 116 */
   strcpy(p_temp_auth_tx->TLF01_details.dest_key, p_auth_tx->TLF01_details.dest_key);
   if(0 == strncmp(p_auth_tx->TLF01_details.terminal_id,VISA_DEFAULT_TID,VISA_DEFAULT_TID_LEN))
   {
	   memset(p_auth_tx->TLF01_details.terminal_id,0x00,VISA_DEFAULT_TID_LEN);
   }

   return(true);

}  /* alter_transaction_data */

/*****************************************************************************

  Function:   send_request_to_host 

  Description: This function will start the timer and send a request to the host
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx to be saved with the timer message.
                  Used to make the message to the host.
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT send_request_to_host( pAUTH_TX p_auth_tx )
{
   pPTE_MSG	p_msg_set_timer = NULL_PTR;
   pPTE_MSG p_msg_term_hdlr = NULL_PTR;
   CHAR     lookup_key[22];
   LONG     ret_code;
   CHAR     ErrorMsg[512]={0};
   CHAR     Buffer[512]={0};
   BYTE     buffer1[sizeof(AUTH_TX)];
   WORD     timer_length;
   pPTE_MSG p_msg_host_out  = NULL_PTR;
   CHAR     gmt_time[20];
   INT      buf_temp_len=0;

   if (GS_TXN_DROPPED == p_auth_tx->TLF01_details.general_status)
   {
      p_msg_term_hdlr = ptemsg_build_msg( MT_AUTH_RESPONSE, 
                                          ST1_NONE,
                                          ST2_NONE,
                                          authorizer_que_name,
                                          application_que_name,
                                          (pBYTE)p_auth_tx,
                                          Auth_Tx_Size,
                                          0 );

      if(NULL_PTR == p_msg_term_hdlr)
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Insufficient Memory to build response to txcntl Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncvisa_log_message( 3, 2, Buffer , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
         return(false);
      } 
      ret_code = pteipc_send(p_msg_term_hdlr, authorizer_que_name); 
      if(ret_code < 0)
      {
         pteipc_get_errormsg(ret_code, ErrorMsg);
         sprintf( Buffer, "failed transmit to authorizer: %s. %s",
                  authorizer_que_name, ErrorMsg);
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvisa_log_message( 3, 3, Buffer , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
         free(p_msg_term_hdlr);
         return(false);
      }
      free(p_msg_term_hdlr);
      return(true);
   }

   /* Build PTEMSG */
   if (GS_TXN_NEW == p_auth_tx->TLF01_details.general_status)
      p_auth_tx->TLF01_details.general_status = GS_TXN_PERF_REQ;

   /* Current Greenwich Mean Time for the transmission-to-host time. */
   if ( 0 == strcmp( NULL_STR,
                     p_auth_tx->TLF01_details.transmission_timestamp ) )
   {
      pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
      strncpy( p_auth_tx->TLF01_details.transmission_timestamp,
               &gmt_time[4], 10 );  /* MMDDhhmmss */
   }

   /* Fill ReqRsp_Buffer from HOST_MESSAGE*/
   if ( p_auth_tx->TLF01_details.general_status == GS_TXN_PERF_REQ )
   {
      build_host_structure(p_auth_tx);
      if (Build_Host_Request( p_auth_tx ))
      {
         /* This must be set prior to setting the timer; it gets saved there. */
         ptetime_get_strtime(p_auth_tx->TLF01_details.host_start_time);

         /* Since there are no responses for ATM Confirm
          * messages, do not set the timer for them.
          */
         if ( p_auth_tx->TLF01_details.tx_key != AUTH_ATM_CONFIRM )
         {
            /* Create the lookup key for the timer. */
            memset(lookup_key, 0x00, sizeof(lookup_key));
            strcpy(lookup_key, AppName);
            strcat(lookup_key, p_auth_tx->TLF01_details.terminal_id);

            if ( p_auth_tx->TLF01_details.tx_key == AUTH_VOID_SALE )
               strcat(lookup_key, p_auth_tx->TLF01_details.orig_sys_trace_num);
            else
               strcat(lookup_key, p_auth_tx->TLF01_details.sys_trace_audit_num);

            /* Build Start Timer message to send to Host Data Server */		
            memcpy(buffer1,p_auth_tx, Auth_Tx_Size);
            timer_length = atoi(ncf01_i.request_timer);
            p_msg_set_timer = ptetimer_bld_start_timer_req(lookup_key,
                            p_auth_tx->TLF01_details.primary_key.transaction_id,
                            p_auth_tx->TLF01_details.terminal_id,
                            timerds_que_name,
                            application_que_name,
                            timer_length,
                            0,
                            (pBYTE)&buffer1,  
                            sizeof(buffer1)); 

            if ( NULL_PTR == p_msg_set_timer )
            {
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Insufficient Memory to build start timer message Term Id:%s ,Merch Id: %s RRN: %s ",
               		   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               ncvisa_log_message( 3, 2, Log_Bufr , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
               return(false);
            }

            /* We can chain the request_timer message with set & clear
             * echo timer request to timerds 
             */
            if(false == chain_echo_host(echo_lookup_key, echo_timer_length,application_que_name,p_msg_set_timer))
            {
               ncvisa_log_message( 3, 3, "Error chaining the timer request with clear & set the echo_timer" , "send_request_to_host" , 0 ,"");
            }
         }
         else
         {
            /* For ATM Confirms, clear and set the echo timer.  */
            if ( echo_timer_length > 0 )
            {
               (void)clear_echo_timer(echo_lookup_key,(pPTE_MSG *)NULL_PTR,
                                     application_que_name, SEND_TO_TIMERDS );

               /* Set the echo timer. */
               if ( false == set_echo_timer(echo_lookup_key, echo_timer_length,
                                   (pPTE_MSG *)NULL_PTR,application_que_name,
                                   SEND_TO_TIMERDS) )
               {
                  ncvisa_log_message( 3, 2, "Error setting the echo_timer" , "send_request_to_host" , 0 ,"");
               }
            }
         }
         Build_Host_Header(p_auth_tx);
         /* Visa requires a special header prepended to its messages. */
		 if(true == ncvisa_check_for_acq_refund_transaction(p_auth_tx) ||
			true == ncvisa_check_for_cadencie_acq_transaction(p_auth_tx))
		 {
			buf_temp_len = atoi(p_auth_tx->resp_data_len);
			if(buf_temp_len < COMM_BUF_SZE && buf_temp_len > 0)
			{
				buflen = buf_temp_len;
				memcpy(ReqRsp_Buffer,p_auth_tx->resp_buffer,buflen);
			}
		 }
         p_msg_host_out = ptemsg_build_msg(MT_OUTGOING, 
                                           ST1_NONE,
                                           ST2_NONE,
                                          "dialogA",
                                           application_que_name,
                                           ReqRsp_Buffer,
                                           buflen,
                                           0 );
         ptemsg_set_orig_comms_info( p_msg_host_out,
                                     p_auth_tx->TLF01_details.tpdu_id );
         if ( p_msg_host_out == NULL_PTR )
         {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            ncvisa_log_message( 3, 2, Log_Bufr , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
             return(false);
         }
		 
         ret_code = pteipc_send(p_msg_host_out, "dialogA");
         if( ret_code < 0 )
         {
            pteipc_get_errormsg( ret_code, ErrorMsg );
            sprintf( Buffer, "error on pteipc_send to que %s: %s",
                    "dialogA", ErrorMsg );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(Buffer,Log_Bufr);
            ncvisa_log_message( 3, 3, Buffer , "send_request_to_host" , 0 ,p_auth_tx->TLF01_details.card_num);
            free( p_msg_host_out );
            return(false);
         }
         else
         {
            /* Update transaction statistics */
            update_txn_stats( &Txn_Counts, p_auth_tx, SENT );
    
            /* Log transaction to trace file. */
            write_to_txn_file();
         }
         free( p_msg_host_out );
      }
      else
      {
         return(false);
      }
   }
   return(true);
}

/*****************************************************************************

  Function:    verify_host_app_status

  Description: This function will check the active transaction counter
               and the host state (both stored in shared memory) to 
               determine whether the host is "up"
  Author: 
      unknown
  Inputs:
      p_auth_tx - used to determine whether this transaction is a reversal
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
   Modified by:
      BGable      6/30/98
******************************************************************************/
INT  verify_host_app_status(pAUTH_TX p_auth_tx)
{
   INT   tempint;
   CHAR  tempstr[14];
   CHAR  err_buf[512] = {0};
   BYTE  net_consec_tmouts_ctr[4];
   BYTE  active_txns_ctr[4];
   BYTE  current_state[2];
   BYTE  prev_src_station[4];
   INT   ret_code = true;


   /* Get host state and counters */
   ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                  active_txns_ctr, current_state, prev_src_station );

   if((current_state[0] != cBOTH_ONLINE) && (current_state[0] != cISSON_ACQOFF))
      ret_code = false;

   /* Check number of consecutive time outs.
    * If too many, issue a warning.
    */
   if ( (MaxConsecTimeouts > 0) && (ret_code == true) )
   {
      if (atoi(net_consec_tmouts_ctr) > MaxConsecTimeouts )
      {
         strcpy( err_buf,
             "Maximum consecutive timeouts reached.");

         (void)set_host_state( vSTAY, vOFFLINE,
                               net_consec_tmouts_ctr,
                               active_txns_ctr );
         ret_code = false;

         /* Update Monitor's display. */
         (void)send_network_status_to_monitor();

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         	     p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(err_buf,Log_Bufr);
         ncvisa_log_message( 1, 2, err_buf , "verify_host_app_status" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }

   /* Check number of active transactions.
    * If too many, issue a warning.
    */
   if ( ret_code == true )
   {
      tempint = atoi(active_txns_ctr) + 1;
      if ( MaxActiveTxns > 0 )
      {
         if( tempint >= MaxActiveTxns )
         {
            strcpy( err_buf,
                   "Maximum active txns reached. Continuing to process." );

            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",p_auth_tx->TLF01_details.terminal_id,
            					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(err_buf,Log_Bufr);
            ncvisa_log_message( 1, 2, err_buf , "verify_host_app_status" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }

      /* Increment the number of active transactions for this host */
      if (p_auth_tx->TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE)
      {
         if ( tempint > 999 )
            tempint = 1;
         memset(&tempstr, 0x00, sizeof(tempstr));
         itoa(tempint,tempstr,10);
         strcpy(active_txns_ctr, tempstr ); 
         WriteVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                         active_txns_ctr, current_state, prev_src_station );
      }
   }

   return( ret_code );
}; /* verify_host_app_status */

/*****************************************************************************

  Function:    perform_host_logon_request

  Description: This function will send a logon request to the host to ensure
               that the host is up and running.
  Author: 
      unknown
  Inputs:
      request_type - the type of request
      source       - indicates the source of the logon request
      p_auth_tx    - the auth_tx structure
      subtype2     - Indicates Acquirer or Issuer
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT perform_host_logon_request(CHAR     request_type[],
                               INT      source,
                               pAUTH_TX p_auth_tx,
                               BYTE     subtype2 )
{
   CHAR   tempstr[10] ="";
   BYTE   net_consec_tmouts_ctr[4];
   BYTE   active_txns_ctr[4];
   BYTE   current_state[2];
   BYTE   prev_src_station[4];
   BYTE   acq_state[2];
   BYTE   iss_state[2];
   INT    ret_val = true;

   memset( &vLogon_Resp_Indicator, 0x00, sizeof(vLogon_Resp_Indicator));

   /* Get host state and counters */
   ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                  active_txns_ctr, current_state, prev_src_station );

   switch (source)
	{
		case SOURCE_IS_FSVISA:
		{
         if ( 0 != strcmp( current_state, ONLINE ) )
			{
            /* Issue a LOGON request to the host. */
				p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;
            strcpy(vLogon_NMI_Code, LOGON_NMI_LOGON);
			}
			break;
		} /* end case SOURCE_IS_FSVISA */

		case SOURCE_IS_STATE:
		{
         if ( 0 == strcmp( current_state, ONLINE ) )
			{
            /* Send a LOGON ECHO request to the host. */
				p_auth_tx->TLF01_details.tx_key = AUTH_LOGON;
				strcpy(vLogon_NMI_Code, LOGON_NMI_ECHO_TEST);
				send_request_to_host(p_auth_tx);
			}
			break;
		} /* end case SOURCE_IS_STATE */

      case SOURCE_IS_OPERATOR:
      {
         memset( acq_state, 0x00, sizeof(acq_state) );
         memset( iss_state, 0x00, sizeof(iss_state) );
         ret_val = perform_op_net_mngmnt_request( request_type, p_auth_tx,
                                                  current_state, subtype2,
                                                  acq_state, iss_state );

         if ( ret_val == true )
         {
            /* Update shared memory. */
            ret_val = set_host_state( acq_state[0],
                                      iss_state[0],
                                      net_consec_tmouts_ctr,
                                      active_txns_ctr );
            if ( ret_val == false )
            {
               ncvisa_log_message( 3, 3, Log_Bufr , "perform_host_logon_request" , 0 ,"");
            }
            else
               (void)send_network_status_to_monitor();
         }
         ret_val = true;
         break;
      }

		case SOURCE_IS_HOST:
		{
         strcpy( p_auth_tx->TLF01_details.response_code, APPROVED );
         p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
			if (strcmp(LOGON_NMI_START , request_type) == 0)
			{
            /* Received a LOGON request from the host. */
            strcpy(vLogon_NMI_Code, LOGON_NMI_START);
			}
			else
			{
            /* Received network request from host. */
            strcpy(vLogon_NMI_Code, LOGON_NMI_ECHO_TEST);
            if (0 == strcmp(request_type,LOGON_NMI_LOGON))
               strcpy(vLogon_Resp_Indicator, LOGON_RESP_INDICATOR);
			}

         perform_op_net_mngmnt_response(request_type, p_auth_tx);
			break;
		} /* end case SOURCE_IS_HOST */

   } /* end switch statement */

	return (ret_val);

};  /* perform_host_logon_request */

/*****************************************************************************

  Function:    send_network_status_to_monitor

  Description: This function will send the network status to the monitor.  This
               includes the acquiring and the issuing status.
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT send_network_status_to_monitor(void)
{
   BYTE        net_consec_tmouts_ctr[4];
   BYTE        active_txns_ctr[4];
   BYTE        current_state[2];
   BYTE        prev_src_station[4];
   NCF01       ncf01;
   pPTE_MSG    p_msg_ins   = NULL_PTR;
   LONG        ret_code;
   CHAR        err_mesg[512] = {0};
   CHAR        Buff[512]={0};
   CHAR        ConnectionId[MAX_ORIG_INFO_SIZE+1] = {0} ;
   XINT        ReturnCode = 0 ;
   INT         nIndex = 0 ;


   /* Get host state and counters */
   ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                  active_txns_ctr, current_state, prev_src_station );

   memset( &ncf01, 0, Ncf01_Size );
   strcpy( ncf01.primary_key.network_id,    AppName      );
   strcpy( ncf01.primary_key.network_type, "A"           );
   strcpy( ncf01.status,                   current_state );

   p_msg_ins = ptemsg_build_msg( MT_SYS_MONITOR,
                                 ST1_NETWORK_UPDATE, 
                                 ST2_NONE,
                                 applnk_que_name, 
                                 application_que_name,
                                 (pBYTE)&ncf01,
                                 Ncf01_Size, 
                                 0 );
   if ( NULL_PTR == p_msg_ins )
   {
      ncvisa_log_message( 3, 2, "Insufficient Memory to create network query message" , "send_network_status_to_monitor" , 0 ,"");
      return( false );
   }

	memset (ConnectionId, 0, sizeof (ConnectionId)) ;
	ReturnCode = ReadMemMonitorTable (ConnectionId) ;
	if (ConnectionId[0] != 0)
   {
      ptemsg_set_orig_comms_info   (p_msg_ins, ConnectionId) ;
      /* Send message to get host specific data from data server. */
      ret_code = pteipc_send( p_msg_ins, applnk_que_name );
      if ( 0 > ret_code )
      {
         pteipc_get_errormsg( ret_code, err_mesg );
         sprintf(Buff,"Failed transmit to que: %s. %s",applnk_que_name,err_mesg);
         ncvisa_log_message( 3, 3, Buff , "send_network_status_to_monitor" , 0 ,"");
         free( p_msg_ins );
         return( false );
      }
   }
   free( p_msg_ins );
/*
 * No need to send out status of Issuer NC.
 * A single status gives state of Acquirer and Issuer.
 */
#ifdef WIN32
   Sleep (500);
#else
   usleep (500000);
#endif

   memset( &ncf01, 0, Ncf01_Size );
   strcpy( ncf01.primary_key.network_id,    AppName      );
   strcpy( ncf01.primary_key.network_type, "I"           );
   strcpy( ncf01.status,                   current_state );

   p_msg_ins = ptemsg_build_msg( MT_SYS_MONITOR,
                                 ST1_NETWORK_UPDATE, 
                                 ST2_NONE,
                                 applnk_que_name, 
                                 application_que_name,
                                 (pBYTE)&ncf01,
                                 Ncf01_Size, 
                                 0 );
   if ( NULL_PTR == p_msg_ins )
   {
      ncvisa_log_message( 3, 2, "Insufficient Memory to create network query message" , "send_network_status_to_monitor" , 0 ,"");
      return( false );
   }

	memset (ConnectionId, 0, sizeof (ConnectionId)) ;
	ReturnCode = ReadMemMonitorTable (ConnectionId) ;
	if (ConnectionId[0] != 0)
   {
      ptemsg_set_orig_comms_info   (p_msg_ins, ConnectionId) ;
      /* Send message to get host specific data from data server. */
      ret_code = pteipc_send( p_msg_ins, applnk_que_name );
      if ( 0 > ret_code )
      {
         pteipc_get_errormsg( ret_code, err_mesg );
         sprintf(Buff,"Failed transmit to que: %s. %s",applnk_que_name,err_mesg);
         ncvisa_log_message( 3, 3, Buff , "send_network_status_to_monitor" , 0 ,"");
         free( p_msg_ins );
         return( false );
      }
   }
   free( p_msg_ins );
   return (true);

};  /* send_network_status_to_monitor */

/******************************************************************************
 *
 *  NAME:         PERFORM_OP_NET_MNGMNT_REQUEST
 *
 *  DESCRIPTION:  This procedure prepares a log on or log off request.
 *
 *  INPUTS:       request_type - Transaction type of request transaction
 *                p_auth_tx    - Transaction record
 *                host_state   - Acquiring and Issuing host state
 *                subtype2     - Indicates Acquirer or Issuer
 *
 *  OUTPUTS:      acq_state - New value for Acquirer state
 *                iss_state - New value for Issuer state
 *
 *  RTRN VALUE:   True will update shared memory, false will not.
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT perform_op_net_mngmnt_request( CHAR     request_type[],
                                   pAUTH_TX p_auth_tx,
                                   pBYTE    host_state,
                                   BYTE     subtype2,
                                   pBYTE    acq_state,
                                   pBYTE    iss_state )
{
   INT      ret_val = true;
   BYTE     state;
   CHAR     gmt_time[15];
   AUTH_TX  auth_tx;
   pPTE_MSG p_msg_set_timer = NULL_PTR;
   WORD     timer_length;

   memset( &auth_tx, 0x00, Auth_Tx_Size );
   strncpy(auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id,
           sizeof(ncf01_i.tpdu_id));
   auth_tx.TLF01_details.tx_key = AUTH_LOGON;

   /* Current Time for the transmission-to-host time */
   memset(gmt_time, 0, sizeof(gmt_time));
   ptetime_get_strtime (gmt_time);
   memcpy (auth_tx.TLF01_details.host_start_time, gmt_time, 14);
   Rj_with_lead_zeros(gmt_time, sizeof(gmt_time)-1);

   /* Use the time hhmmss for sys_trace_audit_num */
   memcpy (auth_tx.TLF01_details.sys_trace_audit_num, gmt_time + 7, 3); 
   strncat(auth_tx.TLF01_details.sys_trace_audit_num, gmt_time + 11,3);

   /* Create RRN and store it in the auth_tx block,format YDDDhmmssjjj */
   if ( 0 == strcmp( NULL_STR, auth_tx.TLF01_details.retrieval_ref_num))
   {
      create_rrn( auth_tx.TLF01_details.retrieval_ref_num, &auth_tx);
   }

   /* Determine type of request and determine if it is needed. */
   memset( vLogon_NMI_Code, 0x00, sizeof(vLogon_NMI_Code) );
   state = host_state[0];
   if (0 == strcmp(request_type, LOGON_REQUEST))
   {
      /*+--------+
        | LOG ON |
        +--------+*/
      if ( subtype2 == ST2_ACQUIRER )
      {
         if ( (state == cBOTH_ONLINE) || (state == cISSOFF_ACQON) )
         {
            /* Acquiring state is Online */
            ret_val = false;
            ncvisa_log_message( 1, 1, "Already online. Log ON request dropped." , "perform_op_net_mngmnt_request" , 0 , "" );
         }
         else
         {
            /* Acquiring state is not Online */
            acq_state[0] = vONLINE;
            iss_state[0] = vSTAY;
            strcpy( vLogon_NMI_Code, LOGON_NMI_LOGON );
            ncvisa_log_message( 1, 1, "Sending LOGON Request to host." , "perform_op_net_mngmnt_request" , 0 ,"");

        }
      }
      else
      {
         if ( (state == cBOTH_ONLINE) || (state == cISSON_ACQOFF) )
         {
            /* Issuing state is Online */
            ret_val =false;
            ncvisa_log_message( 1, 1, "Already online. Log ON request dropped." , "perform_op_net_mngmnt_request" , 0 ,"");
         }
         else
         {
            /* Issuing state is not Online */
            strcpy( vLogon_NMI_Code, LOGON_NMI_LOGON );
            iss_state[0] = vSTAY;
            acq_state[0] = vSTAY;
            ncvisa_log_message( 1, 1, "Sending LOGON Request to host." , "perform_op_net_mngmnt_request" , 0 ,"");

         }
      }
   }
   else if (0 == strcmp(request_type, LOGOFF_REQUEST))
   {
      /*+---------+
        | LOG OFF |
        +---------+*/
      if ( subtype2 == ST2_ACQUIRER )
      {
         if ( (state == cBOTH_OFFLINE) || (state == cISSON_ACQOFF) )
         {
            /* Acquiring state is Ofline */
            ret_val = false;
            ncvisa_log_message( 1, 1, "Not online. Log OFF request dropped." , "perform_op_net_mngmnt_request" , 0 ,"");
         }
         else
         {
            /* Acquiring state is not Offline */
            strcpy( vLogon_NMI_Code, LOGON_NMI_LOGOFF );
            acq_state[0] = vOFFLINE;
            iss_state[0] = vSTAY;
            ncvisa_log_message( 1, 1, "Sending LOGOFF Request to host." , "perform_op_net_mngmnt_request" , 0 ,"");
         }
      }
      else
      {
         if ( (state == cBOTH_OFFLINE) || (state == cISSOFF_ACQON) )
         {
            /* Issuing state is Online */
            ret_val = false;
            ncvisa_log_message( 1, 1, "Not online. Log OFF request dropped." , "perform_op_net_mngmnt_request" , 0 ,"");
         }
         else
         {
            /* Issuing state is not Online */
            strcpy( vLogon_NMI_Code, LOGON_NMI_LOGOFF );
            iss_state[0] = vOFFLINE;
            acq_state[0] = vSTAY;
            ncvisa_log_message( 1, 1, "Sending LOGOFF Request to host." , "perform_op_net_mngmnt_request" , 0 ,"");
         }
      }
   }

/* CR to send SIGN ON and OFF request to Host
Girija Y TF, March 30 2009 */

   else if(0 == strcmp(request_type, SIGNON_ADVICE_RETRIEVAL))
   {
      /*+-------------------------+
        | Sign On Advice Retrieval|
        +-------------------------+*/
	   ret_val = false;
	  if ( (state == cBOTH_ONLINE) || (state == cISSON_ACQOFF) )
         {
            /* Issuing state is Online */
            strcpy( vLogon_NMI_Code, LOGON_NMI_SIGNON );
         }
    }

   else if(0 == strcmp(request_type, SIGNOFF_ADVICE_RETRIEVAL))
   {
      /*+--------------------------+
        | Sign Off Advice Retrieval|
        +--------------------------+*/
	   ret_val = false;
	  if ( (state == cBOTH_ONLINE) || (state == cISSON_ACQOFF) )
         {
            /* Issuing state is Online */
            strcpy( vLogon_NMI_Code, LOGON_NMI_SIGNOFF );
         }
    }

    else 
   {
      /*+------+
        | ECHO |
        +------+*/
      ret_val = false;
      if ( subtype2 == ST2_ACQUIRER )
      {
         if ( (state == cBOTH_ONLINE) || (state == cISSOFF_ACQON) )
         {
            /* Acquiring state is Online */
            strcpy( vLogon_NMI_Code, LOGON_NMI_ECHO_TEST );
         }
      }
      else
      {
         if ( (state == cBOTH_ONLINE) || (state == cISSON_ACQOFF) )
         {
            /* Issuing state is Online */
            strcpy( vLogon_NMI_Code, LOGON_NMI_ECHO_TEST );
         }
      }

      if ( vLogon_NMI_Code[0] != 0x00 )
      {
         /* Set Request Timer for Echoes. */

         /* Build Start Timer message to send to Host Data Server */		
         timer_length = atoi(ncf01_i.request_timer);
         p_msg_set_timer = ptetimer_bld_start_timer_req( echo_request_timer_key,
                            p_auth_tx->TLF01_details.primary_key.transaction_id,
                            p_auth_tx->TLF01_details.terminal_id,
                            timerds_que_name,
                            application_que_name,
                            timer_length,
                            0, "0", 1 ); 

         if ( NULL_PTR == p_msg_set_timer )
         {
            ncvisa_log_message( 3, 2, "Insufficient Memory to build start timer message" , "perform_op_net_mngmnt_request" , 0 ,"");
            memset( vLogon_NMI_Code, 0x00, sizeof(vLogon_NMI_Code) );
         }
         else
         {
            /* We can chain the request_timer message with set & clear
             * echo timer request to timerds 
             */
            if(false == chain_echo_host( echo_lookup_key, echo_timer_length,
                                         application_que_name, p_msg_set_timer))
            {
               memset( vLogon_NMI_Code, 0x00, sizeof(vLogon_NMI_Code) );
               ncvisa_log_message( 3, 3, "Error chaining the timer request with clear & set the echo_timer" , "perform_op_net_mngmnt_request" , 0 ,"");
            }
         }
      }
   }

   if ( vLogon_NMI_Code[0] != 0x00 )
   {
      /* Send request to host. */
      if ( false == hhutil_send_net_mngmnt_request(&auth_tx) )
      {
         /* Failed to send request; do not change host state. */
         ret_val = false;
      }
   }

   return( ret_val );
} /* perform_op_net_mngmnt_request */

/*****************************************************************************

  Function:    perform_op_net_mngmnt_response

  Description: This function will 
  Author: 
      Jimmy Hapitas
  Inputs:
      nmi_code  - the nmi code
      p_auth_tx - the auth_tx structure
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT perform_op_net_mngmnt_response(char nmi_code[], pAUTH_TX p_auth_tx)
{
   INT    ret_val;
   INT    send_response = true;
   INT    host_status_updated = false;
   BYTE   net_consec_tmouts_ctr[4];
   BYTE   active_txns_ctr[4];
   BYTE   current_state[2];
   BYTE   prev_src_station[4];

   p_auth_tx->TLF01_details.tx_key = AUTH_LOGON_RESPONSE;
   strcpy(p_auth_tx->TLF01_details.tpdu_id, ncf01_i.tpdu_id);
   ptetime_get_strtime(p_auth_tx->TLF01_details.host_start_time);

   /* Get host state and counters */
   ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                  active_txns_ctr, current_state, prev_src_station );

   /* Display messages to Monitor for LOGONs and LOGOFFs, but not ECHOs. */
   if (0 == strcmp(nmi_code, LOGON_NMI_LOGON))
   {
      ncvisa_log_message( 1, 1, "Received LOGON request from host" , "perform_op_net_mngmnt_response" , 0 ,"");

      ret_val = set_host_state( vSTAY, vONLINE,
                                net_consec_tmouts_ctr,
                                active_txns_ctr );
      if ( ret_val == true )
         (void)send_network_status_to_monitor();

      if ( (current_state[0] == cBOTH_ONLINE) ||
           (current_state[0] == cISSOFF_ACQON) )
      {
         /* Acquiring state must be online to process 0800's from the host. */
         strcpy(vLogon_Resp_Indicator, LOGON_RESP_INDICATOR);
      }
      else
         send_response = false;
   }
   else if (0 == strcmp(nmi_code, LOGON_NMI_LOGOFF))
   {
      ncvisa_log_message( 1, 1, "Received LOGOFF request from host" , "perform_op_net_mngmnt_response" , 0 ,"");
      ret_val = set_host_state( vSTAY, vOFFLINE,
                                net_consec_tmouts_ctr,
                                active_txns_ctr );
      if ( ret_val == true )
         (void)send_network_status_to_monitor();

      if ( (current_state[0] != cBOTH_ONLINE) &&
           (current_state[0] != cISSOFF_ACQON) )
      {
         /* Acquiring state must be online to process 0800's from the host. */
         send_response = false;
      }
   }
   else
   {
      if ( (current_state[0] != cBOTH_ONLINE) &&
           (current_state[0] != cISSOFF_ACQON) )
      {
         /* Acquiring state must be online to process 0800's from the host. */
         send_response = false;
      }
   }

   strcpy(vLogon_NMI_Code, nmi_code);

   if ( send_response == true )
   {
      /* Send the response */
      strcpy( p_auth_tx->TLF01_details.response_code, APPROVED );
      ret_val = hhutil_send_net_mngmnt_response(p_auth_tx);
   }

   return( ret_val );
} /* perform_op_net_mngmnt_response */
   
/*****************************************************************************

  Function:    

  Description: This function will match the transaction type of a request message 
               to the matching response message.
  Author: 
      unknown
  Inputs:
      request - the transaction type of the incoming request message
  Outputs:
      None
  Return values:
      BYTE - the response transaction type
  Modified by:
      BGable      6/30/98
******************************************************************************/
BYTE match_request_to_response(BYTE request)
{

   if ( request >= AUTH_INVALID )
      return( request );

   switch (request)
   {
      case AUTH_SALE:
	     return(AUTH_SALE_RESPONSE); 
        break;

      case AUTH_ADVICE:
	     return(AUTH_ADVICE_RESPONSE); 
        break;

      case AUTH_REFUND:
	     return(AUTH_REFUND_RESPONSE);  
        break;

      case AUTH_CASH_ADVANCE:
	     return(AUTH_CASH_ADVANCE_RESPONSE); 
        break;

      case AUTH_CASH_ADVANCE_AUTHORIZATION:
         return( AUTH_CASH_ADVANCE_AUTHORIZATION_RESPONSE ); 
      break;

      case AUTH_PAYMENT: 
	     return(AUTH_PAYMENT_RESPONSE); 
        break;


      case AUTH_OFFLINE_SALE: 
	     return(AUTH_OFFLINE_SALE_RESPONSE); 
        break;


      case AUTH_OFFLINE_REFUND:
	     return(AUTH_OFFLINE_REFUND_RESPONSE); 
        break;


      case AUTH_SALE_ADJUSTMENT:
	     return(AUTH_SALE_ADJUSTMENT_RESPONSE); 
        break;


      case AUTH_REFUND_ADJUSTMENT:
	     return(AUTH_REFUND_ADJUSTMENT_RESPONSE); 
        break;


	  case AUTH_PAYMENT_ADJUSTMENT:
	     return(AUTH_PAYMENT_ADJUSTMENT_RESPONSE); 
        break;


      case AUTH_PRE_AUTHORIZATION:
	     return(AUTH_PRE_AUTHORIZATION_RESPONSE); 
        break;


      case AUTH_AUTHORIZATION:
	     return(AUTH_AUTHORIZATION_RESPONSE); 
        break;


      case AUTH_BALANCE_INQUIRY:
	     return(AUTH_BALANCE_INQUIRY_RESPONSE); 
        break;


      case AUTH_VOID_SALE:
	     return(AUTH_VOID_SALE_RESPONSE); 
        break;


      case AUTH_VOID_REFUND: 
	     return(AUTH_VOID_REFUND_RESPONSE);
        break;


      case AUTH_REVERSAL:
	     return(AUTH_REVERSAL_RESPONSE); 
        break;

      case AUTH_REVERSAL_ADVICE:
	     return(AUTH_REVERSAL_ADVICE_RESPONSE); 
        break;

      case AUTH_REVERSAL_REPEAT:
	     return(AUTH_REVERSAL_REPEAT_RESPONSE); 
        break;


      case AUTH_ADMIN_ADVICE:
	     return(AUTH_ADMIN_ADVICE_RESPONSE); 
        break;


      case AUTH_LOGON:
	     return(AUTH_LOGON_RESPONSE); 
        break;


      case AUTH_TEST:
	     return(AUTH_TEST_RESPONSE); 
        break;

      case AUTH_CARD_VERIFICATION:
        return(AUTH_CARD_VERIFICATION_RESPONSE);
        break;

#if BDOR_62_PREAUTH_VOID
      case AUTH_VOID_PRE_AUTHORIZATION:
        return(AUTH_VOID_PRE_AUTHORIZATION_RESPONSE);
        break;
#endif

	  default:
         return( request+100 );
        break;

   }
}; /* match_request_to_response */

/*****************************************************************************

  Function:    perform_standin_processing

  Description: This function will perform the standin processing for those
               transactions that do not require host authorization (offline trans)
  Author: 
      unknown
  Inputs:
      rsp_code   - the response code for the message
      gen_status - the general status for the message
      p_auth_tx  - the auth_tx structure for the message
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void perform_standin_processing(CHAR rsp_code[3], BYTE gen_status,
                                pAUTH_TX p_auth_tx )
{
   pPTE_MSG	p_msg_insert_auth       = NULL_PTR;
   pPTE_MSG p_msg_delete_auth       = NULL_PTR;
   pPTE_MSG p_msg_term_handler_out	= NULL_PTR;

   LONG  ret_code;
   CHAR  Buffer[512] = {0};
   CHAR  ErrorMsg[512] = {0};
   BYTE  buffer1[sizeof(AUTH_TX)];


   if ( AUTH_REVERSAL_RESPONSE != p_auth_tx->TLF01_details.tx_key )
   {
      if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.auth_number ) )
      {
         strcpy(p_auth_tx->TLF01_details.auth_number,
                p_auth_tx->TLF01_details.time_hhmmss);
      }
   }
   /* create response code and store it in the auth_tx block  */
   strcpy(p_auth_tx->TLF01_details.response_code, rsp_code);
   
   if(strcmp(rsp_code,"00")== 0)
   strcpy(p_auth_tx->TLF01_details.response_text, "APPROVED_STAND_IN");
   p_auth_tx->TLF01_details.tx_key =
                    match_request_to_response(p_auth_tx->TLF01_details.tx_key);

   /* change status to completed and store it in the auth_tx block */ 
   p_auth_tx->TLF01_details.general_status = gen_status;

   /* build the message to be sent to the terminal handler */
   memset(buffer1, 0, sizeof(buffer1));
   memcpy(buffer1,p_auth_tx, Auth_Tx_Size);
   p_msg_term_handler_out = ptemsg_build_msg( MT_AUTH_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              authorizer_que_name,
                                              application_que_name,
                                              (pBYTE)&buffer1,
                                              sizeof(buffer1),
                                              0 );

   ret_code = pteipc_send (p_msg_term_handler_out, authorizer_que_name); 
   if ( ret_code < 0 )
   {
 		pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "pteipc error sending to que %s: %s",
               authorizer_que_name, ErrorMsg );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf( Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvisa_log_message( 3, 3, Buffer , "perform_standin_processing" , 0 ,p_auth_tx->TLF01_details.card_num);
      free(p_msg_term_handler_out);
      return;
   }
   free(p_msg_term_handler_out);
	return; 
} /* perform_standin_processing */

/************************************************************************** ****
*
*  NAME:         GET_DATE
*
*  DESCRIPTION:  This procedure takes an input string that contains a date and
*                time.  It copies the date portion into an output string, 
*                formatted slightly different.
*
*  AUTHOR:       Dennis Irby
*
*  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ" *
*  OUTPUTS:      date_str  : Format = "YYYY0M0D" *
*  RTRN VALUE:   None
*
*  MODIFICATIONS:

*************************************************************************** ***/
void get_date( pCHAR time_date, pCHAR date_str ) 
{
   memset ( date_str,  0x00,         9 ); 
   strncpy( date_str,  time_date,    4 ); 
   strncat( date_str, &time_date[5], 2 ); 
   strncat( date_str, &time_date[8], 2 );
}
/************************************************************************** ****
*
*  NAME:         GET_TIME
*
*  DESCRIPTION:  This procedure takes an input string that contains a date and
*                time.  It copies the time portion into an output string, 
*                formatted slightly different.
*
*  AUTHOR:       Dennis Irby
*
*  INPUTS:       time_date : Format = "YYYY-0M-0D-0H.0I.0S.JJJ" *
*  OUTPUTS:      time_str  : Format = "0H0I0S" *
*  RTRN VALUE:   None
*
*  MODIFICATIONS:

*************************************************************************** ***/
void get_time( pCHAR time_date, pCHAR time_str ) 
{
   memset ( time_str,  0x00,          7 ); 
   strncpy( time_str, &time_date[11], 2 ); 
   strncat( time_str, &time_date[14], 2 ); 
   strncat( time_str, &time_date[17], 2 );
}


/*****************************************************************************

  Function:    perform_error_processing

  Description: This function will take care of transactions that were unable
               to get sent to the host due to some type of error.
  Author: 
      unknown
  Inputs:
      p_auth_tx_in - the auth_tx structure to be processed.
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void perform_error_processing(pAUTH_TX p_auth_tx_in)   
{
   char       date_str [ 9] = "";
   char       time_str [ 7] = "";
   char       time_date[25] = "";
   char       resp_code[3] = "58";
   char       unique_tran_str_id[17] = "";

   /* Get local time. */
   ptetime_get_timestamp( time_date );
   get_date( time_date, date_str );
   get_time( time_date, time_str );


   /* Create RRN and store it in the auth_tx block,format YDDDhmmssjjj */
   if ( 0 == strcmp( NULL_STR, p_auth_tx_in->TLF01_details.retrieval_ref_num))
   {
      create_rrn( p_auth_tx_in->TLF01_details.retrieval_ref_num, p_auth_tx_in );
   }
   
   /* create response code and store it in the auth_tx block */
   strcpy(p_auth_tx_in->TLF01_details.response_code, resp_code );


   /* Create date of transaction and store it in the auth_tx table,
   * format "YYYY0M0D".
   */
   strcpy( p_auth_tx_in->TLF01_details.date_yyyymmdd, date_str );

   /* Create time of transaction and store it in the auth_tx 
   table,"0H240I0S" */
   strcpy( p_auth_tx_in->TLF01_details.time_hhmmss, time_str );

   if (p_auth_tx_in->TLF01_details.primary_key.transaction_id [0] == '\0')
   {
      ptetime_get_strtime(unique_tran_str_id);
      memset(p_auth_tx_in->TLF01_details.primary_key.transaction_id, 0, sizeof(p_auth_tx_in->TLF01_details.primary_key.transaction_id));
      memcpy(p_auth_tx_in->TLF01_details.primary_key.transaction_id, p_auth_tx_in->TLF01_details.terminal_id, sizeof(p_auth_tx_in->TLF01_details.terminal_id) - 1);
      strncat(p_auth_tx_in->TLF01_details.primary_key.transaction_id, unique_tran_str_id+2, 12);
   }

   p_auth_tx_in->TLF01_details.general_status = GS_TXN_DROPPED;
     
}; /* perform_error_processing */


/*****************************************************************************

  Function:    get_visa_data

  Description: This function will put the visa specific data into the auth_tx
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx structure
  Outputs:
      p_auth_tx - the auth_tx structure
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT get_visa_data (pAUTH_TX p_auth_tx)
{
   CHAR temp_str1[15];
   CHAR temp_str2[12];
   CHAR time_date[27];

   strcpy(p_auth_tx->TLF01_details.tpdu_id, ncf01_i.tpdu_id);

   /* Get LOCAL Date and Time */
   ptetime_get_timestamp(time_date);

   memset(&temp_str1, 0x00, sizeof(temp_str1));
   memset(&temp_str2, 0x00, sizeof(temp_str2));
   strncpy(temp_str1, time_date, 4);
   strncpy(temp_str2, (time_date + 5), 2);
   strcat (temp_str1, temp_str2);
   strncpy(temp_str2, (time_date + 8), 2);
   strcat (temp_str1, temp_str2);
   if(p_auth_tx->TLF01_details.date_yyyymmdd[0]== 0x00)
       strcpy(p_auth_tx->TLF01_details.date_yyyymmdd, temp_str1);


   memset(&temp_str1, 0x00, sizeof(temp_str1));
   memset(&temp_str2, 0x00, sizeof(temp_str2));
   strncpy(temp_str1, (time_date + 11), 2);
   strncpy(temp_str2, (time_date + 14), 2);
   strcat (temp_str1, temp_str2);
   strncpy(temp_str2, (time_date + 17), 2);
   strcat (temp_str1, temp_str2);
   if(p_auth_tx->TLF01_details.time_hhmmss[0]== 0x00)
      strcpy(p_auth_tx->TLF01_details.time_hhmmss, temp_str1);   

   /* Get Retrieval Reference Number => YDDDnnnnnnnn */
   if ( AUTH_REVERSAL != p_auth_tx->TLF01_details.tx_key )
   {
      switch( p_auth_tx->TLF01_details.tx_key )
      {
         case AUTH_VOID_SALE:
         case AUTH_VOID_REFUND:
         case AUTH_VOID_CASH_ADVANCE:
         case AUTH_OFFLINE_VOID_SALE:
         case AUTH_OFFLINE_VOID_REFUND:
         case AUTH_OFFLINE_SALE:
        	 // For offline also no need to create new RRN, issue fix

            /* RRN for voids must be same as orig in TLF01 for EB's log file. */
            if ( p_auth_tx->TLF01_details.orig_retrieval_ref_num[0] != 0x00 )
               strcpy( p_auth_tx->TLF01_details.retrieval_ref_num,
                       p_auth_tx->TLF01_details.orig_retrieval_ref_num );
            break;

         default:
            create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx);
            break;
      }
   }
   return (true);
}

/*****************************************************************************

  Function:    find_cvc_flag

  Description: This function will parse field 48 of a 0110 response message 
  looking for Tag 87.  This tag contains a 1 byte flag: Magnetic Stripe / 
  CVC Invalid.
  
    FIELD 48 FORMAT:
 
      -------+------+---------+---------+---------+---------+-----
       Total |Trans | First   | First   | First   | Second  |
        Len  | Cat. |SubField |SubField |SubField |SubField | . . .
             | Code |  Tag    | Length  |  Data   |  Tag    |
      -------+------+---------+---------+---------+---------+-----
  BYTES: 3       1       2         2         x         2
 
  Author: 
      unknown
  Inputs:
      src_len - the length of src
      src - field 48
  Outputs:
      dest - the cvc flag
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void find_cvc_flag( pCHAR src_len, pCHAR src,  pCHAR dest )
{
   #define  FIELD48_MAX_LEN  100   
   #define  FIELD48_TAG      "87"
   INT  sub_field_len;
   CHAR tag[3];
   CHAR len[3];
   INT  MainIndex;
   INT  TotalLen;

   TotalLen = atoi( src_len );

   /* MainIndex starts at 1 to skip over the Transaction Category Code. */
   for( MainIndex=1; MainIndex<TotalLen && MainIndex<FIELD48_MAX_LEN; )
   {
      memset( tag, 0x00, sizeof( tag ) );
      memcpy( tag, &src[MainIndex], 2 );
      MainIndex += 2;
      memset( len, 0x00, sizeof(len));
      memcpy( len, &src[MainIndex], 2 );
      sub_field_len = atoi( len );
      MainIndex += 2;

      if ( 0 == strcmp( tag, FIELD48_TAG ) )
      {
         strncpy( dest, &src[MainIndex], sub_field_len );
         break;
      }
      else
      {
         MainIndex += sub_field_len;
      }
   }
}  /* End find_cvc_flag */


/*****************************************************************************

  Function:    calc_julian_day

  Description: This function will calculate the julian day
  Author: 
      unknown
  Inputs:
      date_str -  input string format  = YYYYMMDD...
  Outputs:
      julian_str - output string format = DDD
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
void calc_julian_day(char date_str[], char julian_str[])
{
   char  year[5];
   char  month[3];
   char  day[3];
   int   iYear;
   int   iMonth;
   int   iDay;
   int   julian_day;
   int   i;
   int   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };


   memset (&year, 0, sizeof(year));
   memset (&month, 0, sizeof(month));
   memset (&day,   0, sizeof(day));
   memcpy (&year,  &date_str[0], 4);
   memcpy (&month, &date_str[4], 2);
   memcpy (&day,   &date_str[6], 2);

   iYear  = atoi( year );
   iMonth = atoi( month );
   iDay   = atoi( day );

   for( i=0,julian_day=0; i<iMonth-1 && i<12; i++ )
   {
		julian_day += DaysOfMonth[i];
   }
   julian_day += iDay;

   if ( iMonth > 2 )
      if ( 0 == ( iYear % 4 ) )
          julian_day++;
   
   itoa(julian_day, julian_str, 10);

   Rj_with_lead_zeros (julian_str, 3);
}

/*****************************************************************************

  Function:    process_incoming_message

  Description: This function will process an incoming message from the
               authorizer; it may be a request or a response.
  Author: 
      unknown
  Inputs:
      pmsg - a pointer to the incoming message that contains the auth_tx
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT process_incoming_message(pPTE_MSG p_msg)
{
   pPTE_MSG_DATA   p_msg_data = NULL_PTR;
   LONG            data_len;
   LONG            auth_len;
   pBYTE           p_data_data = NULL_PTR;
   AUTH_TX         auth_tx;
   CHAR            Buff[512]={0};

   /* Get auth_tx structure out of the PTE message. */
   memset(&auth_tx, 0x00, Auth_Tx_Size);
   p_msg_data = ptemsg_get_pte_msg_data(p_msg);
   data_len = ptemsg_get_pte_msg_data_data_length(p_msg_data);
   auth_len = data_len;
   if(Auth_Tx_Size < data_len)
      auth_len = Auth_Tx_Size;
   p_data_data = ptemsg_get_pte_msg_data_data(p_msg_data);
   memcpy(&auth_tx, p_data_data, auth_len);
   p_data_data = p_data_data + Auth_Tx_Size;

   /* No need to continue for authorization advices or ATM confirmations.
    * Visa does not want them.
    */
   if ( (auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE)      ||
        (auth_tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM_RESPONSE) &&
		  (0 ==strncmp(auth_tx.TLF01_details.authorizing_host_queue,"ncvis2",5)) ) /* Girija Y TF, March 31 2009 */
   {
      //Send a msg to updateds to restore the balances.
      update_database( &auth_tx );
      return( true );
   }

   if( (auth_tx.TLF01_details.tx_key < AUTH_SALE) ||
       (auth_tx.TLF01_details.tx_key > AUTH_INVALID) )
      Tran_Flow = SWITCHED_IN;
   else
      Tran_Flow = SWITCHED_OUT;

   strncpy(auth_tx.TLF01_details.tpdu_id, ncf01_i.tpdu_id,
           sizeof(ncf01_i.tpdu_id));

   if(!(auth_tx.TLF01_details.tx_key == AUTH_LOGON_RESPONSE))
   {
      if( Tran_Flow == SWITCHED_IN )
      {
         /* This is a response to a switched in request from the host. */
         ptetime_get_strtime(auth_tx.TLF01_details.tran_finish_time);
         auth_tx.TLF01_details.tx_key =
                        match_request_to_response(auth_tx.TLF01_details.tx_key);



    	 if(0==strncmp(auth_tx.TLF01_details.acquirer_id,"nceqit",6) ||
    	   	(0==strncmp(auth_tx.TLF01_details.acquirer_id,"neqit2",6)))
    	 {
        	 if( ((0 == strcmp(auth_tx.TLF01_details.response_code, "00")) ||
        	      (0 == strcmp(auth_tx.TLF01_details.response_code, "55"))) &&
        	 	  (auth_tx.TLF01_details.tx_key != AUTH_REVERSAL_ADVICE_RESPONSE) )
        	     {
        	         if ( false == update_database( &auth_tx ))
        	         {
        	             memset(Log_Bufr,0x00,sizeof(Log_Bufr));
        	             sprintf(Log_Bufr," Unable to update database tables. Term Id:%s ,Merch Id: %s RRN: %s ",
        	                					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
        	             ncvisa_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);

        	                /* Unable to complete a transaction.  Inform host; log to TLF01. */
        	             strcpy(auth_tx.TLF01_details.response_code,"96");
        	             if(false == incoming_other_host_response_db_reply( &auth_tx ))
        	             {
        	                   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
        	                   sprintf( Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
        	                   		   auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
        	                   ncvisa_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
        	             }
        	          }
        	      }
        	   else
        	   {
        	             /*
        	              * Since the transaction control responded with response code other
        	              * than 00, database is not updated. Sending back the response to
        	              * Host. Insert into TLF01.  Also come here for responses to balance
        	              * inquiries; they do not need to update the database.
        	              *
        	              * Response has already been sent for reversals, so bypass it here.
        	              */

        	         	 //Applicable for both Credit and debit.


        		   if ( auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE ||
        		       auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE)
        	       {
        	             	/*
        	             	 * Txcntl changes the tx_key to AUTH_REVERSAL(410).
        	             	 * Hence change the key here to get 430 response.
        	             	 */
        	    	   auth_tx.TLF01_details.tx_key=AUTH_REVERSAL_ADVICE_RESPONSE;

        	             	//To be able to insert into TLF01 table, the below field msut be set to 0.
        	    	   auth_tx.host2_acquirer_cb_code[0] = 0x00;

        	    	   if(false == incoming_other_host_response_db_reply(&auth_tx))
        	    	   {
        	    		   memset(Log_Bufr,0x00,sizeof(Log_Bufr));
        	    		   sprintf(Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
        	    				   	  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
        	    		   ncvisa_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
        	         	}
        	         }

        		   else
        		   {
        			   if ( auth_tx.TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE )
        	         {
        	            if(false == incoming_other_host_response_db_reply(&auth_tx))
        	            {
        	             	memset(Log_Bufr,0x00,sizeof(Log_Bufr));
        	             	sprintf(Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
        	             					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
        	             	ncvisa_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
        	            }
        	         }
        		   }

        	   }
         }
         else
         {
        	if( ((0 == strcmp(auth_tx.TLF01_details.response_code, "00")) ||
        	     (0 == strcmp(auth_tx.TLF01_details.response_code, "55")) ) &&
        			(auth_tx.TLF01_details.tx_key != AUTH_REVERSAL_ADVICE_RESPONSE))
        	  {

        	     if ( false == update_database( &auth_tx ))
        	     {
        	        memset(Log_Bufr,0x00,sizeof(Log_Bufr));
        	        sprintf(Log_Bufr," Unable to update database tables. Term Id:%s ,Merch Id: %s RRN: %s ",
        	               					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
        	        ncvisa_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);

        	               /* Unable to complete a transaction.  Inform host; log to TLF01. */
        	        strcpy(auth_tx.TLF01_details.response_code,"96");
        	        if(false == incoming_other_host_response_db_reply( &auth_tx ))
        	        {
        	                  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
        	                  sprintf( Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
        	                  		   auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
        	                  ncvisa_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
        	        }
        	     }
        	   }
        	   else
        	   {
        	            /*
        	             * Since the transaction control responded with response code other
        	             * than 00, database is not updated. Sending back the response to
        	             * Host. Insert into TLF01.  Also come here for responses to balance
        	             * inquiries; they do not need to update the database.
        	             *
        	             * Response has already been sent for reversals, so bypass it here.
        	             *
        	             *
        	             */

        		   if ( auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE ||
        				auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_REPEAT_RESPONSE)
        		   {
        		           	             	/*
        		           	             	 * Txcntl changes the tx_key to AUTH_REVERSAL(410).
        		           	             	 * Hence change the key here to get 430 response.
        		           	             	 */
        		         auth_tx.TLF01_details.tx_key=AUTH_REVERSAL_ADVICE_RESPONSE;

        		           	             	//To be able to insert into TLF01 table, the below field msut be set to 0.
        		         auth_tx.host2_acquirer_cb_code[0] = 0x00;

        		       if(false == incoming_other_host_response_db_reply(&auth_tx))
        		       {
        		           (Log_Bufr,0x00,sizeof(Log_Bufr));
        		           	sprintf(Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
        		           	    				   	  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
        		           	ncvisa_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
        		       }
        		  }
        		   else
        		   {
        			   if ( auth_tx.TLF01_details.tx_key != AUTH_REVERSAL_RESPONSE )
        			   {
        				  if(false == incoming_other_host_response_db_reply(&auth_tx))
        				  {
        	                  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
        	                  sprintf(Log_Bufr," Unable to send response to host. Term Id:%s ,Merch Id: %s RRN: %s ",
        	                  		  auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
        	                  ncvisa_log_message( 3, 3, Log_Bufr , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
        				  }
        			   }
        		   }
        	   }
         }
         return(true);
   	 }
   }

   /* Process Switched Out requests. */
   if ( AUTH_REVERSAL == auth_tx.TLF01_details.tx_key )
   {
      /* It is a reversal. */
      strcpy( auth_tx.orig_acq_inst_id, ncf01_i.acquirer_inst_id_code );
      strcpy( auth_tx.amex_pos_entry_mode, ncf01_i.forwarding_inst_id_code );

      /* Display reversal message on Monitor.
       * This message is used to isolate problems to selected bins
       * or to certain merchants or to networks.
       */
      memset( Buff, 0x00, sizeof(Buff) );
      sprintf( Buff, "Reversal going to Visa: %s. ", AppName);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf( Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		   auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buff,Log_Bufr);
      ncvisa_log_message( 3, 2, Buff , "process_incoming_message" , 0 ,auth_tx.TLF01_details.card_num);
   }

   else if ( true == hhutil_verify_pin_block(&auth_tx) )
   {
      /* PIN Translation is required */
      strcpy(auth_tx.TLF01_details.dest_key, ncf01_i.cwk);
      strcpy(auth_tx.TLF01_details.dest_pin_format, ncf01_i.pin_type);
      if(false == hhutil_process_debit_transaction(&auth_tx))
      {
         return(false); /* Messages are in hhutil_process_debit_transaction */
      }
      return(true);
   }

   /* Do not perform PIN translation. */
   strcpy( auth_tx.TLF01_details.security_response_code, "00" );
   return( process_incoming_message_continued( &auth_tx ) );
}  
/******************************************************************************
 *
 *  NAME:         PROCESS_INCOMING_MESSAGE_CONTINUED
 *
 *  DESCRIPTION:  This function continues the processing of an incoming
 *                message.  It is used as a common stepping stone to get
 *                to 'incoming_terminal_request'.  It is called after NCF01
 *                data is obtained, and also after PIN verification has
 *                been performed.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT process_incoming_message_continued( pAUTH_TX p_auth_tx )
{
   if (0 == strcmp(p_auth_tx->TLF01_details.security_response_code, "00"))
   {
      /* Tell the authorizer to check CVV. if TRK2/TRK1 data is present*/
     if((p_auth_tx->TLF01_details.track2[0] == 0x00) && (p_auth_tx->TLF01_details.track1[0] == 0x00))
     {
         if(p_auth_tx->TLF01_details.verify_mag_stripe[0] == 0x00)
		  p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'N';
     }
     else 
         p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';

      if(incoming_terminal_request(p_auth_tx))
         return(true);
   }
   else 
   {
      perform_error_processing( p_auth_tx );
      send_it_back_to_terminal("96", p_auth_tx);
   }
   return(false);
}


/*****************************************************************************

  Function:    process_incoming_message_ncf30_reply

  Description: This function will process an incoming message from the data
               server request for a select of ncf30.  
  Author: 
      E Gable
  Inputs:
      pCurrent - a pointer to the reply message from the data server
  Outputs:
      None
  Return values:
      true  - indicates successful processing.
      false - indicates no successful processing.
  Modified by:
******************************************************************************/
INT process_incoming_message_ncf30_reply(pBYTE pCurrent)
{
   AUTH_TX   auth_tx;
   pPTE_MSG  p_msg_ins = NULL_PTR;
   NCF30     ncf30;
   INT       total_amt;
   INT       actual_amt;
   INT       amt_to_be_reversed;


   memset(&ncf30,   0, Ncf30_Size);
   memset(&auth_tx, 0, Auth_Tx_Size);
   memcpy(&ncf30,   pCurrent+(Auth_Tx_Size), Ncf30_Size);
   memcpy(&auth_tx, pCurrent, Auth_Tx_Size);

   /* Original txn was found in NCF30 for an advice, repeat, or reversal. */
   if ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE )
   {
       /* Treat advices differently. */
      (void)process_auth_advices( &auth_tx, FOUND, ncf30 );
   }
   else if ( auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION )
   {
      (void)process_auth_repeat_part2( &auth_tx, FOUND, &ncf30 );
   }
   else if(auth_tx.TLF01_details.tx_key == AUTH_REFUND)
   {
	   (void)hhutil_transfer_ctl_to_auth_app(&auth_tx);
   }
   else
   {
      update_auth_tx_with_ncf30_data(&ncf30, &auth_tx);
      if ( auth_tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM )
      {
         /* Set up the amounts so txcntl can process
          * the partial reversal correctly:
          *    total_amount    = Amt of original txn
          *    actual_amount   = Amt of money ATM dispensed
          *    reversal_amount = Amt to be reversed into card holders record
          *
          *    This is what we have:
          *       1.  orig_amount   = Amt of orig txn
          *       2.  actual_amount = Amt of money ATM dispensed
          */

         /* First get amount of original transaction.
          * If conversion took place, orig_amount is in USD.
          * If no conversion took place, total_amount already contains
          * the correct value, but we do not know.
          */
         memcpy( auth_tx.TLF01_details.total_amount,
                 auth_tx.TLF01_details.orig_amount,
                 sizeof(auth_tx.TLF01_details.total_amount)-1 );

         /* Now calculate amount to be reversed = total_amt - actual_amt.
          * If conversion took place, actual_amt is in USD, else it is
          * in the same currency as the total_amount.
          */
         total_amt  = atoi(auth_tx.TLF01_details.total_amount );
         actual_amt = atoi(auth_tx.TLF01_details.actual_amount);
         amt_to_be_reversed = total_amt - actual_amt;

         sprintf( auth_tx.TLF01_details.reversal_amount, "%012d",
                  amt_to_be_reversed);
      }

      /* Tell txcntl to go process the reversal. */
      (void)hhutil_transfer_ctl_to_auth_app(&auth_tx);
   }
   return(true);
}  

/******************************************************************************
*
*  NAME:         PROCESS_AUTH_ADVICES
*
*  DESCRIPTION:  This function processes authorization advices.  An advice
*                means the host did not receive a response to an authorization.
*                So they stood in for us.  The advice tells us the disposition
*                of the stand in.  This function checks 2 things: Was the
*                original authorization found in NCF30 (which means ATP approved
*                it), Did the host approve or decline the authorization?
*                The advice contains the authorization number and the response
*                code of the original authorization.
*
*                Here is what happens:
*                1.  Always log advice to TLF01
*                2.  If the original authorization found and the advice contains
*                       a declined response code, then send advice to txcntl and
*                       drop txcntl's response.  Txcntl will treat it as a
*                       reversal.
*                3.  If the original authorization was not found and the advice
*                       contains an approved response code, then do the same as
*                       in step 2.  Txcntl will treat it as an authorization.
*
*  INPUTS:       p_auth_tx - Transaction record
*                status    - Status of the select for the original authorization
*                            FOUND or NOT_FOUND
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   True if successful, false is a problem occurred 
*
*  AUTHOR:       Dennis Irby
*
*******************************************************************************/
INT process_auth_advices( pAUTH_TX p_auth_tx, INT status, NCF30 ncf30 )
{
   INT   ret_val = true;
   char  resp_code[4] = {0};

   /* Visa advices do not need responses.  Log to TLF01.
    * Need to do this prior to going to txcntl where the auth_number
    * and response code may get overwritten.
    */
	if(p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE &&
	   (0 != strncmp(p_auth_tx->TLF01_details.handler_queue, "dcp", 3)) &&
	   p_auth_tx->TLF01_details.product_codes[8].quantity[0] == '1')
	{
		//product_codes[8].quantity[0] is 1 if DE38 is not present in 0120 request
		//product_codes[8].quantity[0] is 0 if DE38 is present in 0120 request.
		//ATP119AMC-5 - clearing the auth number for request for not having DE38
		memset(p_auth_tx->TLF01_details.auth_number,0x00,sizeof(p_auth_tx->TLF01_details.auth_number));
	}

   (void)insert_tlf01(p_auth_tx);

   /* See if we need to send to txcntl. */
   if ( ( (FOUND == status) &&
          (0 != strcmp(p_auth_tx->TLF01_details.response_code, "00" )) )
        ||
        ( (NOT_FOUND == status) &&
          (0 == strcmp(p_auth_tx->TLF01_details.response_code, "00" )) )
      )
   {
      /* Send advice to transaction control. */
      if ( FOUND == status )
      {
         strcpy( resp_code, p_auth_tx->TLF01_details.response_code );
         update_auth_tx_with_ncf30_data( &ncf30, p_auth_tx );
         strcpy( p_auth_tx->TLF01_details.response_code, resp_code );
      }
      ret_val = hhutil_transfer_ctl_to_auth_app( p_auth_tx );
   }
   else
   {
		/*TF - ABHISHEK Changes for 120 messges*/

		/*Advice messages should be sent to Cadencie as long
		as Cadencie is online even there is no related sale transaction
		before 120 transaction*/
		p_auth_tx->TLF01_details.tx_key = AUTH_ADVICE;
		ret_val = hhutil_transfer_ctl_to_auth_app( p_auth_tx );
  	}

   if ( false == ret_val )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Unable to complete processing of an advice (1120). Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncvisa_log_message( 3, 3, Log_Bufr , "process_auth_advices" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   return( ret_val );
 }

/******************************************************************************
 *
 *  NAME:         PROCESS_AUTH_REPEAT_PART1
 *
 *  DESCRIPTION:  This function is the first part of processing a repeat of
 *                an authorization.  This function sets up a call to the
 *                network data server to find the original authorization txn.
 *                After that, processing will be picked up in part2 - this
 *                happens if we find the original, and also if we do not.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BYTE process_auth_repeat_part1( pAUTH_TX p_auth_tx )
{
   BYTE      ret_val = true;
   LONG      ret_code;
   NCF30     ncf30;
   BYTE      buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   CHAR      ErrorMsg[512]={0};
   pPTE_MSG  p_msg_ins = NULL_PTR;

   /* Set up database keys to find original authorization. */
   memset( &ncf30, 0x00, Ncf30_Size );
   strcpy( ncf30.primary_key.network_id, AppName );
   strcpy( ncf30.primary_key.network_type, "A" );
   strcpy( ncf30.primary_key.batch_number, "ACTIVE" );
   strcpy( ncf30.card_nbr, p_auth_tx->TLF01_details.card_num );
   strcpy( ncf30.primary_key.retrieval_ref_num,
           p_auth_tx->TLF01_details.retrieval_ref_num );

   /* Build message to get the original transaction from data server. */
   memcpy( buffer1,              p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &ncf30,    Ncf30_Size   );

   p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
                                 ST1_DB_SELECT, 
                                 ST2_NONE,
                                 netds_que_name, 
                                 application_que_name,
                                 (pBYTE)&buffer1,
                                 sizeof(buffer1), 
                                 NCF30_DATA );
   if ( NULL_PTR == p_msg_ins  )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory for select ncf30 message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncvisa_log_message( 3, 3, Log_Bufr , "process_auth_repeat_part1" , 0 ,p_auth_tx->TLF01_details.card_num);
      ret_val = false;
   }
   else
   {
      /* Send message to get host specific data from data server. */
      ret_code = pteipc_send( p_msg_ins, netds_que_name );
      free( p_msg_ins );
      if ( 0 > ret_code )
      {
         memset( ErrorMsg, 0x00, sizeof(ErrorMsg) );
         pteipc_get_errormsg( ret_code, ErrorMsg );
         sprintf( ErrorMsg, "Failed transmit to queue: %s. %s",
                  netds_que_name, ErrorMsg );

         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(ErrorMsg,Log_Bufr);
         ncvisa_log_message( 3, 3, ErrorMsg , "process_auth_repeats_part1" , 0 ,p_auth_tx->TLF01_details.card_num);
         ret_val = false;
      }
   }

   /* If we failed to send the request off to the data server,
    * chances are we cannot reply to Visa host, but we need to try.
    */
   if ( ret_val == false )
   {
      p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION_RESPONSE;
      strcpy( p_auth_tx->TLF01_details.response_code, SYSTEM_ERROR );

      /* This function inserts into TLF01. */
      (void)incoming_other_host_response_db_reply( p_auth_tx );
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_AUTH_REPEAT_PART2
 *
 *  DESCRIPTION:  This function is called after the original txn was either
 *                found or not found in NCF30.  If found, we will just respond
 *                to the Visa host because we already processed the orig auth,
 *                and we do not want to debit a cardholder's account again.
 *                If orig auth was not found, then we will treat this txn like
 *                a regular authorization.
 *
 *  INPUTS:       p_auth_tx - Transaction record
 *                orig_txn  - Flag to indicate if orig txn was found
 *                p_ncf30   - Original txn, if it was found
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
BYTE process_auth_repeat_part2( pAUTH_TX p_auth_tx, INT orig_txn, pNCF30 p_ncf30)
{
   BYTE  ret_val;

   Tran_Flow = SWITCHED_IN;
   if ( orig_txn == FOUND )
   {
      /* The original txn was found.  Formulate and send response to host. */
      init_txn( p_auth_tx );
      strcpy( p_auth_tx->TLF01_details.auth_number,   p_ncf30->auth_number   );
      strcpy( p_auth_tx->TLF01_details.response_code, p_ncf30->response_code );
      p_auth_tx->TLF01_details.tx_key = AUTH_AUTHORIZATION_RESPONSE;

      /* Send response - also inserts into TLF01. */
      (void)incoming_other_host_response_db_reply( p_auth_tx );
      ret_val = true;
   }
   else
   {
      /* The original txn was not found.  Treat it like a new authorization.
       * This is so Txcntl does not consider it an advice.
       */
      p_auth_tx->TLF01_details.message_type[3] = '0';  /* 0101 -> 0100 */
      ret_val = hhutil_process_host_request( p_auth_tx );
   }
   return( ret_val );
}


/******************************************************************************
*
*  NAME:         DELETE_ORIG_NCF30
*
*  DESCRIPTION:  This function sends a message to the data server to delete
*                a particular NCF30 record.  This will be used to delete 
*                transactions that are being reversed.
*
*  INPUTS:       p_auth_tx - Transaction record 
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   True if successful, false is a problem occurred 
*
*  AUTHOR:       Dennis Irby
*
******************************************************************************/
INT delete_orig_ncf30( pNCF30 pncf30, pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg  = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)]; 
   LONG            ret_code;
   CHAR            Buff[512]={0};
   CHAR            err_mesg[512] = {0};
 
   /* Set up ncf30 keys. */
   strcpy( pncf30->primary_key.network_id,   AppName );
   strcpy( pncf30->primary_key.network_type, "A"     );
   strcpy( pncf30->primary_key.batch_number, "ACTIVE");
   strcpy( pncf30->card_nbr, p_auth_tx->TLF01_details.card_num );
   strcpy( pncf30->primary_key.retrieval_ref_num,
           p_auth_tx->TLF01_details.orig_retrieval_ref_num );

   memset( buffer1, 0x00,      sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,pncf30, Ncf30_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                             ST1_DB_DELETE, 
                             ST2_NONE,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             NCF30_DATA );
   if ( NULL_PTR == p_msg  )
   {
      ncvisa_log_message( 3, 2, "Insufficient Memory to build NCF30 message" , "delete_orig_ncf30" , 0 ,"");
      return( false );
   }

   /* Send message data server. */
   ret_code = pteipc_send( p_msg, netds_que_name ); 
   free( p_msg );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg ); 
      sprintf(Buff,"Failed transmit to data server for ncf30: %s. %s", netds_que_name, err_mesg );
      ncvisa_log_message( 3, 3,Buff, "delete_orig_ncf30" , 0 ,"");
      return( false );
   }
   return(true);
 }

/******************************************************************************
 *
 *  NAME:         UPDATE_AUTH_TX_WITH_NCF30_DATA
 *
 *  DESCRIPTION:  This function takes data from ncf30 and puts it into auth_tx
 *
 *  INPUTS:       pncf30 - Pointer to NCF30 structure
 *
 *  OUTPUTS:      auth_tx - with data from NCF30
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Beth Gable
 *
 ******************************************************************************/
VOID update_auth_tx_with_ncf30_data(pNCF30 pncf30, pAUTH_TX p_auth_tx)
{
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.card_num ) )
      strcpy( p_auth_tx->TLF01_details.card_num, pncf30->card_nbr ); 

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.processing_code ) )
      strcpy(p_auth_tx->TLF01_details.processing_code, pncf30->processing_code);

   strcpy( p_auth_tx->TLF01_details.orig_amount,   pncf30->tran_amount ); 
   strcpy( p_auth_tx->TLF01_details.time_hhmmss,   pncf30->tran_time   ); 
   strcpy( p_auth_tx->TLF01_details.date_yyyymmdd, pncf30->tran_date   ); 

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.orig_retrieval_ref_num))
      strcpy( p_auth_tx->TLF01_details.orig_retrieval_ref_num,
              pncf30->primary_key.retrieval_ref_num ); 

   strcpy( p_auth_tx->TLF01_details.auth_number,   pncf30->auth_number   ); 
   strcpy( p_auth_tx->TLF01_details.response_code, pncf30->response_code ); 
   strcpy( p_auth_tx->TLF01_details.terminal_id,   pncf30->device_id     ); 
   strcpy( p_auth_tx->TLF01_details.orig_message,  pncf30->message_type  ); 

   if ( (0 == strcmp( "000000", p_auth_tx->TLF01_details.orig_sys_trace_num)) ||
        ( p_auth_tx->TLF01_details.orig_sys_trace_num[0] == 0x00 ) )
      strcpy( p_auth_tx->TLF01_details.orig_sys_trace_num,
              pncf30->sys_trace_audit_nbr );

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.transmission_timestamp))
      strcpy( p_auth_tx->TLF01_details.transmission_timestamp,
              pncf30->transmission_timestamp ); 
   else
      strcpy( p_auth_tx->orig_local_time_date,
              pncf30->transmission_timestamp ); 

   strcpy( p_auth_tx->TLF01_details.tran_fee_amount,
           pncf30->tran_fee_amount ); 
   strcpy( p_auth_tx->TLF01_details.mcard_banknet, pncf30->network_data ); 
   if(0 == strcmp( "0400", p_auth_tx->TLF01_details.message_type))
   {
	   //At this point of time, we dont know whether a transaction is STIP or online.
	   //strcpy( p_auth_tx->TLF01_details.product_codes[1].code, pncf30->invoice_nbr );
   }
   else
   {
	   strcpy( p_auth_tx->TLF01_details.product_codes[1].code, pncf30->invoice_nbr );
   }
}

/******************************************************************************
*
*  NAME:         RECON_UPDATE
*
*  DESCRIPTION:  This function sends auth_tx to the data server.
*                Its purpose is to update totals and keep a running count.
*                It also stores the transaction into NCF30. 
*
*  INPUTS:       p_auth_tx - Transaction record 
*
*  OUTPUTS:      None
*
*  RTRN VALUE:   True if successful, false is a problem occurred 
*
*  AUTHOR:       Dennis Irby
*
******************************************************************************/
INT recon_update( pAUTH_TX p_auth_tx, BYTE subtype_2, pCHAR network_type )
{
   pPTE_MSG        p_msg  = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   LONG            ret_code;
   CHAR            Buff[512]={0};
   CHAR            err_mesg[512] = {0};
   NCF30           ncf30;
   CHAR            time_date[25];


   /* Build the NCF30 structure */
   memset( &ncf30, 0x00, Ncf30_Size );
   strcpy( ncf30.primary_key.network_id,    AppName ); 
   strcpy( ncf30.primary_key.network_type, network_type ); 
   strcpy( ncf30.primary_key.batch_number, "ACTIVE" );
   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.retrieval_ref_num))
   {
      create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx );
   }
   strcpy( ncf30.primary_key.retrieval_ref_num,
           p_auth_tx->TLF01_details.retrieval_ref_num );
   strcpy( ncf30.merchant_id, p_auth_tx->TLF01_details.merchant_id );
   strcpy( ncf30.transaction_id,
           p_auth_tx->TLF01_details.primary_key.transaction_id );
   strcpy( ncf30.device_id, p_auth_tx->TLF01_details.terminal_id );
   strcpy( ncf30.card_nbr,  p_auth_tx->TLF01_details.card_num  );
   strcpy( ncf30.message_type, p_auth_tx->TLF01_details.message_type );
   strcpy( ncf30.processing_code, p_auth_tx->TLF01_details.processing_code );
   strcpy( ncf30.tran_amount, p_auth_tx->TLF01_details.total_amount );
   strcpy( ncf30.auth_number, p_auth_tx->TLF01_details.auth_number );
   strcpy( ncf30.response_code, p_auth_tx->TLF01_details.response_code );
   strcpy( ncf30.currency_code, p_auth_tx->TLF01_details.currency_code );
   strcpy( ncf30.transmission_timestamp,p_auth_tx->TLF01_details.transmission_timestamp);

   if ( 0 == strcmp( NULL_STR, p_auth_tx->TLF01_details.date_yyyymmdd ))
   {
      /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
      ptetime_get_timestamp( time_date );

      /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
      get_date( time_date, p_auth_tx->TLF01_details.date_yyyymmdd );
      get_time( time_date, p_auth_tx->TLF01_details.time_hhmmss   );
   }
   strcpy( ncf30.tran_date, p_auth_tx->TLF01_details.date_yyyymmdd );
   strcpy( ncf30.tran_time, p_auth_tx->TLF01_details.time_hhmmss   );
   strcpy( ncf30.sys_trace_audit_nbr, p_auth_tx->TLF01_details.sys_trace_audit_num );
   ncf30.tx_key = p_auth_tx->TLF01_details.tx_key;
   strcpy( ncf30.invoice_nbr, p_auth_tx->TLF01_details.invoice_number );
   strcpy( ncf30.network_data, p_auth_tx->TLF01_details.mcard_banknet );

   /* Build message for data server to update network batch totals. */
   memset( buffer1, 0x00,      sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,&ncf30, Ncf30_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST, 
                             ST1_DB_UPDATE, 
                             subtype_2,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             ALL_DATA );
   if ( NULL_PTR == p_msg  )
   {
      ncvisa_log_message( 3, 2, "Insufficient Memory to build update NCF30 message" , "recon_update" , 0 ,"");
      return( false );
   }

   /* Send message data server. */
   ret_code = pteipc_send( p_msg, netds_que_name );
   free( p_msg );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg ); 
      sprintf(Buff,"Failed transmit to data server for update ncf30: %s. %s", netds_que_name, err_mesg );
      ncvisa_log_message( 3, 2, Buff , "recon_update" , 0 ,"");
      return( false );
   }
   return( true );
}

/******************************************************************************
 *
 *  NAME:         CREATE_RRN
 *
 *  DESCRIPTION:  This procedure creates a retrieval reference number in the
 *                format of YDDDhmmssjjj, that is:
 *                     Y   = last digit of the year
 *                     DDD = Julian day
 *                     h   = last digit of the 24 hour clock (current time)
 *                     mm  = current system minutes
 *                     ss  = current system seconds
 *                     jjj = current system milliseconds
 *
 *                It also populates the GMT for field 7.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      rrn - Formatted as mentioned above in the description
 *                      RRN should be a 13 character string (12+null).
 *
 *                p_auth_tx - TLF01_details.transmission_timestamp
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void create_rrn( pCHAR rrn, pAUTH_TX p_auth_tx)
{
   CHAR    time_date[25];
   CHAR    date_str[9];
   CHAR    time_str[7];
   CHAR    julian_day[4];
   CHAR    gmt_time[15];

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
   ptetime_get_timestamp( time_date );

   /* Reformat to get date "YYYYMMDD" and time "hhmmss" */
   get_date( time_date, date_str );
   get_time( time_date, time_str );

   /* Change date to julian format - DDD */
   calc_julian_day( date_str, julian_day );

   /* Format RRN: "YDDDhmmssjjj" */
   sprintf( rrn, "%1.1s%3.3s%5.5s%3.3s",
            time_date+3,    /* Last digit of year */
            julian_day,     /* Three digit Julian Day */
            time_str+1,     /* Last digit of hour + minutes + seconds [hmmss] */
            time_date+20 ); /* Milliseconds [jjj] */

   /* Get Greenwich Mean Time for Field 7 */
   pintime_get_gmt( gmt_time );  // YYYYMMDDhhmmss 
   if ( p_auth_tx->TLF01_details.transmission_timestamp[0] == 0x00 )
   {
      strncpy( p_auth_tx->TLF01_details.transmission_timestamp, &gmt_time[4], 10 );  // MMDDhhmmss 
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         UPDATE_DATABASE
 *
 *  DESCRIPTION:  This function will send a request to update transaction
 *                control database tables.  All the information to do so is
 *                stored in p_auth_tx.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT update_database( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)];
   LONG       ret_code;
   INT        ret_val = true;
   CHAR       Buff[512]={0};
   CHAR       err_mesg[512] = {0};
   
   // Update Auth_tx with proper currency codes..if coming from VISA... TF Nirbhay/phani
  /* if(strncmp(p_auth_tx->TLF01_details.handler_queue,"ncvisa",6) == 0)
   {
	 if( !IsFieldNullOrSpaces(p_auth_tx->TLF01_details.nfi_seq_nbr,sizeof(p_auth_tx->TLF01_details.nfi_seq_nbr))  || strcmp(p_auth_tx->TLF01_details.nfi_seq_nbr,p_auth_tx->TLF01_details.currency_code) == 0)
	 {  memcpy( p_auth_tx->TLF01_details.sales_amount,
                 p_auth_tx->ch_billing_amt,
                 12 );
	   
	    memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
                 p_auth_tx->ch_billing_curr_code,
                 3 );

	 }
   } */  

	if(p_auth_tx->TLF01_details.tx_key == AUTH_ADVICE_RESPONSE &&
	   (0 != strncmp(p_auth_tx->TLF01_details.handler_queue, "dcp", 3)) &&
	   p_auth_tx->TLF01_details.product_codes[8].quantity[0] == '1')
	{
		//product_codes[8].quantity[0] is 1 if DE38 is not present in 0120 request
		//product_codes[8].quantity[0] is 0 if DE38 is present in 0120 request.
		//ATP119AMC-5 - clearing the auth number for request for not having DE38
		memset(p_auth_tx->TLF01_details.auth_number,0x00,sizeof(p_auth_tx->TLF01_details.auth_number));
	}

   memset(buffer, 0, sizeof(buffer));
   memcpy(buffer, p_auth_tx, Auth_Tx_Size);

   /*
	Check  if its a response for Verification-Only Requests
	*/


   //authorization_queue=nceqit
	if ( (0 == strncmp (p_auth_tx->TLF01_details.pos_condition_code, "51",2)) &&
		 (0 == strncmp (p_auth_tx->TLF01_details.product_codes[0].quantity,"01",2))&&
		 (0 == strncmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN)))
		{
			strncpy(p_auth_tx->TLF01_details.response_code,"85",2);
		}


   p_msg = ptemsg_build_msg(MT_DB_REQUEST, 
                                ST1_DB_UPDATE, 
                                ST2_ACQUIRER,
                                updateds_que_name, 
                                application_que_name,
                                (pBYTE)&buffer,
                                sizeof(buffer), 
                                ALL_DATA); 
   if(NULL_PTR == p_msg)
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf( Log_Bufr," Insufficient Memory to build UPDATE ALL message Term Id:%s ,Merch Id: %s RRN: %s ",
      		   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncvisa_log_message( 3, 2, Log_Bufr , "update_database" , 0 ,p_auth_tx->TLF01_details.card_num);
      ret_val = false;
   }
   else
   {
      ret_code = pteipc_send(p_msg, updateds_que_name); 
      free(p_msg);
      if(ret_code < 0)
	  {
         pteipc_get_errormsg(ret_code, err_mesg);
         sprintf(Buff,"Failed transmit to que: %s. %s",
                 updateds_que_name,err_mesg);
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buff,Log_Bufr);
         ncvisa_log_message( 3, 3, Buff , "update_database" , 0 ,p_auth_tx->TLF01_details.card_num);
         ret_val = false;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         IS_TXN_VISA_ELECTRON
 *
 *  DESCRIPTION:  This function determines if a transaction is a Visa
 *                Electron.  Visa Electron is a debit card that can be used
 *                both at merchant and at ATM levels in the Phillippines as
 *                well as abroad.  However, since it adopts existing credit
 *                card POS infrastructure, all Electron transactions come in
 *                as credit authorization requests.  PIN is required at ATMs
 *                but is optional at POS.  In case PIN is not  used at POS,
 *                signature is used to authenticate the cardholder very much
 *                like a credit card.
 *
 *                This function uses the bin range to determine if a card is
 *                a Visa Electron.  If it is, the track data is parsed to get
 *                the service code.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if transaction is a Visa Electron txn, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT is_txn_visa_electron( pAUTH_TX p_auth_tx )
{
   INT   ret_val = false;
   if(AUTH_OCT_TX == p_auth_tx->TLF01_details.tx_key || AUTH_FAST_FUND == p_auth_tx->TLF01_details.tx_key)
   { /* No validation for Visa elctron card if tx is OCT*/
	   return( ret_val );
   }
   /* Compare card number to bin range to see if this is Visa Electron. */
   if ( 0 == strncmp(p_auth_tx->TLF01_details.card_num,VE_BIN,VE_BIN_LEN) )
   {
      /* This is a Visa Electron card. */
      ret_val = true;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         VALIDATE_VISA_ELECTRON
 *
 *  DESCRIPTION:  This function assumes a transaction has already been
 *                identified as a Visa Electron transaction.  It will
 *                perform the following checks that are required of a
 *                Visa Electron transaction:
 *
 *                   POS Entry Mode must be 90x (card swipe)
 *                   Expiration date from track 2 is validated
 *                   Service code is verified to be that of a VE card
 *                   A flag is set for Txcntl to perform CVV checks
 *
 *                No need to perform these checks for reversals or ATM Confirms.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if validation passed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_visa_electron( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   INT   ret_val = false;

   /* Do not validate reversals or ATM Confirms. */
   if ( (p_auth_tx->TLF01_details.message_type[1] == '4') ||
        (p_auth_tx->TLF01_details.tx_key == AUTH_ATM_CONFIRM) )
   {
      ret_val = true;
   }
   else
   {
      /* Check POS Entry Mode to be card swipe. */
      if ( 0 != strncmp(p_auth_tx->TLF01_details.pos_entry_mode+1,MAGNETIC_STRIPE,2))
      {
         strcpy( p_auth_tx->TLF01_details.response_code, NOT_PERMITTED );
         sprintf( error_msg,
                 "Invalid POS Entry Mode for Visa Electron: %s",
                  p_auth_tx->TLF01_details.pos_entry_mode );
      }
      else
      {
         /* Verify there is track data. */
         if ( (p_auth_tx->TLF01_details.track1[0] != 0x00) ||
              (p_auth_tx->TLF01_details.track2[0] != 0x00)  )
         {
            /* Validate expiration date */
            if ( false == validate_date_yymm(p_auth_tx->TLF01_details.exp_date,
                                             error_msg) )
            {
               strcpy( p_auth_tx->TLF01_details.response_code, EXPIRED_CARD );
               sprintf( error_msg,
                       "VE - invalid expiry date. Card %s, %s",
                        p_auth_tx->TLF01_details.card_num, error_msg );
            }

            /* Validate Service Code */
            else if ( 0 != strcmp(ServiceCode, VISA_ELECTRON_SERVICE_CODE) )
            {
               strcpy( p_auth_tx->TLF01_details.response_code, DO_NOT_HONOR );
               sprintf( error_msg,
                       "Invalid Service Code for Visa Electron: %s, Card: %s",
                        ServiceCode,
                        p_auth_tx->TLF01_details.card_num );
            }
            else
            {
               /* VE has been validated. Tell the authorizer to check CVV. */
               p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';
               ret_val = true;
            }
         }
         else
         {
            /* Cannot have VE without track data. */
            strcpy( p_auth_tx->TLF01_details.response_code, NOT_PERMITTED );
            strcpy( error_msg, "Invalid VE txn - no track data." );
            sprintf( error_msg,
                    "Invalid VE txn - no track data. Stan = %s, RRN = %s",
                     p_auth_tx->TLF01_details.sys_trace_audit_num,
                     p_auth_tx->TLF01_details.retrieval_ref_num );
         }
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         validate_date_yymm
 *
 *  DESCRIPTION:  This function validates that a date in the format of YYMM
 *                has a valid year (numeric and greater than 0) and a valid
 *                month (numeric and ranging from 01 to 12).
 *
 *  INPUTS:       date - Date to be validated
 *
 *  OUTPUTS:      err_buf - Indicates what is wrong with the input
 *
 *  RTRN VALUE:   True if good date, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT validate_date_yymm( pBYTE date, pCHAR err_buf )
{
   INT  ret_val = false;
   INT  iYear;
   INT  iMonth;
   BYTE temp_year[3];
   BYTE temp_month[3];

   memset( temp_year,  0x00, sizeof(temp_year)  );
   memset( temp_month, 0x00, sizeof(temp_month) );

   memcpy( temp_year,  date,   2 );
   memcpy( temp_month, date+2, 2 );

   iYear  = atoi( temp_year  );
   iMonth = atoi( temp_month );

   if ( false == isnum(temp_year) )
      sprintf( err_buf, "Exp Date: Invalid year - not numeric: %s", temp_year );
   else if ( false == isnum(temp_month) )
      sprintf( err_buf, "Exp Date: Invalid month - not numeric: %s",temp_month);
   else if ( (iYear < 0) || (iYear > 60 ) )
      sprintf( err_buf, "Exp Date: Invalid year - not current: %s", temp_year );
   else if ( (iMonth < 1) || (iMonth > 12) )
      sprintf( err_buf, "Exp Date: Invalid month: %s", temp_month );
   else if ( 2 != strlen(temp_year) )
      sprintf( err_buf, "Exp Date: Invalid year - not 2 digits: %s", temp_year);
   else if ( 2 != strlen(temp_month) )
      sprintf( err_buf,"Exp Date: Invalid month - not 2 digits: %s",temp_month);
   else
      ret_val = true;

   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         TXN_IS_BLOCKED
 *
 *  DESCRIPTION:  This function determines is a transaction should be blocked.
 *                This is an Equitable specific requirement.  They do not want
 *                ATM or Cash Advance transactions coming from local banks in
 *                the Phillippines.  Block the transaction if:
 *
 *                Acquiring Institution Country Code (DE19) is 608   AND
 *                Merchant Category Code (MCC, DE18) is one of these:
 *                   6010, 6011, 6012, 6051
 *
 *                NOTE:  This function assumes a txn switched in from Visa
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if transaction should be blocked (not allowed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT txn_is_blocked( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   CHAR  BLOCKED_MCC[][5] = { "6010", "6012", "6051", "done" };

   INT   ret_val = false;
   INT   i = 0;

   /* Check is country code is Phillippines. */
   if ( 0 == strcmp(p_auth_tx->country_code,PESO) )
   {
      /* YES, transaction originated locally within the Phillippines.
       *
       * Loop through the list of blocked MCC's.
       */
      while( BLOCKED_MCC[i][0] != 'd' )
      {
         if ( 0 == strcmp(p_auth_tx->TLF01_details.category_code,BLOCKED_MCC[i]) )
         {
            /* Need to block this transaction. */
            ret_val = true;

            sprintf( error_msg,
                    "Blocked txn from Visa: MCC=%s, CC=%s, Stan=%s, RRN=%s",
                     p_auth_tx->TLF01_details.category_code,
                     p_auth_tx->country_code,
                     p_auth_tx->TLF01_details.sys_trace_audit_num,
                     p_auth_tx->TLF01_details.retrieval_ref_num );

            break;
         }
         i++;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         SEND_TRANSACTION_STATISTICS
 *
 *  DESCRIPTION:  This function will send transaction statistics to Monitor.
 *
 *  INPUTS:       p_msg - This is the request from Monitor to send stats
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT send_transaction_statistics( pPTE_MSG p_msg )
{
   INT   ret_code;
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[512]={0};

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

   /* Send statistics to Monitor */
   memset( err_msg, 0x00, sizeof(err_msg) );
   ret_code = build_and_send_request_message( MT_GET_STATS_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              originator_queue, 
                                              (pBYTE)&Monitor_Stats,
                                              sizeof(MONITOR_STATS), 
                                              0,
                                              originator_info,
                                              err_msg );
   if ( false == ret_code  )
   {
      ncvisa_log_message( 3, 2, err_msg , "send_transaction_statistics" , 0 ,"");
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         RESET_TRANSACTION_STATISTICS
 *
 *  DESCRIPTION:  This function will set the transaction statistics to zero.
 *                This is the number of transactions received from the host,
 *                and the number of transactions this network controller has
 *                sent to the host.  These include number of approved, timed
 *                out, reversed, and declined txns.  Then this function sends
 *                these newly initialized values to the Monitor.
 *
 *                NOTE: The message being sent to the originator is a response
 *                to a get stats message.  This is the correct response to a
 *                reset stats message.
 *
 *  INPUTS:       p_msg - This is the request from Monitor to reset stats
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT reset_transaction_statistics( pPTE_MSG p_msg )
{
   INT   ret_code;
   BYTE  originator_queue[17]="";
   BYTE  originator_info[33] ="";
   CHAR  err_msg[512] = {0};

   /* Reset the statistics */
   (void)reset_stats( &Txn_Counts );

   /* Gather the statistics */
   (void)get_txn_stats( &Txn_Counts, &Monitor_Stats );

   /* Gather originator information */
   strcpy( originator_queue, ptemsg_get_msg_orig_queue ( p_msg ) );
   strcpy( originator_info,  ptemsg_get_msg_orig_comms_info (p_msg) );

   /* Send statistics to Monitor */
   memset( err_msg, 0x00, sizeof(err_msg) );
   ret_code = build_and_send_request_message( MT_GET_STATS_RESPONSE, 
                                              ST1_NONE,
                                              ST2_NONE,
                                              originator_queue, 
                                              (pBYTE)&Monitor_Stats,
                                              sizeof(MONITOR_STATS), 
                                              0,
                                              originator_info,
                                              err_msg );
   if ( false == ret_code  )
   {
      ncvisa_log_message( 3, 2, err_msg , "reset_transaction_statistics" , 0 ,"");
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         INCREMENT_TRAN_ID
 *
 *  DESCRIPTION:  This function locates the last character of the transaction
 *                id and checks to see if it is an integer.  If it is, then it
 *                increments the digit, rolling over to 0 from 9.  If it is not
 *                a digit, then it is set to 0.
 *
 *                This function can be used when a transaction needs to be
 *                inserted into TLF01 a second (or third,...) time due to
 *                reasons such as repeats or rejects.
 *
 *  INPUTS:       p_auth_tx - Transaction structure containing transaction_id
 *
 *  OUTPUTS:      p_auth_tx - Transaction structure with updated transaction_id
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void increment_tran_id( pAUTH_TX p_auth_tx )
{
   INT   len;
   BYTE  last_tran_id;
   BYTE  tran_id[21] = "";

   memcpy( tran_id, p_auth_tx->TLF01_details.primary_key.transaction_id, 20 );
   len = strlen( tran_id );

   if ( len > 0 )
   {
      last_tran_id = tran_id[len-1];

      if ( !isdigit(last_tran_id) )
         last_tran_id = '0';

      else if ( last_tran_id == '9' )
         last_tran_id = '0';

      else
         ++last_tran_id;

      tran_id[len-1] = last_tran_id;

      memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id, tran_id, 20);
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         SET_HOST_STATE
 *
 *  DESCRIPTION:  This function sets the Acquiring and Issuing host states
 *                based on input parameters for each. If one is changing
 *                but the other is not, then set the input to vSTAY. The
 *                shared memory counters are inputs also so they can be
 *                updated at the same time.
 *
 *  INPUTS:       acq_state   - vONLINE, vOFFLINE, or vSTAY
 *                iss_state   - vONLINE, vOFFLINE, or vSTAY
 *                timeouts    - Number of consecutive time outs
 *                active_txns - Number of active transactions
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT set_host_state( BYTE  acq_state, BYTE  iss_state,
                    pBYTE timeouts,  pBYTE active_txns )
{
   INT    ret_val = true;
   INT    update_status = true;
   LONG   retcode;
   BYTE   net_consec_tmouts_ctr[4] = "";
   BYTE   active_txns_ctr[4]       = "";
   BYTE   current_state[2]         = "";
   BYTE   prev_src_station[4]      = "";
   BYTE   host_state;

   /* Get current host state and counters */
   ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                  active_txns_ctr, current_state, prev_src_station );

   host_state = current_state[0];
   switch( host_state )
   {
      case cBOTH_ONLINE:

         if ( (acq_state == vONLINE) || (acq_state == vSTAY) )
         {
            if ( (iss_state == vONLINE) || (iss_state == vSTAY) )
               update_status = false;
            else
            {
               //host_state = cISSOFF_ACQON;
               host_state = cBOTH_OFFLINE;
               strcpy( timeouts,    "0" );
               strcpy( active_txns, "0" );
            }
         }
         else
         {
            if ( (iss_state == vONLINE) || (iss_state == vSTAY) )
               //host_state = cISSON_ACQOFF;
            	update_status = false;
            else
            {
               host_state = cBOTH_OFFLINE;
               strcpy( timeouts,    "0" );
               strcpy( active_txns, "0" );
            }
         }
      break;

      case cBOTH_OFFLINE:

         if ( (acq_state == vOFFLINE) || (acq_state == vSTAY) )
         {
            if ( (iss_state == vOFFLINE) || (iss_state == vSTAY) )
               update_status = false;
            else
            {
               //host_state = cISSON_ACQOFF;
               host_state = cBOTH_ONLINE;
               strcpy( timeouts,    "0" );
               strcpy( active_txns, "0" );
            }
         }
         else
         {
            if ( (iss_state == vOFFLINE) || (iss_state == vSTAY) )
               //host_state = cISSOFF_ACQON;
            	update_status = false;
            else
            {
               host_state = cBOTH_ONLINE;
               strcpy( timeouts,    "0" );
               strcpy( active_txns, "0" );
            }
         }
      break;

      case cISSON_ACQOFF:

         if ( (acq_state == vOFFLINE) || (acq_state == vSTAY) )
         {
            if ( (iss_state == vONLINE) || (iss_state == vSTAY) )
               update_status = false;
            else
            {
               host_state = cBOTH_OFFLINE;
               strcpy( timeouts,    "0" );
               strcpy( active_txns, "0" );
            }
         }
         else
         {
            if ( (iss_state == vONLINE) || (iss_state == vSTAY) )
            	update_status = false;
            else
            {
               //host_state = cISSOFF_ACQON;
               host_state = cBOTH_OFFLINE;
               strcpy( timeouts,    "0" );
               strcpy( active_txns, "0" );
            }
         }
      break;

      case cISSOFF_ACQON:

         if ( (acq_state == vONLINE) || (acq_state == vSTAY) )
         {
            if ( (iss_state == vOFFLINE) || (iss_state == vSTAY) )
               update_status = false;
            else
            {
               host_state = cBOTH_ONLINE;
               strcpy( timeouts,    "0" );
               strcpy( active_txns, "0" );
            }
         }
         else
         {
            if ( (iss_state == vOFFLINE) || (iss_state == vSTAY) )
            	update_status = false;
            else
            {
               //host_state = cISSON_ACQOFF;
               host_state = cBOTH_ONLINE;
               strcpy( timeouts,    "0" );
               strcpy( active_txns, "0" );
            }
         }
      break;

      case cDOWN:
      case cQUERYING:

         strcpy( timeouts,    "0" );
         strcpy( active_txns, "0" );
         if ( acq_state == vONLINE )
         {
            if ( iss_state == vONLINE )
               host_state = cBOTH_ONLINE;
            else
               //host_state = cISSOFF_ACQON;
               host_state = cBOTH_OFFLINE;
         }
         else
         {
            if ( iss_state == vONLINE )
               //host_state = cISSON_ACQOFF;
               host_state = cBOTH_ONLINE;
            else
               host_state = cBOTH_OFFLINE;
         }
      break;
      default:
    	  host_state = cBOTH_OFFLINE;
   }

   /* Ready to update the host state. But first check the counters. */
   if ( timeouts[0] != 0x00 )
   {
      if ( 0 != strcmp(timeouts, net_consec_tmouts_ctr) )
      {
         strcpy( net_consec_tmouts_ctr, timeouts );
         update_status = true;
      }
   }

   if ( active_txns[0] != 0x00 )
   {
      if ( 0 != strcmp(active_txns, active_txns_ctr) )
      {
         strcpy( active_txns_ctr, active_txns );
         update_status = true;
      }
   }

   /* If any changes occurred, update shared memory. */
   if ( update_status == true )
   {
      current_state[0] = host_state;
      retcode = WriteVisaTable( SHARED_MEMORY_TBL,
                                net_consec_tmouts_ctr,
                                active_txns_ctr,
                                current_state,
                                prev_src_station );
      if ( retcode != 0 )
         ret_val = false;
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_0302_REQUEST
 *
 *  DESCRIPTION:  This function begins the process of sending an 0302 message
 *                to Visa. The 0302 transaction is built based on information
 *                sent from dcpiso, information gathered from NCF01, and some
 *                data that is created here.  Once built, the 0302 is sent to
 *                Visa.
 *
 *  INPUTS:       p_msg - This is the incoming message from the device.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false if a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT process_0302_request( pPTE_MSG p_msg )
{
   pPTE_MSG_DATA   p_msg_data  = NULL_PTR;
   pBYTE           p_data_data = NULL_PTR;
   LONG            data_len;
   INT             i;
   INT             len;
   INT             ret_val = true;
   CHAR            nmi_code[5]   = {0};
   CHAR            err_mesg[512] = {0};
   CHAR            time_str [ 7] = {0};
   CHAR            time_date[25] = {0};
   CHAR            err_msg  [41] = {0};
   pBYTE           orig_dialog      = NULL_PTR;
   pBYTE           orig_dialog_info = NULL_PTR;
   AUTH_TX         auth_tx;

   /* Parse into Auth_Tx. */
   p_msg_data  = ptemsg_get_pte_msg_data( p_msg );  
   p_data_data = ptemsg_get_pte_msg_data_data( p_msg_data );
   data_len    = ptemsg_get_pte_msg_data_data_length( p_msg_data );
   copy_to_req_resp_buffer( p_data_data, data_len );

   memset( &auth_tx, 0x00, sizeof(AUTH_TX) );
   if ( true == hhutil_parse_host_response_msg(&auth_tx) )
   {
      memset( ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
      (void)move_iso_response_to_auth_struct( nmi_code, &auth_tx );

      /* Populate Auth_Tx with values generated by this module. */
      strcpy( auth_tx.TLF01_details.terminal_id, TID_0302 );
      init_txn( &auth_tx );
      strcpy( auth_tx.TLF01_details.dispensation.origin, "LO" );
      create_rrn( auth_tx.TLF01_details.retrieval_ref_num, &auth_tx );
      auth_tx.TLF01_details.tx_key = AUTH_CARD_UPDATE;

      memcpy( auth_tx.TLF01_details.tpdu_id,
              ncf01_i.tpdu_id,
              sizeof(ncf01_i.tpdu_id) );

      /* Get originating information */
      strcpy( auth_tx.TLF01_details.handler_queue,
              ptemsg_get_msg_orig_queue( p_msg ) );

      strcpy( auth_tx.TLF01_details.originator_info,
              ptemsg_get_msg_orig_comms_info( p_msg ) );

      /* Find the original dialog queue. It is appended to ReqRsp_Buffer. */
      orig_dialog = ReqRsp_Buffer;
      len = strlen( ORIG_QUEUE );
      for ( i=0; i<buflen; i++ )
      {
         if ( 0 == strncmp(orig_dialog,ORIG_QUEUE,len) )
         {
        	 buflen = i;
        	 break;
         }
         else
            orig_dialog++;
      }
      orig_dialog += strlen( ORIG_QUEUE );
      len = orig_dialog[0];
      if ( len < sizeof( auth_tx.TLF01_details.originator_queue ) )
         memcpy( auth_tx.TLF01_details.originator_queue, orig_dialog+1, len );

      /* Check Host State */
      ret_val = verify_host_app_status( &auth_tx );
      if ( ret_val == true )
      {
         ret_val = send_request_to_host( &auth_tx );
      }
      else
      {
         /* Host is not available.  Send response. */
         ret_val = false;
         strcpy( err_msg, "Unable to issue 0302 to Visa. Host Offline." );
         ncvisa_log_message( 3, 2, err_msg , "process_0302_request" , 0 ,"");
         issue_0302_response( HOST_UNAVAILABLE, &auth_tx );
      }
   }
   else
   {
      ret_val = false;
      strcpy( err_msg, "Unable to parse 0302 msg" );
      ncvisa_log_message( 3, 2, err_msg , "process_0302_request" , 0 ,"");
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         PROCESS_0312_RESPONSE
 *
 *  DESCRIPTION:  This function handles 0312 messages.  These are the responses
 *                to 0302 messages - File Update Requests where card holder
 *                records can be added, deleted, updated, or inquired.
 *
 *  INPUTS:       p_auth_tx - Structure containing most of the transacation data
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void process_0312_response( pAUTH_TX p_auth_tx )
{
   /* Sends response and logs to TLF01. */
   decrement_active_txn_ctr( true );
   issue_0302_response( p_auth_tx->TLF01_details.response_code, p_auth_tx );
   return;
}

/******************************************************************************
 *
 *  NAME:         ISSUE_0302_RESPONSE
 *
 *  DESCRIPTION:  This function creates the response message and sends it
 *                to the originator.  Then logs the txn into TLF01.
 *
 *  INPUTS:       resp_code - Response code
 *                p_auth_tx - Transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void issue_0302_response( pBYTE resp_code, pAUTH_TX p_auth_tx )
{
   BYTE  bitmap[16] = {0xc2,0x20,0x00,0x01,0x0a,0x00,0x00,0x00,
                       0x00,0x80,0x00,0x20,0x08,0x00,0x00,0x02};
   CHAR cadencie_host_queue_name[]="ncvis2";

   pPTE_MSG   p_msg_term = NULL_PTR;
   INT        i;
   INT        len;
   INT        convert = false;
   INT        err_code;
   CHAR       errbuf[512]={0};
   LONG       ret_code;
   CHAR	     Buffer[512] = {0};
   BYTE ReqRsp_Buffer_temp [COMM_BUF_SZE];
   INT    buflen_temp;

   /*if(strncmp(p_auth_tx->TLF01_details.handler_queue, cadencie_host_queue_name,
	   strlen(cadencie_host_queue_name)) == 0)	// nirbhay:
   {
	insert_tlf01( p_auth_tx );
	return;
   }*/


   /* First log the error for declines. */
   if ( 0 != strcmp(resp_code, APPROVED) )
   {
      memset( errbuf, 0x00, sizeof(errbuf) );
      if ( 0 == strcmp(resp_code, TIMED_OUT) )
      {
         convert = true;
         sprintf( Buffer,
                 "0302 Time Out. Resp Code: %s",
                  resp_code );

         strcpy( errbuf, Buffer );
         strcpy( p_auth_tx->TLF01_details.response_code, TIMED_OUT );
      }
      else if ( 0 == strcmp(resp_code, HOST_UNAVAILABLE) )
      {
         convert = true;
         strcpy( Buffer, "0302 Txn not processed. Host Offline." );
         strcpy( errbuf, Buffer );
         strcpy( p_auth_tx->TLF01_details.response_code, HOST_UNAVAILABLE );
      }
      else
      {
         err_code = atoi( p_auth_tx->TLF01_details.resp_source );

         sprintf( Buffer,
                 "0302 Decline. Resp Code: %s, Error Code: %s",
                  p_auth_tx->TLF01_details.response_code,
                  p_auth_tx->TLF01_details.resp_source );

         sprintf( errbuf,
                 "Visa Error Code: %s",
                  p_auth_tx->TLF01_details.resp_source );
      }
      memcpy( p_auth_tx->TLF01_details.response_text, errbuf, 39 );
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf( Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		   p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvisa_log_message( 3, 2, Buffer , "issue_0302_response" , 0 ,p_auth_tx->TLF01_details.card_num);
   }

   /* Put the information into HOST_MESSAGE. */
   build_host_structure( p_auth_tx );

   /* On errors, fields 91 and 101 might be in ASCII. Need to be EBCDIC. */
   if ( convert == true )
   {
      HOST_MESSAGE.file_update[0] = ascii_ebcdic_conv(HOST_MESSAGE.file_update[0], A2E);

      len = HOST_MESSAGE.fname_len[0];
      for( i=0; i<len; i++ )
         HOST_MESSAGE.filename[i] = ascii_ebcdic_conv(HOST_MESSAGE.filename[i], A2E);
   }

   /* Now build the data stream to send to the terminal. */
   {
	  buflen_temp = buflen;
	  memcpy( &ReqRsp_Buffer_temp[0], &ReqRsp_Buffer[0], buflen);
      memset( ReqRsp_Buffer, 0x00, sizeof(ReqRsp_Buffer) );

      /* Message Type */
      ReqRsp_Buffer[0] = 0x03;
      ReqRsp_Buffer[1] = 0x12;

      /* Bitmap */
      memcpy( ReqRsp_Buffer+2, bitmap, 16 );
      buflen = 18;

      /* DE2 - Card Number */
      len = strlen( p_auth_tx->TLF01_details.card_num )/2 + 1;
      memcpy( ReqRsp_Buffer+buflen, HOST_MESSAGE.pan_len, len );
      buflen += len;

      /* DE7 - Greenwich Mean Time */
      memcpy( ReqRsp_Buffer+buflen, HOST_MESSAGE.gmt_time, 5 );
      buflen += 5;

      /* DE11 - System Trace Audit Number */
      memcpy( ReqRsp_Buffer+buflen, HOST_MESSAGE.stan_bin, 3 );
      buflen += 3;

      /* DE32 - Acquirer Institution ID */
      len = strlen( p_auth_tx->TLF01_details.acquiring_id )/2 + 1;
      memcpy( ReqRsp_Buffer+buflen, HOST_MESSAGE.acq_inst_len, len );
      buflen += len;

      /* DE37 - Retrieval Reference Number */
      memcpy( ReqRsp_Buffer+buflen, HOST_MESSAGE.rrn, 12 );
      buflen += 12;

      /* DE39 - Response Code */
      HOST_MESSAGE.responsecd[0] = ascii_ebcdic_conv(HOST_MESSAGE.responsecd[0], A2E);
      HOST_MESSAGE.responsecd[1] = ascii_ebcdic_conv(HOST_MESSAGE.responsecd[1], A2E);

      memcpy( ReqRsp_Buffer+buflen, HOST_MESSAGE.responsecd, 2 );
      buflen += 2;

      /* DE73 - Date, Action */
      memcpy( ReqRsp_Buffer+buflen, HOST_MESSAGE.action, 3 );
      buflen += 3;

      /* DE91 - File Update Code */
      memcpy( ReqRsp_Buffer+buflen,
              HOST_MESSAGE.file_update,
              1 );
      buflen += 1;

      /* DE101 - FILE NAME */
      len = HOST_MESSAGE.fname_len[0] + 1;
      memcpy( ReqRsp_Buffer+buflen, HOST_MESSAGE.fname_len, len );
      buflen += len;

      /* DE127 - FILE MAINTENANCE (FORMAT 2) */
      len = HOST_MESSAGE.file_rec_len[0] + 1;
      if ( len > 0 )
      {
         memcpy( ReqRsp_Buffer+buflen, HOST_MESSAGE.file_rec_len, len );
         buflen += len;
      }
   }

   /* Append original dialog Q to end of ReqRsp_Buffer: Tag, Length, Data. */
   len = strlen( ORIG_QUEUE );
   memcpy( ReqRsp_Buffer+buflen, ORIG_QUEUE, len );
   buflen += len;

   len = strlen( p_auth_tx->TLF01_details.originator_queue );
   ReqRsp_Buffer[buflen++] = len;
   memcpy(ReqRsp_Buffer+buflen, p_auth_tx->TLF01_details.originator_queue, len);
   buflen += len;

   /* Append original dialog Info to end of ReqRsp_Buffer: Tag, Length, Data. */
   len = strlen( ORIG_INFO );
   memcpy( ReqRsp_Buffer+buflen, ORIG_INFO, len );
   buflen += len;

   len = strlen( p_auth_tx->TLF01_details.originator_info );
   ReqRsp_Buffer[buflen++] = len;
   memcpy(ReqRsp_Buffer+buflen, p_auth_tx->TLF01_details.originator_info, len);
   buflen += len;

   /* Send response to device controller. */
   p_msg_term = ptemsg_build_msg( MT_INCOMING_GUI,
                                  ST1_NONE,
                                  ST2_NONE,
                                  p_auth_tx->TLF01_details.handler_queue,
                                  application_que_name,
								  ReqRsp_Buffer_temp,
								  buflen_temp,
                                  0 );
   if ( NULL_PTR == p_msg_term )
   {
      sprintf( errbuf,
              "Insufficient memory to build 0312 response to %s",
               p_auth_tx->TLF01_details.handler_queue );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		  p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(errbuf,Log_Bufr);
      ncvisa_log_message( 3, 2, errbuf , "issue_0302_response" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   else
   {
      ret_code = pteipc_send(p_msg_term,p_auth_tx->TLF01_details.handler_queue);
      if ( ret_code < 0 )
      {
         pteipc_get_errormsg( ret_code, errbuf );
         sprintf( Buffer,
                 "Error on pteipc_send to que %s: %s",
                  p_auth_tx->TLF01_details.handler_queue, errbuf );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		 p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvisa_log_message( 3,3, Buffer , "issue_0302_response" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
      free(p_msg_term);
   }

   /* Log into TLF01. */
   (void)insert_tlf01( p_auth_tx );

   return;
}

/******************************************************************************
 *
 *  NAME:         DECREMENT_ACTIVE_TXN_CTR
 *
 *  DESCRIPTION:  This function decrements the number of active transactions
 *                in shared memory.
 *
 *  INPUTS:       time_out_flag - If true, reset number of time outs also.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void decrement_active_txn_ctr( INT time_out_flag )
{
   INT         tempint;
   CHAR        tempstr[10];
   BYTE        net_consec_tmouts_ctr[4];
   BYTE        active_txns_ctr[4];
   BYTE        current_state[2];
   BYTE        prev_src_station[4];


   /* Get host state and counters */
   ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                  active_txns_ctr, current_state, prev_src_station );

   /* Decrement the active transactions counter field */
   tempint = atoi(active_txns_ctr) -1;
   if ( tempint < 0 )
   {
      /* Do not let it fall below zero. */
      strcpy( active_txns_ctr, "0" );
   }
   else
   {
      itoa(tempint, tempstr, 10);
      strcpy( active_txns_ctr, tempstr );
   }

   /* If the host responded to a request, then we can reset the timeout cnt. */
   if ( time_out_flag == true )
      strcpy( net_consec_tmouts_ctr, "0" );

   /* Update host state and counters */
   WriteVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                   active_txns_ctr, current_state, prev_src_station );
   return;
}

/******************************************************************************
 *
 *  NAME:         IS_TXN_REVERSAL
 *
 *  DESCRIPTION:  This function determines if a transaction is a reversal.
 *                This can be a regular reversal, a repeat reversal, an
 *                ATM Confirm, a reversal advice, or a void. This can also
 *                be a request or response.
 *
 *  INPUTS:       tx_key - Transaction type
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if transaction is a type of reversal, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT is_txn_reversal( BYTE tx_key )
{
   INT   retval;

   switch( tx_key )
   {
      case AUTH_REVERSAL:
      case AUTH_REVERSAL_RESPONSE:
      case AUTH_REVERSAL_REPEAT:

      case AUTH_REVERSAL_ADVICE:
      case AUTH_REVERSAL_ADVICE_RESPONSE:

	  case AUTH_VOID_PRE_AUTHORIZATION:
	  case AUTH_VOID_PRE_AUTHORIZATION_RESPONSE:

      case AUTH_VOID_SALE:
      case AUTH_VOID_SALE_RESPONSE:

      case AUTH_ATM_CONFIRM:

         retval = true;
         break;

      default:
         retval = false;
         break;
   }

   return( retval );
}

