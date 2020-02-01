function ajaxReady(){
	$('.update').click(function(){
		var command_id = $(this).children('.update_command').val();
		rfSend( command_id, 0, function(){
			setTimeout(function(){
				loadWorkspace(window.location.href);
			}, 1000);
		});
	});

	$('.but_update').click(function(){
		loadWorkspace(window.location.href);
	});
}