
// MFCDShowDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "MFCDShow.h"
#include "MFCDShowDlg.h"
#include "afxdialogex.h"
//#include <opengl.h>
#include <Dvdmedia.h> // VIDEOINFOHEADER2

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib,"Strmiids.lib")

STDMETHODIMP MySinkPin::Receive(IN IMediaSample* pSample)
{
	int len = pSample->GetActualDataLength();
	//wchar_t c_debug[128] = { 0 };
	//swprintf_s(c_debug, _T("=========get len=%d==========\n"), len);
	char c_debug[128] = { 0 };
	sprintf_s(c_debug, "=========get len=%d==========\n", len);
	OutputDebugStringA(c_debug);

	return S_OK;
}

IPin* MySinkPin::get_pin()
{
	if (!m_input)
	{
	}

	return m_input;
}

CMySinkFIlter::CMySinkFIlter()
{
}

CMySinkFIlter::~CMySinkFIlter()
{
}

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFCDShowDlg 对话框



CMFCDShowDlg::CMFCDShowDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MFCDSHOW_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFCDShowDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFCDShowDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CMFCDShowDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFCDShowDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CMFCDShowDlg 消息处理程序

BOOL CMFCDShowDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFCDShowDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFCDShowDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFCDShowDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL PinMatchesCategory(IPin* pPin, REFGUID Category)
{
	BOOL bFound = FALSE;
	IKsPropertySet* pKs = NULL;
	HRESULT hr = pPin->QueryInterface(IID_PPV_ARGS(&pKs));
	if (SUCCEEDED(hr))
	{
		GUID PinCategory;
		DWORD cbReturned;
		hr = pKs->Get(AMPROPSETID_Pin, AMPROPERTY_PIN_CATEGORY, NULL, 0, &PinCategory,
			sizeof(GUID), &cbReturned);
		if (SUCCEEDED(hr) && (cbReturned == sizeof(GUID)))
		{
			bFound = (PinCategory == Category);
		}
		pKs->Release();
	}
	return bFound;
}

IPin* GetInputPin(IBaseFilter* filter)
{
	HRESULT hr;
	IPin* pin = NULL;
	IEnumPins* pPinEnum = NULL;
	filter->EnumPins(&pPinEnum);
	if (pPinEnum == NULL)
	{
		return NULL;
	}

	// get first unconnected pin
	hr = pPinEnum->Reset(); // set to first pin

	while (S_OK == pPinEnum->Next(1, &pin, NULL))
	{
		PIN_DIRECTION pPinDir;
		pin->QueryDirection(&pPinDir);
		if (PINDIR_INPUT == pPinDir) // This is an input pin
		{
			IPin* tempPin = NULL;
			if (S_OK != pin->ConnectedTo(&tempPin)) // The pint is not connected
			{
				pPinEnum->Release();
				return pin;
			}
		}
		pin->Release();
	}
	pPinEnum->Release();
	return NULL;
}

IPin* GetOutputPin(IBaseFilter* filter, REFGUID Category)
{
	HRESULT hr;
	IPin* pin = NULL;
	IEnumPins* pPinEnum = NULL;
	filter->EnumPins(&pPinEnum);
	if (pPinEnum == NULL)
	{
		return NULL;
	}
	// get first unconnected pin
	hr = pPinEnum->Reset(); // set to first pin
	while (S_OK == pPinEnum->Next(1, &pin, NULL))
	{
		PIN_DIRECTION pPinDir;
		pin->QueryDirection(&pPinDir);
		if (PINDIR_OUTPUT == pPinDir) // This is an output pin
		{
			if (Category == GUID_NULL || PinMatchesCategory(pin, Category))
			{
				pPinEnum->Release();
				return pin;
			}
		}
		pin->Release();
		pin = NULL;
	}
	pPinEnum->Release();
	return NULL;
}

