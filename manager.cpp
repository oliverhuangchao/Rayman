#include <iostream>
#include <string>
#include <iomanip>
#include "sprite.h"
#include "gamedata.h"
#include "manager.h"
#include "drawable.h"
#include "pinkGear.h"
#include "Rayman.h"
#include "aaline.h"
#include "Bullet.h"
#include "explodingSprite.h"
#include <cmath>



bool getDistance(int a1,int a2, int b1, int b2, int value);

Manager::~Manager() { 
  // These deletions eliminate "definitely lost" and
  // "still reachable"s in Valgrind.
  //std::cout<<usedVector.size()<<'\t'<<freeVector.size()<<std::endl;
  for (unsigned i = 0; i < sprites.size(); ++i) {
    delete sprites[i];
  }
  if(freeVector.size()>0){
    freeVector.clear();
  }
  if(usedVector.size()>0){
    usedVector.clear();
  }
}

Manager::Manager():
	env( SDL_putenv(const_cast<char*>("SDL_VIDEO_CENTERED=center")) ),
	io( IOManager::getInstance() ),
	clock( Clock::getInstance() ),
	screen( io.getScreen() ),
	world_back("back", Gamedata::getInstance().getXmlInt("back/factor") ),
	world_front("front", Gamedata::getInstance().getXmlInt("front/factor") ),
	bar("pinkGear",screen),
	sound(),
	viewport( Viewport::getInstance() ),
	sprites(),
	usedVector(),
	freeVector(),
	currentSprite(0),

	makeVideo( false ),
	eatStar( false ),
	isShowHug ( true ),
	isShowMap ( true ),
	singlePostion( 0 ),

	stopWatch(true),
	stopWatch_Begin(0),
	stopWatch_End(0),
	fistStartPos(0),
	pinkGearStartPos(0),
	fistReadyToTurn(false),
	onceTouch(false),

	unlimitedMode(false),
	hudStartTime(clock.getSeconds()),
	frameCount( 0 ),
	username(  Gamedata::getInstance().getXmlStr("username") ),
	title( Gamedata::getInstance().getXmlStr("screenTitle") ),
	frameMax( Gamedata::getInstance().getXmlInt("frameMax") ),
	score(0)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
      throw string("Unable to initialize SDL: ");
    }
    SDL_WM_SetCaption(title.c_str(), NULL);
    atexit(SDL_Quit);

    // ----- set the Rayman -----
    sprites.push_back( new Rayman("Rayman","runRight", singlePostion) );
    ++singlePostion;

    //-------set the pinkGears-----------
    pinkGearStartPos = singlePostion;
    for (int i = 0; i < Gamedata::getInstance().getXmlInt("pinkGear/count"); i++){
      sprites.push_back( new pinkGear("pinkGear",singlePostion) );
      ++singlePostion;
      //usedVector.push_back(sprites[i+1]);
    }

    /*
    for (int i = 1; i < singlePostion; i++){
      usedVector.push_back(sprites[i]);
      ExplodingSprite *tmp = new ExplodingSprite( *static_cast<pinkGear *>(sprites[i]) );
      freeVector.push_back(tmp);
    }*/

    
    viewport.setObjectToTrack(sprites[currentSprite]);
}

