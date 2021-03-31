var	gDateStampFileName = "get_time.cgi?";
var	gDateStampElementID =	"realTimeClock";
var	gDateStampUpdateTimerStarted = 0;
var	gDateStampRequestCount	= 0;
var	gDateStampTimeOut;
var	gDateStampHttpRequest;
function DateStampCreateHttpRequest()
{
	var xmlhttp = false;
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
				httprequest = new ActiveXObject("Microsoft.XMLHTTP");
			} catch	(e)
			{
			}
		}
	}
	return xmlhttp
}
function DateStampRequestUpdate()
{
	gDateStampHttpRequest	= DateStampCreateHttpRequest();
	if (gDateStampHttpRequest)
	{
		gDateStampHttpRequest.onreadystatechange = DateStampUpdateContents;
		gDateStampHttpRequest.open('GET', gDateStampFileName + " -nocache" + Math.random(), true);
		gDateStampHttpRequest.send(null);
		gDateStampRequestCount++;
	}
}
function DateStampUpdateContents()
{
	if (gDateStampHttpRequest.readyState == 4)
	{
		if (gDateStampHttpRequest.status == 200)
		{
			document.getElementById(gDateStampElementID).innerHTML = gDateStampHttpRequest.responseText;
			gDateStampRequestCount = 0;
		}
	}
}
function dateStampUpdateTimer()
{
	var	dateStampRefreshInterval = 0;
	switch (gDateStampRequestCount)
	{
		case 0:
		{
			dateStampRefreshInterval = 1000;
			break;
		}
		case 1:
		{
			dateStampRefreshInterval = 2000;
			break;
		}
		case 2:
		{
			dateStampRefreshInterval = 5000;
			break;
		}
		case 3:
		{
			dateStampRefreshInterval = 10000;
			break;
		}
		case 4:
		{
			dateStampRefreshInterval = 20000;
			break;
		}
		case 5:
		{
			dateStampRefreshInterval = 30000;
			break;
		}
		case 6:
		{
			dateStampRefreshInterval = 60000;
			break;
		}
		case 7:
		{
			dateStampRefreshInterval = 120000;
			break;
		}
		default:
		{
			dateStampRefreshInterval = 0;
			break
		}
	}
	DateStampRequestUpdate();
	if (dateStampRefreshInterval)
	{
		gDateStampTimeOut = setTimeout("dateStampUpdateTimer()", dateStampRefreshInterval);
	}
}
function DateStampScreenUpdate()
{
	if (!gDateStampUpdateTimerStarted)
	{
		gDateStampUpdateTimerStarted = 1;
		dateStampUpdateTimer();
	}
}
