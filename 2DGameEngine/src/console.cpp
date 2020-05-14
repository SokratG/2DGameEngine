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
        consParam = cp;
        x = cp.X;
        y = cp.Y;
        TextRect.bottom = cp.Y + cp.HEIGHT - cp.MARGIN;
        TextRect.left = cp.X + cp.MARGIN;
        TextRect.right = cp.X + cp.WIDTH - cp.MARGIN;
        TextRect.top = cp.Y + cp.MARGIN;
        try
        {
            graphicDevice = pGraphicDevice;
            input = inputSystem;
            // make console vertex rectangle
            // top left
            VertConsole[0].x = x;
            VertConsole[0].y = y;
            VertConsole[0].z = 0.f;
            VertConsole[0].rhw = 1.f;
            VertConsole[0].color = BackgroundCOLOR;       
            // top right
            VertConsole[1].x = x + cp.WIDTH;
            VertConsole[1].y = y;
            VertConsole[1].z = 0.f;
            VertConsole[1].rhw = 1.f;
            VertConsole[1].color = BackgroundCOLOR;
            // bottom right
            VertConsole[2].x = x + cp.WIDTH;
            VertConsole[2].y = y + cp.HEIGHT;
            VertConsole[2].z = 0.f;
            VertConsole[2].rhw = 1.f;
            VertConsole[2].color = BackgroundCOLOR;
            // bottom left
            VertConsole[3].x = x;
            VertConsole[3].y = y + cp.HEIGHT;
            VertConsole[3].z = 0.f;
            VertConsole[3].rhw = 1.f;
            VertConsole[3].color = BackgroundCOLOR;

            graphicDevice->createVertexBuffer(VertConsole, sizeof(VertConsole), vertexBuffer);
            if (!DXFont.initialize(pGraphicDevice, cp.FONT_HEIGHT, FALSE, FALSE, cp.FONT))
                return FALSE;
            DXFont.setFontColor(FontCOLOR);
        }
        catch (...)
        {
            return FALSE;
        }
        initialized = TRUE;
        return TRUE;
    }

    const VOID GameConsole::draw()
    {
        if (!initialized || !visible || graphicDevice == nullptr)
            return;

        graphicDevice->drawQuad(vertexBuffer); // draw backdrop
        if (text.empty())
            return;
        
        graphicDevice->spriteBeginScene();  // Begin drawing sprites scene
        // calc text rows position 
        TextRect.left = 0;
        TextRect.top = 0;

        DXFont.print("|", TextRect, DT_CALCRECT);   // '|' - have a full height char, DT_CALCRECT - determines width and heigh text rect
        INT32 rowHeight = TextRect.bottom + 2;      // 2 - is a row spacing
        // check for correct data rect
        if (rowHeight <= 0)     
            rowHeight = 20;
        Rows = (consParam.HEIGHT - 2 * consParam.MARGIN) / rowHeight;
        Rows -= 2; // room for input prompt
        // check for correct rows result
        if (Rows <= 0)
            Rows = 5;       

        TextRect.left = static_cast<LONG>(x + consParam.MARGIN);
        TextRect.right = static_cast<LONG>((TextRect.right + consParam.WIDTH - consParam.MARGIN));
        // calc room for input prompt with spacing
        TextRect.bottom = static_cast<LONG>(y + consParam.HEIGHT - 2 * consParam.MARGIN - 2 * rowHeight);
        for (size_t i = scrollAmount; i < (Rows + scrollAmount) && i < (text.size()); ++i) {
            // set text display rect top for this row
            TextRect.top = TextRect.bottom - rowHeight;
            // display one row of text
            DXFont.print(text[i], TextRect, DT_LEFT);
            TextRect.bottom -= rowHeight;
        }

        // display command prompt
        TextRect.bottom = static_cast<LONG>(y + consParam.HEIGHT - consParam.MARGIN);
        TextRect.top = TextRect.bottom - rowHeight;
        string prompt = ">";
        prompt += input->getTextIn();
        DXFont.print(prompt, TextRect, DT_LEFT);

        graphicDevice->spriteEndScene();

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
        if (!initialized || !visible)
            return string();
        
        // check pressed key 
        if (input->isKeyPressed(CONSOLE_KEY))
            hide();
        if (input->isKeyPressed(ESC_KEY))
            return string();

        // check page operation
        if (input->isKeyPressed(VK_UP))
            scrollAmount++;
        else if (input->isKeyPressed(VK_DOWN))
            scrollAmount--;
        else if (input->isKeyPressed(VK_PRIOR))
            scrollAmount += Rows;
        else if (input->isKeyPressed(VK_NEXT))
            scrollAmount -= Rows;

        // check scroll value
        if (scrollAmount < 0)
            scrollAmount = 0;
        if (scrollAmount > (consParam.MAX_LINES - 1))
            scrollAmount = consParam.MAX_LINES - 1;

        size_t text_size = text.size() - 1;
        if (scrollAmount > text_size)
            scrollAmount = text_size;

        // user input
        cmdSTR = input->getTextIn();
        input->clear(inputKEYS::KEYS_DOWN | inputKEYS::KEYS_PRESSED | inputKEYS::MOUSE);

        if (cmdSTR.empty())
            return string();
        if (cmdSTR.at(cmdSTR.length() - 1) != '\r') // check enter pressed last char
            return string();

        cmdSTR.erase(cmdSTR.length() - 1); // remove '\r' in last pos
        input->clearTextIn();
        
        return cmdSTR;
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