void Manager::draw() const {
  float linePos;
  const Uint32 BLUE = SDL_MapRGB(screen->format, 0, 0, 0xFF);

  world_back.draw();
  world_front.draw();
  bar.draw();

  for (unsigned i = 0; i < sprites.size(); ++i) {
    sprites[i]->draw();
  }
  
  linePos = 20;

  io.printMessageAt("press 'h' to show the help list", 20, linePos); linePos +=30;
  io.printMessageAt("press 'm' to hide/show the map", 20, linePos); linePos +=30; 
  io.printMessageValueAt("Seconds: ", 60 - clock.getSeconds(), 20, linePos);linePos +=30; 
  
  if(isShowMap)
    drawSmallMap();

  if(isShowHug){
    //draw the contents  
    io.printMessageValueAt("fps: ", clock.getAvgFps(), 20, linePos);linePos +=30; 
    io.printMessageAt("Author: Chao Huang", 20, linePos);linePos +=30; 
/*
    io.printMessageAt("current birds stored in usedVector", 20, linePos);
    linePos +=30; 

    io.printMessageAt("broken birds stored in freeVector", 20, linePos);
    linePos +=30; 
*/
    io.printMessageAt("press m to show map", 20, linePos);linePos +=30;
    io.printMessageAt("Press T to switch sprites", 20, linePos);linePos +=30; 
    io.printMessageAt("press e to make game easier", 20, linePos);linePos +=30;
    io.printMessageAt("press a/d to turn left/right", 20, linePos);linePos +=30;
    io.printMessageAt("press w to jump", 20, linePos);linePos +=30;
    io.printMessageAt("press s to crawl", 20, linePos);linePos +=30;
    io.printMessageAt("press f to fight", 20, linePos);linePos +=30;
    io.printMessageAt("press r to reset game", 20, linePos);linePos +=30;
    io.printMessageAt("press F2 to make bird strong", 20, linePos);linePos +=30;

    //draw the lines
    float left = (float)Gamedata::getInstance().getXmlInt("myhud/start/left"); 
    float top = (float)Gamedata::getInstance().getXmlInt("myhud/start/top");
    float right = (float)Gamedata::getInstance().getXmlInt("myhud/start/right"); 
    float buttom = linePos + top;

    Draw_AALine(screen, left, top, left, buttom, 2.0f, BLUE);
    Draw_AALine(screen, left, buttom, right, buttom, 2.0f, BLUE);
    Draw_AALine(screen, right, buttom, right, top, 2.0f, BLUE);
    Draw_AALine(screen, right, top, left, top, 2.0f, BLUE);

  }
  if(unlimitedMode){
    io.printMessageAt("unlimited health mode", 500, 50);
  }
  else{
    io.printMessageAt("limited health mode", 500, 50);
  }

  viewport.draw();

  SDL_Flip(screen);
}

void Manager::makeFrame() {
  std::stringstream strm;
  strm << "frames/" << username<< '.' 
       << std::setfill('0') << std::setw(4) 
       << frameCount++ << ".bmp";
  std::string filename( strm.str() );
  std::cout << "Making frame: " << filename << std::endl;
  SDL_SaveBMP(screen, filename.c_str());
}

void Manager::update() {
  ++clock;
  world_back.update();
  world_front.update();
    
  Uint32 ticks = clock.getElapsedTicks();
  
  if(clock.getSeconds() == 4){
    isShowHug = false;
  }


  sprites[0]->update(ticks);//update our hero Rayman

  for (int i = 1; i < (int)sprites.size(); ++i)// update pinkGear start @ 1 position
  {
    //Begin to update the relationship between fist and Rayman
    //if fist exist and it is now update fist
    if(fistStartPos != 0 && i >= fistStartPos){  
      sprites[i]->update(ticks, sprites[0]);
      if(abs(sprites[i]->X() - sprites[0]->X()) > sprites[i]->getFistRange() ){
          fistReadyToTurn = true;
      }
      if(spriteConflict(sprites[i],sprites[0]) && fistReadyToTurn){// if the fist gets back to Rayman
        Bullet *tmp = static_cast<Bullet*>(sprites[i]);
        delete tmp;
        sprites.erase(sprites.begin() + i);
        fistStartPos = 0;
        fistReadyToTurn = false;
        singlePostion--;
        onceTouch = false;//so the fist can hit the bird again
        static_cast<Rayman *>(sprites[currentSprite])->setHaveFist(true);//so the Rayman's fist is available
      }
    }
    else{// if the fist hit the pinkGear
      sprites[i]->update(ticks);
      if (spriteConflict(sprites[i], sprites[fistStartPos]) 
          && !static_cast<pinkGear*>(sprites[i])-> getAlreadyHit()
          && fistStartPos != 0
          && !onceTouch){

        //give a sound to the sprite
        sound[0];
        onceTouch = true;
        int chealth;
        //the health bar get shorter
        if(!unlimitedMode){
          chealth = static_cast<pinkGear*>(sprites[i])->getCurrentHealth();
          static_cast<pinkGear*>(sprites[i])->setCurrentHealth(--chealth);
          bar.update(chealth);
        }

        //if the health bar equal to zero
        if(chealth == 0){
          static_cast<Rayman*>(sprites[0])->detach(static_cast<pinkGear*>(sprites[i]));
          pinkGear *tmp = static_cast<pinkGear*>(sprites[i]);   
          sprites[i] = new ExplodingSprite( *tmp );
          static_cast<pinkGear*>(sprites[i])->setAlreadyHit(true);
          static_cast<pinkGear*>(sprites[i])->setIsDangerous(false);
          score++;
          delete tmp;
          /*
            std::cout<<"hello"<<std::endl;
            sprites[i] = freeVector[i];
          */
        }
        if(chealth == 1){// if the pink gear goes to the dangerous condition
          static_cast<pinkGear*>(sprites[i])->setIsDangerous(true);
          pinkGear *tmp = static_cast<pinkGear*>(sprites[i]);
          sprites[i] = new pinkGear (*tmp, "pinkGearRed");
          delete tmp;
          static_cast<Rayman*>(sprites[0])->attach(static_cast<pinkGear*>(sprites[i]));
        }

      }
    }
  }

  if ( makeVideo && frameCount < frameMax ) {
    makeFrame();
  }

  viewport.update(); 
}

