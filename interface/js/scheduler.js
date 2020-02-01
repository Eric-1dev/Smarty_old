var cur_id;
function ajaxReady(){
	$('.active').click(function(){
		that = this;
		dialog('Подтверждение', 'Действительно хотите изменить состояние задания?', function(){
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
				data: { table: 'scheduler', id: id, state: state }
			});
		});
	});

	$('.del_job').click(function(){
		that = this;
		dialog('Подтверждение', 'Действительно хотите удалить задание?', function(){
			var id = getRowId(that);
			delJob(id, true);
		});
	});

	$('.edit_job').click(function(){
		cur_id = getRowId(this);
		job_edit_add();
	});

	$('.add_job').click(function(){
		cur_id = 0;
		job_edit_add();
	});
}

function delJob(id, showMsg){
	$.ajax({
		type: "POST",
		url: "php/job_delete.php",
		data: { id: id },
		success: function(result){
			if ( showMsg )
				message(result, 'ok', true);
		}
	});
}

function job_edit_add()
{
	editWin = new cWindow("Редактирование задания", 1000, 800, true, true);
	$.ajax({
		type: "POST",
		url: "php/job_edit_add.php",
		data: { id: cur_id },
		success: function(result){
			editWin.winFill(result);
			winReady(); // аналог document.Ready()
		}
	});

	var but = editWin.winAddButton("Отмена", function(){
		editWin.winClose();
	});

	but.focus();

	but_save_exit = editWin.winAddButton("Сохранить и выйти", function(){
		save_job(cur_id);
		editWin.winDestruct();
	});
	$(but_save_exit).addClass('save_but');

	but_save = editWin.winAddButton("Сохранить и продолжить", function(){
		save_job(cur_id);
	});
	$(but_save).addClass('save_but');
}

function winReady(){
// При изменении первого селекта: второй очищается, а лишние пункты скрываются
	$('.act_select').change(function(){
		checkSelector(this, true);
		$('.script_param').change();
	});

// После загрузки страницы скрываем ненужные пункты во вторых селектах
	$('.act_select').each(function(){
		checkSelector(this, false);
	});

	$('.script_param').change(function(){
		checkValSelector(editWin.winGetWinContent(), $(this).val());
	});

	$('.script_param').change();

	$('.empty_tbl').hide();

	addEvents('.tbl');
	$('.single').change();

	$('.add_time').click(function(){
		var new_time = $('.empty_tbl').clone(false);
		new_time.removeClass('empty_tbl');
		new_time.addClass('tbl');
		new_time.insertBefore($('.add_time'));
		addEvents(new_time);	// Заново назначаем события для новой строки, т.к. maskedinput не умеет в clone(true)
		new_time.fadeIn();
	});
}

// Функция добавления событий на объект '.tbl'
function addEvents(obj){
	// Маска ввода времени
	$.mask.definitions['H']='[012]';
	$.mask.definitions['M']='[012345]';
	$(obj).find('.starttime, .stoptime').mask('H9:M9',{
			placeholder: "_",
			completed: function()
				{
					var val = $(this).val().split(':');
					if ( val[0]*1 > 23) val[0] = '23';
					if ( val[1]*1 > 59) val[1] = '59';
				  $(this).val( val.join(':') );
				}
		}
	);

	// Функция кнопки удаления строки в расписании
	var tbl_height = $('.tbl').height();
	$(obj).find('.del_time').click(function(){
		var _time = $(this).parents('.tbl');
		_time.css('position', 'relative').animate({
			'opacity': 0.0,
			'position': 'fixed',
			'left': "+=200"
		}, 200, function(){
			$(_time).html('').css('height', tbl_height);
			$(_time).animate({
				'height': '0px'
			}, 100, function(){
				$(_time).remove();
				checkAll();
			});
		});
	});

	// Кнопка "разово"
	$(obj).find('.single').change(function(){
		if ( $(this).prop('checked') == true )
		{
			$('.empty_tbl').find('.col_stop').hide();
			$('.empty_tbl').find('.single').prop('checked', true);
			//$(this).parents('.tbl_cell_left').find('.col_stop').fadeOut();

			var ph_width = $(this).parents('.tbl_cell_left').find('.col_stop').width();	// Выебоны
			$(this).parents('.tbl_cell_left').find('.col_stop').fadeOut(200, function(){
				$(this).parents('.tbl_cell_left').find('.td_placeholder').width(ph_width).show().animate({
					'width': 0
				}, 100, function(){
					$(this).hide();
				});
			});
		}
		else
		{
			$('.empty_tbl').find('.col_stop').show();
			$('.empty_tbl').find('.single').prop('checked', false);
//			$(this).parents('.tbl_cell_left').find('.col_stop').fadeIn();

			$(this).parents('.tbl_cell_left').find('.col_stop').hide();
			var ph_width = $(this).parents('.tbl_cell_left').find('.col_stop').width();	// Выебоны
			$(this).parents('.tbl_cell_left').find('.td_placeholder').width(0).show().animate({
				'width': ph_width
			}, 100, function(){
				$(this).hide()
				$(this).parents('.tbl_cell_left').find('.col_stop').fadeIn(200);
			});
		}
		checkValues($(this).parents('.tbl_cell_left').find('.stopvalue'));
	});

	// Выделяемые дни недели
	$(obj).find('.dows').click(function(){
		if ( $(this).hasClass('dows_selected') )
			$(this).removeClass('dows_selected');
		else
			$(this).addClass('dows_selected');
	});

	// Только числа в start/stop-value
	$(obj).find('.startvalue, .stopvalue').keyup(function(){
		$(this).val($(this).val().replace(/[^0-9]+$/, ''));
	});

	// Корректность полей value/time
	$(obj).find('.startvalue, .stopvalue').blur(function(){
		checkValues(this);
	});

	$(obj).find('.startvalue, .stopvalue').each(function(){
		checkValues(this);
	});
}

