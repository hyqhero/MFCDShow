
// MFCDShowDlg.h: 头文件
//

#pragma once
#include <atlcomcli.h>
#include <Dshow.h>
#include <memory>

class MySinkPin : public IPin {
public:
	STDMETHODIMP Receive(IN IMediaSample*);

	IPin* get_pin();

private:
	IPin* m_input;
};

class CMySinkFIlter : public IBaseFilter
{
public:
	CMySinkFIlter();
	~CMySinkFIlter();

	BOOL GetPin(IPin** out) {
		if (out && *out) {
			*out = m_input;
			return TRUE;
		}
		return FALSE;
	}
private:
	MySinkPin* m_input;
};

// CMFCDShowDlg 对话框
class CMFCDShowDlg : public CDialogEx
{
// 构造
public:
	CMFCDShowDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MFCDSHOW_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	int init_camera(const char* dev_unique_id);
	BOOL set_camera_output();
	int start_camera_capture();
	int stop_camera_capture();
	//	需要传入ANSI类型的设备ID
	IBaseFilter* get_captrue_fileter(const char* dev_unique_id, int dev_id_length, char* deviceName);

	CComPtr<IGraphBuilder> m_graph_builder;
	CComPtr<IBaseFilter> m_out_filter;
	//	In filter, btw, is really generate capture data to our memory.

	std::unique_ptr<CMySinkFIlter>	m_sink_filter;
	CComPtr<IPin>	m_input_pin;
	CComPtr<IPin>	m_output_pin;

	CComPtr<IMediaControl>	m_control;

	CComPtr<ICreateDevEnum> m_dsDevEnum;
	CComPtr<IEnumMoniker> m_dsMonikerDevEnum;

public:
	afx_msg void OnBnClickedOk();


	afx_msg void OnBnClickedButton2();
};
