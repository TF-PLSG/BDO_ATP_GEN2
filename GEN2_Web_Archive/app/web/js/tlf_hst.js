$(document).ready(function () {

	$(function () {
		$("#UNIQUE_ID").keydown(function (e) {
			// Allow: backspace, delete, tab, escape, enter and .
			if ($.inArray(e.keyCode, [46, 8, 9, 27, 13, 110, 190]) !== -1 ||
				 // Allow: Ctrl+A
				(e.keyCode == 65 && e.ctrlKey === true) || 
				// Allow: Ctrl+C
				(e.keyCode == 67 && e.ctrlKey === true) || 
				 // Allow: home, end, left, right
				(e.keyCode >= 35 && e.keyCode <= 39)) {
					 // let it happen, don't do anything
					 return;
			}
			// Ensure that it is a number and stop the keypress
			if ((e.shiftKey || (e.keyCode < 48 || e.keyCode > 57)) && (e.keyCode < 96 || e.keyCode > 105)) {
				e.preventDefault();
			}
		});
		$('#exp-access').bind('click', function(){
			$('#exp-access-popup').modal('show');
		});
		$('#submit-exp-csv').bind('click', function(){
			if($('input[name="exp[]"]:checked').length > 0) {
				$('#SubmitExportCsv').submit();
				$('#ExpModelColumns').modal('hide');
				$('input[name="exp[]"]').attr("checked", false);
			} else {
				alert('Please Select Columns.');
			}
		});
		$('#ExpModelColumns').on('hidden.bs.modal', function () {
			$('input[name="exp[]"]').attr("checked", false);
		});
		$('#exp-access-submit').bind('click', function(){
			var path_url = $("#exp-access-submit" ).data('path');
			params = {'UserName':$('#UserName').val(), 'Password':$('#Password').val()};
			
			$.ajax({
				url: path_url+'user/exp_access',
				type: 'POST',
				data: params,
				cache: false,
				success: function(data) {
					var obj = jQuery.parseJSON(data);
					if(obj.is_error == false) {
						$('#pwd_error_messages').addClass('hidden');
						$('#pwd_error_messages').removeClass('show');
						$('#exp-access-popup').modal('hide');
						$('#UserName').val('');
						$('#Password').val('');
						$('#ExpModelColumns').modal('show');
						$('#ExpModelColumns').resize();
					} else {
						$('#pwd_error_messages').html('');
						$('#pwd_error_messages').removeClass('hidden');
						$('#pwd_error_messages').addClass('show');
						var errors = obj['error'];
						for(var i in errors) {
							$('#pwd_error_messages').append('<p>'+errors[i]+'</p>');
						}
					}
				}
			});
		});
        $('.form_date').datetimepicker({
            weekStart: 1,
            todayBtn:  1,
            autoclose: 1,
            todayHighlight: 1,
            startView: 2,
            minView: 2,
            forceParse: 1
		});
        $(".hide-filter").click(function() {
			if ($(".filter-div").is(":visible")) {
				$(this).html('<i class="fa fa-fw fa-toggle-up"></i> Show Filter');
            }else {
				$(this).html('<i class="fa fa-fw fa-toggle-down"></i> Hide Filter');  
            }    
			$(".filter-div").toggle();
        });
	});
	function checkfilter() {
		if($('#CHANGED_BY').val() == '' && $('#dtp_input2').val() == '') {
			alert('Please enter value for atleast one search field.');
			return false;
		}
		if($('#CHANGED_BY').val() != '' ||  $('#dtp_input2').val() != '') {
			
			return true;
		}
		return false;
	}

	$("p.anchor").click(function() {
		var mypath =$(this).data("mypath");
		var UNIQUE_ID = event.srcElement.innerHTML;
		
		$.ajax({
            url: mypath+'tlf/hstinfo/'+UNIQUE_ID,
            type: 'GET',
            cache: false,
            success: function(data) {
                $('#modalContent').html(data);
                $('#promotionModal').modal('show');
            }
        });	
	});
	
});