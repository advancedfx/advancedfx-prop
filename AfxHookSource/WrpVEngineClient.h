#pragma once

// Copyright (c) advancedfx.org
//
// Last changes:
// 2015-08-03 dominik.matrixstorm.com
//
// First changes:
// 2009-10-01 dominik.matrixstorm.com

// Description:
// Wrapper(s) for Source's IVEngineClient Interface

#include "SourceInterfaces.h"


// WrpVEngineClient ///////////////////////////////////////////////////////////

class WrpVEngineClient abstract {
public:
	virtual void GetScreenSize( int& width, int& height ) abstract = 0;
	virtual void ServerCmd( const char *szCmdString, bool bReliable = true ) abstract = 0;
	virtual void ClientCmd( const char *szCmdString ) abstract = 0;
	virtual bool Con_IsVisible( void ) abstract = 0;
	virtual float GetLastTimeStamp( void ) abstract = 0; 
	virtual void GetViewAngles( QAngle& va ) abstract = 0;
	virtual void SetViewAngles( QAngle& va ) abstract = 0;
	virtual int GetMaxClients( void ) abstract = 0;
	virtual bool IsInGame( void ) abstract = 0;
	virtual bool IsConnected( void ) abstract = 0;
	virtual bool IsDrawingLoadingImage( void ) abstract = 0;
	virtual void Con_NPrintf( int pos, const char *fmt, ... ) abstract = 0;
	virtual const char *  GetGameDirectory( void ) abstract = 0;
	virtual const VMatrix& WorldToScreenMatrix() abstract = 0;
	virtual const VMatrix& WorldToViewMatrix() abstract = 0;
	virtual char const *  GetLevelName( void ) abstract = 0;
	virtual void EngineStats_BeginFrame( void ) abstract = 0;
	virtual void EngineStats_EndFrame( void ) abstract = 0;
	virtual bool IsPlayingDemo( void ) abstract = 0;
	virtual bool IsRecordingDemo( void ) abstract = 0;
	virtual bool IsPlayingTimeDemo( void ) abstract = 0;
	virtual bool IsPaused( void ) abstract = 0;
	virtual bool IsTakingScreenshot( void ) abstract = 0;
	virtual bool IsHLTV( void ) abstract = 0;
	virtual bool IsLevelMainMenuBackground( void ) abstract = 0;
	virtual void GetMainMenuBackgroundName( char *dest, int destlen ) abstract = 0;
	virtual bool IsInEditMode( void ) abstract = 0;
	virtual float GetScreenAspectRatio() abstract = 0;
	virtual unsigned int GetEngineBuildNumber() abstract = 0;
	virtual const char * GetProductVersionString() abstract = 0;
	virtual bool IsHammerRunning( ) const abstract = 0;
	virtual void ExecuteClientCmd( const char *szCmdString ) abstract = 0;
	virtual int	GetAppID() abstract = 0;
	virtual void ClientCmd_Unrestricted( const char *szCmdString ) abstract = 0;
};


// WrpVEngineClient_012 ////////////////////////////////////////////////////////

class WrpVEngineClient_012 : public WrpVEngineClient
{
public:
	WrpVEngineClient_012(IVEngineClient_012 * iface);

	virtual void GetScreenSize( int& width, int& height );
	virtual void ServerCmd( const char *szCmdString, bool bReliable = true );
	virtual void ClientCmd( const char *szCmdString );
	virtual bool Con_IsVisible( void );
	virtual float GetLastTimeStamp( void ); 
	virtual void GetViewAngles( QAngle& va );
	virtual void SetViewAngles( QAngle& va );
	virtual int GetMaxClients( void );
	virtual bool IsInGame( void );
	virtual bool IsConnected( void );
	virtual bool IsDrawingLoadingImage( void );
	virtual void Con_NPrintf( int pos, const char *fmt, ... );
	virtual const char *  GetGameDirectory( void );
	virtual const VMatrix& WorldToScreenMatrix();
	virtual const VMatrix& WorldToViewMatrix();
	virtual char const *  GetLevelName( void );
	virtual void EngineStats_BeginFrame( void );
	virtual void EngineStats_EndFrame( void );
	virtual bool IsPlayingDemo( void );
	virtual bool IsRecordingDemo( void );
	virtual bool IsPlayingTimeDemo( void );
	virtual bool IsPaused( void );
	virtual bool IsTakingScreenshot( void );
	virtual bool IsHLTV( void );
	virtual bool IsLevelMainMenuBackground( void );
	virtual void GetMainMenuBackgroundName( char *dest, int destlen );
	virtual bool IsInEditMode( void );
	virtual float GetScreenAspectRatio();
	virtual unsigned int GetEngineBuildNumber();
	virtual const char * GetProductVersionString();
	virtual bool IsHammerRunning( ) const;
	virtual void ExecuteClientCmd( const char *szCmdString );
	virtual int	GetAppID();
	virtual void ClientCmd_Unrestricted( const char *szCmdString );

private:
	IVEngineClient_012 * m_VEngineClient_012;
};

