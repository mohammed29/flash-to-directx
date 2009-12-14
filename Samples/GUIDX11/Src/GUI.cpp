//---------------------------------------------------------------------
// Copyright (c) 2009 Maksym Diachenko, Viktor Reutskyy, Anton Suchov.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//---------------------------------------------------------------------

#include "stdafx.h"

#include "GUI.h"

// Libs
#pragma comment (lib, "D3D11.lib")
#pragma comment (lib, "DXErr.lib")
#pragma comment (lib, "DXGI.lib")
#pragma comment (lib, "d3dcompiler.lib")
#ifdef _DEBUG
#pragma comment (lib, "FlashDXDebug_Win32.lib")
#else
#pragma comment (lib, "FlashDXRelease_Win32.lib")
#endif // _DEBUG
//#pragma comment(lib, "FlashDX" CONFIGURATION_NAME "_" PLATFORM_NAME ".lib")

int window_width = 700;
int window_height = 350;

const size_t indexCount = 6;

const wchar_t* movie_path = L"Data/VT.swf";
const IFlashDXPlayer::ETransparencyMode transparency_mode = IFlashDXPlayer::TMODE_FULL_ALPHA;

HWND hWnd; 

IFlashDX*			g_flashDX = NULL;
IFlashDXPlayer*		g_flashPlayer = NULL;

ASInterface*		g_playerASI;

ID3D11Device *pDevice11 = NULL;
IDXGISwapChain *pSwapChain = NULL;
ID3D11DeviceContext *pImmediateContext = NULL;

ID3D11RenderTargetView *m_pRenderTargetView = NULL;
ID3D11Texture2D *pBackBuffer11 = NULL;



ID3D11Texture2D *pTexture  = NULL;
ID3D11ShaderResourceView* g_pTextureRV = NULL;

ID3D11VertexShader*					g_pVertexShader = NULL;
ID3D11PixelShader*					g_pPixelShader = NULL;

ID3D11InputLayout*                  g_pVertexLayout = NULL;

ID3D11Buffer*                       g_pVertexBuffer = NULL;
ID3D11Buffer*                       g_pIndexBuffer = NULL;

struct Vertex
{
	D3DXVECTOR3 Pos;  
	D3DXVECTOR4	Col;
    D3DXVECTOR2 Tex;
};


LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut );

HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register windowclass
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(wc));
	wc.cbSize = sizeof(wc);
	wc.lpszClassName = TEXT("MyClass");
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&wc);

	// Create window
	RECT rc = { 0, 0, window_width, window_height };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// Create window
	hWnd = CreateWindow(wc.lpszClassName, TEXT("Flash GUI in DirectX 11 application"),
							WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,	0,
							rc.right - rc.left, rc.bottom - rc.top,
							NULL, NULL, hInstance, NULL);

	return S_OK;
}

