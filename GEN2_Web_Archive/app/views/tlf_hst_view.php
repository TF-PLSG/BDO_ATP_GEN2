<!-- Sidebar Menu Items - These collapse to the responsive navigation menu on small screens -->
<script type="text/javascript" src="<?php echo WEBPATH; ?>js/tlf_hst.js"></script>
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
            <li><a href="<?php echo PATHURL; ?>batch/auth/" class="nav-items-link"><i class="fa fa-fw fa-list-alt"></i> BCH20</a></li>
			<?php if(isset($_SESSION['user']['ACTION_16']) && substr($_SESSION['user']['ACTION_16'], 2, 1) == 1): ?>
            	<li><a href="<?php echo PATHURL; ?>tlf/hst/" class="nav-items-link-active"><i class="fa fa-fw fa-edit"></i> HST01</a></li>
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
					<span class="page-header-text">HST01 Lookup</span>
					<div class="filter-div-settings text-right">
						<a href="#" id="exp-access" class="btn btn-transparent-outlined"><i class="fa fa-fw fa-upload"></i> Export</a>
						<a href="<?php echo PATHURL; ?>tlf/hst/" class="btn btn-transparent-outlined"><i class="fa fa-fw fa-undo"></i> Reset Filter</a>
						<a href="#" class="btn btn-transparent-outlined hide-filter"><i class="fa fa-fw fa-toggle-down"></i> Hide Filter</a>
					</div>
				</h3>
				
				<div class="well filter-div">
					<form name="signinform" action="<?php echo PATHURL; ?>tlf/hst/" method="POST" class="form-signin" role="form" onsubmit="return checkfilter()">
						<div class="row">
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>User Id</label>
									<input id="CHANGED_BY" maxlength="15" type="text" class="form-control" placeholder="User Id" name="data[CHANGED_BY]" value="<?php echo isset($data['CHANGED_BY'])? $data['CHANGED_BY']:'';?>" >
								</div>
							</div>
							<div class="col-sm-12 col-md-3 mb-3">
								<div class="form-group">
									<label>Date</label>
									<div>
										<div class="input-group date form_date" data-date="" data-date-format="yyyy-mm-dd" data-link-field="dtp_input2" data-link-format="yyyy-mm-dd">
											<input class="form-control" size="16" type="text" name="data[SYSTEM_DATE_START]" value="<?php echo isset($data['SYSTEM_DATE_START'])? $data['SYSTEM_DATE_START']:'';?>" readonly>
											<span class="input-group-addon"><span class="glyphicon glyphicon-remove"></span></span>
											<span class="input-group-addon"><span class="glyphicon glyphicon-calendar"></span></span>
										</div>
									</div>
									<input type="hidden" id="dtp_input2" value="" />
								</div>
							</div>
							<div class="col-sm-12 mb-3">
								<div class="form-group text-center">
									<button class="btn btn-primary border-radius-none" type="submit"><i class="fa fa-fw fa-search"></i>&nbsp;Search</button>
								</div>
							</div>
						</div>
					</form>
				</div>

				<?php if(isset($result['data']) && !empty($result['data'])): ?>
					<div class="grid">
						<table class="table table-hover custom-table tablestyle">
						<thead>
							<tr>
								<th>Unique Id</th>
								<th>File Name</th>
								<th>Modified By</th>
								<th>Date</th>
								<th>Time</th>
								<th>Previous Data</th>
								<th>Current Data</th>
							</tr>
						</thead>
						<tbody>
						<?php foreach($result['data'] as $val): ?>
							<tr>
							   <!-- <td> -->
								  <!-- <p class="anchor" data-mypath="<--?php echo PATHURL;?>">
									<--?php echo $val['UNIQUE_ID']; ?-->
								  <!-- </p>  -->
							  <!-- </td> -->
							  <td><?php echo $val['UNIQUE_ID']; ?></td>
							  <td><?php echo $val['FILE_NAME']; ?></td>
							  <td><?php echo $val['CHANGED_BY']; ?></td>
							  <td><?php 
								if(isset($val['DATE_MODIFIED']) && strlen($val['DATE_MODIFIED']) == 8) {
									echo substr($val['DATE_MODIFIED'], 0, 4).'-'.substr($val['DATE_MODIFIED'], 4, 2).'-'.substr($val['DATE_MODIFIED'], 6, 2);
								} else {
									echo "&nbsp";
								}
								?></td>
							  <td><?php $str= $val['TIME_MODIFIED'];echo  implode(":", str_split($str, 2)); ?></td>
							  <td><?php echo $val['PREVIOUS_DATA']; ?></td>
							  <td><?php echo $val['CURRENT_DATA']; ?></td>
							</tr>
					<?php endforeach; ?>
						</tbody>
						</table>
						<ul class="pager">
						
							<?php if($page == 1):?>
								<li class="disabled"><a href="#"><i class="fa fa-step-backward"></i> First</a></li>
								<li class="disabled"><a href="#"><i class="fa fa-backward"></i> Previous</a></li>
							<?php else: ?>
								<li><a href="<?php echo PATHURL.'tlf/hst/1'; ?>"><i class="fa fa-step-backward"></i> First</a></li>
								<li><a href="<?php echo PATHURL.'tlf/hst/'.($page-1); ?>"><i class="fa fa-backward"></i> Previous</a></li>
							<?php endif;?>
							
							<li><a href="#"><?php echo $page.'/'.$result['page_number'];?></a></li>
							<?php if(isset($result['page_number']) && $result['page_number'] > $page): ?>
								<li><a href="<?php echo PATHURL.'tlf/hst/'.($page+1); ?>">Next <i class="fa fa-forward"></i></a></li>
								<li><a href="<?php echo PATHURL.'tlf/hst/'.$result['page_number']; ?>">Last <i class="fa fa-step-forward"></i></a></li>
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
						<button type="button" class="btn btn-primary" id="exp-access-submit" data-path="<?php echo(PATHURL); ?>" >Log In</button>
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
							<form id="SubmitExportCsv" name="signinform" action="<?php echo PATHURL; ?>tlf/csv/hst/" method="POST" class="form-signin" role="form">
								<div class="modal-header">
									<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
									<h4 class="modal-title" id="myModalLabel">Select CSV Columns:</h4>
								</div>
								<div class="modal-body" id="expcols">
									<table class="table table-hover table-striped tablestyle">
										<?php $columns = array_chunk(array_keys($expColumns), 2); ?>
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

		<!-- Promotions Modal -->
		<div class="modal fade" id="promotionModal" tabindex="-1" role="dialog" aria-labelledby="myModalLabel" aria-hidden="true">
			<div class="modal-dialog">
				<div class="modal-content" >
					<div class="modal-header">
						<button type="button" class="close" data-dismiss="modal" aria-hidden="true">&times;</button>
						<h4 class="modal-title" id="myModalLabel">HST Details:</h4>
					</div>
					<div class="modal-body" id="modalContent">

					</div>
				</div>
			</div>
		</div>
	</div>
</div>
