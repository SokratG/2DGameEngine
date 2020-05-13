#include "console.h"


namespace GameEngine {
    GameConsole::GameConsole()
    {
        graphicDevice = nullptr;
        input = nullptr;
        vertexBuffer = nullptr;
        visible = FALSE;
        x = 0.f; y = 0.f;
        Rows = 0; scrollAmount = 0;
        BackgroundCOLOR = GraphColor::GRAY;
        FontCOLOR = GraphColor::WHITE;
        ZeroMemory(&TextRect, sizeof(RECT));
        ZeroMemory(&VertConsole, sizeof(VertexC) * 4);
        initialized = FALSE;
    }

    GameConsole::~GameConsole()
    {
        onLostDevice();
    }

    BOOL GameConsole::initialize(DirectGraphic* pGraphicDevice, Input* inputSystem, ConsoleParameters& cp)
    {
        return TRUE;
    }

    const VOID GameConsole::draw()
    {
        // TODO
    }


    // Toggle show or hide console
    VOID GameConsole::showHide()    
    {
        if (!initialized)
            return;
        visible = !visible;
        if (input)
            input->clear(inputKEYS::KEYS_PRESSED | inputKEYS::TEXT_IN);
    }


    VOID GameConsole::print(const string& str)
    {
        if (!initialized)
            return;
        text.push_front(str);
        if (text.size() > consParam.MAX_LINES)
            text.pop_back();
    }

    string GameConsole::getCommand()
    {
        // TODO
        return string();
    }


    VOID GameConsole::onLostDevice()
    {
        if (!initialized)
            return;
        DXFont.onLostDevice();
        SAFE_RELEASE(vertexBuffer);
    }

    VOID GameConsole::onResetDevice()
    {
        if (!initialized)
            return;
        graphicDevice->createVertexBuffer(VertConsole, sizeof(VertConsole), vertexBuffer);
        DXFont.onResetDevice();
    }

}