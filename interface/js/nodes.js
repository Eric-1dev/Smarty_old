function ajaxReady(){
	$('.nodes_row').each(function(){
		$(this).children('td:not(:last)').addClass('clickable');
	});
	$('.clickable').click(function(){
		var node = $(this).parent().children('.node_id').html();
		var href = 'commands.php?node='+node;
		loadWorkspace(href);
		history.pushState({url: href}, '', href);
	});

	$('.update').click(function(){
		var node = $(this).parent().prevAll('.node_id').html();
		pingNode( node);
	});
}

function pingNode(node, callback)
{
	$.ajax({
		type: "POST",
		url: "php/send.php",
		data: { command_id: 123, value: node },
		success: function(result){
			message('Статус узла обновлен', 'ok', true);
			if ( callback )
				callback();
		},
	});
}