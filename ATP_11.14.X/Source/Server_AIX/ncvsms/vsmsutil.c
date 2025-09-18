/*******************************************************************************
* Copyright (c) 2007, Hypercom, Inc.
* All Rights Reserved.
*  
* MODULE:       vsmsutil.c
*
* TITLE:       
*  
*	DESCRIPTION: This module contains all the utilities used my the 
*               host handler to  parse and manipulate all incoming 
*               messages from the terminal and the host.
*
* APPLICATION: 
*
* AUTHOR:       B. Gable
*
* REVISION HISTORY:
*
* $Log:   N:\POS\PVCS6.6\EPICPORTZ\PTE\Equitable\App_lib\vsmsutil.c  $

	  Rev 1.39   Apr 01 2009  20:23:00  Ajay S T, TF
   Updated version to 4.4.1.8
   Changed the code to prefix 'C' for field 28 for 0400 msg.

		 Rev 1.38   Mar 20 2009  11:23:00  Ajay S T, TF
   Updated version to 4.4.1.7
   Changed the code to support field 28 for ATM transaction.

		Rev 1.37   Jan 21 2009  17:23:00  Girija Y, TF
   Updated version to 4.4.1.6
   Changed the code for error handling for Fld 48 for 0422 message type to EQIt host.

	  Rev 1.36   Dec 8 2008  15:23:40  Girija Y, TF
   Updated version to 4.4.1.5
   Chaged the code to support 0422 message type to EQIt host.
   
      Rev 1.35   Feb 09 2007 15:19:00   DIrby
   Modified to make field 53.4 configurable from the tf.ini file.  This
   is 2-digit value that indicates which key was used to encrypt the pin.
   SCR 24144
   
      Rev 1.34   Oct 12 2004 16:09:40   dirby
   Modified to store Visa's header fields for switched in txns and
   then to echo fields 7-12 in the response.
   SCR 12755
   
      Rev 1.33   Mar 12 2004 15:03:48   dirby
   Modified parsing of field 48 usage 7a for chargebacks to get the
   whole field.  Previously we were not logging the last 3 chars.
   SCR 1363
   
      Rev 1.32   Dec 16 2003 10:31:38   dirby
   Modified building for field 38 to check first byte.  If null, do not
   copy the field into the HOST_MESSAGE buffer.  Sometimes this
   field contains zeroes after the first byte causing it to be built into
   the outgoing message buffer.
   SCR 1083
   
      Rev 1.31   Oct 15 2003 23:49:46   dirby
   Version 4.2.0.19, removed DE49 from 0430 replies to Visa.
   SCR 1083
   
      Rev 1.30   Oct 15 2003 02:01:14   dirby
   Fixed bug where DE62 was not being populated for reversals.
   SCR 1083
   
      Rev 1.29   Oct 15 2003 00:28:58   dirby
   Modified to distinguish between the two different kinds of administrative
   text messages (0600 and 0620).  They use the same tx_key.
   We need to support both of them.
   SCR 1083
   
      Rev 1.28   Oct 14 2003 11:12:04   dirby
   Made the following changes for certification:
   1.  Do not send DE62 in 0420 if reversal is due to a timeout.
   2.  Do not send DE48 in 0430 response to host.
   3.  Set DE66 to 9 for 0530; do not send 74-77, 86-89, 90, 97.
   SCR 1083
   
      Rev 1.27   Oct 14 2003 00:07:46   dirby
   1.  Modified to always send '0110' in DE22 for partial dispense.
   2.  Fixed bug to identify an 0800 echo from the host.  It was interpreting
   it as a key change.
   SCR 1083
   
      Rev 1.26   Oct 02 2003 13:23:02   dirby
   Fixed DE100 for 0600/0610.  It was not being sent; now it is.
   SCR 1083
   
      Rev 1.25   Oct 01 2003 16:19:26   dirby
   Fixed the following issues:
   1.  Echo DE49 and DE90 (stan) in 0430 response to Visa.
   2.  Added DE33 to 0432 Fee Collection/Funds Disbursement response.
   3.  Echo DE63.3 in 0220 Adjustment/Representment to Visa.
   4.  Do not send DE62.1 if not 'E' in 0220 Adjustment/Representment.
   5.  Log all of DE48 in 0422 Chargeback/Chargeback reversal.
   SCR 1083
   
      Rev 1.24   Sep 29 2003 13:42:08   dirby
   1.  Added support for Fee Collection and Funds Disbursement
       for Acquirers.
   2.  Modified to only use DE33 in 0600 or if DE32 is not present.
   3.  Modified to echo DE63.1 in reversals from 0210 response.
   4.  Modified DE63.3 to use the value from EB host for representments.
   SCR 1083
   
      Rev 1.23   Sep 25 2003 15:49:16   dirby
   Updated to support 0422/0432 Fee Collection and Funds
   Disbursement transactions from the Issuer.
   SCR 1083
   
      Rev 1.22   Sep 23 2003 17:45:06   dirby
   Modified to support 0420 Reversal Advices from the switch.
   SCR 1083
   
      Rev 1.21   Sep 19 2003 13:35:08   dirby
   Modified processing code to set it to '22' for cash disbursement
   adjustments.
   SCR 1083
   
      Rev 1.20   Sep 15 2003 17:58:48   dirby
   Modified to correct many different issues.
   SCR 1083
   
      Rev 1.19   Sep 11 2003 16:40:50   svelaga
   Fixed issues with Field 15, Field 48, Field 62, Field 90 and Field 100.
   Version upgraded to 4.2.0.9
   
      Rev 1.18   Sep 05 2003 11:45:36   mbysani
   1.		 0600 message, Field 32 and Field 33, Field 70, and Field 100 are not
   pass to VISA SMS. 
   		 Field 32 and Field 33 is not pass by EQIT host to ATP. Do you want
   us to send this to you or would you prefer getting the info from ncf01?
   		 Field 70 and Field 100 is pass by Eqit host to ATP but not sent out
   to VISA SMS.
   
      Rev 1.17   Sep 02 2003 09:19:20   mbysani
   Fixed
   For the 0422/0432 message, on the 0432 reply, ATP should just echo field
   90 from 0422 and not populate the field 90 with the system trace.
   
      Rev 1.16   Aug 29 2003 11:24:30   mbysani
   add the acq_id for all the tlf01 inserts
   
      Rev 1.15   Aug 29 2003 11:12:38   mbysani
   changed reversal repeat message type, reversal amount 
   
      Rev 1.14   Aug 26 2003 15:58:10   mbysani
   changed the bitmap for field 63, and removed reversal for adjustments and proc code
   
      Rev 1.13   Aug 25 2003 16:39:28   mbysani
   changed the logging into tlf01 and rrn been generated
   
      Rev 1.12   Aug 21 2003 09:16:32   mbysani
   re-assigned field 63.1 and add the acq id 
   
      Rev 1.11   Aug 05 2003 16:06:36   mbysani
   changed and added fields to tfl01
   
   Field 63.3 - Message Reason Code
    Field 63.5 - PLUS PMC ID
    Field 63.13 - New field in the specification.
    Field 68 - Receiving Institution Country Code
    Field 70 - Network Management Information Code 
    Field 100 - Receiving Institution ID Code
   
      Rev 1.10   Aug 04 2003 15:54:38   mbysani
   modified the recon message types values to different field in tlf01
   
      Rev 1.9   Jul 31 2003 17:18:58   mbysani
   changed the reversals
   
      Rev 1.8   Jul 31 2003 14:26:08   mbysani
   fixed field for voice auth, and support for key exchange message
   
      Rev 1.7   Jul 30 2003 17:00:20   mbysani
   updated field 48
   
      Rev 1.6   Jul 30 2003 13:47:32   mbysani
   updated with in qa 
   
      Rev 1.5   Jul 28 2003 10:37:52   mbysani
   updated when unit testing
   
      Rev 1.4   Jul 24 2003 13:45:28   mbysani
   fixed compilation error on aix
   
      Rev 1.3   Jul 23 2003 08:47:54   mbysani
   update as per visa sms spec
   
      Rev 1.2   Jul 17 2003 17:22:08   mbysani
   Updated for visa sms
   
      Rev 1.1   Jul 02 2003 17:58:38   mbysani
   updated with new messages
   
      Rev 1.193   Mar 25 2003 12:41:00   dirby
   Added 0x5F, which is '^' character, to the EBCDIC/ASCII
   conversion routine.
   SCR 1074
   
      Rev 1.192   Mar 13 2003 15:56:54   dirby
   Fixed a bug that gave an ugly error message on reject messages.
   SCR 1034
   
      Rev 1.191   Mar 13 2003 15:12:32   dirby
   Adding code to handle reject messages from Visa.  If a response
   is rejected, get the original txn from TLF01 and reverse out the
   transaction.  If a request to Visa is rejected, treat it as if Visa
   declined it with a format error response code.
   SCR 1034
   
      Rev 1.190   Mar 11 2003 15:05:42   dirby
   Modified function ascii_ebcdic_conv to include some additional
   special characters that were previously missing, such as: \, ~, and `.
   SCR 1051
   
      Rev 1.189   Mar 07 2003 15:40:32   dirby
   Modified fields 102 and 103 to use a flag to determine if they
   came in a request so they can be echoed in the response.  This
   is to avoid garbage in those fields that sometimes comes from
   the Equitable host; they cause a reject code from Visa of 112.
   SCR 1034
   
      Rev 1.188   Feb 20 2003 15:40:22   dirby
   Added code to get Service Code from track 1 data.  This is for VE txns.
   SCR 1033
   
      Rev 1.187   Dec 10 2002 15:55:40   dirby
   Reversal advices were not being inserted into TLF01.  The insert
   flag was being set for reversals, reversal repeats, and reversal
   advices.  Modified to not set it for reversal advices.
   SCR 845
   
      Rev 1.186   Dec 10 2002 14:21:34   dirby
   Added 1 line to prevent sending a response to Visa a second
   time in the event the original txn is not found for a reversal repeat.
   SCR 845
   
      Rev 1.185   Nov 22 2002 16:07:34   dirby
   Added 2 lines to make sure record is not inserted twice into
   TLF01 for reversals.
   SCR 845
   
      Rev 1.184   Nov 08 2002 11:40:04   dirby
   Added code to not insert record into TLF01 if it has already
   been inserted by the update_all process.
   SCR 845
   
      Rev 1.183   Oct 16 2002 11:46:20   dirby
   Added code to initialize TLF01 prior to inserting it into database if needed.  Also improved error messages where parsing
   track 2 is concerned.
   SCR 820
   
      Rev 1.182   Sep 16 2002 15:22:12   dirby
   Added code to process auth_repeats separately.  They need to
   look in NCF30 for the original auth txn in order to know how to
   process them.  If orig txn is found, do not process the repeat;
   just respond.  If orig txn is not found, process the repeat as a
   normal authorization.
   SCR 879
   
      Rev 1.181   Jul 11 2002 13:30:46   dirby
   Added code to log DE43 to TLF01 for switched out txns.
   SCR 775
   
      Rev 1.180   Jul 07 2002 00:49:08   dirby
   Modified to use precalculated 'sizeof' values instead of recalculating
   structure sizes over and over again.
   SCR 594
   
      Rev 1.179   Jul 05 2002 13:23:52   dirby
   1.  Added code to display a message for reversals.  SCR 621
   2.  Modified to get balance currency code for DE54 from host2_scr_fi_nbr.  SCR 823
   
      Rev 1.178   Jun 21 2002 15:45:04   dirby
   Modified to not respond to reversals if the original txn to be
   reversed is not found in NCF30.  This was causing an insert
   into TLF01 error due to unique constraint violation.
   SCR 793
   
      Rev 1.177   Jun 20 2002 15:57:08   dirby
   Modified to preserve original value of POS Condition Code.
   SCR 794
   
      Rev 1.176   Jun 18 2002 00:06:08   dirby
   Change validation check of DE38, approval number, to allow spaces.
   
      Rev 1.175   Jun 17 2002 10:26:20   dirby
   Set processing code to '00' for Card Verify.
   SCR 787
   
      Rev 1.174   Jun 17 2002 09:53:44   dirby
   Changed processing code to Visa for Card  Verifications from
   '38' to '00'.
   SCR 786
   
      Rev 1.173   Jun 13 2002 14:34:08   dirby
   Modified to log DE43 to TLF01:
      Merchant name goes into TLF01.card_holder_name
      Merchant location & country code go into TLF01.ncf01_retired_cwk.
   This is done so the information is in TLF01 for EB's use.
   SCR 775
   
      Rev 1.172   Jun 10 2002 15:28:16   dirby
   Added code to support 2 cases for Visa Pin Verification Service (PVS).
   Case 1:  Already support - Visa does not do PVS; the pin is passed
   to ATP; ATP verifies the pin.
   CASE 2: New - Visa does PVS; the pin is not passed to ATP.  In
   this case, DE39 will be set to '00' and DEs 52 and 53 will not exist.
   In this case, we allow the txn to continue, without a pin.  Txcntl
   will only verify mag stripe, not the pin.  In case 1, txcntl will do both.
   SCR 770
   
      Rev 1.171   Jun 06 2002 09:20:32   dirby
   Changed DE39 so that response code 19 is not sent in an 0410
   txn.  It is changed to '21', because '19' is not allowed in 0410.
   SCR 764
   
      Rev 1.170   Jun 05 2002 16:14:58   dirby
   1.  Modified to log DE19 to TLF01 field 'num_dbsales'.  SCR 765
   2.  Modified to send response 21 instead of 19 for time outs.  SCR 764
   3.  Modified to send a response to a reversal back to Visa without
        waiting for a response from txcntl.
   SCR  763
   
      Rev 1.169   Jun 03 2002 01:10:06   dirby
   Modified to allow 3rd currencies for balance inquires.  Must
   use the USD account in these cases.
   SCR 760
   
      Rev 1.168   Jun 03 2002 00:30:16   dirby
   Modified DE95 to be echoed in response if it came in a request.
   SCR 759
   
      Rev 1.167   Jun 02 2002 23:43:24   dirby
   Modified to not send out DE95 in 0410's if DE95 was not in the 0400.
   SCR 758
   
      Rev 1.166   May 31 2002 18:27:32   dirby
   Modified to get account type from ACF01.  Use that to set
   currency code in DE54 responses.
   SCR 754
   
      Rev 1.165   May 31 2002 15:03:54   dirby
   Added code to set the currency code in DE54 of responses to
   the Issuer's currency.  This will be Peso or USD, depending on
   which account is used.
   SCR 754
   
      Rev 1.164   May 31 2002 14:09:18   dirby
   Modified to, as an Issuer, echo field 9 of Visa's header when
   sending out a response to Visa.  Do this even though the Visa
   spec indicates to set to all zeroes; during Visa Cert., EB was
   told they need to echo this field back to Visa.
   SCR 755
   
      Rev 1.163   May 30 2002 14:53:48   dirby
   Rewrote the code in function 'move_iso...' for DE54 to parse
   up to 4 sets of balances.  There can be 2 in the Issuer currency
   and 2 in the Acquirer currency (current and available).  Refer to
   the comments at Field 54 in that function for a full description.
   SCR 750.
   
      Rev 1.162   May 24 2002 16:07:36   dirby
   Revised the logging to TLF01.  See large comment box after
   DE126 in move_iso...  function.
   SCR 745
   
      Rev 1.161   May 23 2002 16:36:10   dirby
   Added bit definition 126.
   Added function Field126Parser to parse DE126 into voice_auth_text
   in TLF01.  No need to process this data; just store it in TLF01.
   Added code to store DE60 into TLF01.nfi_name.
   SCR 743
   
   Reversal responses were not sending a 2nd bitmap to Visa,
   therefore DE95 was not being echoed.  Modified to set 2nd
   bitmap if DE95 exists.
   SCR 744
   
      Rev 1.160   May 15 2002 14:26:04   dirby
   Corrected a bug where Visa Tran ID was being converted from
   ASCII to BCD for DE62 reversals.
   SCR 725
   
      Rev 1.159   May 15 2002 13:14:12   dirby
   Changed Visa Tran ID to be BCD (from EBCDIC) when building
   DE62 for reversals.
   SCR 725
   
      Rev 1.158   May 15 2002 12:06:12   dirby
   Added code to store CPS results (DE62) into fields that get
   stored into NCF30 for reversals.  Also added code to get these
   results and use them in reversals.  Field 62 is mandatory in
   reversals.  Also, removed DE61 from reversals.
   SCR 725
   
      Rev 1.157   Apr 24 2002 11:20:26   dirby
   Modified function incoming_other_host_response to not overwrite
   a response code of 'not permitted'.
   SCR 703
   
      Rev 1.156   Apr 21 2002 15:20:12   dirby
   Modified to support reversal repeats as an Acquirer.
   SCR 701
   
      Rev 1.155   Apr 19 2002 21:04:44   dirby
   Removed DE4 (Amount) from ATM Balance Inquiry requests
   going to Visa host.
   SCR 710
   
      Rev 1.154   Apr 19 2002 15:52:40   dirby
   Modified to send DE54 in ATM responses only.  Previously was
   going out in POS Cash responses also.  The first position of
   DE60 is used to determine if transaction is an ATM.
   SCR 702
   
      Rev 1.153   Apr 12 2002 16:42:02   dirby
   Fixed bug where original DE7 was being overwritten with the
   current DE7.
   SCR 699
   
      Rev 1.152   Mar 06 2002 13:12:48   dirby
   Removed DE90 from reversal responses.  This field is optional,
   and if a request comes in with all zeroes in DE90 (except for
   orig msg type), it does not get echoed out properly.
   SCR 616
   
      Rev 1.151   Mar 06 2002 10:30:16   dirby
   Added code to check stan in DE90 (for requests from host).
   If all zeroes, use DE11 as the original stan.  This is for looking
   up original transactions on reversals.
   SCR 616
   
      Rev 1.150   Mar 05 2002 15:08:08   dirby
   Modified the MCC validation to be set either on or off, based on
   a flag set at startup.  Right now, Equitable wants it off.  Do not
   validate MCC for switched in transactions.
   SCR 648
   
      Rev 1.149   Feb 01 2002 10:32:50   dirby
   Modified to set DE25 to '08' if Industry code is '800'.
   SCR 609
   
      Rev 1.148   Jan 17 2002 14:38:22   dirby
   Added code to store original amount, partial amount, and
   original currency code so a record is kept of them prior to
   currency conversion taking place.     SCR 565
   
      Rev 1.147   Jan 16 2002 15:33:10   dirby
   Added transaction tracing functionality.
   SCR 546
   
      Rev 1.146   Jan 14 2002 18:22:34   dirby
   Modified the transaction statistic functions to keep statistics
   on number of approved, declined, timed out, and reversed
   txns.  These stats are displayed on Monitor.
   SCR 484
   
      Rev 1.145   Jan 07 2002 13:05:38   dirby
   Modified to automatically issue a reversal request on time outs.
   SCR 569 - Send automatic reversal on time outs
   
      Rev 1.144   Dec 03 2001 16:14:00   dirby
   Modified field 32 so that it only gets stored into TLF01 if it is not
   already populated.     SCR 414
   
      Rev 1.143   Oct 24 2001 15:06:02   dirby
   Modified to only send out balances in DE54 when txn is approved.   SCR 515
   
      Rev 1.142   Oct 23 2001 17:30:58   dirby
   Implemented the use of CVV_TEST_FLAG to see if CVV
   verification has been performed on a txn.  This affects the
   value in DE44 for responses.   SCR 514
   
      Rev 1.141   Oct 23 2001 16:14:30   dirby
   Added field 44 to balance inquiry response.  This gives the
   result of CVV verification.     SCR 514
   
      Rev 1.140   Oct 23 2001 13:01:56   dirby
   Modified building of field 60 to be '0000000007' for ecommerce
   transactions.  Used to be '5100080007'.
   Also implemented macro definitions for industry codes.
   SCR 509
   
      Rev 1.139   Oct 11 2001 16:11:04   dirby
   Modified to get dispensed amount from DE61 for ATM confirms.   SCR 496
   
      Rev 1.138   Sep 27 2001 09:41:06   dirby
   Modified function build_host_request so that responses to 
   Cash Advances will conditionally have 2 bitmaps.
   
      Rev 1.137   Aug 15 2001 14:56:22   SYARLAGA
   Modified field 33 in move_iso_response_to_auth_struct  function and     field 32 in build_host_structure function.
   
      Rev 1.136   Jun 21 2001 16:53:50   SYARLAGA
   Modified field60 in  build_host_structure function.And Added 				 CardCategoryCode == 6010  in  validate_mcc function.
   
      Rev 1.135   Jun 19 2001 13:47:20   SYARLAGA
   Modified fields 35 and 45 in move_iso_response_to_auth_struct. function and modified validate_mcc function
   
      Rev 1.134   Jun 18 2001 11:00:00   SYARLAGA
   Modified validate_mcc function.
   
      Rev 1.133   Jun 11 2001 09:29:26   SYARLAGA
   Modified field 95
   
      Rev 1.132   Jun 06 2001 14:25:04   SYARLAGA
   Modified field 49 and field 54
   
      Rev 1.131   May 31 2001 14:10:32   SYARLAGA
   Modified field 95 coming from visa
   
      Rev 1.130   May 23 2001 15:43:06   SYARLAGA
   Added field 125 in bitfield_data
   
      Rev 1.129   May 21 2001 13:28:34   dirby
   1.  Corrected the way a response is returned to Visa if MCC validation fails.
       By removing the code and calling 'incoming_other_host_response_db_reply'
       currency codes and amounts are properly echoed.
   2.  Corrected some indentation in same function.
   3.  Added terminal, card, and merchant to error msgs in same function.
   4.  Reworked the way field 44 is built for responses so that if CVV2
       is not validated (for any reason), then field 44.5 is not populated.
   
      Rev 1.128   May 09 2001 09:55:16   SYARLAGA
   Modified field 11.So that the stan in  voice authorization reversal is the stan of the original transaction
   
      Rev 1.127   May 07 2001 07:59:56   dirby
   Added a line to set length of field 44 for a response.
   
      Rev 1.126   Apr 30 2001 16:53:06   SYARLAGA
   Modified field 44.
   
      Rev 1.125   Apr 25 2001 09:25:34   SYARLAGA
   Fixed bug in track2 data to accept more than 32 characters.
   
     
   
      Rev 1.124   Apr 16 2001 15:07:00   SYARLAGA
   Creating  new time stamp and storing the old time stamp in original date and time
   
      Rev 1.123   Mar 13 2001 13:24:18   dirby
   In 'build_host_structure':
   1.  Modified field 95 to use the total amount minus reversal amount.
       This equates to the actual amount credited.  Cannot use actual_amount
       because it gets overwritten.
   
   2.  Modified field 61 so that the length is divided by 2.  It needs to
       reflect a BCD length, not an ASCII length.  This field will always
       be an even number of bytes.
   
      Rev 1.122   Feb 06 2001 12:32:18   dirby
   Modified the definition of fields 41 and 42 to be conditional, not mandatory.
   Modified DE95 to only be populated if reversal amount is less than total amount.
   This means it is a partial reversal.
   
      Rev 1.121   Jan 31 2001 09:31:50   dirby
   1.  Modified to echo RTI of Visa's header in the response of a switched in transaction.
   2.  Fix bug that was preventing reversals from working.  It was looking for card number
   in track data, but reversals have no track data.
   
      Rev 1.120   Jan 04 2001 17:08:34   dirby
   Modified field 43 so it is sent out for POS transactions.  This is the
   way EB's current system does it; so they want us to do so also.  Ideally,
   DE43 is not sent for POS transactions if merchant location (DE43) is the
   same country as the acquirer (DE19).
   
      Rev 1.119   Oct 18 2000 15:28:32   dirby
   1.  Consolidated initialization code for switched in transactions into 'init_txn'.
   2.  Removed commented out code.
   3.  Relocated host_finish_time to be more accurate.
   
      Rev 1.118   Sep 19 2000 16:37:54   dirby
   Updated Visa to support RPS indicator:
      For RPS transactions, DE25 is set to '08'.
      For RPS transactions, DE60, subfields 9 & 10 are set to '02'.
   
      Rev 1.117   Aug 25 2000 16:03:54   dirby
   Removed a message that got sent to Monitor.  It was just for test purposes apparently.
   
      Rev 1.116   Aug 21 2000 16:08:50   dirby
   Added code to support transaction statistics for Monitor.
   
      Rev 1.115   Aug 09 2000 16:57:24   dirby
   Added code to convert pos_entry_mode (DE22) from Visa standard to Hypercom Standard
   for switched in txns.
   
      Rev 1.114   Aug 09 2000 16:13:02   dirby
   Added code to set POS_Entry_Mode (first 2 bytes) to '00' for satellite offices.
   Visa requires this of Equitable.  Equitable uses organization Id of '0001' to
   indicate a merchant is a satellite merchant.
   
      Rev 1.113   Jul 31 2000 15:41:34   dirby
   Modified SQLs into NCF30 to use card number as part of the key.
   
      Rev 1.112   Jul 30 2000 14:43:10   dirby
   Reworked the Log ON/OFF logic to show accurate status
   on Monitor, and to be a little more efficient.
   
      Rev 1.111   Jun 30 2000 14:25:40   dirby
   1.  Reworked the parsing of track2 because, due to
       Visa Electron, this code gets reused.
   2.  Reworked the parsing of track1 to skip over the 1st
       character, which is a Format Code (should be 'B').
   3.  Reworked some error msgs to make them more
       readable in the code.
   
      Rev 1.110   Jun 25 2000 02:00:46   dirby
   Removed the functionality that gets NCF01 every request and response.
   It gets NCF01 only at start up now, never during an online transaction.
   
      Rev 1.109   Jun 24 2000 23:39:12   dirby
   Added ATM CONFIRM functionality.
   
      Rev 1.108   Jun 14 2000 17:33:26   dirby
   Added stuff to validate MCC.
   
      Rev 1.107   Mar 28 2000 14:48:46   ddabberu
   1. get_visa_data() changed.
   2. build_host_Structure() modified to send fld95 only if it has some value in it.
   
      Rev 1.106   Mar 21 2000 10:52:22   ddabberu
   error_msg sent to syslog/monitor when update fails
   
      Rev 1.105   Mar 20 2000 17:25:36   ddabberu
   1.Echo fld90 if sent for reversals
   2.fld44.5 should be null if trk1/trk2 not present.
   3.Send fld38 in responses only when fld39 is 00.
   
      Rev 1.104   Feb 24 2000 16:08:12   ddabberu
   The build_host_structure() changed to support e-commerce industry code (700).
   
   
      Rev 1.103   Feb 16 2000 15:34:54   svelaga
   Logging into TLF01, even the send transaction 
   to host failed.
   
   
      Rev 1.102   Feb 11 2000 16:43:00   svelaga
   Deleted the double delete for reversal from 
   NCF30.
   
      Rev 1.101   Feb 11 2000 09:55:28   svelaga
   Fixed SCR# 386.
   
      Rev 1.100   Feb 10 2000 17:23:30   svelaga
   Implemented the design for SCR #374.
   
      Rev 1.99   Feb 03 2000 11:07:34   svelaga
   Fix to SCR #352. 
   
      Rev 1.98   Feb 02 2000 17:18:52   svelaga
   Fix to SCR #352.
   
   
      Rev 1.97   Jan 28 2000 13:08:58   dirby
   Changed processing code for Card Verifications from 51 to 38.
   This is specifically for Equitable, reference Equitable issue 234.
   
      Rev 1.96   Jan 28 2000 09:44:18   dirby
   Added code to initialize a structure prior to use.
   This is a structure that gets written to shared memory.
   
      Rev 1.95   Jan 27 2000 13:51:36   svelaga
   Fixed SCR #286.
   
   Increased the orig_data ( field 90 ) size to 
   avoid memory overrun.
   
      Rev 1.94   Jan 25 2000 14:04:24   dirby
   Modified field 61 to check length prior to storing it
   in the host message structure.
   
      Rev 1.93   Jan 25 2000 13:10:18   svelaga
   removed 'bf25_pocc' for 110 messages from
   transaction table.
   
      Rev 1.92   Jan 24 2000 16:54:46   svelaga
   Fix to SCR #288
   
      Rev 1.91   Jan 24 2000 12:12:48   ddabberu
   build_host_structure modified to address
   processing_code during voice_auth txns
   
      Rev 1.90   Jan 24 2000 10:54:24   dirby
   Changed field 60 to use position [2] of pos_entry_mode to determine
   if terminal has card swipe capability.
   
      Rev 1.89   Jan 14 2000 14:05:20   ddabberu
   Modified to FIX  SCR #124, SCR#129
   
      Rev 1.88   Jan 13 2000 11:42:40   ddabberu
   modified build_host_structure() to fix SCR62
   
      Rev 1.86   Jan 07 2000 16:40:56   ddabberu
   modified hhutil_db_ok to handle auth_advices
   
      Rev 1.85   Jan 04 2000 13:35:58   ddabberu
   updated to process authorization, auth_advice, and reversals.
   changes made in the following functions:
   process_auth_advices(), incoming_other_host_response(), hhutil_db_ok()
   
      Rev 1.84   Dec 30 1999 16:39:08   dirby
   Modified to update transaction control database tables
   upon successful completion of a switched in authorization.
   
      Rev 1.83   Dec 28 1999 12:54:12   dirby
   Moved code to use network Id and network type
   as keys for NCF30.
   
      Rev 1.82   Dec 22 1999 11:03:02   dirby
   Modified to separate advice processing from reversals.
   
      Rev 1.81   Dec 20 1999 15:16:12   ddabberu
   Integration Testested  and the following functions were modified. 
   insertTLf01(), reconUpdate(),
   
   
      Rev 1.80   Nov 18 1999 08:56:22   dirby
   Fixed some compile bugs.
   
      Rev 1.79   Nov 16 1999 15:29:06   dirby
   Updated to:
   1.  Remove negative file processing,
   2.  Incorporate a new scheme for handling reversals,
   3.  Use the UpdateDS Data Server.
   
      Rev 1.78   Aug 27 1999 09:08:48   egable
   Insert TLF01 if the original transaction is not
   found for a reversal.
   
      Rev 1.77   Aug 25 1999 17:06:46   egable
   Implement the new reversal design.
   
      Rev 1.76   Aug 16 1999 16:38:50   egable
   Save the code for field 126.
   
      Rev 1.75   Aug 13 1999 15:53:44   egable
   Change the processing code logic. 
   
      Rev 1.74   Aug 13 1999 14:57:44   egable
   Change the logic on when NCF30 is inserted
   in the switched in path and add parameters
   to recon_update.
   
      Rev 1.73   Aug 12 1999 16:56:18   egable
   Remove an extra call to recon_update
   
      Rev 1.72   Aug 12 1999 16:51:58   egable
   Insert NCF30 before sending the response
   to the host.  Add code to process the return
   from the dataserver for inserting NCF30.
   
      Rev 1.71   Aug 10 1999 10:29:20   egable
   Correct field 44 to output the CVV result based
   on the response code and the security response
   code.  Change field 22 to use a temporary variable
   so that the original data is not changed.
   
      Rev 1.70   Aug 06 1999 16:40:16   egable
   Use the data from the ATM to fill field 61 for
   partial reversals.
   
      Rev 1.69   Aug 05 1999 13:26:26   svelaga
   increased the size of the temporary string.
   
      Rev 1.68   Aug 04 1999 14:49:02   egable
   Correct the echoing of field 62.
   
      Rev 1.67   Aug 03 1999 14:43:46   egable
   1) Change field 60 to be 22 instead of 21 for
   ATM transactions. 2) Echo field 62. 3) For
   partial reversals, don't output field 60, output
   field 61.
   
      Rev 1.66   Jul 29 1999 21:44:28   egable
   Correct the processing of fields 60,61 and 120.
   (The length is # of bytes, not # of characters).
   
      Rev 1.65   Jul 28 1999 22:16:36   egable
   Don't check for PIN block for a reversal.
   
      Rev 1.64   Jul 28 1999 20:39:44   egable
   Add the setting of acquirer_id before calling
   pin translation.
   
      Rev 1.63   Jul 27 1999 20:59:48   egable
   Add the voice_category_code to determine
   field 25.
   
      Rev 1.62   Jul 27 1999 18:12:38   egable
   Correct the tx_key for balance inquiries.
   
      Rev 1.61   Jul 27 1999 15:38:24   egable
   Only output field 61 for ATM transactions.
   
      Rev 1.60   Jul 27 1999 15:35:20   egable
   Add fields to a reversal for ATM.
   
      Rev 1.59   Jul 26 1999 20:05:30   egable
   Correct field 25, Visa wants a 00 for ATMs.
   
      Rev 1.58   Jul 26 1999 16:24:48   egable
   Correct errors in function returns.
   
      Rev 1.57   Jul 26 1999 10:11:18   egable
   Correct the source of the data for field 43.
   
      Rev 1.56   Jul 21 1999 15:46:56   egable
   Change field 62 to be bitmap
   
      Rev 1.55   Jul 19 1999 14:00:10   egable
   Correct field 54.
   
      Rev 1.54   Jul 16 1999 15:40:08   egable
   Add fields 19 and 25 for balance inquiry.
   
      Rev 1.53   Jul 15 1999 13:32:24   egable
   Correct fields used in balance inquiry.
   
      Rev 1.52   Jul 14 1999 10:30:14   egable
   Change field 53 to have 01 in positions 7-8.
   
      Rev 1.51   Jul 13 1999 16:01:52   egable
   Validate incoming data; add a free; change
   strncpy to memcpy in move_iso; output 
   messages to syslog as well as monitor; 
   clear out temporary variables before using
   them.
   
      Rev 1.50   Jul 12 1999 12:04:20   egable
   Correct error in issuing path for balance
   inquiry.  Correct CVV checking.
   
      Rev 1.49   01 Jul 1999 14:06:14   dirby
   1.  Removed fields 6, 10, and 51 from authorization responses.  This is a
   result of Visa's response to the certification scripts EB sent them.
   2.  Visa reported field 90 did not echo correctly for 0400/0401 requests.
   Did not find anything wrong, so I just initialized 'tempstr' and changed
   'strncpy' to 'memcpy'.
   
      Rev 1.48   Jun 25 1999 15:11:52   egable
   Correct for odd length PAN and track2.
   
      Rev 1.47   Jun 23 1999 15:27:24   egable
   Undid the correction to field 43, it was already
   correct.
   
      Rev 1.46   Jun 23 1999 14:39:36   egable
   Correct field 43.
   
      Rev 1.45   Jun 21 1999 16:03:10   egable
   Correct the format of the outgoing pin block
   
      Rev 1.44   Jun 18 1999 16:44:16   egable
   Use voice_auth_text instead of response_text.
   
      Rev 1.43   Jun 16 1999 16:20:56   egable
   Trim the spaces from the end of field 41.
   
      Rev 1.42   Jun 10 1999 13:10:46   egable
   If the data in Field 43 is smaller than the 
   field, fill will spaces.
   
      Rev 1.41   Jun 09 1999 08:55:42   egable
   Added code to catch invalid message formats in order to
   error out gracefully.  The invalid message format is based
   on the length of data received from Visa being shorter than
   expected.  Search for 'invalid_msg_format' to find all changes.
   
   
      Rev 1.40   Jun 04 1999 13:23:28   egable
   Change the response text message list to
   match Visa spec.
   
      Rev 1.39   Jun 03 1999 17:38:38   egable
   Comment out the processing of field 48, 
   we don't use the data.
   
      Rev 1.38   Jun 03 1999 15:30:14   egable
   Remove field 61 from reversal response,
   correct the terminal id, if it is less that 8
   characters (this affects the lookup key).
   
      Rev 1.37   Jun 02 1999 15:55:54   egable
   Add  field 52 and 53 to cash advance, quasi
   cash, etc requests.
   
      Rev 1.36   Jun 02 1999 14:20:12   egable
   Correct for odd length field 2, correct for odd
   length 35, don't output field 95 unless there 
   is a partial reversal.
   
      Rev 1.35   Jun 01 1999 11:53:26   egable
   Correct the message type in field 90, Add
   field 90 to reversal response, correct POS
   entry mode.
   
      Rev 1.34   May 27 1999 13:30:34   egable
   Correct the addition_pos_information and
   add function to fill the response_text.
   
      Rev 1.33   May 25 1999 16:11:56   egable
   Put the previous source station flag into
   shared memory to allow alternating source
   station ids.
   
   
      Rev 1.32   May 24 1999 15:00:22   egable
   Correct errors with processing codes.
   
      Rev 1.31   May 24 1999 14:42:14   egable
   Fix field 41 and field 60.
   
      Rev 1.30   May 23 1999 14:51:32   egable
   Add the cash_advance_authorization.
   
      Rev 1.29   May 22 1999 10:51:02   egable
   Correct the source/dest source station id,
   correct field 48 (caused bad currency code),
   correct odd length field 32 problem, added
   a carrot to the EBCDIC chart.
   
      Rev 1.28   May 21 1999 11:30:38   egable
   Correct field 44 to only output 44.5 if CVV
   was checked.
   
      Rev 1.27   May 20 1999 10:59:10   egable
   Correct currency code, echo fields 3 and 25 when request
   from host.
   
      Rev 1.26   May 18 1999 10:15:54   egable
   Correct lookup key, protect against a short
   terminal ID, correct processing code.
   
      Rev 1.25   May 14 1999 15:56:06   egable
   Correct errors with field 95 and field 43.
   
      Rev 1.24   May 12 1999 16:56:52   egable
   Correct the source and destination station
   ids in the Visa header.  Correct the pos condition
   code.
   
      Rev 1.23   May 07 1999 14:45:32   egable
   Correct fields 43, 60, 61, 90 and message_type
   in TLF01.
   
      Rev 1.22   May 04 1999 17:00:24   egable
   Remove the currency conversion code, 
   bring in field 61, and only output field 44
   if ATM trans.
   
      Rev 1.21   May 04 1999 08:46:16   egable
   Update processing code and message type.
   
      Rev 1.20   Apr 29 1999 14:34:28   egable
   Include milliseconds in the transaction id.
   
      Rev 1.19   Apr 29 1999 13:20:58   egable
   Correct field 60 and 62.
   
      Rev 1.18   Apr 13 1999 10:30:36   egable
   Correct cash advance transaction.
   
      Rev 1.17   Apr 12 1999 14:47:04   egable
   Change the security que name to racal.
   
      Rev 1.16   Apr 09 1999 11:17:52   egable
   Fixed transaction id.
   
      Rev 1.15   Apr 08 1999 16:39:18   egable
   Error report #510.  Removed the "Select TLF01
   error" message.
   
      Rev 1.14   Apr 08 1999 09:32:22   egable
   ER # 505 - Fix the quasi cash transaction.
   ER #501 - Fix a blank message to monitor.
   ER #504 - Correct cash advance processing code.
   
      Rev 1.13   Mar 30 1999 15:55:24   egable
    
   
      Rev 1.12   Mar 29 1999 16:50:58   egable
   Correct interface with security.  Change _itoa
   to itoa.
   
      Rev 1.11   Mar 26 1999 15:27:28   egable
   Correct logon and quasi cash and nmi_code.
   
      Rev 1.10   Mar 25 1999 13:56:08   egable
   Update host_start_time, host_finish_time,
   transaction_start_time and transaction_finish_time
   and correct currency code.
   
      Rev 1.9   Mar 23 1999 08:27:20   egable
   Correct the card_number.
   
      Rev 1.8   Mar 17 1999 17:26:36   egable
   Correct currency conversion.
   
      Rev 1.7   Mar 16 1999 16:24:24   egable
   Changes to Fields 44 and 62 and 45.
   
      Rev 1.6   Mar 15 1999 13:37:56   egable
   Make corrections from unit test of acquiring 
   path.
   
      Rev 1.5   Mar 12 1999 08:33:08   egable
   Make changes from unit testing.
   
      Rev 1.4   Mar 09 1999 14:52:38   egable
   Correct errors found in unit testing and protect
   against a null pointer.
   
      Rev 1.3   Feb 23 1999 09:28:48   egable
   Correct errors and change data server que
   name.
   
      Rev 1.2   Feb 02 1999 15:59:38   egable
   Changes to message flow, headers and
   'if' statements.  This is the basis for JCB.
   
   
      Rev 1.1   Jan 11 1999 09:56:54   egable
   Initial changes for equitable, added  issuing
   paths and several message types.
   
      Rev 1.0   17 Dec 1998 16:25:18   jhapitas
   Initial Equitable Revison
   
      Rev 1.0   Dec 04 1998 14:56:34   MSALEH
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
#include "vsmsutil.h"
#include "vsmstran.h"

#include "app_info.h"
#include "equitdb.h"
#include "nc_dbstruct.h"
#include "tx_dbstruct.h"
#include "vsmsmain.h"
#include "memmnger.h"
#include "txutils.h"
#include "genutil.h"
#include "txtrace.h"

BYTE      a2e[257];
BYTE      e2a[257];
BOOLEAN   maps_initialised = false;

void find_cvc_flag( pCHAR, pCHAR, pCHAR);

void TestResponses(BYTE []);     /* debug_code */

//added by farad 8/13/98
void Field62Response( pAUTH_TX p_auth_tx );

void Field63Response( pAUTH_TX p_auth_tx );

/* functions in other parts of host handler */
extern void   perform_error_processing(pAUTH_TX);
extern BOOLEAN send_it_back_to_terminal(BYTE[3], pAUTH_TX);
extern CHAR FWD_INS_CTRY_CODE     [MAX_INI_INFO_SIZE];

/**********************************
     VARIABLES
**********************************/
/* local variables */
INT    reqidx;                  /* next open position in request buffer */
INT    buflen;                  /* total size of buffer */
INT    bitmapidx;               /* offset of bitmap... */
INT    invalid_msg_format;
INT    de23_present;


CHAR   applnk_que_name [] = "applnkA" ;
CHAR   security_que_name [] = "racalA" ;

/* global variables */
extern NCF01       ncf01_i;
extern NCF01       ncf01_a;
extern BYTE     current_state[2];
extern CHAR     AppName[];

extern CHAR  ncvsms_Error_warning_Filename[256];
extern CHAR  ncvsms_module_error_warning_file_name[256];
extern CHAR  ncvsms_error_warning_file_path[256];
extern CHAR  Log_Bufr[512]; /* used to copy term id, card num and merch id to error msg
	   	   	   	   	   	   	   	   and passed it to print in File and monitor */
CHAR masked_card_num[20] = {0};

UINT Max_File_Size_Defined = 0 ;
/* Database structure sizes */
extern INT    Tlf01_Size;
extern INT    Ncf01_Size;
extern INT    Ncf30_Size;
extern INT    Auth_Tx_Size;

extern CHAR  updateds_que_name[];
extern CHAR  trands_que_name [];
extern CHAR  devds_que_name [];
extern CHAR  netds_que_name [];
extern CHAR  authorizer_que_name [];
extern CHAR  cur_app_name[];
extern CHAR  oprmon_que_name[];
extern CHAR  dialog_que_name [];
extern CHAR  vLogon_Resp_Indicator[];
extern CHAR  vLogon_NMI_Code[];
extern BYTE  orig_host_start_time[];
extern CHAR  SHARED_MEMORY_TBL[];
extern CHAR  ServiceCode[];
extern BYTE  temp_stan[7];
extern CHAR  eqit_que_name [];        
CHAR Dest_Stat_Id[7];

/* Statistics for Monitor */
extern TXN_COUNTS     Txn_Counts;
extern MONITOR_STATS  Monitor_Stats;
extern INT    Ncf01_Size;
/* Txn Tracing Info */
extern CHAR   TraceFile[];
extern INT    Tracing;
extern FILE   TraceFile_fp;
extern FILE   *pTraceFile_fp;
extern FILE   **ppTraceFile_fp;

/* Miscellaneous */
extern INT    MCC_Validation_Functionality;
extern INT    Txn_Is_A_Reject;
extern CHAR   PinZoneKeyIndex[];

/*Station ID variable to read from nc_config_inc file*/
extern BYTE Station_ID1[STATION_ID_MAX_LENGTH]= "";
extern BYTE Station_ID2[STATION_ID_MAX_LENGTH]="";

/*******************************************************/
/* REQUEST BUILD BUFFER                                */
/*******************************************************/
BYTE ReqRsp_Buffer [COMM_BUF_SZE];

#define  VISA_HDR_SIZE   22
#define  REJECT_VISA_HDR_SIZE 26



char  A2E[4] = "A2E";    /*  ASCII to EBCDIC conversion */
char  E2A[4] = "E2A";    /*  EBCDIC to ASCII conversion */


/********************************************************/
/*  STRUCTURE FOR PRIVATE FIELD 63, TABLE 73			*/
/********************************************************/
typedef struct 
{
	char data_length       [3];
	char table_id          [3];
	char registration_flag [2];
	char account_selection [2];
	char card_data         [17];
	char challenge1        [5];
	char challenge2        [5];
	char cresponse         [3];
}Table73_Type;
  
/********************************************************/
/*  STRUCTURE FOR PRIVATE FIELD 63, TABLE 74			*/
/********************************************************/
typedef struct 
{
	char data_length       [3];
	char table_id          [3];
	char challenge1        [5];
	char challenge2        [5];
}Table74_Type;
  


/******************************************************/
/* DATA STRUCTURE FOR STORING DATA IN FINAL FORMAT... */
/******************************************************/
struct iso_msg_rec HOST_MESSAGE;



static  char proccode_str [7];

static  int fs_challenge_number = 1;

struct totals_record
   {
   BYTE captured_sales_count   [3];
   BYTE captured_sales_amount  [12];
   BYTE captured_refund_count  [3];
   BYTE captured_refund_amount [12];
   BYTE debit_sales_count      [3];
   BYTE debit_sales_amount     [12];
   BYTE debit_refund_count     [3];
   BYTE debit_refund_amount    [12];
   };


/*******************************************************/
/*  TRANSACTION ENUMERATION                            */
/*******************************************************/
enum trantypes {
   tt_sale,             /* sale */
   tt_check_dl,         /* check - driv lice verify */
   tt_debit,            /* debit */
   tt_offline,          /* offline txn */
   tt_refund,           /* refund */
   tt_settle,           /* settlement */
   tt_void,             /* void */
   tt_test,             /* test */
   tt_sale_response     /* Sale Response , Chipstipe Demo */
   };

enum tranopts1 {
   topt1_notip       = 1,      /* no tip amount */
   topt1_noacct      = 2,      /* no account required in proc code */
   topt1_nocapture   = 4,      /* no capture required */
   topt1_nosystime   = 8,      /* no system time required */
   topt1_credit      = 16,     /* credit type transaction */
   topt1_preadv      = 32,     /* send all advises prior */
   topt1_settle      = 64,     /* settle type transaction */
   topt1_postadv     = 128     /* send single advise after */
   };

enum tranopts2 {
   topt2_noprerev     = 1,      /* no reversal prior */
   topt2_crfl         = 2,      /* CRFL processing allowed */
   topt2_floor        = 4,      /* floor limit processing allowed */
   topt2_referral     = 8,      /* referral processing allowed */
   topt2_batpend      = 16,     /* set batch pending flag for this transaction */
   topt2_debit        = 32,     /* debit transaction */
   topt2_reversal     = 64,     /* send reversal if failure */
   topt2_new_proccode = 128     /* use msg's original proccode */
   };


enum bitfields {
   bf_done           = 0,      /* end of list    */
   bf_1xbitfield     = 1,      /* extended bit field */
   bf_2pan           = 2,      /* pan            */
   bf_3proc          = 3,      /* processing code */
   bf_4amount        = 4,      /* amount   */
   bf_5settleamt     = 5,      /* Settlement Amount */
   bf_6crdbillamt    = 6,      /* amount cardholder billing */
   bf_7trntime       = 7,      /* transmission date-time */
   bf_9setconvrate   = 9,      /* Settlement Conversion rate */
   bf_10convrate     = 10,     /* Conversion rate, cardholder billing */
   bf_11stan         = 11,     /* system trace number */
   bf_12time         = 12,     /* time    */
   bf_13date         = 13,     /* date, trans  */
   bf_14expiry       = 14,     /* expiry date  */
   bf_15datestl      = 15,     /* settlement date */
   bf_16convdate     = 16,     /* Conversion date */
   bf_18sicnumber    = 18,     /* Merchant's type of business */
   bf_19ctrycd       = 19,     /* Acquiring institution's country code */
   bf_20panctrycd    = 20,     /* pan extended, country code */
   bf_21instctrycd   = 21,     /* Fwdng Inst Cntry Code */
   bf_22pose         = 22,     /* pos entry mode */
   bf_23cardseqnum	 = 23,		/* PAN seq nbr */
   bf_24nii          = 24,     /* nii      */
   bf_25pocc         = 25,     /* pos condition code */
   bf_26pincc        = 26,     /* pin capture code */
   bf_28fee          = 28,     /* Amount Transacion fee  */
   bf_32idcode       = 32,     /* Acquiring institution's ID code */
   bf_33fwdidcode    = 33,     /* Forwarding institution's ID code */
   bf_34panext       = 34,     /* pan extended */
   bf_35track2       = 35,     /* track 2 data */
   bf_37rrn          = 37,     /* retrieval reference number */
   bf_38authcode     = 38,     /* auth code   */
   bf_39respcode     = 39,     /* response code*/
   bf_40src          = 40,     /* service restriction code */
   bf_41termid       = 41,     /* terminal id  */
   bf_42cardacc      = 42,     /* card acceptor id */
   bf_43cardaccname  = 43,     /* card acceptor name/location */
   bf_44respdata     = 44,     /* additional response data */
   bf_45track1       = 45,     /* track 1 data   */
   bf_48addl_data    = 48,     /* Additional Data - Private */
   bf_49trncurcd     = 49,     /* transaction currency code */
   bf_50stlmtcurrcd  = 50,     /* settlement currency code             */
   bf_51crdcurcd     = 51,     /* card currency code */
   bf_52pinblock     = 52,     /* pin block    */
   bf_53seccntl      = 53,     /* security related control info */
   bf_54addlamts     = 54,     /* additional amounts */
   bf_55ICCdata		 = 55,		/*EMV Data*/
   bf_59posdata      = 59,     /* national point-of-service geographic data */
   bf_60priv60       = 60,     /* private field 60 */
   bf_61priv61       = 61,     /* private field 61 */
   bf_62priv62       = 62,     /* private field 62 */
   bf_63priv63       = 63,     /* private field 63 */
   bf_66setl_code    = 66,     /* Settlement Code                      */
   bf_68rcvcntrycd   = 68,     /* Rcvg Inst Cntry Code */
   bf_69setlcntrycd  = 69,     /* Settlement Inst Cntry Code */
   bf_70nmicode      = 70,     /* network management information code */
   bf_73action       = 73,     /* Date, action */
   bf_74credit_cnt   = 74,     /* number of credits                    */
   bf_75crd_rev_cnt  = 75,     /* number of reversal credits           */
   bf_76debit_cnt    = 76,     /* number of debits                     */
   bf_77dbt_rev_cnt  = 77,     /* number of reversal debits            */
   bf_86credit_amt   = 86,     /* credit amount                        */
   bf_87crd_rev_amt  = 87,     /* credit reversal amount               */
   bf_88debit_amt    = 88,     /* debit amount                         */
   bf_89dbt_rev_amt  = 89,     /* debit reversal amount                */
   bf_90orgdata      = 90,     /* original data elements   */
   bf_91fileupdate   = 91,     /* file update code         */
   bf_93rspind       = 93,     /* response indicator       */
   bf_95replamts     = 95,     /* replacement amounts      */
   bf_96msgsec       = 96,     /* message security code    */
   bf_97net_setl     = 97,     /* net settlement amount                */
   bf_99setl_id      = 99,     /* settlement institution Id code       */
   bf_100rcvid       = 100,    /* receiving inst id code   */
   bf_101fname       = 101,    /* file name                */
   bf_102acct1       = 102,    /* acct id 1                */
   bf_103acct2       = 103,    /* acct id 2                */
   bf_104trandesc    = 104,    /* transaction description  */
   bf_105TrpleDES    = 105,    /* Dbl-Lngth DES Key (Trple DES) */ 
   bf_115addTrace    = 115,    /* Addtnl Trace Data 1  */
   bf_118intractry   = 118,    /* intra country data       */
   bf_119settlesvc   = 119,    /* Settlmt Svc Data (International only)       */
   bf_120orgmsgid    = 120,    /* original message type id */
   bf_121issid       = 121,    /* issuing inst id code     */
   bf_122openuse     = 122,    /* remaining open-to-use    */
   bf_123addverf     = 123,    /* address verification data*/
   bf_124freeform    = 124,    /* free form text - Japan   */
   bf_125supinfo     = 125,    /* cris alert               */
   bf_126priv126     = 126,    /* private fields VSEC,CVV2 */
   bf_127filerecs    = 127     /* free form text - Japan   */
   }; 

/* LENGTHs FOR EACH BIT FIELD */

#define bf2_len       2
#define bf2_max_len  19
#define bf3_len       6
#define bf4_len      12
#define bf5_len      12
#define bf6_len      12
#define bf7_len      10
#define bf9_len       8
#define bf10_len      8
#define bf11_len      6
#define bf14_len      4
#define bf15_len      4
#define bf16_len      4
#define bf18_len      4
#define bf20_len      3
#define bf22_len      3
#define bf28_len      9
#define bf32_len      2
#define bf32data_len 11
#define bf33_len      2
#define bf35_len      2
#define bf37_len     12
#define bf38_len      6
#define bf39_len      2
#define bf41_len      8
#define bf42_len     15
#define bf44_len      2
#define bf45_len      2
#define bf48_len      3
#define bf49_len      3
#define bf50_len      3
#define bf51_len      3
#define bf54_len      3
#define bf60_len      3
#define bf61_len      3
#define bf62_len      3
#define bf63_len      3
#define bf70_len      3
#define bf90_len     42
#define bf94_len      7
#define bf102_len     2
#define bf103_len     2
#define bf120_len     3
#define bf121_len     3
#define bf126_len     1
#define bf127_len     3


       
#define EXTENDED_BITFIELD 128
#define NORMAL_BITFIELD    64
INT     BitFieldSize = NORMAL_BITFIELD;


struct trandef
{
   BYTE   td_trantype;           /* tran type for this record */
   BYTE   td_msgid[2];           /* iso 8583 message id       */
   BYTE   td_proccode[3];        /* iso 8583 processing code  */
   enum	  tranopts1  td_opts1;   /* options */
   enum   tranopts2  td_opts2;
   enum   bitfields  td_bitfields[EXTENDED_BITFIELD]; /* fields to put in message */
};

static  struct trandef TRANTABLE[] = {
      { 
      /********************************************
         S A L E
      ********************************************/
      AUTH_SALE,
      0x01,0x00,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_23cardseqnum,
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
	  bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_52pinblock,/* COND */
      bf_53seccntl, /* COND */
      bf_55ICCdata,
      bf_60priv60,   /* MAND */
      bf_62priv62,   /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************
         S A L E  R E S P O N S E
      ********************************************/
      AUTH_SALE_RESPONSE,
      0x01,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,    /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND f*/
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
      bf_54addlamts, /* COND */
      bf_62priv62,   /* COND */
      bf_102acct1,   /* COND */
      bf_103acct2,   /* COND */
      bf_121issid,   /* COND */
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
         R E F U N D 
      ********************************************/
      AUTH_REFUND,
      0x01,0x00,              /* message id */
      0x20,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_60priv60,  /* MAND */
      bf_done      /* end of fields to move */
       },

      {
	  /********************************************
         P A Y M E N T 
      ********************************************/
      AUTH_PAYMENT,
      0x01,0x00,              /* message id */
      0x40,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_60priv60,	/* MAND */
      bf_done      /* end of fields to move */
	  
	  },

      {   
      /********************************************
         R E V E R S A L   R E P E A T
      ********************************************/
      AUTH_REVERSAL_REPEAT,
      0x04,0x21,              /* message id */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* COND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_22pose,      /* COND */
      bf_25pocc,      /* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* COND */
      bf_37rrn,       /* MAND */
      bf_38authcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccname,/*COND */
      bf_49trncurcd,  /* MAND */
      bf_60priv60,    /* MAND */
      bf_62priv62,    /* COND */
      bf_90orgdata,   /* MAND */
      bf_95replamts,  /* COND */
      bf_done         /* end of fields to move */
      },
      {
      /********************************************
         O F F L I N E   S A L E  
      ********************************************/
      AUTH_OFFLINE_SALE,
      0x02,0x20,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_62priv62,  /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         O F F L I N E   R E F U N D 
      ********************************************/
      AUTH_OFFLINE_REFUND,
      0x02,0x20,              /* message id */
      0x20,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_62priv62,  /* COND */
      bf_done      /* end of fields to move */
      },
      
      {
      /********************************************
         S A L E   A D J U S T M E N T   
      ********************************************/
      AUTH_SALE_ADJUSTMENT,
      0x02,0x20,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_62priv62,  /* COND */
      bf_done      /* end of fields to move */
      },
	   {
      /********************************************
         R E F U N D    A D J U S T M E N T   
      ********************************************/
      AUTH_REFUND_ADJUSTMENT,
      0x02,0x20,              /* message id */
      0x22,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_62priv62,  /* COND */
      bf_done      /* end of fields to move */
      },
    
      {   
      /********************************************
         P A Y M E N T    A D J U S T M E N T   
      ********************************************/
      AUTH_PAYMENT_ADJUSTMENT,
      0x02,0x20,              /* message id */
      0x42,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_62priv62,  /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         A U T H O R I Z A T I O N 
      ********************************************/
      AUTH_AUTHORIZATION,
      0x01,0x00,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_23cardseqnum,
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_52pinblock,/* COND */
      bf_53seccntl, /* COND */
      bf_55ICCdata,
 	   bf_60priv60,	/* MAND */
	   bf_62priv62,  /* COND */
      bf_done      /* end of fields to move */
      },

      {
      /**********************************************
         A U T H O R I Z A T I O N   R E S P O N S E
      ***********************************************/
      AUTH_AUTHORIZATION_RESPONSE,
      0x01,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	   /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
      bf_62priv62,   /* COND */
      bf_102acct1,   /* COND */
      bf_103acct2,   /* COND */
      bf_121issid,   /* COND */
      bf_done      /* end of fields to move */
      },

	  {  
      /********************************************
         P R E   A U T H O R I Z A T I O N 
      ********************************************/
      AUTH_PRE_AUTHORIZATION,
      0x01,0x00,              /* message id */
      0x30,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_23cardseqnum,
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd, /* MAND */
      bf_55ICCdata,
      bf_60priv60,   /* MAND */
      bf_done      /* end of fields to move */
      },

      { 
      /*******************************************************
         C A S H   A D V A N C E  A U T H O R I Z A T I O N
      *******************************************************/
      AUTH_CASH_ADVANCE_AUTHORIZATION,
      0x01,0x00,              /* message id */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_23cardseqnum,
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_52pinblock,/* COND */
      bf_53seccntl, /* COND */
      bf_55ICCdata,
      bf_60priv60,	/* MAND */
      bf_done      /* end of fields to move */
      },


      {  
      /********************************************
         V O I D   S A L E
      ********************************************/
      AUTH_VOID_SALE,
      0x01,0x00,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_60priv60,	/* MAND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         V O I D   R E F U N D
      ********************************************/
      AUTH_VOID_REFUND,
      0x01,0x00,              /* message id */
      0x22,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_reversal,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,    /* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,   /* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,   /* MAND */
      bf_22pose,     /* COND */
      bf_25pocc,     /* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,   /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,      /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd, /* MAND */
      bf_60priv60,   /* MAND */
      bf_done      /* end of fields to move */
      },

      {
      /********************************************
         L O G O N 
      ********************************************/
      AUTH_LOGON,
      0x08,0x00,              /* message id */
      0x92,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_37rrn,       /* MAND */
      bf_48addl_data, /* COND */
      bf_63priv63,    /* MAND */
      bf_70nmicode,   /* MAND */
      bf_done       /* end of fields to move */
      },

      {
      /********************************************
         L O G O N   R E S P O N S E
      ********************************************/
      AUTH_LOGON_RESPONSE,
      0x08,0x10,              /* message id */
      0x92,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_7trntime,   /* MAND */
	   bf_11stan,     /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_63priv63,   /* MAND */
      bf_70nmicode,  /* MAND */
      bf_93rspind,   /* COND */
      bf_done        /* end of fields to move */
      },

      {
      /********************************************
         T E S T   T R A N
      ********************************************/
      AUTH_TEST,
      0x08,0x00,              /* message id */
      0x99,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_noacct+topt1_nocapture,
      topt2_noprerev,
      /**** BIT FIELDS ****/
      bf_3proc,
      bf_24nii,
      bf_41termid,
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
         C A R D   V E R I F I C A T I O N
      ********************************************/
      AUTH_CARD_VERIFICATION,
      0x01,0x00,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_new_proccode,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_60priv60,	/* MAND */
      bf_done      /* end of fields to move */
      },
     
      {
      /********************************************
         Q U A S I   C A S H
      ********************************************/
      AUTH_QUASI_CASH,
      0x01,0x00,              /* message id */
      0x11,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* COND */
      bf_25pocc,	/* MAND */
	  bf_28fee,		/* MAND */         /*TF - Ajay. Added */
      bf_32idcode,  /* MAND */
      bf_35track2,	/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* COND */
      bf_45track1,	/* COND */
      bf_49trncurcd,/* MAND */
      bf_52pinblock,/* COND */
      bf_53seccntl, /* COND */
      bf_60priv60,	/* MAND */
      bf_done      /* end of fields to move */
      },

      {
      /**********************************************
         Q U A S I   C A S H   R E S P O N S E
      ***********************************************/
      AUTH_QUASI_CASH_RESPONSE,
      0x01,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,      /* MAND */
      bf_4amount,	   /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_44respdata, /* COND */
      bf_49trncurcd, /* MAND */
      bf_62priv62,   /* COND */
      bf_102acct1,   /* COND */
      bf_103acct2,   /* COND */
      bf_121issid,   /* COND */
      bf_done      /* end of fields to move */
      },

      { 
      /********************************************
          A T M   C O N F I R M
       ********************************************/
      AUTH_ATM_CONFIRM,
      0x01,0x02,
      0x00,0x00,0x00,  /* This is not used.  It is not correct. */
      //**** OPTIONS ****
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* MAND */
      bf_3proc,	       /* MAND */
      bf_4amount,      /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd,/* COND */
	  bf_28fee,		/* MAND */
      bf_32idcode,   /* MAND */
      bf_34panext,   /* COND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* COND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,/* MAND */
      bf_49trncurcd, /* MAND */
      bf_61priv61,	   /* MAND */
      bf_done        /* end of fields to move */
      },

     /***************************************************
	   V I S A    S M S    M E S S A G E     T Y P E S 
	  ****************************************************/
      { 
      /************************************
         C A S H   D I S B U R S E M E N T
      **************************************/
      AUTH_CASH_ADVANCE,
      0x02,0x00,              /* message id */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_12time,     /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,	/* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* MAND */
      bf_23cardseqnum,
      bf_25pocc,	/* MAND */
      bf_28fee,      /* COND */
      bf_32idcode,   /* MAND */
      bf_33fwdidcode,/* COND */
      bf_35track2,	/* MAND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* MAND */
      bf_49trncurcd,/* MAND */
      bf_52pinblock,/* MAND */
      bf_53seccntl, /* MAND */
      bf_55ICCdata,
      bf_59posdata, /* COND */
      bf_60priv60,	/* OPT */
      bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */
      bf_68rcvcntrycd, /* COND */
	  bf_126priv126,   /* COND */
      bf_done      /* end of fields to move */
      },

      { 
      /*****************************************************
         C A S H   D I S B U R S E M E N T   R E S P O N S E
      ******************************************************/
      AUTH_CASH_ADVANCE_RESPONSE,
      0x02,0x10,              /* message id */
      0x01,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	   /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_15datestl,  /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_25pocc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_37rrn,      /* MAND */
      bf_38authcode, /* COND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* MAND */
      bf_42cardacc,  /* MAND */
      bf_44respdata, /* OPT */
      bf_48addl_data, /* COND */
      bf_49trncurcd, /* MAND */
      bf_54addlamts, /* OPT */
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */      
      bf_done      /* end of fields to move */
      },

      {  
      /********************************************
         B A L A N C E   I N Q U I R Y 
      ********************************************/
      AUTH_BALANCE_INQUIRY,
      0x02,0x00,              /* message id */
      0x30,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,	  /* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	  /* MAND */
      bf_12time,    /* MAND */
      bf_13date,    /* MAND */
      bf_14expiry,  /* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	  /* MAND */
      bf_23cardseqnum,
      bf_25pocc,	  /* MAND */
      bf_28fee,     /* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode,/* COND */
      bf_34panext,  /* COND */
      bf_35track2,  /* MAND */
      bf_37rrn,     /* MAND */
      bf_41termid,  /* MAND */
      bf_42cardacc, /* MAND */
      bf_43cardaccname,	/* MAND */
      bf_49trncurcd,/* COND */
      bf_52pinblock,/* MAND */
      bf_53seccntl, /* MAND */
      bf_55ICCdata,
      bf_59posdata, /* COND */
      bf_60priv60,  /* MAND */
      bf_63priv63,  /* MAND */
	  bf_126priv126, /* COND */
      bf_done      /* end of fields to move */
      },      

      {  
      /***************************************************
         B A L A N C E   I N Q U I R Y   R E S P O N S E
      ****************************************************/
      AUTH_BALANCE_INQUIRY_RESPONSE,
      0x02,0x10,              /* message id */
      0x30,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* COND */
      bf_3proc,      /* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,     /* MAND */
      bf_15datestl,  /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_25pocc,     /* COND */
      bf_32idcode,   /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* COND */
      bf_42cardacc,  /* MAND */
      bf_44respdata, /* OPT */
      bf_48addl_data, /* COND */
      bf_49trncurcd, /* OPT */
      bf_54addlamts, /* COND */
      /*bf_62priv62,*/   /* COND */
      bf_63priv63,   /* MAND */
      bf_102acct1,   /* OPT */
      bf_done      /* end of fields to move */
      },      

      {   
      /********************************************
         R E V E R S A L  
      ********************************************/
      AUTH_REVERSAL,
      0x04,0x20,              /* message id */
      0x00,0x00,0x00,         /* processing code  (use original) */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,       /* MAND */
      bf_4amount,     /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_12time,      /* MAND */
      bf_13date,      /* MAND */
      bf_14expiry,    /* COND */
      bf_18sicnumber, /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_22pose,      /* MAND */
      bf_25pocc,      /* MAND */
      bf_28fee,       /* COND */
      bf_32idcode,    /* MAND */
      bf_33fwdidcode, /* COND */
      bf_37rrn,       /* MAND */
      bf_38authcode,  /* COND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_43cardaccname,/*MAND */
      bf_48addl_data,  /* COND */
      bf_49trncurcd,  /* MAND */
      bf_59posdata,   /* COND */
      bf_60priv60,    /* MAND */
      bf_62priv62,    /* COND */
      bf_63priv63,  /* MAND */
      bf_90orgdata,   /* MAND */
	  bf_126priv126,  /* COND */
      bf_done         /* end of fields to move */
      },
      {
      /********************************************
         R E V E R S A L    R E S P O N S E
      ********************************************/
      AUTH_REVERSAL_RESPONSE,
      0x04,0x10,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,	    /* MAND */
      bf_15datestl,   /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_25pocc,	    /* MAND */
      bf_32idcode,    /* MAND */
      bf_37rrn,       /* MAND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_48addl_data, /* COND  */
      bf_62priv62,    /* COND  */
      bf_63priv63,    /* MAND */
      bf_90orgdata,   /* MAND */
      bf_done      /* end of fields to move */
      },

      {
      /*******************************************************
         R E V E R S A L    A D V I C E    R E S P O N S E
      *******************************************************/
      AUTH_REVERSAL_ADVICE_RESPONSE,
      0x04,0x30,              /* message id */
      0x00,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_credit+topt1_postadv,
      topt2_batpend,
      /**** BIT FIELDS ****/
      bf_2pan,        /* MAND */
      bf_3proc,	    /* MAND */
      bf_7trntime,    /* MAND */
      bf_11stan,	    /* MAND */
      bf_15datestl,   /* MAND */
      bf_19ctrycd,    /* MAND */
      bf_25pocc,	    /* MAND */
      bf_32idcode,    /* MAND */
      bf_37rrn,       /* MAND */
      bf_39respcode,  /* MAND */
      bf_41termid,    /* MAND */
      bf_42cardacc,   /* MAND */
      bf_62priv62,    /* COND */
      bf_63priv63,    /* MAND */
      bf_90orgdata,   /* MAND */
      bf_done      /* end of fields to move */
      },

      { 
      /************************************************
         B A C K   O F F I C E   A D J U S T M E N T
      ********************************************/
      AUTH_BACKOFFICE_ADJUSTMENT,
      0x02,0x20,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_12time,     /* MAND */
      bf_13date,     /* MAND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* MAND */
      bf_25pocc,	/* MAND */
      bf_28fee,      /* COND */
      bf_32idcode,   /* MAND */
      bf_33fwdidcode,/* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */ 
      bf_43cardaccname,	/* MAND */
      bf_44respdata,   /* COND */
      bf_48addl_data,  /* COND */
      bf_49trncurcd,/* MAND */
      bf_59posdata, /* COND */
      bf_60priv60,	/* COND */
      bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */	  
      bf_done      /* end of fields to move */
      },
      { 
      /****************************************************************
        B A C K   O F F I C E   A D J U S T M E N T   R E S P O N S E
      ****************************************************************/
      AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE,
      0x02,0x30,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_15datestl,  /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_25pocc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_42cardacc,  /* MAND */
      bf_48addl_data,/* COND */
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_102acct1,   /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /***********************************************************
         C A S H    D I S B U R S E M E N T   A D J U S T M E N T
      *************************************************************/
      AUTH_CASH_DISBURSEMENT_ADJUSTMENT, /* SEND a cash disbu.. adjustment (220) */
      0x02,0x20,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_12time,    /* MAND */
      bf_13date,     /* MAND */
      bf_14expiry,   /* COND */
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_22pose,	/* MAND */
      bf_25pocc,	/* MAND */
      bf_28fee,     /* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode, /* COND */
      bf_37rrn,     /* MAND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */ 
      bf_43cardaccname,	/* MAND */
      bf_44respdata,   /* COND */
      bf_48addl_data,  /* COND */
      bf_49trncurcd,/* MAND */
      bf_59posdata, /* COND */
      bf_60priv60,	/* COND */
      bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_done      /* end of fields to move */
      },
      { 
      /****************************************************************************
         C A S H    D I S B U R S E M E N T  A D J U S T M E N T   R E S P O N S E
      *****************************************************************************/
      AUTH_CASH_DISBURSEMENT_ADJUSTMENT_RESPONSE,
      0x02,0x30,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_15datestl,  /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_25pocc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* MAND */
      bf_42cardacc,	/* MAND */
      bf_48addl_data, /* COND */
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_102acct1,   /* COND */
      bf_done      /* end of fields to move */
      },

      { 
      /*****************************************
         R E P R E S E N T M E N T    
      *******************************************/
      AUTH_REPRESENTMENT,
      0x02,0x20,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,          /* MAND */
      bf_3proc,         /* MAND */
      bf_4amount,       /* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
      bf_13date,        /* MAND */
      bf_18sicnumber,   /* MAND */
      bf_19ctrycd,      /* MAND */
      bf_20panctrycd,   /* COND */	
      bf_25pocc,        /* MAND */
      bf_28fee,         /* COND */
      bf_32idcode,      /* MAND */
      bf_33fwdidcode,   /* COND */
      bf_37rrn,         /* MAND */
      bf_41termid,      /* MAND */
      bf_42cardacc,     /* MAND */
      bf_43cardaccname, /* MAND */
      bf_44respdata,    /* COND */
      bf_48addl_data,   /* COND */
      bf_49trncurcd,    /* MAND */
      bf_59posdata,     /* COND */
      bf_62priv62,      /* COND */
      bf_63priv63,      /* MAND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************************
        R E P R E S E N T M E N T     R E S P O N S E
      ********************************************************/
      AUTH_REPRESENTMENT_RESPONSE,
      0x02,0x30,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_15datestl,  /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_20panctrycd, /* COND */     
      bf_25pocc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */
      bf_48addl_data, /* COND */     
      bf_62priv62,   /* COND */
      bf_63priv63,   /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_102acct1,
      bf_done      /* end of fields to move */
      },
      { 
      /***************************************************
         F E E   C O L L E C T I O N   A C Q U I R E R
      ***************************************************/
      AUTH_FEE_COLLECTION_ACQUIRER,
      0x02,0x20,              /* message id */
      0x19,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,          /* COND */
      bf_3proc,         /* MAND */
      bf_4amount,       /* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_19ctrycd,      /* MAND */
      bf_32idcode,      /* MAND */
      bf_37rrn,         /* MAND */
      bf_48addl_data,   /* MAND */
      bf_49trncurcd,    /* MAND */
      bf_63priv63,      /* MAND */
      bf_100rcvid,      /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /*********************************************************
         F U N D S   D I S B U R S E M E N T   A C Q U I R E R
      **********************************************************/
      AUTH_FUNDS_DISBURSEMENT_ACQUIRER,
      0x02,0x20,              /* message id */
      0x29,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,          /* COND */
      bf_3proc,         /* MAND */
      bf_4amount,       /* MAND */
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
	  bf_12time,    /* MAND */
	  bf_13date,    /* MAND */
      bf_19ctrycd,      /* MAND */
      bf_32idcode,      /* MAND */
      bf_37rrn,         /* MAND */
      bf_48addl_data,   /* MAND */
      bf_49trncurcd,    /* MAND */
      bf_63priv63,      /* MAND */
      bf_100rcvid,      /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /***************************
         C H A R G E B A C K 
      ******************************/
      AUTH_CHARGEBACK,
      0x04,0x22,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
	  bf_5settleamt, 
      bf_7trntime,  /* MAND */
	  bf_9setconvrate,
      bf_11stan,	/* MAND */
      bf_13date,     /* MAND */
	  bf_15datestl,
	  bf_16convdate,
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_20panctrycd, /* COND */	
	  bf_21instctrycd,
      bf_25pocc,	/* MAND */
      bf_28fee,     /* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode, /* COND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */
      bf_62priv62,  /* COND */
      bf_63priv63,  /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_done      /* end of fields to move */
      },
      { 
      /******************************************
        C H A R G E B A C K    R E S P O N S E
      ********************************************/
      AUTH_CHARGEBACK_RESPONSE,
      0x04,0x32,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_15datestl,  /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_25pocc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */
      bf_63priv63,   /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_done      /* end of fields to move */
      },
      { 
      /*****************************************
         C H A R G E B A C K    R E V E R S A L
      *******************************************/
      AUTH_CHARGEBACK_REVERSAL,
      0x04,0x22,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,      /* MAND */
      bf_3proc,		/* MAND */
      bf_4amount,	/* MAND */
	  bf_5settleamt,
      bf_7trntime,  /* MAND */
	  bf_9setconvrate,
      bf_11stan,	/* MAND */
      bf_13date,     /* MAND */
	  bf_15datestl,
	  bf_16convdate,
      bf_18sicnumber,/* MAND */
      bf_19ctrycd,  /* MAND */
      bf_20panctrycd, /* COND */	
	  bf_21instctrycd,
      bf_25pocc,	/* MAND */
      bf_28fee,     /* COND */
      bf_32idcode,  /* MAND */
      bf_33fwdidcode, /* COND */
      bf_37rrn,     /* MAND */
      bf_38authcode, /* COND */
      bf_41termid,	/* MAND */
      bf_42cardacc,	/* MAND */
      bf_43cardaccname,	/* MAND */
      bf_63priv63,  /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************************
        C H A R G E B A C K  R E V E R S A L  R E S P O N S E
      ********************************************************/
      AUTH_CHARGEBACK_REVERSAL_RESPONSE,
      0x04,0x32,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,       /* MAND */
      bf_3proc,		/* MAND */
      bf_7trntime,   /* MAND */
      bf_11stan,	   /* MAND */
      bf_15datestl,  /* MAND */
      bf_19ctrycd,   /* MAND */
      bf_25pocc,     /* MAND */
      bf_32idcode,   /* MAND */
      bf_37rrn,      /* MAND */
      bf_39respcode, /* MAND */
      bf_41termid,   /* MAND */
      bf_42cardacc,	/* MAND */
      bf_63priv63,   /* MAND */
      bf_90orgdata,  /* OPT*/
      bf_done      /* end of fields to move */
      },
      { 
      /***************************************************************
        F E E   C O L L E C T I O N   I S S U E R    R E S P O N S E
      ***************************************************************/
      AUTH_FEE_COLLECTION_ISSUER_RESPONSE,
      0x04,0x32,              /* message id */
      0x19,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* COND */
      bf_3proc,        /* MAND */
      bf_7trntime,     /* MAND */
      bf_11stan,       /* MAND */
      bf_15datestl,    /* MAND */
      bf_19ctrycd,     /* MAND */
      bf_21instctrycd, /* COND */
      bf_32idcode,     /* MAND */
      bf_33fwdidcode,  /* MAND */
      bf_37rrn,        /* MAND */
      bf_39respcode,   /* MAND */
      bf_63priv63,     /* MAND */
      bf_done      /* end of fields to move */
      },
      { 
      /***********************************************************************
        F U N D S   D I S B U R S E M E N T   I S S U E R    R E S P O N S E
      ***********************************************************************/
      AUTH_FUNDS_DISBURSEMENT_ISSUER_RESPONSE,
      0x04,0x32,              /* message id */
      0x29,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_2pan,         /* COND */
      bf_3proc,        /* MAND */
      bf_7trntime,     /* MAND */
      bf_11stan,       /* MAND */
      bf_15datestl,    /* MAND */
      bf_19ctrycd,     /* MAND */
      bf_21instctrycd, /* COND */
      bf_32idcode,     /* MAND */
      bf_33fwdidcode,  /* MAND */
      bf_37rrn,        /* MAND */
      bf_39respcode,   /* MAND */
      bf_63priv63,     /* MAND */
      bf_done      /* end of fields to move */
      },
      { 
      /*****************************************
        A U T O    R E C O N C I L I A T I O N     
      *******************************************/
      AUTH_RECONCILIATION,
      0x05,0x20,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_15datestl, /* MAND */
      bf_50stlmtcurrcd,  /* COND */
      bf_63priv63,  /* MAND */
      bf_69setlcntrycd, /* COND */
      bf_70nmicode,   /* MAND */
      bf_74credit_cnt,/* MAND */
      bf_75crd_rev_cnt, /* MAND */
      bf_76debit_cnt,   /* MAND */
      bf_77dbt_rev_cnt, /* MAND */
      bf_86credit_amt,  /* MAND */
      bf_87crd_rev_amt, /* MAND */
      bf_88debit_amt,   /* MAND */
      bf_89dbt_rev_amt,  /* MAND */
      bf_90orgdata,  /* MAND */
      bf_97net_setl, /* MAND */
      bf_99setl_id,  /* MAND */
      bf_done      /* end of fields to move */
      },
      { 
      /***************************************************************
        A U T O    R E C O N C I L I A T I O N      R E S P O N S E
      ****************************************************************/
      AUTH_RECONCILIATION_RESPONSE,
      0x05,0x30,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,      /* MAND */
      bf_11stan,        /* MAND */
      bf_15datestl,     /* MAND */
      bf_50stlmtcurrcd, /* COND */
      bf_63priv63,      /* MAND */
      bf_66setl_code,   /* MAND */
      bf_69setlcntrycd, /* COND */
      bf_70nmicode,     /* MAND */
      bf_99setl_id,     /* MAND */
      bf_done      /* end of fields to move */
      },

      { 
      /*****************************************
         A D M I N I S T R A T I V E     
      *******************************************/
      AUTH_ADMINISTRATIVE_TEXT,
      0x06,0x00,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,      /* MAND */
      bf_11stan,        /* MAND */
      bf_33fwdidcode,   /* MAND */
      bf_37rrn,         /* MAND */
      bf_43cardaccname, /* MAND */
      bf_48addl_data,   /* COND */
      bf_63priv63,      /* MAND */
      bf_68rcvcntrycd,  /* COND */
      bf_70nmicode,     /* MAND */
      bf_100rcvid,      /* MAND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************************
        A D M I N I S T R A T I V E      R E S P O N S E
      ********************************************************/
      AUTH_ADMINISTRATIVE_TEXT_RESPONSE,
      0x06,0x10,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_15datestl, /* MAND */
      bf_37rrn,     /* MAND */
      bf_39respcode,  /* MAND */	  
      bf_63priv63,  /* MAND */
      bf_68rcvcntrycd, /* COND */
      bf_70nmicode,   /* MAND */
      bf_100rcvid,  /* MAND */
      bf_done      /* end of fields to move */
      },
      
      { 
      /********************************************
         F U N D S    T R A N S F E R  T O T A L S     
      *******************************************/
      AUTH_FUND_TRANSFER_TOTAL,
      0x06,0x20,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_15datestl, /* MAND */
      bf_37rrn,     /* MAND */
      bf_48addl_data,  /* MAND */
      bf_50stlmtcurrcd, /* MAND */
      bf_63priv63,  /* MAND */
      bf_69setlcntrycd, /* MAND */
      bf_70nmicode,   /* MAND */
      bf_99setl_id,  /* MAND */
      bf_done      /* end of fields to move */
      },
      { 
      /********************************************************
        F U N D S    T R A N S F E R  T O T A L S      R E S P O N S E
      ********************************************************/
      AUTH_FUND_TRANSFER_TOTAL_RESPONSE,
      0x06,0x30,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,  /* MAND */
      bf_11stan,	/* MAND */
      bf_15datestl, /* MAND */
      bf_37rrn,     /* MAND */
      bf_39respcode,  /* MAND */	  
      bf_50stlmtcurrcd,  /* MAND */
      bf_63priv63,  /* MAND */
      bf_69setlcntrycd, /* MAND */
      bf_70nmicode,   /* MAND */
      bf_99setl_id,  /* MAND */
      bf_done      /* end of fields to move */
      },
      {
      /********************************************
         D Y N A M I C   K E Y   E X C H A N G E    
      **********************************************/
      AUTH_KEY_CHANGE,
      0x08,0x00,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,    /* MAND */
      bf_11stan,	    /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_48addl_data, /* COND */
      bf_53seccntl,   /* MAND */
      bf_63priv63,    /* MAND */
      bf_70nmicode,   /* MAND */
      bf_90orgdata,   /* MAND */
      bf_96msgsec,    /* COND */
      bf_105TrpleDES, /* COND */
      bf_done      /* end of fields to move */
      },
      { 
      /************************************************************
        D Y N A M I C   K E Y   E X C H A N G E   R E S P O N S E
      *************************************************************/
      AUTH_KEY_CHANGE_RESPONSE,
      0x08,0x10,              /* message id */
      0x02,0x00,0x00,         /* processing code */
      /**** OPTIONS ****/
      topt1_postadv,
      topt2_referral+topt2_reversal+topt2_floor,
      /**** BIT FIELDS ****/
      bf_7trntime,    /* MAND */
      bf_11stan,      /* MAND */
      bf_33fwdidcode, /* MAND */
      bf_39respcode,  /* MAND */
      bf_63priv63,    /* MAND */
      bf_70nmicode,   /* MAND */
      bf_done      /* end of fields to move */
      }

   };

#define N_TRANENTRY (sizeof(TRANTABLE) / sizeof(struct trandef))

/********************************************************/
/* BIT SETTING TABLE                                    */
/********************************************************/
struct isobit 
   {
   INT byte_no;         /* byte number for this bit field */
   INT bit_mask;        /* bit to set in this byte        */
   };

const struct isobit BITTAB[128+1] =
   {
   { 0, 0 }, /*zero justify */
   { 0, 0x80 },   /* bit 1  */   { 0, 0x40 },   /* bit 2  */   { 0, 0x20 },   /* bit 3  */   { 0, 0x10 },   /* bit 4  */
   { 0, 0x08 },   /* bit 5  */   { 0, 0x04 },   /* bit 6  */   { 0, 0x02 },   /* bit 7  */   { 0, 0x01 },   /* bit 8  */
   { 1, 0x80 },   /* bit 9  */   { 1, 0x40 },   /* bit 10 */   { 1, 0x20 },   /* bit 11 */   { 1, 0x10 },   /* bit 12 */
   { 1, 0x08 },   /* bit 13 */   { 1, 0x04 },   /* bit 14 */   { 1, 0x02 },   /* bit 15 */   { 1, 0x01 },   /* bit 16 */
   { 2, 0x80 },   /* bit 17 */   { 2, 0x40 },   /* bit 18 */   { 2, 0x20 },   /* bit 19 */   { 2, 0x10 },   /* bit 20 */
   { 2, 0x08 },   /* bit 21 */   { 2, 0x04 },   /* bit 22 */   { 2, 0x02 },   /* bit 23 */   { 2, 0x01 },   /* bit 24 */
   { 3, 0x80 },   /* bit 25 */   { 3, 0x40 },   /* bit 26 */   { 3, 0x20 },   /* bit 27 */   { 3, 0x10 },   /* bit 28 */
   { 3, 0x08 },   /* bit 29 */   { 3, 0x04 },   /* bit 30 */   { 3, 0x02 },   /* bit 31 */   { 3, 0x01 },   /* bit 32 */
   { 4, 0x80 },   /* bit 33 */   { 4, 0x40 },   /* bit 34 */   { 4, 0x20 },   /* bit 35 */   { 4, 0x10 },   /* bit 36 */
   { 4, 0x08 },   /* bit 37 */   { 4, 0x04 },   /* bit 38 */   { 4, 0x02 },   /* bit 39 */   { 4, 0x01 },   /* bit 40 */
   { 5, 0x80 },   /* bit 41 */   { 5, 0x40 },   /* bit 42 */   { 5, 0x20 },   /* bit 43 */   { 5, 0x10 },   /* bit 44 */
   { 5, 0x08 },   /* bit 45 */   { 5, 0x04 },   /* bit 46 */   { 5, 0x02 },   /* bit 47 */   { 5, 0x01 },   /* bit 48 */
   { 6, 0x80 },   /* bit 49 */   { 6, 0x40 },   /* bit 50 */   { 6, 0x20 },   /* bit 51 */   { 6, 0x10 },   /* bit 52 */
   { 6, 0x08 },   /* bit 53 */   { 6, 0x04 },   /* bit 54 */   { 6, 0x02 },   /* bit 55 */   { 6, 0x01 },   /* bit 56 */
   { 7, 0x80 },   /* bit 57 */   { 7, 0x40 },   /* bit 58 */   { 7, 0x20 },   /* bit 59 */   { 7, 0x10 },   /* bit 60 */
   { 7, 0x08 },   /* bit 61 */   { 7, 0x04 },   /* bit 62 */   { 7, 0x02 },   /* bit 63 */   { 7, 0x01 },    /* bit 64 */

   { 8, 0x80 },    /* bit 1  */   { 8, 0x40 },    /* bit 2  */   { 8, 0x20 },    /* bit 3  */   { 8, 0x10 },    /* bit 4  */
   { 8, 0x08 },    /* bit 5  */   { 8, 0x04 },    /* bit 6  */   { 8, 0x02 },    /* bit 7  */   { 8, 0x01 },    /* bit 8  */
   { 9, 0x80 },    /* bit 9  */   { 9, 0x40 },    /* bit 10 */   { 9, 0x20 },    /* bit 11 */   { 9, 0x10 },    /* bit 12 */
   { 9, 0x08 },    /* bit 13 */   { 9, 0x04 },    /* bit 14 */   { 9, 0x02 },    /* bit 15 */   { 9, 0x01 },    /* bit 16 */
   { 10, 0x80 },   /* bit 17 */   { 10, 0x40 },   /* bit 18 */   { 10, 0x20 },   /* bit 19 */   { 10, 0x10 },   /* bit 20 */
   { 10, 0x08 },   /* bit 21 */   { 10, 0x04 },   /* bit 22 */   { 10, 0x02 },   /* bit 23 */   { 10, 0x01 },   /* bit 24 */
   { 11, 0x80 },   /* bit 25 */   { 11, 0x40 },   /* bit 26 */   { 11, 0x20 },   /* bit 27 */   { 11, 0x10 },   /* bit 28 */
   { 11, 0x08 },   /* bit 29 */   { 11, 0x04 },   /* bit 30 */   { 11, 0x02 },   /* bit 31 */   { 11, 0x01 },   /* bit 32 */
   { 12, 0x80 },   /* bit 33 */   { 12, 0x40 },   /* bit 34 */   { 12, 0x20 },   /* bit 35 */   { 12, 0x10 },   /* bit 36 */
   { 12, 0x08 },   /* bit 37 */   { 12, 0x04 },   /* bit 38 */   { 12, 0x02 },   /* bit 39 */   { 12, 0x01 },   /* bit 40 */
   { 13, 0x80 },   /* bit 41 */   { 13, 0x40 },   /* bit 42 */   { 13, 0x20 },   /* bit 43 */   { 13, 0x10 },   /* bit 44 */
   { 13, 0x08 },   /* bit 45 */   { 13, 0x04 },   /* bit 46 */   { 13, 0x02 },   /* bit 47 */   { 13, 0x01 },   /* bit 48 */
   { 14, 0x80 },   /* bit 49 */   { 14, 0x40 },   /* bit 50 */   { 14, 0x20 },   /* bit 51 */   { 14, 0x10 },   /* bit 52 */
   { 14, 0x08 },   /* bit 53 */   { 14, 0x04 },   /* bit 54 */   { 14, 0x02 },   /* bit 55 */   { 14, 0x01 },   /* bit 56 */
   { 15, 0x80 },   /* bit 57 */   { 15, 0x40 },   /* bit 58 */   { 15, 0x20 },   /* bit 59 */   { 15, 0x10 },   /* bit 60 */
   { 15, 0x08 },   /* bit 61 */   { 15, 0x04 },   /* bit 62 */   { 15, 0x02 },   /* bit 63 */   { 15, 0x01 }   /* bit 64 */
   };


  //farad 8/13/1998 the last modifications.

/************************************************
  Bit Field Data Record
*************************************************/
struct bitfield_data 
   {
   enum bitfields  bd_fieldid;                           /* id for this field */
   pBYTE           p_bd_pdata;                           /* ptr to data to move */
   INT             bd_fieldlen;                          /* # of bytes in iso8583 field */
   INT             (*bd_BFCond)(enum bitfields, BYTE);  /* ptr to conditional function */
   void            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
   void            (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
   };


//struct bitfield_data {
  // enum bitfields  bd_fieldid;                           /* id for this field */
  // unsigned char   *bd_pdata;                            /* ptr to data to move */
  // int             bd_fieldlen;                          /* # of bytes in visa iso field */
  // int             (*bd_BFCond)(enum bitfields, char *); /* ptr to conditional function */
  // void            (*bd_BFMove)(enum bitfields);         /* ptr to move function */
  // void            (*bd_BFRsp)(enum bitfields);          /* ptr to response handler function */
  // };


/**********************************
      FUNCTION PROTOTYPES
**********************************/
/* functions local */
#ifdef USE_ORIGINAL
INT CondProc(enum bitfields, BYTE);
INT CondExpd(enum bitfields, BYTE);
INT CondPan(enum bitfields, BYTE);
INT CondTrack1(enum bitfields, BYTE);
INT Move_Always(enum bitfields, BYTE);
INT Move_IfThere(enum bitfields, BYTE);
INT vMove_IfThere(enum bitfields, BYTE);
INT Move_Never(enum bitfields, BYTE);
void EBCDICMove(enum bitfields);
void vEBCDICMove(enum bitfields);
void RspEBCDICvMove(enum bitfields);
void RspEBCDICMove(enum bitfields);
void RspIgnore(enum bitfields);
void vRspIgnore(enum bitfields);
void Rj_with_lead_zeros( pCHAR str, INT FieldLen );
BYTE ascii_ebcdic_conv(BYTE conv_char, pCHAR conv_type );
INT Move23_IfThere(enum bitfields, BYTE);

void Build_Host_Header (pAUTH_TX);
void asc_to_vbcd (pCHAR, INT, BYTE [] );
void asc_to_bcd (pCHAR, INT, BYTE [] );
void str_asc_to_bcd(pCHAR, INT, BYTE [] ); 
void vBCDMove(enum bitfields fieldid);
void RspMovevBCD (enum bitfields fieldid);
void GenericMove(enum bitfields fieldid);
void RspMoveGeneric(enum bitfields fieldid);
void vDataMove(enum bitfields fieldid);
void RspMovevData (enum bitfields fieldid);
#else

	static INT CondProc(enum bitfields, BYTE);
	static INT CondExpd(enum bitfields, BYTE);
	static INT CondPan(enum bitfields, BYTE);
	static INT CondTrack1(enum bitfields, BYTE);
	static INT Move_Always(enum bitfields, BYTE);
	static INT Move_IfThere(enum bitfields, BYTE);
	static INT Move23_IfThere(enum bitfields, BYTE);
	static INT vMove_IfThere(enum bitfields, BYTE);
	static INT Move_Never(enum bitfields, BYTE);
	static void EBCDICMove(enum bitfields);
	static void vEBCDICMove(enum bitfields);
	static void RspEBCDICvMove(enum bitfields);
	static void RspEBCDICMove(enum bitfields);
	static void RspIgnore(enum bitfields);
	static void vRspIgnore(enum bitfields);
	void Rj_with_lead_zeros( pCHAR str, INT FieldLen );
	static BYTE ascii_ebcdic_conv(BYTE conv_char, pCHAR conv_type );

	void Build_Host_Header (pAUTH_TX);
	static void asc_to_vbcd (pCHAR, INT, BYTE [] );
	static void asc_to_bcd (pCHAR, INT, BYTE [] );
	static void str_asc_to_bcd(pCHAR, INT, BYTE [] ); 
	static void vBCDMove(enum bitfields fieldid);
	static void RspMovevBCD (enum bitfields fieldid);
	static void GenericMove(enum bitfields fieldid);
	static void RspMoveGeneric(enum bitfields fieldid);
	static void vDataMove(enum bitfields fieldid);
	static void RspMovevData (enum bitfields fieldid);

		
#endif

//The next struct was modified on 8/13/98 by farad.

static  struct bitfield_data BFData[] = {
   {
   bf_2pan,                      /* field */
   HOST_MESSAGE.pan_len,         /* data to move */
   1,                            /* # of bytes in pan len field */
   Move_IfThere,                 /* conditional  */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_3proc,                     /* field */
   HOST_MESSAGE.proccode,        /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_4amount,                   /* field */
   HOST_MESSAGE.amount_bin,      /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_5settleamt,                /* field */
   HOST_MESSAGE.settle_amt,      /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_6crdbillamt,               /* field */
   HOST_MESSAGE.card_bill_amt,   /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_7trntime,                  /* field */
   HOST_MESSAGE.gmt_time,        /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_9setconvrate,              /* field */
   HOST_MESSAGE.conv_rate_stl,   /* data to move */
   4,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_10convrate,                /* field */
   HOST_MESSAGE.conv_rate,       /* data to move */
   4,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_11stan,                    /* field */
   HOST_MESSAGE.stan_bin,        /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_12time,                    /* field */
   HOST_MESSAGE.time_bin,        /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_13date,                    /* field */
   HOST_MESSAGE.date_bin,         /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_14expiry,                  /* field */
   HOST_MESSAGE.date_expd,       /* data to move */
   2,                            /* # of bytes in visa iso field */
   CondExpd,                     /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_15datestl,                 /* field */
   HOST_MESSAGE.date_settle,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_16convdate,                /* field */
   HOST_MESSAGE.conv_date,       /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_18sicnumber,               /* field */
   HOST_MESSAGE.sic_number,      /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_19ctrycd,                  /* field */
   HOST_MESSAGE.ctry_code,       /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_20panctrycd,                    /* field */
   HOST_MESSAGE.pan_ctry_cd,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_21instctrycd,                    /* field */
   HOST_MESSAGE.fwd_inst_ctry_cd,     /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_22pose,                    /* field */
   HOST_MESSAGE.posentry,         /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_23cardseqnum,              /* field */
   HOST_MESSAGE.cardseqnum,      /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move23_IfThere,               /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_25pocc,                    /* field */
   HOST_MESSAGE.pos_condcd,      /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_26pincc,                 /* field */
   HOST_MESSAGE.pin_captcd,      /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_28fee,	                 /* field */
   HOST_MESSAGE.tran_fee,        /* data to move */
   9,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_32idcode,                  /* field */
   HOST_MESSAGE.acq_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_33fwdidcode,                    /* field */
   HOST_MESSAGE.fwd_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_34panext,                    /* field */
   HOST_MESSAGE.pan_ext_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_35track2,                  /* field */
   HOST_MESSAGE.track2_len,      /* data to move */
   1,                            /* # of bytes in track2 len field */
   vMove_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_37rrn,                     /* field */
   HOST_MESSAGE.rrn,             /* data to move */
   12,                           /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_38authcode,                /* field */
   HOST_MESSAGE.authcode,        /* data to move */
   6,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_39respcode,                /* field */
   HOST_MESSAGE.responsecd,      /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_40src,                     /* field */
   HOST_MESSAGE.src,             /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_41termid,                  /* field */
   HOST_MESSAGE.termid,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_42cardacc,                 /* field */
   HOST_MESSAGE.cardaccid,       /* data to move */
   15,                           /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_43cardaccname,             /* field */
   HOST_MESSAGE.cardaccname,     /* data to move */
   40,                           /* # of bytes in visa iso field */
   Move_IfThere,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_44respdata,                /* field */
   HOST_MESSAGE.addl_rsp_len,    /* data to move */
   1,                            /* # of bytes in resp data len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_45track1,                  /* field */
   HOST_MESSAGE.track1_len,      /* data to move */
   1,                            /* # of bytes in track1 len field */
   CondTrack1,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_48addl_data,               /* field */
   HOST_MESSAGE.addl_data_len,   /* data to move */
   1,                            /* # of bytes in addl data len field */
   Move_IfThere,                 /* conditional    */
   vEBCDICMove,                    /* move routine */
   RspEBCDICvMove                  /* response routine */
   },
   {
   bf_49trncurcd,                /* field */
   HOST_MESSAGE.tran_curr_cd,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_50stlmtcurrcd,             /* field */
   HOST_MESSAGE.settle_cur_cd,   /* data to move */
   2,                            /* # of bytes in visa sms iso field */
   Move_Always,                 /* conditional  */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_51crdcurcd,                /* field */
   HOST_MESSAGE.card_curr_cd,    /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_52pinblock,                /* field */
   HOST_MESSAGE.pin_block,       /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_53seccntl,                 /* field */
   HOST_MESSAGE.sec_cntl,        /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_54addlamts,                /* field */
   HOST_MESSAGE.addl_len,        /* data to move */
   1,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_55ICCdata,                /* field */
   HOST_MESSAGE.ICCdata_len,      /* data to move */
   1,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_59posdata,                 /* field */
   HOST_MESSAGE.posdata_len,     /* data to move */
   1,                            /* # of bytes in addl len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_60priv60,                  /* field */
   HOST_MESSAGE.priv60_len,      /* data to move */
   1,                            /* # of bytes in priv60 len field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_61priv61,                  /* field */
   HOST_MESSAGE.priv61_len,      /* data to move */
   1,                            /* # of bytes in priv61 len field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_62priv62,                  /* field */
   HOST_MESSAGE.priv62_len,      /* data to move */
   1,                            /* # of bytes in priv62 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_63priv63,                  /* field */
   HOST_MESSAGE.priv63_len,      /* data to move */
   1,                            /* # of bytes in priv63 len field */
   Move_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_66setl_code,               /* field */
   HOST_MESSAGE.setl_code,       /* data to move */
   1,                            /* # of bytes in visa sms iso field */
   Move_IfThere,                  /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_68rcvcntrycd,               /* field */
   HOST_MESSAGE.rcv_Inst_cnty_cd,       /* data to move */
   2,                            /* # of bytes in visa sms iso field */
   Move_IfThere,                  /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_69setlcntrycd,               /* field */
   HOST_MESSAGE.set_Inst_cnty_cd,       /* data to move */
   2,                            /* # of bytes in visa sms iso field */
   Move_IfThere,                  /* mandatory    */
   GenericMove,                   /* move routine */
   RspMoveGeneric                 /* response routine */
   },
   {
   bf_70nmicode,                 /* field */
   HOST_MESSAGE.nmi_code,        /* data to move */
   2,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_73action,                  /* field */
   HOST_MESSAGE.action,          /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_74credit_cnt,               /* field */
   HOST_MESSAGE.credit_num,          /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_75crd_rev_cnt,               /* field */
   HOST_MESSAGE.credit_rev_num,          /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_76debit_cnt,               /* field */
   HOST_MESSAGE.debit_num,          /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_77dbt_rev_cnt,               /* field */
   HOST_MESSAGE.debit_rev_num,          /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   
   {
   bf_86credit_amt,               /* field */
   HOST_MESSAGE.credit_amt,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_87crd_rev_amt,               /* field */
   HOST_MESSAGE.credit_rev_amt,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_88debit_amt,               /* field */
   HOST_MESSAGE.debit_amt,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_89dbt_rev_amt,               /* field */
   HOST_MESSAGE.debit_rev_amt,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },


   {
   bf_90orgdata,                 /* field */
   HOST_MESSAGE.orig_data,       /* data to move */
   21,                           /* # of bytes in visa iso field */
   Move_IfThere,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_91fileupdate,              /* field */
   HOST_MESSAGE.file_update,     /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Always,                  /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_93rspind,                  /* field */
   HOST_MESSAGE.resp_ind,        /* data to move */
   5,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_95replamts,                /* field */
   HOST_MESSAGE.repl_amts,       /* data to move */
   42,                           /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional  */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_96msgsec,                  /* field */
   HOST_MESSAGE.msgsec,          /* data to move */
   8,                            /* # of bytes in visa iso field */
   Move_Always,                  /* mandatory    */
   GenericMove,                  /* move routine */
   RspMoveGeneric                /* response routine */
   },
   {
   bf_97net_setl,                /* field */
   HOST_MESSAGE.net_setl_amount,   /* data to move */
   17,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                  /* mandatory    */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_99setl_id,                 /* field */
   HOST_MESSAGE.setl_id_len,     /* data to move */
   7,                           /* # of bytes in cirrus iso field */
   Move_IfThere,                  /* mandatory    */
   vBCDMove,                  /* move routine */
   RspMovevBCD                /* response routine */
   },
   {
   bf_100rcvid,                  /* field */
   HOST_MESSAGE.recv_inst_len,   /* data to move */
   7,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_101fname,                  /* field */
   HOST_MESSAGE.fname_len,       /* data to move */
   1,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_102acct1,                  /* field */
   HOST_MESSAGE.acct_id_1_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_103acct2,                  /* field */
   HOST_MESSAGE.acct_id_2_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_104trandesc,               /* field */
   HOST_MESSAGE.trandesc_len,    /* data to move */
   1,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_105TrpleDES,               /* field */
   HOST_MESSAGE.cwk,    /* data to move */
   16,                            /* # of bytes in track1 len field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                  /* move routine */
   RspEBCDICMove                /* response routine */
   },
   {
   bf_118intractry,              /* field */
   HOST_MESSAGE.intra_ctry_len,  /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_Never,                   /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
   {
   bf_120orgmsgid,               /* field */
   HOST_MESSAGE.org_msg_id,      /* data to move */
   3,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vBCDMove,                     /* move routine */
   RspMovevBCD                   /* response routine */
   },
   {
   bf_121issid,                  /* field */
   HOST_MESSAGE.iss_inst_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                   /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_122openuse,                /* field */
   HOST_MESSAGE.open_use_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   EBCDICMove,                   /* move routine */
   RspEBCDICMove                 /* response routine */
   },
   {
   bf_123addverf,                /* field */
   HOST_MESSAGE.addr_verf_len,    /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
   bf_124freeform,               /* field */
   HOST_MESSAGE.free_text_len,   /* data to move */
   1,                            /* # of bytes in visa iso field */
   Move_IfThere,                 /* conditional */
   vEBCDICMove,                  /* move routine */
   RspEBCDICvMove                /* response routine */
   },
   {
    bf_125supinfo,               /* field */
    HOST_MESSAGE.sup_info_len,   /*data to move */
    1,							 /* # of bytes in visa iso field */ 
    Move_IfThere,				 /* conditional */
	vEBCDICMove,                  /* move routine */
    RspEBCDICvMove                   /* response routine */
   },

   {
   bf_126priv126,                 /* field */
   HOST_MESSAGE.de126_len,     /* data to move */
   1,                            /* # of bytes in priv60 len field */
   vMove_IfThere,                 /* conditional */
   vDataMove,                    /* move routine */
   RspMovevData                  /* response routine */
   },
//   {
//   bf_127filerecs,               /* field */
//   HOST_MESSAGE.file_rec_len,   /* data to move */
//   1,                            /* # of bytes in visa iso field */
//   Move_IfThere,                 /* conditional */
//   vDataMove,                    /* move routine */
//   RspMovevData                  /* response routine */
//   },

   {
   bf_done,                      /* field */
   NULL,                         /* data to move */
   99,                           /* # of bytes in visa iso field */
   NULL,                         /* conditional */
   NULL,                         /* move routine */
   NULL                          /* response routine */
   }
   }; /* end BFData */


#define N_BFData ( sizeof(BFData) / sizeof (struct bitfield_data) )
          

/*******************************************************************************************************/
/* ROUTINES                                                                                            */
/*******************************************************************************************************/
#ifdef USE_ORIGINAL
	struct bitfield_data *Find_Bitfield_Table (enum bitfields);
	struct trandef       *Find_Tran_Table     (BYTE trantype);
#else
	static struct bitfield_data *Find_Bitfield_Table (enum bitfields);
	static struct trandef       *Find_Tran_Table     (BYTE trantype);

#endif

/*****************************************************************************

  Function:    GenericMove

  Description: This function is the generic move function that will move
               data from the HOST_MESSAGE structure to the the raw ISO message.
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void GenericMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      memcpy(&ReqRsp_Buffer[reqidx],      /* move to request buffer */
             pBF->p_bd_pdata,         /* from data pointer      */
             pBF->bd_fieldlen);      /* for length of field    */

      reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
   }
} /* end GenericMove */

/*****************************************************************************

  Function:    bcd_to_asc

  Description: This function will convert bcd string to an ascii string 
  Author: 
      Farad       8/13/98
  Inputs:
      bstring  - the bcd string to conver to ascii
      blen     - the length of bstring
  Outputs:
      astr     - the new ascii string
      astr_len - the length of astr
  Return values:
      None
  Modified by:
******************************************************************************/
static void bcd_to_asc (bstring, blen, astr, astr_len)
   BYTE bstring[];
   INT          blen;
   pCHAR        astr;
   INT          astr_len;
{
   int  i = 0;

   for (i = 0; i < (blen * 2); i=i+2)
      {
      astr[i] = (bstring[i/2] / 16) + '0';
      astr[i+1] = (bstring[i/2] & 0x0F) + '0';
      }

   astr [(blen * 2)] = '\0';
} /* bcd_to_asc */

/*****************************************************************************

  Function:    RspMoveGeneric

  Description: This function is the generic move function that will move
               data from the raw ISO message to the HOST_MESSAGE structure.
  Author: 
      Farad       8/13/98
  Inputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
      fieldid                - the bit field of the data to be moved
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data from the incominging ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspMoveGeneric(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
        /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if ((reqidx + pBF->bd_fieldlen) <= buflen)
   {
      if (pBF != NULL)
      {
         memcpy(pBF->p_bd_pdata,         /* to data pointer      */
                &ReqRsp_Buffer[reqidx],      /* move from request buffer */
                pBF->bd_fieldlen);      /* for length of field    */

         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      }
   }
   else
      invalid_msg_format = true;

} /* end RspMoveGeneric */

/*****************************************************************************

  Function:    vEBCDICMove

  Description: This function is the  move function that will move data
               from the HOST_MESSAGE structure (after converting it to 
               EBCDIC) to the the raw ISO message.  Note: it moves the 
               number indicated by the length variable pointed to by 
               pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void vEBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

	//The next 4 lines were modified by farad on 8/13/98
   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata[0];
      ReqRsp_Buffer [reqidx] = pBF->p_bd_pdata[0];
      reqidx++;

      for( i=1; i<=movelen; i++ )
      {
         ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
         reqidx++;
      }
   }
} /* end vEBCDICMove */

/*****************************************************************************

  Function:    EBCDICMove

  Description: This function is the  move function that will move data
               from the HOST_MESSAGE structure (after converting it to 
               EBCDIC) to the the raw ISO message.  Note: it moves the 
               number indicated by the length variable pointed to by 
               pBF->bd_fieldlen.
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void EBCDICMove( enum bitfields fieldid )
{
   struct bitfield_data *pBF;
   INT                  movelen;
   INT                  i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      for( i=0; i<movelen; i++ )
      {
         ReqRsp_Buffer[reqidx] = ascii_ebcdic_conv (pBF->p_bd_pdata[i], A2E);
         reqidx++;
      }
   }
} /* end EBCDICMove */

/*****************************************************************************

  Function:    RspEBCDICvMove

  Description: This function will move data from the the raw ISO message 
               (after converting it from EBCDIC) to the HOST_MESSAGE structure.  
               Note: it moves the number indicated by the length variable pointed 
               to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspEBCDICvMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx];
      if ((reqidx+movelen+1) <= buflen)
      {
         pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
         reqidx++;

         /* Move the data, converting it from EBCDIC to ASCII. */
         for( i=1; i<=movelen; i++)
         {
            pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
            reqidx++;
         }
      }
      else
         invalid_msg_format = true;
   }
} /* end RspEBCDICvMove */

/*****************************************************************************

  Function:    RspEBCDICMove

  Description: This function will move data from the the raw ISO message 
               (after converting it from EBCDIC) to the HOST_MESSAGE structure.  
               Note: it moves the number indicated by the length variable pointed
               to by pBF->bd_fieldlen.
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void RspEBCDICMove (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  i;
   int                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);
   if (pBF != NULL)
   {
      movelen = pBF->bd_fieldlen;

      /* Move the data, converting it from EBCDIC to ASCII. */
      if ((reqidx + movelen) <= buflen)
      {
         for( i=0; i<movelen; i++)
         {
            pBF->p_bd_pdata [i] = ascii_ebcdic_conv (ReqRsp_Buffer[reqidx], E2A);
            reqidx++;
         }
      }
      else
         invalid_msg_format = true;
   }
} /* end RspEBCDICMove */

/*****************************************************************************

  Function:    Rj_with_lead_zeros

  Description: This function will right justify a string and add leading zeros
  Author: 
      unknown
  Inputs:
      str        - the ascii string to be right justified 
      FieldWidth
  Outputs:
      str        - the right justified ascii string
  Return values:
      None
  Modified by:
******************************************************************************/
void Rj_with_lead_zeros( pCHAR str, INT FieldWidth )
{
   INT i;
   INT dif;
   INT strlength;

   strlength = strlen(str);

   /* Convert any leading blanks to zero. */
   for (i = 0; i < strlength; i++)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x30;
      else
         break;
   }

   /* Remove any trailing blanks. */
   for (i = strlength-1; i >= 0; i--)
   {
      if ( 0x20 == str[i] )
         str [i] = 0x00;
      else
         break;
   }

   strlength = strlen(str);
   dif       = FieldWidth - strlength;

   if (dif > 0)
   {
      /* Right justify, then prepend leading zeros. */
      memmove (&str [dif], &str[0], strlength+1);
      memset  (&str[0], 0x30, dif);
   }
}

/*****************************************************************************

  Function:    RspIgnore  

  Description: This function will not copy the response field, but will increment
               the index by the length of the response field.
  Author: 
      unknown
  Inputs:
      fieldid   - the bit field of the data not to be moved
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;


   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      if ((reqidx + pBF->bd_fieldlen) <= buflen)
         reqidx = reqidx + pBF->bd_fieldlen;   /* increment buffer pointer */
      else
         invalid_msg_format = true;
   }
} /* end RspIgnore */

/*****************************************************************************

  Function:    RspIgnore  

  Description: This function will not copy the response field, but will increment
               the index by the variable length of the response field.
  Author: 
      unknown
  Inputs:
      fieldid   - the bit field of the data not to be moved
  Outputs:
      None
  Return values:
      None
  Modified by:
      Farad      8/13/98
******************************************************************************/
static  void vRspIgnore(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   int                  movelen;

   /*---------------------------------------------------*/
   /* Find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx] + pBF->bd_fieldlen;

      if ((reqidx + movelen) <= buflen)
         reqidx = reqidx + movelen;   /* increment buffer pointer */
      else
         invalid_msg_format = true;
   }
} /* end vRspIgnore */

/*****************************************************************************

  Function:    bcd_to_int   

  Description: This function will convert a bcd string into an integer. 
  Author: 
      unknown
  Inputs:
      bcd_data  - a pointer to the bcd string to be converted
      bcd_len   - the length of the string pointed to by bcd_data
  Outputs:
      None
  Return values:
      INT - the integer created by converting bcd_data to an integer.
  Modified by:
******************************************************************************/
static  INT bcd_to_int ( BYTE *bcd_data,
                 int           bcd_len    )
{
   INT   temp_int;
   INT   multiplier;
   INT   i;


   multiplier = 1;
   temp_int   = 0;
   
   for (i = (bcd_len-1); i >= 0; i--)
      {
      temp_int+= (bcd_data [i] & 0x0f) * multiplier;
      multiplier = multiplier * 10;
      temp_int+= (bcd_data [i] / 16) * multiplier;
      multiplier = multiplier * 10;
      }

   return (temp_int);
}

/*****************************************************************************

  Function:    vBCDMove 

  Description: This function will move data from the HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void vBCDMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* Determine number of bytes to move. */
   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata[0];
      ReqRsp_Buffer[reqidx] = pBF->p_bd_pdata[0];
      reqidx++;

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen = (movelen / 2);

      memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vBCDMove */

/*****************************************************************************

  Function:    

  Description: This function will move data from the raw ISO message to 
               the HOST_MESSAGE structure.  
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure for the outgoing ISO message
  Return values:
      None
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  void RspMovevBCD (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer[reqidx];
      pBF->p_bd_pdata[0] = ReqRsp_Buffer[reqidx];
      reqidx++;

      if (( 60 == fieldid ) || ( 61 == fieldid ) || ( 120 == fieldid ))
         movelen *= 2;

      if ((movelen % 2) != 0)
         movelen++;

      movelen/= 2;

      if (( reqidx + movelen) <= buflen)
      {
         memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

         reqidx = reqidx + movelen;   /* increment buffer pointer */
      }
      else
         invalid_msg_format = true;
   }
} /* end RspMovevBCD */

/*****************************************************************************

  Function:    vDataMove

  Description: This function will move data from the HOST_MESSAGE structure to 
               the the raw ISO message.  Note: it moves the number indicated 
               by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void vDataMove(enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = pBF->p_bd_pdata [0];
      ReqRsp_Buffer [reqidx] = pBF->p_bd_pdata [0];
      reqidx++;

      memcpy(&ReqRsp_Buffer[reqidx], &pBF->p_bd_pdata[1], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end vDataMove */

/*****************************************************************************

  Function:    RspMovevData  

  Description: This function will move data from the the raw ISO message 
               to the HOST_MESSAGE structure.  Note: it moves the number 
               indicated by the length variable pointed to by pBF->p_db_data[0].
  Author: 
      unknown
  Inputs:
      fieldid                - the bit field of the data to be moved
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               outgoing ISO message
  Return values:
      None
  Modified by:
******************************************************************************/
static  void RspMovevData (enum bitfields fieldid)
{
   struct bitfield_data *pBF;
   INT                  movelen;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   if (pBF != NULL)
   {
      movelen = ReqRsp_Buffer [reqidx];
      pBF->p_bd_pdata[0] = ReqRsp_Buffer [reqidx];
      reqidx++;

      memcpy(&pBF->p_bd_pdata[1], &ReqRsp_Buffer[reqidx], movelen);

      reqidx = reqidx + movelen;   /* increment buffer pointer */
   }
} /* end RspMovevData */

/*****************************************************************************

  Function:    CondPan   

  Description: This function will move the account number if it is there.
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of the account number 
      p_trantype - pointer to the tran type
  Outputs:
      None
  Return values:
      true  - account number moved
      false - account number not moved
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondPan(enum bitfields bitfield, BYTE p_trantype)
{
/* JH Apr 29  changed procedure to only send the pan if it is there */
   if (HOST_MESSAGE.pan_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(Move_IfThere(bitfield, p_trantype));
  
return(false);
} /* end CondPan */

/*****************************************************************************

  Function:    CondExpd

  Description: This function will move the expiration dat if it is there.
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of the account number 
      p_trantype - pointer to the tran type
  Outputs:
      None
  Return values:
      true  - expiration date moved
      false - expiration date not moved
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondExpd(enum bitfields bitfield, BYTE p_trantype)
{
        /* if track2 data here, no need for expdate field */
   if (HOST_MESSAGE.date_expd [1] != 0x00)    /* JH Apr 29  Fixed bug */
       return(true);
  
return(false);
} /* end CondExpd */

/*****************************************************************************

  Function:    CondProc

  Description: This function will determine whether the message type is not 
               a reversal. 
  Author: 
      unknown
  Inputs:
      bitfield - the bit field of the account number 
      trantype - tran type
  Outputs:
      None
  Return values:
      true  - not reversal
      false - reversal
  Modified by:
      BGable      6/30/98
******************************************************************************/
static  INT CondProc(enum bitfields bitfield, BYTE trantype)
{
   if (HOST_MESSAGE.msgtype_bcd [0] != 0x04)
      return (true);

   return(false);
} /* end CondProc */

/*****************************************************************************

  Function:    CondTrack1

  Description: This function will determine whether or not there is a track1. 
  Author: 
      unknown
  Inputs:
      bitfield   - the bit field of track1
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - track1 
      false - no track1
  Return values:
      None
  Modified by:
******************************************************************************/
static  INT CondTrack1 (enum bitfields bitfield, BYTE p_trantype)
{
   if (HOST_MESSAGE.track1_len [0] != 0x00)   /* JH Apr 29  Fixed bug */
       return(true);

return(false);
} /* end CondTrack1 */

/*****************************************************************************

  Function:    Move_IfThere

  Description: This function will determine if the data for fieldid is there.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is there 
      false - field not there
  Modified by:
******************************************************************************/
static  INT Move_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;
        INT i;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
      for (i=0; i < pBF->bd_fieldlen; i++)
         {
         if ( (pBF->p_bd_pdata [i] != 0) && (pBF->p_bd_pdata [i] != ' ') ) 
            return(true);
         }
   }
   return(false);
} /* end Move_IfThere */

/*****************************************************************************

  Function:    Move23_IfThere

  Description: This function will determine if field 23 is present
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is  there 
      false - field not there
******************************************************************************/
static INT Move23_IfThere( enum bitfields fieldid, BYTE p_trantype )
{
   struct bitfield_data *pBF;
   INT    retval = false;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /*---------------------------------------------------*/
   /* in any byte non-zero, it's there, return true     */
   /*---------------------------------------------------*/
   if (pBF != NULL)
   {
      retval = de23_present;
      de23_present = false;
   }
   return( retval );
} /* end Move23_IfThere */


/*****************************************************************************

  Function:    vMove_IfThere 
  Description: This function will determine if the data for fieldid is there for
               fields with variable lengths.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field is there 
      false - field not there
  Modified by:
******************************************************************************/
static  INT vMove_IfThere(enum bitfields fieldid, BYTE p_trantype)
{
   struct bitfield_data *pBF;

   /*---------------------------------------------------*/
   /* find pointer to bitfield info for this bitfield...*/
   /*---------------------------------------------------*/
   pBF = Find_Bitfield_Table(fieldid);

   /* if length byte is non-zero */
   if (pBF != NULL)
   {
      if (pBF->p_bd_pdata [0] != 0)
         return(true);
   }
   return(false);
} /* end Move_IfThere */

/*****************************************************************************

  Function:    Move_Always

  Description: This function will cause data to be move always by always 
               returning true.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      true  - field should be moved
  Modified by:
******************************************************************************/
static  INT Move_Always(enum bitfields bitfield, BYTE p_trantype)
{
   return(true);
} /* end Move_Always */

/*****************************************************************************

  Function:    Move_Never

  Description: This function will cause data to never be moved by always 
               returning false.
  Author: 
      unknown
  Inputs:
      fieldid    - the field id of the data to be moved
      p_trantype - a pointer to tran type
  Outputs:
      None
  Return values:
      false  - field should not be moved
  Modified by:
******************************************************************************/
static  INT Move_Never(enum bitfields bitfield, BYTE p_trantype)
{
   return(false);
} /* end Move_Never */

/*****************************************************************************

  Function:    Find_Tran_Table

  Description: This function will determine whether the given trantype is in
               the tran table.  If it is, it will return a pointer to the data
               for that trantype.
  Author: 
      unknown
  Inputs:
      trantype - the transaction type
  Outputs:
      None
  Return values:
      pointer to the trandef for the trantype
  Modified by:
******************************************************************************/
static  struct trandef *Find_Tran_Table (BYTE trantype)
{
   struct trandef *ptrandef = NULL;   /* point to return record */
   int      i;                        /* loop index */

   
   for ( i=0; (i <= N_TRANENTRY) && (TRANTABLE[i].td_trantype > 0); 
         i++)
   {
      if ( TRANTABLE[i].td_trantype == trantype )
      {
         ptrandef = &TRANTABLE[i];
         return(ptrandef);
         break;
      }
   }
   if (NULL == ptrandef)
   {
      ncvsms_log_message( 3, 2, "This trantype not found in TRANTABLE" , "Find_Tran_Table" , 0 ,"");
      return(ptrandef);
   }
   return (NULL);
} /* Find_Tran_Table */   

/*****************************************************************************

  Function:    Find_Bitfield_Table

  Description: This function will determine whether the bitfield is in the
               bitfield table.
  Author: 
      unknown
  Inputs:
      bitfield - the bitfield to search for
  Outputs:
      None
  Return values:
      pointer to the bitfield data for the bitfield entered.
  Modified by:
******************************************************************************/
static  struct bitfield_data *Find_Bitfield_Table (enum bitfields bitfield)
{
   struct bitfield_data *pBF = NULL;      /* point to return record       */
   INT      i;            /* loop index         */
   CHAR     temp_str[100] = {0};

   for (i=0; i <= N_BFData; i++)
      if ( BFData[i].bd_fieldid == bitfield)
      {
         pBF = &BFData[i];
         return (pBF);
         break;
      }
 
   if (NULL == pBF)
   {
      sprintf(temp_str, "Bitfield: %d not found in BFData", bitfield);
      ncvsms_log_message( 3, 2, temp_str , "Find_Bitfield_Table" , 0 ,"");
      return (pBF);
   };
 
   return (NULL);
} /* Find_Bitfield_Table */

/*****************************************************************************

  Function:    ascii_ebcdic_conv

  Description: This function will convert from ascii to EBCDIC or from 
               EBCDIC to ascii depending on conv_type.
  Author: 
      unknown
  Inputs:
      conv_char - the character to convert
      conv_type - A2E - convert from ascii to EBCDIC
                  E2A - convert from EBCDIC to ascii
  Outputs:
      None
  Return values:
      BYTE - the converted character
******************************************************************************/
static  BYTE ascii_ebcdic_conv( unsigned char conv_char, char *conv_type )
{
   INT            i;
   INT    found = 0;
   BYTE  ReturnChar;

      /*
        +------------------------------------------------------------+
        |   0     1     2     3     4     5     6     7     8     9  |
        |   a     b     c     d     e     f     g     h     i     j  |
        |   k     l     m     n     o     p     q     r     s     t  |
        |   u     v     w     x     y     z     A     B     C     D  |
        |   E     F     G     H     I     J     K     L     M     N  |
        |   O     P     Q     R     S     T     U     V     W     X  |
        |   Y     Z           !     "     #     $     %     &     '  |
        |   (     )     *     +     ,     -     .     /     :     ;  |
        |   <     =     >     ?     @     \     _     `     {     |  |
        |   }     ~                                                  |
        +------------------------------------------------------------+
      */

   static unsigned char ebcdic_chart[] = 
      {
         0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9,
         0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x91,
         0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0xA2, 0xA3,
         0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xC1, 0xC2, 0xC3, 0xC4,
         0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5,
         0xD6, 0xD7, 0xD8, 0xD9, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7,
         0xE8, 0xE9, 0x40, 0x5A, 0x7F, 0x7B, 0x5B, 0x6C, 0x50, 0x7D,
         0x4D, 0x5D, 0x5C, 0x4E, 0x6B, 0x60, 0x4B, 0x61, 0x7A, 0x5E,
         0x4C, 0x7E, 0x6E, 0x6F, 0x7C, 0xE0, 0x6D, 0x79, 0xC0, 0x4F,
         0xD0, 0xA1, 0x5F
      };

   static unsigned char ascii_chart[] = 
      {
         0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
         0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A,
         0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
         0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x41, 0x42, 0x43, 0x44,
         0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E,
         0x4F, 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
         0x59, 0x5A, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27,
         0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x3A, 0x3B,
         0x3C, 0x3D, 0x3E, 0x3F, 0x40, 0x5C, 0x5F, 0x60, 0x7B, 0x7C,
         0x7D, 0x7E, 0x5E
      };


   if ( 0 == strcmp( conv_type, A2E ) )
   {
      /* CONVERT ASCII TO EBCDIC */
      //the next line is modified by farad 8/13/98
	  for( i=0; i<93 && !found; i++ )
      {
         if ( conv_char == ascii_chart[i] )
         {
            ReturnChar = ebcdic_chart[i];
            found = 1;
         }
      }
   }
   else if ( 0 == strcmp( conv_type, E2A ) )
   {
      /* CONVERT EBCDIC TO ASCII */
      //the next line is modified by farad 8/13/98
      for( i=0; i<93 && !found; i++ )
      {
         if ( conv_char == ebcdic_chart[i] )
         {
            ReturnChar = ascii_chart[i];
            found = 1;
         }
      }
   }

   if ( 0 == found )
      ReturnChar = 0x00;

   return( ReturnChar );
}   //end of the ascii_ebcdic_conv

/*****************************************************************************

  Function:    hhutil_send_net_mngmnt_response

  Description: This function will send the network management response to the host
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT hhutil_send_net_mngmnt_response(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   LONG      ret_code;
   CHAR	    ErrorMsg[512]={0};
   CHAR	    Buffer[512]={0};

   build_host_structure(p_auth_tx);
   Build_Host_Request(p_auth_tx);
   Build_Host_Header(p_auth_tx);

   p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
                                      ST1_NONE,
                                      ST2_NONE,
								              dialog_que_name,
                                      application_que_name,
                                      ReqRsp_Buffer,
                                      buflen,
                                      0 );
       
	   
	ptemsg_set_orig_comms_info(p_msg_host_out, p_auth_tx->TLF01_details.tpdu_id);
     
   if ( NULL_PTR == p_msg_host_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncvsms_log_message( 3, 2, Log_Bufr , "hhutil_send_net_mngmnt_response" , 0 ,p_auth_tx->TLF01_details.card_num);
	   return(false);           
	}
   
   ret_code = pteipc_send(p_msg_host_out, dialog_que_name );
   free(p_msg_host_out);
 	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
		sprintf( Buffer, "pteipc error sending to que %s: %s", dialog_que_name, ErrorMsg );
		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      			p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		strcat(Buffer,Log_Bufr);
		ncvsms_log_message( 3, 3, Buffer , "hhutil_send_net_mngmnt_response" , 0 ,p_auth_tx->TLF01_details.card_num);
		return(false);
	}
 	else
 	{
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT
    
      /* Log transaction to trace file. */
      write_to_txn_file();
    }
 	return(true);
} /* hhutil_send_net_mngmnt_response */

/*****************************************************************************

  Function:    hhutil_send_net_mngmnt_request

  Description: This function will send the network management request to the host
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT hhutil_send_net_mngmnt_request(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	 p_msg_host_out        = NULL_PTR;
   LONG      ret_code;
   CHAR	    ErrorMsg[512]={0};
   CHAR	    Buffer[512]={0};
   CHAR      gmt_time[20];
   
   /* Current Greenwich Mean Time for the transmission-to-host time. */
   pintime_get_gmt( gmt_time );  /* YYYYMMDDhhmmss */
   strncpy( p_auth_tx->TLF01_details.transmission_timestamp,
            &gmt_time[4], 10 );  /* MMDDhhmmss */

   build_host_structure(p_auth_tx);
   Build_Host_Request(p_auth_tx);
   Build_Host_Header(p_auth_tx);

   p_msg_host_out = ptemsg_build_msg( MT_OUTGOING, 
                                      ST1_NONE,
                                      ST2_NONE,
								              dialog_que_name,
                                      application_que_name,
                                      ReqRsp_Buffer,
                                      buflen,
                                      0 );
       
	   
	ptemsg_set_orig_comms_info(p_msg_host_out, p_auth_tx->TLF01_details.tpdu_id);
     
   if ( NULL_PTR == p_msg_host_out )
   {
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Insufficient Memory to build host message Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      ncvsms_log_message( 3, 3, Log_Bufr , "hhutil_send_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);
	   return(false);           
	}
   
   ret_code = pteipc_send(p_msg_host_out, dialog_que_name );
   free( p_msg_host_out );
 	if( ret_code < 0 )
	{
		pteipc_get_errormsg( ret_code, ErrorMsg );
		sprintf( Buffer, "pteipc error sending to que %s: %s", dialog_que_name, ErrorMsg );
		memset(Log_Bufr,0x00,sizeof(Log_Bufr));
		sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      			p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
		strcat(Buffer,Log_Bufr);
		ncvsms_log_message( 3, 3, Buffer , "hhutil_send_net_mngmnt_request" , 0 ,p_auth_tx->TLF01_details.card_num);
		return(false);
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, MGT );//praneeth changed SENT to MGT
    
      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   return(true);
} /* hhutil_send_net_mngmnt_request */


/*****************************************************************************

  Function:    Field62Response

  Description: This function will decode field 62 data from HOST_MESSAGE
               and stores pertinent data into the auth_tx structure.
  Author: 
      unknown
  Inputs:
      HOST_MESSAGE (global)  - structure that contains field 62 data 
  Outputs:
      p_auth_tx - Transaction record - parsed data goes into it
  Return values:
      None
  Modified by:
******************************************************************************/
void Field62Response( pAUTH_TX p_auth_tx )
{
   INT     i;
   INT     cps_bit;
   INT     cps_byte;
   INT     Len;
   INT     TotalLen;
   INT     BitMap_Len = 8;
   BYTE    BitMap[8];
   BYTE    TempStr[51];
   CHAR    temp_txn_id[17];


   /* Get length of field. */
//   TotalLen = (INT)HOST_MESSAGE.priv62_len;
   TotalLen = HOST_MESSAGE.priv62_len[0];

   /* Copy bitmap into local variable. */
   if ( TotalLen > 0 )
   {
      memset( BitMap, 0x00, sizeof(BitMap) );
      for( i=0; i<BitMap_Len; i++ )
         BitMap[i] = HOST_MESSAGE.priv62[i];

      memset( TempStr, 0x00, sizeof(TempStr) );
      memcpy( TempStr, &HOST_MESSAGE.priv62[8],
              sizeof(HOST_MESSAGE.priv62) - BitMap_Len );
   }

   /* Parse the bitmap for 1st 3 bytes, extracting the fields we want. */
   Len = 0;
   for( cps_byte=1; cps_byte<=3; cps_byte++ )
   {
      for( cps_bit=1; cps_bit<=8; cps_bit++ )
      {
         if ( BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
         {
            /* The bit is set. */
            switch( cps_byte )
            {
               case 1: /* Byte 1 */
                      switch( cps_bit )
                      {
                         case 1:  
                                /* CPS Authorization */
                                p_auth_tx->TLF01_details.visa_char_indicator =
                                   ascii_ebcdic_conv( TempStr[Len], E2A );

                                Len++;
                                break;
                         case 2:
                                /* CPS Transaction ID */
                                memset( temp_txn_id, 0x00, sizeof(temp_txn_id));
                                bcd_to_asc( &TempStr[Len], 8, temp_txn_id, 16 );
                                strcpy( p_auth_tx->TLF01_details.visa_tran_id,
                                       &temp_txn_id[1] );
                                Len += 8;
                                break;
                         case 3:
                                /* CPS Validation Code */
                                for( i=0; i<4 && ((Len+i)<sizeof(TempStr)); i++)
                                   p_auth_tx->TLF01_details.visa_validation_code[i]
                                   = ascii_ebcdic_conv( TempStr[Len+i], E2A );
                                Len += 4;
                                break;
                         case 7:
                                Len += 26;
                                break;
                         case 8:
                                Len += 3;
                                break;
                         default:
                                Len++;
                                break;
                      }
                      break;
               case 2: /* Byte 2 */
                      switch( cps_bit+8 )
                      {
                         case 10: Len += 3;
                                  break;
                         case 14: Len += 6;
                                  break;
                         case 16: Len += 2;
                                  break;
                         default: Len++;
                                  break;
                      }
                      break;
               case 3: /* Byte 3 */
                      switch( cps_bit+16 )
                      {
                         case 17:
                                  /*for( i=0; (i<13) && ((Len+i)<sizeof(TempStr)); i++ )                                     p_auth_tx->TLF01_details.cps_master_card_info[i]
                                    = ascii_ebcdic_conv( TempStr[Len+i], E2A );*/
                        	 	 break;

                         case 23:
								/* Product ID */
								/*for( i=0; (i<2) && ((Len+i)<sizeof(TempStr)); i++ )
								{
									 p_auth_tx->TLF01_details.product_codes[7].code[i]
												= ascii_ebcdic_conv( TempStr[Len+i], E2A );
								}
								Len += 2;*/
							break;
                         default: break;
                      }
                      break;
               default: break;
            }  /* End SWITCH based on BYTE */
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
   }  /* End LOOP on BYTE */
}


/*****************************************************************************

  Function:    Field63Response

  Description: This function will decode field 63 data from HOST_MESSAGE
               and stores pertinent data into the auth_tx structure.
  Author: 
      unknown
  Inputs:
      HOST_MESSAGE (global)  - structure that contains field 62 data 
  Outputs:
      p_auth_tx - Transaction record - parsed data goes into it
  Return values:
      None
  Modified by:
******************************************************************************/
void Field63Response( pAUTH_TX p_auth_tx )
{
   INT     i;
   INT     cps_bit;
   INT     cps_byte;
   INT     Len;
   INT     TotalLen=0;
   INT     BitMap_Len = 3;
   BYTE    BitMap[3];
   BYTE    TempStr[100];
   CHAR    temp_txn_id[17];


   /* Get length of field. */
   TotalLen = HOST_MESSAGE.priv63_len[0];

   /* Copy bitmap into local variable. */
   if ( TotalLen > 0 )
   {
      memset( BitMap, 0x00, sizeof(BitMap) );
      for( i=0; i<BitMap_Len; i++ )
         BitMap[i] = HOST_MESSAGE.priv63[i];

      memset( TempStr, 0x00, sizeof(TempStr) );
      memcpy( TempStr, &HOST_MESSAGE.priv63[BitMap_Len],
              TotalLen - BitMap_Len );
   }

   /* Parse the bitmap for 1st 3 bytes bitmap, extracting the fields we want in 1st byte of the bitmap. */
   Len = 0;
   for( cps_byte=1; cps_byte<=2; cps_byte++ )
   {
      for( cps_bit=1; cps_bit<=8; cps_bit++ )
      {
         if ( BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
         {
            /* The bit is set. */
            switch( cps_byte )
            {
               case 1: /* Byte 1 */
                      switch( cps_bit )
                      {
                         case 1:  
                                /* Network ID */
                                memset( temp_txn_id, 0x00, sizeof(temp_txn_id));
                                bcd_to_asc( &TempStr[Len], 2, temp_txn_id, 4 );
										  strcpy( p_auth_tx->MCF01_details.vip_code, temp_txn_id );
										  strcpy( p_auth_tx->TLF01_details.product_codes[10].code, temp_txn_id );
                                Len+= 2;
                                break;
                         case 3:
                                /* Message Reason Code */
                                memset( temp_txn_id, 0x00, sizeof(temp_txn_id));
                                bcd_to_asc( &TempStr[Len], 2, temp_txn_id, 4 );
										  strcpy( p_auth_tx->MCF01_details.max_num_returns, temp_txn_id );
										  strcpy( p_auth_tx->TLF01_details.product_codes[11].code, temp_txn_id );

                                Len += 2;
                                break;
                         case 4:
                                /* Message Reason Code */
                                Len += 2;
                                break;
                         case 5: /* Plus PMC ID */
                                memset( temp_txn_id, 0x00, sizeof(temp_txn_id));
                                bcd_to_asc( &TempStr[Len], 3, temp_txn_id, 6 );
										  strcpy( p_auth_tx->MCF01_details.process_control_id, temp_txn_id );
										  strcpy( p_auth_tx->TLF01_details.product_codes[6].amount, temp_txn_id );
                                Len += 3;
                                break;
                         default:
                                Len++;
                                break;
                      }
                      break;
               case 2: /* Byte 2 */
                      switch( cps_bit+8 )
                      {
                         case 9: Len +=14;
                                  break;
                         case 13: memset( temp_txn_id, 0x00, sizeof(temp_txn_id));
							      bcd_to_asc( &TempStr[Len], 3, temp_txn_id, 6 );
									strcpy( p_auth_tx->MCF01_details.deferred_start_date, temp_txn_id );
							      strcpy( p_auth_tx->TLF01_details.product_codes[4].amount, temp_txn_id );

							      Len +=3;
                                  break;
                         default: Len++;
                                  break;
                      }
                      break;
               default: break;
            }  /* End SWITCH based on BYTE */
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
   }  /* End LOOP on BYTE */
}


/*****************************************************************************

  Function:    convert_from_julian_day

  Description: This function will 
     convert_from_julian_day - Get MMDD from YYDDD
     input string format  = YYDDD
     output string format = MMDD
  Author: 
      unknown
  Inputs:
      julian_str - the julian date to be converted
  Outputs:
      date_time  - MMDD
  Return values:
      None
  Modified by:
******************************************************************************/
void convert_from_julian_day( pCHAR julian_str, pCHAR date_time )
{
   CHAR  Year[3];
   CHAR  Month[3];
   CHAR  Days[4];
   INT   iYear;
   INT   NumDays;
   INT   JulianDay;
   INT   iMonth;
   INT   i;
   INT   DaysOfMonth[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };

   /* Account for Leap Year */
   strncpy( Year, julian_str, 2 );
   iYear = atoi( Year );
   if ( 0 == ( iYear % 4 ) )
      DaysOfMonth[1]++;

   strncpy( Days, julian_str+2, 3 );
   JulianDay = atoi( Days );
   NumDays = 0;
   for( i=0; i<12; i++ )
   {
      if ( NumDays+DaysOfMonth[i] >= JulianDay )
      {
         iMonth = i+1;
         NumDays = JulianDay - NumDays;
         break;
      }
      else
      {
         NumDays += DaysOfMonth[i];
      }
   }
	
	itoa(iMonth, Month, 10);
   Rj_with_lead_zeros( Month, 2 );
	itoa(NumDays, Days, 10);
   Rj_with_lead_zeros( Days, 2 );
	strncpy( date_time, Month, 2 );
   strncat( date_time+2, Days,  2 );
}
/*****************************************************************************

  Function:    Build_Host_Request

  Description: This function will build the host request buffer.
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Outputs:
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted message
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
BOOLEAN Build_Host_Request ( pAUTH_TX p_auth_tx)
{
   struct trandef       *pTranDef;
   struct bitfield_data *pBF;
   INT                  bfidx;         /* index to bitfields for this transaction */

   pTranDef = Find_Tran_Table(p_auth_tx->TLF01_details.tx_key);   /* get pointer to tran table */
   if (pTranDef != NULL)
   {    
      /*-----------------------------------------*/
      /* Copy in Message Id for this transaction */
      /*-----------------------------------------*/
      memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
      bitmapidx = 2 + VISA_HDR_SIZE;            /* point to bitmap     */

      /* point past bitmap   */
      if ( (AUTH_AUTHORIZATION_RESPONSE == p_auth_tx->TLF01_details.tx_key) ||
           (AUTH_CASH_ADVANCE_RESPONSE  == p_auth_tx->TLF01_details.tx_key)  )
      {
         if ((0 != strlen(HOST_MESSAGE.acct_id_1)) ||
             (0 != strlen(HOST_MESSAGE.acct_id_2)) ||
             (0 != strlen(HOST_MESSAGE.iss_inst_id)))  
         {
            reqidx = 18 + VISA_HDR_SIZE;
            ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
            BitFieldSize = EXTENDED_BITFIELD;
         }
         else 
         {
            reqidx = 10 + VISA_HDR_SIZE;
            BitFieldSize = NORMAL_BITFIELD;
         }
      }
      else if ( AUTH_REVERSAL_ADVICE_RESPONSE == p_auth_tx->TLF01_details.tx_key )
      {
         if ( 0 != strlen(HOST_MESSAGE.orig_data) )
         {
            reqidx = 18 + VISA_HDR_SIZE;
            ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;
            BitFieldSize = EXTENDED_BITFIELD;
         }
         else 
         {
            reqidx = 10 + VISA_HDR_SIZE;
            BitFieldSize = NORMAL_BITFIELD;
         }
      }
      else if (AUTH_REVERSAL_RESPONSE == p_auth_tx->TLF01_details.tx_key)
      {
         if ((0 != strlen(HOST_MESSAGE.acct_id_1  )) ||
             (0 != strlen(HOST_MESSAGE.acct_id_2  )) ||
             (0 != strlen(HOST_MESSAGE.iss_inst_id)) ||
             (0 != strlen(HOST_MESSAGE.repl_amts  )) ||
             (0 != strlen(HOST_MESSAGE.intra_ctry ))   )  
         {
            reqidx = 18 + VISA_HDR_SIZE;
            ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
            BitFieldSize = EXTENDED_BITFIELD;
         }
         else 
         {
            reqidx = 10 + VISA_HDR_SIZE;
            BitFieldSize = NORMAL_BITFIELD;
         }
      }
      else if ( (AUTH_CHARGEBACK_RESPONSE          == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_CHARGEBACK_REVERSAL_RESPONSE == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_RECONCILIATION_RESPONSE      == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_ADMINISTRATIVE_TEXT_RESPONSE == p_auth_tx->TLF01_details.tx_key) ||
					 (AUTH_ADMINISTRATIVE_TEXT          == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_KEY_CHANGE_RESPONSE          == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_FUND_TRANSFER_TOTAL_RESPONSE == p_auth_tx->TLF01_details.tx_key))
      {
         reqidx = 18 + VISA_HDR_SIZE;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if ( AUTH_CASH_DISBURSEMENT_ADJUSTMENT == p_auth_tx->TLF01_details.tx_key)
      {
         reqidx = 18 + VISA_HDR_SIZE;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if ( (AUTH_BACKOFFICE_ADJUSTMENT == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_REPRESENTMENT == p_auth_tx->TLF01_details.tx_key) )
      {
         reqidx = 10 + VISA_HDR_SIZE;
         BitFieldSize = NORMAL_BITFIELD;
      }
      else if ( (AUTH_REVERSAL        == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_REVERSAL_REPEAT == p_auth_tx->TLF01_details.tx_key)  )
      {
         reqidx = 18 + VISA_HDR_SIZE;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if ( (AUTH_LOGON == p_auth_tx->TLF01_details.tx_key) ||(AUTH_LOGON_RESPONSE == p_auth_tx->TLF01_details.tx_key) )
      {
         reqidx = 18 + VISA_HDR_SIZE;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else if ( (AUTH_FEE_COLLECTION_ACQUIRER     == p_auth_tx->TLF01_details.tx_key) ||
                (AUTH_FUNDS_DISBURSEMENT_ACQUIRER == p_auth_tx->TLF01_details.tx_key)  )
      {
         if ( (HOST_MESSAGE.pan_len[0] == 0x00) && (HOST_MESSAGE.recv_inst_len[0] != 0x00))
         {
            /* 2 bitmaps */
            reqidx = 18 + VISA_HDR_SIZE;
            ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;
            BitFieldSize = EXTENDED_BITFIELD;
         }
         else
         {
            /* 1 bitmap */
            reqidx = 10 + VISA_HDR_SIZE;
            BitFieldSize = NORMAL_BITFIELD;
         }
      }
      else if (HOST_MESSAGE.de126_len[0] != 0x00)/* check for VbV field 126 & create two bit maps */
      {
         reqidx = 18 + VISA_HDR_SIZE;
         ReqRsp_Buffer[bitmapidx] |= BITTAB[bf_1xbitfield].bit_mask;   /* 2 bitmaps */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else
      {
         reqidx = 10 + VISA_HDR_SIZE;
         BitFieldSize = NORMAL_BITFIELD;
      }

      memcpy( &ReqRsp_Buffer[0+VISA_HDR_SIZE],
              pTranDef->td_msgid,
              sizeof(pTranDef->td_msgid));

      /* Administrative text response can be 0610 or 0630. But they use the
       * same tx_key, which corresponds to 0610.  We may need to change 0610
       * to 0630.
       */
      if ( (HOST_MESSAGE.nmi_code[0] == 0x08) &&
           (HOST_MESSAGE.nmi_code[1] == 0x83)  )
      {
         if ( p_auth_tx->TLF01_details.message_type[2] == '2' )
         {
            /* This is an 0620, not an 0600. So change response value
             * from 0610 to 0630.
             */
            memset( &ReqRsp_Buffer[0+VISA_HDR_SIZE+1], 0x30, 1 );
         }
      }

      /*   
         if this msg doesn't require the original proccode (its already in the ISO
         field), overwrite it with the one from the trandef table.
       */
   
      if ((pTranDef->td_opts2 & topt2_new_proccode) != 0) 
         memcpy (&HOST_MESSAGE.proccode, pTranDef->td_proccode, sizeof(pTranDef->td_proccode));
      

      /*-----------------------------------------------------*/
      /* for all requested bitfields for this transaction... */
      /*-----------------------------------------------------*/
      for (bfidx=0; bfidx < BitFieldSize; bfidx++)
      {
         /*----------------------------*/
         /* if not end of bitfields... */
         /*----------------------------*/
         if (pTranDef->td_bitfields[bfidx] != bf_done)
         {
            /*---------------------------------------------------*/
            /* find pointer to bitfield info for this bitfield...*/
            /*---------------------------------------------------*/
            pBF = Find_Bitfield_Table(pTranDef->td_bitfields[bfidx]);

            if (pBF != NULL)
            {
               if ( pBF->bd_fieldid > BitFieldSize )
                  break;

               if(bfidx == 25)
               {
                  if(strlen(p_auth_tx->TLF01_details.pos_condition_code) == 0)
                     continue;
               }
               /*-----------------------------------------------------------*/
               /* if conditional routine returns true, move the bitfield... */
               /*-----------------------------------------------------------*/
               if ((pBF->bd_BFCond)(pBF->bd_fieldid,p_auth_tx->TLF01_details.tx_key) == true)
               {
                  /*------------------------------*/
                  /* set the bit in the bitmap... */
                  /*------------------------------*/        
                  ReqRsp_Buffer[bitmapidx + BITTAB[pTranDef->td_bitfields[bfidx]].byte_no] |=
                        BITTAB[pTranDef->td_bitfields[bfidx]].bit_mask;

                  /*--------------------------*/
                  /* call the move routine... */
                  /*--------------------------*/
                  (pBF->bd_BFMove)(pBF->bd_fieldid);
               }  /* end if */
            }
         }
         else
         {
            bfidx = BitFieldSize;
         }  /* end if */
      } /* end for */

      buflen = reqidx;
      return(true);
   }
   else
      return(false);
} /* end Build_Request_Message */

/*****************************************************************************

  Function:    hhutil_parse_host_response_msg

  Description: This function will parse a host response message
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx structure
      ReqRsp_Buffer (global) - the buffer that contains the ISO formatted msg
  Outputs:
      HOST_MESSAGE (global)  - structure that contains data to go into the 
                               ISO message
  Return values:
      true  - no error occurred
      false - an error occurred
******************************************************************************/
INT hhutil_parse_host_response_msg(pAUTH_TX p_auth_tx)
{
   struct bitfield_data *pBF;
   INT                  hdr_size;
   INT                  i;
   INT                  ret_val = true;
   INT                  bfidx;    /* index to bitfields for this transaction */
   CHAR				    ErrorMsg[512]={0};
   CHAR                 tid[9];
   CHAR                 mid[16];
   CHAR                 card_num[20];
   CHAR                 tempstr[22];


   memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );
   memset( &Dest_Stat_Id, 0x00, sizeof(Dest_Stat_Id) );

   if (0x1A == ReqRsp_Buffer[0])
   {
      Txn_Is_A_Reject = true;
      hdr_size = VISA_HDR_SIZE + REJECT_VISA_HDR_SIZE;

      /* Get the Reject Code */
      memcpy( HOST_MESSAGE.msgtype_bcd,
             &ReqRsp_Buffer[REJECT_VISA_HDR_SIZE - 2],
             sizeof(HOST_MESSAGE.msgtype_bcd));
 
      bcd_to_asc( HOST_MESSAGE.msgtype_bcd,
                  sizeof(HOST_MESSAGE.msgtype_bcd),
                  p_auth_tx->TLF01_details.message_type,
                  sizeof(p_auth_tx->TLF01_details.message_type) );

      strcpy( ErrorMsg, "Received header reject code: " );
      strcat( ErrorMsg, p_auth_tx->TLF01_details.message_type );

      ret_val = true;
   }

   else if (0x16 == ReqRsp_Buffer[0])
   {
      Txn_Is_A_Reject = false;
      hdr_size = VISA_HDR_SIZE;

      /* Parse the Destination Station Id */
      memcpy( HOST_MESSAGE.dest_stat_id,
             &ReqRsp_Buffer[5],
              sizeof(HOST_MESSAGE.dest_stat_id) );

      /* Store the destination station id */
      bcd_to_asc( HOST_MESSAGE.dest_stat_id,
                  sizeof(HOST_MESSAGE.dest_stat_id),
                  p_auth_tx->incoming_dest_station_id,
                  sizeof(p_auth_tx->incoming_dest_station_id) );

      /* Parse the Source Station Id */
      memcpy( HOST_MESSAGE.src_stat_id,
             &ReqRsp_Buffer[8],
              sizeof(HOST_MESSAGE.src_stat_id) );

      /* Store the source station id */
      bcd_to_asc( HOST_MESSAGE.src_stat_id,
                  sizeof(HOST_MESSAGE.src_stat_id),
                  p_auth_tx->incoming_src_station_id,
                  sizeof(p_auth_tx->incoming_src_station_id) );

      /* Store the Header so we can echo any fields we need to.
       * This is for switched in txns.  Set the first byte to
       * 0x02 to indicate a switched in txn.  If this txn is a
       * response to a switched out txn, this field does not
       * get used.
       */
      HOST_MESSAGE.header[0] = 0x02;
      memcpy( HOST_MESSAGE.header+1, ReqRsp_Buffer, 22 );
   }
   else
   {
      Txn_Is_A_Reject = false;
      ncvsms_log_message( 3, 2, "Invalid message format received. Unexpected Visa SMS Header length" , "hhutil_parse_host_response_msg" , 0 ,"");
      ret_val = false;
   }

   if ( ret_val == true )
   {
      /* Get the Message Type */
      memcpy( HOST_MESSAGE.msgtype_bcd,
             &ReqRsp_Buffer[hdr_size],
              sizeof(HOST_MESSAGE.msgtype_bcd) );

      /* Point past bitmap, but 1st check for extended bitmap. */
      bitmapidx = 2 + hdr_size;
      if ( ReqRsp_Buffer[bitmapidx]  &  BITTAB[bf_1xbitfield].bit_mask )
      {
         reqidx = 18 + hdr_size;         /* Extended     */
         BitFieldSize = EXTENDED_BITFIELD;
      }
      else
      {
         reqidx = 10 + hdr_size;         /* Not Extended */
         BitFieldSize = NORMAL_BITFIELD;
      }

      /*------------------------------------------------------------------
         For every bitfield present in message ( except first bit ) . . .
      -------------------------------------------------------------------*/
      for (bfidx = 2; bfidx <= BitFieldSize; bfidx++)
      {
         if (ReqRsp_Buffer[bitmapidx+BITTAB[bfidx].byte_no] & BITTAB[bfidx].bit_mask)
         {
            /* Bit is on, field is there.... */
            pBF = Find_Bitfield_Table(bfidx);       

            /*---------------------------------------*/
            /* call the Response Handling routine... */
            /*---------------------------------------*/
	
            if (pBF != NULL)
            {
               (pBF->bd_BFRsp)(pBF->bd_fieldid);
               if (true == invalid_msg_format)
               {
                  ncvsms_log_message( 3, 2, "Unable to parse transaction from Visa SMS Network." , "hhutil_parse_host_response_msg" , 0 ,"");
                  ret_val = false;
                  break;
               }
            }
            else
            {
               ret_val = false;
               break;
            }
         }
      }

      if ( Txn_Is_A_Reject == true )
      {
         /* Terminal ID, Merchant ID, and Card Number and log reject message. */
         memset( tid,      0x00, sizeof(tid)      );
         memset( mid,      0x00, sizeof(mid)      );
         memset( card_num, 0x00, sizeof(card_num) );

         memcpy( tid, HOST_MESSAGE.termid,    sizeof(tid)-1 );
         memcpy( mid, HOST_MESSAGE.cardaccid, sizeof(mid)-1 );

         /* Card Number is a little more tricky to get.
          * It is variable length, and might have a leading zero.
          */
         memset( tempstr, 0x00, sizeof(tempstr) );
         bcd_to_asc( HOST_MESSAGE.pan_bin,
                    (HOST_MESSAGE.pan_len[0]+1)/2,
                     tempstr,
                     sizeof(p_auth_tx->TLF01_details.card_num) );

         i = HOST_MESSAGE.pan_len[0] % 2;
         memcpy( card_num, tempstr+i, sizeof(card_num)-1 );

         /* All this work to log a useful message.  Here it is. */
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,
         					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(ErrorMsg,Log_Bufr);
         ncvsms_log_message( 3, 3, ErrorMsg , "hhutil_parse_host_response_msg" , 0 ,p_auth_tx->TLF01_details.card_num);
      }
   }
   return( ret_val );
} /* hhutil_parse_host_response_msg */
/******************************************************************************
 *
 *  NAME:         generate_VISA_EMV_data
 *
 *  DESCRIPTION:  This function steps through the EMV subfield tags.  If one
 *                that is supported by VISA is found, it is copied into the
 *                outgoing Host Message buffer.  The incoming data stream is
 *                not changed.
 *
 *  INPUTS:       emv_len_in  - Length of emv_data_in
 *                emv_data_in - EMV data that comes into ATP
 *
 *  OUTPUTS:      HOST_MESSAGE.ICCdata - EMV data that goes to MC
 *                HOST_MESSAGE.ICCdata_len - Length of EMV data to MC
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/

void genutil_VISA_EMV_To_Auth_Tx( pCHAR emv_raw, INT total_len, pAUTH_TX p_auth_tx )
{
   INT    fieldLength;
   INT    dataIndex;
   INT    bTwoBytesTag;
   BYTE   byteOneOfTag;
   BYTE   byteTwoOfTag;
   BYTE   b_string[4]  = {0};
   CHAR   a_string[4]  = {0};
   CHAR   tempstr[500] = {0};


   dataIndex   = 0;
   if(emv_raw[dataIndex] == 0x01)
   {
		dataIndex=dataIndex+3;
   }
   else if(emv_raw[dataIndex] == 0x00)
   {
	   dataIndex=dataIndex+3;
   }

   while (dataIndex <= total_len)
   {
      bTwoBytesTag = 0;
      byteOneOfTag = emv_raw[dataIndex];
      dataIndex++;
      if ((byteOneOfTag & 0x1F) == 0x1F)
      {
         bTwoBytesTag = 1;
         byteTwoOfTag = emv_raw[dataIndex];
         dataIndex++;
      }
      fieldLength = emv_raw[dataIndex];
      dataIndex++;

      if (bTwoBytesTag)
      {
         if (byteOneOfTag == 0x5F)
         {
            if (byteTwoOfTag == 0x2A) 
            {
               genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
               memcpy(p_auth_tx->EMV_details.currency_code, tempstr, sizeof(p_auth_tx->EMV_details.currency_code)-1);
               dataIndex += fieldLength;
            }
            else if (byteTwoOfTag == 0x34) 
            {
               genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));	
               memcpy(p_auth_tx->EMV_details.pan_sequence_number, tempstr, sizeof(p_auth_tx->EMV_details.pan_sequence_number)-1);
               dataIndex += fieldLength;
            }
         }
         else if (byteOneOfTag == 0x9F)
         {
            switch (byteTwoOfTag)
            {
               case 0x02: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_amount, tempstr, sizeof(p_auth_tx->EMV_details.tran_amount)-1);
                        break;
               case 0x03:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.other_amount, tempstr, sizeof(p_auth_tx->EMV_details.other_amount)-1);
                        break;
               case 0x09: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.version, tempstr, sizeof(p_auth_tx->EMV_details.version)-1);
                        break;
               case 0x10: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.iad, tempstr, sizeof(p_auth_tx->EMV_details.iad)-1);
                        break;
               case 0x1A: 
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.country_code, tempstr+1, sizeof(p_auth_tx->EMV_details.country_code)-1);
                        break;
               case 0x15:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.mcc, tempstr, sizeof(p_auth_tx->EMV_details.mcc)-1);
                        break;
               case 0x1E:
                        memcpy(p_auth_tx->EMV_details.serial_number, &emv_raw[dataIndex], fieldLength);
                        break;
               case 0x26: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.app_crypto, tempstr, sizeof(p_auth_tx->EMV_details.app_crypto)-1);
                        break;
               case 0x27: 
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.crypto_info, tempstr, sizeof(p_auth_tx->EMV_details.crypto_info)-1);
                        break;
               case 0x33:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_capabilities, tempstr, sizeof(p_auth_tx->EMV_details.terminal_capabilities)-1);
                        break;
               case 0x34:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.cvm_results, tempstr, sizeof(p_auth_tx->EMV_details.cvm_results)-1);
                        break;
               case 0x35:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.terminal_type, tempstr, sizeof(p_auth_tx->EMV_details.terminal_type)-1);
                        break;
               case 0x36:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.atc, tempstr, sizeof(p_auth_tx->EMV_details.atc)-1);
                        break;
               case 0x37:
                        genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.unpredictable_number, tempstr, sizeof(p_auth_tx->EMV_details.unpredictable_number)-1);
                        break;
               case 0x41:
                        genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                        memcpy(p_auth_tx->EMV_details.tran_sequence_ctr, tempstr, sizeof(p_auth_tx->EMV_details.tran_sequence_ctr)-1);
                        break;
               case 0x53:
                        memcpy(p_auth_tx->EMV_details.tcc, &emv_raw[dataIndex], fieldLength);
                        break;
               default:
                        break;
            }
            dataIndex += fieldLength;
         }
      }
      else
      {
         switch (byteOneOfTag)
         {
            case 0x71:
                     /* Get overall length of this field - including tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength+2;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1, a_string, 2 );

                     /* Get Tag ID */
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+2, "71", 2 ); /* Tag ID */

                     /* Get length of data for this tag. */
                     memset( a_string, 0x00, sizeof(a_string) );
                     memset( b_string, 0x00, sizeof(b_string) );
                     b_string[0] = fieldLength;
                     genutil_bin_to_ascii( b_string, 1, a_string, sizeof(a_string) );
                     memcpy( p_auth_tx->EMV_details.issuer_script_1+4, a_string, 2 );

                     /* Finally put in the data. */
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_script_1+6, tempstr, sizeof(p_auth_tx->EMV_details.issuer_script_1)-7);

                     break;
            case 0x82:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.interchange_profile, tempstr, sizeof(p_auth_tx->EMV_details.interchange_profile)-1);
                     break;
            case 0x84:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.app_id, tempstr, sizeof(p_auth_tx->EMV_details.app_id)-1);
                     break;
            case 0x91:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.issuer_auth_data+1, tempstr, sizeof(p_auth_tx->EMV_details.issuer_auth_data)-2);
                     p_auth_tx->EMV_details.issuer_auth_data[0] = fieldLength;
                     break;
            case 0x95:
                     genutil_bin_to_ascii (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tvr, tempstr, sizeof(p_auth_tx->EMV_details.tvr)-1);
                     break;
            case 0x9A:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_date, tempstr, sizeof(p_auth_tx->EMV_details.tran_date)-1);
                     break;
            case 0x9C:
                     genutil_bcd_to_asc (&emv_raw[dataIndex], fieldLength , &tempstr[0], sizeof(tempstr));
                     memcpy(p_auth_tx->EMV_details.tran_type, tempstr, sizeof(p_auth_tx->EMV_details.tran_type)-1);
                     break;
            default:
                     break;
         }
         dataIndex += fieldLength;
      }
   }
   return;
}

/******************************************************************************
 *
 *  NAME:         generate_VISA_EMV_data
 *
 *  DESCRIPTION:  This function steps through the EMV subfield tags.  If one
 *                that is supported by VISA is found, it is copied into the
 *                outgoing Host Message buffer.  The incoming data stream is
 *                not changed.
 *
 *  INPUTS:       emv_len_in  - Length of emv_data_in
 *                emv_data_in - EMV data that comes into ATP
 *
 *  OUTPUTS:      HOST_MESSAGE.ICCdata - EMV data that goes to MC
 *                HOST_MESSAGE.ICCdata_len - Length of EMV data to MC
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Phani
 *
 ******************************************************************************/
void generate_VISA_EMV_data( INT emv_len_in, pBYTE emv_data_in )
{
   INT   tag_len;
   BYTE  emv_out_len[3] = "",temp_len[2]="";
   int	tga34_len=-1;

	  
   if(emv_len_in > 255)
   	emv_len_in = 255;
   HOST_MESSAGE.ICCdata[0]=0x01;
   HOST_MESSAGE.ICCdata[1]=0x00;
 //  HOST_MESSAGE.ICCdata[2]=0x01;
 	sprintf( emv_out_len, "%01x", emv_len_in );
   genutil_asc_to_bin((BYTE *)emv_out_len,(BYTE *)temp_len,&tag_len);
   memcpy(&HOST_MESSAGE.ICCdata[2],temp_len,1);
   // One Byte Data Set ID followed by length data set IDs
   memcpy(&HOST_MESSAGE.ICCdata[3],emv_data_in+2,emv_len_in);
   sprintf( emv_out_len, "%01x", emv_len_in+3 );
   genutil_asc_to_bin((BYTE *)emv_out_len,(BYTE *)temp_len,&tag_len);
   memcpy(HOST_MESSAGE.ICCdata_len,temp_len,1);
   return;
}

/*****************************************************************************

  Function:    move_iso_response_to_auth_struct

  Description: This function will convert ISO8583 Fields from their binary fields
               to their ascii fields... 
  Author:
      unknown
  Inputs:
      nmi_code  - 
      p_auth_tx - a pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true  - no error occurred
      false - an error occurred
  Modified by:
******************************************************************************/
INT move_iso_response_to_auth_struct(CHAR nmi_code[5], pAUTH_TX p_auth_tx)
{
   CHAR  tempstr [512]={0};
   pCHAR tempptr;    
   CHAR  templen [6];	 
   INT   i;
   INT   actual_int;
   INT   total_int;
   INT   reversal_int;
   CHAR  temp1[15];
   CHAR  temp2[15];
   CHAR  error_msg[512]={0};
   CHAR  temp[38]; 
   CHAR  Buffer[512]={0};
   Addl_Amts  acct_bal[4];
   INT        bal_set;
   INT        len;
   INT        addl_amts_size;
   CHAR  date_str [ 9];
   CHAR  time_date[25];
   
   memset(&temp1, 0x00, sizeof(temp1));
   memset(&temp2, 0x00, sizeof(temp2));
   memcpy(temp1, p_auth_tx->incoming_src_station_id,
          sizeof( p_auth_tx->incoming_src_station_id ));
   memcpy(temp2, p_auth_tx->incoming_dest_station_id,
          sizeof( p_auth_tx->incoming_dest_station_id));

   memset(p_auth_tx, 0x00, Auth_Tx_Size);

   memcpy(p_auth_tx->incoming_src_station_id,  temp1,
          sizeof( p_auth_tx->incoming_src_station_id ));
   memcpy(p_auth_tx->incoming_dest_station_id, temp2,
          sizeof( p_auth_tx->incoming_dest_station_id));

   memset(vLogon_NMI_Code, 0x00, 4);

   /* VISA HEADER */

   /* Store in Merchant Contact field for switched in txns.
    * If txn is switched out, this field will not get copied
    * to the auth_tx from the timer, so it has no effect.
    */
   memcpy( p_auth_tx->MCF01_details.contact, HOST_MESSAGE.header, 23 );

   /* MESSAGE TYPE */
   bcd_to_asc (HOST_MESSAGE.msgtype_bcd, sizeof(HOST_MESSAGE.msgtype_bcd), 
               p_auth_tx->TLF01_details.message_type,
               sizeof(p_auth_tx->TLF01_details.message_type));

   /* FIELD 2: PAN */
   memset(&tempstr, 0x00, sizeof(tempstr));
   memcpy(p_auth_tx->TLF01_details.card_num_len,HOST_MESSAGE.pan_len,bf2_len);
   bcd_to_asc( HOST_MESSAGE.pan_bin, (HOST_MESSAGE.pan_len[0]+1)/2, tempstr,
               sizeof(p_auth_tx->TLF01_details.card_num) );
   if ( 1 == (HOST_MESSAGE.pan_len[0] % 2) )
   {
      itoa((HOST_MESSAGE.pan_len[0]),p_auth_tx->TLF01_details.card_num_len,10);
      memcpy( p_auth_tx->TLF01_details.card_num, tempstr+1,
              sizeof(p_auth_tx->TLF01_details.card_num) );
   }
   else
   {
      itoa(HOST_MESSAGE.pan_len[0],p_auth_tx->TLF01_details.card_num_len,10);
      memcpy( p_auth_tx->TLF01_details.card_num, tempstr,
              sizeof(p_auth_tx->TLF01_details.card_num) );
   }

   /* FIELD 3: PROCESSING CODE */
   bcd_to_asc (HOST_MESSAGE.proccode,   sizeof(HOST_MESSAGE.proccode),
               p_auth_tx->TLF01_details.processing_code,
               sizeof(p_auth_tx->TLF01_details.processing_code));

   /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
         memcpy(p_auth_tx->TLF01_details.product_codes[17].amount,p_auth_tx->TLF01_details.processing_code,sizeof(p_auth_tx->TLF01_details.processing_code)-1);

   /* FIELD 4: AMOUNT */
   bcd_to_asc (HOST_MESSAGE.amount_bin, sizeof(HOST_MESSAGE.amount_bin),
               p_auth_tx->TLF01_details.total_amount,
               sizeof(p_auth_tx->TLF01_details.total_amount));
   /* FIELD 5: SETTLEMENT AMOUNT */ /* Girija Y - TF */
   bcd_to_asc (HOST_MESSAGE.settle_amt, sizeof(HOST_MESSAGE.settle_amt),
               p_auth_tx->TLF01_details.settlement_total,
               sizeof(p_auth_tx->TLF01_details.settlement_total));

   /* FIELD 6: CARDHOLDER BILLING AMOUNT */
   bcd_to_asc (HOST_MESSAGE.card_bill_amt,   sizeof(HOST_MESSAGE.card_bill_amt),
               p_auth_tx->ch_billing_amt, sizeof(p_auth_tx->ch_billing_amt));

   /* FIELD 7: GREENWICH TIME - MMDDhhmmss */
   bcd_to_asc (HOST_MESSAGE.gmt_time,   sizeof(HOST_MESSAGE.gmt_time),
               &p_auth_tx->TLF01_details.transmission_timestamp,
               sizeof(p_auth_tx->TLF01_details.transmission_timestamp));

   /* FIELD 9: SETTLEMENT CONVERSION RATE */ /* Girija Y - TF */
   bcd_to_asc (HOST_MESSAGE.conv_rate_stl,   sizeof(HOST_MESSAGE.conv_rate_stl),
               p_auth_tx->settlement_conv_rate,
               sizeof(p_auth_tx->settlement_conv_rate));

   /* FIELD 10: CARDHOLDER BILLING CONVERSION RATE */
   bcd_to_asc (HOST_MESSAGE.conv_rate,   sizeof(HOST_MESSAGE.conv_rate),
               p_auth_tx->ch_billing_conv_rate,
               sizeof(p_auth_tx->ch_billing_conv_rate));

   /* FIELD 11: SYSTEMS TRACE AUDIT NUMBER */
   bcd_to_asc (HOST_MESSAGE.stan_bin, sizeof(HOST_MESSAGE.stan_bin),
               p_auth_tx->TLF01_details.sys_trace_audit_num,
               sizeof(p_auth_tx->TLF01_details.sys_trace_audit_num));

   /* FIELD 12: LOCAL TRANSACTION TIME */
   if ( HOST_MESSAGE.time_bin[0] != 0x00 )
   {
      memset( tempstr, 0x00, sizeof(tempstr) );
		bcd_to_asc( HOST_MESSAGE.time_bin, sizeof(HOST_MESSAGE.time_bin),
                  tempstr, sizeof(HOST_MESSAGE.time_bin)*2);
      memcpy( p_auth_tx->TLF01_details.time_hhmmss, tempstr, 6);
   }

   /* FIELD 13: LOCAL TRANSACTION DATE */
   if ( HOST_MESSAGE.date_bin[0] != 0x00 )
   {
      /* Get local time in order to get the year. */
      memset( date_str,  0x00, sizeof(date_str)  );
      memset( time_date, 0x00, sizeof(time_date) );
      memset( tempstr, 0x00, sizeof(tempstr) );
      ptetime_get_timestamp( time_date );
      get_date( time_date, date_str );

      memset( p_auth_tx->TLF01_details.date_yyyymmdd, '0', 4 );
		
		bcd_to_asc (HOST_MESSAGE.date_bin, sizeof(HOST_MESSAGE.date_bin),
               tempstr,
               sizeof(HOST_MESSAGE.date_bin)*2);

      memcpy( p_auth_tx->TLF01_details.date_yyyymmdd+4,
              tempstr, 4 );
      memcpy( p_auth_tx->TLF01_details.date_yyyymmdd, date_str, 4 ); /* Year */
   }


   /* FIELD 14: DATE EXPIRED */
   bcd_to_asc (HOST_MESSAGE.date_expd, sizeof(HOST_MESSAGE.date_expd),
               p_auth_tx->TLF01_details.exp_date,
               sizeof(p_auth_tx->TLF01_details.exp_date));

   /* FIELD 15: SETTLEMENT DATE (MMDD) */
   bcd_to_asc (HOST_MESSAGE.date_settle, sizeof(HOST_MESSAGE.date_settle),
               p_auth_tx->TLF01_details.settlement_date,
               sizeof(p_auth_tx->TLF01_details.settlement_date));

   /* FIELD 16: CONVERSION DATE (MMDD) */  /* Girija Y - TF */
   bcd_to_asc (HOST_MESSAGE.conv_date, sizeof(HOST_MESSAGE.conv_date),
               p_auth_tx->conv_date,
               sizeof(p_auth_tx->conv_date));

   /* FIELD 18: SIC NUMBER (MERCHANT TYPE) */
   bcd_to_asc (HOST_MESSAGE.sic_number, sizeof(HOST_MESSAGE.sic_number),
               p_auth_tx->TLF01_details.category_code,
               sizeof(p_auth_tx->TLF01_details.category_code));

   /* FIELD 19: COUNTRY CODE */
   memset( &tempstr, 0x00, sizeof(tempstr) );
   bcd_to_asc( HOST_MESSAGE.ctry_code,
               sizeof(HOST_MESSAGE.ctry_code),
               &tempstr[0],  sizeof(tempstr));
   memcpy(p_auth_tx->country_code, tempstr+1,sizeof(p_auth_tx->country_code)-1);

   /* Also store here for logging to TLF01. */
   memcpy( p_auth_tx->TLF01_details.num_dbsales, p_auth_tx->country_code, 3 );

   /* FIELD 20: PRIMARY ACCOUNT NUMBER EXTENDED COUNTRY CODE */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.pan_ctry_cd, sizeof(HOST_MESSAGE.pan_ctry_cd),
               &tempstr[0],  sizeof(tempstr));
   memcpy(p_auth_tx->pan_ext_ctry_code, tempstr+1,
          sizeof(p_auth_tx->pan_ext_ctry_code)-1);


      /* FIELD 21: FORWARD  INST COUNTRY CODE */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.fwd_inst_ctry_cd, sizeof(HOST_MESSAGE.fwd_inst_ctry_cd),
               &tempstr[0],  sizeof(tempstr));
/*   memcpy(p_auth_tx->pan_ext_ctry_code, tempstr+1,
          sizeof(p_auth_tx->pan_ext_ctry_code)-1);*/



   /* FIELD 22: POS ENTRY MODE  */
   bcd_to_asc (HOST_MESSAGE.posentry, sizeof(HOST_MESSAGE.posentry),
               p_auth_tx->TLF01_details.pos_entry_mode,
               sizeof(p_auth_tx->TLF01_details.pos_entry_mode));

   /*+-----------------------------------------------------------------+
     | Need to convert field 22 to the Hypercom Standard.              |
     |                                                                 |
     |   Visa is close to the standard, except Visa swipe and filler.  |
     |       Visa swipe = 90   convert to   Standard = 02              |
     |                                                                 |
     |   For standard, filler should be at the beginning:              |
     |       Visa = xyz0    convert to    Standard = 0xyz              |
     +-----------------------------------------------------------------+*/
   if ( 0==strncmp("90", p_auth_tx->TLF01_details.pos_entry_mode, 2) )
      memcpy( p_auth_tx->TLF01_details.pos_entry_mode, "02", 2 );

   for( i=2; i>=0; i-- )
      p_auth_tx->TLF01_details.pos_entry_mode[i+1] =
          p_auth_tx->TLF01_details.pos_entry_mode[i];
   p_auth_tx->TLF01_details.pos_entry_mode[0] = '0';
   
   memcpy( p_auth_tx->TLF01_details.product_codes[17].code,
  		   p_auth_tx->TLF01_details.pos_entry_mode,sizeof(p_auth_tx->TLF01_details.pos_entry_mode)-1);

    /* FIELD 23 : CARD SEQ NO HOST_MESSAGE.cardseqnum*/
	if ( HOST_MESSAGE.cardseqnum[0] != 0x00 ||
		 HOST_MESSAGE.cardseqnum[1] != 0x00 
		) 	
	{	bcd_to_asc (HOST_MESSAGE.cardseqnum, sizeof(HOST_MESSAGE.cardseqnum),
	               p_auth_tx->host2_message_seq_nbr,
	               sizeof(p_auth_tx->host2_message_seq_nbr));
	}

   /* FIELD 24: NII (Round Trip Indicator, RTI, from Visa's header */

   /* First byte is the RTI.
    * Second byte, if 'E' indicates to echo this in response.
    */
   memcpy( p_auth_tx->TLF01_details.nii, HOST_MESSAGE.nii, 2 );

   /* FIELD 25: POS CONDITION CODE */
   bcd_to_asc (HOST_MESSAGE.pos_condcd, sizeof(HOST_MESSAGE.pos_condcd),
               p_auth_tx->TLF01_details.pos_condition_code,
               sizeof(p_auth_tx->TLF01_details.pos_condition_code));

   /* FIELD 28: TRANSACTION FEE AMOUNT */    
   if ( HOST_MESSAGE.tran_fee[0] != 0x00 )
   {
      p_auth_tx->TLF01_details.cvc_data[0] = HOST_MESSAGE.tran_fee[0];
      memcpy( p_auth_tx->TLF01_details.tran_fee_amount,
             &HOST_MESSAGE.tran_fee[1], bf28_len-1 );
   }


   /* FIELD 32: ACQUIRING INSTITUTION ID CODE  */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.acq_inst_id, (HOST_MESSAGE.acq_inst_len[0]+1)/2,
				  &tempstr[0], sizeof(tempstr));
   if (1 == HOST_MESSAGE.acq_inst_len[0] % 2)
   {
      memcpy(p_auth_tx->acq_inst_id_cd, tempstr+1,
             sizeof(p_auth_tx->acq_inst_id_cd)-1);
   }
   else
   {
      memcpy(p_auth_tx->acq_inst_id_cd, tempstr,
             sizeof(p_auth_tx->acq_inst_id_cd)-1);
   }

   /* 
    This statement is required : because to insert into TLF01, we need to copy
	acquirer institution code into TLF01 structure variable.
	*/
   strcpy(p_auth_tx->TLF01_details.acquiring_id, p_auth_tx->acq_inst_id_cd);

   p_auth_tx->acq_inst_id_len[0] =  HOST_MESSAGE.acq_inst_len[0];

   
   /* FIELD 33: FORWARDING INSTITUTION ID CODE  */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.fwd_inst_id, (HOST_MESSAGE.fwd_inst_len[0]+1)/2,
				  &tempstr[0], sizeof(tempstr));
   if (1 == HOST_MESSAGE.fwd_inst_len[0] % 2)
   {
      memcpy(p_auth_tx->forwarding_inst_id_cd, tempstr+1,
             sizeof(p_auth_tx->forwarding_inst_id_cd)-1);
   }
   else
   {
      memcpy(p_auth_tx->forwarding_inst_id_cd, tempstr,
             sizeof(p_auth_tx->forwarding_inst_id_cd)-1);
   }
   p_auth_tx->forwarding_inst_id_len[0] =  HOST_MESSAGE.fwd_inst_len[0];

   /* Store here for TLF01. */
   strcpy( p_auth_tx->TLF01_details.forwarding_institution_id,
           p_auth_tx->forwarding_inst_id_cd );

   /* FIELD 34: PRIMARY ACCOUNT NUMBER EXTENDED */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.pan_ext, (HOST_MESSAGE.pan_ext_len[0]+1)/2,
   		     &tempstr[0], sizeof(tempstr));
   if (1 == (HOST_MESSAGE.pan_ext_len[0] % 2) )
   {
      memcpy(p_auth_tx->pan_extended, tempstr+1,
             sizeof(p_auth_tx->pan_extended)-2);
   }
   else
   {
      memcpy(p_auth_tx->pan_extended, tempstr,
             sizeof(p_auth_tx->pan_extended)-1);
   }

   /* FIELD 35: TRACK 2 */
   if ( HOST_MESSAGE.track2_len[0] != 0x00 )
   {
      bcd_to_asc (HOST_MESSAGE.track2, (HOST_MESSAGE.track2_len[0]+1)/2,temp,
                  sizeof(p_auth_tx->TLF01_details.track2));

      if(temp[0]== '0')
      {
         strcpy(p_auth_tx->TLF01_details.track2,&temp[1]);
      }
      else
      {
         strcpy(p_auth_tx->TLF01_details.track2,temp);
      }

      memset(p_auth_tx->TLF01_details.exp_date, 0 ,
             sizeof(p_auth_tx->TLF01_details.exp_date));

      memset(p_auth_tx->TLF01_details.card_num, 0 ,
             sizeof(p_auth_tx->TLF01_details.card_num));

      if ( false == parse_track2( p_auth_tx, error_msg ) )
      {
         memcpy( p_auth_tx->TLF01_details.terminal_id, HOST_MESSAGE.termid,
                 sizeof(HOST_MESSAGE.termid));
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,
         					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(error_msg,Log_Bufr);
         ncvsms_log_message( 3, 2, error_msg , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         return(false);
      }
   }

   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
   memcpy( p_auth_tx->TLF01_details.retrieval_ref_num, HOST_MESSAGE.rrn,
           sizeof(HOST_MESSAGE.rrn));

   /* FIELD 38: AUTH NUMBER */
   memcpy( p_auth_tx->TLF01_details.auth_number, HOST_MESSAGE.authcode,
           sizeof(HOST_MESSAGE.authcode));

   /* FIELD 39: RESPONSE CODE */
   memcpy( p_auth_tx->TLF01_details.response_code, HOST_MESSAGE.responsecd,
           sizeof(HOST_MESSAGE.responsecd));

   /* FIELD 41: TERMINAL ID */
   memcpy( p_auth_tx->TLF01_details.terminal_id, HOST_MESSAGE.termid,
           sizeof(HOST_MESSAGE.termid));

   /* Do this in case terminal Id is not 8 characters.
    * For example, DBS has 7 character terminal Ids.
    */
   trim_spaces( p_auth_tx->TLF01_details.terminal_id );

   /* FIELD 42: CARD ACCEPTOR ID CODE */
   memcpy( p_auth_tx->TLF01_details.merchant_id, HOST_MESSAGE.cardaccid,
           sizeof(HOST_MESSAGE.cardaccid));

   /* FIELD 43: CARD ACCEPTOR NAME/LOCATION */
   memcpy( p_auth_tx->acceptor_term_name, HOST_MESSAGE.cardaccname,    25);
   memcpy( p_auth_tx->acceptor_city_name, HOST_MESSAGE.cardaccname+25, 13);
   memcpy( p_auth_tx->acceptor_country,   HOST_MESSAGE.cardaccname+38, 2);

   /* Store DE43 here so it gets logged to TLF01:
    *   Merchant Name         --> TLF01.card_holder_name
    *   City and Country Code --> TLF01.ncf01_retired_cwk
    * SCR 775
    */
   memcpy( p_auth_tx->TLF01_details.card_holder_name,
           HOST_MESSAGE.cardaccname,
           25 );
   memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
           HOST_MESSAGE.cardaccname+25,
           15 );

   /* FIELD 44: ADDITIONAL RESPONSE DATA */
   if (HOST_MESSAGE.addl_rsp_len[0] > 25)
      HOST_MESSAGE.addl_rsp_len[0] = 25;

   memcpy (&p_auth_tx->TLF01_details.resp_source, &HOST_MESSAGE.addlrspdata,
           HOST_MESSAGE.addl_rsp_len[0]);

   /* FIELD 45: TRACK1 */
   if ( HOST_MESSAGE.track1_len[0] != 0x00 )
   {
      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( p_auth_tx->TLF01_details.track1, &HOST_MESSAGE.track1,
              HOST_MESSAGE.track1_len[0]);
      strcpy( tempstr, p_auth_tx->TLF01_details.track1 );

      /* Get card number */
      if (0x00 != (tempptr = strchr(tempstr, '^'))) 
      {
         /* Get length of card number.  Need to account for the first character,
          * a Format Code (B), which is not part of the card number.
          */
         i = tempptr-tempstr-1;
         memcpy(p_auth_tx->TLF01_details.card_num, tempstr+1, i );
         sprintf( p_auth_tx->TLF01_details.card_num_len, "%02d", i);
      }

      /* Get exp date */
      /* Search for ^ from the END of track1 because track1 contains two ^s. */
      if ((tempptr = strrchr(tempstr, '^')) != 0x00) 
      {
         memcpy( p_auth_tx->TLF01_details.exp_date, tempptr+1,
                 sizeof(p_auth_tx->TLF01_details.exp_date)-1);

         /* Get the service code - used for Visa Electron. */
         memset( ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
         memcpy( ServiceCode, tempptr+5, 3 );
      }
   }

   /* FIELD 48 ADDITONAL DATA USAGE 6a or b */
   if ((p_auth_tx->TLF01_details.tx_key == AUTH_FUND_TRANSFER_TOTAL) &&
       ( HOST_MESSAGE.addl_data_len[0] != 0x00 ) )
   {
      memcpy( p_auth_tx->TLF01_details.voice_auth_text,
              HOST_MESSAGE.addl_data,
              HOST_MESSAGE.addl_data_len[0] );
   }
   else if ( HOST_MESSAGE.addl_data_len[0] != 0x00 )
   {
      /* FIELD 48 ADDITONAL DATA USAGE 7a field Chargebacks & Representments */
      if (HOST_MESSAGE.addl_data[0] == 'V')
      {
		  if( HOST_MESSAGE.addl_data_len[0] < 240 )
		  {
				/* Taking the chargeback reference  and message text */
				memcpy( p_auth_tx->TLF01_details.voice_auth_text,
						HOST_MESSAGE.addl_data, HOST_MESSAGE.addl_data_len[0] );
		  }
		  else
		  {
			  sprintf(Buffer,"Length of Field 48 is more than 240 Bytes which is not supported");
			  memset(Log_Bufr,0x00,sizeof(Log_Bufr));
			  sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
			  					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
			  strcat(Buffer,Log_Bufr);
			  ncvsms_log_message( 3, 2, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
			  return(false);
		  }

      }

      /* FIELD 48 ADDITONAL DATA USAGE 9a field Text Messages */ 
      else if (HOST_MESSAGE.addl_data[0] == '#')
      {
         memcpy( p_auth_tx->TLF01_details.voice_auth_text,
                 HOST_MESSAGE.addl_data + 1,
                 HOST_MESSAGE.addl_data_len[0] - 1 );
      }

      /* FIELD 48 ADDITONAL DATA USAGE 10 field Time Stamp (Plus only) */ 
      else if (HOST_MESSAGE.addl_data[0] == 'P')
      {
         memcpy( p_auth_tx->TLF01_details.voice_auth_text,
                 HOST_MESSAGE.addl_data + 1,
                 HOST_MESSAGE.addl_data_len[0] - 1 );
      }

      /* FIELD 48 ADDITONAL DATA  Usage 14: Dynamic Key Exchange Check Value*/
      else if (HOST_MESSAGE.addl_data[0] == '&')
      {
         memcpy( p_auth_tx->TLF01_details.voice_auth_text,
                 HOST_MESSAGE.addl_data + 1,
                 HOST_MESSAGE.addl_data_len[0] - 1 );
      }

      /* FIELD 48 ADDITONAL DATA ANY USAGE field  */ 
      else
      {
         memcpy( p_auth_tx->TLF01_details.voice_auth_text,
                 HOST_MESSAGE.addl_data,
                 HOST_MESSAGE.addl_data_len[0] );
      }
   }

   /* FIELD 49 TRANSACTION CURRENCY CODE */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.tran_curr_cd,   sizeof(HOST_MESSAGE.tran_curr_cd),
               tempstr, sizeof(p_auth_tx->TLF01_details.currency_code));   
   memcpy( p_auth_tx->TLF01_details.currency_code, tempstr+1,
           sizeof(p_auth_tx->TLF01_details.currency_code)-1 );

   /* FIELD 50 currency code  settlement*/
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.settle_cur_cd,   sizeof(HOST_MESSAGE.settle_cur_cd),
               tempstr, sizeof(tempstr));   
   memcpy( p_auth_tx->TLF01_details.product_codes[5].code, tempstr,
		   CURRENCY_CODE_LEN);

   /* FIELD 51 CARDHOLDER BILLING CURRENCY CODE */
   memset(&tempstr, 0x00, sizeof(tempstr));
   bcd_to_asc (HOST_MESSAGE.card_curr_cd,   sizeof(HOST_MESSAGE.card_curr_cd),
               tempstr, sizeof(p_auth_tx->ch_billing_curr_code));
   memcpy( p_auth_tx->ch_billing_curr_code, tempstr+1,
           sizeof(p_auth_tx->ch_billing_curr_code)-1 );


   /* FIELD 52 PIN DATA */
   if (false == blank_or_null_string(HOST_MESSAGE.pin_block,
                                     sizeof(HOST_MESSAGE.pin_block)))
   {
      memset(tempstr, 0, sizeof(tempstr));
      genutil_bin_to_ascii (HOST_MESSAGE.pin_block,
                            sizeof(HOST_MESSAGE.pin_block), 
                            &tempstr[0], sizeof(tempstr));   
      memcpy (p_auth_tx->TLF01_details.pin_block, tempstr,
              sizeof(p_auth_tx->TLF01_details.pin_block)-1);
   }

   /* FIELD 54 ADDITIONAL AMOUNTS *
    * ---------------------------
    * This field contains current and/or available balance in one or
    * two currencies.  The two currencies are the issuer's currency
    * and the acquirer's currency.  There can be up to 4 sets of balances
    * here:  current and available for Issuer and Acquirer.  Acquirer
    * currency, which is what we want, is always the 3rd or the 3rd and
    * 4th set.
    *
    * If there is 1 set, it is in the Issuer's currency.  In this case,
    * just copy it into the auth_tx values, cause it is also our currency.
    *
    * If there are 2 sets, the Issuer has sent current and available in
    * the Issuer's currency.  Again, just copy these into auth_tx, because
    * if the Issuer sends only one currency, Issuer and Acquirer currency
    * must be the same.
    *
    * If there are 3 sets, copy the 3rd set (Acquirer) into auth_tx.
    *
    * If there are 4 sets, copy the 3rd and 4th (both Acquirer currencies)
    * into auth_tx.  Need to distinguish between current and available
    * balance.
    *
    * Current Balance:   For credit accounts, this is the current account
    *                    ledger balance and refers to the amount of credit
    *                    remaining to the customer.
    *
    * Available Balance: This is typically ledger balance less outstanding
    *                    authorizations.
    */

   /* Only get this data for responses. */
   len = HOST_MESSAGE.addl_len[0];
   if ( (len > 0) && (p_auth_tx->TLF01_details.message_type[2] == '1') )
   {
      memset( tempstr, 0x00, sizeof(tempstr) );
      memcpy( tempstr, HOST_MESSAGE.addlamts, len );

      addl_amts_size = sizeof(Addl_Amts);
      bal_set = 0;

      /* Copy balance sets into structure that temporarily holds balances. */
      i = 0;
      while( i < len )
      {
         /* Initialize the structure that will hold all four sets. */
         memset( &acct_bal[bal_set], 0x00, addl_amts_size );

         memcpy( acct_bal[bal_set].acct_type, tempstr+i,   2 );
         memcpy( acct_bal[bal_set].amt_type,  tempstr+i+2, 2 );
         memcpy( acct_bal[bal_set].cur_code,  tempstr+i+4, 3 );
         acct_bal[bal_set].sign = tempstr[i+7];
         memcpy( acct_bal[bal_set].amount,    tempstr+i+8, 12);

         bal_set++;
         i += 20;   /* Increase ptr in tempstr by length of 1 set. */
      }

      /* Find the correct balance sets and copy them into auth_tx. */
      switch( bal_set )
      {
         case 1:  (void)get_balances( &acct_bal[0], p_auth_tx );
                  break;

         case 2:  (void)get_balances( &acct_bal[0], p_auth_tx );
                  (void)get_balances( &acct_bal[1], p_auth_tx );
                  break;

         case 3:  (void)get_balances( &acct_bal[2], p_auth_tx );
                  break;

         case 4:  (void)get_balances( &acct_bal[2], p_auth_tx );
                  (void)get_balances( &acct_bal[3], p_auth_tx );
                  break;
      }
   }
   else
   {
      memset(&tempstr, 0x00, sizeof(tempstr));
      memset(&templen, 0x00, sizeof(templen));
      memcpy(templen,HOST_MESSAGE.addl_len,bf54_len);
      memcpy(tempstr,HOST_MESSAGE.addlamts,sizeof(HOST_MESSAGE.addlamts));
      i = 0;
      /* the loop is controlled by the ifs at the end of the loop */
      while (i < 21 && templen[0] > 19)
      {
         if (0 == strncmp(tempstr+i+2,"01",2))   /* indicates current balance */
         {
            memcpy(p_auth_tx->ACF01_details.cash_available, tempstr+i+11, 9);
            if ('D' == tempstr[i+7])
            {
               p_auth_tx->current_balance_sign = '-';
            }
            else if ('C' == tempstr[i+7])
            {
               p_auth_tx->current_balance_sign = '+';
            }
            strncpy(p_auth_tx->TLF01_details.currency_code, tempstr+i+4, 3);    
         }
         else if (0 == strncmp(tempstr+i+2,"02",2)) /*indicates available balance*/
         {
            memcpy(p_auth_tx->ACF01_details.cash_limit, tempstr+i+11,9);
            if ('D' == tempstr[i+7])
            {
               p_auth_tx->available_balance_sign = '-';
            }
            else if ('C' == tempstr[i+7])
            {
               p_auth_tx->available_balance_sign = '+';
            }
            strncpy(p_auth_tx->TLF01_details.currency_code, tempstr+i+4, 3);    
         }
         if (templen[0] > 20)
            i = i + 20;  /* This will allow us to get the second set of data */
         else
            i = 40;      /* This will cause the loop to end */
      }
   }

	/*	FIELD 55 EMV/CHIP/ data */
		if ( HOST_MESSAGE.ICCdata_len[0] > 0 )
		{	INT emv_temp_len=0;	 
		   BYTE  ICCdata [256] = {0};
		  emv_temp_len = HOST_MESSAGE.ICCdata_len[0];
		  memcpy(p_auth_tx->EMV_details.emv_block,HOST_MESSAGE.ICCdata_len,1);
		  if ( emv_temp_len > 0) 
		  { 	memset(ICCdata, 0, sizeof(ICCdata)); 
				memcpy(ICCdata, HOST_MESSAGE.ICCdata, emv_temp_len);	
				memcpy( p_auth_tx->EMV_details.emv_block+1,HOST_MESSAGE.ICCdata,emv_temp_len);
				genutil_VISA_EMV_To_Auth_Tx( ICCdata, emv_temp_len, p_auth_tx);   
		  }
		}

    /* FIELD 59 Natl POS GEO DATA  */
   if ( HOST_MESSAGE.posdata_len[0] > 0 )
   {
      memset(&tempstr, 0x00, sizeof(tempstr));
      memset(&templen, 0x00, sizeof(templen));
      bcd_to_asc( HOST_MESSAGE.posdata_len, sizeof(HOST_MESSAGE.posdata_len),
                 &templen[0], sizeof(templen));

      bcd_to_asc( HOST_MESSAGE.posdata, (atoi(templen)),
                  (&tempstr[0]), sizeof(tempstr));
  /* what do we do with this incoming data */


   }


   /* FIELD 60 RESERVED */
   if ( HOST_MESSAGE.priv60_len[0] > 0 )
   {
      memset(&tempstr, 0x00, sizeof(tempstr));
      memset(&templen, 0x00, sizeof(templen));
      bcd_to_asc( HOST_MESSAGE.priv60_len, sizeof(HOST_MESSAGE.priv60_len),
                 &templen[0], sizeof(templen));

      bcd_to_asc( HOST_MESSAGE.priv60, (atoi(templen)),
                  (&tempstr[0]), sizeof(tempstr));

      memcpy( p_auth_tx->additional_pos_information, tempstr,
              sizeof(p_auth_tx->additional_pos_information)-1);

      /* Copy into here so this information is logged into TLF01 (SCR 743). */
      memcpy( p_auth_tx->TLF01_details.nfi_name, tempstr,
              sizeof(p_auth_tx->TLF01_details.nfi_name)-1 );

      strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,
    		  tempstr,sizeof(p_auth_tx->TLF01_details.product_codes[16].amount)-1);
   }

   /* FIELD 61: OTHER AMOUNTS */
   memset(&tempstr, 0x00, sizeof(tempstr));
   memset(&templen, 0x00, sizeof(templen));
	bcd_to_asc (HOST_MESSAGE.priv61_len, sizeof(HOST_MESSAGE.priv61_len),
				  &templen[0], sizeof(templen));
   bcd_to_asc (HOST_MESSAGE.priv61, (atoi(templen)),
				  (&tempstr[0]), sizeof(tempstr));
   memcpy(p_auth_tx->TLF01_details.add_amounts, tempstr,
          sizeof(p_auth_tx->TLF01_details.add_amounts)-1);

   if ( 0 == strcmp(p_auth_tx->TLF01_details.message_type, "0102") )
   {
      /* This is an ATM Confirmation message.
       * DE61, subfield 1 (positions 1-12) is the dispensed amount.
       */
      memcpy( p_auth_tx->TLF01_details.actual_amount, tempstr, 12 );
   }

   /* FIELD 62 CUSTOM PAYMENT SERVICE FIELDS - BITMAP */
   /* Store this for echoing. */
   memcpy(p_auth_tx->int_net_facility_data, HOST_MESSAGE.priv62, 
          sizeof(HOST_MESSAGE.priv62 ));

   memcpy(p_auth_tx->int_net_facility_len, HOST_MESSAGE.priv62_len, 
          sizeof(HOST_MESSAGE.priv62_len ));
   if (HOST_MESSAGE.priv62_len[0] > 0)  
   {
      Field62Response( p_auth_tx );
      if ( false == isalpha(p_auth_tx->TLF01_details.visa_char_indicator) )
         p_auth_tx->TLF01_details.visa_char_indicator = ' ';

      /* Store 62.1 and 62.2 into the following fields
       * so they get stored into NCF30 for reversals.
       * This code was added late.  Since there are no fields in NCF30 large
       * enough to hold DE62, use a combination of network data and invoice
       * number.  Network data will hold 62.1 (1 byte) and the first 11 bytes
       * of 62.2 (15 bytes).  The last 4 bytes of 62.2 go into invoice number.
       */
      p_auth_tx->TLF01_details.mcard_banknet[0] =
                                   p_auth_tx->TLF01_details.visa_char_indicator;

      memcpy( p_auth_tx->TLF01_details.mcard_banknet+1,
              p_auth_tx->TLF01_details.visa_tran_id,
              11);

      memcpy( p_auth_tx->TLF01_details.invoice_number,
              p_auth_tx->TLF01_details.visa_tran_id+11,
              4 );
		memcpy( p_auth_tx->TLF01_details.product_codes[1].code,p_auth_tx->TLF01_details.invoice_number,6 );
	  //strcpy ( ncf30.invoice_nbr,auth_tx->TLF01_details.product_codes[1].code);
	  
   }
   else
      p_auth_tx->TLF01_details.visa_char_indicator = ' ';


   /* FIELD 63: SMS Private-Use Fields FIELDS */
   if (HOST_MESSAGE.priv63_len[0] > 0)  
   {
      Field63Response( p_auth_tx );

      /* Need to store DE63.1 in case txn gets reversed.
       * Only place in NCF30 to store it is in invoice_number
       * along with part of the visa_txn_id.
       */
      strcat( p_auth_tx->TLF01_details.invoice_number,
              p_auth_tx->MCF01_details.vip_code );
   }
	
   /* Field 68 Receiving Institution Country Code */
   bcd_to_asc (HOST_MESSAGE.rcv_Inst_cnty_cd,   2,
               p_auth_tx->TLF01_details.product_codes[8].code, 4);



    /* Field 69 Settlement Institution Country Code */
	   memset( tempstr, 0x00, sizeof(tempstr) );
	   bcd_to_asc (HOST_MESSAGE.set_Inst_cnty_cd,   sizeof(HOST_MESSAGE.set_Inst_cnty_cd),
				   tempstr, sizeof(HOST_MESSAGE.set_Inst_cnty_cd));
	   memcpy (p_auth_tx->TLF01_details.product_codes[6].code, tempstr, sizeof(HOST_MESSAGE.set_Inst_cnty_cd));

   /* FIELD 70: NETWORK MANAGEMENT INFORMATION CODE */
   bcd_to_asc (HOST_MESSAGE.nmi_code,   sizeof(HOST_MESSAGE.nmi_code),
               nmi_code, sizeof(nmi_code));
   memcpy (p_auth_tx->function_code, nmi_code, sizeof(nmi_code));
	memcpy (p_auth_tx->TLF01_details.product_codes[9].code,nmi_code, sizeof(nmi_code));

    /*  For Automatic Reconcilliation Advice */
   if ( 0 == strcmp(p_auth_tx->TLF01_details.message_type, "0520") )
   {
	   /* FIELD 74 Credit NUMBER */
	   memset( tempstr, 0x00, sizeof(tempstr) );
		len = sizeof(HOST_MESSAGE.credit_num);
	   bcd_to_asc (HOST_MESSAGE.credit_num,   sizeof(HOST_MESSAGE.credit_num),
				   tempstr, len*2);
		i = atoi(tempstr);
		itoa(i,p_auth_tx->TLF01_details.product_codes[10].amount,10);
	   /* memcpy (p_auth_tx->TLF01_details.product_codes[0].code, tempstr, len*2); */

	   
		/* FIELD 75 Credits, Reversal Number */
	   memset( tempstr, 0x00, sizeof(tempstr) );
		len = sizeof(HOST_MESSAGE.credit_rev_num);
	   bcd_to_asc (HOST_MESSAGE.credit_rev_num,   sizeof(HOST_MESSAGE.credit_rev_num),
				   tempstr, len*2);
		i = atoi(tempstr);
		itoa(i,p_auth_tx->TLF01_details.product_codes[11].amount,10);

	   /*memcpy (p_auth_tx->TLF01_details.product_codes[1].code, tempstr, len*2); */


	   /* FIELD 76 Debits, Number */
	   memset( tempstr, 0x00, sizeof(tempstr) );
		len = sizeof(HOST_MESSAGE.debit_num);
	   bcd_to_asc (HOST_MESSAGE.debit_num,   sizeof(HOST_MESSAGE.debit_num),
				   tempstr, len*2);
		i = atoi(tempstr);
		itoa(i,p_auth_tx->TLF01_details.product_codes[12].amount,10);
		/*memcpy (p_auth_tx->TLF01_details.product_codes[2].code, tempstr, len*2);*/

	   /* FIELD 77 Debits, Reversal Number */
	   memset( tempstr, 0x00, sizeof(tempstr) );
		len = sizeof(HOST_MESSAGE.debit_rev_num);
	   bcd_to_asc (HOST_MESSAGE.debit_rev_num,   sizeof(HOST_MESSAGE.debit_rev_num),
				   tempstr, len*2);
		i = atoi(tempstr);
		itoa(i,p_auth_tx->TLF01_details.product_codes[13].amount,10);
	   /*memcpy (p_auth_tx->TLF01_details.product_codes[3].code, tempstr, len*2);*/


	   /* FIELD 86 Credits, Amount */
	   memset( tempstr, 0x00, sizeof(tempstr) );
		len = sizeof(HOST_MESSAGE.credit_num);
	   bcd_to_asc (HOST_MESSAGE.credit_num,   sizeof(HOST_MESSAGE.credit_amt),
				   tempstr, len*2);
		i = atoi(tempstr);
		itoa(i,p_auth_tx->TLF01_details.product_codes[0].amount,10);
	   /* memcpy (p_auth_tx->TLF01_details.product_codes[0].amount, tempstr, len*2); */

	   /* FIELD 87 Credits, Reversal Amount */
	   memset( tempstr, 0x00, sizeof(tempstr) );
		len = sizeof(HOST_MESSAGE.credit_rev_amt);
	   bcd_to_asc (HOST_MESSAGE.credit_rev_amt,   sizeof(HOST_MESSAGE.credit_rev_amt),
				   tempstr, len*2);
		i = atoi(tempstr);
		itoa(i,p_auth_tx->TLF01_details.product_codes[1].amount,10);
	   /* memcpy (p_auth_tx->TLF01_details.product_codes[1].amount, tempstr, len*2); */

	   /* FIELD 88 Debits, Amount */
	   memset( tempstr, 0x00, sizeof(tempstr) );
		len = sizeof(HOST_MESSAGE.debit_amt);
	   bcd_to_asc (HOST_MESSAGE.debit_amt,   sizeof(HOST_MESSAGE.debit_amt),
				   tempstr, len*2);
		i = atoi(tempstr);
		itoa(i,p_auth_tx->TLF01_details.product_codes[2].amount,10);
	   /* memcpy (p_auth_tx->TLF01_details.product_codes[2].amount, tempstr, len*2); */

	   /* FIELD 89 Debits, Reversal Amount */
	   memset( tempstr, 0x00, sizeof(tempstr) );
		len = sizeof(HOST_MESSAGE.debit_rev_amt);
	   bcd_to_asc (HOST_MESSAGE.debit_rev_amt,   sizeof(HOST_MESSAGE.debit_rev_amt),
				   tempstr, len*2);
		i = atoi(tempstr);
		itoa(i,p_auth_tx->TLF01_details.product_codes[3].amount,10);

	   /* memcpy (p_auth_tx->TLF01_details.product_codes[3].amount, tempstr, len*2); */

   }   


   /* FIELD 90 ORIGINAL DATA ELEMENTS */
   if ( HOST_MESSAGE.orig_data[0] != 0x00 )
   {
      memset( tempstr, 0x00, sizeof(tempstr) );
      bcd_to_asc (HOST_MESSAGE.orig_data,   sizeof(HOST_MESSAGE.orig_data),
                  tempstr, sizeof(HOST_MESSAGE.orig_data)*2);
      if((tempstr[1] == '0') && (p_auth_tx->TLF01_details.message_type[1]=='4'))
      {
         sprintf(Buffer, "Invalid data for original data elements");
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvsms_log_message( 3, 3, Buffer , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         return false;
      }
      else
      {
         memcpy(p_auth_tx->TLF01_details.orig_message, tempstr, 4);
         memcpy(p_auth_tx->TLF01_details.orig_sys_trace_num, tempstr+4, 6);
         memcpy(p_auth_tx->orig_local_time_date, tempstr+10,10);
         memcpy(p_auth_tx->orig_acq_inst_id,tempstr+20,11);

         /* Actually forwarding institution id code */
         memcpy(p_auth_tx->amex_pos_entry_mode,tempstr+31,11);

         /* for AUTH_CARGEBACK and AUTH_CHARGEBACK_REVERSAL,
          * preserver the orig_sys_trace_num coming in
          */
         if(strncmp(p_auth_tx->TLF01_details.message_type,"0422",4) != 0 ) 
         {
            /* Since all values after message type in DE90 can be zero,
             * check the stan.  If zero, will need to use DE11 for reversals
             * to look up original transaction.
             */
            if ( 0 == strcmp(p_auth_tx->TLF01_details.orig_sys_trace_num,"000000"))
            {
               memcpy( p_auth_tx->TLF01_details.orig_sys_trace_num,
                       p_auth_tx->TLF01_details.sys_trace_audit_num, 6 );

               /* Set a flag to indicate we did this so we can echo zeroes. */
               p_auth_tx->host_processing_code[3] = '0';
            }
         }
      }
   }

   /* FIELD 95 REPLACEMENT AMOUNTS */
   if ( p_auth_tx->TLF01_details.actual_amount[0] == 0x00 )
   {
      memcpy( p_auth_tx->TLF01_details.actual_amount, HOST_MESSAGE.repl_amts,
              sizeof(p_auth_tx->TLF01_details.actual_amount)-1);

      /* Set flag here so we know to either echo or not echo this field. */
      if ( HOST_MESSAGE.repl_amts[0] != 0x00 )
      {
         p_auth_tx->MCF01_details.totals_ind[0] = 'E';
         memcpy( p_auth_tx->MCF01_details.address01,
                 HOST_MESSAGE.repl_amts,   30 );
         memcpy( p_auth_tx->MCF01_details.address02,
                 HOST_MESSAGE.repl_amts+30,12 );
      }
   }

   /* FIELD 96: Not Used by Visa (Hdr Fld 9, Msg Status Flags, from Visa hdr. */

   /* First 3 bytes are the Message Status Flags.
    * Fourth byte, if 'E' indicates to echo this in response.
    * Use the MCF01 field because for switched in txns, MCF01 is not used.
    */
   memcpy( p_auth_tx->MCF01_details.class_code, HOST_MESSAGE.msgsec, 4 );

    /*  For Automatic Reconcilliation Advice */
   if ( 0 == strcmp(p_auth_tx->TLF01_details.message_type, "0520") )
   {
	   /* FIELD 97 Amount, Net Settlement */
	   memcpy (p_auth_tx->TLF01_details.product_codes[4].code, &HOST_MESSAGE.net_setl_amount[0], 1);
		memset( tempstr, 0x00, sizeof(tempstr) );
		memcpy (tempstr, &HOST_MESSAGE.net_setl_amount[1], sizeof(HOST_MESSAGE.net_setl_amount)-1);
		i = atoi(tempstr);
		itoa(i,p_auth_tx->TLF01_details.settlement_total,10);

   }

    /*  For Automatic Reconcilliation Advice */
   if ( ( 0 == strcmp(p_auth_tx->TLF01_details.message_type, "0520") ) ||
		( 0 == strcmp(p_auth_tx->TLF01_details.message_type, "0620") ))
   {
      /* FIELD 99: SETTLEMENT INSTITUTION ID CODE */
	  len = HOST_MESSAGE.setl_id_len[0];
     memset( tempstr, 0x00, sizeof(tempstr) );
	  bcd_to_asc (HOST_MESSAGE.setl_id,   len,
				   tempstr, len*2);
     memcpy( p_auth_tx->TLF01_details.product_codes[5].amount, tempstr, len );
      
   }

   /* FIELD 100 Receiving Institution Identification Code */
   if ( HOST_MESSAGE.recv_inst_len[0] > 0 )
   {
   	memset( tempstr, 0x00, sizeof(tempstr) );
		len = HOST_MESSAGE.recv_inst_len[0];
	   bcd_to_asc (HOST_MESSAGE.recv_inst_id,  HOST_MESSAGE.recv_inst_len[0],
				   tempstr, len*2);	   
	   if ( (len % 2) == 1 )
	   {
		memcpy (p_auth_tx->receiving_inst_id_code, &tempstr[1], len);
		memcpy (p_auth_tx->TLF01_details.product_codes[7].amount, &tempstr[1], len);
	   }
	   else
	   {
		memcpy (p_auth_tx->receiving_inst_id_code, tempstr, len);
		memcpy (p_auth_tx->TLF01_details.product_codes[7].amount, tempstr, len);
	   }



   }

   /* FIELD 102 ACCOUNT ID #1 */
   if ( HOST_MESSAGE.acct_id_1[0] != 0x00 )
   {
      memcpy( p_auth_tx->acct_id_1, HOST_MESSAGE.acct_id_1,
              sizeof(HOST_MESSAGE.acct_id_1));

      /* Set flag indicating this field came in a request.
       * At this point, we don't know if this is a request
       * or response, but that is ok.  Use FAF01 because
       * EB is not using the Fleet functionality at all.
       */
      p_auth_tx->FAF01_details.velocity[0].amount[0] = '1';
   }

   /* FIELD 103 ACCOUNT ID #2 */
   if ( HOST_MESSAGE.acct_id_2[0] != 0x00 )
   {
      memcpy( p_auth_tx->acct_id_2, HOST_MESSAGE.acct_id_2,
              sizeof(HOST_MESSAGE.acct_id_2));

      /* Set flag indicating this field came in a request.
       * At this point, we don't know if this is a request
       * or response, but that is ok.  Use FAF01 because
       * EB is not using the Fleet functionality at all.
       */
      p_auth_tx->FAF01_details.velocity[0].amount[1] = '1';
   }

   /*  Field 105 DBL-Length DES KEY */
   if ( 0 == strcmp(p_auth_tx->TLF01_details.message_type, "0800") )
   {
      if ( HOST_MESSAGE.cwk[0] != 0x00 )
         memcpy( ncf01_i.cwk, HOST_MESSAGE.cwk,
                 sizeof(HOST_MESSAGE.cwk));
   }

   /* FIELD 121 ISSUING INSTITUTION ID CODE */
   memcpy( p_auth_tx->iss_inst_id_cd, HOST_MESSAGE.iss_inst_id,
           sizeof(HOST_MESSAGE.iss_inst_id));

   /* FIELD 125 SUPPORT INFORMATION */
	if (HOST_MESSAGE.sup_info[0] != 0x00)
	{
		memcpy(p_auth_tx->TLF01_details.voice_auth_text,HOST_MESSAGE.sup_info,
	      sizeof(HOST_MESSAGE.sup_info));
	}

   /* FIELD 126 PRIVATE USE FIELDS */
   if ( HOST_MESSAGE.de126_len[0] > 0 )
   {
      /* Store this information in voice_auth_text.  It is the only field
       * large enough to hold it.  No need to add any fields to TLF01.
       * EB just wants this information logged for now - no processing on it.
       */
      Field126Parser( p_auth_tx->TLF01_details.voice_auth_text );
   }

   /* Store the original currency code and amount
    * before converting them (if required)
    */
   memcpy(p_auth_tx->reconciliation_cur_code,
          p_auth_tx->TLF01_details.currency_code,
          sizeof(p_auth_tx->TLF01_details.currency_code));
   memcpy(p_auth_tx->reconciliation_amount,
          p_auth_tx->TLF01_details.total_amount,
          sizeof(p_auth_tx->TLF01_details.total_amount));
   memcpy(p_auth_tx->TLF01_details.orig_amount,
          p_auth_tx->TLF01_details.actual_amount,
          sizeof(p_auth_tx->TLF01_details.actual_amount));

   /* Save the partial reversal amount into a field that does
    * not get modified.
    */
   memcpy( p_auth_tx->TLF01_details.refund_amount,
           p_auth_tx->TLF01_details.actual_amount, 12 );

   /*+------------------------------------------------------------------+
     | Here is a requirement unique to Equitable:                       |
     | ------------------------------------------                       |
     |                                                                  |
     |   If field 49 is "608" (PESO), use field 4 for the amount field, |
     |   and use the PESO Balance Account for authorization purposes.   |
     |                                                                  |
     |   If field 49 is not "608", use field 6 for the amount field, it |
     |   will be in US Dollars.                                         |
     |                                                                  |
     | This is for switched in, or Issuing, transactions only.          |
     |                                                                  |
     |                                                                  |
     | Requirements for Logging to TLF01:                               |
     | ----------------------------------                               |
     | 1.  If Currency Code (cc) is Peso                                |
     |     1.1  DE4  --> total_amount                                   |
     |     1.2  DE6  --> sales_amount  (not used, just logged)          |
     |     1.3  DE49 --> currency_code                                  |
     |     1.4  DE51 --> nfi_seq_nbr (not used, just logged)            |
     |                                                                  |
     | 2.  Else if cc is USD                                            |
     |     2.1  DE4  --> total_amount                                   |
     |     2.2  DE4  --> sales_amount (same value as DE6)               |
     |     2.3  DE49 --> currency_code                                  |
     |     2.4  DE49 --> nfi_seq_nbr (not used, just logged)            |
     |                                                                  |
     | 3.  Else (cc is other)                                           |
     |     3.1  DE4  --> sales_amount  (not used, just logged)          |
     |     3.2  DE6  --> total_amount                                   |
     |     3.3  DE49 --> nfi_seq_nbr (not used, just logged)            |
     |     3.4  DE51 --> currency_code                                  |
     |                                                                  |
     +------------------------------------------------------------------+*/
   if ((0 == strncmp(p_auth_tx->TLF01_details.message_type, "0100", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0101", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0120", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0400", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0401", 4)) ||
   (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0420", 4)))
   {
      /* A currency code of Pesos */
      if ( 0 != strcmp( PESO, p_auth_tx->TLF01_details.currency_code ) )
      {
         /* or dollars is allowed. */
         if ( 0 != strcmp( USD, p_auth_tx->TLF01_details.currency_code ) )
         { 
            /* else use ch billing's currency code */
            if ( 0 == strcmp( USD, p_auth_tx->ch_billing_curr_code ) )
            {
               /* Currency Code is neither Peso nor USD.
                * Store according to comment box above.
                */

               /* Copy DE4 to sales_amount.  */
               memcpy( p_auth_tx->TLF01_details.sales_amount,
                       p_auth_tx->TLF01_details.total_amount,
                       12 );

               /* Copy DE6 to total_amount. */
               memcpy( p_auth_tx->TLF01_details.total_amount,
                       p_auth_tx->ch_billing_amt,
                       12 );

               /* Copy DE49 to nfi_seq_nbr. */
               memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
                       p_auth_tx->TLF01_details.currency_code,
                       3 );

               /* Copy DE51 to currency_code. */
               memcpy( p_auth_tx->TLF01_details.currency_code,
                       p_auth_tx->ch_billing_curr_code,
                       3 );

               memcpy (p_auth_tx->TLF01_details.actual_amount,
                       p_auth_tx->TLF01_details.add_amounts+24,
                       sizeof(p_auth_tx->TLF01_details.actual_amount));
            }
            else
            {
               if ( 0==strncmp(p_auth_tx->TLF01_details.processing_code,"30",2))
               {
                  /* Balance Inquiry can have a 3rd currency code.  But must
                   * use USD account.  Need to save original currency code
                   * to echo back out.
                   */
                  memcpy( p_auth_tx->MCF01_details.region_nbr,
                          p_auth_tx->TLF01_details.currency_code,
                          3 );
                  memcpy( p_auth_tx->TLF01_details.currency_code, USD, 3 );
               }
               else
               {
                  /* This is used to determine if the currency code is ok. */
                  memcpy(p_auth_tx->TLF01_details.response_code, "91", 2);
               }
            }
         }
         else
         {
            /* Currency Code is USD.  Store according to comment box above. */

            /* DE4  is already in total_amount.  */
            /* DE49 is already in currency_code. */

            /* Copy DE4 to sales_amount. */
            memcpy( p_auth_tx->TLF01_details.sales_amount,
                    p_auth_tx->TLF01_details.total_amount,
                    12 );

            /* Copy DE49 to nfi_seq_nbr. */
            memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
                    p_auth_tx->TLF01_details.currency_code,
                    3 );
         }
      }
      else
      {
         /* Currency Code is PESO.  Store according to comment box above. */

         /* DE4  is already in total_amount.  */
         /* DE49 is already in currency_code. */

         /* Copy DE6 to sales_amount. */
         memcpy( p_auth_tx->TLF01_details.sales_amount,
                 p_auth_tx->ch_billing_amt,
                 12 );

         /* Copy DE51 to nfi_seq_nbr. */
         memcpy( p_auth_tx->TLF01_details.nfi_seq_nbr,
                 p_auth_tx->ch_billing_curr_code,
                 3 );
      }
   }

   if ((0 == strncmp(p_auth_tx->TLF01_details.message_type, "0400", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0401", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0102", 4)) ||
       (0 == strncmp(p_auth_tx->TLF01_details.message_type, "0420", 4)))
   {
      total_int    = atoi(p_auth_tx->TLF01_details.total_amount);
      actual_int   = atoi(p_auth_tx->TLF01_details.actual_amount);
      reversal_int = total_int - actual_int;
      itoa(reversal_int, p_auth_tx->TLF01_details.reversal_amount,10);
      Rj_with_lead_zeros(p_auth_tx->TLF01_details.reversal_amount, 12);

      /* Do this check only for reversals, not ATM confirms.
       * The reason is, DE61 (actual amt for ATM Confirm) is in USD,
       * whereas DE4 will be in the original currency.
       */
      if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
      {
         if(actual_int > total_int)
         {
            memset( tempstr, 0x00, sizeof(tempstr) );
            sprintf( tempstr,
                    "Dispensed amount (%d) is greater than total amount (%d).",
                     actual_int, total_int );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(tempstr,Log_Bufr);
            ncvsms_log_message( 3, 3, tempstr , "move_iso_response_to_auth_struct" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }
   }
   if (false == verify_auth_tx_data(p_auth_tx))
      return(false);

   return(true);


} /* move_iso_response_to_auth_struct */

/*****************************************************************************

  Function:    build_host_structure

  Description: This function will use the data in the auth_tx entry to build 
               the HOST_MESSAGE structure that will be required to build the 
               VISA message for the host.

  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      None
  Modified by:
      BGable      6/30/98
*********************y*********************************************************/
void build_host_structure( pAUTH_TX p_auth_tx )
{	
	#define  F126_BITMAP_LEN  8

   CHAR   temp_str[70]       	= {0};
   CHAR   orig_data[43]		 	= {0};
   CHAR   new_string[50]	 	= {0};
   CHAR   tempstr [33]		 	= {0};
   UINT   count				 	= 0;
   INT    i					 	= 0;
   BYTE   F62BitMap[8]			= {0};
   INT    F62Len				= 0;
   BYTE   F63BitMap[3]			= {0};
   INT    F63Len				= 0;
   BYTE   visa_char				= 0;
   BYTE   visa_tran[17]			= {0};
   BYTE   visa_tran_bcd[8]		= {0};
   BYTE   pos_entry_mode [5] 	= {0};
   BYTE   pos_cc[3] 			= {0};
   BYTE   processing_code [7]	= {0};
   BYTE   temp_currency_code[4]	= {0};
   BYTE   acct_type				= 0;
   INT    iIndustry				= 0;
   INT    RPS_flag 				= 0;
   DWORD  total_amount			= 0;
   DWORD  reversal_amount		= 0;
   CHAR   gmt_time[15]			= {0};
   CHAR   time_date[25] 		= {0};
   INT  emv_txn					= 0;
   INT  emv_len					= 0;
   CHAR DE62_27[8] 				= {0};
   CHAR DE62_27_ebcidic[8] 		= {0};
   INT    F126Len 				= 0;
   INT	  len					= 0;
   BYTE   cvv_data[50]			= {0};
   BYTE   bcavv_data[41]		= {0};

   /* Initialize the host structure before filling it. */

    memset( &HOST_MESSAGE, 0x00, sizeof(HOST_MESSAGE) );
   iIndustry = atoi( p_auth_tx->industry_code );

   /* VISA HEADER */

   /* Stored in Merchant Contact field for switched in txns.
    * If txn is switched out, this field will not get copied
    * to the auth_tx from the timer, so it has no effect.
    */
   memcpy( HOST_MESSAGE.header, p_auth_tx->MCF01_details.contact, 23 );

		if ( (p_auth_tx->EMV_details.emv_block[0] == 0x00) &&
	        (p_auth_tx->EMV_details.emv_block[1] == 0x00)  )
	      emv_txn = false;
	   else
	   {
	      emv_txn = true;
	      
	   }

   /* FIELD 2: PAN - Prepend leading zero if length is odd. */
	HOST_MESSAGE.pan_len[0] = atoi( p_auth_tx->TLF01_details.card_num_len );

   if ( (HOST_MESSAGE.pan_len[0] % 2) == 1 )
   {
      memset (temp_str, 0, sizeof(temp_str));
      temp_str[0] = '0';
      memcpy (&temp_str[1], p_auth_tx->TLF01_details.card_num,
              HOST_MESSAGE.pan_len[0]);
      str_asc_to_bcd(temp_str,HOST_MESSAGE.pan_len[0] + 1,HOST_MESSAGE.pan_bin);
      HOST_MESSAGE.pan_len[0] = strlen(p_auth_tx->TLF01_details.card_num);
   }
   else
   {
      str_asc_to_bcd (p_auth_tx->TLF01_details.card_num, sizeof(HOST_MESSAGE.pan_bin)*2, HOST_MESSAGE.pan_bin);
      HOST_MESSAGE.pan_len[0] = strlen(p_auth_tx->TLF01_details.card_num);
   }

   
   /* FIELD 3: PROCESSING CODE */
   memcpy( processing_code,
           p_auth_tx->TLF01_details.processing_code,
           sizeof(processing_code) );

   if( p_auth_tx->TLF01_details.tx_key == AUTH_CASH_DISBURSEMENT_ADJUSTMENT )
   {
      memcpy( processing_code, "22", 2 );
   }
   else if((p_auth_tx->TLF01_details.tx_key != AUTH_REPRESENTMENT)           &&
           (p_auth_tx->TLF01_details.tx_key != AUTH_BACKOFFICE_ADJUSTMENT)   &&
           (p_auth_tx->TLF01_details.tx_key != AUTH_FEE_COLLECTION_ACQUIRER) &&
           (p_auth_tx->TLF01_details.tx_key != AUTH_FUNDS_DISBURSEMENT_ACQUIRER))
   {
      /* Just pass processing code through for adjustment and representment. */
      if( (p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && 
          (p_auth_tx->TLF01_details.tx_key <  AUTH_INVALID) )
      { /* request */
         if (00 == strcmp(processing_code, NULL_STR))
         {
            strncpy(processing_code, "000000", 6);
         }
         switch (iIndustry)
         {
            case CASH:   /* Cash */
                  strncpy(processing_code, "01", 2);
               break;
            case ATM:   /* ATM */
                  strncpy(processing_code, "01", 2);
               break;
            case QUASI_CASH:   /* Quasi Cash */
                  strncpy(processing_code, "11", 2);
               break;
            default:
               break;
         }
         if(('Y' == p_auth_tx->TLF01_details.voice_auth[0]) &&( strncmp(processing_code,"18",2) == 0))
         {
            strncpy(processing_code, "11", 2);
         }

         if (AUTH_CASH_ADVANCE == p_auth_tx->TLF01_details.tx_key) 
            strncpy(processing_code, "01", 2);

         if (AUTH_BALANCE_INQUIRY == p_auth_tx->TLF01_details.tx_key) 
            strncpy(processing_code, "30", 2);

         /* These 2 lines are in here specifically for Equitable. */
         if (AUTH_CARD_VERIFICATION == p_auth_tx->TLF01_details.tx_key) 
            memcpy( processing_code, "00", 2 );
      }
   }
   asc_to_bcd (processing_code, sizeof(HOST_MESSAGE.proccode)*2,
               HOST_MESSAGE.proccode);
   /* In order to show proceesing code in transaction lookup page( OCT-2017 Mandated changes) */
   	memcpy(p_auth_tx->TLF01_details.product_codes[17].amount,processing_code,sizeof(HOST_MESSAGE.proccode)*2);


   /* FIELD 4: AMOUNT */
	if(p_auth_tx->TLF01_details.tx_key == AUTH_CASH_DISBURSEMENT_ADJUSTMENT)
	{
		asc_to_bcd (p_auth_tx->TLF01_details.reversal_amount, sizeof(HOST_MESSAGE.amount_bin)*2,
               HOST_MESSAGE.amount_bin);		
	}
	else
	{
		asc_to_bcd (p_auth_tx->TLF01_details.total_amount, sizeof(HOST_MESSAGE.amount_bin)*2,
               HOST_MESSAGE.amount_bin);
   }
   /* FIELD 6: CARDHOLDER BILLING AMOUNT */
   if (p_auth_tx->ch_billing_amt [0] != '\0')
   {
      asc_to_bcd (p_auth_tx->ch_billing_amt, sizeof(HOST_MESSAGE.card_bill_amt)*2,
                  HOST_MESSAGE.card_bill_amt);
   }

   /* FIELD 7: GREENWICH TIME - MMDDhhmmss from the YYYYMMDDhhmmss date */
   if(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL)
   {
      if ( p_auth_tx->orig_local_time_date[0] == 0x00 )
      {
         strcpy( p_auth_tx->orig_local_time_date,
                 p_auth_tx->TLF01_details.transmission_timestamp);

         pintime_get_gmt( gmt_time );
         strncpy( p_auth_tx->TLF01_details.transmission_timestamp,
                 &gmt_time[4],
                  10 );
      }
   }

   asc_to_bcd (p_auth_tx->TLF01_details.transmission_timestamp,
                sizeof(HOST_MESSAGE.gmt_time)*2,
                HOST_MESSAGE.gmt_time);

   /* FIELD 10: CARDHOLDER BILLING CONVERSION RATE */
   if (true == hhutil_verify_pin_block(p_auth_tx))
   {
      asc_to_bcd (p_auth_tx->ch_billing_conv_rate,
                  sizeof(HOST_MESSAGE.conv_rate)*2,
                  HOST_MESSAGE.conv_rate);
   }

   /* FIELD 11: STAN */
   if ( p_auth_tx->TLF01_details.tx_key == AUTH_CASH_DISBURSEMENT_ADJUSTMENT )
   {
      /* Need to use a different stan for these.
       * Cannot use the one from the orig 0200.
       */
      memset( temp_stan, 0x00, sizeof(temp_stan) );
      memcpy( temp_stan,
              p_auth_tx->TLF01_details.transmission_timestamp+4,
              6 );
      asc_to_bcd (temp_stan,
                  sizeof(HOST_MESSAGE.stan_bin)*2,
                  HOST_MESSAGE.stan_bin);
   }
   else
   {
      if(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL)
      {
         strcpy( p_auth_tx->TLF01_details.sys_trace_audit_num,
                 p_auth_tx->TLF01_details.orig_sys_trace_num);
      }
      asc_to_bcd (p_auth_tx->TLF01_details.sys_trace_audit_num,
                  sizeof(HOST_MESSAGE.stan_bin)*2,
                  HOST_MESSAGE.stan_bin);
   }

   /* FIELD 12: TIME */
	 if ( p_auth_tx->TLF01_details.time_hhmmss[0] == 0x00 )
    {
		ptetime_get_timestamp( time_date );
		get_time( time_date, p_auth_tx->TLF01_details.time_hhmmss );
	 }
    asc_to_bcd (p_auth_tx->TLF01_details.time_hhmmss, sizeof(HOST_MESSAGE.time_bin)*2,
                 HOST_MESSAGE.time_bin);
  	
   
   /* FIELD 13: DATE */
   memset(temp_str, 0, sizeof(temp_str));

   if ( p_auth_tx->TLF01_details.date_yyyymmdd[0] == 0x00 )
   {
	   ptetime_get_timestamp( temp_str );
	   get_date( temp_str, p_auth_tx->TLF01_details.date_yyyymmdd );
   }
	asc_to_bcd (p_auth_tx->TLF01_details.date_yyyymmdd+4, sizeof(HOST_MESSAGE.date_bin)*2,
               HOST_MESSAGE.date_bin);

   /* FIELD 14: EXPIRATION DATE */
   asc_to_bcd (p_auth_tx->TLF01_details.exp_date, sizeof(HOST_MESSAGE.date_expd)*2,
               HOST_MESSAGE.date_expd);

	/* FIELD 15: SETTLEMENT DATE (MMDD) */
   /*
	if(p_auth_tx->TLF01_details.tx_key == AUTH_ADMINISTRATIVE_TEXT_RESPONSE)
	{
		asc_to_bcd ("0203", sizeof(HOST_MESSAGE.date_settle)*2,
						HOST_MESSAGE.date_settle);
	}
	else
	{
	*/
		asc_to_bcd (p_auth_tx->TLF01_details.settlement_date, sizeof(HOST_MESSAGE.date_settle)*2,
						HOST_MESSAGE.date_settle);
	/* } */

   /* FIELD 18: SIC NUMBER */

/*   asc_to_bcd (p_auth_tx->TLF01_details.category_code , sizeof(HOST_MESSAGE.sic_number)*2,
               HOST_MESSAGE.sic_number);*/

   asc_to_bcd ("6011" , sizeof(HOST_MESSAGE.sic_number)*2, HOST_MESSAGE.sic_number);

   /* FIELD 19: COUNTRY CODE */
   if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID))
   { /* request */
      strcpy (temp_str, ncf01_i.country_code);
      Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2);
      asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2, HOST_MESSAGE.ctry_code);
   }
   else /* response */
   {
      strcpy (temp_str, p_auth_tx->country_code);
      Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2);
      asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.ctry_code)*2, HOST_MESSAGE.ctry_code);
   }

   /* FIELD 20: PAN EXTENDED, COUNTRY CODE */
   strcpy (temp_str, p_auth_tx->pan_ext_ctry_code);
   Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.pan_ctry_cd)*2);
   asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.pan_ctry_cd)*2, HOST_MESSAGE.pan_ctry_cd);

    /* FIELD 21: Forwarding Institution Country Code *
     * This field should not go out in any requests.
     * In responses, no action needed. If it came in as part of a switched in
     * txn, then the info is still in HOST_MESSAGE, so it is automatically
     * echoed.  Therefore, this field is commented out.
     *
   if( (p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) &&
       (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID) )
   {
      * Request *
      strcpy (temp_str, FWD_INS_CTRY_CODE);
      Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.fwd_inst_ctry_cd)*2);
      asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.fwd_inst_ctry_cd)*2, HOST_MESSAGE.fwd_inst_ctry_cd);
   } */

   /* FIELD 22: POS ENTRY MODE */
   /* Use a temporary variable to create the new pos entry mode so
    * that the original value is inserted into TLF01.
    */
   memcpy (pos_entry_mode, p_auth_tx->TLF01_details.pos_entry_mode, sizeof(pos_entry_mode));
   if (pos_entry_mode[0] != '\0')  
   {
      /*+--------------------------------------------------------------------+
        | Equitable is using some POS terminals at their provincial satellite|
        | offices acting as offsite authorization centers. Their satellite   |
        | offices use the POS terminal to key in transaction to get approval.|
        | Each satellite office has its own merchant number assignment.  The |
        | problem is that the POS Indicator being sent to Visa Int'l is      |
        | currently defaulted to '01'.  Visa is requiring Equitable to use   |
        | POS Entry Mode Code = '00' for their satellite offices.  Equitable |
        | will indicate which merchants are satellites with an Organization  |
        | ID of "0001".                                                      |
        +--------------------------------------------------------------------+*/

      if (p_auth_tx->TLF01_details.tx_key == AUTH_CASH_DISBURSEMENT_ADJUSTMENT)
      {
         /* This can be hard coded for partial dispense. */
         strcpy( pos_entry_mode, "0110" );
      }
      else if (0 == strcmp(pos_entry_mode, "0901"))
      {
         strcpy(pos_entry_mode,"9010");
      }
	  else if (emv_txn == true)
      {
		  if (0 == strncmp("007",p_auth_tx->TLF01_details.pos_entry_mode,3) )
			{
			   /* Contact less Txn */
			   strcpy( pos_entry_mode, "07n0" );
			}
			else
			{
			   /* Normal Chip Card Txn */
			   strcpy( pos_entry_mode, "05n0" );
			}
			pos_entry_mode[2] = p_auth_tx->TLF01_details.pos_entry_mode[3];
      }else if(0 == strncmp(pos_entry_mode, "080",3)) // Fall back from IST
      {
      	 strcpy( pos_entry_mode, "90n0" );
		 pos_entry_mode[2] = p_auth_tx->TLF01_details.pos_entry_mode[3];
      }
      else
      {
			if ( 0 == strcmp( "0001",
									 p_auth_tx->MCF01_details.primary_key.organization_id) )
			{
				/* '+1' is used to bypass the 0 filler in the first byte. */
				memset( pos_entry_mode+1, '0', 2 );
			}
		
			/*
			 * input is Hypercom iso8583: 0aab   where
			 *    aa = PAN entry mode
			 *    b  = PIN entry capabitlity
			 *
			 * convert to Visa iso8583:   aab0   where
			 *    aa = PAN entry mode
			 *    b  = PIN entry capabitlity
			 */
			pos_entry_mode [0] = 0x30;     /* set to '0' */
			pos_entry_mode [1] = pos_entry_mode [2];
			pos_entry_mode [2] = pos_entry_mode [3];
			pos_entry_mode [3] = 0x30;     /* set to '0' */

			switch (pos_entry_mode [1])
			{
				case '0': break;  /* OK.  leave as is */
				case '1': break;  /* OK.  leave as is */
				case '5': break;  /* OK.  leave as is */
				case '2':
					pos_entry_mode [0] = '9';
					pos_entry_mode [1] = '0';
					break;
				default:
					/* unknown PAN mode: force to '0' */
					pos_entry_mode [1] = '0';
					break;
			}

			switch (pos_entry_mode [2])	
			{
				case '0': break;  /* OK.  leave as is */
				case '1': break;  /* OK.  leave as is */
				case '2': break;  /* OK.  leave as is */
				default:
					/* unknown PIN mode: force to '0' */
					pos_entry_mode [2] = '0';
					break;
			}
			pos_entry_mode [4] = '\0';
		}
   }

   asc_to_bcd (pos_entry_mode, sizeof(HOST_MESSAGE.posentry)*2, 
               HOST_MESSAGE.posentry);

   memcpy(p_auth_tx->TLF01_details.product_codes[17].code,pos_entry_mode,sizeof(HOST_MESSAGE.posentry)*2);

/*Field 23 Card seq number*/
   de23_present = false;
   if ((p_auth_tx->host2_message_seq_nbr[0] != 0x00) )
	 {
		/* Card Seq Nbr not in DE55. It came in DE23 from the terminal. */
		i = atoi( p_auth_tx->host2_message_seq_nbr );
		 de23_present = true;
		/* Value must be right justified, zero filled. */
		sprintf( temp_str, "%04d", i );
		asc_to_bcd( temp_str, sizeof(HOST_MESSAGE.cardseqnum)*2, 
					HOST_MESSAGE.cardseqnum);
	 }


   /* FIELD 25: POS CONDITION CODE */
   memcpy( pos_cc, p_auth_tx->TLF01_details.pos_condition_code, 2 );
   if ((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) &&
       (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID))
   {
      /* Request */
      if ( ECOMMERCE == iIndustry )
         /* E-Commerce */
         strcpy( pos_cc, "59" );

      else if ( (MAIL_PHONE == iIndustry) || (RPS == iIndustry) )
      {
         /* Mail/phone order or Recurring Payment */
         strcpy( pos_cc, "08" );

         /* Set RPS flag for field 60, if Recurring Payment. */
         if ( RPS == iIndustry )
            RPS_flag = 1;
      }
      else
      {
         i = atoi(p_auth_tx->MCF01_details.voice_category_code);
         if ((i >= 5960) && (i <= 5969))
            /* Mail/phone order */
            strcpy( pos_cc, "08" );

         else if (NULL != strstr( p_auth_tx->TLF01_details.handler_queue, "rps" ) )
         {
            /* RPS */
            strcpy( pos_cc, "08" );
            RPS_flag = 1;
         }

         else
            /* Any other purchase */
            strcpy( pos_cc, "00" );
      }

	  if (p_auth_tx->TLF01_details.tx_key == AUTH_REPRESENTMENT)
	  {
		strcpy( pos_cc, "13" );
	  }
	  else if (p_auth_tx->TLF01_details.tx_key == AUTH_CHARGEBACK_RESPONSE)
	  {
		strcpy( pos_cc, "17" );
	  }
	  else if (p_auth_tx->TLF01_details.tx_key == AUTH_CHARGEBACK_REVERSAL_RESPONSE)
	  {
		strcpy( pos_cc, "54" );
	  }
   }
   asc_to_bcd(pos_cc,sizeof(HOST_MESSAGE.pos_condcd)*2,HOST_MESSAGE.pos_condcd);

   /* FIELD 28: TRANSACTION FEE AMOUNT */
   /* If reversal txn is reversal prefix 'C' else  prefix 'D'*/

   if ( p_auth_tx->TLF01_details.tran_fee_amount[0] != 0x00 )
   {
   		if((p_auth_tx->TLF01_details.tx_key==AUTH_REVERSAL) || 
	  	(p_auth_tx->TLF01_details.tx_key==AUTH_REVERSAL_REPEAT)||
	  	(p_auth_tx->TLF01_details.tx_key==AUTH_REVERSAL_ADVICE))
   {
	    HOST_MESSAGE.tran_fee[0]='C'; // TF Phani: Earlier default value 'D', Now changed to C or D
   		}else
	    HOST_MESSAGE.tran_fee[0]='D';
      //HOST_MESSAGE.tran_fee[0] = p_auth_tx->TLF01_details.cvc_data[0];
	   memcpy( &HOST_MESSAGE.tran_fee[1], p_auth_tx->TLF01_details.tran_fee_amount, bf28_len-1 );
   }
   else
   {
	  	   memset( &HOST_MESSAGE.tran_fee[1],'0', bf28_len-1);              /* Set with all zero.. TF - Ajay*/
   }

   /* FIELD 32: ACQUIRING INSTITUTION ID CODE */
   memset (new_string, 0, sizeof(new_string));
   if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) && (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID))
   { /* request */
      strcpy(new_string,ncf01_i.acquirer_inst_id_code);
      HOST_MESSAGE.acq_inst_len[0] = strlen(new_string);

      /* Store here for TLF01. */
      if ( p_auth_tx->TLF01_details.acquiring_id[0] == 0x00 )
         strcpy( p_auth_tx->TLF01_details.acquiring_id, new_string );
   }
   else
   { /* response */
      HOST_MESSAGE.acq_inst_len[0] = p_auth_tx->acq_inst_id_len[0];
      strcpy(new_string,p_auth_tx->acq_inst_id_cd);
   }
    
   /* If odd length, provide a leading zero. */
	if (1 == (strlen(new_string) % 2) )
   {
      memset (temp_str, 0, sizeof(temp_str));
      temp_str [0] = '0';
      strcpy( &temp_str[1], new_string );
      memset (new_string, 0, sizeof(new_string));
      strcpy( new_string,temp_str);
   }

   asc_to_vbcd (new_string, sizeof(HOST_MESSAGE.acq_inst_id)*2,
                HOST_MESSAGE.acq_inst_id);

   /* FIELD 33: FORWARDING INSTITUTION ID CODE
    *
    * Only use this field if DE32 is not present, except always use in 0600
    * and 0432.
    */
   if ((p_auth_tx->TLF01_details.tx_key == AUTH_ADMINISTRATIVE_TEXT               ) ||
       (p_auth_tx->TLF01_details.tx_key == AUTH_ADMINISTRATIVE_TEXT_RESPONSE      ) ||
       (p_auth_tx->TLF01_details.tx_key == AUTH_FEE_COLLECTION_ISSUER_RESPONSE    ) ||
       (p_auth_tx->TLF01_details.tx_key == AUTH_FUNDS_DISBURSEMENT_ISSUER_RESPONSE) ||
       (HOST_MESSAGE.acq_inst_len[0]    == 0x00                                   )  )
   {
      if (p_auth_tx->forwarding_inst_id_cd[0] == 0x00)
      {
         strcpy( p_auth_tx->forwarding_inst_id_cd,
                 ncf01_i.forwarding_inst_id_code );
      }
      if (p_auth_tx->forwarding_inst_id_cd[0] != 0x00)
      {
         memset(temp_str, 0, sizeof(temp_str));
         HOST_MESSAGE.fwd_inst_len[0] = strlen(p_auth_tx->forwarding_inst_id_cd);
         if (1 == HOST_MESSAGE.fwd_inst_len[0] % 2) 
         {
            temp_str [0] = '0';
            strcpy( &temp_str[1], p_auth_tx->forwarding_inst_id_cd );
         }
         else
            strcpy( temp_str, p_auth_tx->forwarding_inst_id_cd );

         asc_to_vbcd (temp_str, sizeof(HOST_MESSAGE.fwd_inst_id)*2,
                      HOST_MESSAGE.fwd_inst_id);
      }
   }

   /* FIELD 34: PAN EXTENDED */
   if ( 1 == (strlen(p_auth_tx->pan_extended) % 2) )
   {
      memset (temp_str, 0, sizeof(temp_str));
      temp_str[0] = '0';
      memcpy (&temp_str[1], p_auth_tx->pan_extended, strlen(p_auth_tx->pan_extended));
      str_asc_to_bcd( temp_str, strlen(temp_str), HOST_MESSAGE.pan_ext );
	   HOST_MESSAGE.pan_ext_len[0] = strlen( temp_str );
   }
   else
   {
	   str_asc_to_bcd (p_auth_tx->pan_extended, sizeof(HOST_MESSAGE.pan_ext)*2, HOST_MESSAGE.pan_ext);
	   HOST_MESSAGE.pan_ext_len[0] = strlen( p_auth_tx->pan_extended );
   }

   /* FIELD 35: TRACK 35 */
	if (p_auth_tx->TLF01_details.track2 [0] != '\0')
   {

      /* If odd length, prepend leading zero. */
      if ( 1 == (strlen(p_auth_tx->TLF01_details.track2) % 2) )
      {
         memset (temp_str, 0, sizeof(temp_str));
         temp_str [0] = '0';
         memcpy (&temp_str[1], p_auth_tx->TLF01_details.track2, strlen(p_auth_tx->TLF01_details.track2));
         asc_to_vbcd (temp_str, sizeof(HOST_MESSAGE.track2)*2, HOST_MESSAGE.track2);
         HOST_MESSAGE.track2_len[0] = strlen(p_auth_tx->TLF01_details.track2);
      }
      else
      {
         asc_to_vbcd (p_auth_tx->TLF01_details.track2, sizeof(HOST_MESSAGE.track2)*2, HOST_MESSAGE.track2);
         HOST_MESSAGE.track2_len[0] = strlen(p_auth_tx->TLF01_details.track2);
      }
   }
    
   /* FIELD 37: RETRIEVAL REFERENCE NUMBER */
	if (p_auth_tx->TLF01_details.orig_retrieval_ref_num [0] != '\0')
      memcpy (HOST_MESSAGE.rrn, p_auth_tx->TLF01_details.orig_retrieval_ref_num, sizeof(HOST_MESSAGE.rrn));
   else
      memcpy (HOST_MESSAGE.rrn, p_auth_tx->TLF01_details.retrieval_ref_num, sizeof(HOST_MESSAGE.rrn));

			
   /* FIELD 39: RESPONSE CODE */
   if ( p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE )
   {
      if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code,TIMED_OUT) )
      {
         /* This response code is not allowed in an 0410 response.
          * Instead, use 'no action taken - unable to back out prior txn'.
          * Refer to SCR 764.
          */
         strcpy( p_auth_tx->TLF01_details.response_code, NO_ACTION_TAKEN );
      }
   }
   memcpy( HOST_MESSAGE.responsecd, p_auth_tx->TLF01_details.response_code, 2 );


   /* FIELD 38: AUTHORIZATION IDENTIFICATION RESPONSE 
      Populate this field only when the response code is Zero
   */
   if(p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)
   {
        if(strcmp(p_auth_tx->TLF01_details.response_code,"00") == 0)
            memcpy( HOST_MESSAGE.authcode, p_auth_tx->TLF01_details.auth_number,sizeof(HOST_MESSAGE.authcode) );
   }
   else if ( p_auth_tx->TLF01_details.auth_number[0] != 0x00 )
   {
      memcpy( HOST_MESSAGE.authcode,
              p_auth_tx->TLF01_details.auth_number,
              sizeof(HOST_MESSAGE.authcode) );
   }

   /* FIELD 41: CARD ACCEPTOR TERMINAL ID */
   if( (( 0 == strlen(p_auth_tx->TLF01_details.terminal_id)) &&
	    ( 0 == strncmp (p_auth_tx->TLF01_details.handler_queue,"dcp",3)))&&
	   (( 0 == strncmp (p_auth_tx->TLF01_details.message_type,"0100",4))||
		( 0 == strncmp (p_auth_tx->TLF01_details.message_type,"0200",4))))
  {
	   memcpy(HOST_MESSAGE.termid,VISA_DEFAULT_TID,VISA_DEFAULT_TID_LEN);
	   memset(p_auth_tx->TLF01_details.terminal_id,0x00,VISA_DEFAULT_TID_LEN);
	   strncpy(p_auth_tx->TLF01_details.terminal_id,VISA_DEFAULT_TID,VISA_DEFAULT_TID_LEN);
  }
   else if (strlen(p_auth_tx->TLF01_details.terminal_id) < 8)
   {
      memset(temp_str, 0, sizeof(temp_str));
      strcpy(temp_str, p_auth_tx->TLF01_details.terminal_id);
      for (i = strlen(p_auth_tx->TLF01_details.terminal_id); i <8 ; i++)
         temp_str[i] = ' ';
      memcpy(HOST_MESSAGE.termid,temp_str, sizeof(HOST_MESSAGE.termid));
   }
   else
      memcpy(HOST_MESSAGE.termid,p_auth_tx->TLF01_details.terminal_id, sizeof(HOST_MESSAGE.termid));

   /* FIELD 42: CARD ACCEPTOR IDENTIFICATION CODE */
	memset(HOST_MESSAGE.cardaccid, 0x20, sizeof(HOST_MESSAGE.cardaccid));
   memcpy(HOST_MESSAGE.cardaccid, p_auth_tx->TLF01_details.merchant_id, strlen(p_auth_tx->TLF01_details.merchant_id));	

   /* FIELD 43: CARD ACCEPTOR NAME/LOCATION */
   if((p_auth_tx->TLF01_details.tx_key >= AUTH_SALE) &&
      (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID))
   {
      /* Only send this field for Switched out transactions. */
      memset(HOST_MESSAGE.cardaccname, 0x20, sizeof(HOST_MESSAGE.cardaccname));
      if ( iIndustry == ATM )
      {
         /* Transaction is ATM. */
         memcpy( HOST_MESSAGE.cardaccname, p_auth_tx->MCF01_details.address01,
                 strlen(p_auth_tx->MCF01_details.address01));	
      }
      else
      {
         /* Transaction is a POS transaction. */
         memcpy( HOST_MESSAGE.cardaccname, p_auth_tx->MCF01_details.name02,
                 strlen(p_auth_tx->MCF01_details.name02));	
      }
      memcpy( HOST_MESSAGE.cardaccname+25, p_auth_tx->MCF01_details.city,
              strlen(p_auth_tx->MCF01_details.city));	
      memcpy( HOST_MESSAGE.cardaccname+38, "PH", 2);

      /* Store DE43 here so it gets logged to TLF01:
       *   Merchant Name         --> TLF01.card_holder_name
       *   City and Country Code --> TLF01.ncf01_retired_cwk
       * SCR 775
       */
      memcpy( p_auth_tx->TLF01_details.card_holder_name,
              HOST_MESSAGE.cardaccname,
              25 );
      memcpy( p_auth_tx->TLF01_details.ncf01_retired_cwk,
              HOST_MESSAGE.cardaccname+25,
              15 );
   }

   /* FIELD 44: ADDITIONAL RESPONSE DATA */
   if (p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)  
   {  /* response */

      /* Always set field 44.1 to '5'.  This indicates the issuer
       * has set the response code.
       */
      HOST_MESSAGE.addlrspdata[0]='5';

      if( p_auth_tx->TLF01_details.cvc == CVV_TEST_FLAG )
      {
         /* CVV and CVV2 were not validated. Just send back field 44.1 */
         HOST_MESSAGE.addl_rsp_len[0] = 1;
      }
      else
      {
         /* CVV or CVV2 was validated.  Need to populate 44.5 so 
          * space fill the fields in between 1 and 5.
          */
         HOST_MESSAGE.addlrspdata[1]=' ';
         HOST_MESSAGE.addlrspdata[2]=' ';
         HOST_MESSAGE.addlrspdata[3]=' ';

         /* This combo means that CVV failed */
         if(0 != strncmp(p_auth_tx->TLF01_details.security_response_code,"00",2) &&
           (0 == strncmp(p_auth_tx->TLF01_details.response_code, "05", 2)))
         {
            HOST_MESSAGE.addlrspdata[4] = '1';
         }
         else
         {
            HOST_MESSAGE.addlrspdata[4] = '2';
         }

         /* Set the length of the field. */
         HOST_MESSAGE.addl_rsp_len[0] = 5;
      }
   }
   
   /* FIELD 45: TRACK 1 Usage 7a Representment */
   if (p_auth_tx->TLF01_details.track1 [0] != '\0')
   {
      HOST_MESSAGE.track1_len[0] = strlen(p_auth_tx->TLF01_details.track1);
		memcpy (HOST_MESSAGE.track1, p_auth_tx->TLF01_details.track1, HOST_MESSAGE.track1_len[0]);
   }


	/* FIELD 48 Addntl Data-Private Usage 9a */
   memset( &HOST_MESSAGE.addl_data, 0x00, sizeof(HOST_MESSAGE.addl_data) );
   memset(temp_str, 0, sizeof(temp_str));

   if (p_auth_tx->TLF01_details.tx_key == AUTH_ADMINISTRATIVE_TEXT )  
   {		
	   temp_str[0] = '#';
	   strcpy(&temp_str[1],p_auth_tx->TLF01_details.voice_auth_text);
   }

   else if ( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL)                    || 
             (p_auth_tx->TLF01_details.tx_key == AUTH_REPRESENTMENT)               ||
             (p_auth_tx->TLF01_details.tx_key == AUTH_CASH_DISBURSEMENT_ADJUSTMENT)||
             (p_auth_tx->TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT )      ||
             (p_auth_tx->TLF01_details.tx_key == AUTH_CHARGEBACK )                 ||
             (p_auth_tx->TLF01_details.tx_key == AUTH_CHARGEBACK_REVERSAL )
      ) 
   {
      /* FIELD 48 Addntl Data-Private Usage 10 */
      if(p_auth_tx->MCF01_details.vs_tcc_code[0] == 'P')	   
      {
         temp_str[0] = p_auth_tx->MCF01_details.vs_tcc_code[0];
         memcpy(&temp_str[1],p_auth_tx->MCF01_details.negative_group_id,9);
         memcpy(&temp_str[10],p_auth_tx->MCF01_details.group_type,4);		
      }
      else if( p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE)
      {
         temp_str[0] = 'P';
         i = strlen( p_auth_tx->TLF01_details.voice_auth_text );
         memcpy( &temp_str[1], p_auth_tx->TLF01_details.voice_auth_text, i );
      }

      /* FIELD 48 Addntl Data-Private Usage 7a */
      else if(p_auth_tx->MCF01_details.vs_tcc_code[0] == 'V')	   
      {
         temp_str[0] = p_auth_tx->MCF01_details.vs_tcc_code[0];
         temp_str[1] = p_auth_tx->MCF01_details.pos_code[0];
         temp_str[2] = p_auth_tx->MCF01_details.mall_code[0];
         memcpy(&temp_str[3],p_auth_tx->MCF01_details.password,6);
         memcpy(&temp_str[9],p_auth_tx->MCF01_details.name01,30);
         memcpy(&temp_str[39],p_auth_tx->MCF01_details.name02,20);
      }
   }
   else if ((p_auth_tx->TLF01_details.tx_key == AUTH_FEE_COLLECTION_ACQUIRER)   ||
            (p_auth_tx->TLF01_details.tx_key == AUTH_FUNDS_DISBURSEMENT_ACQUIRER))
   {
      /* Usage 5 */
      memcpy( temp_str,     p_auth_tx->MCF01_details.name01, 30 );
      memcpy(&temp_str[30], p_auth_tx->MCF01_details.name02, 20 );
   }

   HOST_MESSAGE.addl_data_len[0] = strlen(temp_str );
   memcpy( HOST_MESSAGE.addl_data, &temp_str,
           HOST_MESSAGE.addl_data_len[0] );

   /* END OF FIELD 48 */

   /* FIELD 49: TRANSACTION CURRENCY CODE */
   memset(temp_str, 0, sizeof(temp_str));
   if ( p_auth_tx->TLF01_details.tx_key > 100 )
   {
      if ( p_auth_tx->MCF01_details.region_nbr[0] != 0x00 )
         memcpy( temp_str, p_auth_tx->MCF01_details.region_nbr, 3 );
      else
         strcpy( temp_str, p_auth_tx->TLF01_details.currency_code );
   }
   else
      strcpy (temp_str, p_auth_tx->TLF01_details.currency_code);     

   Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.tran_curr_cd)*2);
   asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.tran_curr_cd)*2, HOST_MESSAGE.tran_curr_cd);

   /* FIELD 50 currency code  settlement*/
	if (p_auth_tx->TLF01_details.tx_key == AUTH_RECONCILIATION_RESPONSE)  
	{ /* Copy from the request */
		
		asc_to_bcd (p_auth_tx->TLF01_details.product_codes[5].code, sizeof(HOST_MESSAGE.settle_cur_cd)*2, HOST_MESSAGE.settle_cur_cd);

	}

   /* FIELD 51: CARDHOLDER BILLING CURRENCY CODE */
	if (p_auth_tx->ch_billing_curr_code [0] != '\0')
   {
      memset(temp_str, 0, sizeof(temp_str));
      strcpy (temp_str, p_auth_tx->ch_billing_curr_code);     
      Rj_with_lead_zeros (temp_str, sizeof(HOST_MESSAGE.card_curr_cd)*2);
      asc_to_bcd (temp_str, sizeof(HOST_MESSAGE.card_curr_cd)*2, HOST_MESSAGE.card_curr_cd);
   }

   /* FIELD 52: PIN BLOCK */
   if (true == hhutil_verify_pin_block(p_auth_tx))
   {
      memset(tempstr, 0, sizeof(tempstr));
      ascii_to_bin( p_auth_tx->TLF01_details.pin_block, strlen(p_auth_tx->TLF01_details.pin_block),
                    &tempstr[0], sizeof(tempstr));
      memcpy (HOST_MESSAGE.pin_block, tempstr, sizeof(HOST_MESSAGE.pin_block));
   }

   /* FIELD 53: SECURITY RELATED CONTROL INFORMATION */
   if (true == hhutil_verify_pin_block(p_auth_tx))
   {
      memset( tempstr, 0x00, sizeof(tempstr) );

      strcpy( tempstr, "200101"        ); /* Subfields 1-3 */
      strcat( tempstr, PinZoneKeyIndex ); /* Subfield  4   */
      strcat( tempstr, "00000000"      ); /* Subfields 5-6 */

      asc_to_bcd( tempstr,
                  sizeof(HOST_MESSAGE.sec_cntl)*2,
                  HOST_MESSAGE.sec_cntl );
   }

   /* FIELD 54: ADDITIONAL AMOUNT */

   /* This field is only for ATMs. */
   if ( p_auth_tx->additional_pos_information[0] == ATM_TERMINAL )
   {
      /* Only return balance if approved. */
      if ( 0 == strcmp(p_auth_tx->TLF01_details.response_code, APPROVED) )
      {
         /* We will return balance. Must return it in Issuer's currency.
          * That means if txn is Peso, set currency code (cc) to 608.
          * If txn is USD, set cc to 840.  For third currencies, use the
          * the account from ACF01. Default to USD, as that is what it
          * should be for a third currency.
          */
         memset( temp_currency_code, 0x00, sizeof(temp_currency_code) );
         if ( p_auth_tx->host2_src_fi_nbr[0] != 0x00 )
         {
            strcpy( temp_currency_code, p_auth_tx->host2_src_fi_nbr );
         }
         else if ( 0 == strcmp(PESO,p_auth_tx->TLF01_details.currency_code) )
            strcpy( temp_currency_code, p_auth_tx->TLF01_details.currency_code);

         else if ( 0 == strcmp(USD,p_auth_tx->TLF01_details.currency_code) )
            strcpy( temp_currency_code, p_auth_tx->TLF01_details.currency_code);

         else
         {
            acct_type = p_auth_tx->ACF01_details.primary_key.account_type[1];
            if ( acct_type == DOLLAR_ACCOUNT_TYPE )
               strcpy( temp_currency_code, USD );

            else if ( acct_type == PESO_ACCOUNT_TYPE )
               strcpy( temp_currency_code, PESO );

            else
               strcpy( temp_currency_code, USD );
         }

         count = 0;
         memset(new_string, 0, sizeof(new_string));
         if(0 != strlen(p_auth_tx->ACF01_details.cash_available))
         {
            memcpy( new_string+count,
                    p_auth_tx->TLF01_details.processing_code+2,
                    2 );
            count = count + 2;
            memcpy(new_string+count, "01", 2);
            count = count + 2;
            memcpy( new_string+count, temp_currency_code, 3 );
            count = count + 3;
            if ('-' == p_auth_tx->current_balance_sign)
            {
               memcpy(new_string+count, "D" , 1);
            }
            else
            {
               memcpy(new_string+count, "C" , 1);
            }
            count++;
            strcpy( temp_str, p_auth_tx->ACF01_details.cash_available );
            Rj_with_lead_zeros (temp_str, 12);
            memcpy(new_string+count, temp_str, 12);
            count = count + 12;
         }
         if (0 != strlen(p_auth_tx->ACF01_details.cash_limit))
         {
            memcpy( new_string+count,
                    p_auth_tx->TLF01_details.processing_code+2,
                    2 );
            count = count + 2;
            memcpy(new_string+count, "02", 2);
            count = count + 2;
            memcpy(new_string+count, temp_currency_code, 3);
            count = count + 3;
            if ('-' == p_auth_tx->available_balance_sign)
            {
               memcpy(new_string+count, "D" , 1);
            }
            else
            {
               memcpy(new_string+count, "C" , 1);
            }
            count++;
            strcpy (temp_str,p_auth_tx->ACF01_details.cash_limit); 
            Rj_with_lead_zeros (temp_str, 12);
            memcpy(new_string+count, temp_str, 12);
            count = count + 12;
         }
         memcpy( HOST_MESSAGE.addlamts,
                 new_string,
                 sizeof(HOST_MESSAGE.addlamts));
         HOST_MESSAGE.addl_len[0] = strlen(HOST_MESSAGE.addlamts);
      }
   }

   /* FIELD DE55 EMV CHIP DATA*/

   if(emv_txn == true)
	   {
		   if (p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)	
		   {//Resposne
			   
			   if(strcmp(p_auth_tx->TLF01_details.product_codes[1].code,STANDIN)==0 )
			   {
				   emv_len= p_auth_tx->EMV_details.future_field1[0];   
				   if(emv_len > 0  && emv_len < 255)
				   {   memcpy(HOST_MESSAGE.ICCdata_len,p_auth_tx->EMV_details.future_field1,emv_len+1);
					   memcpy(HOST_MESSAGE.ICCdata,p_auth_tx->EMV_details.future_field1+1,emv_len);
				   }
			   }
			   else
			   {
				   emv_len= p_auth_tx->EMV_details.emv_block[0];   
				   if(emv_len > 0 && emv_len < 255 )
				   {   memcpy(HOST_MESSAGE.ICCdata_len,p_auth_tx->EMV_details.emv_block,emv_len+1);
					   memcpy(HOST_MESSAGE.ICCdata,p_auth_tx->EMV_details.emv_block+1,emv_len);
				   }
			   }
		   }
		   else
		   {//Request
			   CHAR temp_emv_data[256]={0};
			   emv_len = p_auth_tx->EMV_details.emv_block[0];
			    if(strncmp(p_auth_tx->TLF01_details.handler_queue,"nceqit",5)== 0  ||
			       strncmp(p_auth_tx->TLF01_details.handler_queue,"neqit2",5)== 0)
				{
					if(emv_len > 0 && emv_len < 255 )
				   {   memcpy(HOST_MESSAGE.ICCdata_len,p_auth_tx->EMV_details.emv_block,emv_len+1);
					   memcpy(HOST_MESSAGE.ICCdata,p_auth_tx->EMV_details.emv_block+1,emv_len);
				   }	
			   	}else 
			   {
			   		memcpy(temp_emv_data,p_auth_tx->EMV_details.emv_block,sizeof(p_auth_tx->EMV_details.emv_block));
			   		generate_VISA_EMV_data(emv_len,temp_emv_data);
			   		}
		   }
	   }


   /* FIELD 60: ADDITIONAL POS INFORMATION */
   if ( emv_txn == true )
	 {
		memset( tempstr, 0x00, sizeof(tempstr) );
		strcpy( tempstr, "25000010" );
		
		   
		HOST_MESSAGE.priv60_len[0] = (strlen(tempstr)+ 1) / 2;
		str_asc_to_bcd( tempstr, sizeof(HOST_MESSAGE.priv60)*2,
						HOST_MESSAGE.priv60);
   
		/* Store here for Fraudguard. */
		memcpy( p_auth_tx->TLF01_details.nfi_name, tempstr,
				sizeof(p_auth_tx->TLF01_details.nfi_name)-1 );

		strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,
				tempstr,sizeof(p_auth_tx->TLF01_details.product_codes[16].amount));
   }
   else if(0 == strncmp(p_auth_tx->TLF01_details.pos_entry_mode,"002",3))
   {
		memset( tempstr, 0x00, sizeof(tempstr) );
		strcpy( tempstr, "25000010002" );
		HOST_MESSAGE.priv60_len[0] = (strlen(tempstr)+ 1) / 2;
	    str_asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.priv60)*2,
	                  HOST_MESSAGE.priv60);
		strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,
				tempstr,sizeof(p_auth_tx->TLF01_details.product_codes[16].amount));
	 }
   else if ('Y' != p_auth_tx->TLF01_details.voice_auth[0])
   {
      memset(p_auth_tx->additional_pos_information, 0, sizeof(p_auth_tx->additional_pos_information));

      if ( RPS_flag == 1 ) //RPS
      {
         strcpy(p_auth_tx->additional_pos_information,"5000080002");
      }
      else if ( iIndustry == ECOMMERCE ) //E-Commerce
      {
        strcpy(p_auth_tx->additional_pos_information,"0000000007");
      }
      else if (0 == strcmp(p_auth_tx->TLF01_details.terminal_type,"01"))    /* TERMINAL TYPE */
      {
         p_auth_tx->additional_pos_information[0] = '3';   /* Unattended gas pump */
      }
      else if ( iIndustry == ATM ) 
      {
         p_auth_tx->additional_pos_information[0] = ATM_TERMINAL;   /* ATM */
      }
      else
      {
         p_auth_tx->additional_pos_information[0] = '0';   /* Unspecified */
      }

      /* Ignore the following for Ecommerce. */
      if ( iIndustry != ECOMMERCE )
      {
         if (p_auth_tx->TLF01_details.pos_entry_mode[2] == '2')
         {

            /* Magnetic stripe read capability */
            p_auth_tx->additional_pos_information[1] = '2';
         }
         else if ( iIndustry == ATM ) /* ATM */
         {
            /*  magnetic stripe read capability */
            p_auth_tx->additional_pos_information[1] = '2';
         }
         else if (p_auth_tx->TLF01_details.pos_entry_mode[2] == '1')
         {
            /* Magnetic stripe read capability */
            p_auth_tx->additional_pos_information[1] = '2';
         }
         else 
         {
            /* No magnetic stripe read capability */
            p_auth_tx->additional_pos_information[1] = '1';
         }
      }

	    /* Terminal has Chip Read capability. */
        // p_auth_tx->additional_pos_information[1] = '5';
		 if ( emv_txn == true )
		 {p_auth_tx->additional_pos_information[6] = '1';
		  p_auth_tx->additional_pos_information[1] = '5';
		 }
	  	//Fall back case, if fall back, put 60.3 = 2 or 6
	  	if(strncmp(p_auth_tx->TLF01_details.pos_entry_mode,"080",3) == 0)
		{	p_auth_tx->additional_pos_information[2] = '2';
			p_auth_tx->additional_pos_information[3] = '0';
			p_auth_tx->additional_pos_information[1] = '5';
	  
	  	}
      asc_to_vbcd (p_auth_tx->additional_pos_information,
                   strlen(p_auth_tx->additional_pos_information),
                   HOST_MESSAGE.priv60);
      HOST_MESSAGE.priv60_len[0] = (strlen(p_auth_tx->additional_pos_information)/2);

      strncpy(p_auth_tx->TLF01_details.product_codes[16].amount,
    		  p_auth_tx->additional_pos_information,sizeof(p_auth_tx->additional_pos_information));
   }


   /* FIELD 61: OTHER AMOUNTS */
   if (p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)  
   {  /* response */
	   asc_to_vbcd (p_auth_tx->TLF01_details.add_amounts, sizeof(HOST_MESSAGE.priv61)*2, HOST_MESSAGE.priv61);
	   HOST_MESSAGE.priv61_len[0] = strlen(p_auth_tx->TLF01_details.add_amounts);
   }
   else
   {
 	   asc_to_vbcd (p_auth_tx->TLF01_details.actual_amount, sizeof(HOST_MESSAGE.priv61)*2, HOST_MESSAGE.priv61);
   	   i = strlen(p_auth_tx->TLF01_details.actual_amount);
       if ( i < 12  ||  i > 36 )
          memset( &HOST_MESSAGE.priv61_len, 0x00, sizeof(HOST_MESSAGE.priv61_len) );
       else
       {
          /* Length is for BCD string, so divide by 2. */
          HOST_MESSAGE.priv61_len[0] = (strlen(p_auth_tx->TLF01_details.actual_amount)) / 2;
       }
   }
   
   /* FIELD 62: CUSTOM PAYMENT SERVICE (CPS) FIELDS */
   if (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID)  
   {
      if ( iIndustry == ATM )
      {
         /*+---------------------------------+
           | This field is in bitmap format: |
           |    Length:    1 byte            |
           |    Bit map:   8 bytes           |
           |    Field 1:   1 byte   EBCDIC   |
           |    Field 4:   1 byte   EBCDIC   |
           |    Field 5:   1 byte   BCD      |
           |                                 |
           | Field 1 is always set.          |
           | Fields 4 and 5 are set for Ind  |
           | codes 300 and 400 only.         |
           +---------------------------------+*/

         /* Initialize bitmap and field length. */
         memset( F62BitMap, 0x00, 8 );
         F62Len = 8;   /* Length of bitmap */

         if ( p_auth_tx->TLF01_details.message_type[1] != '4' )
         {
            /* Not a reversal */

            /* FIELD 62.1 - AUTH CHARACTERISTICS INDICATOR */
            i = 1;
            if(p_auth_tx->TLF01_details.visa_char_indicator == 0x00)
               HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( 'Y', A2E );

            else if ( (p_auth_tx->TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT) ||
                      (p_auth_tx->TLF01_details.tx_key == AUTH_REPRESENTMENT)  )
            {
               if ( p_auth_tx->TLF01_details.visa_char_indicator == 'E' )
               {
                  /* Only send this field if E for these 2 txns types. */
                  HOST_MESSAGE.priv62[F62Len] = 
                   ascii_ebcdic_conv( p_auth_tx->TLF01_details.visa_char_indicator,
                                      A2E );
               }
               else
                  i = 0;
            }
            else
               HOST_MESSAGE.priv62[F62Len] = 
                ascii_ebcdic_conv( p_auth_tx->TLF01_details.visa_char_indicator,
                                   A2E );

            if ( i == 1 )
            {
               F62BitMap[0] |= 0x80;      /* Set bit 1 */
               F62Len += 1;
            }

            /* FIELD 62.2 - VISA TRANSACTION ID */
            if(p_auth_tx->TLF01_details.visa_tran_id[0] !=  0x00)
            {
               memset(visa_tran, 0x00, sizeof(visa_tran) );
               memset(visa_tran_bcd, 0x00, sizeof(visa_tran_bcd) );

               visa_tran[0] = '0'; /* Right justify the tran id. */
               memcpy(visa_tran+1, p_auth_tx->TLF01_details.visa_tran_id,15);

               F62BitMap[0] |= 0x40;     /* Set bit 2 */
               asc_to_bcd( visa_tran, strlen(visa_tran), visa_tran_bcd );
               memcpy( HOST_MESSAGE.priv62+F62Len, visa_tran_bcd, 8 );
               F62Len += 8;
            }
         }
         else
         {
            /* This is a reversal. Echo 62.1 and 62.2 - except for timeouts.
             * These are stored in a combination of mcard_banknet and invoice.
             * The first byte of mcard_banknet is 62.1, followed by 11 bytes
             * of 62.2.  The last 4 bytes of 62.2 are in invoice number.  This
             * was done in order to store this data into NCF30 (network_data and
             * invoice_nbr without requiring a database change.  There was no
             * available field in NCF30 large enough to hold 62.2
             *
             * Note 1:  Only send 62.1 if it is a valid, positive value.  It
             * cannot be 'N' or blank.  Valid values are: A E V C M I P.
             *
             * Note 2:  Only send 62.2 if it exists.  If reversal is due to a
             * late response from Visa (time out), it will not exist.
             */
            if ( 0 != strcmp(p_auth_tx->TLF01_details.response_code,"TO") )
            {
               /* This is not a reversal caused by a time out. Send DE62. */
               visa_char = p_auth_tx->TLF01_details.mcard_banknet[0];

               if ((visa_char != 'N') && (visa_char != 0x00) && (visa_char != ' '))
               {
                  HOST_MESSAGE.priv62[F62Len] = ascii_ebcdic_conv( visa_char,A2E );
                  F62BitMap[0] |= 0x80;      /* Set bit 1 */
                  F62Len += 1;
               }

               if ( p_auth_tx->TLF01_details.mcard_banknet[1] != 0x00 )
               {
                  memset(visa_tran, 0x00, sizeof(visa_tran) );
                  memset(visa_tran_bcd, 0x00, sizeof(visa_tran_bcd) );

                  visa_tran[0] = '0'; /* Right justify the tran id. */
                  memcpy(visa_tran+1, p_auth_tx->TLF01_details.mcard_banknet+1,11);
                  memcpy(visa_tran+12,p_auth_tx->TLF01_details.invoice_number,  4);

                  F62BitMap[0] |= 0x40;     /* Set bit 2 */
                  asc_to_bcd( visa_tran, strlen(visa_tran), visa_tran_bcd );
                  memcpy( HOST_MESSAGE.priv62+F62Len, visa_tran_bcd, 8 );
                  F62Len += 8;
               }
            }
         }
         /* send de62.27 if present*/
         if( p_auth_tx->TLF01_details.product_codes[8].amount[0] != '\0' )
         {
        	 strncpy(DE62_27,p_auth_tx->TLF01_details.product_codes[8].amount,7);
        	 convert_to_ebcdic( DE62_27,DE62_27_ebcidic, 7 );
        	 memcpy( HOST_MESSAGE.priv62+F62Len, DE62_27_ebcidic, 7 );
        	 F62Len += 7;
        	 F62BitMap[3] |= 0x20;
         }
         if ( F62Len > 8 )
         {
            memcpy( &HOST_MESSAGE.priv62, F62BitMap, 8 );
            HOST_MESSAGE.priv62_len[0] = F62Len;
         }
      }
   }
   else
   {  /* response */
      memcpy( &HOST_MESSAGE.priv62,
              p_auth_tx->int_net_facility_data,
              sizeof(HOST_MESSAGE.priv62 ));
      memcpy( HOST_MESSAGE.priv62_len,
              p_auth_tx->int_net_facility_len,
              sizeof(HOST_MESSAGE.priv62_len ));
   }

   /* FIELD 63: SMS Private-Use Fields FIELDS */
   if (p_auth_tx->TLF01_details.tx_key >= AUTH_SALE)  
   {
      /*+---------------------------------+
        | This field is in bitmap format: |
        |    Length:    1 byte            |
        |    Bit map:   3 bytes           |
        |    Field 1:   2 byte   BCD      |
        |    Field 3:   2 byte   BCD      |
        |    Field 5:   3 byte   BCD      |
        |                                 |
        | Field 1 is always set.          |
        | Fields 5 are set for Ind        |
        | codes 300 and 400 only.         |
        +---------------------------------+*/

      /* Initialize bitmap and field length. */
      memset( F63BitMap, 0x00, 3 );
      F63Len = 3;   /* Length of bitmap */

      if ( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE)       ||
           (p_auth_tx->TLF01_details.tx_key == AUTH_FEE_COLLECTION_ISSUER_RESPONSE) ||
           (p_auth_tx->TLF01_details.tx_key == AUTH_FUNDS_DISBURSEMENT_ISSUER_RESPONSE) )
      {
         /* Echo field 63.1 only. */
         asc_to_bcd( p_auth_tx->MCF01_details.vip_code,
                     4, &HOST_MESSAGE.priv63[F63Len] );

         F63BitMap[0] |= 0x80;      /* Set bit 1 */
         F63Len += 2;
      }
      else
      {
         if ( p_auth_tx->TLF01_details.tx_key == AUTH_LOGON || 
              p_auth_tx->TLF01_details.tx_key == AUTH_KEY_CHANGE ||
              p_auth_tx->TLF01_details.tx_key == AUTH_LOGON_RESPONSE ||
			     p_auth_tx->TLF01_details.tx_key == AUTH_KEY_CHANGE_RESPONSE )
         {
            asc_to_bcd( "0002", 2, &HOST_MESSAGE.priv63[F63Len] );
            F63BitMap[0] |= 0x80;      /* Set bit 1 */
            F63Len += 2;
         }

         else if ( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL) ||
		 			(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE) ||
		 			(p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_REPEAT) ||
                   (p_auth_tx->TLF01_details.tx_key == AUTH_CASH_DISBURSEMENT_ADJUSTMENT))
         {
            memset( tempstr, 0x00, sizeof(tempstr) );
			if(strncmp(p_auth_tx->TLF01_details.handler_queue,"nceqit",5)== 0 ||
				strncmp(p_auth_tx->TLF01_details.handler_queue,"neqit",5)== 0)
			   strcpy(tempstr,"0000");
            else if ( strlen(p_auth_tx->TLF01_details.invoice_number) > 4 )
               memcpy( tempstr, p_auth_tx->TLF01_details.invoice_number+4, 4 );
            else
               memcpy( tempstr, p_auth_tx->TLF01_details.invoice_number,   4 );

            asc_to_bcd( tempstr, 4, &HOST_MESSAGE.priv63[F63Len] );
            F63BitMap[0] |= 0x80;      /* Set bit 1 */
            F63Len += 2;
         }

         else if (p_auth_tx->MCF01_details.vip_code[0] == 0x00)
         {
            asc_to_bcd( "0000", 2, &HOST_MESSAGE.priv63[F63Len] );
            F63BitMap[0] |= 0x80;      /* Set bit 1 */
            F63Len += 2;
         }

         else if (p_auth_tx->MCF01_details.vip_code[0] != 0x00)
         {
            /* previously save value*/
            asc_to_bcd( p_auth_tx->MCF01_details.vip_code,
                        4, &HOST_MESSAGE.priv63[F63Len] );
            F63BitMap[0] |= 0x80;      /* Set bit 1 */
            F63Len += 2;
         }

         /* Plus Proprietary Member Center ID field 63.3 */
         if (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL)
         {
            asc_to_bcd( "2502", 4, &HOST_MESSAGE.priv63[F63Len] );
            F63BitMap[0] |= 0x20;      /* Set bit 3 */
            F63Len += 2;
         }
         else if (p_auth_tx->TLF01_details.tx_key == AUTH_CASH_DISBURSEMENT_ADJUSTMENT)
         {
            asc_to_bcd( "2002", 4, &HOST_MESSAGE.priv63[F63Len] );
            F63BitMap[0] |= 0x20;      /* Set bit 3 */
            F63Len += 2;
         }

         else if (p_auth_tx->TLF01_details.tx_key == AUTH_FEE_COLLECTION_ACQUIRER || 
                  p_auth_tx->TLF01_details.tx_key == AUTH_FUNDS_DISBURSEMENT_ACQUIRER ||
                  p_auth_tx->TLF01_details.tx_key == AUTH_BACKOFFICE_ADJUSTMENT   ||
                  p_auth_tx->TLF01_details.tx_key == AUTH_REPRESENTMENT   )
         {
            asc_to_bcd( p_auth_tx->MCF01_details.max_num_returns,
                        4, &HOST_MESSAGE.priv63[F63Len] );
            F63BitMap[0] |= 0x20;      /* Set bit 3 */
            F63Len += 2;
         }

         /* Plus Proprietary Member Center ID field 63.5 */
         if (p_auth_tx->TLF01_details.product_codes[6].amount[0] != 0x00)
         {
            asc_to_bcd( p_auth_tx->TLF01_details.product_codes[6].amount,
                        6, &HOST_MESSAGE.priv63[F63Len] );
            F63BitMap[0] |= 0x08;      /* Set bit 5 */
            F63Len += 3;
         }

         /* 63.13 Decimal Positions Indicator send if present for all
          * switched out transactions to visa sms except bal inquiry
          */
         if (p_auth_tx->TLF01_details.tx_key < AUTH_INVALID && 
             p_auth_tx->TLF01_details.tx_key != AUTH_BALANCE_INQUIRY  )
         {
            if (p_auth_tx->MCF01_details.deferred_start_date[0] != 0x00)
            {
               asc_to_bcd( p_auth_tx->MCF01_details.deferred_start_date,
                           6, &HOST_MESSAGE.priv63[F63Len] );
               F63BitMap[1] |= 0x08;      /* Set bit 13 */
               F63Len += 3;
            }
         }
      }
      memcpy( &HOST_MESSAGE.priv63, F63BitMap, 3 );
      HOST_MESSAGE.priv63_len[0] = F63Len;
   }

   /* FIELD 66: Settlement Codes */

   if ( (p_auth_tx->TLF01_details.tx_key == AUTH_RECONCILIATION) ||
        (p_auth_tx->TLF01_details.tx_key == AUTH_RECONCILIATION_RESPONSE) )
   {
      /*+------------------------------------------------+
        | 1 = In balance                                 |
        | 2 = Out of balance                             |
        | 9 = Acknowledgement without reconciliation     |
        |                                                |
        | We will always send back '9' in the response.  |
        | We are not reconciling, just replying. So this |
        | also means we do not need to send back fields  |
        | 74-77, 86-89, and 97.                          |
        +------------------------------------------------+*/

      asc_to_bcd( "09", 2, HOST_MESSAGE.setl_code );
   }

	/* Field 68 Receiving Institution Country Code */
	if (p_auth_tx->TLF01_details.product_codes[8].code[0] != 0x00)
	{

		asc_to_bcd( p_auth_tx->TLF01_details.product_codes[8].code, sizeof(HOST_MESSAGE.rcv_Inst_cnty_cd), HOST_MESSAGE.rcv_Inst_cnty_cd);
	}

   /* Field 69 Settlement Institution Country Code */
   asc_to_bcd( p_auth_tx->TLF01_details.product_codes[6].code, 4, HOST_MESSAGE.set_Inst_cnty_cd );

   /* FIELD 70: Network Management Info Code */
   if(p_auth_tx->TLF01_details.tx_key == AUTH_ADMINISTRATIVE_TEXT && 
      p_auth_tx->TLF01_details.service_code[0]  != 0x00)
   {
      if ( 1 == (strlen(p_auth_tx->TLF01_details.service_code) % 2) )
      {
         memset( temp_str, 0, sizeof(temp_str) );
         temp_str[0] = '0';
         memcpy( &temp_str[1],
                 p_auth_tx->TLF01_details.service_code,
                 strlen(p_auth_tx->TLF01_details.service_code) );
         str_asc_to_bcd( temp_str, strlen(temp_str), HOST_MESSAGE.nmi_code );
      }
      else
      {
         str_asc_to_bcd( p_auth_tx->TLF01_details.service_code,
                         sizeof(HOST_MESSAGE.nmi_code)*2,
                         HOST_MESSAGE.nmi_code );
      }
   }
   else if (0 == strlen(vLogon_NMI_Code))  
   {
      asc_to_bcd (p_auth_tx->function_code, sizeof(HOST_MESSAGE.nmi_code)*2,
                  HOST_MESSAGE.nmi_code);  
   }
   else
   {
      asc_to_bcd (vLogon_NMI_Code, sizeof(HOST_MESSAGE.nmi_code)*2,
               HOST_MESSAGE.nmi_code);  
   }

   /***************************************************************
    *                                                             *
    * No need to send the reconciliation fields because DE66 = 9. *
    *                                                             *
    ***************************************************************/
   /* FIELD 74 Credit NUMBER *
   memset(tempstr, 0, sizeof(tempstr));
   strcpy(tempstr, p_auth_tx->TLF01_details.product_codes[10].amount);
   Rj_with_lead_zeros (tempstr, sizeof(HOST_MESSAGE.credit_num)*2);
   asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.credit_num)*2,HOST_MESSAGE.credit_num);  

   * FIELD 75 Credits, Reversal Number *
   memset(tempstr, 0, sizeof(tempstr));
   strcpy(tempstr,p_auth_tx->TLF01_details.product_codes[11].amount);
   Rj_with_lead_zeros (tempstr, sizeof(HOST_MESSAGE.credit_rev_num)*2);
   asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.credit_rev_num)*2,
               HOST_MESSAGE.credit_rev_num);  

   * FIELD 76 Debits, Number *
   memset(tempstr, 0, sizeof(tempstr));
   strcpy(tempstr,p_auth_tx->TLF01_details.product_codes[12].amount);
   Rj_with_lead_zeros (tempstr, sizeof(HOST_MESSAGE.debit_num)*2);
   asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.debit_num)*2,
               HOST_MESSAGE.debit_num);  

   * FIELD 77 Debits, Reversal Number *
   memset(tempstr, 0, sizeof(tempstr));
   strcpy(tempstr,p_auth_tx->TLF01_details.product_codes[13].amount);
   Rj_with_lead_zeros (tempstr, sizeof(HOST_MESSAGE.debit_rev_num)*2);
   asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.debit_rev_num)*2,
               HOST_MESSAGE.debit_rev_num);  

   * FIELD 86 Credits, Amount *
   memset(tempstr, 0, sizeof(tempstr));
   strcpy(tempstr,p_auth_tx->TLF01_details.product_codes[0].amount);
   Rj_with_lead_zeros (tempstr, sizeof(HOST_MESSAGE.credit_amt)*2);
   asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.credit_amt)*2,
               HOST_MESSAGE.credit_amt);  

   * FIELD 87 Credits, Reversal Amount *
   memset(tempstr, 0, sizeof(tempstr));
   strcpy(tempstr,p_auth_tx->TLF01_details.product_codes[1].amount);
   Rj_with_lead_zeros (tempstr, sizeof(HOST_MESSAGE.credit_rev_amt)*2);
   asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.credit_rev_amt)*2,
               HOST_MESSAGE.credit_rev_amt);  

   * FIELD 88 Debits, Amount *
   memset(tempstr, 0, sizeof(tempstr));
   strcpy(tempstr,p_auth_tx->TLF01_details.product_codes[2].amount);
   Rj_with_lead_zeros (tempstr, sizeof(HOST_MESSAGE.debit_amt)*2);
   asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.debit_amt)*2,
               HOST_MESSAGE.debit_amt);  

   * FIELD 89 Debits, Reversal Amount *
   memset(tempstr, 0, sizeof(tempstr));
   strcpy(tempstr,p_auth_tx->TLF01_details.product_codes[3].amount);
   Rj_with_lead_zeros (tempstr, sizeof(HOST_MESSAGE.debit_rev_amt)*2);
   asc_to_bcd (tempstr, sizeof(HOST_MESSAGE.debit_rev_amt)*2,
               HOST_MESSAGE.debit_rev_amt);  


   /* FIELD 90: Get original data elements for field 90. */
   if( (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE)        || 
       (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_ADVICE_RESPONSE) ||
       (p_auth_tx->TLF01_details.tx_key == AUTH_CHARGEBACK_RESPONSE)      ||
       (p_auth_tx->TLF01_details.tx_key == AUTH_CHARGEBACK_REVERSAL_RESPONSE ))
   {
      //ECHO FLD 90 Data
      memset (orig_data, 0, sizeof(orig_data));
      strcat (orig_data, p_auth_tx->TLF01_details.orig_message);

      /* If orig stan came is as all zeroes, it will have been overwritten.
       * Check flag to see if this happened.  If yes, echo out zeroes.
       */
      if ( p_auth_tx->host_processing_code[3] == '0' )
         strcpy( p_auth_tx->TLF01_details.orig_sys_trace_num, "000000" );

      strcat (orig_data, p_auth_tx->TLF01_details.orig_sys_trace_num);
      strcat (orig_data, p_auth_tx->orig_local_time_date);  

      for (count = 1; count <= (11-strlen(p_auth_tx->orig_acq_inst_id)); count++)
         strcat(orig_data, "0");

      strcat (orig_data, p_auth_tx->orig_acq_inst_id);
      for (count = 1; count <= (11-strlen(p_auth_tx->amex_pos_entry_mode)); count++)
         strcat(orig_data, "0");

      strcat (orig_data, p_auth_tx->amex_pos_entry_mode);
   }
   else
   {
      memset (orig_data, 0, sizeof(orig_data));
      strcpy (orig_data, "0200");
      strcat (orig_data, p_auth_tx->TLF01_details.orig_sys_trace_num);

      strcat (orig_data, p_auth_tx->orig_local_time_date);

      for (count = 1; count <= (11-strlen(p_auth_tx->orig_acq_inst_id)); count++)
         strcat(orig_data, "0");
      strcat (orig_data, p_auth_tx->orig_acq_inst_id);

      /* Forward institution id code : if present
       * Fill with zeros, if Forward institution id code is null.
       * Since we are never sending out DE33, this is always zeroes.
       */
      strncat( orig_data, "00000000000", 11 );
   }
   str_asc_to_bcd (orig_data, strlen(orig_data), HOST_MESSAGE.orig_data );

   /* FIELD 93 RESPONSE INDICATOR */
   memcpy( HOST_MESSAGE.resp_ind, vLogon_Resp_Indicator, sizeof(HOST_MESSAGE.resp_ind ));

   /* FIELD 95 REPLACEMENT AMOUNTS */

   /* This field is used only for partial reversals. */
   if ( p_auth_tx->TLF01_details.message_type[1] == '4' )
   {
      if(  (atol(p_auth_tx->TLF01_details.actual_amount) == 0) &&
           ((p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL) ||
            (p_auth_tx->TLF01_details.tx_key == AUTH_REVERSAL_REPEAT)) )
      {
          //don't send fld95
      }
      else if ( p_auth_tx->TLF01_details.tx_key > 100 )
      {
         /* This is a response.  Echo DE95 if it was in the request. */
         if ( p_auth_tx->MCF01_details.totals_ind[0] == 'E' )
         {
            memcpy( HOST_MESSAGE.repl_amts,
                    p_auth_tx->MCF01_details.address01,
                    30 );
            memcpy( HOST_MESSAGE.repl_amts+30,
                    p_auth_tx->MCF01_details.address02,
                    12 );
         }
      }
      else
      {
         total_amount    = atol(p_auth_tx->TLF01_details.total_amount);
         reversal_amount = atol(p_auth_tx->TLF01_details.reversal_amount);
         if ( (reversal_amount < total_amount) &&
              (reversal_amount > 0           )  )
         {
            memset( new_string, 0x00, sizeof(new_string) );
            sprintf( new_string, "%012d", total_amount-reversal_amount );
            for (count = 12; count < 42; count++)
               new_string[count] = '0';    /* zero fill the last 30+ digits */

            memcpy (HOST_MESSAGE.repl_amts, new_string,
                    sizeof(HOST_MESSAGE.repl_amts));
         }
      }
   }

	/* FIELD 97 Amount, Net Settlement */

   /* This field is not needed in 0530 because DE66 = 9. *
   memset( tempstr, 0x00, sizeof(tempstr) );
   strcpy(tempstr,p_auth_tx->TLF01_details.settlement_total);
   Rj_with_lead_zeros (tempstr, sizeof(HOST_MESSAGE.net_setl_amount));

   memcpy (HOST_MESSAGE.net_setl_amount, p_auth_tx->TLF01_details.product_codes[4].code, 1);
   memcpy (HOST_MESSAGE.net_setl_amount +1 , tempstr + 1, sizeof(HOST_MESSAGE.net_setl_amount) );
   */	
	
	/* FIELD 99: SETTLEMENT INSTITUTION ID CODE */
	if (p_auth_tx->TLF01_details.product_codes[5].amount[0] != 0x00) 
	{
		/*  use the code from the incoming message*/
		HOST_MESSAGE.setl_id_len[0] = strlen(p_auth_tx->TLF01_details.product_codes[5].amount );		
		asc_to_bcd (p_auth_tx->TLF01_details.product_codes[5].amount, HOST_MESSAGE.setl_id_len[0],
               HOST_MESSAGE.setl_id);  		
	}
	
   /* FIELD 100 Receiving Institution Identification Code */
   if ( p_auth_tx->receiving_inst_id_code[0] != 0x00 )
   {
      i = false;
      if ((p_auth_tx->TLF01_details.tx_key == AUTH_FEE_COLLECTION_ACQUIRER) ||
          (p_auth_tx->TLF01_details.tx_key == AUTH_FUNDS_DISBURSEMENT_ACQUIRER))
      {
         if ( HOST_MESSAGE.pan_len[0] == 0x00 )
         {
            /* If Fee Collection or Funds Disbursement,
             * only use DE100 if DE2 is not present.
             */
            i = true;
         }
      }
      else
         i = true;

      if ( i == true )
      {
         HOST_MESSAGE.recv_inst_len[0] = strlen(p_auth_tx->receiving_inst_id_code);

         if ( (HOST_MESSAGE.recv_inst_len[0] % 2) == 1 )
         {
            memset (temp_str, 0, sizeof(temp_str));
            temp_str[0] = '0';
            memcpy( &temp_str[1],
                    p_auth_tx->receiving_inst_id_code,
                    HOST_MESSAGE.recv_inst_len[0] );
            asc_to_bcd (temp_str, HOST_MESSAGE.recv_inst_len[0] + 1,
                        HOST_MESSAGE.recv_inst_id);  	
         }
         else
            asc_to_bcd( p_auth_tx->receiving_inst_id_code,
                        HOST_MESSAGE.recv_inst_len[0],
                        HOST_MESSAGE.recv_inst_id);
      }
   }

   /* FIELD 102: ACCOUNT ID 1 */
   if ( p_auth_tx->acct_id_1[0] != 0x00 )
   {
      if ( p_auth_tx->TLF01_details.tx_key > AUTH_INVALID )
      {
         /* Response */
         if ( p_auth_tx->FAF01_details.velocity[0].amount[0] == '1' )
         {
            /* This flag indicates that this field came in the request. */
            HOST_MESSAGE.acct_id_1_len[0] = strlen(p_auth_tx->acct_id_1 );
            memcpy( HOST_MESSAGE.acct_id_1,
                    p_auth_tx->acct_id_1,
                    HOST_MESSAGE.acct_id_1_len[0] );
         }
      }
      else
      {
         HOST_MESSAGE.acct_id_1_len[0] = strlen(p_auth_tx->acct_id_1 );
         memcpy( HOST_MESSAGE.acct_id_1,
                 p_auth_tx->acct_id_1,
                 HOST_MESSAGE.acct_id_1_len[0] );
      }
   }

   /* FIELD 103: ACCOUNT ID 2 */
   if ( p_auth_tx->acct_id_2[0] != 0x00 )
   {
      if ( p_auth_tx->TLF01_details.tx_key > AUTH_INVALID )
      {
         /* Response */
         if ( p_auth_tx->FAF01_details.velocity[0].amount[1] == '1' )
         {
            /* This flag indicates that this field came in the request. */
            HOST_MESSAGE.acct_id_2_len[0] = strlen( p_auth_tx->acct_id_2 );
            memcpy( HOST_MESSAGE.acct_id_2,
                    p_auth_tx->acct_id_2,
                    HOST_MESSAGE.acct_id_2_len[0] );
         }
      }
      else
      {
         HOST_MESSAGE.acct_id_2_len[0] = strlen( p_auth_tx->acct_id_2 );
         memcpy( HOST_MESSAGE.acct_id_2,
                 p_auth_tx->acct_id_2,
                 HOST_MESSAGE.acct_id_2_len[0] );
      }
   }

   /* FIELD 120 ORIGINAL MESSAGE ID */
	str_asc_to_bcd ("020100", sizeof(HOST_MESSAGE.org_msg_id)*2, HOST_MESSAGE.org_msg_id);

   /* FIELD 121: ISSUING INSTITUTION ID CODE */
   HOST_MESSAGE.iss_inst_len[0] = strlen( p_auth_tx->iss_inst_id_cd);
   memcpy( HOST_MESSAGE.iss_inst_id, p_auth_tx->iss_inst_id_cd, HOST_MESSAGE.iss_inst_len[0]); 
     
   

   /* FIELD 126: Visa Private Use Fields */
   F126Len = F126_BITMAP_LEN;

	if('\0' != p_auth_tx->TLF01_details.operator_id[0])
	{
		 len =0;
		 HOST_MESSAGE.de126[0] ^= 0x01; /* Set bitmap - bit 8 */
		 genutil_asc_to_bin(p_auth_tx->TLF01_details.operator_id,temp_str,&len);
		 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
		 F126Len += len;
		 len = 0;
		 genutil_asc_to_bin(p_auth_tx->TLF01_details.workstation,temp_str,&len);
		 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
		 F126Len += len;
	}

	if ('\0' != p_auth_tx->TLF01_details.dcf01_retired_cwk[0] &&
		 'C' != p_auth_tx->TLF01_details.dcf01_retired_cwk[0])
	{
		 len =0;
		 memset(cvv_data,0x00,sizeof(cvv_data));
		 memset(bcavv_data,0x00,sizeof(bcavv_data));
		 memcpy(cvv_data,p_auth_tx->TLF01_details.dcf01_retired_cwk, 40);
		 genutil_asc_to_bin(cvv_data,bcavv_data, &len );
		 memcpy( &HOST_MESSAGE.de126[F126Len], bcavv_data, len );
		 F126Len += 20;
		 HOST_MESSAGE.de126[1] ^= 0x80;  /* Set bitmap - bit 9 */
	}

	if ('\0' != p_auth_tx->TLF01_details.product_codes[15].code[0])
	{
		 len =0;
		 memset(temp_str, 0x00, sizeof(temp_str));
		 for( i=0; i<6; i++ )
		 {
			 temp_str[i] =
			   ascii_ebcdic_conv( p_auth_tx->TLF01_details.product_codes[15].code[i], A2E );
		 }
		 len = i;
		 memcpy( &HOST_MESSAGE.de126[F126Len], temp_str, len );
		 F126Len += len;
		 HOST_MESSAGE.de126[1] ^= 0x40; /* Set bitmap - bit 10 */
		 len = 0;
	}

	/* Build DCC value - '1' , F126.19 for ATM  */
	if ((0 == strncmp(p_auth_tx->TLF01_details.processing_code, "01", 2) ||
		 0 == strncmp(p_auth_tx->TLF01_details.processing_code, "30", 2)) &&
		   p_auth_tx->TLF01_details.product_codes[0].code[3] == '1' )
	{
			HOST_MESSAGE.de126[F126Len] = ascii_ebcdic_conv
					( p_auth_tx->TLF01_details.product_codes[0].code[3], A2E );
			 F126Len += 1;
			 HOST_MESSAGE.de126[2] ^= 0x20;  /* Set bitmap - bit 19 */
	}

	if('\0' != p_auth_tx->TLF01_details.product_codes[0].code[2])
	{
		HOST_MESSAGE.de126[F126Len] = ascii_ebcdic_conv
				( p_auth_tx->TLF01_details.product_codes[0].code[2], A2E );
		 F126Len += 1;
		 HOST_MESSAGE.de126[2] ^= 0x10;  /* Set bitmap - bit 20 */
	}

	if ( F126Len > F126_BITMAP_LEN )
	{
		 /* Set the length. */
		 HOST_MESSAGE.de126_len[0] = F126Len;
	}
   /* FIELD 126: Visa Private Use Fields *
   if ((0 != strlen(p_auth_tx->TLF01_details.cvc_data)) && 
      ('Y' == p_auth_tx->TLF01_details.voice_auth[0]))
   {
       str_asc_to_bcd ("0040000000000000", sizeof(HOST_MESSAGE.priv126)*2, HOST_MESSAGE.priv126);
       memset (&temp_str, 0, sizeof(tempstr));
       strncpy(temp_str, "10",2);
       strncpy(temp_str+2, p_auth_tx->TLF01_details.cvc_data,3);
	   for (i =0; i<= 5; i++)
	   {
		  HOST_MESSAGE.priv126[8+i] = ascii_ebcdic_conv (temp_str[i], A2E);
       }
	   HOST_MESSAGE.priv126_len[0] = strlen( HOST_MESSAGE.priv126);
   }*/
}/* end Build_Host_Structure */

/*****************************************************************************

  Function:    copy_to_req_resp_buffer

  Description: This function will copy the incoming request buffer to the 
               ReqRsp_Buffer.
  Author: 
      unknown
  Inputs:
      indata - incoming request buffer.
      msglen - the length of indata.
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
void copy_to_req_resp_buffer (pCHAR indata, INT  msglen   )
{
   memset(ReqRsp_Buffer, 0, COMM_BUF_SZE);   /* zero out the buffer */
   buflen = msglen;
   reqidx = 0; 
   memcpy(ReqRsp_Buffer, indata, msglen);
   invalid_msg_format = false;
} /* end copy_to_req_resp_buffer */

/*****************************************************************************

  Function:    get_tran_type_2

  Description: This function will get the transaction type using the HOST_MESSAGE
               structure
  Author: 
      unknown
  Inputs:
      tx_key - the current transaction type
      HOST_MESSAGE (global) - the request buffer containing the message from the host
  Outputs:
      None
  Return values:
      BYTE - the calculated transaction type
******************************************************************************/
BYTE  get_tran_type_2(BYTE tx_key) 
{
   BYTE  ret_val;

   /*+--------+
     |  01xx  |
     +--------+*/
   if ( 0x01 == HOST_MESSAGE.msgtype_bcd[0] )
   {
      /* Authorization request */
      if ( 0x00 == HOST_MESSAGE.msgtype_bcd[1] ) 
      {
         switch( HOST_MESSAGE.proccode[0] )
         {
            case 00:   ret_val = AUTH_AUTHORIZATION;
                       break;

            case 30:   ret_val = AUTH_BALANCE_INQUIRY;
                       break;

            case 31:   ret_val = AUTH_BALANCE_INQUIRY;
                       break;

            case 17:   ret_val = AUTH_QUASI_CASH;
                       break;

            case 01:   ret_val = AUTH_CASH_ADVANCE;
                       break;

            case 38:   ret_val = AUTH_CARD_VERIFICATION;
                       break;

            case 48:   ret_val = AUTH_BALANCE_INQUIRY;
                       break;

            default:   ret_val = AUTH_INVALID;
                       break;
         }
      }

      else if ( 0x01 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_AUTHORIZATION;

      /* ATM confirmation  request */  
      else if ( 0x02 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_ATM_CONFIRM;


      /* Authorization response */
      else if ( 0x10 == HOST_MESSAGE.msgtype_bcd[1] )
      {
         /* Verify the procesing code to know what kind */
         switch(HOST_MESSAGE.proccode[0])
         {
            case 0x00:	 if (AUTH_SALE == tx_key)
                            ret_val = AUTH_SALE_RESPONSE;
                         else
                            ret_val = AUTH_AUTHORIZATION_RESPONSE;
                         break;

            case 0x20:   ret_val = AUTH_REFUND_RESPONSE;
                         break;

            case 0x02:   ret_val = AUTH_VOID_SALE_RESPONSE;
                         break;

            case 0x22:   ret_val = AUTH_VOID_REFUND_RESPONSE;
                         break;

            case 0x11:   ret_val = AUTH_QUASI_CASH_RESPONSE;
                         break;

            case 0x01:   ret_val = AUTH_CASH_ADVANCE_RESPONSE;
                         break;

            case 0x30:   ret_val = AUTH_BALANCE_INQUIRY_RESPONSE;
                         break;

            case 0x31:   ret_val = AUTH_BALANCE_INQUIRY_RESPONSE;
                         break;

            case 0x40:   ret_val = AUTH_PAYMENT_RESPONSE;
                         break;

            case 0x42:   ret_val = AUTH_VOID_PAYMENT_RESPONSE;
                         break;

                         /* 38 from terminal, 51 from host */
            case 0x51:   ret_val = AUTH_CARD_VERIFICATION_RESPONSE;
                         break;

                         /* Might also be 38 */
            case 0x38:   ret_val = AUTH_CARD_VERIFICATION_RESPONSE;
                         break;

            default:     ret_val = AUTH_INVALID;
                         break;
         }
      }

      /* ATM confirmation  response */
      else if ( 0x12 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_ATM_CONFIRM_RESPONSE;

      else if ( 0x30 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_ADVICE_RESPONSE ;


      else if ( 0x20 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_ADVICE;

      else
         ret_val = AUTH_INVALID;
   }

   /*+--------+
     |  02xx  |
     +--------+*/
   else if ( 0x02 == HOST_MESSAGE.msgtype_bcd[0] )
   {
      /* 0210 */
      if ( 0x10 == HOST_MESSAGE.msgtype_bcd[1] )
      {
         switch(HOST_MESSAGE.proccode[0])
         {
            case 0x01:   ret_val = AUTH_CASH_ADVANCE_RESPONSE;
                         break;

            case 0x30:   ret_val = AUTH_BALANCE_INQUIRY_RESPONSE;
                         break;

            case 0x00:   ret_val = AUTH_SALE_RESPONSE;
                         break;

            case 0x20:   ret_val = AUTH_REFUND_RESPONSE;
                         break;

            case 0x02:   ret_val = AUTH_VOID_SALE_RESPONSE;
                         break;

            case 0x22:   ret_val = AUTH_VOID_REFUND_RESPONSE;
                         break;

            case 0x40:   ret_val = AUTH_PAYMENT_RESPONSE;
                         break;

            default:     ret_val = AUTH_INVALID;
                         break;
         }
      }  

      /* 0230 */
      else if ( 0x30 == HOST_MESSAGE.msgtype_bcd[1] )
      {
         if ( HOST_MESSAGE.proccode[0] == 0x19 )
            ret_val = AUTH_FEE_COLLECTION_ACQUIRER_RESPONSE;

         else if ( HOST_MESSAGE.proccode[0] == 0x29 )
            ret_val = AUTH_FUNDS_DISBURSEMENT_ACQUIRER_RESPONSE;

         /* Check field 25 to determine txn type. */
         else if (HOST_MESSAGE.pos_condcd[0] == 0x13)
         {
            ret_val = AUTH_REPRESENTMENT_RESPONSE;
         }
         else
         {
            /* BACKOFFICE ADJUSTMENT or CASH DISBURSEMENT ADJUSTMENT */
            ret_val = AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE;
         }
      } 

   }

   /*+--------+
     |  04xx  |
     +--------+*/
   else if ( 0x04 == HOST_MESSAGE.msgtype_bcd[0] )
   {
      if ( 0x00 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_REVERSAL;

      else if ( 0x01 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_REVERSAL_REPEAT;

      else if ( 0x10 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_REVERSAL_RESPONSE;

      else if ( 0x11 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_REVERSAL_REPEAT_RESPONSE;   

      else if ( 0x20 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_REVERSAL_ADVICE;

      else if ( 0x22 == HOST_MESSAGE.msgtype_bcd[1] )
      {
         if (HOST_MESSAGE.pos_condcd[0] == 0x17) 
            ret_val = AUTH_CHARGEBACK;

         else if (HOST_MESSAGE.pos_condcd[0] == 0x54) 	 
            ret_val = AUTH_CHARGEBACK_REVERSAL;

         else
         {
            switch(HOST_MESSAGE.proccode[0])
            {
               case 0x19:   ret_val = AUTH_FEE_COLLECTION_ISSUER;
                            break;

               case 0x29:   ret_val = AUTH_FUNDS_DISBURSEMENT_ISSUER;
                            break;

               default:     ret_val = AUTH_INVALID;
                            break;
            }
         }
      }
      else if ( 0x30 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_REVERSAL_ADVICE_RESPONSE;

      else
         ret_val = AUTH_INVALID;
   }

   /*+--------+
     |  05xx  |
     +--------+*/
   else if ( 0x05 == HOST_MESSAGE.msgtype_bcd [0] )
   {
      if ( 0x20 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_RECONCILIATION;

      else
         ret_val = AUTH_INVALID;
   }

   /*+--------+
     |  06xx  |
     +--------+*/
   else if ( 0x06 == HOST_MESSAGE.msgtype_bcd [0] )
   {
      if ( 0x00 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_ADMINISTRATIVE_TEXT;

      else if ( 0x10 == HOST_MESSAGE.msgtype_bcd[1] )
         ret_val = AUTH_ADMINISTRATIVE_TEXT_RESPONSE;

      else if ( 0x20 == HOST_MESSAGE.msgtype_bcd[1] )
      {
         if ( (HOST_MESSAGE.nmi_code[0] == 0x08) &&
              (HOST_MESSAGE.nmi_code[1] == 0x83)  )
         {
            ret_val = AUTH_ADMINISTRATIVE_TEXT; /* 0620 Free Text Msg */
         }
         else
         {
            ret_val = AUTH_FUND_TRANSFER_TOTAL;
         }
      }
      else
         ret_val = AUTH_INVALID;
   }

   /*+--------+
     |  08xx  |
     +--------+*/
   else if ( 0x08 == HOST_MESSAGE.msgtype_bcd [0] )
   {
      if ( 0x00 == HOST_MESSAGE.msgtype_bcd [1] )
      {
         /* Is this an echo? */
         if ( (HOST_MESSAGE.nmi_code[0] == 0x03) &&
              (HOST_MESSAGE.nmi_code[1] == 0x01)  )
         {
            /* Yes */
            ret_val = AUTH_LOGON;
         }

         /* FIELD 70 should be 101 or 160 or 161 or 162 */
         else if ( HOST_MESSAGE.nmi_code[1] == 0x01 || 
                   HOST_MESSAGE.nmi_code[1] == 0x60 || 
                   HOST_MESSAGE.nmi_code[1] == 0x61 ||
                   HOST_MESSAGE.nmi_code[1] == 0x62)
         {
            ret_val = AUTH_KEY_CHANGE;
         }
         else
         {
            switch( HOST_MESSAGE.proccode[0] )
            {
               case 91:   ret_val = AUTH_STATISTICS;
                          break;

               case 92:   ret_val = AUTH_LOGON;
                          break;

               case 99:   ret_val = AUTH_TEST;
                          break;

               default:   ret_val = AUTH_LOGON;
                          break;
            }
         }
      }
      else if ( 0x10 == HOST_MESSAGE.msgtype_bcd [1] )
      {
         switch(HOST_MESSAGE.proccode[0])
         {
            case 0x91:   ret_val = AUTH_STATISTICS_RESPONSE;
                         break;

            case 0x92:   ret_val = AUTH_LOGON_RESPONSE;
                         break;

            case 0x99:   ret_val = AUTH_TEST_RESPONSE;
                         break;

            default:     ret_val = AUTH_LOGON_RESPONSE;
                         break;
         }
      }	
   }

   /* A transaction that the server does not understand */
   else
      ret_val = AUTH_INVALID;

   return( ret_val );
}

/*****************************************************************************

  Function:    get_tran_type

  Description: This function will gets the transaction type from a
                 message based on message type and processing code. 
  Author: 
      Dennis Irby
  Inputs:
      p_auth_tx - Transaction record
  Outputs:
      None
  Return values:
      None
  Modified by:
******************************************************************************/
BYTE  get_tran_type( pAUTH_TX p_auth_tx ) 
{
   INT  ProcCode;
   BYTE TranType;

   ProcCode = atoi( p_auth_tx->TLF01_details.processing_code );
   ProcCode = ProcCode / 10000;   /* Get just the 1st 2 digits. */
   TranType = p_auth_tx->TLF01_details.tx_key;

   /* Check if transaction is a financial response or an authorization */
   /* response.  These are sent from the MCard host as 0110's.         */
   if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0110" ) )
   {
      /* It is, now verify the procesing code to know what kind */
      switch( ProcCode )
      {
         case 0:
            if ( AUTH_SALE == TranType )
               return( AUTH_SALE_RESPONSE );
            else
               return( AUTH_AUTHORIZATION_RESPONSE );
         break;

         case 20:
            return( AUTH_REFUND_RESPONSE );
         break;

         case 2:
            return( AUTH_VOID_SALE_RESPONSE );
         break;

         case 22:
            return( AUTH_VOID_REFUND_RESPONSE );
         break;

         case 17:	
            return(AUTH_QUASI_CASH_RESPONSE);
			break;

         case 01:
            return( AUTH_CASH_ADVANCE_RESPONSE );
         break;

         case 30:
            return( AUTH_BALANCE_INQUIRY_RESPONSE );
         break;

         case 31:
            return( AUTH_BALANCE_INQUIRY_RESPONSE );
         break;

         case 40:
            return( AUTH_PAYMENT_RESPONSE );
         break;

    /*     case 42:
            return( AUTH_VOID_PAYMENT_RESPONSE );
         break; DJI debug_code */
   
         default:
            return( AUTH_INVALID );
         break;
      } /* End MCard's version of 0110's */
   }  
  
   /* Check for an advise response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0230" ) )
   {
      switch( ProcCode )
      {

         case 00:
            return( AUTH_OFFLINE_SALE_RESPONSE );
         break;

         case 20:
            return( AUTH_OFFLINE_REFUND_RESPONSE );
         break;

         case 2:
            return( AUTH_SALE_ADJUSTMENT_RESPONSE );
         break;

         case 22:
            return( AUTH_REFUND_ADJUSTMENT_RESPONSE );
         break;

         case 42:
            return( AUTH_PAYMENT_ADJUSTMENT_RESPONSE );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   } 
 
   /* Check for a Reversal response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0410" ) )
      return( AUTH_REVERSAL_RESPONSE );

  /* or a ATM confirmation  response */  
  //The next line was modified by egable on 12/23/98
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0112" ) )
	   return( AUTH_ATM_CONFIRM_RESPONSE );

  /* or a ATM confirmation  response */  
  //The next line was modified by egable on 12/23/98
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0130" ) )
	   return( AUTH_ADVICE_RESPONSE );

  /* or a ATM confirmation  response */  
  //The next line was modified by egable on 12/23/98
  else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0430" ) )
	   return( AUTH_REVERSAL_ADVICE_RESPONSE );

  else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0411" ) )
	   return( AUTH_REVERSAL_REPEAT_RESPONSE );

   /* Check for a statistics response from MCard host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0810" ) )
   {
      switch( ProcCode )
      {
         case 91:
            return( AUTH_STATISTICS_RESPONSE );
         break;

         case 92:
            return( AUTH_LOGON_RESPONSE );
         break;

         case 99:
            return( AUTH_TEST_RESPONSE );
         break;

         default:
            return( AUTH_LOGON_RESPONSE );
         break;
      }
   }	

   /* 0200 from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0200" ) )
   {
      switch( ProcCode )
      {
	     case 00:
	        return( AUTH_SALE );
         break;

         case 20:
            return( AUTH_REFUND );
         break;

         case 02:
            return( AUTH_VOID_SALE );
         break;

         case 22:
            return( AUTH_VOID_REFUND );
         break;

         case 01:
            return( AUTH_CASH_ADVANCE );
         break;

         case 40:
            return( AUTH_PAYMENT );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }

   /* Check for an advise request from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0220" ) )
   {
      switch( ProcCode )
      {
         case 00:
            return( AUTH_OFFLINE_SALE );
         break;

         case 20:
            return( AUTH_OFFLINE_REFUND );
         break;

         case 02:
            return( AUTH_SALE_ADJUSTMENT );
         break;

         case 22:
            return( AUTH_REFUND_ADJUSTMENT );
         break;

         case 42:
            return( AUTH_PAYMENT_ADJUSTMENT );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }
  
   /* Check for an Authorization request from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0100" ) )
   {
      switch( ProcCode )
      {
         case 00:
            return( AUTH_AUTHORIZATION );
         break;

         case 30:
            return( AUTH_BALANCE_INQUIRY );
         break;

         case 11:	
            return(AUTH_QUASI_CASH_RESPONSE);
			break;

         case 01:	
            return(AUTH_CASH_ADVANCE_RESPONSE);
			break;

         case 31:
            return( AUTH_BALANCE_INQUIRY );
         break;

         case 38:
            return(AUTH_CARD_VERIFICATION);
			break;

         case 48:
            return(AUTH_BALANCE_INQUIRY);
			break;

///      case 0x11:	return(AUTH_QUASI_CASH);
///					  break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }

   /* Check for a Reversal request from another host */  
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0400" ) )
      return( AUTH_REVERSAL );

   /* or a ATM confirmation  request */  
  //The next line was modified by egable on 12/23/98
  else if ( (0x01 == HOST_MESSAGE.msgtype_bcd [0]) && (0x02 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_ATM_CONFIRM );

  else if ( (0x06 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_ADMIN_ADVICE );

  /* or a ATM confirmation  request */  
  //The next line was modified by egable on 12/23/98
  else if ( (0x01 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20 == HOST_MESSAGE.msgtype_bcd[1]) )
      return( AUTH_ADVICE );

  /* or a ATM confirmation  request */  
  //The next line was modified by egable on 12/23/98
  else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x20 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_REVERSAL_ADVICE );

  else if ( (0x04 == HOST_MESSAGE.msgtype_bcd [0]) && (0x01 == HOST_MESSAGE.msgtype_bcd[1]) )
	   return( AUTH_REVERSAL_REPEAT );

   /* Check for a batch upload from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0320" ) )
      return( AUTH_BATCH_UPLOAD ); 
 
   /* Check for a settlement transaction from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0500" ) )
   {
      switch( ProcCode )
      {
         case 92:
            return( AUTH_SETTLE );
         break;

         case 96:
            return( AUTH_SETTLE_TRAILER );
         break;

         default:
            return( AUTH_INVALID );
         break;
      }
   }

   /* Check for a statistics transaction from another host */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0800" ) )
   {
      switch( ProcCode )
      {
         case 91:
            return( AUTH_STATISTICS );
         break;

         case 92:
            return( AUTH_LOGON );
         break;

         case 99:
            return( AUTH_TEST );
         break;

         default:
            return( AUTH_LOGON );
         break;
      }
   }

   /*-----------------------------------------------------------------*/
   /* Check for response message types when dealing with other hosts. */
   /*-----------------------------------------------------------------*/
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0210" ) )
   {
      /* 0210 */
      switch( ProcCode )
      {
         case 00: return( AUTH_SALE_RESPONSE );
                    break;

         case 01: return( AUTH_CASH_ADVANCE_RESPONSE );
                    break;

         case 20: return( AUTH_REFUND_RESPONSE );
                    break;

         case 02: return( AUTH_VOID_SALE_RESPONSE );
                    break;

         case 22: return( AUTH_VOID_REFUND_RESPONSE );
                    break;

         case 40: return( AUTH_PAYMENT_RESPONSE );
                    break;

  /*       case 42: return( AUTH_VOID_PAYMENT_RESPONSE );
                    break;   DJI debug_code */

         default:   return( AUTH_INVALID );
                    break;
      }
   }  
   /* 0230 */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0230" ) )
   {
      switch( ProcCode )
      {
         case 00: return( AUTH_OFFLINE_SALE_RESPONSE );
                    break;

/*check this with equit or irby
         case 02:
         case 22:
            return( AUTH_BACKOFFICE_ADJUSTMENT_RESPONSE );
         break;*/


         case 20: return( AUTH_OFFLINE_REFUND_RESPONSE );
                    break;

         case 02: return( AUTH_SALE_ADJUSTMENT_RESPONSE );
                    break;

         case 22: return( AUTH_REFUND_ADJUSTMENT_RESPONSE );
                    break;

         case 42: return( AUTH_PAYMENT_ADJUSTMENT_RESPONSE );
                    break;

         default:   return( AUTH_INVALID );
                    break;
      }
   } 

   /* 0110 */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0110" ) )
   {
      switch( ProcCode )
      {
         case 00: return( AUTH_AUTHORIZATION_RESPONSE );
                    break;

         case 30: return( AUTH_BALANCE_INQUIRY_RESPONSE );
                    break;

         case 31: return( AUTH_BALANCE_INQUIRY_RESPONSE );
                    break;

         default:   return( AUTH_INVALID );
                    break;
      }
   }

   /* 0410 */
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0410" ) )
      return( AUTH_REVERSAL_RESPONSE );

   /* 0810 */ 
   else if ( 0 == strcmp( p_auth_tx->TLF01_details.message_type, "0810" ) )
   {
      switch( ProcCode )
      {
         case 91: return( AUTH_STATISTICS_RESPONSE );
                    break;

         case 92: return( AUTH_LOGON_RESPONSE );
                    break;

         case 99: return( AUTH_TEST_RESPONSE );
                    break;

         default:   return( AUTH_LOGON_RESPONSE );
                    break;
      }
   }

   /* Type Unknown */
   else
      return( AUTH_INVALID );
}


/*****************************************************************************

  Function:    str_asc_to_bcd

  Description: This function will convert an ascii string to bcd.
  Author: 
      unknown
  Inputs:
      p_astr - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad     8/14/98
******************************************************************************/
static  void str_asc_to_bcd (p_astr, how_many, bstring)
   pCHAR        p_astr;
   INT          how_many;
   BYTE			 bstring[];
{
   UINT  i = 0;
   UINT  start;

   if (strlen(p_astr) > (UINT)how_many)
      start = strlen(p_astr) - how_many;
   else
      start = 0;

   //for (i = (strlen(p_astr) - how_many); i < strlen(p_astr); i=i+2)
   for (i = start; i < strlen(p_astr); i=i+2)
   { 
      /* if I hit a null terminator, quit.  */
      if ('\0' == p_astr[i])
         break;

      if (p_astr[i] != ' ')
         bstring[i/2] = ((p_astr[i]-'0')*16);

      if (((i+1) < strlen(p_astr)) && (p_astr[i+1] != ' ') )
		   bstring[i/2] += (p_astr[i+1]-'0');

   }

} /* str_asc_to_bcd */

/*****************************************************************************

  Function:    asc_to_bcd
  Description: This function will convert an ascii string to bcd.
  Author: 
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      how_many - number of characters in p_astr to convert
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad      8/14/98
******************************************************************************/
static  void asc_to_bcd (afield, how_many, bstring)
   pCHAR         afield;
   INT           how_many;
   BYTE          bstring[];
{  
   pCHAR  astr;
   UINT   i = 0;

   astr = afield;
   
   for (i = (strlen(afield) - how_many); i < strlen(afield); i=i+2)
   {
      if ('\0' == astr [i])
         break;

      if (astr[i] != ' ')
         bstring[i/2] = ((astr[i]-'0')*16);

      if (((i+1) < strlen(afield)) && (astr[i+1] != ' '))
         bstring[i/2] += (astr[i+1]-'0');

   }
   
} /* asc_to_bcd */

/*****************************************************************************

  Function:    

  Description: This function will convert a variable length ascii string to bcd.
  Author: 
      unknown
  Inputs:
      afield - a pointer to the ascii string to convert
      fld_len - the length of the field
  Outputs:
      bstring - the bcd string 
  Return values:
      None
  Modified by:
      FArad      8/14/98
******************************************************************************/
static  void asc_to_vbcd (afield, fld_len, bcd_buf)
   pCHAR         afield;
   INT           fld_len;   
   BYTE bcd_buf[];
{
   pCHAR astr;
   INT  i = 0;
   

   astr = afield;

   for (i = 0; i < fld_len; i=i+2)
      {
      /* if I hit a null terminator, quit.  */
      if ('\0' == astr[i])
         break;

      bcd_buf [i/2] = ((astr[i]-'0')*16);

      if ((i+1) < fld_len)
         bcd_buf [i/2] += (astr[i+1]-'0');
      else
         bcd_buf [i/2] += 0x0f;
      }
} /* asc_to_vbcd */

/*****************************************************************************

  Function:    Build_Host_Header

  Description: This function will build Visa's header and prepend it to the
               outgoing transaction.  This is required on all transactions
               going to Visa, whether they are requests or responses.

  Author: 
      unknown
  Inputs:
      p_auth_tx     - Contains Round Trip Indicator (RTI) and tx_key
      ReqRsp_Buffer - Without Visa's header
  Outputs:
      ReqRsp_Buffer - With Visa's header prepended
  Return values:
      None
******************************************************************************/
void Build_Host_Header (pAUTH_TX p_auth_tx)
{
   struct trandef  *pTranDef;
   BYTE            cMsgLen[2];
   BYTE            StationId[7];
   BYTE            net_consec_tmouts_ctr[4];
   BYTE            active_txns_ctr[4];
   BYTE            current_state[2];
   BYTE            prev_src_station[2];
	INT				Station_ID_Lenght = 0;

   pTranDef = Find_Tran_Table(p_auth_tx->TLF01_details.tx_key);            /* Get pointer to tran table */
   //memcpy(&ReqRsp_Buffer[0], pTranDef->td_msgid, sizeof(pTranDef->td_msgid));

   memset(&ReqRsp_Buffer[0], 0x16, 1 );   /* Msg hdr len = 22,           Field 1 */
   memset(&ReqRsp_Buffer[1], 0x01, 1 );   /* Hdr format type = binary 1, Field 2 */
   memset(&ReqRsp_Buffer[2], 0x02, 1 );   /* Text format is of type 2,   Field 3 */

                                          /* Length of message,          Field 4 */
   cMsgLen[0] = ((buflen) & 0xff00)/256;
   cMsgLen[1] =  (buflen) & 0x00ff;
   memcpy(&ReqRsp_Buffer[3], cMsgLen, 2 );

   if (p_auth_tx->TLF01_details.tx_key > AUTH_INVALID)
   {
      //  Put the incoming dest into the outgoing src and incoming src into incoming dest
      str_asc_to_bcd (p_auth_tx->incoming_src_station_id, sizeof( p_auth_tx->incoming_src_station_id )-1, &ReqRsp_Buffer[5] );
      str_asc_to_bcd (p_auth_tx->incoming_dest_station_id, sizeof( p_auth_tx->incoming_dest_station_id )-1, &ReqRsp_Buffer[8] );
   }
   else
   {
	   memset(&ReqRsp_Buffer[5], 0x00, 3 );   /* Dest   station Id,          Field 5 */
      /* Source station Id - Alternate between 2 of them.                   Field 6 */
      ReadVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                        active_txns_ctr, current_state, prev_src_station );

      if(prev_src_station != NULL_PTR)
      {
         memset( StationId, 0x00, sizeof( StationId ) );
         //the next line was replaced by the following line by farad on 8/19/98.
	     //if ( 0 == strncmp( ftos(host_app_src_station_flag), "1", 1 ))
         if ( '1' == prev_src_station[0] )
	     {            
            // Check the staion ID from Config file, If Station ID is null take from DB.
			Station_ID_Lenght = (INT)strlen(Station_ID1);
			if((Station_ID_Lenght > 0) &(Station_ID_Lenght <= 7)) 
			{
			  strcpy( StationId, Station_ID1);
			}
			else
              strcpy( StationId, ncf01_i.station_id1);
              p_auth_tx->ncf01_prev_src_station[0]  = '2';
              prev_src_station[0]  = '2';
         }
         else
         {
            // Check the staion ID from Config file, If Station ID is null take from DB.
			Station_ID_Lenght =(INT) strlen(Station_ID2);
			if((Station_ID_Lenght > 0) &(Station_ID_Lenght <= 7) )
			{
			  strcpy( StationId, Station_ID2);
			}
			else
              strcpy( StationId, ncf01_i.station_id2);
              p_auth_tx->ncf01_prev_src_station[0]  = '1';
              prev_src_station[0]  = '1';
         }
         str_asc_to_bcd (StationId, sizeof( StationId )-1, &ReqRsp_Buffer[8] );
      }
      else
      {
	      //memset(&ReqRsp_Buffer[8],0x00, 3);
	      memset( StationId, 0x00, sizeof( StationId ) );
	      strcpy( StationId, ncf01_i.station_id1);
         p_auth_tx->ncf01_prev_src_station[0]  = '2';
         prev_src_station[0]  = '2';
      }
      WriteVisaTable( SHARED_MEMORY_TBL, net_consec_tmouts_ctr, 
                        active_txns_ctr, current_state, prev_src_station );
   }

   /* --------------------------------------------------------------------- *
    * Fields 7 - 12 must be echoed back to Visa for switched in txns.
    * This is a switched in txn if first byte below is 0x02.
    *
    * Note 1:  The Merchant Contact field is used to store the header
    *          from Visa.  Merchant record is not used on switched in txns.
    *
    * Note 2:  Documentation indicates field 9 to be set to zeroes, but
    *          during certification, Visa informed us that it is best
    *          to echo it.
    * --------------------------------------------------------------------- */
   if ( p_auth_tx->MCF01_details.contact[0] == 0x02 )
   {
      /* Switched IN */
      ReqRsp_Buffer[11] = HOST_MESSAGE.header[12];                /* Field 7 */

      memcpy( &ReqRsp_Buffer[12], &HOST_MESSAGE.header[13], 2 );  /* Field 8 */
      memcpy( &ReqRsp_Buffer[14], &HOST_MESSAGE.header[15], 3 );  /* Field 9 */

      ReqRsp_Buffer[17] = HOST_MESSAGE.header[18];                /* Field 10*/

      memcpy( &ReqRsp_Buffer[18], &HOST_MESSAGE.header[19], 3 );  /* Field 11*/

      ReqRsp_Buffer[21] = HOST_MESSAGE.header[22];                /* Field 12*/
   }
   else
   {
      /* Switched OUT */
      memset(&ReqRsp_Buffer[11],0x00, 1 );   /* Round Trip Control,   Field 7 */
      memset(&ReqRsp_Buffer[12],0x00, 2 );   /* BASE II flags,        Field 8 */
      memset(&ReqRsp_Buffer[14],0x00, 3 );   /* Message Status Flags, Field 9 */
      memset(&ReqRsp_Buffer[17],0x00, 1 );   /* Batch Number,         Field 10*/
      memset(&ReqRsp_Buffer[18],0x00, 3 );   /* Reserved,             Field 11*/
      memset(&ReqRsp_Buffer[21],0x00, 1 );   /* User Information,     Field 12*/
   }
   return;
}

/*****************************************************************************

  Function:    blank_string

  Description: This function will determine if a string is blank
  Author: 
      unknown
  Inputs:
      astr - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/
BOOLEAN blank_string(pCHAR astr, INT max_len)
{
   int i;
   for(i = 0;i < max_len; i++)
   {
      if (astr[i] != ' ')
         return(false);
   }
   return(true);    
}

/*****************************************************************************

  Function:    blank_or_null_string

  Description: This function will determine if a string is blank or contains
               only nulls
  Author: 
      E. Gable  3/7/99
  Inputs:
      astr - the string to test
      max_len - the length of the string to test.
  Outputs:
      None
  Return values:
      true - the string is blank
      false - the string is not blank
  Modified by:
******************************************************************************/
BOOLEAN blank_or_null_string(pCHAR astr, INT max_len)
{
   int i;
   for(i = 0;i < max_len; i++)
   {
      if ((astr[i] != ' ') && (astr[i] != '\0'))
         return(false);
   }
   return(true);    
}
/*****************************************************************************

  Function:    hhutil_get_strtime

  Description: This function will get the start time
  Author: 
      unknown
  Inputs:
      None
  Outputs:
      deststr - the destination of the start time.
  Return values:
      None
  Modified by:
******************************************************************************/
void hhutil_get_strtime ( pCHAR deststr )
{
   time_t seconds;

   seconds = time(NULL_PTR);        /* get the current date and time in seconds */
   itoa(seconds, deststr, 10);    /* convert the long integer to a string */
}

/*****************************************************************************

  Function:    hhutil_verify_pin_block

  Description: This function will determine whether or not there is a pin block
               in the auth_tx
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx structure
  Outputs:
      None
  Return values:
      true  - pin block
      false - no pin block 
  Modified by:
      BGable      6/30/98
******************************************************************************/
BOOLEAN hhutil_verify_pin_block(pAUTH_TX p_auth_tx)
{
   if(0 == strlen(p_auth_tx->TLF01_details.pin_block))
      return(false);
   if(blank_string(p_auth_tx->TLF01_details.pin_block, strlen(p_auth_tx->TLF01_details.pin_block)))
      return(false);
   return(true);
}

/*****************************************************************************

  Function:    hhutil_process_debit_transaction

  Description: This function will request pin translation.  (The pin translation
               will convert the pin that is encrypted using the terminal key to 
               a pin encrypted using the host key).
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure 
  Outputs:
      None
  Return values:
      true  - successful 
      false - an error occurred when trying to send message to security function.
  Modified by:
******************************************************************************/
BOOLEAN hhutil_process_debit_transaction(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	 p_msg = NULL_PTR;
   CHAR	    Buffer[512] = {0};
   CHAR	    ErrorMsg[512] = {0};
   BYTE      buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];
   LONG      ret_code;

   /* Let racal know that this is from ncvisa */
   strcpy(p_auth_tx->TLF01_details.acquirer_id, "ncvsms");

   /* Build message for crypto. */
   memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size,&ncf01_i, Ncf01_Size );

   p_msg     = ptemsg_build_msg( MT_NSP_TRANSLATE_PIN, 
                                 ST1_NONE,
                                 ST2_NONE,
                                 security_que_name, 
                                 application_que_name,
                                 (pBYTE)&buffer1,
                                 sizeof(buffer1), 
                                 0 );
   if ( NULL_PTR == p_msg )
	{
	   ncvsms_log_message( 3, 2, "Insufficient Memory to build pin translation message" , "hhutil_process_debit_transaction" , 0 ,"");
		return(false);
	}
	ret_code = pteipc_send(p_msg, security_que_name);
	if( ret_code < 0 )
	{
	   pteipc_get_errormsg( ret_code, ErrorMsg );
       sprintf( Buffer, "error on pteipc_send to que %s: %s", security_que_name, ErrorMsg );
       ncvsms_log_message( 3, 3, Buffer , "hhutil_process_debit_transaction" , 0 ,"");
       free( p_msg );
       return(false);
	}
	
   free( p_msg );
   return(true);
}

/*****************************************************************************

  Function:    hhutil_transfer_ctl_to_auth_app

  Description: This function will transfer the auth_tx to the authorizer.  
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BOOLEAN hhutil_transfer_ctl_to_auth_app ( pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_host_out = NULL_PTR;
   LONG   ret_code;
   CHAR	 Buffer[512] = {0};
   CHAR	 ErrorMsg[512] = {0};


   p_msg_host_out = ptemsg_build_msg(MT_AUTH_REQUEST,
                                     ST1_NONE, ST2_NONE,
                                     authorizer_que_name,
                                     application_que_name,
                                     (pBYTE)p_auth_tx,
                                     Auth_Tx_Size, 0);
   if ( NULL_PTR == p_msg_host_out )
   {
      ncvsms_log_message( 3, 2, "Insufficient Memory to build host message" , "hhutil_transfer_ctl_to_auth_app" , 0 ,"");
      return(false);
   }
   ret_code = pteipc_send(p_msg_host_out, authorizer_que_name);
   free( p_msg_host_out );
   if( ret_code < 0 )
   {
      pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer,
              "hhutil_transfer_ctl: error on pteipc_send to que %s: %s",
               authorizer_que_name, ErrorMsg );
      ncvsms_log_message( 3, 3, Buffer , "hhutil_transfer_ctl_to_auth_app" , 0 ,"");
      return(false);
   }
   return(true);
}

/*****************************************************************************

  Function:    hhutil_process_host_request

  Description: This function will process a host request by filling the auth_tx
               structure with data.
  Author: 
      unknown
  Inputs:
      p_auth_tx - pointer to the auth_tx structure
  Outputs:
      p_auth_tx - pointer to the auth_tx structure
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BYTE hhutil_process_host_request(pAUTH_TX p_auth_tx)
{
   CHAR           ErrorMsg[512] = {0};
   CHAR           temp[17] = "";
   pPTE_MSG       p_msg_ins = NULL_PTR;
   NCF30          ncf30;
   CHAR           Buff[512] = {0};
   BYTE           buffer1[sizeof(AUTH_TX)+sizeof(NCF30)];
   BYTE           subtype2;
   BYTE           ret_val;
   LONG           ret_code;

   /* This response_code will be set if field 4 not PESOs and field 5 not USD */
   if (0 == strncmp(p_auth_tx->TLF01_details.response_code, "91", 2))
   {
      ncvsms_log_message( 3, 1, "Currency code is not US dollars" , "hhutil_process_host_request" , 0 ,"");
      perform_error_processing( p_auth_tx );
      return(false);
   }

   /* If a PIN is required, verify that one was entered */
   if ((AUTH_REVERSAL_ADVICE != p_auth_tx->TLF01_details.tx_key) && 
       (AUTH_REVERSAL        != p_auth_tx->TLF01_details.tx_key) &&
       (AUTH_ATM_CONFIRM     != p_auth_tx->TLF01_details.tx_key) &&
       (AUTH_REVERSAL_REPEAT != p_auth_tx->TLF01_details.tx_key))
   {
      if ((ATM_TERMINAL == p_auth_tx->additional_pos_information[0]))
      {
         if (false == hhutil_verify_pin_block(p_auth_tx))
         {
            /* If Visa does Pin Verification Service (PVS) checking, then that
             * means that Visa already verified the Pin.  ATP does not need to.
             * In this case, DE39 is set to '00' and DEs 52 and 53 are not
             * sent to ATP.  So ATP will not verify the pin, but it still
             * needs to do mag stripe verification.  All we have to do here to
             * accomodate this requirement is to not return false if pin does
             * not exist.
             */
            if ( 0 != strcmp(p_auth_tx->TLF01_details.response_code,APPROVED) )
               return(false);
         }
      }
   }

   /* Tell the authorizer to check CVV. if TRK2/TRK1 data is present*/
   if((p_auth_tx->TLF01_details.track2[0] == 0x00) && (p_auth_tx->TLF01_details.track1[0] == 0x00))
   {
      p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'N';
   }
   else 
      p_auth_tx->TLF01_details.verify_mag_stripe[0] = 'Y';

   /* Set the following flag so on the response, we can tell if
    * CVV verification has been performed.  This affects DE44 in
    * the response.  Txcntl will overwrite this field if it
    * performs CVV verification.
    */
   p_auth_tx->TLF01_details.cvc = CVV_TEST_FLAG;

   if ( AUTH_REVERSAL        == p_auth_tx->TLF01_details.tx_key ||
        AUTH_REVERSAL_REPEAT == p_auth_tx->TLF01_details.tx_key ||
        AUTH_REVERSAL_ADVICE == p_auth_tx->TLF01_details.tx_key ||
        AUTH_ATM_CONFIRM     == p_auth_tx->TLF01_details.tx_key ||
        AUTH_ADVICE          == p_auth_tx->TLF01_details.tx_key   )
   {
      /* Display reversal message on Monitor.
       * This message is used to isolate problems to selected bins
       * or to certain merchants or to networks.
       */
      memset( Buff, 0x00, sizeof(Buff) );
      sprintf( Buff,
              "Reversal from VISA: %s. Card %s, Merchant %s",
               AppName,
               p_auth_tx->TLF01_details.card_num,
               p_auth_tx->TLF01_details.merchant_id );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buff,Log_Bufr);
      ncvsms_log_message( 2, 3, Buff , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);

      /* Must get the original transaction for a reversal and advice.
       * If the original transaction is not found, reply to the
       * host for reversals and don't send the transaction to the authorizer.
       */
      memset( &ncf30, 0x00, Ncf30_Size );
      strcpy( ncf30.primary_key.network_id, AppName );
      strcpy( ncf30.primary_key.network_type, "A" );
      if ( AUTH_ADVICE == p_auth_tx->TLF01_details.tx_key )
      {
         strcpy(p_auth_tx->TLF01_details.message_type,"0120");
         subtype2 = ST2_NONE;
         strcpy( ncf30.primary_key.batch_number, "ACTIVE" );
         strcpy( ncf30.card_nbr, p_auth_tx->TLF01_details.card_num );
         strcpy( ncf30.primary_key.retrieval_ref_num,
                 p_auth_tx->TLF01_details.retrieval_ref_num );
      }
      else if ( AUTH_ATM_CONFIRM == p_auth_tx->TLF01_details.tx_key )
      {
         /* In Visa's 0102 message, only the RRN, terminal ID
          * and merchant ID are guaranteed, along with the
          * processing code, to match the original transaction.
          *
          * Note:  This is a partial reversal.
          */
         subtype2 = ST2_DB_SELECT_NCF30_BY_RRN_AND_TERMINAL;
         strcpy( ncf30.primary_key.retrieval_ref_num,
                 p_auth_tx->TLF01_details.retrieval_ref_num );
         strcpy( ncf30.device_id, p_auth_tx->TLF01_details.terminal_id );
      }
      else
      {
         strcpy(p_auth_tx->TLF01_details.message_type,"0400");
         subtype2 = ST2_DB_SELECT_TRAN_BY_STAN;
         strcpy( ncf30.card_nbr, p_auth_tx->TLF01_details.card_num );
         strcpy( ncf30.tran_amount, p_auth_tx->TLF01_details.total_amount );
		 
		 
		 /* Since all values after message type in DE90 can be zero,
		* check the stan.  If zero, will need to use DE11 for reversals
		* to look up original transaction.
		*/

	 	strcpy( ncf30.sys_trace_audit_nbr,p_auth_tx->TLF01_details.orig_sys_trace_num );
			

         if ( AUTH_REVERSAL        == p_auth_tx->TLF01_details.tx_key ||
              AUTH_REVERSAL_REPEAT == p_auth_tx->TLF01_details.tx_key  )
         {
            /* For reversals, set flag to not insert into TLF01 when performing
             * update all at completion of this transaction.  Reversals will get
             * inserted after sending to NETDS for the original txn, when the
             * response is sent to the device.
             */
            p_auth_tx->host2_acquirer_cb_code[0] = '*';
         }
      }

      /* Build message to get the original transaction from data server. */
      memcpy( buffer1,p_auth_tx, Auth_Tx_Size );
      memcpy( buffer1+Auth_Tx_Size,&ncf30, Ncf30_Size );
      ret_val = true;

      p_msg_ins = ptemsg_build_msg( MT_DB_REQUEST, 
                                    ST1_DB_SELECT, 
                                    subtype2,
                                    netds_que_name, 
                                    application_que_name,
                                    (pBYTE)&buffer1,
                                    sizeof(buffer1), 
                                    NCF30_DATA );
      if ( NULL_PTR == p_msg_ins  )
      {
         ncvsms_log_message( 3, 3, "Insufficient Memory for select ncf30 message" , "hhutil_process_host_request" , 0 ,"");
         ret_val = false;
      }
      else
      {
         /* Send message to get host specific data from data server. */
         ret_code = pteipc_send( p_msg_ins, netds_que_name );
         free( p_msg_ins );
         if ( 0 > ret_code )
         {
            pteipc_get_errormsg( ret_code, ErrorMsg );
            sprintf( Buff,"Failed transmit to queue: %s. %s",
                     netds_que_name,ErrorMsg );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(Buff,Log_Bufr);
            ncvsms_log_message( 3, 3, Buff , "hhutil_process_host_request" , 0 ,p_auth_tx->TLF01_details.card_num);
            ret_val = false;
         }
      }

      if ( AUTH_REVERSAL        == p_auth_tx->TLF01_details.tx_key ||
           AUTH_REVERSAL_REPEAT == p_auth_tx->TLF01_details.tx_key  )
      {
         /* Send immediate response to Visa for reversals.
          * There is no response for reversal advices.
          */
         p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
         strcpy( p_auth_tx->TLF01_details.response_code, "00" );

         /* Clear this flag so txn gets inserted into TLF01 in the next step. */
         p_auth_tx->host2_acquirer_cb_code[0] = 0x00;

         /* This function inserts into TLF01. */
         (void)incoming_other_host_response_db_reply( p_auth_tx );
      }
      return( ret_val );
   }

   if ( MCC_Validation_Functionality == MCC_VALIDATION_ON )
   {
      /* Validate Merchant Category Code (MCC) */
      ret_code = validate_mcc( p_auth_tx );
      if ( ret_code == false )
      {
         /* Invalid MCC.  Decline and return to sender. */
         strcpy( p_auth_tx->TLF01_details.response_code, CALL_ISSUER );
         p_auth_tx->TLF01_details.general_status = GS_TXN_TRANSACTION_NOT_ALLOWED;
         p_auth_tx->TLF01_details.tx_key += 100;

         /* Build & send response message to the Visa host. */
         return( incoming_other_host_response_db_reply( p_auth_tx ) );
      }
   }

   /* Dont send 0422 request to Txcntl, directly forward to nceqit - Girija Y,TF */
   if( (p_auth_tx->TLF01_details.message_type[1] == '4') &&
		(p_auth_tx->TLF01_details.message_type[2] == '2') )
   {
	   pPTE_MSG	p_msg_host_out = NULL_PTR;
	   LONG   ret_code;
	   CHAR	 Buffer[256];
	   CHAR	 ErrorMsg[100];


	   p_msg_host_out = ptemsg_build_msg(MT_AUTH_REQUEST,
										 ST1_NONE, ST2_NONE,
										 eqit_que_name,
										 application_que_name,
										 (pBYTE)p_auth_tx,
										 Auth_Tx_Size, 0);
	   if ( NULL_PTR == p_msg_host_out )
	   {
		  ncvsms_log_message( 3, 2, "Insufficient Memory to build host message" , "hhutil_process_host_request" , 0 ,"");
		  return(false);
	   }
	   ret_code = pteipc_send(p_msg_host_out, eqit_que_name);
	   free( p_msg_host_out );
	   if( ret_code < 0 )
	   {
		  pteipc_get_errormsg( ret_code, ErrorMsg );
		  sprintf( Buffer,
				  "Process Host Request: error on pteipc_send to que %s: %s",
				   eqit_que_name, ErrorMsg );
		  ncvsms_log_message( 3, 3, "Insufficient Memory to build host message" , "hhutil_process_host_request" , 0 ,"");
		  return(false);
	   }
	   return(true);
   } 
   /* Process the authorization request from the host. */
   if (false == hhutil_transfer_ctl_to_auth_app(p_auth_tx))
   {
      ncvsms_log_message( 3, 3, "Unable to send the request to the authorizor application" , "hhutil_process_host_request" , 0 ,"");
      return(false);
   }
   return(true);
}

/*****************************************************************************

  Function:    hhutil_send_host_response

  Description: This function will send the host response message out to the 
               VISA host.
  Author: 
      unknown
  Inputs:
      p_auth_tx - the auth_tx structure, used to get the tpdu_id 
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
BOOLEAN hhutil_send_host_response(pAUTH_TX p_auth_tx)
{
   pPTE_MSG	p_msg_out;
   LONG   ret_code;
   CHAR	 Buffer[512] = {0};
   CHAR	 ErrorMsg[512] = {0};

   p_msg_out = ptemsg_build_msg(MT_OUTGOING,
	         		            ST1_NONE,
						            ST2_NONE,
						            "dialogA",
						            application_que_name,
                              ReqRsp_Buffer,
                              buflen,
						            0); 

   if ( NULL_PTR == p_msg_out )
	{
	   ncvsms_log_message( 3, 2, "Insufficient Memory to build host message" , "hhutil_send_host_response" , 0 ,"");
	   return(false);
	}
   if ( p_auth_tx->TLF01_details.tpdu_id[0] != 0x00 )
      ptemsg_set_orig_comms_info(p_msg_out,p_auth_tx->TLF01_details.tpdu_id);

   else if ( ncf01_a.tpdu_id[0] != 0x00 )
      ptemsg_set_orig_comms_info(p_msg_out,ncf01_a.tpdu_id);

   else if ( p_auth_tx->TLF01_details.originator_info[0] != 0x00 )
      ptemsg_set_orig_comms_info(p_msg_out, p_auth_tx->TLF01_details.originator_info );

   else
   {
      ncvsms_log_message( 3, 2, "Do not know which host connection to send to." , "hhutil_send_host_response" , 0 ,"");
      free( p_msg_out );
      return(false);
   }

   ret_code = pteipc_send(p_msg_out, "dialogA");
   free( p_msg_out );
   if( ret_code < 0 )
   {
      pteipc_get_errormsg( ret_code, ErrorMsg );
      sprintf( Buffer, "error on pteipc_send to que %s: %s", "dialogA", ErrorMsg );
      ncvsms_log_message( 3, 3, Buffer , "hhutil_send_host_response" , 0 ,"");

      return(false);
	}
   else
   {
      /* Update transaction statistics */
      update_txn_stats( &Txn_Counts, p_auth_tx, SENT );
    
      /* Log transaction to trace file. */
      write_to_txn_file();
   }
   return(true);
}

/*****************************************************************************

  Function:    incoming_other_host_response

  Description: This function will process an incoming response from another host
               that needs to go to the VISA host.  Reversals get processed
               differently.  For reversal responses, perform an update_all
               to update any tables required by txcntl; get result, then delete
               the original transaction from NCF30 and respond with an approval
               to Visa.
  Author: 
      unknown
  Inputs:
      p_auth_tx - a pointer to the auth_tx of the response message
  Outputs:
      None
  Return values:
      true - successful
      false - unsuccessful
  Modified by:
******************************************************************************/
INT incoming_other_host_response(pAUTH_TX p_auth_tx)
{
   INT    ret_code;
   NCF30  ncf30;

   /* Responses to reversals and advices must be approved. */
   if ( (AUTH_REVERSAL_RESPONSE        == p_auth_tx->TLF01_details.tx_key) ||
        (AUTH_REVERSAL_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key)  )
   {
      if ( 0 != strcmp(p_auth_tx->TLF01_details.response_code, NOT_PERMITTED) )
         strcpy( p_auth_tx->TLF01_details.response_code, "00" );

      ret_code = delete_orig_ncf30( &ncf30, p_auth_tx );
      if ( false == ret_code )
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Unable to delete reversal's original txn in NCF30 Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncvsms_log_message( 3, 3, Log_Bufr , "incoming_other_host_response" , 0 ,p_auth_tx->TLF01_details.card_num);

      }
   }
   

   
   /* Continue to send response to host. */
   return( incoming_other_host_response_db_reply( p_auth_tx ) );
}

/******************************************************************************
 *
 *  NAME:         incoming_other_host_response_db_reply
 *
 *  DESCRIPTION:  This function processes a response for a host.  The request
 *                from the host is stored in p_auth_tx.  This function picks
 *                up where 'incoming_other_host_response' left off.  You come
 *                here after updating NCF21 and NCF30 if they required updating.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *                
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
INT incoming_other_host_response_db_reply( pAUTH_TX p_auth_tx)
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   CHAR       temp[3] = "";
   CHAR       err_mesg[512] = {0};
   CHAR       temp_amount[13];
   CHAR       temp_cur_code[4];
   INT		  iRet_code = true;

   if (AUTH_REVERSAL_REPEAT_RESPONSE == p_auth_tx->TLF01_details.tx_key)   
   {
      p_auth_tx->TLF01_details.tx_key = AUTH_REVERSAL_RESPONSE;
   }

   if ( AUTH_ADVICE_RESPONSE != p_auth_tx->TLF01_details.tx_key )
   {
      if ((AUTH_ADMINISTRATIVE_TEXT_RESPONSE       != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_FUND_TRANSFER_TOTAL_RESPONSE       != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_KEY_CHANGE_RESPONSE                != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_RECONCILIATION_RESPONSE            != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_FEE_COLLECTION_ISSUER_RESPONSE     != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_FUNDS_DISBURSEMENT_ISSUER_RESPONSE != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_CHARGEBACK_REVERSAL_RESPONSE       != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_CHARGEBACK_RESPONSE                != p_auth_tx->TLF01_details.tx_key))
      {
         strncpy(temp_cur_code, p_auth_tx->TLF01_details.currency_code,
                 sizeof(p_auth_tx->TLF01_details.currency_code));
         strncpy(temp_amount, p_auth_tx->TLF01_details.total_amount,
                 sizeof(p_auth_tx->TLF01_details.total_amount));

         strncpy(p_auth_tx->TLF01_details.currency_code,
                 p_auth_tx->reconciliation_cur_code,
                 sizeof(p_auth_tx->TLF01_details.currency_code));
         strncpy(p_auth_tx->TLF01_details.total_amount,
                 p_auth_tx->reconciliation_amount,
                 sizeof(p_auth_tx->TLF01_details.total_amount));
         strncpy(p_auth_tx->TLF01_details.actual_amount,
                 p_auth_tx->TLF01_details.orig_amount,
                 sizeof(p_auth_tx->TLF01_details.actual_amount));
      }
      build_host_structure(p_auth_tx);
      if (true == Build_Host_Request(p_auth_tx))
      {
         Build_Host_Header(p_auth_tx);
         if(false == hhutil_send_host_response(p_auth_tx))
         {
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Error sending response to Visa host. Term Id:%s ,Merch Id: %s RRN: %s ",
            					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            ncvsms_log_message( 3, 2, Log_Bufr , "incoming_other_host_response_db_reply" , 0 ,p_auth_tx->TLF01_details.card_num);
            iRet_code = false;
         }
      }
      else
      {
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Error building response for Visa host. Term Id:%s ,Merch Id: %s RRN: %s ",
         					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
         ncvsms_log_message( 3, 3, Log_Bufr , "incoming_other_host_response_db_reply" , 0 ,p_auth_tx->TLF01_details.card_num);

         iRet_code = false;
      }

      if ((AUTH_ADMINISTRATIVE_TEXT_RESPONSE       != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_FUND_TRANSFER_TOTAL_RESPONSE       != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_KEY_CHANGE_RESPONSE                != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_RECONCILIATION_RESPONSE            != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_FEE_COLLECTION_ISSUER_RESPONSE     != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_FUNDS_DISBURSEMENT_ISSUER_RESPONSE != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_CHARGEBACK_REVERSAL_RESPONSE       != p_auth_tx->TLF01_details.tx_key)  &&
          (AUTH_CHARGEBACK_RESPONSE                != p_auth_tx->TLF01_details.tx_key))
      {
         strncpy(p_auth_tx->TLF01_details.currency_code,temp_cur_code,
                 sizeof(p_auth_tx->TLF01_details.currency_code));
         strncpy(p_auth_tx->TLF01_details.total_amount,temp_amount,
                 sizeof(p_auth_tx->TLF01_details.total_amount));
      }
   }
   bcd_to_asc (ReqRsp_Buffer+22, 2, p_auth_tx->TLF01_details.message_type, 
               sizeof(p_auth_tx->TLF01_details.message_type));
	
   strcpy(p_auth_tx->TLF01_details.acquirer_id, "ncvsms");

   /* Update the cwk */
   if (AUTH_KEY_CHANGE_RESPONSE == p_auth_tx->TLF01_details.tx_key)
   {
      update_ncf01(p_auth_tx);
   }
 
   insert_tlf01(p_auth_tx);

   return iRet_code;
}

/******************************************************************************
 *
 *  NAME:         update_ncf01
 *
 *  DESCRIPTION:  This function updates the NCF01 record with the new pin key.
 *                
 *
 *  INPUTS:       p_auth_tx - Transaction record from host
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/

void update_ncf01( pAUTH_TX p_auth_tx )
{
   pPTE_MSG        p_msg = NULL_PTR;
   BYTE            buffer1[sizeof(AUTH_TX)+sizeof(NCF01)];
   BOOLEAN         ret_code = false;


   /* Build message to get host specific data from data server. */
   memset( buffer1, 0x00,     sizeof(buffer1) );
   memcpy( buffer1, p_auth_tx, Auth_Tx_Size );
   memcpy( buffer1+Auth_Tx_Size, &ncf01_i, Ncf01_Size );

   p_msg = ptemsg_build_msg( MT_DB_REQUEST,
                             ST1_DB_UPDATE,
                             ST2_NONE,
                             netds_que_name, 
                             application_que_name,
                             (pBYTE)&buffer1,
                             sizeof(buffer1), 
                             NCF01_DATA );
   if ( NULL_PTR == p_msg  )
   {
         ncvsms_log_message( 3, 2, "Insufficient Memory to build insert NCF01 message" , "insert_tlf01" , 0 ,"");
         ret_code = false;

   }
   else
   {
      /* Send message to update host specific data with data server. */
      ret_code = send_msg( p_msg, netds_que_name, p_auth_tx );
      free( p_msg );
   }
}

/******************************************************************************
 *
 *  NAME:         SEND_MSG
 *
 *  DESCRIPTION:  This function sends a PTE message to a specified queue.
 *
 *  INPUTS:       p_msg     - Pointer to a PTE_MSG structure
 *                que_name  - Name of the destination queue
 *                p_auth_tx - Transaction structure used here for error msgs.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if successful, false is a problem occurred
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
BOOLEAN send_msg( pPTE_MSG p_msg, pCHAR que_name, pAUTH_TX p_auth_tx )
{
   BOOLEAN  ret_val = true;
   LONG     ret_code;
   CHAR     Buff[512] = {0};
   CHAR     err_mesg[512]   = {0};
   BYTE     terminal_id[9]  = {0};
   BYTE     card_num[20]    = {0};
   BYTE     merchant_id[16] = {0};

   ret_code = pteipc_send( p_msg, que_name );
   if ( 0 > ret_code )
   {
      pteipc_get_errormsg( ret_code, err_mesg );
      sprintf(Buff, "pteipc error sending to que: %s. %s", que_name, err_mesg );

      if ( NULL_PTR != p_auth_tx )
      {
         strcpy( terminal_id, p_auth_tx->TLF01_details.terminal_id );
         strcpy( card_num,    p_auth_tx->TLF01_details.card_num    );
         strcpy( merchant_id, p_auth_tx->TLF01_details.merchant_id );
      }
         ncvsms_log_message( 3, 2, Buff , "send_msg" , 0 ,"");
      ret_val = false;
   }
   return( ret_val );

}



/*****************************************************************************

  Function:   hhutil_parse_db_ok

  Description: This function determines which type of database request the
               incoming message is from and routes it to the correct function.
  Author: 
      BGable   7/13/98
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Modified by:
      
******************************************************************************/
void hhutil_parse_db_ok(pPTE_MSG p_msg_in)
{
   pBYTE         pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE          subtype1;
   BYTE          subtype2;
   BYTE          app_data_type;
   CHAR          buffer[512] = {0};
   AUTH_TX       auth_tx;

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data);
   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
   subtype2 = ptemsg_get_msg_subtype2(p_msg_in);

   if (ST1_DB_SELECT == subtype1)
   {
      if (NCF30_DATA == app_data_type)
      {
         /*  The reversal record was found,
          *  send the transaction to the authorizer.
          */
         if (false == process_incoming_message_ncf30_reply(pCurrent))
         {
            memset( &auth_tx, 0,        Auth_Tx_Size );
            memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Unable to send the request to the authorizor application Term Id:%s ,Merch Id: %s RRN: %s ",
            					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            ncvsms_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);


         } 
      }
      else if ( TLF01_DATA == app_data_type )
      {
         /* A response to Visa was rejected.  We found the orig txn in TLF01.
          * Visa never got the response, so we need to reverse this txn out,
          * if it was approved.
          */
         memset( &auth_tx, 0x00,     Auth_Tx_Size );
         memcpy( &auth_tx, pCurrent, Auth_Tx_Size );
         memcpy( &auth_tx.TLF01_details, pCurrent+(Auth_Tx_Size), Tlf01_Size );

         if ( 0 == strcmp( auth_tx.TLF01_details.response_code,APPROVED) )
         {
            strcpy( auth_tx.TLF01_details.message_type, "0400" );

            auth_tx.TLF01_details.tx_key = AUTH_REVERSAL;

            strcpy( auth_tx.TLF01_details.orig_message,
                    auth_tx.TLF01_details.message_type );

            strcpy( auth_tx.orig_local_time_date,
                    auth_tx.TLF01_details.transmission_timestamp );

            strcpy( auth_tx.TLF01_details.response_text,
                   "Reversal for a Rejected Message" );

            /* Increment the last digit of the transaction id
             * for later insert into TLF01.
             */
            increment_tran_id( &auth_tx );

            if ( auth_tx.TLF01_details.orig_retrieval_ref_num[0] == 0x00 )
            {
               strcpy( auth_tx.TLF01_details.orig_retrieval_ref_num,
                       auth_tx.TLF01_details.retrieval_ref_num  );
            }

            if ( auth_tx.TLF01_details.orig_sys_trace_num[0] == 0x00 )
            {
               strcpy( auth_tx.TLF01_details.orig_sys_trace_num,
                       auth_tx.TLF01_details.sys_trace_audit_num );
            }

            /* Set up the amount to be reversed. */
            strcpy( auth_tx.TLF01_details.reversal_amount,
                    auth_tx.TLF01_details.total_amount );

            strcpy( auth_tx.TLF01_details.actual_amount, "000000000000" );

            /* Send reversal request to txcntl. */
            if ( false == hhutil_transfer_ctl_to_auth_app(&auth_tx) )
            {
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Unable to send reversal for rejected txn Term Id:%s ,Merch Id: %s RRN: %s ",
               			auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
               ncvsms_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
            }
         }
      }
      else
      {
         sprintf( buffer,
                 "Invalid app data type (%c) from data server.",
                  app_data_type );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(buffer,Log_Bufr);
         ncvsms_log_message( 3, 2, buffer , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
      }
   }
   else if (ST1_DB_INSERT == subtype1)
   {
      if (TLF01_DATA == app_data_type)
      {
         /* This is a reply to an insert TLF01 */
      }
   }
   else if (ST1_DB_DELETE == subtype1)
   {
   }
   else if (ST1_DB_UPDATE == subtype1)
   {
      if (ALL_DATA == app_data_type)
      {
         memset( &auth_tx, 0,        Auth_Tx_Size );
         memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

         if ( (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_RESPONSE       ) ||
              (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT_RESPONSE) ||
              (auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE         ) ||
              (auth_tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM_RESPONSE    ) ||
              (0 == strcmp("0130", auth_tx.TLF01_details.message_type)      )  )
         {
            /* Already sent the response to host.
             * Already logged into TLF01.
             */
            return; 
         }
         else
         {
            (void)incoming_other_host_response_db_reply( &auth_tx );
         }
      }
		else if (NCF01_DATA == app_data_type)
		{
			/* this reply from netds for ncf01 */
		}
   }
   else
   {
      memset( &auth_tx, 0,        Auth_Tx_Size );
      memcpy( &auth_tx, pCurrent, Auth_Tx_Size );

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Invalid incoming message type Term Id:%s ,Merch Id: %s RRN: %s ",
      		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      ncvsms_log_message( 3, 2, Log_Bufr , "hhutil_parse_db_ok" , 0 ,auth_tx.TLF01_details.card_num);
   }
   return;
} /* hhutil_parse_db_ok */

/*****************************************************************************

  Function:   hhutil_parse_db_error

  Description: This function determines which type of database request the
               incoming error message is from and routes it to the correct
               function.
  Author: 
      BGable   7/13/98
  Inputs:
      p_msg_in - pointer to the incoming message
  Outputs:
      None
  Return values:
      None
  Modified by:
      
******************************************************************************/
void hhutil_parse_db_error(pPTE_MSG p_msg_in)
{
   pBYTE pCurrent;
   pPTE_MSG_DATA p_msg_data;
   BYTE  app_data_type;
   BYTE  subtype1;
   BYTE  subtype2;
   CHAR error_msg[512] = {0};
   AUTH_TX       auth_tx;
   CHAR Buffer[512] = {0};
   NCF30 ncf30;

   p_msg_data = ptemsg_get_pte_msg_data(p_msg_in);
   pCurrent = ptemsg_get_pte_msg_data_data(p_msg_data);

   memset(&auth_tx,  0, Auth_Tx_Size);
   memcpy(&auth_tx,  pCurrent, Auth_Tx_Size);
   strcpy(error_msg, pCurrent+(Auth_Tx_Size));

   app_data_type = ptemsg_get_pte_msg_data_app_data_type(p_msg_data);
   subtype1 = ptemsg_get_msg_subtype1(p_msg_in);
   subtype2 = ptemsg_get_msg_subtype2(p_msg_in);

   if (ST1_DB_SELECT == subtype1)
   {
      if (NCF30_DATA == app_data_type)
      {
         /* Unable to select an original txn for a reversal or advice.
          * Reversals have already been responded to, while reversal
          * advices do not require a response.  Txn has already been
          * inserted into TLF01 for reversal, but not for reversal
          * advice.
          */
         if ( AUTH_REVERSAL        == auth_tx.TLF01_details.tx_key ||
              AUTH_REVERSAL_REPEAT == auth_tx.TLF01_details.tx_key )
         {
         }
         else if ( auth_tx.TLF01_details.tx_key == AUTH_ADVICE )
         {
             memset( &ncf30, 0x00, Ncf30_Size );
             (void)process_auth_advices( &auth_tx, NOT_FOUND, ncf30 );
         }
         else if ( auth_tx.TLF01_details.tx_key == AUTH_ATM_CONFIRM )
         {
            /* Could not find original txn.  No response necessary.
             * Log it and put out an error message.
             */
            insert_tlf01(&auth_tx);
            sprintf( Buffer,
                 "Could not find original txn for 0102 msg. " );
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
            strcat(Buffer,Log_Bufr);
            ncvsms_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
         }
         else if ( auth_tx.TLF01_details.tx_key == AUTH_AUTHORIZATION )
         {
             memset( &ncf30, 0x00, Ncf30_Size );
             (void)process_auth_repeat_part2( &auth_tx, NOT_FOUND, &ncf30 );
         }
         else
            insert_tlf01(&auth_tx);
      }
      else if (TLF01_DATA == app_data_type)
      {
         /* A response we sent to the Visa network was rejected due to
          * format error, or invalid value, or some other reason. We
          * attempted to find the orig txn in TLF01 to determine if we
          * should reverse it out. But we were unable to get the orig
          * txn.
          */
         sprintf( Buffer,
                 "Response to Visa rejected. Unable to get orig txn: %s",
                  error_msg );
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         		auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvsms_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
      }
   }
   else if ( ST1_DB_UPDATE == subtype1 )
   {
      if ( ALL_DATA == app_data_type )
      {
         sprintf( Buffer,
                 "NCVISA:Problems with updating the database tables: %s",
                  error_msg);
         memset(Log_Bufr,0x00,sizeof(Log_Bufr));
         sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
         					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
         strcat(Buffer,Log_Bufr);
         ncvsms_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);

         /* Response for Auth Advice and reversals have already been sent. */
         if ( (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL       ) ||
              (auth_tx.TLF01_details.tx_key == AUTH_REVERSAL_REPEAT)  )
         {
            /* Do nothing */
         }
         else if ( (auth_tx.TLF01_details.tx_key == AUTH_ADVICE_RESPONSE) ||
                   (0 == strcmp("0130", auth_tx.TLF01_details.message_type)) )
         {
            /* Do nothing */
         }
         else
         {
            /* Respond with system error, log to TLF01. */
            strcpy( auth_tx.TLF01_details.response_code, "96" );
            (void)incoming_other_host_response_db_reply( &auth_tx );
         }
      }
   }
   else if (TLF01_DATA == app_data_type)
   {
      sprintf(Buffer,"Insert TLF01 error: %s",error_msg);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					auth_tx.TLF01_details.terminal_id,auth_tx.TLF01_details.merchant_id,auth_tx.TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 2, Buffer , "hhutil_parse_db_error" , 0 ,auth_tx.TLF01_details.card_num);
   }
   return;

} /*hhutil_parse_db_error*/

/*****************************************************************************

  Function:    hhutil_process_request

  Description: This function will send a request to the dataserver.  It is used
               by hhutil_get_ncf01_cfg.
  Author: 
      unknown
  Inputs:
      p_msg_out - a pointer to the outgoing message (the request to the dataserver)
      p_msg_in  - a pointer to the incoming message (from the dataserver)
      que_name  - the name of the queue to send the request to
      error_msg - the errror message from the dataserver
  Outputs:
      None
  Return values:
      dberror - the type of error
  Modified by:
******************************************************************************/
BYTE hhutil_process_request( pPTE_MSG p_msg_out, pPTE_MSG *p_msg_in, pCHAR que_name, pCHAR error_msg )
{
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	CHAR			temp_str[512] = {0};
	LONG			retcode;
	BYTE			dberror;

	if( NULL_PTR == p_msg_out )
	{
		strcpy( error_msg, "Insufficient memory to build request message" );
		return PTEMSG_INSUFFICIENT_MEMORY;
	}

   *p_msg_in = pteipc_send_receive( p_msg_out, que_name, interactive_que_name, &retcode );
   if( NULL_PTR == *p_msg_in )
	{
		pteipc_get_errormsg( retcode, temp_str );
		sprintf( error_msg, "Communication Error during request to Terminal DataServer: %s", temp_str );
		return PTEMSG_IPC_ERROR;
	}

	if( ( dberror = ptemsg_get_result_code( *p_msg_in ) ) != PTEMSG_OK )
	{
		p_msg_data    = ptemsg_get_pte_msg_data( *p_msg_in );
		p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
		data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

      strcpy (temp_str, p_data+Auth_Tx_Size);
		sprintf( error_msg, "Database Error: %s", temp_str );
		free( *p_msg_in );
	}

	return dberror;
}

/*****************************************************************************

  Function:    hhutil_get_ncf01_cfg

  Description: This function will send a request to the dataserver to select 
               NCF01.  This is done syncronously and should be used only by
               visamain.c!!!
  Author: 
      BGable     6/30/98
  Inputs:
      pncf01 - pointer to the requested ncf01 structure
  Outputs:
      pncf01 - pointer to the returned ncf01 structure
      error_msg - error message returned by the data server
  Return values:
      None
  Modified by:
******************************************************************************/
BYTE hhutil_get_ncf01_cfg( pNCF01 pncf01, pCHAR error_msg )
{
	pPTE_MSG		p_msg_out;
	pPTE_MSG		p_msg_in;
	LONG			data_len;
	pBYTE			p_data;
	pPTE_MSG_DATA	p_msg_data;
	BYTE			rcode;
	BYTE     buffer[sizeof(AUTH_TX)+sizeof(NCF01)];
    AUTH_TX  auth_tx;

    memset(buffer,0, sizeof(buffer));

   memcpy(buffer,&auth_tx, Auth_Tx_Size);
   memcpy(buffer+Auth_Tx_Size,pncf01, Ncf01_Size);

	p_msg_in = NULL_PTR;
   p_msg_out = ptemsg_build_msg( MT_DB_REQUEST,
								  ST1_DB_SELECT,
								  0,
								  NULL_PTR,
								  interactive_que_name,
								  (pBYTE) buffer,
								  sizeof( buffer),
								  NCF01_DATA );


	if( ( rcode = hhutil_process_request( p_msg_out, &p_msg_in, netds_que_name, error_msg ) ) != PTEMSG_OK )	
		return ( rcode );

	p_msg_data    = ptemsg_get_pte_msg_data( p_msg_in );
	p_data        = ptemsg_get_pte_msg_data_data( p_msg_data );
	data_len      = ptemsg_get_pte_msg_data_data_length( p_msg_data );

   memset (pncf01, 0, Ncf01_Size);
   memcpy(pncf01, p_data+(Auth_Tx_Size), Ncf01_Size);
   
   free( p_msg_in );
	return ( PTEMSG_OK );
}

/*************************************************************************************/
/***************************************************************************************/
XINT ReadVisaTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state, pBYTE prev_src)
{

   XINT           Rt, Ret;
   VISA_MEM       mem_crdtbl;
   LONG           CurrentPosition = 0L;

 
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(MEMACCESSERROR);
   memset(&mem_crdtbl, 0, sizeof(VISA_MEM));
   Ret = MemRead(Rt, CurrentPosition +1, sizeof(VISA_MEM), &mem_crdtbl, MEM_WAIT);
   strcpy(tmouts, mem_crdtbl.net_consec_tmouts_ctr);
   strcpy(txns, mem_crdtbl.active_txns_ctr);
   strcpy(state, mem_crdtbl.current_state);
   strcpy(prev_src, mem_crdtbl.prev_src_station);

   return(0);
      
}
/***************************************************************************************/
/***************************************************************************************/
LONG WriteVisaTable(pCHAR TableName, pBYTE tmouts, pBYTE txns, pBYTE state, pBYTE prev_src)
{
   XINT Rt, Ret,result;
   SECTION TempSec, RetSec;
   MEMINFOMEM MemInfo;
   LONG SectionSize = 0L, Current_Offset = 0L, Offset = 0L;
   VISA_MEM    mcrdtbl;
   
   
   Rt = MemAccess(TableName);
   if(Rt < 0)
      return(Rt);
   memset( &MemInfo, 0x00, sizeof(MEMINFOMEM) );
   Ret = MemInfoMem(Rt, &MemInfo);
   SectionSize = MemInfo.Size;
   Current_Offset = 0L;
   Ret = MemLock(MEM_ALL, MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                   MEM_EOL), &RetSec, MEM_WAIT);
   Ret = MemWrite(Rt, 0L, SectionSize, MEM_FILL(0x00), MEM_WAIT);
   strcpy( mcrdtbl.net_consec_tmouts_ctr, tmouts );
   strcpy( mcrdtbl.active_txns_ctr, txns);
  // memcpy( mcrdtbl.current_state, state,1);
   mcrdtbl.current_state[0] = state[0];
   mcrdtbl.current_state[1] = '\0';
   strcpy( mcrdtbl.prev_src_station, prev_src);
   Ret = MemWrite(Rt, Current_Offset + 1, sizeof(mcrdtbl), &mcrdtbl, MEM_WAIT);
   result = MemUnlock(MemList(*MemSectionBuild(&TempSec, Rt, Offset, SectionSize),
                                  MEM_EOL), &RetSec);
   if(Ret < 0)
   {
      return(Ret);
   }
   
   return(0L);
}

/*****************************************************************************

  Function:    hhutil_get_response_text

  Description: This function will fill the response_text field in auth_tx.TLF01.
               Currently this will only be used by voice auth transactions.
  Author: 
      BGable     5/26/99
  Inputs:
     pAUTH_TX - pointer to auth_tx
  Outputs:
     pAUTH_TX - pointer to auth_tx (with the response text updated)
  Return values:
     None
  Modified by:
******************************************************************************/
VOID hhutil_get_response_text( pAUTH_TX p_auth_tx)
{
   INT i;
   static unsigned char response[][35] = {
   {"00Approved"},
   {"01Refer to Card Issuer"},
   {"02Refer to Card Issuer Special Cond"},
   {"03Invalid Merchant"},   
   {"04Pick-Up Card"},

   {"05Do Not Honor"},
   {"06Error"},   
   {"07Pick-Up, Special Condition"},   
   {"09Request In Progress"},   
   {"11VIP Approval"},   

   {"12Invalid Transaction"},   
   {"13Invalid Amount"},   
   {"14Invalid Account Number"},   
   {"15No Such Issuer"},   
   {"17Customer Cancellation"},   

   {"19Re-enter transaction"},   
   {"20Invalid Response"},   
   {"21No Action Taken"},   
   {"30Format Error"},   
   {"31No Bank Support by Switch"},   

   {"33Expired Card, Pick-Up"},   
   {"34Suspected Card, Pick-Up"},   
   {"36Restricted Card, Pick-Up"},   
   {"40Function Not Supported"},   
   {"41Lost, Stolen, Pick-Up"},   

   {"42Universal Acct Not Found"},   
   {"43Stolen Card, Pick-Up"},   
   {"51Not Sufficient Funds"},   
   {"54Expired Card, Pick-Up"},   
   {"55Incorrect PIN"},   

   {"56No Card Record"},   
   {"57Trnsxn Not Permitted To Card"},   
   {"58Trnsxn Not Permintted To Term"},   
   {"59Suspected Fraud, Not Pick-Up"},   
   {"61Exceeds Amount Limit"},   

   {"62Restricted Card, Not Pick-Up"},   
   {"63Security Violation"},   
   {"65Exceeds WithDrawl Freq Limit"},   
   {"68Response Received Too Late"},   
   {"75# of PIN Tries Exceeded"},   

   {"76Incorrect Reversal"},   
   {"77Lost, Stolen, Pick-Up"},   
   {"78Shop In Black List"},   
   {"79Account Status Flag False"},   
   {"87Incorrect Passport Number"},   

   {"88Incorrect Date of Birth"},   
   {"89Not Approved"},   
   {"90Cutoff Is In Progress"},   
   {"91Issuer Or Switch Inoperative"},   
   {"92Institution Can't Be Routed To"},   

   {"94Duplicate Transmission"},   
   {"96System Malfunction"}   
   };

 
   for (i = 0; i < 47; i++)
   {
      if (0 == strncmp(p_auth_tx->TLF01_details.response_code, response[i], 2))
      {
         strcpy(p_auth_tx->TLF01_details.voice_auth_text, response[i]+2);
         break;
      }

   }
   if ('\0' == p_auth_tx->TLF01_details.voice_auth_text[0])
   {
      strcpy(p_auth_tx->TLF01_details.voice_auth_text, response[1]+2);
   }
}

/******************************************************************************
 *
 *  NAME:         TRIM_SPACES
 *
 *  DESCRIPTION:  This function converts spaces at the end of a string into
 *                nulls.
 *
 *  INPUTS:       pString - String that does not want spaces at the end
 *
 *  OUTPUTS:      pString - Same string, but without trailing spaces
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       D. Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
void trim_spaces( pBYTE pString )
{

   INT len, position;

   len = strlen(pString) - 1;
   for(position = len; position >= 0; position--)
   {
      if(pString[position] == ' ')
         pString[position] = 0;
      else
         break;
   }
}

/*****************************************************************************

  Function:   ascii_to_bin

  Description:
      Convert an ascii string to a binary string
  Author: 
      SVelaga   6/21/99
  Inputs:
      astr      - pointer to the incoming ascii string
      ascii_len - length of astr
      blen      - length of bstring
  Outputs:
      binary_string  - pointer to the outgoing binary string
  Return values:
      NONE
  Modified by:

******************************************************************************/
void ascii_to_bin( CHAR *astr, INT ascii_len, BYTE *binary_string, INT blen)
{

	BYTE temp_char;
	INT i , j = 0;

	
	memset(binary_string, 0x00, blen);
	

	for(i=0, j=0; ((i < blen ) && (j < ascii_len)); i++, j+=2)
	{
		 	
		if( astr[j] <= '9' )
		{
			if(astr[j+1] <= '9')
				temp_char = (( (unsigned char )(astr[j] & 0x0f ) << 4 ) |  (unsigned char )(astr[j+1] & 0x0f ));
			else
				temp_char = (( (unsigned char )(astr[j] & 0x0f ) << 4 ) |  (unsigned char )((astr[j+1] + '9') & 0x0f ));

		}
		else 
		{
			if(astr[j+1] <= '9')
				temp_char = (( (unsigned char )((astr[j] + '9')  & 0x0f ) << 4 ) |  (unsigned char )(astr[j+1]  & 0x0f ));
			else
				temp_char = (( (unsigned char )((astr[j] + '9')  & 0x0f ) << 4 ) |  (unsigned char )((astr[j+1] + '9') & 0x0f ));
		}
		 
			binary_string[i] = temp_char;
	}

}

INT isnum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);
	
   for (i = 0; i < length; i++)
   {
      if (!isdigit(string[i]))
        return(false);
   }
   return(true);
}

INT isalphanum(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if (!isalnum(string[i]))
        return(false);
   }
   return(true);
}

/******************************************************************************
 *
 *  NAME:         isalphanum_space
 *
 *  DESCRIPTION:  This function checks for a string to be all numbers and / or
 *                alphabetical characters and / or spaces.
 *
 *  INPUTS:       in_str - String to be checked
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if string is all alphanumeric/spaces, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT isalphanum_space(pCHAR string)
{
   INT i;
   INT length;

   length = (INT)strlen(string);

   for (i = 0; i < length; i++)
   {
      if ( string[i] != ' ' )
      {
         if (!isalnum(string[i]))
           return(false);
      }
   }
   return(true);
}

/*****************************************************************************

  Function:    VERIFY_AUTH_TX_DATA

  Description: This function validates data in an auth_tx buffer.  It verifies
               numeric fields are numeric; it verifies alphanumeric fields are
               alphanumeric; and it verifies the processing code is for a
               transaction that ATP supports for Equitable.

  Author: 
      unknown
  Inputs:
      p_auth_tx - Contains data to be validated
  Outputs:
      None
  Return values:
      true  - indicates data is good
      false - indicates at least one piece of data is bad.
  Modified by:
      BGable      6/30/98
******************************************************************************/
INT verify_auth_tx_data(pAUTH_TX p_auth_tx)
{
   CHAR  Buffer[512] = {0};
   
   /* FIELD 2: PAN */
   if( false == isnum(p_auth_tx->TLF01_details.card_num))
   {
      sprintf(Buffer, "Invalid data for card number: %s",
              p_auth_tx->TLF01_details.card_num);

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
                                    
      return(false);
   }

   /* FIELD 3: PROCESSING CODE */
   if( false == isnum(p_auth_tx->TLF01_details.processing_code))
   {
      sprintf(Buffer, "Invalid data for processing code: %s",
              p_auth_tx->TLF01_details.processing_code);

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   if (0 == strcmp(p_auth_tx->TLF01_details.message_type,"0100"))
      if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"00", 2))
         if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"11", 2))
            if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"01",2))
               if (0 != strncmp(p_auth_tx->TLF01_details.processing_code,"30",2))
            {
               sprintf(Buffer, "Invalid data for processing code: %s",
                       p_auth_tx->TLF01_details.processing_code);
               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
               			p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               strcat(Buffer,Log_Bufr);
               ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);

               return(false);
            }

   /* FIELD 4: TRANSACTION AMOUNT */
   if( false == isnum(p_auth_tx->TLF01_details.total_amount))
   {
      sprintf(Buffer, "Invalid data for total amount: %s",
              p_auth_tx->TLF01_details.total_amount);

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);

      return(false);
   }

   /* FIELD 5: SETTLEMENT AMOUNT */
   if( false == isnum(p_auth_tx->TLF01_details.settlement_total))
   {
      sprintf(Buffer, "Invalid data for settlement total: %s",
              p_auth_tx->TLF01_details.settlement_total);

      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);

      return(false);
   }

   /* FIELD 7: TRANSMISSION DATE AND TIME */
   if( false == isnum(p_auth_tx->TLF01_details.transmission_timestamp))
   {
      sprintf(Buffer, "Invalid data for transmission timestamp: %s",
              p_auth_tx->TLF01_details.transmission_timestamp);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 11: STAN */
   if( false == isnum(p_auth_tx->TLF01_details.sys_trace_audit_num))
   {
      sprintf(Buffer, "Invalid data for system trace audit number: %s",
              p_auth_tx->TLF01_details.sys_trace_audit_num);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 14: EXPIRATION DATE */
   if( false == isnum(p_auth_tx->TLF01_details.exp_date))
   {
      sprintf(Buffer, "Invalid data for expiration date: %s",
              p_auth_tx->TLF01_details.exp_date);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 19: ACQ INST COUNTRY CODE */
   if( false == isnum(p_auth_tx->country_code))
   {
      sprintf(Buffer,
             "Invalid data for acqiring institution country code: %s",
              p_auth_tx->country_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      			p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 26: CARD ACCEPTOR BUSINESS CODE */
   if( false == isnum(p_auth_tx->TLF01_details.category_code))
   {
      sprintf(Buffer, "Invalid data for category code: %s",
              p_auth_tx->TLF01_details.category_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      		p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 32: ACQUIRING INST ID CODE */
   if( false == isnum(p_auth_tx->acq_inst_id_cd))
   {
      sprintf(Buffer,
             "Invalid data for acquiring institution ID code: %s",
              p_auth_tx->acq_inst_id_cd);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 33: FORWARDING INST ID CODE */
   if( false == isnum(p_auth_tx->forwarding_inst_id_cd))
   {
      sprintf(Buffer, "Invalid data for forwarding institution ID code: %s", p_auth_tx->forwarding_inst_id_cd);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 37: RRN */
   if( false == isalphanum(p_auth_tx->TLF01_details.retrieval_ref_num))
   {
      sprintf(Buffer, "Invalid data for retrieval reference number: %s", p_auth_tx->TLF01_details.retrieval_ref_num);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   /* FIELD 38: AUTH CODE */
   if( false == isalphanum_space(p_auth_tx->TLF01_details.auth_number))
   {
      sprintf(Buffer, "Invalid data for approval code: %s",
              p_auth_tx->TLF01_details.auth_number);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 39: RESPONSE CODE */
   if( false == isalphanum(p_auth_tx->TLF01_details.response_code))
   {
      sprintf(Buffer, "Invalid data for action code: %s",
              p_auth_tx->TLF01_details.response_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }

   /* FIELD 49: CURRENCY CODE, TRANSACTION */
   if( false == isnum(p_auth_tx->TLF01_details.currency_code))
   {
      sprintf(Buffer, "Invalid data for transaction currency code: %s",
              p_auth_tx->TLF01_details.currency_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(Buffer,Log_Bufr);
      ncvsms_log_message( 3, 3, Buffer , "verify_auth_tx_data" , 0 ,p_auth_tx->TLF01_details.card_num);
      return(false);
   }
   return(true);
}


/******************************************************************************
 *
 *  NAME:         INSERT_TLF01
 *
 *  DESCRIPTION:  This function inserts a transaction into TLF01.
 *
 *  INPUTS:       p_auth_tx - This is the transaction
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   true if successful, else false
 *
 *  AUTHOR:       D. Irby
 *
 ******************************************************************************/
INT insert_tlf01( pAUTH_TX p_auth_tx )
{
   pPTE_MSG   p_msg_ins = NULL_PTR;
   BYTE       buffer[sizeof(AUTH_TX)+sizeof(TLF01)];
   LONG       ret_code;
   CHAR       Buff[512] = {0};
   CHAR       err_mesg[512] = {0};
   INT        ret_val;
	strcpy(p_auth_tx->TLF01_details.acquirer_id, "ncvsms");
   if ( p_auth_tx->host2_acquirer_cb_code[0] == '*' )
   {
      /* TLF01 has already been inserted. */
      ret_val = true;
   }
   else
   {
      p_auth_tx->host2_acquirer_cb_code[0] = '*';

      if ( p_auth_tx->TLF01_details.primary_key.transaction_id[0] == 0x00 )
      {
         /* Tran ID might be missing in the event of parsing errors. */
         init_txn( p_auth_tx );
      }

      memset(buffer, 0, sizeof(buffer));
      ptetime_get_strtime(p_auth_tx->TLF01_details.tran_finish_time);
      memcpy( buffer, p_auth_tx, Auth_Tx_Size );
      memcpy(buffer+Auth_Tx_Size,&p_auth_tx->TLF01_details, Tlf01_Size);

      p_msg_ins = ptemsg_build_msg(MT_DB_REQUEST, 
                                   ST1_DB_INSERT, 
                                   ST2_NONE,
                                   updateds_que_name, 
                                   application_que_name,
                                   (pBYTE)&buffer,
                                   sizeof(buffer), 
                                   TLF01_DATA); 
      if(NULL_PTR == p_msg_ins)
      {
         ncvsms_log_message( 3, 2, "Insufficient Memory to build insert TLF01 message" , "insert_tlf01" , 0 ,"");
         ret_val = false;
      }
      else
      {
         ret_code = pteipc_send(p_msg_ins, updateds_que_name); 
         free(p_msg_ins);
         if(ret_code < 0)
         {
            pteipc_get_errormsg(ret_code, err_mesg);
            sprintf( Buff,
                    "Failed transmit to que: %s. %s",
                     updateds_que_name,err_mesg );
            ncvsms_log_message( 3, 2, Buff , "insert_tlf01" , 0 ,"");
            ret_val = false;
         }
         else
            ret_val = true;
      }
   }
   return( ret_val );
}

/******************************************************************************
 *
 *  NAME:         VALIDATE_MCC
 *
 *  DESCRIPTION:  This function verifies the merchant category code resides
 *                within a specified range, if there is no track data.  If
 *                there is track data, no validation is performed, except
 *                that it is numeric.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   True if message is successfully sent, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 *  MODIFICATIONS:
 ******************************************************************************/
LONG validate_mcc( pAUTH_TX p_auth_tx )
{
   LONG ret_code = true;
   INT  mcc;
   INT  len;
   INT  i;
   CHAR tempstr[512]={0};
   char strCardCategoryCode[256] = {0} ;
    int  CardCategoryCode = 0 ;

   len = strlen( p_auth_tx->TLF01_details.category_code );
   if ( len != bf18_len )
   {
      ret_code = false;
      memset( tempstr, 0x00, sizeof(tempstr) );
      sprintf(tempstr,"Invalid MCC: %s",p_auth_tx->TLF01_details.category_code);
      memset(Log_Bufr,0x00,sizeof(Log_Bufr));
      sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
      					p_auth_tx->TLF01_details.terminal_id,
      					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
      strcat(tempstr,Log_Bufr);
      ncvsms_log_message( 3, 2, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
   }
   else
   {
      /* Make sure MCC is numeric. */
      for( i=0; i<4; i++ )
      {
         if ( !isdigit( p_auth_tx->TLF01_details.category_code[i] ) )
         {
            ret_code = false;
            memset( tempstr, 0x00, sizeof(tempstr) );
            sprintf(tempstr,"Invalid MCC: %s",
                    p_auth_tx->TLF01_details.category_code);
            memset(Log_Bufr,0x00,sizeof(Log_Bufr));
            sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
            					p_auth_tx->TLF01_details.terminal_id,
            					p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
            strcat(tempstr,Log_Bufr);
            ncvsms_log_message( 3, 2, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
         }
      }

      if ( ret_code == true )
      {
         /* See if track data exists. */
        /* if ( (p_auth_tx->TLF01_details.track1[0] == 0x00) &&
              (p_auth_tx->TLF01_details.track2[0] == 0x00)  ) */

			  if ( (HOST_MESSAGE.track1_len[0] == 0x00) &&
                   (HOST_MESSAGE.track2_len[0] == 0x00))
		 {
            /* No track data.  Need to verify the MCC. */
            mcc = atoi( p_auth_tx->TLF01_details.category_code );
			strcpy (strCardCategoryCode, p_auth_tx->TLF01_details.category_code) ;
			CardCategoryCode = atoi (strCardCategoryCode) ;

            if ( ((MCC_RANGE1_LOW <= mcc)  &&  (mcc <= MCC_RANGE1_HIGH)) ||
                 ((MCC_RANGE2_LOW <= mcc)  &&  (mcc <= MCC_RANGE2_HIGH)) ||
				 (CardCategoryCode == 7011 )||(CardCategoryCode == 6010 ))
            {
               ret_code = true;
            }
            else
            {
               ret_code = false;
               memset( tempstr, 0x00, sizeof(tempstr) );
               sprintf(tempstr,"Invalid MCC: %s.  No Track Data.",
                       p_auth_tx->TLF01_details.category_code);

               memset(Log_Bufr,0x00,sizeof(Log_Bufr));
               sprintf(Log_Bufr," Term Id:%s ,Merch Id: %s RRN: %s ",
               					p_auth_tx->TLF01_details.terminal_id,p_auth_tx->TLF01_details.merchant_id,p_auth_tx->TLF01_details.retrieval_ref_num);
               strcat(tempstr,Log_Bufr);
               ncvsms_log_message( 3, 3, tempstr , "validate_mcc" , 0 ,p_auth_tx->TLF01_details.card_num);
            }
         }
      }
   }
   return(ret_code);
}

/******************************************************************************
 *
 *  NAME:         PARSE_TRACK2
 *
 *  DESCRIPTION:  This function parses track2 data.  It looks for the first
 *                non-numeric character to determine the field separator.
 *                It compares to anything in the card number field in auth_tx.
 *                It compares the expiration date to anything in the exp date
 *                field in auth_tx.  If empty, it copies them into those fields.
 *                It also copies the service code into a global variable in
 *                order to check for a Visa Electron transaction.
 *
 *  INPUTS:       p_auth_tx - Structure containing transaction information
 *
 *  OUTPUTS:      error_msg - Textual message in case of failure
 *
 *  RTRN VALUE:   True if track2 is successfully parsed, else false
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
INT parse_track2( pAUTH_TX p_auth_tx, pCHAR error_msg )
{
   INT    ret_val = true;
   INT    card_num_len;
   INT    track2_idx;
   pBYTE  field_separator   = NULL_PTR;

   if ( p_auth_tx->TLF01_details.track2[0] != 0x00 )
   {
      /* Locate the field separator. */
      field_separator = p_auth_tx->TLF01_details.track2;
      for(track2_idx=0;track2_idx <= bf2_max_len;track2_idx++,field_separator++)
      {
         if ( !isdigit( *field_separator ) )
            break;
      }

      if ( track2_idx > bf2_max_len )
      {
         ret_val = false;
         sprintf( error_msg,
                 "Unable to parse track2. No field separator. Track2: %s",
                  p_auth_tx->TLF01_details.track2 );
      }
      else
      {
         /* Put Card Number into auth_tx. */
         if ( p_auth_tx->TLF01_details.card_num[0] == 0x00 )
         {
            card_num_len = field_separator - p_auth_tx->TLF01_details.track2;

           /* If track2 length is odd, ignore leading zero on card number. */
               memcpy( p_auth_tx->TLF01_details.card_num,
                       p_auth_tx->TLF01_details.track2, card_num_len);
            
            sprintf( p_auth_tx->TLF01_details.card_num_len,"%02d",card_num_len);
         }

         /* Get the expiration date. */
         if ( p_auth_tx->TLF01_details.exp_date[0] == 0x00 )
         {
            memcpy( p_auth_tx->TLF01_details.exp_date, field_separator+1, 4 );
         }

         /* Get the service code. */
         memset( ServiceCode, 0x00, SERVICE_CODE_SIZE_PLUS_1 );
         memcpy( ServiceCode, field_separator+5, 3 );
      }
   }
   return( ret_val );
}


/******************************************************************************
 *
 *  NAME:         BUILD_AND_SEND_REQUEST_MESSAGE
 *
 *  DESCRIPTION:  This procedure builds and sends a request message to the 
 *                desired queue.
 *
 *  INPUTS:       msgtype  - PTE message type 
 *                subtype1 - Subtype1 for the message (see PTE.h)
 *                subtype2 - Subtype2 for the message (see PTE.h and APP_INFO.h) 
 *                que_name - Name of the queue to send the message to 
 *                buffer   - Buffer that contains the data to be sent, 
 *                length   - Length of buffer
 *                app_type - App data type for the message (see APP_INFO.h)
 *                comm_info- string containing originator communications info
 *
 *  OUTPUTS:      err_msg  - Text to describe reason for failure
 *
 *  RTRN VALUE:   True if successful, False if a problem occurred
 *
 *  AUTHOR:       Beth Gable
 *
 ******************************************************************************/
INT build_and_send_request_message( BYTE     msgtype,
                                    BYTE     subtype1,
                                    BYTE     subtype2, 
                                    pBYTE    que_name, 
                                    pBYTE    buffer, 
                                    INT      length,
                                    BYTE     app_type,
                                    pCHAR    comm_info,
                                    pCHAR    err_msg )
{
   CHAR            Buff[512] = {0};
   pPTE_MSG        p_msg    = NULL_PTR;
   INT             ret_code = true;
   LONG            msg_ret_code;

   p_msg = ptemsg_build_msg( msgtype, 
                             subtype1, 
                             subtype2,
                             que_name, 
                             application_que_name,
                             buffer,
                             length, 
                             app_type );

   if ( NULL_PTR == p_msg  )
   {
      strcpy( err_msg, "Insufficient Memory to build PTE message" );
      ret_code = false;
   }
   else
   {
      /* Set originator communications information. */
      if ( 0 != strcmp( NULL_STR, comm_info ) )
         ptemsg_set_orig_comms_info( p_msg, comm_info );

      /* Send message que_name */
      msg_ret_code = pteipc_send(p_msg, que_name);
      if(msg_ret_code < 0)
	  {
         pteipc_get_errormsg(msg_ret_code, err_msg);
         memset( Buff, 0x00, sizeof(Buff) );
         strcpy( Buff, "Failed transmit to queue: " );
         strcat( Buff, que_name );
         strncat(Buff, err_msg, 65 );
         ret_code = false;
	  }
      free(p_msg);
   }
   return( ret_code );
}

/******************************************************************************
 *
 *  NAME:         init_txn
 *
 *  DESCRIPTION:  This function initializes a new auth_tx transaction.
 *
 *  INPUTS:       p_auth_tx - Data structure containing transaction record.
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   NONE
 *
 *  AUTHOR:       Dirby
 *
 ******************************************************************************/
void init_txn( pAUTH_TX p_auth_tx )
{
   CHAR   unique_tran_str_id[20] = "";
   CHAR   date_str [ 9] = "";
   CHAR   time_str [ 7] = "";
   CHAR   time_date[25] = "";
   CHAR   temp_unique_tran[15]="";
   INT	   tid_len = 0;
   INT	   unique_tran_str_id_len=0;

	/* Get local time. */
   if ( p_auth_tx->TLF01_details.date_yyyymmdd[0] == 0x00 )
   {
	  ptetime_get_timestamp( time_date );
	  get_date( time_date, date_str );
	  get_time( time_date, time_str );
	  strcpy( p_auth_tx->TLF01_details.date_yyyymmdd, date_str );
	  strcpy( p_auth_tx->TLF01_details.time_hhmmss,   time_str );
   }

   /* Create a unique transaction id for the new auth_tx record. */
   ptetime_get_strtime( unique_tran_str_id );
   memset( p_auth_tx->TLF01_details.primary_key.transaction_id, 0,
		   sizeof(p_auth_tx->TLF01_details.primary_key.transaction_id) );
   tid_len= strlen(p_auth_tx->TLF01_details.terminal_id);
   if(tid_len == 8)
   {
   		memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
		   		p_auth_tx->TLF01_details.terminal_id, 
		   		sizeof( p_auth_tx->TLF01_details.terminal_id) - 1 );
   		strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
			unique_tran_str_id+2, 12 );
		p_auth_tx->TLF01_details.primary_key.transaction_id[20]=0x00;
   }
   else
   {
		 /* We have to now prepare 20 digit unique Transaction ID.
		 unique_tran_str_id will be having 14 digit length
		 now we will make it as 20 by adding YYMMDD in the beginning of it.
		 20 byte will look like
		 YYMMDD"unique_tran_str_id"
		 for example if unique_tran_str_id is "1444752896.334" then it will become
		 YYMMDD1444752896.334
		 Based on ther terminal length we will copy data from new unique_tran_str_id
	   */
	   strncpy(temp_unique_tran,unique_tran_str_id,14);
	   memset(unique_tran_str_id,0x00,sizeof(unique_tran_str_id));

	   strncpy(unique_tran_str_id,
				p_auth_tx->TLF01_details.date_yyyymmdd+2, 6); //YYMMDD
	   strncat (unique_tran_str_id,temp_unique_tran,14);
	   
	   unique_tran_str_id_len= strlen(unique_tran_str_id);
	   memcpy( p_auth_tx->TLF01_details.primary_key.transaction_id,
			   p_auth_tx->TLF01_details.terminal_id, tid_len );
	   strncat( p_auth_tx->TLF01_details.primary_key.transaction_id,
				unique_tran_str_id+tid_len, 
				(unique_tran_str_id_len-tid_len) );
		p_auth_tx->TLF01_details.primary_key.transaction_id[20]=0x00;
   }
   /* Initialize some of the auth_tx fields. */
   p_auth_tx->TLF01_details.general_status		=  GS_TXN_NEW;
   p_auth_tx->TLF01_details.entry_type			=  FINANCIAL;
   strcpy( p_auth_tx->TLF01_details.dispensation.origin, "RO" );
   strcpy( p_auth_tx->TLF01_details.dispensation.auth_2, "RA" );
   strcpy( p_auth_tx->TLF01_details.issuer_id,			  AppName);
   strcpy( p_auth_tx->TLF01_details.handler_queue,		  application_que_name);
   strcpy( p_auth_tx->TLF01_details.source_key, 		  ncf01_a.cwk);
   strcpy( p_auth_tx->TLF01_details.source_pin_format,	  ncf01_a.pin_type);

   if ( p_auth_tx->TLF01_details.tran_start_time[0] == 0x00 )
	  strncpy( p_auth_tx->TLF01_details.tran_start_time, unique_tran_str_id, 16 );

   /* Create RRN */
   if ( p_auth_tx->TLF01_details.retrieval_ref_num[0] == 0x00 )
   {
	  create_rrn( p_auth_tx->TLF01_details.retrieval_ref_num, p_auth_tx );
   }
}


/******************************************************************************
 *
 *  NAME:         WRITE_TO_TXN_FILE
 *
 *  DESCRIPTION:  This function calls the necessary library APIs to write a
 *                stream of data to the transaction trace file.
 *
 *  INPUTS:       None
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Dennis Irby
 *
 ******************************************************************************/
void write_to_txn_file()
{
   INT  rc;
   CHAR err_buf[256];

   /* Log transaction to trace file. */
   if ( Tracing == TRACE_ON )
   {
      rc =log_trace_entry(ppTraceFile_fp,(pCHAR)ReqRsp_Buffer,buflen,TraceFile);
      if ( rc < 0 )
      {
         /* Error while attempting to log a trace entry. */
         Tracing = TRACE_OFF;
         memset( err_buf,   0x00, sizeof(err_buf)   );
         sprintf( err_buf,
                 "Error logging entry to Txn Trace File = %s",
                  TraceFile );
         ncvsms_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");

         if ( rc == -1 )
            strcpy( err_buf,
                   "Unable to write txn into trace file. Setting trace off.");
         else if ( rc == -2 )
            strcpy( err_buf, "Unable to close trace file. Setting trace off.");
         else if ( rc == -3 )
            strcpy( err_buf, "Unable to open trace file. Setting trace off." );
         else
            sprintf(err_buf,"Unknown return value: '%d' Setting trace off.",rc);


         ncvsms_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");
         /* Close the trace file. */
         if ( false == close_trace_file( ppTraceFile_fp ))
         {
            sprintf( err_buf, "Unable to close trace file %s", TraceFile );
            ncvsms_log_message( 3, 1, err_buf , "write_to_txn_file" , 0 ,"");
         }
      }
   }
   return;
}


/*****************************************************************************
 *
 *  Function:    FIELD126PARSER
 *
 *  Description: This function parses field 126 from Visa and puts it into
 *               'outstring' in ASCII format, including the bitmap.  The
 *               bitmap in 'outstring' will be 16 ASCII bytes.
 *
 * Author:       D. Irby
 *
 * Inputs:       HOST_MESSAGE.de126 (Global)
 *
 * Outputs:      outstring - DE126 in ASCII format.
 *
 * Return values: None
 *
 ******************************************************************************/
void Field126Parser( pBYTE outstring )
{
   #define BCD_BITMAP_LEN   8
   #define ASC_BITMAP_LEN  16

   INT     i;
   INT     cps_bit;
   INT     cps_byte;
   INT     TotalLen;
   INT     inptr  = BCD_BITMAP_LEN;   /* Point to inbuf, past bitmap. */
   INT     outptr = ASC_BITMAP_LEN;   /* Point to outstring, past bitmap */
   BYTE    bcd_BitMap[8] ="";
   BYTE    asc_BitMap[17]="";
   BYTE    inbuf[255]    ="";
   BYTE    tempstr[58];


   /* Get length of field. */
   TotalLen = (INT)HOST_MESSAGE.de126_len[0];
   memcpy( inbuf, HOST_MESSAGE.de126, TotalLen );

   /* Copy bitmap into local variable. */
   memcpy( bcd_BitMap, HOST_MESSAGE.de126, BCD_BITMAP_LEN );

   /* Convert bitmap to ASCII. */
   bcd_to_asc( bcd_BitMap, BCD_BITMAP_LEN, asc_BitMap, ASC_BITMAP_LEN );
   memcpy( outstring, asc_BitMap, ASC_BITMAP_LEN );

   /* Parse the bitmap for the first 2 bytes only - others are unused. */
   for( cps_byte=1; cps_byte<=2; cps_byte++ )
   {
      for( cps_bit=1; cps_bit<=8; cps_bit++ )
      {
         if ( bcd_BitMap[cps_byte-1] & BITTAB[cps_bit].bit_mask )
         {
            /* The bit is set. */
            switch( cps_byte )
            {
               case 1: /* Byte 1 */
                      switch( cps_bit )
                      {
                         case 1:  
                                /* Customer Name */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<25; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 25;
                                outptr += 25;
                                break;
                         case 2:
                                /* Customer Address */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<57; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 57;
                                outptr += 57;
                                break;
                         case 3:
                                /* Biller Address */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<57; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 57;
                                outptr += 57;
                                break;
                         case 4:
                                /* Biller Telephone Number */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<18; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 18;
                                outptr += 18;
                                break;
                         case 5:
                                /* Process by Date */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<6; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 6;
                                outptr += 6;
                                break;
                         case 6:
                                /* Cardholder Certificate Serial Number */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 1,
                                                      tempstr, sizeof(tempstr));
                                strncat( outstring+outptr, tempstr, 2 );
                                inptr++;
                                outptr += 2;
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                bcd_to_asc( &inbuf[inptr], 16, tempstr, 32 );
                                strcat( outstring+outptr, tempstr );
                                inptr  += 16;
                                outptr += 32;
                                break;
                         case 7:
                                /* Merchant Certificate Serial Number */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 1,
                                                      tempstr, sizeof(tempstr));
                                strncat( outstring+outptr, tempstr, 2 );
                                inptr++;
                                outptr += 2;
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                bcd_to_asc( &inbuf[inptr], 16, tempstr, 32 );
                                strcat( outstring+outptr, tempstr );
                                inptr  += 16;
                                outptr += 32;
                                break;
                         case 8:
                                /* Transaction ID (VSEC) */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 20,
                                                      tempstr, sizeof(tempstr));
                                strcat( outstring+outptr, tempstr );
                                inptr  += 20;
                                outptr += 20;
                                break;
                      }
                      break;
               case 2: /* Byte 2 */
                      switch( cps_bit+8 )
                      {
                         case  9:
                                /* TranStain (VSEC) */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                genutil_bin_to_ascii( &inbuf[inptr], 20,
                                                      tempstr, sizeof(tempstr));
                                strcat( outstring+outptr, tempstr );
                                inptr  += 20;
                                outptr += 20;
                                break;
                         case 10:
                                /* CVV2 Request Data */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<6; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 6;
                                outptr += 6;
                                break;
                         case 11:
                                /* IGOTS Transaction Description */
                                memset(tempstr, 0x00, sizeof(tempstr) );
                                for( i=0; i<2; i++ )
                                {
                                   tempstr[i] =
                                      ascii_ebcdic_conv( inbuf[inptr+i], E2A );
                                }
                                strcat( outstring+outptr, tempstr );
                                inptr  += 2;
                                outptr += 2;
                                break;
                         case 12:
                                /* Service Development Indicator */
                                strncat( outstring+outptr, &inbuf[inptr], 24 );
                                break;
                      }
                      break;
               default: break;
            }  /* End SWITCH based on BYTE */
         }  /* End IF bit is set */
      }  /* End LOOP on BIT */
   }  /* End LOOP on BYTE */
}

/*****************************************************************************
 *
 *  Function:    GET_BALANCES
 *
 *  Description: This function copies balance information from a balance
 *               structure into auth_tx for current and available balances.
 *
 * Author:       D. Irby
 *
 * Inputs:       p_acct_bal - Structure containing account balances.
 *
 * Outputs:      p_auth_tx  - Transaction structure
 *
 * Return values: None
 *
 ******************************************************************************/
void get_balances( pAddl_Amts p_acct_bal, pAUTH_TX p_auth_tx )
{
   /* Currency Code */
   memcpy( p_auth_tx->TLF01_details.currency_code, p_acct_bal->cur_code, 3 );

   if ( p_acct_bal->amt_type[1] == '1' )
   {
      /* --------------- */
      /* CURRENT BALANCE */
      /* --------------- */
      memcpy( p_auth_tx->ACF01_details.cash_available,
              p_acct_bal->amount+3, 9 );

      /* Sign */
      if ( p_acct_bal->sign == 'C' )
         p_auth_tx->current_balance_sign = '+';
      else
         p_auth_tx->current_balance_sign = '-';
   }
   else
   {
      /* ----------------- */
      /* AVAILABLE BALANCE */
      /* ----------------- */
      memcpy( p_auth_tx->ACF01_details.cash_limit,
              p_acct_bal->amount+3, 9 );

      /* Sign */
      if ( p_acct_bal->sign == 'C' )
         p_auth_tx->available_balance_sign = '+';
      else
         p_auth_tx->available_balance_sign = '-';
   }
   return;
}

/*******************************************************
 * This function will get the values from tf.ini file
 * from the section DATASERVER with keyname DB_ERROR_STATICS_FLAG &
 *	DB_ERROR_STATICS_VALUE which will help to decide the db statics
 *	logging mechanism.
 * *******************************************************/

void ncvsms_get_error_warning_file_name_path(void )
{
   DWORD rc;
   CHAR  filename   [80] = {0};
   CHAR  tmpstr     [80] = {0};

   /* Get path to the tf.ini file. */
   memset( tmpstr, 0x00, sizeof(tmpstr) );
   GetPinnacleConfigDirectory(tmpstr);
   sprintf(filename, "%s%s", tmpstr, "tf.ini" );
   CHAR  ncvsms_error_warning_file_size[5] = {0};

   /*Read path for creating file in order to log db statics and db oracle error messages */
   rc = GetPrivateProfileString(
								  "DATASERVERS",             /* points to section name         */
								  "DB_STATICS_PATH",       	/* points to key name             */
								   "",  					/* Default string                 */
								   ncvsms_error_warning_file_path,              	/* points to destination buffer   */
								   sizeof(ncvsms_error_warning_file_path)-1,   	 /* size of destination buffer     */
								   filename                  /* points to ini filename         */
						 	   );

   rc = GetPrivateProfileString(
								  "ERROR_WARNING",             /* points to section name         */
								  AppName,       	/* points to key name             */
								  "",  					/* Default string                 */
								  ncvsms_module_error_warning_file_name,              	/* points to destination buffer   */
								  sizeof(ncvsms_module_error_warning_file_name)-1,   	 /* size of destination buffer     */
								  filename                  /* points to ini filename         */
								);

   rc = GetPrivateProfileString(
							  "ERROR_WARNING",             /* points to section name         */
							  "ERROR_WARNING_FILE_SIZE",       	/* points to key name             */
							  "500",  					/* Default string                 */
							  ncvsms_error_warning_file_size,              	/* points to destination buffer   */
							  sizeof(ncvsms_error_warning_file_size)-1,   	 /* size of destination buffer     */
							  filename                  /* points to ini filename         */
							);

	/* File size size conversion */
	Max_File_Size_Defined = atoi(ncvsms_error_warning_file_size);
	if(Max_File_Size_Defined <= 0)
	{
		Max_File_Size_Defined = 500 ;
	}
	Max_File_Size_Defined = Max_File_Size_Defined * ONE_MB_IN_BYTES ; /* 1 MB = 1048576 BYTES*/


   if((strlen(ncvsms_error_warning_file_path) > 0) &&
	  (strlen(ncvsms_module_error_warning_file_name)> 0))
   {
	   ncvsms_create_Error_Filename();
   }
}

INT ncvsms_Log_error_warning_to_File(pCHAR Error_Buf,int sev, pCHAR func,int detail)
{
	INT   ret_val = 0;
	INT   len=0;
	INT   cardlen = 0;
	INT   path_len = 0;
	INT   nIndex = 0;
	CHAR  time_date[25]  ={0};
	CHAR  timestamp[14]  ={0};
	CHAR  current_mmdd[5]={0};
	CHAR Buffer[1024]={0};
	CHAR card_tid_mid_details [256] = {0};
	CHAR tempcard[21] = {0};
	CHAR masked_cardnum[21] = {0};
	char *cardIndex = NULL ;
	FILE *fp;
	UINT file_size = 0 ;
	char buf[256] = {0} ;


	/* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( time_date );

	/* Pick out the month and day to compare to filename extenstion. */
    memcpy( current_mmdd,   time_date+5, 2 );
    memcpy( current_mmdd+2, time_date+8, 2 );

	/* Compare filename extension to current date. */
    len = strlen(ncvsms_Error_warning_Filename);
    path_len = strlen(ncvsms_error_warning_file_path);
    if( len == 0 ||
    	path_len==0 )
    {
    	LogEvent(Error_Buf,INFO_MSG);
    	return 0;
    }
    if ( 0 != strcmp(&ncvsms_Error_warning_Filename[len-4], current_mmdd) )
    {
		/* Now create the new filename.*/
    	ncvsms_create_Error_Filename();
    }
	if((fp = fopen(ncvsms_Error_warning_Filename,"a+b"))==NULL)
	{
		LogEvent(Error_Buf,INFO_MSG);
		return 0;
	}
	strcpy(Buffer,time_date);
	strcat(Buffer,":");
	if(sev == 1)
	{
		strcat(Buffer," INFO");
	}
	else if (sev == 2)
	{
		strcat(Buffer," WARN");
	}
	else
	{
		strcat(Buffer," ERROR");
	}
	strcat(Buffer,": ");
	strcat(Buffer,Error_Buf);
	strcat(Buffer," ");
	strcat(Buffer, func);
	strcat(Buffer,"\n");
	len=strlen(Buffer);

	if(fwrite(Buffer, len, NUM_SIZE_WRITES, fp)==NUM_SIZE_WRITES)
	{
		// Do nothing fall below
	}
	else
	{
		LogEvent(Buffer,INFO_MSG);
		fclose(fp);
		return  0;
	}
	fseek (fp, 0, SEEK_END);
	file_size=ftell (fp);

	if( file_size >= Max_File_Size_Defined )
	{
		sprintf(buf,"ERROR_WARNING_FILE  size is exceeding the permissible size, need attention" );
		TxUtils_Send_Msg_To_Operator( 1, buf,1, ALERT_MSG, "", 4, INFO_ERROR,NULL,NULL,NULL );
	}
	fclose(fp);
	return  0;
}

void ncvsms_create_Error_Filename()
{
	CHAR  system_date[25] = {0};

   /* Get system timestamp "YYYY-MM-DD-hh.mm.ss.jjj" */
	ptetime_get_timestamp( system_date );

    strcpy( ncvsms_Error_warning_Filename, ncvsms_error_warning_file_path );
    strcat( ncvsms_Error_warning_Filename, ncvsms_module_error_warning_file_name );
	strcat( ncvsms_Error_warning_Filename, ".EW." );
    strncat(ncvsms_Error_warning_Filename, system_date,   4 );  /* YYYY */
    strncat(ncvsms_Error_warning_Filename, system_date+5, 2 );  /* MM   */
    strncat(ncvsms_Error_warning_Filename, system_date+8, 2 );  /* DD   */
}
/******************************************************************************
 *
 *  NAME:         ncvsms_log_message
 *
 *  DESCRIPTION:  This function takes a string and input parameters and
 *                formats a message to be sent to either Monitor, Event Log
 *                or both.  Based on inputs, it sets the severity.
 *
 *  INPUTS:       dest - 1 = Monitor, 2 = File/Event log, 3 = Both
 *                sev  - 1 = Info,    2 = Warning,   3 = Error
 *                msg  - Text message to be logged
 *                func - Name of function logging the error
 *
 *  OUTPUTS:      None
 *
 *  RTRN VALUE:   None
 *
 *  AUTHOR:       Abhishek Verma
 *
 ******************************************************************************/
void ncvsms_log_message( INT dest, INT sev, pCHAR msg, pCHAR func, int details ,pCHAR card_num)
{
   INT  monitor_flag = 0;
   INT  eventlog_flag = 0;
   INT  msg_type = 0;
   BYTE severity = 0;
   BYTE log_type[20] = {0};
   CHAR text_message [512] = {0};
   INT  Appname_len = 0;
   CHAR print_buffer[256] = {0} ;
   INT text_message_len = 0;
   CHAR card_num_msg[56] = {0};

   /* Set Monitor flag */
   if ( dest == 1  ||  dest == 3 )
      monitor_flag = 1;
   else
      monitor_flag = 0;

   /* Set Event Log flag */
   /*if ( dest > 1 )
      eventlog_flag = 1;
   else
      eventlog_flag = 0;*/

   /* Set message type and severity */
   if ( sev == 2 )
   {
      msg_type = WARN_MSG;
      severity = '3';
      strcpy( log_type, WARNING_ERROR );
   }
   else if ( sev == 1 )
   {
      msg_type = INFO_MSG;
      severity = '0';
      strcpy( log_type, INFO_ERROR );
   }
   else if ( sev == 3 )
   {
      msg_type = ALERT_MSG;
      severity = '4';
      strcpy( log_type, FATAL_ERROR );
   }

   /* Make sure text message is not too long. */
   sprintf(text_message,"%s ,",AppName);
   Appname_len = strlen(text_message);

   memcpy( text_message + Appname_len, msg, (sizeof(text_message)-1 -Appname_len));
   if (card_num[0] == NULL)
   {
	   // No card number, hence masking not required
   }
   else
   {
	   Mask_card_number(card_num);
	   sprintf(card_num_msg, "Card num : %s",masked_card_num);
	   strcat(text_message,card_num_msg);
   }
   /* Call function to post the message. */
   ncvsms_Log_error_warning_to_File(text_message,sev,func,details);
   if(monitor_flag == 1)
   {
	   text_message_len = strlen(text_message);
	   if(text_message_len > 255)
	   {
		   strncpy(print_buffer,text_message,254); // Maximum msg sent to moniotr is 256
		   TxUtils_Send_Msg_To_Operator( monitor_flag, print_buffer,
									 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
	   }
	   else
	   {
		   TxUtils_Send_Msg_To_Operator( monitor_flag, text_message,
									 	 eventlog_flag, msg_type, func,
										 severity, log_type,
										 NULL, NULL,NULL );
	   }
   }

   return;
}

void Mask_card_number(char * card_num)
{
	int cardlen = 0;
	int nIndex 	= 0;
	memset(masked_card_num ,0x00, sizeof(masked_card_num));
	cardlen = strlen( card_num );
	if (cardlen > 0)
	{
		/* Masking card number first 5 and last 4 in clear form remaining all masked. */
		strncpy( masked_card_num, card_num, cardlen);
		for ( nIndex = cardlen-5; nIndex >= cardlen-11; nIndex-- )
		{
			masked_card_num[nIndex] = '*';
		}
	}
}


/*************************************************************************************
       NAME:               convert_to_ebcdic

       DESCRIPTION:        converts a stream of bytes from ASCII to EBCDIC

       INPUTS:
           source          pointer to a stream of bytes to be converted
           len             length of the byte stream
       OUTPUTS:
           dest            pointer to stream of converted bytes

       RETURNS:
           None.

       AUTHOR:             Ram Malathkar
*************************************************************************************/
void convert_to_ebcdic( pBYTE source, pBYTE dest, INT len )
{
   INT temp;
   BYTE temp_byte;

   if( !maps_initialised )
      init_character_code_maps();

   for( temp = 0; temp < len; temp++ )
   {
      temp_byte = *(source + temp);
      *(dest + temp) = a2e[temp_byte];
   }

   return;
} /* convert_to_ebcdic */

/*****************************************************************************

Function:      init_character_code_maps

Description:   This function will initialise the EBCDIC and ASCII code
               maps needed for the conversion routines.

Author:
               Ram Malathkar
Inputs:
               None
Outputs:
               None
Return values:
               None
******************************************************************************/
void init_character_code_maps()
{
   // ascii to ebcdic map
   memset( a2e, 0, sizeof( a2e ) );

   a2e[0x30]=0xF0; a2e[0x31]=0xF1; a2e[0x32]=0xF2; a2e[0x33]=0xF3; a2e[0x34]=0xF4;
   a2e[0x35]=0xF5; a2e[0x36]=0xF6; a2e[0x37]=0xF7; a2e[0x38]=0xF8; a2e[0x39]=0xF9;
   a2e[0x61]=0x81; a2e[0x62]=0x82; a2e[0x63]=0x83; a2e[0x64]=0x84; a2e[0x65]=0x85;
   a2e[0x66]=0x86; a2e[0x67]=0x87; a2e[0x68]=0x88; a2e[0x69]=0x89; a2e[0x6A]=0x91;
   a2e[0x6B]=0x92; a2e[0x6C]=0x93; a2e[0x6D]=0x94; a2e[0x6E]=0x95; a2e[0x6F]=0x96;
   a2e[0x70]=0x97; a2e[0x71]=0x98; a2e[0x72]=0x99; a2e[0x73]=0xA2; a2e[0x74]=0xA3;
   a2e[0x75]=0xA4; a2e[0x76]=0xA5; a2e[0x77]=0xA6; a2e[0x78]=0xA7; a2e[0x79]=0xA8;
   a2e[0x7A]=0xA9; a2e[0x41]=0xC1; a2e[0x42]=0xC2; a2e[0x43]=0xC3; a2e[0x44]=0xC4;
   a2e[0x45]=0xC5; a2e[0x46]=0xC6; a2e[0x47]=0xC7; a2e[0x48]=0xC8; a2e[0x49]=0xC9;
   a2e[0x4A]=0xD1; a2e[0x4B]=0xD2; a2e[0x4C]=0xD3; a2e[0x4D]=0xD4; a2e[0x4E]=0xD5;
   a2e[0x4F]=0xD6; a2e[0x50]=0xD7; a2e[0x51]=0xD8; a2e[0x52]=0xD9; a2e[0x53]=0xE2;
   a2e[0x54]=0xE3; a2e[0x55]=0xE4; a2e[0x56]=0xE5; a2e[0x57]=0xE6; a2e[0x58]=0xE7;
   a2e[0x59]=0xE8; a2e[0x5A]=0xE9; a2e[0x20]=0x40; a2e[0x2E]=0x4B; a2e[0x3C]=0x4C;
   a2e[0x28]=0x4D; a2e[0x2B]=0x4E; a2e[0x7C]=0x4F; a2e[0x26]=0x50; a2e[0x21]=0x5A;
   a2e[0x24]=0x5B; a2e[0x2A]=0x5C; a2e[0x29]=0x5D; a2e[0x3B]=0x5E; a2e[0x2D]=0x60;
   a2e[0x2F]=0x61; a2e[0x2C]=0x6B; a2e[0x25]=0x6C; a2e[0x5F]=0x6D; a2e[0x3E]=0x6E;
   a2e[0x3F]=0x6F; a2e[0x3A]=0x7A; a2e[0x23]=0x7B; a2e[0x40]=0x7C; a2e[0x27]=0x7D;
   a2e[0x3D]=0x7E; a2e[0x22]=0x7F;

   // ebcdic to ascii map
   memset( e2a, 0, sizeof( e2a ) );

   e2a[0xF0]=0x30; e2a[0xF1]=0x31; e2a[0xF2]=0x32; e2a[0xF3]=0x33; e2a[0xF4]=0x34;
   e2a[0xF5]=0x35; e2a[0xF6]=0x36; e2a[0xF7]=0x37; e2a[0xF8]=0x38; e2a[0xF9]=0x39;
   e2a[0x81]=0x61; e2a[0x82]=0x62; e2a[0x83]=0x63; e2a[0x84]=0x64; e2a[0x85]=0x65;
   e2a[0x86]=0x66; e2a[0x87]=0x67; e2a[0x88]=0x68; e2a[0x89]=0x69; e2a[0x91]=0x6A;
   e2a[0x92]=0x6B; e2a[0x93]=0x6C; e2a[0x94]=0x6D; e2a[0x95]=0x6E; e2a[0x96]=0x6F;
   e2a[0x97]=0x70; e2a[0x98]=0x71; e2a[0x99]=0x72; e2a[0xA2]=0x73; e2a[0xA3]=0x74;
   e2a[0xA4]=0x75; e2a[0xA5]=0x76; e2a[0xA6]=0x77; e2a[0xA7]=0x78; e2a[0xA8]=0x79;
   e2a[0xA9]=0x7A; e2a[0xC1]=0x41; e2a[0xC2]=0x42; e2a[0xC3]=0x43; e2a[0xC4]=0x44;
   e2a[0xC5]=0x45; e2a[0xC6]=0x46; e2a[0xC7]=0x47; e2a[0xC8]=0x48; e2a[0xC9]=0x49;
   e2a[0xD1]=0x4A; e2a[0xD2]=0x4B; e2a[0xD3]=0x4C; e2a[0xD4]=0x4D; e2a[0xD5]=0x4E;
   e2a[0xD6]=0x4F; e2a[0xD7]=0x50; e2a[0xD8]=0x51; e2a[0xD9]=0x52; e2a[0xE2]=0x53;
   e2a[0xE3]=0x54; e2a[0xE4]=0x55; e2a[0xE5]=0x56; e2a[0xE6]=0x57; e2a[0xE7]=0x58;
   e2a[0xE8]=0x59; e2a[0xE9]=0x5A; e2a[0x40]=0x20; e2a[0x4B]=0x2E; e2a[0x4C]=0x3C;
   e2a[0x4D]=0x28; e2a[0x4E]=0x2B; e2a[0x4F]=0x7C; e2a[0x50]=0x26; e2a[0x5A]=0x21;
   e2a[0x5B]=0x24; e2a[0x5C]=0x2A; e2a[0x5D]=0x29; e2a[0x5E]=0x3B; e2a[0x60]=0x2D;
   e2a[0x61]=0x2F; e2a[0x6B]=0x2C; e2a[0x6C]=0x25; e2a[0x6D]=0x5F; e2a[0x6E]=0x3E;
   e2a[0x6F]=0x3F; e2a[0x7A]=0x3A; e2a[0x7B]=0x23; e2a[0x7C]=0x40; e2a[0x7D]=0x27;
   e2a[0x7E]=0x3D; e2a[0x7F]=0x22;

   maps_initialised = true;

} /* init_character_code_maps */