// WrpVEngineClient_013 ////////////////////////////////////////////////////////

class WrpVEngineClient_013 : public WrpVEngineClient
{
public:
	WrpVEngineClient_013(IVEngineClient_013 * iface);

	virtual void GetScreenSize( int& width, int& height );
	virtual void ServerCmd( const char *szCmdString, bool bReliable = true );
	virtual void ClientCmd( const char *szCmdString );
	virtual bool Con_IsVisible( void );
	virtual float GetLastTimeStamp( void ); 
	virtual void GetViewAngles( QAngle& va );
	virtual void SetViewAngles( QAngle& va );
	virtual int GetMaxClients( void );
	virtual bool IsInGame( void );
	virtual bool IsConnected( void );
	virtual bool IsDrawingLoadingImage( void );
	virtual void Con_NPrintf( int pos, const char *fmt, ... );
	virtual const char *  GetGameDirectory( void );
	virtual const VMatrix& WorldToScreenMatrix();
	virtual const VMatrix& WorldToViewMatrix();
	virtual char const *  GetLevelName( void );
	virtual void EngineStats_BeginFrame( void );
	virtual void EngineStats_EndFrame( void );
	virtual bool IsPlayingDemo( void );
	virtual bool IsRecordingDemo( void );
	virtual bool IsPlayingTimeDemo( void );
	virtual bool IsPaused( void );
	virtual bool IsTakingScreenshot( void );
	virtual bool IsHLTV( void );
	virtual bool IsLevelMainMenuBackground( void );
	virtual void GetMainMenuBackgroundName( char *dest, int destlen );
	virtual bool IsInEditMode( void );
	virtual float GetScreenAspectRatio();
	virtual unsigned int GetEngineBuildNumber();
	virtual const char * GetProductVersionString();
	virtual bool IsHammerRunning( ) const;
	virtual void ExecuteClientCmd( const char *szCmdString );
	virtual int	GetAppID();
	virtual void ClientCmd_Unrestricted( const char *szCmdString );

private:
	IVEngineClient_013 * m_VEngineClient_013;
};

// WrpVEngineClient_013 ////////////////////////////////////////////////////////

class WrpVEngineClient_013_csgo : public WrpVEngineClient
{
public:
	WrpVEngineClient_013_csgo(IVEngineClient_013_csgo * iface);

	virtual void GetScreenSize( int& width, int& height );
	virtual void ServerCmd( const char *szCmdString, bool bReliable = true );
	virtual void ClientCmd( const char *szCmdString );
	virtual bool Con_IsVisible( void );
	virtual float GetLastTimeStamp( void ); 
	virtual void GetViewAngles( QAngle& va );
	virtual void SetViewAngles( QAngle& va );
	virtual int GetMaxClients( void );
	virtual bool IsInGame( void );
	virtual bool IsConnected( void );
	virtual bool IsDrawingLoadingImage( void );
	virtual void Con_NPrintf( int pos, const char *fmt, ... );
	virtual const char *  GetGameDirectory( void );
	virtual const VMatrix& WorldToScreenMatrix();
	virtual const VMatrix& WorldToViewMatrix();
	virtual char const *  GetLevelName( void );
	virtual void EngineStats_BeginFrame( void );
	virtual void EngineStats_EndFrame( void );
	virtual bool IsPlayingDemo( void );
	virtual bool IsRecordingDemo( void );
	virtual bool IsPlayingTimeDemo( void );
	virtual bool IsPaused( void );
	virtual bool IsTakingScreenshot( void );
	virtual bool IsHLTV( void );
	virtual bool IsLevelMainMenuBackground( void );
	virtual void GetMainMenuBackgroundName( char *dest, int destlen );
	virtual bool IsInEditMode( void );
	virtual float GetScreenAspectRatio();
	virtual unsigned int GetEngineBuildNumber();
	virtual const char * GetProductVersionString();
	virtual bool IsHammerRunning( ) const;
	virtual void ExecuteClientCmd( const char *szCmdString );
	virtual int	GetAppID();
	virtual void ClientCmd_Unrestricted( const char *szCmdString );

private:
	IVEngineClient_013_csgo * m_VEngineClient_013;
};
