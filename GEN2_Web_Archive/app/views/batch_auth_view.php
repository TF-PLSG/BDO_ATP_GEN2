<!-- Sidebar Menu Items - These collapse to the responsive navigation menu on small screens -->
<script type="text/javascript" src="<?php echo WEBPATH; ?>js/batch_auth.js"></script>
	<div class="navbar navbar-default nav-items-navbar" role="navigation">
      <div class="container">
        <div class="navbar-header">
          <button type="button" class="navbar-toggle" data-toggle="collapse" data-target=".navbar-collapse">
            <span class="sr-only">Toggle navigation</span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
            <span class="icon-bar"></span>
          </button>
        </div>
        <div class="navbar-collapse collapse">
          <ul class="nav navbar-nav">
            <li><a href="<?php echo PATHURL; ?>tlf/transaction/" class="nav-items-link"><i class="fa fa-fw fa-file"></i> TLF01</a></li>
            <li><a href="<?php echo PATHURL; ?>batch/header/" class="nav-items-link"><i class="fa fa-fw fa-table"></i> BCH01</a></li>
            <li><a href="<?php echo PATHURL; ?>batch/details/" class="nav-items-link"><i class="fa fa-fw fa-file-text"></i> BCH11</a></li>
            <li><a href="<?php echo PATHURL; ?>batch/auth/" class="nav-items-link-active"><i class="fa fa-fw fa-list-alt"></i> BCH20</a></li>
			<?php if(isset($_SESSION['user']['ACTION_16']) && substr($_SESSION['user']['ACTION_16'], 2, 1) == 1): ?>
            	<li><a href="<?php echo PATHURL; ?>tlf/hst/" class="nav-items-link"><i class="fa fa-fw fa-edit"></i> HST01</a></li>
			<?php endif; ?>
          </ul>
        </div><!--/.nav-collapse -->
      </div>
    </div>
