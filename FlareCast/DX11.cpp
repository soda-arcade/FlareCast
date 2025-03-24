﻿#include "DX11.h"

#define SAFE_RELEASE(x) {if(x != nullptr) x->Release();}
#define CLEAN_ON_FAIL(hr) {if (FAILED(hr)) goto CLEANUP;}

D3D_FEATURE_LEVEL gFeatureLevels[] = {
	D3D_FEATURE_LEVEL_11_0,
	D3D_FEATURE_LEVEL_10_1,
	D3D_FEATURE_LEVEL_10_0,
	D3D_FEATURE_LEVEL_9_3,
	D3D_FEATURE_LEVEL_9_2,
	D3D_FEATURE_LEVEL_9_1,
};
UINT gNumFeatureLevels = 6;
ID3D11Device* _lDevice;
ID3D11DeviceContext* _lImmediateContext;
IDXGIOutputDuplication* _lDeskDupl;
DXGI_OUTPUT_DESC _lOutputDesc;
DXGI_OUTDUPL_DESC _lOutputDuplDesc;
ID3D11Texture2D* _lAcquiredDesktopImage;
D3D11_MAPPED_SUBRESOURCE _resource;


void DX11::clear()
{
	if (_lDeskDupl != NULL) _lDeskDupl->Release();
	if (_lAcquiredDesktopImage != NULL) _lAcquiredDesktopImage->Release();
}

bool DX11::recover()
{
	HRESULT hr;

	// Get DXGI device
	IDXGIDevice* lDxgiDevice = 0;
	hr = _lDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&lDxgiDevice));

	if (FAILED(hr))
	{
		if (lDxgiDevice != nullptr) lDxgiDevice->Release();
		return false;
	}

	// Get DXGI adapter
	IDXGIAdapter* lDxgiAdapter;
	hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&lDxgiAdapter));

	if (FAILED(hr))
	{
		if (lDxgiAdapter != nullptr) lDxgiAdapter->Release();
		return false;
	}

	//DXGI_ADAPTER_DESC adapterDesc;
	//hr = lDxgiAdapter->GetDesc(&adapterDesc);

	lDxgiDevice->Release();

	// Get output
	IDXGIOutput* lDxgiOutput;
	hr = lDxgiAdapter->EnumOutputs(_currentScreen, &lDxgiOutput);

	if (FAILED(hr))
	{
		if (lDxgiOutput != nullptr) lDxgiOutput->Release();
		return false;
	}

	lDxgiAdapter->Release();

	hr = lDxgiOutput->GetDesc(&_lOutputDesc);

	if (FAILED(hr))
	{
		return false;
	}

	// QI for Output 1
	IDXGIOutput1* lDxgiOutput1;
	hr = lDxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&lDxgiOutput1);

	if (FAILED(hr))
	{
		if (lDxgiOutput1 != nullptr) lDxgiOutput1->Release();
		return false;
	}

	lDxgiOutput->Release();

	// Create desktop duplication
	hr = lDxgiOutput1->DuplicateOutput(_lDevice, &_lDeskDupl);

	if (FAILED(hr))
	{
		if (_lDeskDupl != nullptr) _lDeskDupl->Release();
		return false;
	}

	lDxgiOutput1->Release();

	// Create GUI drawing texture
	_lDeskDupl->GetDesc(&_lOutputDuplDesc);
	// Create CPU access texture
	_desc.Width = _lOutputDuplDesc.ModeDesc.Width;
	_desc.Height = _lOutputDuplDesc.ModeDesc.Height;
	_desc.Format = _lOutputDuplDesc.ModeDesc.Format;
	std::cout << _desc.Width << "x" << _desc.Height << "\n\n\n";

	_desc.ArraySize = 1;
	_desc.BindFlags = 0;
	_desc.MiscFlags = 0;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.MipLevels = 1;
	_desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
	_desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;

	return true;
}