bool Manager::spriteConflict( Drawable* multi, Drawable* single ){
  if ( getDistance(multi->X()+multi->getFrameWidth()/2, multi->Y()+multi->getFrameHeight()/2, single->X()+single->getFrameWidth()/2,single->Y() + single->getFrameHeight()/2, 50)){
   
    return true;
  }
  else
    return false;
}

bool getDistance(int x1,int y1, int x2, int y2, int value){// return the value 
  int result = sqrt( pow( (x1 - x2), 2 ) + pow( ( y1 - y2 ), 2 ) );
  if (result <= value)
    return true;
  else
    return false;
}

int Manager::play() {
  SDL_Event event;
  std::cout<<sprites.size()<<std::endl;

  bool done = false;
  bool keyCatch = false;
  bool keepTouch = false;//keep touching the keyboard

  while ( not done ) {

    SDL_PollEvent(&event);
    Uint8 *keystate = SDL_GetKeyState(NULL);
    if(event.type ==  SDL_QUIT) { done = true; break; }
//-------------------- key UP function Add here--------------
    if(event.type == SDL_KEYUP) { // if anykey is up then see which key

      keepTouch = false;
      keyCatch = false;
      if( !keystate[SDLK_w] ){//release w but keep a or d 
        if(currentSprite == 0){
            if( keystate[SDLK_a] ){
              static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
              static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
              keepTouch = true;
            }
            if( keystate[SDLK_d] ){
              static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
              static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
              keepTouch = true;
            }
        }
      }

      if( !keystate[SDLK_s] ){//release s but keep a or d
       if(currentSprite == 0){
          static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(false);
          if( keystate[SDLK_a] ){
              static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
              static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
              keepTouch = true;
            }
          if( keystate[SDLK_d] ){
              static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
              static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
              keepTouch = true;
            }
        }
      }

      if( keystate[SDLK_w] && !keystate[SDLK_a] && !keystate[SDLK_d] ){//release a or d but keep w
        if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(false);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
            keepTouch = false;
        }
      }

      if( keystate[SDLK_s] && !keystate[SDLK_a] && !keystate[SDLK_d] ){//release a or d but keep s
        if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(false);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
            keepTouch = false;
        }
      }

      if( !keystate[SDLK_f] && !stopWatch ){//release f 
        if( currentSprite == 0 && static_cast<Rayman *>(sprites[currentSprite])->getHaveFist() ){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsFist(false);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
            stopWatch_End = clock.getSeconds();
            keepTouch = false;
            stopWatch = true;
            if (static_cast<Rayman *>(sprites[currentSprite]) -> getFaceDirection() == 1){
              sprites.push_back( new Bullet("singleFistRight",singlePostion) );
              static_cast<Bullet *>(sprites[singlePostion]) -> setFaceDirection(1);
              sprites[singlePostion] -> X( sprites[currentSprite] -> X() + 50);
              sprites[singlePostion] -> Y( sprites[currentSprite] -> Y() + 80);
              sprites[singlePostion] -> velocityX( static_cast<Bullet*>(sprites[singlePostion]) -> getSpeed() );
              sprites[singlePostion] -> velocityY( 0 );
              fistStartPos = singlePostion;
              ++singlePostion;
            }
            else{
              sprites.push_back( new Bullet("singleFistLeft",singlePostion) );
              static_cast<Bullet *>(sprites[singlePostion]) -> setFaceDirection(-1);//set the fist direction
              sprites[singlePostion] -> X( sprites[currentSprite] -> X() + 50);
              sprites[singlePostion] -> Y( sprites[currentSprite] -> Y() + 80);
              sprites[singlePostion] -> velocityX( -1 * static_cast<Bullet*>(sprites[singlePostion]) -> getSpeed() );
              sprites[singlePostion] -> velocityY( 0 );
              fistStartPos = singlePostion;
              ++singlePostion;
            }
            static_cast<Rayman *>(sprites[currentSprite])->setHaveFist(false);
        }
      }


    }
