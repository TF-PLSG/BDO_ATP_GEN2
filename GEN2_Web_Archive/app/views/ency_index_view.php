</nav>
<div id="page-wrapper">
	<div class="container-fluid">

		<!-- Page Heading -->
		<div class="row">
			<div class="col-lg-12">
				<h3 class="page-header">
					<small>Password encryption</small>
				</h3>
				<div class="well filter-div">
					<?php if(is_array($err) && isset($err['is_error']) && $err['is_error'] !== false): ?>
					<div class="alert alert-info" role="alert"><?php echo $err['message']; ?></div>
					<?php endif;?>
					<form id="submitSearch" name="signinform" action="<?php echo PATHURL; ?>ency/index/" method="POST" class="form-signin" role="form" onsubmit="return checkfilter()">
						
						<table border="0" cellpadding="0" cellspacing="0" width="100%">
							<!--tr>
								<td valign="top">
									<div class="form-group">
										<label class="control-label">Salt:</label>
										<input type="text" maxlength="25" id="SALT" class="form-control" placeholder="SALT" name="data[SALT]" value="<?php echo isset($data['SALT'])? $data['SALT']:'';?>" >
									</div>
								</td>
							</tr>
							<tr-->
								<td>
									<div class="form-group">
										<label class="control-label">Password:</label>
										<input type="password" maxlength="100" id="PASSWORD" class="form-control" placeholder="PASSWORD" name="data[PASSWORD]" value="">
									</div>
								</td>
							</tr>
							<tr>
								<td>
									<div class="form-group">
										<label class="control-label">Encrypted Password:</label>
										<?php if(isset($encrypted) && !empty($encrypted)): 
											echo $encrypted; 
										else: 
											echo '-'; 
										endif; ?>
									</div>
								</td>
							</tr>
							<tr>
								<td valign="top">
									<div class="form-group text-center">
										<button class="btn btn-primary" type="submit">Submit</button>
									</div>
								</td>
							</tr>
						</table>
					</form>
				</div>
			</div>
		</div>
	</div>
</div>