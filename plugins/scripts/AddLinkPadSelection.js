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
						} catch (e) {
							return (null);
						}
					}
				}
			} else { return (doc); }
		})(_window);

	var range = doc.selection.createRange();

	var path = sleipnir.UserFolder + 'setting\\linkpad.tmp';
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	var stream;
	try {
		stream = fso.CreateTextFile(path, false);
	} catch(e) {
		stream = fso.OpenTextFile(path, 8);
	}
	if (range.htmlText != '') {
		var base = doc.getElementsByTagName('head')[0].getElementsByTagName('base');
		if ((base.length == 0) || (base[0].href == '')) {
			base = doc.location.href;
			var temp = base.split('/');
			if (temp[temp.length - 1] != '') { temp[temp.length - 1] = ''; }
			base = temp.join('/');
		} else {
			base = base[0].href;
		}
		var html = new ActiveXObject('htmlfile');
		html.write('<head><base href="'+base+'"></head><body>'+range.htmlText+'</body>');
		var links = html.body.getElementsByTagName('a');
		for (var i = 0, len = links.length; i < len; i++) {
			if ((links[i].href != '') && (!links[i].href.match(/^javascript:/))) {
				stream.Write(links[i].innerText + '\r\n' + links[i].href + '\r\n');
			}
		}
	} else {
		stream.Write(document.title + '\r\n' + document.location.href + '\r\n');
	}

	stream.Close();
})();