// ------------------ kEY Down Here------------- 
    if(event.type == SDL_KEYDOWN) {
      keepTouch = true; 

      if (keystate[SDLK_ESCAPE] || keystate[SDLK_q]) {
        done = true;
        break;
      }

      if (!keyCatch) //keyboard touch once function
      {
        switch (event.key.keysym.sym){
          case SDLK_t:
            currentSprite = (currentSprite+1) % sprites.size();
            viewport.setObjectToTrack(sprites[currentSprite]);
            break;
          case SDLK_l: clock.toggleSloMo();
            break;
          case SDLK_p:
            if ( clock.isPaused() ) clock.unpause();
            else clock.pause();
            break;
          case SDLK_F4:
            std::cout << "Making video frames" << std::endl;
            makeVideo = true;
            break;
          case SDLK_F2:
            unlimitedMode = !unlimitedMode;
            break;
          case SDLK_e://if they are pinkGear
            for (int i=1;i<(int)sprites.size();i++){
              sprites[i] -> setIsMoved(! sprites[i] -> getIsMoved());
            }
            break;
          case SDLK_h:
            isShowHug = !isShowHug;
            break;
          case SDLK_m:
            isShowMap = !isShowMap;
            break;
          case SDLK_w: 
            if(currentSprite == 0)
              static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(true);
            break;
          case SDLK_r:
          /*
            score = 0;
            static_cast<Rayman*>(sprites[0])->detachAll();
            if(fistStartPos != 0){
              //std::cout<<fistStartPos<<std::endl;
              for (int i = pinkGearStartPos;i<fistStartPos;i++){
                Drawable *tmp = sprites[i];   
                sprites[i] = new pinkGear("pinkGear", i);
                delete tmp;
              }
            }
            else{
              for (int i = pinkGearStartPos;i<(int)sprites.size();i++){
                Drawable *tmp = sprites[i];   
                sprites[i] = new pinkGear("pinkGear", i);
                //sprites[i] = freeVector[0];
                //freeVector.erase(freeVector.begin()+i);
                delete tmp;
              }
               
              //for (int i = 0;i<freeVector.size();i++){
                //Drawable *tmp = sprites[i];   
                //sprites[i] = new pinkGear("pinkGear", i);
                //sprites[i] = freeVector[0];
                //freeVector.erase(freeVector.begin()+i);
               // delete tmp;
              }
            }
            */
            resetGame();
            break;
          default:
            break;
        }
        keyCatch = true;
      }

      if ( keystate[SDLK_a] &&  keepTouch) { //keep running to left 
        if (currentSprite == 0){
          static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
          static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
        }
      }

      if ( keystate[SDLK_d] &&  keepTouch) { //keep runing to right
        if (currentSprite == 0){
          static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
          static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
        }
      }
      if ( keystate[SDLK_s] && keepTouch) {//keep crawing position
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(true);
          }
      }

      if ( keystate[SDLK_s] && keystate[SDLK_d] ) {//right crawing
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
          }
      }
      if ( keystate[SDLK_s] && keystate[SDLK_a] ) {//left crawing
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
          }
      }

      if ( keystate[SDLK_w] && keystate[SDLK_d] ) {//right jumping
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(1);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(1);
          }
      }
      if ( keystate[SDLK_w] && keystate[SDLK_a] ) {//left jumping
          if(currentSprite == 0){
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsJump(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(-1);
            static_cast<Rayman *>(sprites[currentSprite]) -> setFaceDirection(-1);
          }
      }

      if ( keystate[SDLK_f] && keepTouch ) {//keep fisting
          if (stopWatch){
            stopWatch_Begin = clock.getSeconds();
            stopWatch = false;
          }

          if( currentSprite == 0 && static_cast<Rayman *>(sprites[currentSprite]) -> getHaveFist() ){
            static_cast<Rayman *>(sprites[currentSprite]) -> setFistDuration( clock.getSeconds() - stopWatch_Begin );
            static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsFist(true);
            static_cast<Rayman *>(sprites[currentSprite]) -> setIsCrawl(false);
          }
      }
    }

    if (currentSprite == 0 && !keepTouch){
          static_cast<Rayman *>(sprites[currentSprite]) -> setFrameDirection(0);
    }
    
    draw();
    update();
    if(winGame()){
      std::cout<<"win"<<std::endl;
      pause();
      return clock.getSeconds();
    }
    else{
      if(loseGame()){
        std::cout<<"lost"<<std::endl;
        pause();
        return 0;
      }
    }  
  }
  return -1;
}

