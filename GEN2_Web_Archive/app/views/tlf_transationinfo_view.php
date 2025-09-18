 <?php function encrypt($str) {
  $encoded = "";
  for( $i = 0; $i < strlen($str); $i++ ) {
    $b = ord($str[$i]);
    $a = $b ^ 51;  // <-- must be same number used to encode the character
    $encoded .= chr($a);
  }
  return base64_encode($encoded);
} //var_dump($val); ?>
 <div class="row" style="font-size: 12px;">
 	<div class="col-md-12">
	 <table class="table" style="background:#fff;border:1px solid #cccccc;">
		<tr>
		<?php
		if(strcmp($data['PRODUCT4_QUANTITY'], '19') == 0) : ?>
			<td class="light-blue-background">
				<i class="fa fa-fw fa-circle-o"></i>
			</td>
			<td class="light-blue-background"><strong>Host Processed</strong></td>
		
			<td class="light-blue-background">
				<i class="fa fa-fw fa-circle-o"></i>	
			</td>				
			<td class="light-blue-background"><strong>ATP STIP Processed</strong></td>
		
			<td class="light-blue-background">
			<i class="fa fa-fw fa-check-circle-o"></i>
			</td>
			<td class="light-blue-background"><strong>TO ATP STIP Processed</strong></td>
	
	
	<?php elseif(strcmp($data['PRODUCT2_CODE'], 'stndin') != 0) : ?>
			<td class="light-blue-background">
				<i class="fa fa-fw fa-check-circle-o"></i>	
			</td>				
			<td class="light-blue-background"><strong>Host Processed</strong></td>
			<td class="light-blue-background">
				<i class="fa fa-fw fa-circle-o"></i>
			</td>
			<td class="light-blue-background"><strong>ATP STIP Processed</strong></td>
			<td class="light-blue-background">
				<i class="fa fa-fw fa-circle-o"></i>
			</td>
			<td class="light-blue-background"><strong>TO ATP STIP Processed</strong></td>

	<?php
	  elseif (strcmp($data['PRODUCT2_CODE'], 'stndin') == 0): ?>
			<td class="light-blue-background">
				<i class="fa fa-fw fa-circle-o"></i>
			</td>
			<td class="light-blue-background"><strong>Host Processed</strong></td>

			<td class="light-blue-background">
				<i class="fa fa-fw fa-check-circle-o"></i>	
			</td>				
			<td class="light-blue-background"><strong>ATP STIP Processed</strong></td>
		
			<td class="light-blue-background">
				<i class="fa fa-fw fa-circle-o"></i>
			</td>
			<td class="light-blue-background"><strong>TO ATP STIP Processed</strong></td>
	
		
	<?php endif;?>
		
	
			<td class="light-blue-background">
				<?php if(ord($data['TX_KEY']) == AUTH_AUTHORIZATION_OVERRIDE_G || ord($data['TX_KEY']) == AUTH_CASH_ADVANCE_AUTHORIZATION_OVERRIDE_G): ?>
					
					<i class="fa fa-fw fa-check-circle-o"></i>
				<?php else: ?>
					<i class="fa fa-fw fa-circle-o"></i>
				<?php endif;?>
			</td>
			<td class="light-blue-background"><strong>Voice Override(VOD)</strong></td>
		
			<td class="light-blue-background">
				<?php if(strlen($data['NBR_OF_PROD_CODES']) > 0): ?>
					<i class="fa fa-fw fa-check-circle-o"></i>
				<?php else: ?>
					<i class="fa fa-fw fa-circle-o"></i>
				<?php endif;?>
			</td>
			<td class="light-blue-background"><strong>Override Merchant Denial</strong></td>

				</tr>
	</table>
	 </div>


	<div class="col-md-12"><?php 
	if(isset($data) && is_array($data) && !empty($data)){ ?>
	
    <table class="table" style="background:#fff;border:1px solid #cccccc;">
		<tr>
			<td class="light-blue-background"><strong>Transaction Type:</strong></td>
			<td class="light-blue-background"><?php echo $data['TransactionType']; ?></td>
			
			<td><strong>Service Code:</strong></td>
			<td><?php echo $data['SERVICE_CODE']; ?></td>
			
			<td class="light-blue-background"><strong>Earmarked Currency:</strong></td>
			<td class="light-blue-background"><?php echo $data['EarMarkedCurrency']; ?></td>
			
			
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Message Type:</strong></td>
			<td class="light-blue-background"><?php echo $data['MESSAGE_TYPE']; ?></td> 
			
			<td><strong>Original Txn Amount:</td>
			<td><?php echo $data['ORG_TXN_AMT']; ?></td>
			
			<td class="light-blue-background"><strong>Operator ID:</td>
			<td class="light-blue-background"><?php echo $data['OPERATOR_ID']; ?></td>
			
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Retrieval Reference Num:</strong></td>
			<td class="light-blue-background"><?php echo $data['RETRIEVAL_REF_NUM']; ?></td>
			
			<td><strong>Original Txn Amt Currency:</td>
			<td><?php echo $data['ORG_TXN_CUR']; ?></td>
			<td class="light-blue-background"><strong>MCARD REF</td>
			<td class="light-blue-background"><?php echo $data['MCARD_BANKNET']; ?></td>
			
			
			
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Processing Code:</strong></td>
			<td style="padding: 0px;" class="light-blue-background">
				<table class="table light-blue-background" style="margin:0px">
					<tr>
						<td class="light-blue-background"><?php echo $data['PROCESSING_CODE']; ?></td>
						<td class="light-blue-background"><?php echo $data['PRODUCT18_AMOUNT']; ?></td>
					</tr>
				</table>
			</td>
			
			<td><strong>Expiration Date:</strong></td>
			<td><?php echo $data['EXP_DATE']; ?></td>
			
			<!--td><strong>BDO Marchant Acq Txn:</td>
			<td><?php echo $data['PRODUCT5_CODE']; ?></td-->
			<td class="light-blue-background"><strong>Settle Date</strong></td>
			<td class="light-blue-background"><?php echo $data['SETTLEMENT_DATE']; ?></td>
			
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Card Number:</strong></td>
			<td class="light-blue-background" id="view_trainfo_card" data-mask="<?php echo encrypt($data['CARD_NUM_MASK']); ?>" data-unmask="<?php echo encrypt($data['CARD_NUM']); ?>" ></td>
			
			<td><strong>Acquiring Country:</td>
			<td><?php echo $data['CURRENCY_CODE']; ?></td>
			
			<td class="light-blue-background"><strong>Acquiring Inst. ID:</td>
			<td class="light-blue-background"><?php echo $data['ACQUIRING_ID']; ?></td>
			
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Transaction Amount:</strong></td>
			<td class="light-blue-background"><?php echo $data['TOTAL_AMOUNT']; ?></td>
		
			<td><strong>ECI:</td>
			<!--<td><?php echo $data['PROFILE']; ?></td> -->
			<td style="padding: 0px;">
				<table class="table" style="background:#fff;border:1px solid #cccccc;margin:0px">
					<tr>
						<td><?php echo $data['PROFILE']; ?></td>
						<td><?php echo $data['PROFILE_1']; ?></td>
					</tr>
				</table>
			</td>
			
			<td class="light-blue-background"><strong>Forwarding Inst. ID:</td>
			<td class="light-blue-background"><?php echo $data['FORWARDING_INSTITUTION_ID']; ?></td>
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Currency Code:</strong></td>
			<td class="light-blue-background"><?php echo $data['CURRENCY_CODE']; ?></td>
					
			<td><strong>Original ECI/Reason:</td>
			<td><?php echo $data['ORG_ECI_REASON']; ?></td>
			
			<td class="light-blue-background"><strong>System Date:</td>
			<td class="light-blue-background"><?php echo $data['SYSTEM_DATE']; ?></td>
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Response Code:</strong></td>
			<!--td><?php echo $data['RESPONSE_CODE']; ?></td-->
			<td style="padding: 0px;">
				<table class="table" style="background:#fff;border:1px solid #cccccc;margin:0px">
					<tr>
						<td class="light-blue-background"><?php echo $data['RESPONSE_CODE']; ?></td>
						<!--td><?php echo $data['RESPONSE_CODE_BY_SCHEME']; ?></td-->
						<td class="light-blue-background"><?php if(isset($data['RESPONSE_CODE_BY_SCHEME'])) 
						{ 
							echo $data['RESPONSE_CODE_BY_SCHEME']; 
						} else 
						{ 
							echo $data['RESPONSE_CODE_BY_SCHEME']=' '; 
						}?>
						</td>
						
						
						
					</tr>
				</table>
			</td>
				
		
			<td><strong>Action Code</td>
			<td><?php echo $data['ACTION_CODE']; ?></td>
			
			<td class="light-blue-background"><strong>Invoice Number:</td>
			<td class="light-blue-background"><?php echo $data['INVOICE_NUMBER']; ?></td>
			
			<!--td><strong>Invoice Number:</td>-->
			<!--td><?php echo $data['INVOICE_NUMBER']; ?></td>-->
			
			<!--<td><strong>Bin Low:</td>-->
			<!--<td><?php echo $data['PRODUCT3_CODE']; ?></td>-->
			
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Bin Low:</td>
			<td class="light-blue-background"><?php echo $data['PRODUCT3_CODE']; ?></td>
			
			<td><strong>Terminal ID:</strong></td>
			<td><?php echo $data['TERMINAL_ID']; ?></td>
		
			<td class="light-blue-background"><strong>Transaction Time:</td>
			<td class="light-blue-background"><?php echo $data['TIME_HHMMSS']; ?></td>
			
		</tr>
		<tr>
		
			<td class="light-blue-background"><strong>Auth Profile:</td>
			<td class="light-blue-background"><?php echo $data['PRODUCT4_CODE']; ?></td>
			
			<td><strong>Merchant ID:</strong></td>
			<td><?php echo $data['MERCHANT_ID']; ?></td>
			
			<td class="light-blue-background"><strong>Transaction Date:</td>
			<td class="light-blue-background"><?php echo $data['DATE_YYYYMMDD']; ?></td>
			
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Purge Date:</strong></td>
			<td class="light-blue-background"><?php echo $data['PURGE_DATE']; ?></td>
			
			<td><strong>Category Code(MCC):</strong></td>
			<td><?php echo $data['CATEGORY_CODE']; ?></td>
			
			<td class="light-blue-background"><strong>Auth ID Response:</td>
			<td class="light-blue-background"><?php echo $data['AUTH_NUMBER']; ?></td>
			
		</tr>
		<tr>
			<td class="light-blue-background"><strong>Reason Code:</strong></td>
			<td class="light-blue-background">&nbsp;</td>
		
			<td><strong>Merchant Name:</strong></td>
			<td><?php echo $data['CARD_HOLDER_NAME']; ?></td>
		
			<td class="light-blue-background"><strong>Response Text:</td>
			<td class="light-blue-background"><?php echo $data['RESPONSE_TEXT']; ?></td>
	
		</tr><tr>
			
			<td class="light-blue-background"><strong>System Trace Audit Num:</strong></td>
			<td class="light-blue-background"><?php echo $data['SYS_TRACE_AUDIT_NUM']; ?></td>
		
			<td><strong>POS Entry Mode:</strong></td>
			<td style="padding: 0px;">
				<table class="table" style="background:#fff;border:1px solid #cccccc;margin:0px">
					<tr>
						<td><?php echo $data['POS_ENTRY_MODE']; ?></td>
						<td><?php echo $data['POS_ENTRY_MODE_1']; ?></td>
					</tr>
				</table>
			</td>
			
			<td class="light-blue-background"><strong>Acquiring Amount:</td>
			<td class="light-blue-background"><?php echo $data['ORIG_AMOUNT']; ?></td>
			
		</tr><tr>

		
			<td class="light-blue-background"><strong>Available Balance:</strong></td>
			<td class="light-blue-background"><?php echo $data['OUTSTANDING_BALANCE']; ?></td>
		
			<td><strong>POS Condition Code:</strong></td>
			<td><?php echo $data['POS_CONDITION_CODE']; ?></td>
			
			<td class="light-blue-background"><strong>Acquiring Currency:</td>
			<td class="light-blue-background"><?php echo $data['NFI_SEQ_NBR']; ?></td>
			
		</tr><tr>

		
			<td class="light-blue-background"><strong>Credit Limit:</strong></td>
			<td class="light-blue-background"><?php echo $data['CREDIT_LIMIT']; ?></td>
			
			<td><strong>POS Data:</strong></td>
			<td><?php echo $data['PRODUCT17_AMOUNT']; ?></td>
			
			<td class="light-blue-background"><strong>Earmarked Amt:</strong></td>
			<td class="light-blue-background"><?php echo $data['EarMarkedAmt']; ?></td>
			
		</tr>
		<tr>
			
			<td class="light-blue-background"><strong>Tran ID:</strong></td>
			<td class="light-blue-background"><?php echo $data['TRAN_ID1']; ?>
			<br><?php echo $data['TRAN_ID2']; ?></td>
				
					
			<td><strong>CUP Indicator:</strong></td>
			<td><?php echo $data['CUP_INDICATOR']; ?></td>
			
			<td class="light-blue-background"><strong>Visa Merchant Identifier:</strong></td>
			<td class="light-blue-background"><?php echo $data['VISA_MERCHANT_IDENTIFIER']; ?></td>
			
						
		</tr>
	
		<tr>
			
			<td class="light-blue-background"><strong>Digital Comm Program Indicator:</strong></td>
			<td class="light-blue-background"><?php echo $data['DIGITAL_COMM_PROGRAM_INDICATOR']; ?></td>
			
			<td><strong>Digital Entry Identifier:</strong></td>
			<td><?php echo $data['DIGITAL_ENTRY_IDENTIFIER']; ?></td>
			
			<td class="light-blue-background"><strong>POS Data Extd Cond Codes:</strong></td>
			<td class="light-blue-background"><?php echo $data['POS_DATA_EXTENDED_COND_CODE']; ?></td>
			
			
		</tr>
		
		<tr>
		
			<td class="light-blue-background"><strong>MRA Sent Time</strong></td>
			
			<td class="light-blue-background"><?php if(isset($data1['TRANSACTION_SEND_TIME'])) 
			{ 
				echo $data1['TRANSACTION_SEND_TIME']; 
			} else 
			{ 
				echo $data1['TRANSACTION_SEND_TIME']=' '; 
			}?>
			</td>
			
			<td><strong>Replacement Amount:</strong></td>
			<td><?php echo $data['REVERSAL_AMOUNT']; ?></td>
			
			<td class="light-blue-background"><strong>Host Name:</strong></td>
			<td class="light-blue-background"><?php echo $data['REDEMPTION_AMOUNT']; ?></td>
			
		</tr>
		
     </table>

	 <table class="table">
		<p class="font-primary"><strong>Deferred Payments</strong></p>
	 <tr>
				<td width="15%" style="border: 1px solid #ccc; border-right: none;" class="font-bold">Gross Amount:</td>
				<td style="border-bottom: 1px solid #ccc;"><?php echo $data['DEF_GROSS_AMT'];?></td>
				
				<td width="15%" style="border-bottom: 1px solid #ccc;border-left: 1px solid #ccc;" class="font-bold">Monthly Amort Amount:</td>
				<td style="border-bottom: 1px solid #ccc;"><?php echo $data['MONTHLY_AMORT_AMT']; ?></td>
				
				<td width="10%" style="border: 1px solid #ccc; border-right: none;" class="font-bold">Term Length:</td>
				<td style="border-bottom: 1px solid #ccc;"><?php echo $data['DEFERRED_TERM_LENGTH']; ?></td>

				<td width="5%" style="border: 1px solid #ccc; border-right: none;" class="font-bold">Factor(%):</td>
				<td style="border-bottom: 1px solid #ccc;border-right: 1px solid #ccc;"><?php echo $data['DEFERRED_FACTOR']; ?></td>

			</tr>
	 </table>
	
		<table class="table" style="background:#fff;">
			<tr>
				<td width="5%" style="border: 1px solid #ccc;" class="font-bold">NII</td>
				<td width="15%" style="border-right: 1px solid #ccc;border-bottom: 1px solid #ccc;border-left: none;"><?php if(isset($data['NII'])) 
					{ 
						echo $data['NII']; 
					} 
					else 
					{ 
						echo $data['NII']=' '; 
					}?>
				</td>
				<td width="5%" style="border: none;"><strong></strong></td>
				<td width="15%" style="border-right: 1px solid #ccc;border-bottom: 1px solid #ccc;border-left: 1px solid #ccc;" class="font-bold">Text</td>
				<td style="border-right: 1px solid #ccc; border-bottom: 1px solid #ccc;"><?php echo $data['VOICE_AUTH_TEXT1']; ?>
				<?php echo $data['VOICE_AUTH_TEXT2']; ?></td>
			</tr>
		</table>

	 </div>
	
		
		</div>
	   
	<?php } ?>
	</div>
</div>

<script>
function checkMask() {
		ele = $('#view_trainfo_card');
	if(mask == 0) {
		text = ele.data('unmask');
		text = decrypt(text);
		ele.html(text);
	} else {
		text = ele.data('mask');
		text = decrypt(text);
		ele.html(text);
	}
}
checkMask();
</script>