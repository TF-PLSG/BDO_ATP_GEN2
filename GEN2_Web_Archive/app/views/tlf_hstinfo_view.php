<?php 
$field['UNIQUE_ID'] = 'Unique Id';
$field['FILE_NAME'] = 'File Name';
$field['FILE_PRIMARY_KEY'] = 'File Key';
$field['DATE_MODIFIED'] = 'Modified Date';
$field['FIELD_TAG'] = 'Field';
$field['CURRENT_DATA'] = 'Current Data';
$field['CHANGED_BY'] = 'Modified By';
$field['TIME_MODIFIED'] = 'Modified Time';
$field['PREVIOUS_DATA'] = 'Previous Data';
$field['RESERVED'] = 'Reserved';

?>
<div>
	<?php if(!empty($data)): ?>
    <table class="table table-hover table-striped" style="background:#fff;border:1px solid #cccccc;">
		<tr>
			<td><strong>Unique id:</strong></td>
			<td><?php echo $data['UNIQUE_ID']; ?></td>
		
			<td><strong>File Name:</strong></td>
			<td><?php echo $data['FILE_NAME']; ?></td>
		</tr>
		<tr>
			<td><strong>File primary key:</strong></td>
			<td><?php echo $data['FILE_PRIMARY_KEY']; ?></td>
		
			<td><strong>Changed by:</strong></td>
			<td><?php echo $data['CHANGED_BY']; ?></td>
		</tr>
		<tr>
			<td><strong>Date modified:</strong></td>
			<td>
				<?php
				if(isset($data['DATE_MODIFIED']) && strlen($data['DATE_MODIFIED']) == 8) {
					echo substr($data['DATE_MODIFIED'], 0, 4).'-'.substr($data['DATE_MODIFIED'], 4, 2).'-'.substr($data['DATE_MODIFIED'], 6, 2);
				} else {
					echo "&nbsp";
				}
				?>
			</td>
			<td><strong>Time modified:</strong></td>
			<td>
				<?php
				if(isset($data['TIME_MODIFIED']) && strlen($data['TIME_MODIFIED']) == 6) {
					echo substr($data['TIME_MODIFIED'], 0, 2).':'.substr($data['TIME_MODIFIED'], 2, 2).':'.substr($data['TIME_MODIFIED'], 4, 2);
				} else {
					echo "&nbsp";
				}
				?>
			</td>
		</tr>
		<tr>
			<td><strong>Field tag:</strong></td>
			<td><?php echo $data['FIELD_TAG']; ?></td>
		
			<td><strong>Previous Data:</strong></td>
			<td><?php echo $data['PREVIOUS_DATA']; ?></td>
		</tr>
		<tr>
			<td><strong>Current data:</strong></td>
			<td><?php echo $data['CURRENT_DATA']; ?></td>
		
			<td><strong>Reserved:</strong></td>
			<td><?php echo $data['RESERVED']; ?></td>
		</tr>
    </table>
	<?php endif; ?>
</div>
