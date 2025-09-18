$(document).ready(function () {

	$(function () {
		$('#UserName, #Password').bind("cut copy paste",function(e) {
			  e.preventDefault();
		});
	});

});