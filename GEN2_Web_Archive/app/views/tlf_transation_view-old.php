<!-- Sidebar Menu Items - These collapse to the responsive navigation menu on small screens -->
<script type="text/javascript" src="<?php echo WEBPATH; ?>js/tlf_transaction.js"></script>
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
            <li><a href="<?php echo PATHURL; ?>tlf/transaction/" class="nav-items-link-active"><i class="fa fa-fw fa-file"></i> TLF01</a></li>
            <li><a href="<?php echo PATHURL; ?>batch/header/" class="nav-items-link"><i class="fa fa-fw fa-table"></i> BCH01</a></li>
            <li><a href="<?php echo PATHURL; ?>batch/details/" class="nav-items-link"><i class="fa fa-fw fa-file-text"></i> BCH11</a></li>
            <li><a href="<?php echo PATHURL; ?>batch/auth/" class="nav-items-link"><i class="fa fa-fw fa-list-alt"></i> BCH20</a></li>
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
			<div class="col-lg-12 resultset">
				<h3 class="page-header">
					<span class="page-header-text">TLF01 Lookup</span>
					<div class="filter-div-settings text-right">
						<?php if(isset($_SESSION['user']['ACTION_16']) && substr($_SESSION['user']['ACTION_16'], 0, 1) == 1): ?>
							<!-- <?php if(isset($data['UNMASK']) && $data['UNMASK'] == 1) : ?>
								<a href="#" class="btn btn-xs btn-info mask-card-number unmask"><i class="fa fa-fw fa-circle-o"></i> Mask CC</a>
							<?php else: ?>
								<a href="#" class="btn btn-xs btn-info mask-card-number mask"><i class="fa fa-fw fa-dot-circle-o"></i> UnMask CC</a>
							<?php endif;?> -->
						<?php endif;?>
						<a href="#" id="exp-access" class="btn btn-transparent-outlined"><i class="fa fa-fw fa-upload"></i> Export</a>
						<a href="<?php echo PATHURL; ?>tlf/transaction/" class="btn btn-transparent-outlined"><i class="fa fa-fw fa-undo"></i> Reset Filter</a>
						<a href="#" class="btn btn-transparent-outlined hide-filter"><i class="fa fa-fw fa-toggle-down"></i> Hide Filter</a>
					</div>
				</h3>
				
				<div class="well filter-div">
					<form id="submitSearch" name="signinform" action="<?php echo PATHURL; ?>tlf/transaction/" method="POST" class="form-signin" role="form" onsubmit="return checkdate()">
					<input type="hidden" name="data[UNMASK]" value="<?php echo isset($data['UNMASK'])? $data['UNMASK']:0;?>" id="MASKUNMASK">
						<div class="row">
						<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Transaction ID</label>
									<input type="text" maxlength="50" class="form-control input-blue-border" placeholder="Transaction ID" id="TRANSACTION_ID" name="data[TRANSACTION_ID]" value="<?php echo isset($data['TRANSACTION_ID'])? $data['TRANSACTION_ID']:'';?>">
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Terminal ID</label>
									<input type="text" maxlength="8" class="form-control input-blue-border" placeholder="Terminal Id" id="TERMINAL_ID" name="data[TERMINAL_ID]" value="<?php echo isset($data['TERMINAL_ID'])? $data['TERMINAL_ID']:'';?>">
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Merchant ID</label>
									<input type="text" maxlength="15" class="form-control input-blue-border" placeholder="Merchant Id" id="MERCHANT_ID" name="data[MERCHANT_ID]" value="<?php echo isset($data['MERCHANT_ID'])? $data['MERCHANT_ID']:'';?>">
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Card Number</label>
									<input type="text" maxlength="21" class="form-control input-blue-border" placeholder="Card Number" id="CARD_NUM" name="data[CARD_NUM]" value="<?php echo isset($data['CARD_NUM'])? $data['CARD_NUM']:'';?>">									
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Auth ID</label>
									<input type="text" maxlength="10" class="form-control input-blue-border" placeholder="Auth Id" id="AUTH_NUMBER" name="data[AUTH_NUMBER]" value="<?php echo isset($data['AUTH_NUMBER'])? $data['AUTH_NUMBER']:'';?>">
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Transaction Date Start</label>
									<div>
										<div class="input-group date form_date" id="form_date1" data-date="" data-date-format="yyyy-mm-dd" data-link-field="dtp_input1" data-link-format="yyyy-mm-dd">
											<input id="dtp_input_1" class="form-control input-blue-border" size="16" type="text" name="data[SYSTEM_DATE_START]" value="<?php echo isset($data['SYSTEM_DATE_START'])? $data['SYSTEM_DATE_START']:'';?>" readonly>
											<span class="input-group-addon"><span class="glyphicon glyphicon-remove"></span></span>
											<span class="input-group-addon"><span class="glyphicon glyphicon-calendar"></span></span>
										</div>
									</div>
										<input type="hidden" id="dtp_input1" value="" />
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Transaction Date End</label>
									<div>
									<div class="input-group date form_date" id="form_date2" data-date="" data-date-format="yyyy-mm-dd" data-link-field="dtp_input2" data-link-format="yyyy-mm-dd">
											<input id="dtp_input_2" class="form-control input-blue-border" size="16" type="text" readonly name="data[SYSTEM_DATE_END]" value="<?php echo isset($data['SYSTEM_DATE_END'])? $data['SYSTEM_DATE_END']:'';?>">
											<span class="input-group-addon"><span class="glyphicon glyphicon-remove"></span></span>
											<span class="input-group-addon"><span class="glyphicon glyphicon-calendar"></span></span>
										</div>
										<input type="hidden" id="dtp_input2" value="" />
									</div>
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="row">
									<label class="ml-4">Transaction Time Period (HHMM)</label>
									<div class="col-sm-12">
									<div class="form-group" style="display: flex;">
										<!-- <label>Transaction Time Start:</label> -->
										<input type="text" maxlength="4" class="form-control input-blue-border mr-2" placeholder="Start Time 🕚" id="TRANSACTION_START_TIME" name="data[TRANSACTION_START_TIME]" value="<?php echo isset($data['TRANSACTION_START_TIME'])? $data['TRANSACTION_START_TIME']:'';?>">
										<input type="text" maxlength="4" class="form-control input-blue-border" placeholder="End Time 🕚" id="TRANSACTION_END_TIME" name="data[TRANSACTION_END_TIME]" value="<?php echo isset($data['TRANSACTION_END_TIME'])? $data['TRANSACTION_END_TIME']:'';?>">
									</div>
									</div>
									<!-- <div class="col-sm-6">
										<div class="form-group">
											<label>Transaction Time End:</label>
											<input type="text" maxlength="4" class="form-control input-blue-border" placeholder="HHMM" id="TRANSACTION_END_TIME" name="data[TRANSACTION_END_TIME]" value="<?php echo isset($data['TRANSACTION_END_TIME'])? $data['TRANSACTION_END_TIME']:'';?>">
										</div>
									</div> -->
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Host Name</label>
									<input type="text" maxlength="10" class="form-control input-blue-border" placeholder="Host Name" id="HOST_NAME" name="data[REDEMPTION_AMOUNT]" value="<?php echo isset($data['REDEMPTION_AMOUNT'])? $data['REDEMPTION_AMOUNT']:'';?>">
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<button class="btn btn-primary border-radius-none mt-4" type="submit" ><i class="fa fa-fw fa-search"></i>&nbsp;Search</button>
								</div>
							</div>
							
						</div>
					</form>
				</div>
				<?php if(isset($resultset['data']) && !empty($resultset['data'])): ?>
				<div class="overflow-visible">
						<h4 class="text--primary"><strong>Transaction list</strong></h4>
						<table class="table table-hover custom-table" id="tablestyle" >
						<thead>
							<tr>
								<th>Transaction Id</th> 
								<th>Card Number</th>
								<th>Merchant Id</th>
								<th>Terminal Id</th>
								<th>Tran Date</th>
								<th>Tran Time</th>
								<th>Tran Amount</th>
								<th>Resp Code</th>
								<th>Auth Id <!--Result Code--></th>
								<th>Acquirer ID</th>
								<th>Issuer ID</th>
								<th>Sys Trace Nbr</th>
								<th>Ref Nbr</th>
								<th>System Date</th>
							</tr>
						</thead>
						<tbody>
						<?php //echo '<pre>'; print_r($resultset); echo 'jabadyo'; ?>
						<?php foreach($resultset['data'] as $val): ?>
							<tr>
								<td>
								  <p  class="anchor" data-mydate=<?php echo trim($val['SYSTEM_DATE']); ?> data-mypath="<?php echo PATHURL; ?>"><?php echo trim($val['TRANSACTION_ID']); ?></p>
								</td>
								<td>
								<?php if(isset($data['UNMASK']) && $data['UNMASK'] == 1) : ?>
									<?php echo $val['CARD_NUM']; ?>
								<?php else: ?>
									<?php if(!empty($val['CARD_NUM']) && strlen($val['CARD_NUM']) > 10): ?>
										<?php echo substr($val['CARD_NUM'], 0, 6) . str_repeat("X", strlen($val['CARD_NUM']) - 10) . substr($val['CARD_NUM'], -4); ?>
									<?php endif; ?>
								<?php endif; ?>
								</td>
								<td><?php echo $val['MERCHANT_ID']; ?></td>
								<td><?php echo $val['TERMINAL_ID']; ?></td>
								<td><?php echo date('Y-m-d', strtotime($val['DATE_YYYYMMDD'])); ?></td>
								<td><?php echo $val['TIME_HHMMSS']; ?></td>
								<td><?php
									if(!empty($val["TOTAL_AMOUNT"])) {
										$TOTAL_AMOUNT = ltrim($val["TOTAL_AMOUNT"], 0);
										$TOTAL_AMOUNT = substr_replace($TOTAL_AMOUNT, '.', -2, 0);
									} else {
										$TOTAL_AMOUNT = '';
									}
									echo $TOTAL_AMOUNT;
									?>
								</td>
								<td><?php echo $val['RESPONSE_CODE']; //$val['PROCESSING_CODE']; ?></td>
								<td><?php echo $val['AUTH_NUMBER']; //$val['SECURITY_RESPONSE_CODE']; ?></td>
								<td><?php echo $val['ACQUIRER_ID']; ?></td>
								<td><?php echo $val['ISSUER_ID']; ?></td>
								<td><?php echo $val['SYS_TRACE_AUDIT_NUM']; ?></td>
								<td><?php echo $val['RETRIEVAL_REF_NUM']; ?></td>
								<td>
									<?php if(!empty($val['SYSTEM_DATE'])) {
										echo substr($val['SYSTEM_DATE'], 0, 4).'-'.substr($val['SYSTEM_DATE'], 4, 2).'-'.substr($val['SYSTEM_DATE'], 6, 2);
									} ?>
								</td>
														  
							</tr>
					<?php endforeach; ?>
						</tbody>
						</table>
					
				
					<ul class="pager">
						
						<?php if($page == 1):?>
							<li class="disabled"><a href="#"><i class="fa fa-step-backward"></i> First</a></li>
							<li class="disabled"><a href="#"><i class="fa fa-backward"></i> Previous</a></li>
						<?php else: ?>
							<li><a href="<?php echo PATHURL.'tlf/transaction/1'; ?>"><i class="fa fa-step-backward"></i> First</a></li>
							<li><a href="<?php echo PATHURL.'tlf/transaction/'.($page-1); ?>"><i class="fa fa-backward"></i> Previous</a></li>
						<?php endif;?>
						
						<li><a href="#"><?php echo $page.'/'.$resultset['page_number'];?></a></li>
						<?php if(isset($resultset['page_number']) && $resultset['page_number'] > $page): ?>
							<li><a href="<?php echo PATHURL.'tlf/transaction/'.($page+1); ?>">Next <i class="fa fa-forward"></i></a></li>
							<li><a href="<?php echo PATHURL.'tlf/transaction/'.$resultset['page_number']; ?>">Last <i class="fa fa-step-forward"></i></a></li>
						<?php else: ?>
							<li class="disabled"><a href="#">Next <i class="fa fa-forward"></i></a></li>
							<li class="disabled"><a href="#">Last <i class="fa fa-step-forward"></i></a></li>
						<?php endif; ?>
					</ul>
				</div>
				<?php elseif($error !== false): ?>
					<div class="alert alert-info" role="alert"><?php echo $error; ?></div>
				<?php elseif(isset($resultset['Total']) && $resultset['Total'] == 0): ?>
				
					<div class="alert alert-info" role="alert">No records found.</div>
				<?php endif; ?>
			</div>
		</div>
		<!-- Promotions Modal -->
		<div class="modal fade" id="tlfModel" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
			<div class="modal-dialog">
				<div class="modal-content" >
					<div class="modal-header">
						<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
						<h4 class="modal-title" id="myModalLabel">TLF01 Details:</h4>
					</div>
					<div class="modal-body" id="modalContent">

					</div>
				</div>
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
							<input type="text" id="UserName" placeholder="User Name" class="form-control input-blue-border">
						</div>
						<div class="form-group">
							<label class="control-label">Password:</label>
							<input type="password" id="Password" placeholder="Password" class="form-control input-blue-border">
						</div>
					</div>
					<div class="modal-footer">
						<button type="button" class="btn btn-primary" data-path="<?php echo(PATHURL) ?>" id="exp-access-submit" >Log In</button>
					</div>
				</div>
			</div>
		</div>
		<!-- Promotions Modal -->
		<div class="modal fade" id="ExpModelColumns" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
			<div class="modal-dialog">
				<div class="modal-content" >
					<?php if(!isset($resultset['Total']) || $resultset['Total'] == 0): ?>
							<div class="alert alert-info" role="alert">No records found:(</div>
						<?php else: ?>
							<form id="SubmitExportCsv" name="signinform" action="<?php echo PATHURL; ?>tlf/csv/transaction/" method="POST" class="form-signin" role="form">
					<div class="modal-header">
						<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
						<h4 class="modal-title" id="myModalLabel">Select CSV Columns:</h4>
					</div>
					<div class="modal-body" id="expcols">
						
							 <table class="table table-hover table-striped" id="tablestyle2">
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
					<div class="modal-footer text-center">
					<button type="button" class="btn btn-primary" id="submit-exp-csv" >Submit</button>
							
					</div>
					</form>
					<?php endif; ?>
				</div>
			</div>
		</div>
	</div>
	<input type="hidden" id="my_variable" value="<? echo($_SESSION['user']['ACTION_16']);?>"/>
	
</div>