int CMFCDShowDlg::init_camera(const char* devUniqueID)
{
	HRESULT hr = m_graph_builder.CoCreateInstance(CLSID_FilterGraph);//------mmm, okey
	//HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL,
	//	CLSCTX_INPROC_SERVER, IID_IGraphBuilder,
	//	(void**)&m_graph_builder.p);
	if (FAILED(hr)) {
		return -1;
	}

	hr = m_graph_builder->QueryInterface(&m_control.p);
	if (FAILED(hr)) {
		return -1;
	}

	char frendly_dev_name[256] = {0};
	m_out_filter = get_captrue_fileter(devUniqueID, strlen(devUniqueID) + 1, frendly_dev_name);
	if (NULL == m_out_filter)
	{
		OutputDebugStringA("===========device id not found.=============\n");
		return -1;
	}
	//	绑定输入filter
	hr = m_graph_builder->AddFilter(m_out_filter, _T("CaptureFilter"));
	//	获取输入pin对象
	m_output_pin = GetOutputPin(m_out_filter, PIN_CATEGORY_CAPTURE);

	//	绑定输出filter
	//	output pin --> input pin, so input pin can receive data success.
	//	so we need implement input pin
	//m_sink_filter = std::make_unique<CMySinkFIlter>();
	//hr = m_graph_builder->AddFilter(m_sink_filter.get(), _T("SinkFilter"));
	//BOOL ret = m_sink_filter->GetPin(&m_input_pin.p);
	//if (!ret){
	//	OutputDebugStringA("===========input pin failed.============\n");
	//}

	//set_camera_output();

	m_control->Pause();

	return 0;
}

BOOL CMFCDShowDlg::set_camera_output()
{

	//// Get the best matching capability
	//VideoCaptureCapability capability;
	//int32_t capabilityIndex;

	//// Store the new requested size
	//_requestedCapability = requestedCapability;
	//// Match the requested capability with the supported.
	//if ((capabilityIndex = _dsInfo.GetBestMatchedCapability(_deviceUniqueId,
	//	_requestedCapability,
	//	capability)) < 0)
	//{
	//	return -1;
	//}
	////Reduce the frame rate if possible.
	//if (capability.maxFPS > requestedCapability.maxFPS)
	//{
	//	capability.maxFPS = requestedCapability.maxFPS;
	//}
	//else if (capability.maxFPS <= 0)
	//{
	//	capability.maxFPS = 30;
	//}
	//// Store the new expected capture delay
	//_captureDelay = capability.expectedCaptureDelay;

	//// Convert it to the windows capability index since they are not nexessary
	//// the same
	//VideoCaptureCapabilityWindows windowsCapability;
	//if (_dsInfo.GetWindowsCapability(capabilityIndex, windowsCapability) != 0)
	//{
	//	return -1;
	//}

	CComPtr<IAMStreamConfig> streamConfig = NULL;
	AM_MEDIA_TYPE* pmt = NULL;
	VIDEO_STREAM_CONFIG_CAPS caps;

	HRESULT hr = m_input_pin->QueryInterface(IID_IAMStreamConfig,
		(void**)&streamConfig);
	if (hr)
	{
		OutputDebugStringA("Can't get the Capture format settings.");
		return -1;
	}

	//Get the windows capability from the capture device
	bool isDVCamera = false;
	hr = streamConfig->GetStreamCaps(
		/*windowsCapability.directShowCapabilityIndex*/0,
		&pmt, reinterpret_cast<BYTE*> (&caps));
	if (hr == S_OK)
	{
		if (pmt->formattype == FORMAT_VideoInfo2)
		{
			VIDEOINFOHEADER2* h =
				reinterpret_cast<VIDEOINFOHEADER2*> (pmt->pbFormat);
			//if (capability.maxFPS > 0
			//	&& windowsCapability.supportFrameRateControl)
			//{
			//	h->AvgTimePerFrame = REFERENCE_TIME(10000000.0
			//		/ capability.maxFPS);
			//}
			h->AvgTimePerFrame = REFERENCE_TIME(10000000.0 / 25);
		}
		else
		{
			VIDEOINFOHEADER* h = reinterpret_cast<VIDEOINFOHEADER*>
				(pmt->pbFormat);
			//if (capability.maxFPS > 0
			//	&& windowsCapability.supportFrameRateControl)
			//{
			//	h->AvgTimePerFrame = REFERENCE_TIME(10000000.0
			//		/ capability.maxFPS);
			//}
			h->AvgTimePerFrame = REFERENCE_TIME(10000000.0 / 25);
		}

		// Set the sink filter to request this capability
		//_sinkFilter->SetMatchingMediaType(capability);
		//Order the capture device to use this capability
		hr += streamConfig->SetFormat(pmt);

		//Check if this is a DV camera and we need to add MS DV Filter
		if (pmt->subtype == MEDIASUBTYPE_dvsl
			|| pmt->subtype == MEDIASUBTYPE_dvsd
			|| pmt->subtype == MEDIASUBTYPE_dvhd)
			isDVCamera = true; // This is a DV camera. Use MS DV filter
	}
	else {
		char c_debug[128] = { 0 };
		sprintf_s(c_debug, "Failed to get stream caps, hr:%04x", hr);
		OutputDebugStringA(c_debug);
	}

	if (FAILED(hr))
	{
		OutputDebugStringA("Failed to set capture device output format");
		return -1;
	}

	//if (isDVCamera)
	//{
	//	hr = ConnectDVCamera();
	//}
	//else
	{
		hr = m_graph_builder->ConnectDirect(m_input_pin, m_output_pin,
			NULL);
	}
	if (hr != S_OK)
	{
		char c_debug[128] = { 0 };
		sprintf_s(c_debug, "Failed to connect the Capture graph % d", hr);
		OutputDebugStringA(c_debug);
		return -1;
	}

	return 0;
}

