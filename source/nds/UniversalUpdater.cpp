/*
*   This file is part of Universal-Updater
*   Copyright (C) 2019-2021 Universal-Team
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include <dirent.h>
#include <fat.h>
#include "nitrofs.h"
#include "UniversalUpdater.hpp"
#include "gui.hpp"
#include "tonccpy.h"

#include "rpc.h"
#include <lwip/sockets.h>
#include <dsiwifi9.h>

#include "Downloading.hpp"

int lSocket;
struct sockaddr_in sLocalAddr;

void appwifi_log(const char* s)
{
    iprintf("%s", s);
}

void appwifi_connect(void)
{
    rpc_init();
}

void appwifi_reconnect(void)
{
    rpc_deinit();
    rpc_init();
}

void appwifi_echoserver(void)
{
    lSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (lSocket < 0) goto fail;

    memset((char *)&sLocalAddr, 0, sizeof(sLocalAddr));
    sLocalAddr.sin_family = AF_INET;
    sLocalAddr.sin_len = sizeof(sLocalAddr);
    sLocalAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    sLocalAddr.sin_port = htons(1234);
    
    if (bind(lSocket, (struct sockaddr *)&sLocalAddr, sizeof(sLocalAddr)) < 0) {
        close(lSocket);
        goto fail;
    }

    if ( listen(lSocket, 20) != 0 ){
        close(lSocket);
        goto fail;
    }
    
    return;

fail:
    iprintf("not sure what happened\n");
    while(1) {
        swiWaitForVBlank();
    }
}

void appwifi_echoserver_tick(void)
{
    int clientfd;
    struct sockaddr_in client_addr;
    int addrlen = sizeof(client_addr);
    char buffer[256];
    int nbytes;

    fcntl(lSocket, F_SETFL, O_NONBLOCK);
    clientfd = accept(lSocket, (struct sockaddr*)&client_addr, (socklen_t*)&addrlen);
    if (clientfd > 0)
    {
        iprintf("echo_server: Accept %x\n", clientfd);
        do
        {
            nbytes = recv(clientfd, buffer, sizeof(buffer),0);
            iprintf("echo_server: Got %x bytes\n", nbytes);
            if (nbytes>0) send(clientfd, buffer, nbytes, 0);
        }
        while (nbytes>0);

        close(clientfd);
    }
}

/*
	Initialize everything as needed.
*/
void UU::Initialize(char *ARGV[]) {
	keysSetRepeat(20, 8);

	if (!fatInitDefault()) {
		consoleDemoInit();
		iprintf("FAT init failed!\n");
		while (1) swiWaitForVBlank();
	}

	/* Try init NitroFS at a few likely locations. */
	if (!nitroFSInit(ARGV[0])) {
		if (!nitroFSInit("Universal-Updater.nds")) {
			if (!nitroFSInit("/_nds/Universal-Updater/Universal-Updater.nds")) {
				consoleDemoInit();
				iprintf("NitroFS init failed!\n\n");
				iprintf("Copy Universal-Updater.nds to\n\n");
				iprintf("/_nds/Universal-Updater/\n");
				iprintf("           Universal-Updater.nds\n");
				iprintf("or launch using TWiLight Menu++\nor nds-hb-menu.");
				while (1) swiWaitForVBlank();
			}
		}
	}
	
	/* Create Directories. */
	mkdir("/_nds", 0x777);
	mkdir("/_nds/Universal-Updater", 0x777);
	mkdir("/_nds/Universal-Updater/stores", 0x777);
	mkdir("/_nds/Universal-Updater/shortcuts", 0x777);

	/* Initialize graphics. */
	Gui::init({"sd:/_nds/Universal-Updater/font.nftr", "fat:/_nds/Universal-Updater/font.nftr", "nitro:/graphics/font/default.nftr"});

	constexpr uint16_t Palette[] = {
		0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xB126, 0xB126, 0xB126, 0xB126, 0x9883, 0x9883, 0x9883, 0x9883, 0xA4A5, 0xA4A5, 0xA4A5, 0xA4A5,
		0xFFFF, 0xB9CE, 0xD6B5, 0xFFFF, 0xCE0D, 0xCE0D, 0xCE0D, 0xCE0D, 0xC189, 0xC189, 0xC189, 0xC189, 0xF735, 0xC1CC, 0xD22E, 0xF735
	};
	tonccpy(BG_PALETTE, Palette, sizeof(Palette));
	tonccpy(BG_PALETTE_SUB, Palette, sizeof(Palette));

	/* Load classes. */
	this->GData = std::make_unique<GFXData>();
	this->CData = std::make_unique<ConfigData>();
	this->TData = std::make_unique<ThemeData>();
	this->MData = std::make_unique<Meta>();
	this->Store = std::make_unique<UniStore>("/_nds/Universal-Updater/stores/universal-db.unistore", "universal-db.unistore");

	this->_Tabs = std::make_unique<Tabs>();
	this->TGrid = std::make_unique<TopGrid>();
	this->TList = std::make_unique<TopList>();

	consoleDemoInit();

	DSiWifi_SetLogHandler(appwifi_log);
    DSiWifi_SetConnectHandler(appwifi_connect);
    DSiWifi_SetReconnectHandler(appwifi_reconnect);
    DSiWifi_InitDefault(true);

	appwifi_echoserver();
};


