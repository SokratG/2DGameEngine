#pragma once
#include <windows.h>
#include <XAudio2.h>	// include for avoid errors
#include <xact3.h>
#include "constants.h"
#include "CoreError.h"

namespace GameEngine {


	class Audio
	{
	private:
		IXACT3Engine* xactEngine; // XACT sound engine
		IXACT3WaveBank* waveBank; // XACT wave bank
		IXACT3SoundBank* soundBank; // XACT sound bank
		XACT_RUNTIME_PARAMETERS xactParams;
		XACTINDEX signalIndex;

		PVOID mapWaveBank;		// for mapping view file 
		PVOID soundBankData;	// store data bytes sound bank 
		BOOL initialized;
		Audio();
	public:
		static Audio* CreateAudioControl(); // create XACT audio class
		virtual ~Audio();
		HRESULT initialize(LPCWSTR WAVE_BANK_FILE, LPCWSTR SOUND_BANK_FILE);
		XACT_RUNTIME_PARAMETERS* getXactParams() { return &xactParams; }
		VOID run();
		VOID play(const CHAR namesound[]);
		VOID stop(const CHAR namesound[]);
	};

}