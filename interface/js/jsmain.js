$.ajaxSetup({
	async: true,
	type: "POST",
	beforeSend: function(){
		$('#waiting').show();
	},
	complete: function(){
		$('#waiting').hide();
	}
 });

window.onpopstate = function(event) {
	loadWorkspace(event.state.url);
};

function loadWorkspace(url){
	$.ajax({
		url: url,
		data: { ajax: true },
		success: function(result){
			$('#work_space').html(result);
			if ( typeof ajaxReady == 'function' )
				ajaxReady();
			ajaxReadyMain();
		},
		error: function(){
			message('Не могу загрузить страницу', 'warning');
		}
	});
}

$(document).ready(function(){
	var page = $('#page').val();
	if ( page != '' )
	{
		history.replaceState({url: page}, '', page);
		loadWorkspace(page);
	}
	$('.menu_but').click(function(e){
		e.preventDefault();
		var href = $(this).attr('href');
		loadWorkspace(href);
		$('.menu_but').removeClass('selected_page');
		$(this).addClass('selected_page');
		history.pushState({url: href}, '', href);
	});
});

function ajaxReadyMain(){
	$('.table_blur, .button').attr('unselectable','on')
	.css({'-moz-user-select':'-moz-none',
		'-moz-user-select':'none',
		'-o-user-select':'none',
		'-khtml-user-select':'none',
		'-webkit-user-select':'none',
		'-ms-user-select':'none',
		'user-select':'none'
	}).bind('selectstart', function(){ return false; });
		
	var tblWidth = -1;
	$('.table_blur').each(function(){
		if ( $(this).width() > tblWidth )
			tblWidth = $(this).width();
	}).each(function(){
		$(this).width(tblWidth);
	});
}

function message(str, type, reload, dest)
{
	type = typeof type !== 'undefined' ?  type : 'ok';
	reload = typeof reload !== 'undefined' ?  reload : false;
	dest = typeof dest !== 'undefined' ?  dest : false;
	if ( reload )
	{
		if ( dest )
		{
			loadWorkspace(dest);
			history.pushState({url: dest}, '', dest);
		}
		else
			loadWorkspace(window.location.href);
	}
	switch (type)
	{
		case 'warning':
			$('#message').removeClass("ok");
			$('#message').addClass("warning");
			$('#message_icon').removeClass("fa-info-circle");
			$('#message_icon').addClass("fa-warning");
			break;
		case 'ok':
			$('#message').removeClass("warning");
			$('#message').addClass("ok");
			$('#message_icon').addClass("fa-info-circle");
			$('#message_icon').removeClass("fa-warning");
			break;
	}

	$('#message_content').html(str);
	$('#message').stop(true, true).animate({
		bottom: '20px' },
		400).delay(2000);
	$('#message').animate({
		bottom: '-130px' },
		400);
}

function rfSend( command_id, value, callback )
{
	$.ajax({
		type: "POST",
		url: "php/send.php",
		data: { command_id: command_id, value: value },
		success: function(result){
			if (result == '0')
			{
				message('Сообщение отправлено', 'ok');
				if ( callback )
					callback();
			}
			else
				message(result);
		},
	});
}

function dialog(title, text, callback){
	dlg = new cWindow(title, 400, 200, false, false);
	dlg.winAddButton("Отмена", function(){
		dlg.winClose();
	});

	var but = dlg.winAddButton("Ок", function(){
		dlg.winClose();
		callback();
	});
	but.focus();
	dlg.winFill(text);
}

function getRowId(obj){
	return $(obj).parents('.tbl_row').attr('id');
}

// Скрывает ненужные пункты в селектах
function checkSelector(obj, erase){
	var selValue = obj.value;
	var nextSel;
	if ( $(obj).hasClass('script_node_id') )
		nextSel = $('.script_param');
	else
		nextSel = $(obj).nextAll('select');
	nextSel.children('option').each(function(){
		if ( $(this).hasClass('key_' + selValue) )
			$(this).show();
		else
			$(this).hide();
	});
	if (erase)
		$(nextSel).val('');
}

function checkValSelector(obj, value){
	$(obj).find('.com_key').hide();
	$(obj).find('.com_key_'+value).fadeIn();
	$('.norm_value_min').removeClass('value_min');
	$('.norm_value_max').removeClass('value_max');
	$(obj).find('.com_key_'+value+' .norm_value_min').addClass('value_min');
	$(obj).find('.com_key_'+value+' .norm_value_max').addClass('value_max');
}