bool Manager::winGame(){
  if (score == (int) sprites.size() -1){
    return true;
  }
  else
    return false;
}

bool Manager::loseGame(){
  if (clock.getSeconds() == 60){
    return true;
  }
  else
    return false;
}


//----- add more pink sprite in this game -----
void Manager::setNumberOfSprites(int number) {
  if ( number > static_cast<int>( sprites.size() -1 ) ) {
    number = number - sprites.size() + 1;
    for (int i = 0; i < number; ++i) {
      sprites.push_back( new pinkGear("pinkGear", singlePostion) );
       ++singlePostion;
    }
  }
  else {
    number = sprites.size() - 1 - number;
    for (int i = 0; i < number; ++i) {
      delete sprites.back();
      sprites.pop_back();
      --singlePostion;
    }
  }
}

void Manager::resetGame()//rese the game
{
  score = 0;
  clock.start();// time should also be restarted again
  static_cast<Rayman*>(sprites[0])->detachAll();
  if(fistStartPos != 0){
    //std::cout<<fistStartPos<<std::endl;
    for (int i = pinkGearStartPos;i<fistStartPos;i++){
      Drawable *tmp = sprites[i];   
      sprites[i] = new pinkGear("pinkGear", i);
      delete tmp;
    }
  }
  else{
    for (int i = pinkGearStartPos;i<(int)sprites.size();i++){
      Drawable *tmp = sprites[i];   
      sprites[i] = new pinkGear("pinkGear", i);
      //sprites[i] = freeVector[0];
      //freeVector.erase(freeVector.begin()+i);
      delete tmp;
    }
     /* 
    for (int i = 0;i<freeVector.size();i++){
      Drawable *tmp = sprites[i];   
      sprites[i] = new pinkGear("pinkGear", i);
      //sprites[i] = freeVector[0];
      //freeVector.erase(freeVector.begin()+i);
      delete tmp;
    }*/
  }
}

void Manager::drawSmallMap() const{ // draw a mini map for the game
  const Uint32 BLUE = SDL_MapRGB(screen->format, 0, 0, 0xFF);
  const Uint32 RED = SDL_MapRGB(screen->format, 0xFF, 0, 0);
  const Uint32 GREEN = SDL_MapRGB(screen->format, 0, 0xFF, 0);


  float left = (float)Gamedata::getInstance().getXmlInt("myMap/start/left"); 
  float top = (float)Gamedata::getInstance().getXmlInt("myMap/start/top");
  float right = (float)Gamedata::getInstance().getXmlInt("myMap/start/right"); 
  float buttom = (float)Gamedata::getInstance().getXmlInt("myMap/start/buttom"); 

  Draw_AALine(screen, left, top, left, buttom, 2.0f, BLUE);
  Draw_AALine(screen, left, buttom, right, buttom, 2.0f, BLUE);
  Draw_AALine(screen, right, buttom, right, top, 2.0f, BLUE);
  Draw_AALine(screen, right, top, left, top, 2.0f, BLUE);

  float RaymanX = sprites[0]->X() / world_back.getWidth() * (right-left) + left;
  float RaymanY = sprites[0]->Y() / world_back.getHeight() * (buttom-top) + top;

  Draw_AALine(screen, RaymanX, RaymanY, RaymanX, RaymanY+20, 10.0f, GREEN);
  

  float spriteX, spriteY;
  for(int k = 1;k<(int)sprites.size();k++){
     spriteX = sprites[k]->X() / world_back.getWidth() * (right-left) + left;
     spriteY = sprites[k]->Y() / world_back.getHeight() * (buttom-top) + top;
     Draw_AALine(screen, spriteX, spriteY, spriteX, spriteY+5, 10.0f, RED);
  }
  //std::cout<<RaymanX<<std::endl;
}
