function ajaxReady(){
	$('.table_blur td').addClass('clickable');

	$('.table_blur td').click(function(e){
		var row = $(this).parent();
		var get_set = row.find('.get_set').html();
		var mode = 0;
		if ( get_set == 'Get' )
			mode = 1;
		var command = row.find("input[type='hidden']").val();
		$('#cur_command').val(command);
		showValueSelector(e.pageX, e.pageY, mode);
		return false;
	});
	
	$('#send_but').click(function(){
		var node_id = $('#node_id').val();
		var command_id = $('#cur_command').val();
		var value = $('#value_text').val();
		if (value == '')
		{
			$('#value_text').focus();
			return;
		}
		rfSend( command_id, value );
		hideValueSelector();
	});
	
	$("#value_text").keydown(function(event){
		// Разрешаем: backspace, delete, tab и escape
		if ( event.keyCode == 46 || event.keyCode == 8 || event.keyCode == 9 ||
			// Разрешаем: Ctrl+A
			(event.keyCode == 65 && event.ctrlKey === true) ||
			// Разрешаем: home, end, влево, вправо
			(event.keyCode >= 35 && event.keyCode <= 39) ||
			// Разрешаем: знак минус
			event.keyCode == 109 ) {
				// Ничего не делаем
				return;
		}
		else if ( event.keyCode == 27 )
			hideValueSelector();
		else if ( event.keyCode == 13 )
			$('#send_but').click();
		else {
			// Убеждаемся, что это цифра, и останавливаем событие keypress
			if ( (event.keyCode < 48 || event.keyCode > 57) && (event.keyCode < 96 || event.keyCode > 105 ) ) {
				event.preventDefault();
			}
		}
	});
}

function showValueSelector(x, y, mode){
	if ( mode )
	{
		$('#value_header').html("Подтвердите отправку<br>");
		$('#value_text').hide().val('0');
	}
	else
		$('#value_header').html("Введите значение<br>");

	var _width = $('#value_selector').width();
	var _height = $('#value_selector').height();
	var _x = x, _y = y;
	if ( x + _width + 20 > $('body').width() )
		_x = $('body').width() - _width - 20;
	if ( y + _height + 20 > $('body').height() )
		_y = $('body').height() - _height - 20;
	$('#value_selector').css({'top': _y ,'left': _x}).fadeIn('fast');

	if ( mode )
		$('#send_but').focus();
	else
		$('#value_text').show().val('').focus();
}

function hideValueSelector()
{
	$('#value_selector').fadeOut('fast');
}

$(document).click( function(event){
	if( $(event.target).closest('#value_selector').length ) 
		return;
	hideValueSelector();
	event.stopPropagation();
});