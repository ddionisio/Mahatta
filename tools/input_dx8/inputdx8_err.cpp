#include "input_i.h"

#define ERROR_DIMSG(msg, header) (LogMsg(LOG_FILE, header, msg))

/////////////////////////////////////
// Name:	
// Purpose:	
// Output:	
// Return:	
/////////////////////////////////////
 void DInputError(HRESULT hr, const tCHAR *header)
{
	switch(hr)
	{
	case DI_BUFFEROVERFLOW:
		ERROR_DIMSG(L"The device buffer overflowed and some input was lost. This value is equal to the S_FALSE standard COM return value.",header);
		break;
	case DI_DOWNLOADSKIPPED:
		ERROR_DIMSG(L"The parameters of the effect were successfully updated, but the effect could not be downloaded because the associated device was not acquired in exclusive mode.",header);
		break;
	case DI_EFFECTRESTARTED:
		ERROR_DIMSG(L"The effect was stopped, the parameters were updated, and the effect was restarted.",header);
		break;
	case DI_POLLEDDEVICE:
		ERROR_DIMSG(L"The device is a polled device. As a result, device buffering does not collect any data and event notifications is not signaled until the IDirectInputDevice8::Poll method is called.",header);
		break;
	case DI_SETTINGSNOTSAVED:
		ERROR_DIMSG(L"The action map was applied to the device, but the settings could not be saved.",header);
		break;
	case DI_TRUNCATED:
		ERROR_DIMSG(L"The parameters of the effect were successfully updated, but some of them were beyond the capabilities of the device and were truncated to the nearest supported value.",header);
		break;
	case DI_TRUNCATEDANDRESTARTED:
		ERROR_DIMSG(L"Equal to DI_EFFECTRESTARTED | DI_TRUNCATED.",header);
		break;
	case DI_WRITEPROTECT:
		ERROR_DIMSG(L"A SUCCESS code indicating that settings cannot be modified.",header);
		break;
	case DIERR_ACQUIRED:
		ERROR_DIMSG(L"The operation cannot be performed while the device is acquired.",header);
		break;
	case DIERR_ALREADYINITIALIZED:
		ERROR_DIMSG(L"This object is already initialized",header);
		break;
	case DIERR_BADDRIVERVER:
		ERROR_DIMSG(L"The object could not be created due to an incompatible driver version or mismatched or incomplete driver components.",header);
		break;
	case DIERR_BETADIRECTINPUTVERSION:
		ERROR_DIMSG(L"The application was written for an unsupported prerelease version of DirectInput.",header);
		break;
	case DIERR_DEVICEFULL:
		ERROR_DIMSG(L"The device is full.",header);
		break;
	case DIERR_DEVICENOTREG:
		ERROR_DIMSG(L"The device or device instance is not registered with DirectInput. This value is equal to the REGDB_E_CLASSNOTREG standard COM return value.",header);
		break;
	case DIERR_EFFECTPLAYING:
		ERROR_DIMSG(L"The parameters were updated in memory but were not downloaded to the device because the device does not support updating an effect while it is still playing.",header);
		break;
	case DIERR_GENERIC:
		ERROR_DIMSG(L"An undetermined error occurred inside the DirectInput subsystem. This value is equal to the E_FAIL standard COM return value.",header);
		break;
	case DIERR_HANDLEEXISTS:
		ERROR_DIMSG(L"The device already has an event notification associated with it. This value is equal to the E_ACCESSDENIED standard COM return value.",header);
		break;
	case DIERR_HASEFFECTS:
		ERROR_DIMSG(L"The device cannot be reinitialized because effects are attached to it.",header);
		break;
	case DIERR_INCOMPLETEEFFECT:
		ERROR_DIMSG(L"The effect could not be downloaded because essential information is missing. For example, no axes have been associated with the effect, or no type-specific information has been supplied.",header);
		break;
	case DIERR_INPUTLOST:
		ERROR_DIMSG(L"Access to the input device has been lost. It must be reacquired.",header);
		break;
	case DIERR_INVALIDPARAM:
		ERROR_DIMSG(L"An invalid parameter was passed to the returning function, or the object was not in a state that permitted the function to be called. This value is equal to the E_INVALIDARG standard COM return value.",header);
		break;
	case DIERR_MAPFILEFAIL:
		ERROR_DIMSG(L"An error has occured either reading the vendor-supplied action-mapping file for the device or reading or writing the user configuration mapping file for the device.",header);
		break;
	case DIERR_MOREDATA:
		ERROR_DIMSG(L"Not all the requested information fit into the buffer.",header);
		break;
	case DIERR_NOAGGREGATION:
		ERROR_DIMSG(L"This object does not support aggregation.",header);
		break;
	case DIERR_NOINTERFACE:
		ERROR_DIMSG(L"The object does not support the specified interface. This value is equal to the E_NOINTERFACE standard COM return value.",header);
		break;
	case DIERR_NOTACQUIRED:
		ERROR_DIMSG(L"The operation cannot be performed unless the device is acquired.",header);
		break;
	case DIERR_NOTBUFFERED:
		ERROR_DIMSG(L"The device is not buffered. Set the DIPROP_BUFFERSIZE property to enable buffering.",header);
		break;
	case DIERR_NOTDOWNLOADED:
		ERROR_DIMSG(L"The effect is not downloaded.",header);
		break;
	case DIERR_NOTEXCLUSIVEACQUIRED:
		ERROR_DIMSG(L"The operation cannot be performed unless the device is acquired in DISCL_EXCLUSIVE mode.",header);
		break;
	case DIERR_NOTFOUND:
		ERROR_DIMSG(L"The requested object does not exist.",header);
		break;
	case DIERR_NOTINITIALIZED:
		ERROR_DIMSG(L"This object has not been initialized.",header);
		break;
	case DIERR_OLDDIRECTINPUTVERSION:
		ERROR_DIMSG(L"The application requires a newer version of DirectInput.",header);
		break;
	case DIERR_OUTOFMEMORY:
		ERROR_DIMSG(L"The DirectInput subsystem could not allocate sufficient memory to complete the call. This value is equal to the E_OUTOFMEMORY standard COM return value.",header);
		break;
	case DIERR_REPORTFULL:
		ERROR_DIMSG(L"More information was requested to be sent than can be sent to the device.",header);
		break;
	case DIERR_UNPLUGGED:
		ERROR_DIMSG(L"The operation could not be completed because the device is not plugged in.",header);
		break;
	case DIERR_UNSUPPORTED:
		ERROR_DIMSG(L"The function called is not supported at this time. This value is equal to the E_NOTIMPL standard COM return value.",header);
		break;
	}
}