HRESULT InitDevice()
{
	// Create DXGI factory to enumerate adapters
	IDXGIFactory1 *pDXGIFactory;

	HRESULT hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&pDXGIFactory);
	if(FAILED(hResult)) {
		MessageBox(NULL, DXGetErrorDescription(hResult), TEXT("CreateDXGIFactory1"), MB_OK);
		return 0;
	}

	// Use the first adapter
	IDXGIAdapter1 *pAdapter;

	hResult = pDXGIFactory->EnumAdapters1(0, &pAdapter);
	if(FAILED(hResult)) {
		MessageBox(NULL, DXGetErrorDescription(hResult), TEXT("EnumAdapters1"), MB_OK);
		return 0;
	}

	pDXGIFactory->Release();

	// Create D3D11 device and swapchain
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	scd.SampleDesc.Count = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = TRUE;

	hResult = D3D11CreateDeviceAndSwapChain(
		pAdapter,
		D3D_DRIVER_TYPE_UNKNOWN,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG |
		D3D11_CREATE_DEVICE_BGRA_SUPPORT |
		D3D11_CREATE_DEVICE_SINGLETHREADED,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice11,
		NULL,
		&pImmediateContext
		);
	if(FAILED(hResult)) {
		MessageBox(NULL, DXGetErrorDescription(hResult), TEXT("D3D11CreateDeviceAndSwapChain"), MB_OK);
		return 0;
	}


	// Create vertex buffer
	{
		const size_t vertexCount = 4;
		Vertex vertices[vertexCount] =
		{
			{ D3DXVECTOR3( -1.0f, -1.0f, 0.5f ),D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ), D3DXVECTOR2( 0.0f, 1.0f ) },
			{ D3DXVECTOR3( 1.0f, -1.0f, 0.5f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ),D3DXVECTOR2( 1.0f, 1.0f ) },
			{ D3DXVECTOR3( 1.0f, 1.0f, 0.5f ),  D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ),D3DXVECTOR2( 1.0f, 0.0f ) },
			{ D3DXVECTOR3( -1.0f, 1.0f, 0.5f ), D3DXVECTOR4( 1.0f, 1.0f, 1.0f, 1.0f ),D3DXVECTOR2( 0.0f, 0.0f ) },
		};
		
		D3D11_BUFFER_DESC bd = {0};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( Vertex ) * vertexCount;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData = {0};
		InitData.pSysMem = vertices;
		HRESULT hr = pDevice11->CreateBuffer( &bd, &InitData, &g_pVertexBuffer );
		if( FAILED( hr ) )
			return hr;

		// Set vertex buffer
		UINT stride = sizeof( Vertex );
		UINT offset = 0;
		pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );
	}

	{
		// Create index buffer
	
		DWORD indices[indexCount] =
		{
			3,1,0,
			2,1,3,
		};

		D3D11_BUFFER_DESC bd = {0};
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof( DWORD ) * indexCount;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		bd.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData = {0};
		InitData.pSysMem = indices;

		HRESULT hr = pDevice11->CreateBuffer( &bd, &InitData, &g_pIndexBuffer );
		if( FAILED( hr ) )
			return hr;

		// Set index buffer
		pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0 );

		// Set primitive topology
		pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	}

	ID3DBlob* vs = NULL;
	ID3DBlob* ps = NULL;

	// create shaders
	{

		HRESULT
		hr = CompileShaderFromFile(L"Data\\GUI_dx11.hlsl", "RenderSceneVS", "vs_4_0", &vs);
		assert(SUCCEEDED(hr));
		hr = CompileShaderFromFile(L"Data\\GUI_dx11.hlsl", "RenderScenePS", "ps_4_0", &ps);
		assert(SUCCEEDED(hr));
		hr = pDevice11->CreateVertexShader( vs->GetBufferPointer(), vs->GetBufferSize(), NULL, &g_pVertexShader );
		assert(SUCCEEDED(hr));
		hr = pDevice11->CreatePixelShader( ps->GetBufferPointer(), ps->GetBufferSize(), NULL, &g_pPixelShader );
		assert(SUCCEEDED(hr));
	}

	// create input layout
	{
		// Create our vertex input layout
		const D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		
		UINT numElements = sizeof( layout ) / sizeof( layout[0] );
		HRESULT hr = pDevice11->CreateInputLayout( layout, numElements, vs->GetBufferPointer(),
			vs->GetBufferSize(), &g_pVertexLayout );
		assert(SUCCEEDED(hr));
	}

	SAFE_RELEASE(vs);
	SAFE_RELEASE(ps);

	RecreateTargets(window_width, window_height);

	return S_OK;
}

void DeinitDevice()
{
	// Release
	pSwapChain->SetFullscreenState(FALSE, NULL);

	pImmediateContext->ClearState();

	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pBackBuffer11);

	pImmediateContext->Flush();
	SAFE_RELEASE(pImmediateContext);
	SAFE_RELEASE(pSwapChain);
	SAFE_RELEASE(pDevice11);
}

bool InitFlashDX()
{
	//---------------------------------------------------------------------
	// Initialize Flash-to-DirectX
	//---------------------------------------------------------------------
	g_flashDX = GetFlashToDirectXInstance();
	double flashVersion = g_flashDX->GetFlashVersion();
	g_flashPlayer = g_flashDX->CreatePlayer(window_width, window_height);

	if (!g_flashPlayer)
	{
		MessageBox(NULL, L"Flash Player failed to initialize.", L"Error", MB_OK);
		return FALSE;
	}

	g_playerASI = new ASInterface(g_flashPlayer);

	//---------------------------------------------------------------------
	// Load and play movie
	//---------------------------------------------------------------------
	g_flashPlayer->LoadMovie(movie_path);

	g_flashPlayer->SetTransparencyMode(transparency_mode);
	g_flashPlayer->SetBackgroundColor(RGB(0, 0, 0));
	
	return true;
}