function save_job(id){
	var job_name = $('.job_name').val();
	var command_id = $('.script_param').val();
	var job_desc = $('.job_desc').val();
	var active = 0;
	var times = [];
	var i = 0;
	if ( $('.script_active').prop('checked') )
		var active = 1;
	if ( !$('.tbl').length )
	{
		message('Заполните график!', 'warning', false);
		return;
	}
	$('.tbl').each(function(){	// собираем в кучу строки расписания
		times[i] = [];
		times[i][0] = $(this).find('.startvalue').val();
		times[i][1] = getTimeFromStr($(this).find('.starttime').val());
		if ( $(this).find('.single').prop('checked') )
			times[i][3] = -1;
		else
		{
			times[i][2] = $(this).find('.stopvalue').val();
			times[i][3] = getTimeFromStr($(this).find('.stoptime').val());
		}
		var days = 0;
		var offset;
		$(this).find('.dows').each(function(){
			if ( $(this).hasClass('dows_selected') )
			{
				offset = $(this).attr("value");
				days|= 1<<offset;
			}
		});
		times[i][4] = days;
		i++;
	});
	var str = JSON.stringify(times);
	$.ajax({
		url: "php/job_save.php",
		data: { id: cur_id, job_name: job_name, command_id: command_id,
				job_desc: job_desc, active: active, times: str },
		beforeSend: function(){
			$('#blocker').fadeIn(100);
			$('#waiting').show();
		},
		success: function(result){
			if ( cur_id )
			{
				var arr = result.split('|');
				message(arr[0], 'ok', true);
			}
			else
			{
				var arr = result.split('|');
				message(arr[0], 'ok', true);
				cur_id = arr[1];
			}
		},
		complete: function(){
			$('#blocker').fadeOut(100);
			$('#waiting').hide();
		}
	});
}

function getTimeFromStr(str){
	if ( str == '' )
		return -1;
	return str.split(':')[0]*60 + str.split(':')[1]*1;
}

function checkValues(obj){
	var value_min = $('.value_min').html();
	var value_max = $('.value_max').html();
	var single = $(obj).parents('.tbl_cell_left').find('.single').prop('checked');
	if ( $(obj).hasClass('startvalue') )
	{
		if ( ($(obj).val() >= value_min) && ($(obj).val() <= value_max) && ($(obj).val() != '') )
		{
			$(obj).removeClass('text_error');
			checkAll();
		}
		else
		{
			$(obj).addClass('text_error');
			checkAll();
		}
		return;
	}
	if ( (single == false) )
	{
		if ( $(obj).hasClass('stopvalue') )
		{
			if ( ($(obj).val() >= value_min) && ($(obj).val() <= value_max) && ($(obj).val() != '') )
			{
				$(obj).removeClass('text_error');
				checkAll();
			}
			else
			{
				$(obj).addClass('text_error');
				checkAll();
			}
			return;
		}
	}
	else
	{
		$(obj).removeClass('text_error');
		checkAll();
		return;
	}
}

function checkAll(){
	if ( $('.text_error').length )
		$('.save_but').prop('disabled', true);
	else
		$('.save_but').prop('disabled', false);
}