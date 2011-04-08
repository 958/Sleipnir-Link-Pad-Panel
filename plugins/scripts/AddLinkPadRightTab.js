(function() {
	var path = sleipnir.UserFolder + 'setting\\linkpad.tmp';
	var fso = new ActiveXObject('Scripting.FileSystemObject');
	var stream;
	try {
		stream = fso.CreateTextFile(path, false);
	} catch(e) {
		stream = fso.OpenTextFile(path, 8);
	}

	for (var i = sleipnir.API.ActiveIndex+1, len = sleipnir.API.GetCount(); i < len; i++) {
		var id = sleipnir.API.GetDocumentID(i);
		var document = sleipnir.API.GetDocumentObject(id);
		if (document != null) {
			stream.Write(document.title + '\r\n' + document.location.href + '\r\n');
		}
	}

	stream.Close();
})();