/*
	Scan the key input.
*/
void UU::ScanInput() {
	scanKeys();
	this->Down = keysDown();
	this->Repeat = keysDownRepeat();
	touchRead(&this->T);
	this->T.px = this->T.px * 5 / 4;
	this->T.py = this->T.py * 5 / 4;
};


/*
	Draws Universal-Updater's UI.
*/
void UU::Draw() {
	this->GData->DrawTop();

	/* Ensure it isn't a nullptr. */
	if (this->Store && this->Store->UniStoreValid()) {
		Gui::DrawStringCentered(0, 3, TEXT_LARGE, TEXT_COLOR, this->Store->GetUniStoreTitle(), 390);

		switch(this->TMode) {
			case TopMode::Grid:
				this->TGrid->Draw();
				break;

			case TopMode::List:
				this->TList->Draw();
				break;
		}

		this->_Tabs->DrawTop();

	} else {
		Gui::DrawStringCentered(0, 3, TEXT_LARGE, TEXT_COLOR, "Invalid UniStore", 390);
	}

	this->GData->UpdateFont(true);
	this->GData->DrawBottom();
	this->_Tabs->DrawBottom();
	this->GData->UpdateFont(false);
};


/*
	Main Handler of the app. Handle Input and display stuff here.
*/
int UU::Handler(char *ARGV[]) {
	this->Initialize(ARGV);

	// TODO: It only redraws on button press, forcing an initial draw for now
	this->Draw();

	consoleDemoInit();

	while (!this->Exiting) {
		swiWaitForVBlank();

		// this->ScanInput();

		// /* Handle Top List if possible. */
		// if (this->_Tabs->HandleTopScroll()) {
		// 	switch(this->TMode) {
		// 		case TopMode::Grid:
		// 			this->TGrid->Handler();
		// 			break;

		// 		case TopMode::List:
		// 			this->TList->Handler();
		// 			break;
		// 	}
		// }

		// this->_Tabs->Handler(); // Tabs are always handled.
		
		// if (this->Repeat) this->Draw();

		appwifi_echoserver_tick();

        u32 addr = DSiWifi_GetIP();
        u8 addr_bytes[4];
        
        memcpy(addr_bytes, &addr, 4);
        
        iprintf("\x1b[s\x1b[0;0HCur IP: \x1b[36m%u.%u.%u.%u        \x1b[u\x1b[37m", addr_bytes[0], addr_bytes[1], addr_bytes[2], addr_bytes[3]);

		if(addr_bytes[0] == 10) {
			printf("%d\n", downloadToFile("https://home.pk11.us/files/hi.txt", "sd:/hi.txt"));
			while(1)swiWaitForVBlank();
		}


        swiWaitForVBlank();
        scanKeys();
        if (keysDown()&KEY_START) break;
        if (keysDown()&KEY_B) {
            iprintf("asdf\n");
        }
	}

	this->CData->Sav();
	return 0;
};


void UU::SwitchTopMode(const UU::TopMode TMode) {
	this->TMode = TMode;

	switch(this->TMode) {
		case TopMode::Grid:
			this->TGrid->Update();
			break;

		case TopMode::List:
			this->TList->Update();
			break;
	}

	this->GData->UpdateUniStoreSprites();
};