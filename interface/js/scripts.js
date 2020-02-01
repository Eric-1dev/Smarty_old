function ajaxReady(){
	$('.active').click(function(){
		that = this;
		dialog('Подтверждение', 'Действительно хотите изменить состояние скрипта?', function(){
			var id = getRowId(that);
			var state;

			if ( $(that).hasClass('fa-check-square-o') )
			{
				$(that).removeClass('fa-check-square-o');
				$(that).addClass('fa-square-o');
				state = 0;
			}
			else
			{
				$(that).removeClass('fa-square-o');
				$(that).addClass('fa-check-square-o');
				state = 1;
			}
			$.ajax({
				type: "POST",
				url: "php/row_activate.php",
				data: { table: 'scripts', id: id, state: state }
			});
		});
	});

	$('.div_action').click(function(){
		$('.selected').removeClass('selected');
		$(this).addClass('selected');
	});

	$('.div_action').dblclick(function(){
		var obj = $(this);
		action_move(obj);
	});

	$('.move_but').click(function(){
		var obj = $('.selected');
		action_move(obj);
	});

	$('.up_down').click(function(){
		var obj = $('.selected');
		if ( obj.parent().hasClass('div_left_td') )
		{
			var obj2;
			if ( $(this).hasClass('but_up') )
			{
				obj2 = obj.prev('.div_action');
				obj.insertBefore(obj2);
			}
			else
			{
				obj2 = obj.next('.div_action');
				obj.insertAfter(obj2);
			}
		}
	});

	// При изменении первого селекта: второй очищается, а лишние пункты скрываются
	$('.act_select').change(function(){
		checkSelector(this, true);
	});

	// После загрузки страницы скрываем ненужные пункты во вторых селектах
	$('.act_select').each(function(){
		checkSelector(this, false);
	});

	$('.div_right_td').find($("select, input[type='text']")).hide();

	$('.but_save').click(function(){
		var script_id = $('.script_id').val();
		var script_name = $('.script_name').val();
		var script_node_id = $('.script_node_id').val();
		var script_param = $('.script_param').val();
		var script_value_min = $('.script_value_min').val();
		var script_value_max = $('.script_value_max').val();
		var script_active;
		if ( $('.script_active').prop("checked") )
			script_active = 1;
		else
			script_active = 0;

		var actions = [];
		var i=0;
		$('.div_left_td').children('.div_action').each(function(){
			actions[i] = [];
			actions[i][0] = $(this).children('.actionId').val();
			actions[i][1] = $(this).children('.value1').val();
			actions[i][2] = $(this).children('.value2').val();
			i++;
		});
		var str = JSON.stringify(actions);
		$.ajax({
			type: "POST",
			url: "php/script_save.php",
			data: { script_id: script_id, script_name: script_name,
					script_node_id: script_node_id, script_param: script_param,
					script_value_min: script_value_min, script_value_max: script_value_max,
					script_active: script_active, actions: str },
			success: function(result){
				if ( script_id )
					message(result, 'ok', true);
				else
				{
					var arr = result.split('|');
					message(arr[0], 'ok', true, 'scripts_edit.php?script='+arr[1]);
				}
			}
		});
	});

	$('.add_script').click(function(){
		href='scripts_edit.php';
		loadWorkspace(href);
		history.pushState({url: href}, '', href);
	});

	$('.del_script').click(function(){
		var that = this;
		dialog('Подтверждение', 'Действительно хотите удалить скрипт?', function(){
			id = getRowId(that);
			if ( id )
			{
				$.ajax({
					type: "POST",
					url: "php/script_delete.php",
					data: { id: id },
					success: function(result){
						message(result, 'ok', true);
					}
				});
			}
		});
	});

	$('.edit_script').click(function(){
		var href = 'scripts_edit.php?script='+getRowId(this);
		loadWorkspace(href);
		history.pushState({url: href}, '', href);
	});
}

function action_move(obj){
	if ( $(obj).parent().hasClass('div_left_td') )
	{
		$(obj).css('position', 'relative').animate({
			'opacity': 0.0,
			'position': 'fixed',
			'left': "+=500"
		}, 400, function(){
			$(this).remove();
		});
	}
	else
	{
		var obj_clone = obj.clone(true);
		$('.div_left_td').append(obj_clone);
		$(obj_clone).find($("select, input[type='text']")).show();
		$(obj_clone).click();
	}
}