<div id="page-wrapper">
	<div class="container page-body">

		<!-- Page Heading -->
		<div class="row">
			<div class="col-lg-12">
				<h3 class="page-header">
					<span class="page-header-text">BCH20 Authorization Batch Details Lookup</span>
					<div class="filter-div-settings text-right">
						<a href="#" id="exp-access" class="btn btn-transparent-outlined"><i class="fa fa-fw fa-upload"></i> Export</a>
						<a href="<?php echo PATHURL; ?>batch/auth/" class="btn btn-transparent-outlined"><i class="fa fa-fw fa-undo"></i> Reset Filter</a>
						<a href="#" class="btn btn-transparent-outlined hide-filter"><i class="fa fa-fw fa-toggle-down"></i> Hide Filter</a>
					</div>
				</h3>
				
				<div class="well filter-div">
					<form id="submitSearch" name="signinform" action="<?php echo PATHURL; ?>batch/auth/" method="POST" class="form-signin" role="form" onsubmit="">
									<?php
				if(isset($data['UNMASK'])) {
				if($data['UNMASK'] != 0 && $data['UNMASK'] != 1) {
					$data['UNMASK'] = 0;
				}
				}
					
				?>
					<input type="hidden" name="data[UNMASK]" value="<?php echo isset($data['UNMASK'])? $data['UNMASK']:0;?>" id="MASKUNMASK">
					<div class="row">
						<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Device Id</label>
									<input type="text" maxlength="10" id="DEVICE_ID" class="form-control" placeholder="Device Id" name="data[DEVICE_ID]" value="<?php echo isset($data['DEVICE_ID'])? htmlentities($data['DEVICE_ID']):'';?>" >
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Merchant Id</label>
									<input type="text" maxlength="15" id="MERCHANT_ID" class="form-control" placeholder="Merchant Id" name="data[MERCHANT_ID]" value="<?php echo isset($data['MERCHANT_ID'])? htmlentities($data['MERCHANT_ID']):'';?>">
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Batch Number</label>
									<input type="text" maxlength="10" id="BATCH_NBR" class="form-control" placeholder="Batch Number" name="data[BATCH_NBR]" value="<?php echo isset($data['BATCH_NBR'])? htmlentities($data['BATCH_NBR']):'';?>">
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label> Retrieval Ref No</label>
									<input type="text" maxlength="15" id="RETRIEVAL_REF_NUM" class="form-control" placeholder="Retrieval Ref No" name="data[RETRIEVAL_REF_NUM]" value="<?php echo isset($data['RETRIEVAL_REF_NUM'])? htmlentities($data['RETRIEVAL_REF_NUM']):'';?>">
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Transaction Date</label>
									<div>
										<div class="input-group date form_date" data-date="" data-date-format="yyyy-mm-dd" data-link-field="dtp_input1" data-link-format="yyyy-mm-dd">
											<input class="form-control" size="16" type="text" name="data[TRAN_DATE]" value="<?php echo isset($data['TRAN_DATE'])? htmlentities($data['TRAN_DATE']):'';?>" readonly>
											<span class="input-group-addon"><span class="glyphicon glyphicon-remove"></span></span>
											<span class="input-group-addon"><span class="glyphicon glyphicon-calendar"></span></span>
										</div>
									</div>
									<input type="hidden" id="dtp_input1" value="" />
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Open Date</label>
									<div>
										<div class="input-group date form_date" data-date="" data-date-format="yyyy-mm-dd" data-link-field="dtp_input2" data-link-format="yyyy-mm-dd">
											<input class="form-control" size="16" type="text" readonly name="data[OPEN_DATE]" value="<?php echo isset($data['OPEN_DATE'])? htmlentities($data['OPEN_DATE']):'';?>">
											<span class="input-group-addon"><span class="glyphicon glyphicon-remove"></span></span>
											<span class="input-group-addon"><span class="glyphicon glyphicon-calendar"></span></span>
										</div>
										<input type="hidden" id="dtp_input2" value="" />
									</div>
								</div>
							</div>
							<div class="col-sm-12 mb-3 mt-3">
								<div class="form-group text-center">
									<button class="btn btn-primary border-radius-none" type="submit"><i class="fa fa-fw fa-search"></i>&nbsp;Search</button>
								</div>
							</div>
						</div>
					</form>
				</div>

				<?php if(isset($result['data']) && !empty($result['data'])): ?>
					<div class="overflow-visible">
						<table class="table table-hover custom-table tablestyle">
						<thead>
							<tr>
								<th>Transaction Id</th>
								<th>Device Id</th>
								<th>Batch Number</th>
								<th>Merchant Id</th>
								<th>Card Number &nbsp; &nbsp; <a href="javascript:toggleMask()"><span class="toggleBtn"><i class="fa fa-eye-slash"></i></span></a></th>
								<th>Retrieval Ref number</th>
								<th>Transaction Date</th>
								<th>Open Date</th>
								<th>Ticket Number</th>
							</tr>
						</thead>
						<tbody>
						<?php foreach($result['data'] as $val): ?>
							<tr>
							   <td>
								  <p class="anchor" data-mypath="<?php echo PATHURL;?>">
									<?php echo $val['TRANSACTION_ID']; ?>
								  </p>
							  </td>
							  <td><?php echo $val['DEVICE_ID']; ?></td>
							  <td><?php echo $val['BATCH_NBR']; ?></td>
							  <td><?php echo $val['MERCHANT_ID']; ?></td>
							  <td>
									<?php $maskCardNo = ""; if(isset($data['UNMASK']) )  {?>
									<?php if(!empty($val['CARD_NBR']) && strlen($val['CARD_NBR']) > 10) {
										$maskCardNo = substr($val['CARD_NBR'], 0, 6) . str_repeat("X", strlen($val['CARD_NBR']) - 10) . substr($val['CARD_NBR'], -4);
									}  ?>
									<span class="cardNumber" data-mask="<?php echo $maskCardNo; ?>" data-unmask="<?php echo $val['CARD_NBR']; ?>">
									<?php } ?><?php echo $maskCardNo; ?></span>
							  </td>
							  <td><?php echo $val['RETRIEVAL_REF_NUM']; ?></td>
							  <td>
									<?php 
									if(!empty($val['TRAN_DATE'])) {
										echo date('Y-m-d', strtotime($val['TRAN_DATE']));
									} else {
										echo $val['TRAN_DATE'];
									} ?>
								</td>
							  <td>
									<?php 
									if(!empty($val['OPEN_DATE'])) {
										echo date('Y-m-d', strtotime($val['OPEN_DATE']));
									} else {
										echo $val['OPEN_DATE'];
									} ?>
								</td>
								<td><?php echo $val['TICKET_NBR']; ?></td>
							</tr>
					<?php endforeach; ?>
						</tbody>
						</table>
						<ul class="pager">
						
							<?php if($page == 1):?>
								<li class="disabled"><a href="#"><i class="fa fa-step-backward"></i> First</a></li>
								<li class="disabled"><a href="#"><i class="fa fa-backward"></i> Previous</a></li>
							<?php else: ?>
								<li><a href="<?php echo PATHURL.'batch/auth/1'; ?>"><i class="fa fa-step-backward"></i> First</a></li>
								<li><a href="<?php echo PATHURL.'batch/auth/'.($page-1); ?>"><i class="fa fa-backward"></i> Previous</a></li>
							<?php endif;?>
							
							<li><a href="#"><?php echo $page.'/'.$result['page_number'];?></a></li>
							<?php if(isset($result['page_number']) && $result['page_number'] > $page): ?>
								<li><a href="<?php echo PATHURL.'batch/auth/'.($page+1); ?>">Next <i class="fa fa-forward"></i></a></li>
								<li><a href="<?php echo PATHURL.'batch/auth/'.$result['page_number']; ?>">Last <i class="fa fa-step-forward"></i></a></li>
							<?php else: ?>
								<li class="disabled"><a href="#">Next <i class="fa fa-forward"></i></a></li>
								<li class="disabled"><a href="#">Last <i class="fa fa-step-forward"></i></a></li>
							<?php endif; ?>
						</ul>
					</div>
				<?php elseif($error !== false): ?>
					<div class="alert alert-info" role="alert"><?php echo $error; ?></div>
				<?php elseif(isset($result['Total']) && $result['Total'] == 0): ?>
					<div class="alert alert-info" role="alert">No records found.</div>
				<?php endif; ?>
			</div>
		</div>
	
		<div class="modal fade" id="exp-access-popup" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="false">
			<div class="modal-dialog">
				<div class="modal-content">
					<div class="modal-header">
						<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
						<h4 class="modal-title" id="myModalLabel">Authentication</h4>
					</div>
					<div class="alert alert-danger hidden" id="pwd_error_messages"></div>
			 
					<div class="modal-body">
						<div class="form-group">
							<label class="control-label">User Name:</label>
							<input type="text" id="UserName" placeholder="User Name" class="form-control">
						</div>
						<div class="form-group">
							<label class="control-label">Password:</label>
							<input type="password" id="Password" placeholder="Password" class="form-control">
						</div>
					</div>
					<div class="modal-footer">
						<button type="button" class="btn btn-primary" data-path="<?php echo(PATHURL); ?>" id="exp-access-submit" >Log In</button>
					</div>
				</div>
			</div>
		</div>
		<!-- Promotions Modal -->
		<div class="modal fade" id="ExpModelColumns" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
			<div class="modal-dialog">
				<div class="modal-content" >
					<?php if(!isset($result['Total']) || $result['Total'] == 0): ?>
							<div class="alert alert-info" role="alert">No records found:(</div>
						<?php else: ?>
							<form id="SubmitExportCsv" name="signinform" action="<?php echo PATHURL; ?>batch/csv/auth/" method="POST" class="form-signin" role="form">
					<div class="modal-header">
						<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
						<h4 class="modal-title" id="myModalLabel">Select CSV Columns:</h4>
					</div>
					<div class="modal-body" id="expcols">
						
							 <table class="table table-hover table-striped tablestyle">
								<?php $columns = array_chunk(array_keys($expColumns), 4); ?>
								<?php foreach($columns as $m): ?>
								<tr>
								<?php foreach($m as $n): ?>
									<td><input type="checkbox" value="<?php echo $n; ?>" name="exp[]"></td>
									<td><strong><?php echo $expColumns[$n]; ?></strong></td>
								<?php endforeach; ?>
								</tr>
								<?php endforeach; ?>
							</table>
					</div>
					<input type="hidden" name="card_num_mask" id="card_num_mask" value="1">
					<div class="modal-footer text-center">
					<button type="button" class="btn btn-primary" id="submit-exp-csv" >Submit</button>
							
					</div>
					</form>
					<?php endif; ?>
				</div>
			</div>
		</div>
		<!-- Promotions Modal -->
		<div class="modal fade" id="promotionModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
			<div class="modal-dialog">
				<div class="modal-content" >
					<div class="modal-header">
						<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
						<h4 class="modal-title" id="myModalLabel">Batch Auth Details:</h4>
					</div>
					<div class="modal-body" id="modalContent">

					</div>
				</div>
			</div>
		</div>
		<input type="hidden" id="my_variable" value="<? echo($_SESSION['user']['ACTION_16']);?>"/>
	</div>
</div>


<script>
var mask = 1;
function toggleMask() {
	if(mask == 1) {
		$('.toggleBtn').html('<i class="fa fa-eye"></i>');
		mask = 0;
		$('.cardNumber').each(function(i,v) {
		text = $(v).data('unmask');
		$(v).html(text);
		$('#card_num_mask').val(1);
	});
	} else {
		$('.toggleBtn').html('<i class="fa fa-eye-slash"></i>');
		mask = 1;
		$('.cardNumber').each(function(i,v) {
		text = $(v).data('mask');
		$(v).html(text);
		$('#card_num_mask').val(0);
	});
	}
	
	
}
function decrypt(str) {
	str = atob(str);
  var decoded = "";
  for (i=0; i<str.length;i++) {
	var a = str.charCodeAt(i);
	var b = a ^ 51;    // bitwise XOR with any number, e.g. 123
	decoded = decoded+String.fromCharCode(b);
  }
  return decoded;
}
function encrypt(str) {
  var encoded = "";
  for (i=0; i<str.length;i++) {
	var a = str.charCodeAt(i);
	var b = a ^ 51;    // bitwise XOR with any number, e.g. 123
	encoded = encoded+String.fromCharCode(b);
  }
  return btoa(encoded);
}

</script>