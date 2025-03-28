#pragma once

#include "../Texture.h"

typedef ID3D11ShaderResourceView* Icon;

class AppIcons
{
public:
	static void init(ID3D11Device* pd3dDevice);

	static ID3D11ShaderResourceView* play;
	static ID3D11ShaderResourceView* stop;

	static ID3D11ShaderResourceView* yes;
	static ID3D11ShaderResourceView* no;

	static ID3D11ShaderResourceView* send;
	static ID3D11ShaderResourceView* back;

	static ID3D11ShaderResourceView* lock;
	static ID3D11ShaderResourceView* unlock;

	static ID3D11ShaderResourceView* padOn;
	static ID3D11ShaderResourceView* padOff;
	static ID3D11ShaderResourceView* plug;

	static ID3D11ShaderResourceView* micOn;
	static ID3D11ShaderResourceView* micOff;
	static ID3D11ShaderResourceView* speakersOn;
	static ID3D11ShaderResourceView* speakersOff;

	static ID3D11ShaderResourceView* video;

	static ID3D11ShaderResourceView* refresh;
	static ID3D11ShaderResourceView* sort;
	static ID3D11ShaderResourceView* submit;
	static ID3D11ShaderResourceView* saveOn;
	static ID3D11ShaderResourceView* saveOff;
	static ID3D11ShaderResourceView* edit;
	static ID3D11ShaderResourceView* image;
	static ID3D11ShaderResourceView* trash;
	static ID3D11ShaderResourceView* move;

	static ID3D11ShaderResourceView* chat;
	static ID3D11ShaderResourceView* users;
	static ID3D11ShaderResourceView* log;
	static ID3D11ShaderResourceView* logoff;

	static ID3D11ShaderResourceView* userOn;
	static ID3D11ShaderResourceView* userOff;
	static ID3D11ShaderResourceView* block;
	static ID3D11ShaderResourceView* kick;

	static ID3D11ShaderResourceView* info;
	static ID3D11ShaderResourceView* logo;
	static ID3D11ShaderResourceView* keymap;//-- CodeSomnia Add--

	static ID3D11ShaderResourceView* puppet;
	static ID3D11ShaderResourceView* xinput;
	static ID3D11ShaderResourceView* dinput;
	static ID3D11ShaderResourceView* ds4;
	static ID3D11ShaderResourceView* windows;
	static ID3D11ShaderResourceView* sdl;

	static ID3D11ShaderResourceView* xbox;
	static ID3D11ShaderResourceView* xbox1;
	static ID3D11ShaderResourceView* xbox2;
	static ID3D11ShaderResourceView* xbox3;
	static ID3D11ShaderResourceView* xbox4;

	static ID3D11ShaderResourceView* vpad;

	static ID3D11ShaderResourceView* btc;
	static ID3D11ShaderResourceView* ltc;
	static ID3D11ShaderResourceView* eth;
	
	static ID3D11ShaderResourceView* settings;
	static ID3D11ShaderResourceView* websocket;
	static ID3D11ShaderResourceView* editReason;
	static ID3D11ShaderResourceView* buttonLock;

	static ID3D11ShaderResourceView* library;
	static ID3D11ShaderResourceView* overlay;

	static ID3D11ShaderResourceView* hotseat;
	static ID3D11ShaderResourceView* tournament;

	static ID3D11ShaderResourceView* crown;
	static ID3D11ShaderResourceView* star;
	static ID3D11ShaderResourceView* eye;

	static ID3D11ShaderResourceView* developer;

	static ID3D11ShaderResourceView* sodaArcadeLogo;

	static ID3D11ShaderResourceView* arcadeIcon;
	static ID3D11ShaderResourceView* parsecIcon;

	static ID3D11ShaderResourceView* hammerIcon;

private:
	static Texture _play;
	static Texture _stop;

	static Texture _yes;
	static Texture _no;
	
	static Texture _send;
	static Texture _back;

	static Texture _padOn;
	static Texture _padOff;
	static Texture _plug;

	static Texture _lock;
	static Texture _unlock;
	
	static Texture _micOn;
	static Texture _micOff;
	static Texture _speakersOn;
	static Texture _speakersOff;

	static Texture _video;
	
	static Texture _refresh;
	static Texture _sort;
	static Texture _submit;
	static Texture _saveOn;
	static Texture _saveOff;
	static Texture _edit;
	static Texture _image;
	static Texture _trash;
	static Texture _move;

	static Texture _chat;
	static Texture _users;
	static Texture _log;
	static Texture _logoff;

	static Texture _userOn;
	static Texture _userOff;
	static Texture _block;
	static Texture _kick;

	static Texture _info;
	static Texture _keymap; //-- CodeSomnia Add--
	static Texture _logo;

	static Texture _puppet;
	static Texture _xinput;
	static Texture _dinput;
	static Texture _ds4;
	static Texture _windows;
	static Texture _sdl;

	static Texture _xbox;
	static Texture _xbox1;
	static Texture _xbox2;
	static Texture _xbox3;
	static Texture _xbox4;

	static Texture _vpad;

	static Texture _btc;
	static Texture _ltc;
	static Texture _eth;
	
	static Texture _settings;
	static Texture _websocket;
	static Texture _editReason;
	static Texture _buttonLock;

	static Texture _library;
	static Texture _overlay;

	static Texture _hotseat;
	static Texture _tournament;

	static Texture _crown;
	static Texture _star;
	static Texture _eye;

	static Texture _developer;

	static Texture _sodaArcadeLogo;

	static Texture _arcadeIcon;
	static Texture _parsecIcon;

	static Texture _hammerIcon;

};