bool DX11::recover2()
{
	bool success = false;

	if (_gpus.size() <= 0 || _currentGPU >= _gpus.size()) return success;

	HRESULT hr;
	IDXGIAdapter1* dxgiAdapter = _gpus[_currentGPU].adapter;
	IDXGIOutput* dxgiOutput = nullptr;
	IDXGIOutput1* dxgiOutput1 = nullptr;
	IDXGIDevice* dxgiDevice = nullptr;

	hr = D3D11CreateDevice(
		dxgiAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL, NULL, NULL, NULL,
		D3D11_SDK_VERSION,
		&_lDevice,
		nullptr,
		&_lImmediateContext
	);
	CLEAN_ON_FAIL(hr);

	hr = dxgiAdapter->EnumOutputs(_currentScreen, &dxgiOutput);
	CLEAN_ON_FAIL(hr);

	hr = dxgiOutput->QueryInterface(__uuidof(dxgiOutput1), reinterpret_cast<void**>(&dxgiOutput1));
	CLEAN_ON_FAIL(hr);

	hr = _lDevice->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
	CLEAN_ON_FAIL(hr);

	dxgiOutput1->DuplicateOutput(dxgiDevice, &_lDeskDupl);
	CLEAN_ON_FAIL(hr);

	if (_lDeskDupl != nullptr)
	{
		_lDeskDupl->GetDesc(&_lOutputDuplDesc);
		_desc.Width = _lOutputDuplDesc.ModeDesc.Width;
		_desc.Height = _lOutputDuplDesc.ModeDesc.Height;
		_desc.Format = _lOutputDuplDesc.ModeDesc.Format;
		_desc.ArraySize = 1;
		_desc.BindFlags = 0;
		_desc.MiscFlags = 0;
		_desc.SampleDesc.Count = 1;
		_desc.SampleDesc.Quality = 0;
		_desc.MipLevels = 1;
		_desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
		_desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;
	}

	success = true;

CLEANUP:
	//SAFE_RELEASE(_lDevice);
	//SAFE_RELEASE(_lImmediateContext);
	//SAFE_RELEASE(dxgiAdapter);
	SAFE_RELEASE(dxgiOutput);
	SAFE_RELEASE(dxgiOutput1);
	SAFE_RELEASE(dxgiDevice);

	return success;
}

bool DX11::clearAndRecover()
{
	_mutex.lock();
	clear();
	bool result = recover();
	_mutex.unlock();
	return result;
}

void DX11::enumGPUS()
{
	HRESULT hr;

	IDXGIFactory1* dxgiFactory = nullptr;
	hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(&dxgiFactory));
	if (FAILED(hr))
	{
		if (dxgiFactory != nullptr) dxgiFactory->Release();
		return;
	}

	vector<GPU>::iterator it;
	for (it = _gpus.begin(); it != _gpus.end(); ++it)
	{
		if ((*it).adapter != nullptr)
		{
			(*it).adapter->Release();
		}
	}
	_gpus.clear();
	_gpuNames.clear();

	IDXGIAdapter1* dxgiAdapter = nullptr;
	for (UINT i = 0; i < 20; i++)
	{
		hr = dxgiFactory->EnumAdapters1(i, &dxgiAdapter);
		if (FAILED(hr))
		{
			break;
		}

		DXGI_ADAPTER_DESC1 desc;
		dxgiAdapter->GetDesc1(&desc);

		_gpus.push_back(GPU(dxgiAdapter, desc));

		std::string wname = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(desc.Description);
		_gpuNames.push_back("[" + to_string(i) + "] " + wname);
	}

	if (_currentGPU >= _gpus.size())
	{
		_currentGPU = 0;
	}

	if (dxgiFactory != nullptr) dxgiFactory->Release();
}

bool DX11::init()
{
	_currentScreen = Config::cfg.video.monitor;
	_currentGPU = Config::cfg.video.adapter;
	enumGPUS();

	int lresult(-1);

	D3D_FEATURE_LEVEL lFeatureLevel;

	IDXGIAdapter* idxgiAdapter = nullptr;
	if (!_gpus.empty() && _currentGPU < _gpus.size())
	{
		idxgiAdapter = _gpus[_currentGPU].adapter;
	}

	HRESULT hr(E_FAIL);
	hr = D3D11CreateDevice(
		idxgiAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		nullptr,
		0,
		gFeatureLevels,
		gNumFeatureLevels,
		D3D11_SDK_VERSION,
		&_lDevice,
		&lFeatureLevel,
		&_lImmediateContext);

	if (FAILED(hr))
		return false;

	if (!_lDevice)
		return false;

	fetchScreenList();
	if (_screens.empty() || _currentScreen >= _screens.size()) {
		_currentScreen = 0;
	}

	// Get DXGI device
	IDXGIDevice* lDxgiDevice;
	hr = _lDevice->QueryInterface(__uuidof(IDXGIAdapter), (void**)&lDxgiDevice);

	if (FAILED(hr))
	{
		if (lDxgiDevice != nullptr) lDxgiDevice->Release();
		return false;
	}

	// Get DXGI adapter
	IDXGIAdapter* lDxgiAdapter;
	hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&lDxgiAdapter);

	if (FAILED(hr))
	{
		if (lDxgiAdapter != nullptr) lDxgiAdapter->Release();
		return false;
	}

	lDxgiDevice->Release();

	// Get output
	IDXGIOutput* lDxgiOutput;
	hr = lDxgiAdapter->EnumOutputs(_currentScreen, &lDxgiOutput);

	if (FAILED(hr))
	{
		if (lDxgiOutput != nullptr) lDxgiOutput->Release();
		return false;
	}

	lDxgiAdapter->Release();

	hr = lDxgiOutput->GetDesc(&_lOutputDesc);

	if (FAILED(hr))
		return false;

	// QI for Output 1
	IDXGIOutput1* lDxgiOutput1;
	hr = lDxgiOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&lDxgiOutput1);

	if (FAILED(hr))
	{
		if (lDxgiOutput1 != nullptr) lDxgiOutput1->Release();
		return false;
	}

	lDxgiOutput->Release();

	// Create desktop duplication
	hr = lDxgiOutput1->DuplicateOutput(_lDevice, &_lDeskDupl);

	if (FAILED(hr))
	{
		if (_lDeskDupl != nullptr) _lDeskDupl->Release();
		return false;
	}

	lDxgiOutput1->Release();

	// Create CPU access texture
	_lDeskDupl->GetDesc(&_lOutputDuplDesc);
	_desc.Width = _lOutputDuplDesc.ModeDesc.Width;
	_desc.Height = _lOutputDuplDesc.ModeDesc.Height;
	_desc.Format = _lOutputDuplDesc.ModeDesc.Format;
	_desc.ArraySize = 1;
	_desc.BindFlags = 0;
	_desc.MiscFlags = 0;
	_desc.SampleDesc.Count = 1;
	_desc.SampleDesc.Quality = 0;
	_desc.MipLevels = 1;
	_desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
	_desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;

	return true;
}


