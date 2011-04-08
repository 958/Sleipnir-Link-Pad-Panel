try {
	(function() {
		if (document == null) { return; }
		var target = 
			(function(win) {
				var doc = win.document;
				doc.body.attachEvent('onmousemove', function(e) {ae = elementFromPoint(e.x, e.y)});
				doc.body.fireEvent('onmousemove');
				if (ae.tagName.match(/FRAME|IFRAME/i)) {
					var fw = doc.frames; var fe = doc.getElementsByTagName(ae.tagName);
					for (var i = 0; i < fw.length; i++) {
						if (fe.item(i) == ae) {
							try {
								return (arguments.callee(fw.item(i)));
							}
							catch (e) {
								return (null);
							}
						}
					}
				} else { return (ae); }
			})(_window);
		while (target.nodeName != 'A' && target.nodeName != 'HTML') target = target.parentElement;
	
		var path = sleipnir.UserFolder + 'setting\\linkpad.tmp';
		var fso = new ActiveXObject('Scripting.FileSystemObject');
		var stream;
		try {
			stream = fso.CreateTextFile(path, false);
		} catch(e) {
			stream = fso.OpenTextFile(path, 8);
		}
	
		if ((target.nodeName == 'A') && (target.href != '') && (!target.href.match(/^javascript:/))) {
			stream.Write(target.innerText + '\r\n' + target.href + '\r\n');
		} else {
			stream.Write(document.title + '\r\n' + document.location.href + '\r\n');
		}
		stream.Close();
	})();
}catch(e){}
