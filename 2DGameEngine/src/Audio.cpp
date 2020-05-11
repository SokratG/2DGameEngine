#include "Audio.h"



namespace GameEngine {

	Audio::Audio() : xactEngine(nullptr), waveBank(nullptr), soundBank(nullptr),
					 mapWaveBank(nullptr), soundBankData(nullptr)
 	{
		signalIndex = 0;
		ZeroMemory(&xactParams, sizeof(XACT_RUNTIME_PARAMETERS));
		

		HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
		if (SUCCEEDED(result))
			initialized = TRUE;
		else
			initialized = FALSE;
	}

	Audio::~Audio()
	{
		if (xactEngine) {
			xactEngine->ShutDown();
			xactEngine->Release();
		}
		if (soundBankData) 
			SAFE_DELETE_ARRAY(soundBankData);
		soundBankData = nullptr;

		// release memory mapped files
		if (mapWaveBank) 
			UnmapViewOfFile(mapWaveBank);
		mapWaveBank = nullptr;

		if (initialized)
			CoUninitialize();
	}

	HRESULT Audio::initialize(LPCWSTR WAVE_BANK_FILE, LPCWSTR SOUND_BANK_FILE)
	{
		HANDLE hFile;			// file with sound in XACT format
		HANDLE hMapFile;		// hande mapping file to memory
		DWORD fileSize = 0;
		DWORD bytesRead = 0;
		HRESULT result = E_FAIL;

		if (!initialized)
			return result;
		result = XACT3CreateEngine(0, &xactEngine); // create device xact engine
		if (FAILED(result) || !xactEngine)
			return E_FAIL;

		// init xact paramateres and create XACT runtime:
		xactParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
		result = xactEngine->Initialize(&xactParams);
		if (FAILED(result))
			return result;
		
		// create sound wave bank file in memory mapped file 
		result = E_FAIL;
		hFile = CreateFile(WAVE_BANK_FILE, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, nullptr);

		if (hFile != INVALID_HANDLE_VALUE) {
			fileSize = GetFileSize(hFile, 0);

			if (fileSize != -1) {
				hMapFile = CreateFileMapping(hFile, 0, PAGE_READONLY, 0, fileSize, nullptr);
				if (hMapFile) {
					mapWaveBank = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);
					if (mapWaveBank)
						result = xactEngine->CreateInMemoryWaveBank(mapWaveBank, fileSize, 0, 0, &waveBank);
					CloseHandle(hMapFile); // now mapWaveBank maintain a handle of file(1)
				}
			}
			CloseHandle(hFile); // (1)
		}
		if (FAILED(result))
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

		// Read and register the sound bank file with XACT system
		result = E_FAIL;
		hFile = CreateFile(SOUND_BANK_FILE, GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, nullptr);

		if (hFile != INVALID_HANDLE_VALUE) {
			fileSize = GetFileSize(hFile, 0);

			if (fileSize != -1) {
				// reserve memory for sound bank
				soundBankData = new BYTE[fileSize]; 
				if (soundBankData) {
					if (ReadFile(hFile, soundBankData, fileSize, &bytesRead, nullptr))
						result = xactEngine->CreateSoundBank(soundBankData, fileSize, 0, 0, &soundBank);
				}
			}
			CloseHandle(hFile);
		}
		if (FAILED(result))
			return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

		return S_OK;
	}

	VOID Audio::run()
	{
		if (!xactEngine)
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error not init XACT Engine"));
		xactEngine->DoWork();
	}


	VOID Audio::play(const CHAR namesound[])
	{
		if (!soundBank)
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error not init XACT sound bank"));
		signalIndex = soundBank->GetCueIndex(namesound);
		soundBank->Play(signalIndex, 0, 0, nullptr);
	}

	VOID Audio::stop(const CHAR namesound[])
	{
		if (!soundBank)
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error not init XACT sound bank"));
		signalIndex = soundBank->GetCueIndex(namesound);
		soundBank->Stop(signalIndex, XACT_FLAG_STOP_IMMEDIATE);
	}

	Audio* Audio::CreateAudioControl()
	{
		Audio* xactAudio = nullptr;
		try
		{
			xactAudio = new Audio;
		}
		catch (const std::bad_alloc& alloc)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, alloc.what()));
		}
		catch (...)
		{
			throw(CoreError::GameEngineError(CoreError::FATAL_ERROR, "Error creating audio control class"));
		}
		return xactAudio;
	}



}
