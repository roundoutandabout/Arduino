var xmlHttp=createXmlHttpObject();

function createXmlHttpObject() {
		if (window.XMLHttpRequest){
			xmlHttp=new XMLHttpRequest();
		} else {
			xmlHttp=new ActiveXObject('Microsoft.XMLHTTP');
		}
		return xmlHttp;
	}

function process() {
		if(xmlHttp.readyState==0 || xmlHttp.readyState==4) {
			xmlHttp.open('PUT','xml',true);
			xmlHttp.onreadystatechange=handleServerResponse;
			xmlHttp.send(null);
		}
		setTimeout('process()',1000);
	}

function handleServerResponse() {
		if(xmlHttp.readyState==4 && xmlHttp.status==200) {
			xmlResponse=xmlHttp.responseXML;
			xmldoc = xmlResponse.getElementsByTagName('response');
			message = xmldoc[0].firstChild.nodeValue;
			document.getElementById('runtime').innerHTML=message;
		}
	}