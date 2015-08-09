#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include "menuManager.h"
#include "manager.h"
#include "world.h"

MenuManager::MenuManager() :
  env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
  screen( IOManager::getInstance().getScreen() ),
  clock( Clock::getInstance() ),
  backColor(),
  menu(),
  numberOfSprites(-1),
  Records()
 // numberOfTry(0)
{ 
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    throw string("Unable to initialize SDL: ");
  }
  backColor.r = Gamedata::getInstance().getXmlInt("back/red");
  backColor.g = Gamedata::getInstance().getXmlInt("back/green");
  backColor.b = Gamedata::getInstance().getXmlInt("back/blue");
  atexit(SDL_Quit); 
}

void MenuManager::drawBackground() const {
  SDL_FillRect( screen, NULL, 
    SDL_MapRGB(screen->format, backColor.r, backColor.g, backColor.b) );
  SDL_Rect dest = {0, 0, 0, 0};
  SDL_BlitSurface( screen, NULL, screen, &dest );
}


// ----- show game result (win or lose) in the end -----
void MenuManager::showResultPage(int result){
  IOManager& io = IOManager::getInstance().getInstance();
  const string msg("Pause the game: ");
  io.clearString();
  drawBackground();
  io.printStringAfterMessage(msg, 20, 120);
  if(result > 1){
    World win("win",1.0);
    win.draw();
  }
  if(result == 0){
    World gover("gameover",1.0);
    gover.draw();
  }

  SDL_Flip(screen);
  SDL_Delay(1000);
  SDL_Flip(screen);
}
//----- get the help list contents -----
void MenuManager::getHelpPage() {
  IOManager& io = IOManager::getInstance().getInstance();
  const string msg("Help List: ");
  string resultMsg ;
  io.clearString();
  drawBackground();
  int linePos = 120;
  io.printStringAfterMessage(msg, 20, linePos);linePos +=30;
  io.printStringAfterMessage("press h to show help menu", 20, linePos);linePos +=30;
  io.printMessageAt("press m to show min map", 20, linePos);linePos +=30;
  io.printMessageAt("Press T to switch sprites", 20, linePos);linePos +=30; 
  io.printMessageAt("press e to make game easier", 20, linePos);linePos +=30;
  io.printMessageAt("press a/d to turn left/right", 20, linePos);linePos +=30;
  io.printMessageAt("press w to jump", 20, linePos);linePos +=30;
  io.printMessageAt("press s to crawl", 20, linePos);linePos +=30;
  io.printMessageAt("press f to fight", 20, linePos);linePos +=30;
  io.printMessageAt("press r to reset game", 20, linePos);linePos +=30;
  io.printMessageAt("press F2 to make bird strong", 20, linePos);
  SDL_Flip(screen);
  SDL_Delay(2000);
  SDL_Flip(screen);
}



//----- get the number of sprites in this game -----
void MenuManager::getNumberOfSprites() {
  IOManager& io = IOManager::getInstance().getInstance();
  SDL_Event event;
  bool done = false;
  bool nameDone = false;
  const string msg("How many sprites: ");
  io.clearString();
  while ( not done ) {
    Uint8 *keystate = SDL_GetKeyState(NULL);
    while ( SDL_PollEvent(&event) )
    switch (event.type) {
      case SDL_KEYDOWN: {
        if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
          done = true;
        }
        if (keystate[SDLK_RETURN]) {
          nameDone = true;
        }
        io.buildString(event);
      }
    }
    drawBackground();
    io.printStringAfterMessage(msg, 20, 120);
    if ( nameDone ) {
      std::string number = io.getString();
      std::stringstream strm;
      strm << number;
      strm >> numberOfSprites;
      strm.clear(); // clear error flags
      strm.str(std::string()); // clear contents
      strm << "You will fight with " << numberOfSprites << " Pink Bird !!!";
      io.printMessageAt(strm.str(), 20, 160);
      SDL_Flip(screen);
      SDL_Delay(1000);
      done = true;
    }
    if ( !done ) {
      SDL_Flip(screen);
    }
  }
}

void MenuManager::play() {
  SDL_Event event;
  bool done = false;
  // Here, we need to create an instance of the Manager,
  // the one that manages the game not the menu:
  Manager manager;
  manager.pause();

  while ( not done ) {
    drawBackground();
    menu.draw();
    SDL_Flip(screen);
    while ( SDL_PollEvent(&event) ) {
      if (event.type ==  SDL_QUIT) { break; }
      if(event.type == SDL_KEYDOWN) {
        std::cout << "KEY: " << event.key.keysym.sym << std::endl;
        switch ( event.key.keysym.sym ) {
          case SDLK_ESCAPE :
          case SDLK_q : {
            done = true;
            break;
          }
          case SDLK_RETURN : {
            menu.lightOn();
            if ( menu.getIconClicked() == "Start Game" ) {
              // but first, unpause the game:
              manager.unpause();
              int result = manager.play(); 
              Records.push_back( result );
              showResultPage(result);
              manager.pause();
            }
            if ( menu.getIconClicked() == "Bird Number" ) {
              getNumberOfSprites();
              // After we get the number, we must set it to Manager:
              manager.setNumberOfSprites( numberOfSprites );
              manager.resetGame();//renew the game, valgrin will not works well here
            }
            if ( menu.getIconClicked() == "Help" ) {
              // Here is where we explain how to play the game"
              std::cout << "Give help  ..." << std::endl;
              getHelpPage();
            }
            if ( menu.getIconClicked() == "Exit" ) {
              drawBackground();
              menu.draw();
              SDL_Flip(screen);
              SDL_Delay(250);
              done = true;
            }
          }
          break;
          case SDLK_DOWN   : {
              menu.increment();
            break;
          }
          case SDLK_UP   : {
              menu.decrement();
            break;
          }
          default          : break;
        }
      }
      if(event.type == SDL_KEYUP) { 
        menu.lightOff();
      }
    }
  }
}