void DeinitFlash()
{
	delete g_playerASI;
	g_flashDX->DestroyPlayer(g_flashPlayer);
}

void Render()
{
// 	float ClearColor[4] = { 0, 1.0f, 1.0f, 1.0f };
// 	pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, clearColor);
	//pImmediateContext->IASetPrimitiveTopology( D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	pImmediateContext->IASetInputLayout( g_pVertexLayout );
	
	UINT stride = sizeof( Vertex );
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &stride, &offset );

	pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the shaders
	pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );

	unsigned int numDirtyRects = 0; const RECT* dirtyRects = NULL;
	if (g_flashPlayer->IsNeedUpdate(NULL, &dirtyRects, &numDirtyRects))
	{
		// Get DXGI surface from texture
		IDXGISurface1 *pSurface = NULL;

		HRESULT hResult = pTexture->QueryInterface(__uuidof(IDXGISurface1), (void**)&pSurface);
		assert(SUCCEEDED(hResult));

		for(unsigned i = 0; i < numDirtyRects; ++i)
		{
			// Get surface DC for the texture
			HDC hDC;
			hResult = pSurface->GetDC(TRUE, &hDC);
			assert(SUCCEEDED(hResult));

			// Draw flash frame only once
			if(i == 0)
				g_flashPlayer->DrawFrame(hDC);

			// Release the DC specifying the dirty rect
			RECT updateRect = dirtyRects[i];
			pSurface->ReleaseDC(&updateRect);
		}

		pSurface->Release();
	}

	// Copy the content from the texture to the back-buffer
	//pImmediateContext->CopyResource(pBackBuffer11, pTexture);

	pImmediateContext->PSSetShaderResources( 0, 1, &g_pTextureRV );

	pImmediateContext->DrawIndexed( 6, 0, 0 );

	// Present to screen
	pSwapChain->Present(0, 0);
}

