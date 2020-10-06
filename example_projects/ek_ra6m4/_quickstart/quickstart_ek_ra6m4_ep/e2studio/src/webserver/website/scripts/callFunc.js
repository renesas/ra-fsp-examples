var	gCalFuncElementID;
var gCalFuncArgument;
var	gCalFuncHttpRequest;
function CalFuncCreateHttpRequest()
{
	var	xmlhttp	= false;
	var e;
	if (window.XMLHttpRequest)
	{
		xmlhttp	= new XMLHttpRequest();
	}
	else
	{
		xmlhttp	= new ActiveXObject("Microsoft.XMLHTTP");
		try
		{
			httprequest=new	ActiveXObject("Msxml2.XMLHTTP");
		} 
		catch (e)
		{
			try
			{
				httprequest=new	ActiveXObject("Microsoft.XMLHTTP");
			} catch	(e)
			{
			}
		}
	}
	return xmlhttp
}
function CalFuncUpdateContents()
{
	if (gCalFuncHttpRequest.readyState == 4)
	{
		if (gCalFuncHttpRequest.status == 200)
		{
			document.getElementById(gCalFuncElementID).innerHTML = gCalFuncHttpRequest.responseText;
		}
	}
}
function callFunc(elementID, funcFileName, argument)
{
	gCalFuncHttpRequest	= CalFuncCreateHttpRequest();
	if (gCalFuncHttpRequest)
	{
        gCalFuncElementID = elementID;
		gCalFuncHttpRequest.onreadystatechange = CalFuncUpdateContents;
		gCalFuncHttpRequest.open('GET', funcFileName + "? -" + argument + " -nocache" + Math.random(), true);
		gCalFuncHttpRequest.send(null);
	}
}

