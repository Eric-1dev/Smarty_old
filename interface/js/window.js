// Класс окна
// name - заголовок
// x,y - начальные размеры окна
// resize - возможность растягивать окно
// needConfCan - нужно ли подтверждать закрытие окна
function cWindow(name, x, y, resize, needConfCan)
{
	var state = 0;
	var xsize = x + 'px';
	var ysize = y + 'px';
	var xpos = ( $(window).width() - x )/2 + 'px';
	var ypos = ( $(window).height() - y )/2 + 'px';
	var resizable = resize;
	var needConfirmCancel = needConfCan;
	var name = name;
	var that = this;

//---------------------------------> Отрисовка контейнера окна
	var winMain = document.createElement("div");
	winMain.style.left = xpos;
	winMain.style.top = ypos;
	winMain.style.width = xsize;
	winMain.style.height = ysize;
	winMain.className = "window";
	winMain.style.display = "none";
//	document.body.insertBefore( winMain, document.getElementById('waiting') );
	document.body.appendChild(winMain);
	$(winMain).fadeIn();
	$(winMain).draggable({
		opacity: 0.5,
		handle: '.winHeader',
		containment: 'window',
		addClasses: false,
		stop: function(){
			xpos = winMain.style.left;
			ypos = winMain.style.top;
		}
	});
	if ( resizable )
	{
		$(winMain).resizable({
			containment: 'html',
			minWidth: 640,
			minHeight: 480,
			stop: function(event, ui){
				xsize = winMain.style.width;
				ysize = winMain.style.height;
			}
		});
	}
	$(winMain).attr("tabindex",-1).focus();
	$(winMain).keydown(function(e){
		switch (e.which)
		{
			case 13:
				return false;
				break;
			case 27:
				that.winClose();
				break;
		}
	});

//---------------------------------> Отрисовка блокера
	var blocker = document.createElement("div");
	blocker.className = "blocker";
	blocker.style.display = "none";
	document.body.insertBefore( blocker, winMain );
	$(blocker).fadeIn();
	
//---------------------------------> Отрисовка заголовка
	var winHeader = document.createElement("div");
	winHeader.className = "winHeader";
	winMain.appendChild(winHeader);
	if ( resizable )
	{
		winHeader.ondblclick = function() {
			that.winMinMax();
		};
	}
	winHeader.onmousedown = winHeader.onselectstart = function() {
		return false;
	}
	  
//---------------------------------> Отрисовка содержимого
	var winContent = document.createElement("div");
	winContent.className = "winContent";
	winMain.appendChild(winContent);

//---------------------------------> Отрисовка подвала
	var winFooter = document.createElement("div");
	winFooter.className = "winFooter";
	winMain.appendChild(winFooter);

//---------------------------------> Отрисовка имени окна
	var winName = document.createElement("div");
	winName.className = "winName";
	winName.innerHTML = name;
	winHeader.appendChild(winName);

//---------------------------------> Отрисовка блока кнопок заголовка
	var winButtons = document.createElement("div");
	winButtons.className = "winButtons";
	winHeader.appendChild(winButtons);

//---------------------------------> Отрисовка кнопки развернуть
	if ( resizable )
	{
		var winMinMaxBut = document.createElement("i");
		winMinMaxBut.className = "winTitleBut fa fa-window-maximize";
		winButtons.appendChild(winMinMaxBut);
		winMinMaxBut.onclick = function() {
			that.winMinMax();
		};
	}

//---------------------------------> Отрисовка кнопки закрыть
	var winCloseBut = document.createElement("i");
	winCloseBut.className = "winTitleBut fa fa-window-close";
	winButtons.appendChild(winCloseBut);
	winCloseBut.onclick = function() {
		that.winClose();
	};

//---------------------------------> Отрисовка блока кнопок подвала
	var winFooterBut = document.createElement("div");
	winFooterBut.className = "winFooterBut";
	winFooter.appendChild(winFooterBut);

//---------------------------------> Возвращает div контента
	this.winGetWinContent = function() {
		return winContent;
	}

//---------------------------------> Функция наполнения окна
	this.winFill = function(content, callback) {
		winContent.innerHTML = content;
		if ( callback )
			callback();
	}

//---------------------------------> Функция разворачивания/сворачивания
	this.winMinMax = function() {
		if (this.state)	
		{
			this.state = 0;
			$(winMain).animate({
				width: xsize,
				height: ysize,
				left: xpos,
				top: ypos,
				}, 400);
			winMinMaxBut.className = "winTitleBut fa fa-window-maximize";
			$(winMain).resizable("enable").draggable("enable");
		}
		else
		{
			this.state = 1;
			$(winMain).animate({
				width: "100%",
				height: "100%",
				left: "0px",
				top: "0px",
				}, 400);
			winMinMaxBut.className = "winTitleBut fa fa-window-restore";
			$(winMain).resizable("disable").draggable("disable");
		}
	}

//---------------------------------> Функция закрытия окна
	this.winClose = function() {
		if ( needConfirmCancel )
			dialog("Подтверждение закрытия", "Все несохраненные данные будут потеряны.<br>Вы действительно хотите закрыть?", function() {
				that.winDestruct();
			});
		else
			that.winDestruct();
	}

	this.winDestruct = function() {
		$(winMain).fadeOut(function() {
			$(this).remove()
		});
		
		$(blocker).fadeOut(function() {
			$(this).remove()
		});
		delete that;
	}

//---------------------------------> Функция добавления кнопки в подвал
	this.winAddButton = function(name, func) {
		var winBut = document.createElement("button");
		winBut.innerHTML = name;
		winBut.onclick = function() {
			func();
			winBut.blur();
		}
		winBut.className = "winBut";
		winFooterBut.insertBefore(winBut, winFooterBut.firstChild);
		return winBut;
	}
}