int CMFCDShowDlg::start_camera_capture()
{
	return 0;
}

int CMFCDShowDlg::stop_camera_capture()
{
	return 0;
}

IBaseFilter* CMFCDShowDlg::get_captrue_fileter(const char* dev_unique_id, int dev_id_length, char* deviceName)
{
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		IID_ICreateDevEnum, (void**)&m_dsDevEnum);
	//m_dsDevEnum.CoCreateInstance(CLSID_SystemDeviceEnum);
	//m_dsDevEnum->QueryInterface(IID_ICreateDevEnum, (void**)& m_dsDevEnum.p);

	if (hr != NOERROR) {
		OutputDebugStringA("=======get_captrue_fileter failed, 1======\n");
		return NULL;
	}

	m_dsDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &m_dsMonikerDevEnum.p, 0);
	if (hr != NOERROR) {
		OutputDebugStringA("=======get_captrue_fileter failed, 2======\n");
		return NULL;
	}

	m_dsMonikerDevEnum->Reset();
	ULONG cFetched;
	CComPtr<IMoniker> pM;

	IBaseFilter* captureFilter = NULL;
	bool deviceFound = false;
	while (S_OK == m_dsMonikerDevEnum->Next(1, &pM.p, &cFetched) && !deviceFound) {
		CComPtr<IPropertyBag> pBag;
		hr = pM->BindToStorage(0, 0, IID_IPropertyBag, (void**)&pBag.p);
		if (S_OK == hr)
		{
			// Find the description or friendly name.
			VARIANT varName;
			VariantInit(&varName);
			if (dev_id_length > 0)
			{
				hr = pBag->Read(L"DevicePath", &varName, 0);
				if (FAILED(hr))
				{
					hr = pBag->Read(L"Description", &varName, 0);
					if (FAILED(hr))
					{
						hr = pBag->Read(L"FriendlyName", &varName, 0);
					}
				}

				if (hr != NOERROR) {
					char tempDevicePathUTF8[256];
					tempDevicePathUTF8[0] = 0;
					WideCharToMultiByte(CP_UTF8, 0, varName.bstrVal, -1,
						tempDevicePathUTF8,
						sizeof(tempDevicePathUTF8), NULL,
						NULL);
					if (strncmp(tempDevicePathUTF8,
						(const char*)dev_unique_id,
						dev_id_length) == 0)
					{
						// We have found the requested device
						deviceFound = true;
						hr = pM->BindToObject(0, 0, IID_IBaseFilter,
							(void**)&captureFilter);
						if (FAILED(hr))
						{
							OutputDebugStringA("========failed to bind device.=======\n");
							return NULL;
						}

						hr = pBag->Read(L"FriendlyName", &varName, 0);
						if (SUCCEEDED(hr) && deviceName) {
							WideCharToMultiByte(CP_UTF8, 0, varName.bstrVal, -1,
								deviceName,
								256, NULL,
								NULL);
						}
						//if (productUniqueIdUTF8
						//	&& productUniqueIdUTF8Length > 0) // Get the device name
						//{

						//	GetProductId(deviceUniqueIdUTF8,
						//		productUniqueIdUTF8,
						//		productUniqueIdUTF8Length);
						//}
					}
				}
			}

			VariantClear(&varName);
		}
	}

	return captureFilter;
}

void CMFCDShowDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnOK();
}




void CMFCDShowDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码

	init_camera("usb#vid_045e&pid_0990&mi_00#6&db32c28&0&0000#{65e8773d-8f56-11d0-a3b9-00a0c9223196}\global");
}
