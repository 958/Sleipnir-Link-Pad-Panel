(function() {
	if (document == null) { return; }
	var doc = 
		(function(win) {
			var doc = win.document; var ae = doc.activeElement;
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
			} else { return (doc); }
		})(_window);
	var target = doc.activeElement;
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