bool RecreateTargets(unsigned int newWidth, unsigned int newHeight)
{
	window_width = newWidth; window_height = newHeight;
	
	SAFE_RELEASE(pTexture);
	SAFE_RELEASE(pBackBuffer11);
	SAFE_RELEASE(m_pRenderTargetView);
	HRESULT hr = pSwapChain->ResizeBuffers(1, window_width, window_height, 
								DXGI_FORMAT_B8G8R8A8_UNORM, 0);
	assert(SUCCEEDED(hr));
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer11));
	assert(SUCCEEDED(hr));
	
	hr = pDevice11 ->CreateRenderTargetView( pBackBuffer11, NULL, &m_pRenderTargetView );
	assert(SUCCEEDED(hr));

	SAFE_RELEASE(pBackBuffer11);

	// bind the render target
	pImmediateContext->OMSetRenderTargets( 1, &m_pRenderTargetView, NULL );

	// the viewable area
	D3D11_VIEWPORT vp;
	vp.Width		= (FLOAT)window_width;
	vp.Height		= (FLOAT)window_height;
	vp.MinDepth		= 0.0f;
	vp.MaxDepth		= 1.0f;
	vp.TopLeftX		= 0;
	vp.TopLeftY		= 0;
	pImmediateContext->RSSetViewports( 1, &vp );

	// tell the device context that the current vertex buffer is a trianglelist
	// default to this for now, abstract it later
	pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );


	// Create the texture for flash renderer
	D3D11_TEXTURE2D_DESC textureDesc;

	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width = window_width;
	textureDesc.Height = window_height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;

	hr = pDevice11->CreateTexture2D(&textureDesc, NULL, &pTexture);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MostDetailedMip = 0;
	viewDesc.Texture2D.MipLevels = 1; 
	hr = pDevice11->CreateShaderResourceView(pTexture, &viewDesc, &g_pTextureRV);
	assert(SUCCEEDED(hr));

	return true;
}


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// Set current directory to module directory
	wchar_t imagePath[MAX_PATH];
	GetModuleFileName(NULL, imagePath, MAX_PATH);
	size_t pathLen = wcslen(imagePath);
	wchar_t* pathEnd = imagePath + pathLen - 1;
	while (pathEnd >= imagePath && (*pathEnd != L'\\' && *pathEnd != L'/'))
		--pathEnd;

	if (pathEnd >= imagePath)
		*pathEnd++ = 0;

	SetCurrentDirectory(imagePath);
	
	InitWindow(hInstance, nCmdShow);

	InitDevice();
	
	InitFlashDX();

	ShowWindow(hWnd, nCmdShow);


	while(true) {
		MSG msg;
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0) {
			if(msg.message == WM_QUIT)
				break;
			else {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else 
		{
			Render();
		}
	}

	DeinitFlash();
	DeinitDevice();

// 	UnregisterClass(wc.lpszClassName, hInstance);

	return 0;
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch(msg) {
		// Window destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_PAINT:
			ValidateRect(hWnd, NULL);
			return 0;
		case WM_MOUSEMOVE:
			if (g_flashPlayer)
				g_flashPlayer->SetMousePos(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));
			return 0;
		case WM_LBUTTONDOWN:
			if (g_flashPlayer)
				g_flashPlayer->SetMouseButtonState(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), IFlashDXPlayer::eMouse1, true);
			return 0;
		case WM_LBUTTONUP:
			if (g_flashPlayer)
				g_flashPlayer->SetMouseButtonState(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), IFlashDXPlayer::eMouse1, false);
			return 0;
		case WM_KEYDOWN:
			if (g_flashPlayer)
				g_flashPlayer->SendKey(true, wParam, lParam);
			return 0;
		case WM_KEYUP:
			if (g_flashPlayer)
				g_flashPlayer->SendKey(false, wParam, lParam);
			return 0;
		case WM_CHAR:
			if (g_flashPlayer)
				g_flashPlayer->SendChar(wParam, lParam);
			return 0;
		case WM_SIZE:
			if (g_flashPlayer)
			{
				RecreateTargets(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			}
			return 0;
		case WM_SETCURSOR:
			if (g_flashPlayer)
			{
				static bool restoreCursor = true;
				if (LOWORD(lParam) != HTCLIENT)
					restoreCursor = true;

				if (restoreCursor)
				{
					restoreCursor = false;
					break; // DefWindowProc will set the cursor
				}
				return 1;
			}
			break;
		default:
			break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// copypasted from DX SDK samples
//--------------------------------------------------------------------------------------
// Use this until D3DX11 comes online and we get some compilation helpers
//--------------------------------------------------------------------------------------
HRESULT CompileShaderFromFile( WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut )
{
	HRESULT hr = S_OK;

// 	// find the file
// 	WCHAR str[MAX_PATH];
// 	V_RETURN( DXUTFindDXSDKMediaFileCch( str, MAX_PATH, szFileName ) );

	// open the file
	HANDLE hFile = CreateFile( szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN, NULL );
	if( INVALID_HANDLE_VALUE == hFile )
		return E_FAIL;

	// Get the file size
	LARGE_INTEGER FileSize;
	GetFileSizeEx( hFile, &FileSize );

	// create enough space for the file data
	BYTE* pFileData = new BYTE[ FileSize.LowPart ];
	if( !pFileData )
		return E_OUTOFMEMORY;

	// read the data in
	DWORD BytesRead;
	if( !ReadFile( hFile, pFileData, FileSize.LowPart, &BytesRead, NULL ) )
		return E_FAIL; 

	CloseHandle( hFile );

	// Compile the shader
	ID3DBlob* pErrorBlob;
	hr = D3DCompile( pFileData, FileSize.LowPart, "none", NULL, NULL, szEntryPoint, szShaderModel, D3D10_SHADER_ENABLE_STRICTNESS, 0, ppBlobOut, &pErrorBlob );

	delete []pFileData;

	if( FAILED(hr) )
	{
		OutputDebugStringA( (char*)pErrorBlob->GetBufferPointer() );
		SAFE_RELEASE( pErrorBlob );
		return hr;
	}
	SAFE_RELEASE( pErrorBlob );

	return S_OK;
}