bool DX11::captureScreen(ParsecDSO* ps)
{
	static ID3D11Texture2D* lastFramePointer;

	if (!_lDeskDupl)
	{
		if (!clearAndRecover())
		{
			return false;
		}
	}

	_mutex.lock();

	HRESULT hr(E_FAIL), hr0(E_FAIL);
	IDXGIResource* lDesktopResource = nullptr;
	DXGI_OUTDUPL_FRAME_INFO lFrameInfo;
	ID3D11Texture2D* currTexture = NULL;

	hr = _lDeskDupl->AcquireNextFrame(4, &lFrameInfo, &lDesktopResource);
	if (FAILED(hr)) {
		_lDeskDupl->ReleaseFrame();

		_mutex.unlock();
		if (hr != DXGI_ERROR_WAIT_TIMEOUT)
		{
			if (lDesktopResource != nullptr) lDesktopResource->Release();
			if (lastFramePointer != nullptr) lastFramePointer->Release();
			clearAndRecover();
		}

		return false;
	}

	// QI for ID3D11Texture2D
	hr = lDesktopResource->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&_lAcquiredDesktopImage);

	//////////////////////////////////////// 
	// LEL
	//////////////////////////////////////// 
	ParsecHostD3D11SubmitFrame(ps, 0, _lDevice, _lImmediateContext, _lAcquiredDesktopImage);
	lastFramePointer = _lAcquiredDesktopImage;

	_lDeskDupl->ReleaseFrame();

	_mutex.unlock();
	return true;
}

vector<string> DX11::listScreens()
{
	return _screens;
}

void DX11::setScreen(UINT index)
{
	if (index < _screens.size())
	{
		_currentScreen = index;
		Config::cfg.video.monitor = index;
		Config::cfg.Save();
		clearAndRecover();
	}
}

UINT DX11::getScreen()
{
	return _currentScreen;
}

vector<string> DX11::listGPUs()
{
	return _gpuNames;
}

void DX11::setGPU(size_t index)
{
	if (index < _gpus.size())
	{
		_mutex.lock();
		_currentGPU = index;
		Config::cfg.video.adapter = index;
		Config::cfg.Save();
		fetchScreenList();
		init();
		_mutex.unlock();
	}
}

size_t DX11::getGPU()
{
	return _currentGPU;
}

void DX11::fetchScreenList()
{
	HRESULT hr;

	// Get DXGI device
	IDXGIDevice* lDxgiDevice = 0;
	hr = _lDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&lDxgiDevice);

	if (FAILED(hr))
	{
		if (lDxgiDevice != nullptr) lDxgiDevice->Release();
		return;
	}

	// Get DXGI adapter
	IDXGIAdapter* lDxgiAdapter;
	hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&lDxgiAdapter);

	if (FAILED(hr))
	{
		if (lDxgiAdapter != nullptr) lDxgiAdapter->Release();
		return;
	}

	lDxgiDevice->Release();

	_screens.clear();
	UINT screen = 0;
	IDXGIOutput* lDxgiOutput;
	while (lDxgiAdapter->EnumOutputs(screen, &lDxgiOutput) != DXGI_ERROR_NOT_FOUND)
	{
		hr = lDxgiOutput->GetDesc(&_lOutputDesc);

		if (FAILED(hr))
		{
			_screens.push_back(
				string() + "[" + to_string(screen) + "] " + "Unknown Adapter"
			);
			screen++;
			continue;
		}

		std::string name = std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(_lOutputDesc.DeviceName);
		int width = _lOutputDesc.DesktopCoordinates.right - _lOutputDesc.DesktopCoordinates.left;
		int height = _lOutputDesc.DesktopCoordinates.bottom - _lOutputDesc.DesktopCoordinates.top;
		_screens.push_back("[" + to_string(screen) + "] " + name + " (" + to_string(width) + "x" + to_string(height) + ")");

		lDxgiOutput->Release();

		screen++;
	}

	lDxgiAdapter->Release();
}


