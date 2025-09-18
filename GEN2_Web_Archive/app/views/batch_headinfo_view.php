 <div>
	<?php if(!empty($data)): ?>
	
    <table class="table" style="background:#fff;border:1px solid #cccccc;">
		<tbody>
			<tr>
				<td class="light-blue-background"><strong>Device id:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['DEVICE_ID']; ?></td>
				<td><strong>Batch nbr:</strong></td>
				<td><?php echo $data['BATCH_NBR']; ?></td>
				<td class="light-blue-background"><strong>Merchant id:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['MERCHANT_ID']; ?></td>
			</tr>
			<tr>
				<td class="light-blue-background"><strong>Total credit cnt:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['TOTAL_CREDIT_CNT']; ?></td>
				<td><strong>Total credit amt:</strong></td>
				<td><?php echo $data['TOTAL_CREDIT_AMT']; ?></td>
				<td class="light-blue-background"><strong>Total credit refund cnt:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['TOTAL_CREDIT_REFUND_CNT']; ?></td>
			</tr>
			
			<tr>
				<td class="light-blue-background"><strong>Total credit refund amt:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['TOTAL_CREDIT_REFUND_AMT']; ?></td>
				<td><strong>Total debit cnt:</strong></td>
				<td><?php echo $data['TOTAL_DEBIT_CNT']; ?></td>
				<td class="light-blue-background"><strong>Total debit amt:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['TOTAL_DEBIT_AMT']; ?></td>
			</tr>
			<tr>
				<td class="light-blue-background"><strong>Total debit refund cnt:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['TOTAL_DEBIT_REFUND_CNT']; ?></td>
				<td><strong>Total debit refund amt:</strong></td>
				<td><?php echo $data['TOTAL_DEBIT_REFUND_AMT']; ?></td>
				<td class="light-blue-background"><strong>Total void credit cnt:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['TOTAL_VOID_CREDIT_CNT']; ?></td>
			</tr>
			
			<tr>
				<td class="light-blue-background"><strong>Total void credit amt:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['TOTAL_VOID_CREDIT_AMT']; ?></td>
				<td><strong>Total void credit refund cnt:</strong></td>
				<td><?php echo $data['TOTAL_VOID_CREDIT_REFUND_CNT']; ?></td>
				<td class="light-blue-background"><strong>Total void credit refund amt:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['TOTAL_VOID_CREDIT_REFUND_AMT']; ?></td>
			</tr>
			<tr>
				<td class="light-blue-background"><strong>Open date:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['OPEN_DATE']; ?></td>
				<td><strong>Open time:</strong></td>
				<td><?php echo $data['OPEN_TIME']; ?></td>
				<td class="light-blue-background"><strong>Release date:</strong></td>
				<td class="light-blue-background font-bold">
				<?php 
					if(!empty($data['RELEASE_DATE'])) {
						echo date('Y-m-d', strtotime($data['RELEASE_DATE']));
					} else {
						echo $data['RELEASE_DATE'];
					}
				?>
				</td>
			</tr>

			<tr>
				<td class="light-blue-background"><strong>Release time:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['RELEASE_TIME']; ?></td>
				<td><strong>Batch status:</strong></td>
				<td><?php echo $data['BATCH_STATUS']; ?></td>
				<td class="light-blue-background"><strong>Visa merchant id:</strong></td>
				<td class="light-blue-background font-bold"><?php echo $data['VISA_MERCHANT_ID']; ?></td>
			</tr>
			<tr>
				<td class="light-blue-background"><strong>Purge date:</strong></td>
				<td class="light-blue-background font-bold">
					<?php
					if(isset($data['PURGE_DATE']) && strlen($data['PURGE_DATE']) == 8) {
						echo substr($data['PURGE_DATE'], 0, 4).'-'.substr($data['PURGE_DATE'], 4, 2).'-'.substr($data['PURGE_DATE'], 6, 2);
					} else {
						echo "&nbsp";
					}
					?>
				</td>
				<td></td>
				<td></td>
				<td class="light-blue-background"></td>
				<td class="light-blue-background"></td>
			</tr>
		</tbody>
    </table>
	<?php endif; ?>
</div>
