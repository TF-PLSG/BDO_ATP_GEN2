 <?php function encrypt($str) {
  $encoded = "";
  for( $i = 0; $i < strlen($str); $i++ ) {
    $b = ord($str[$i]);
    $a = $b ^ 51;  // <-- must be same number used to encode the character
    $encoded .= chr($a);
  }
  return base64_encode($encoded);
} //var_dump($val); ?>
 <div>
    <table class="table" style="background:#fff;border:1px solid #cccccc;">
		<tr>
			<td class="light-blue-background"><strong>Merchant ID:</strong></td>
			<td class="light-blue-background font-bold"><?php echo $data['MERCHANT_ID']; ?></td>
		
			<td><strong>Device ID:</td>
			<td class="font-bold"><?php echo $data['DEVICE_ID']; ?></td>

			<td class="light-blue-background"><strong>Response Code:</td>
			<td class="light-blue-background font-bold"><?php echo $data['RESPONSE_CODE']; ?></td>
		</tr>

		<tr>
			<td class="light-blue-background"><strong>Card Number:</strong></td>
			<td class="light-blue-background font-bold" id="view_trainfo_card" data-mask="<?php echo encrypt($data['CARD_NBR_MASK']); ?>" data-unmask="<?php echo encrypt($data['CARD_NBR']); ?>" ></td>
		
			<td><strong>Retrieval Reference Num:</td>
			<td class="font-bold"><?php echo $data['RETRIEVAL_REF_NUM']; ?></td>

			<td class="light-blue-background"><strong>Processing Code:</td>
			<td class="light-blue-background font-bold"><?php echo $data['PROCESSING_CODE']; ?></td>
		</tr>

		<tr>
			<td class="light-blue-background"><strong>Transaction ID:</strong></td>
			<td class="light-blue-background font-bold"><?php echo $data['TRANSACTION_ID']; ?></td>
		
			<td><strong>Sys Trace Audit Num:</td>
			<td class="font-bold"><?php echo $data['SYS_TRACE_AUDIT_NUM']; ?></td>

			<td class="light-blue-background"><strong>Invoice Number:</td>
			<td class="light-blue-background font-bold"><?php echo $data['INVOICE_NBR']; ?></td>
		</tr>

		<tr>
			<td class="light-blue-background"><strong>Transaction Amount:</strong></td>
			<td class="light-blue-background font-bold"><?php 
				if(isset($data['TRAN_AMOUNT']) && !empty($data['TRAN_AMOUNT'])) {
					$temp = ltrim($data['TRAN_AMOUNT'], 0);
					echo substr_replace($temp, '.', -2, 0);
				} else {
					echo $data['TRAN_AMOUNT'];
				}
			?></td>
		
			<td><strong>Default Gross Amt:</td>
			<td class="font-bold"><?php echo $data['DEF_GROSS_AMT']; ?></td>

			<td class="light-blue-background"><strong>Transaction Date:</td>
			<td class="light-blue-background font-bold">
				<?php 
				if(!empty($data['TRAN_DATE'])) {
					echo date('Y-m-d', strtotime($data['TRAN_DATE']));
				} else {
					echo $data['TRAN_DATE'];
				} ?>
			</td>
		</tr>

		<tr>
			<td class="light-blue-background"><strong>Transaction Time:</strong></td>
			<td class="light-blue-background font-bold">
				<?php 
					if(!empty($data['TRAN_TIME'])) {
						 echo substr($data['TRAN_TIME'], 0, 2).':'.substr($data['TRAN_TIME'], 2, 2).':'.substr($data['TRAN_TIME'], 4, 2);
					} else {
						echo $data['TRAN_TIME'];
					}?>
			</td>
		
			<td><strong>Open Date:</td>
			<td class="font-bold">
				<?php 
				if(!empty($data['OPEN_DATE'])) {
					echo date('Y-m-d', strtotime($data['OPEN_DATE']));
				} else {
					echo $data['OPEN_DATE'];
				} ?>
			</td>

			<td class="light-blue-background"><strong>Open Time:</td>
			<td class="light-blue-background font-bold">
				<?php 
					if(!empty($data['OPEN_TIME'])) {
						 echo substr($data['OPEN_TIME'], 0, 2).':'.substr($data['OPEN_TIME'], 2, 2).':'.substr($data['OPEN_TIME'], 4, 2);
					} else {
						echo $data['OPEN_TIME'];
					}?>
			</td>
		</tr>

		<tr>
			<td class="light-blue-background"><strong>Transmission Time:</strong></td>
			<td class="light-blue-background font-bold"><?php echo $data['TRANSMISSION_TIMESTAMP']; ?></td>
		
			<td><strong>Exp:</td>
			<td class="font-bold"><?php echo $data['EXP_DATE']; ?></td>

			<td class="light-blue-background"><strong>Auth Number:</td>
			<td class="light-blue-background font-bold"><?php echo $data['AUTH_NUMBER']; ?></td>
		</tr>

		<tr>
			<td class="light-blue-background"><strong>Batch Number:</strong></td>
			<td class="light-blue-background font-bold"><?php echo $data['BATCH_NBR']; ?></td>
		
			<td><strong>Purge Date:</td>
			<td class="font-bold">
			<?php
				if(isset($data['PURGE_DATE']) && strlen($data['PURGE_DATE']) == 8) {
					echo substr($data['PURGE_DATE'], 0, 4).'-'.substr($data['PURGE_DATE'], 4, 2).'-'.substr($data['PURGE_DATE'], 6, 2);
				} else {
					echo "&nbsp";
				}
			?>
			</td>
			<td class="light-blue-background"></td>
			<td class="light-blue-background"></td>
		</tr>
     </table>
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
