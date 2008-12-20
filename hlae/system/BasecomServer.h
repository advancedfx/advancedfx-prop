#ifndef HLAE_BASECOMSERVER_H
#define HLAE_BASECOMSERVER_H

class CBCServerInternal;

class CHlaeBcServer
{
public:
	CHlaeBcServer( System::Windows::Forms::ToolStripContentPanel ^gameWindowParent );
	~CHlaeBcServer();

	bool Destroy();

	friend class CBCServerInternal;

private:
	System::Runtime::InteropServices::GCHandle _gameWindowParent;
	void *_hwndGameWindowParent;
	CBCServerInternal * _pBCServerInternal;

	bool _OnGameWindowClose(); // tells the server that it's server game window will be closed to allow it to react and communicate with the client
	
	// also called by CBCServerInternal:
	void *_OnCreateWindow(int nWidth, int nHeight);
	bool _OnDestroyWindow();
	bool _OnFilmingStart();
	bool _OnFilmingStop();
	bool _UpdateWindow(int nWidth, int nHeight);
};


#endif