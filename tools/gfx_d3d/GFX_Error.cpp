#include "gfx_i.h"

u8 GFXCheckError(HRESULT hr, const tCHAR *header)
{
	switch(hr)
	{
	case D3D_OK:
		return FALSE;
	case D3DERR_CONFLICTINGRENDERSTATE:
		LogMsg(LOG_FILE, header, L"The currently set render states cannot be used together.");
		return TRUE;
	case D3DERR_CONFLICTINGTEXTUREFILTER:
		LogMsg(LOG_FILE, header, L"The current texture filters cannot be used together.");
		return TRUE;
	case D3DERR_CONFLICTINGTEXTUREPALETTE:
		LogMsg(LOG_FILE, header, L"The current textures cannot be used simultaneously. This generally occurs when a multitexture device requires that all palletized textures simultaneously enabled also share the same palette.");
		return TRUE;
	case D3DERR_DEVICELOST:
		LogMsg(LOG_FILE, header, L"The device is lost and cannot be restored at the current time, so rendering is not possible.");
		return TRUE;
	case D3DERR_DEVICENOTRESET:
		LogMsg(LOG_FILE, header, L"The device cannot be reset.");
		return TRUE;
	case D3DERR_DRIVERINTERNALERROR:
		LogMsg(LOG_FILE, header, L"Internal driver error.");
		return TRUE;
	case D3DERR_INVALIDCALL:
		LogMsg(LOG_FILE, header, L"The method call is invalid. For example, a method's parameter may have an invalid value.");
		return TRUE;
	case D3DXERR_INVALIDDATA:
		LogMsg(LOG_FILE, header, L"The data is invalid.");
		return TRUE;
	case D3DERR_INVALIDDEVICE:
		LogMsg(LOG_FILE, header, L"The requested device type is not valid.");
		return TRUE;
	case D3DERR_MOREDATA:
		LogMsg(LOG_FILE, header, L"There is more data available than the specified buffer size can hold.");
		return TRUE;
	case D3DERR_NOTAVAILABLE:
		LogMsg(LOG_FILE, header, L"This device does not support the queried technique.");
		return TRUE;
	case D3DERR_NOTFOUND:
		LogMsg(LOG_FILE, header, L"The requested item was not found.");
		return TRUE;
	case D3DERR_OUTOFVIDEOMEMORY:
		LogMsg(LOG_FILE, header, L"Direct3D does not have enough display memory to perform the operation.");
		return TRUE;
	case D3DERR_TOOMANYOPERATIONS:
		LogMsg(LOG_FILE, header, L"The application is requesting more texture-filtering operations than the device supports.");
		return TRUE;
	case D3DERR_UNSUPPORTEDALPHAARG:
		LogMsg(LOG_FILE, header, L"The device does not support a specified texture-blending argument for the alpha channel.");
		return TRUE;
	case D3DERR_UNSUPPORTEDALPHAOPERATION:
		LogMsg(LOG_FILE, header, L"The device does not support a specified texture-blending operation for the alpha channel.");
		return TRUE;
	case D3DERR_UNSUPPORTEDCOLORARG:
		LogMsg(LOG_FILE, header, L"The device does not support a specified texture-blending argument for color values.");
		return TRUE;
	case D3DERR_UNSUPPORTEDCOLOROPERATION:
		LogMsg(LOG_FILE, header, L"The device does not support a specified texture-blending operation for color values.");
		return TRUE;
	case D3DERR_UNSUPPORTEDFACTORVALUE:
		LogMsg(LOG_FILE, header, L"The device does not support the specified texture factor value.");
		return TRUE;
	case D3DERR_UNSUPPORTEDTEXTUREFILTER:
		LogMsg(LOG_FILE, header, L"The device does not support the specified texture filter.");
		return TRUE;
	case D3DERR_WRONGTEXTUREFORMAT:
		LogMsg(LOG_FILE, header, L"The pixel format of the texture surface is not valid.");
		return TRUE;
	case D3DXERR_CANNOTATTRSORT:
		LogMsg(LOG_FILE, header, L"Attribute sort (D3DXMESHOPT_ATTRSORT) is not supported as an optimization technique.");
		return TRUE;

	case E_FAIL:
		LogMsg(LOG_FILE, header, L"An undetermined error occurred inside the Direct3D subsystem.");
		return TRUE;
	case E_INVALIDARG:
		LogMsg(LOG_FILE, header, L"An invalid parameter was passed to the returning function");
		return TRUE;
	case E_OUTOFMEMORY:
		LogMsg(LOG_FILE, header, L"Microsoft® Direct3D® could not allocate sufficient memory to complete the call.");
		return TRUE;
	/*case E_INVALIDCALL:
		LogMsg(LOG_FILE, header, L"The method call is invalid. For example, a method's parameter may have an invalid value.");
		return FALSE;*/
	}

	return FALSE;
}