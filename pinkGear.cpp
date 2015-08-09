#include <cmath>
#include "pinkGear.h"
#include "gamedata.h"
#include "frameFactory.h"

pinkGear::~pinkGear(){}

pinkGear::pinkGear(const std::string& name, const int order) :
  Drawable(name,
           Vector2f(Gamedata::getInstance().getXmlInt(name+"/startLoc/x")+rand()%100*5, 
                    Gamedata::getInstance().getXmlInt(name+"/startLoc/y")), 
           Vector2f(300, 300),
           order,
           Gamedata::getInstance().getXmlInt(name + "/width"),
           Gamedata::getInstance().getXmlInt(name + "/height")
           ),
  frame( FrameFactory::getInstance().getFrame(name) ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  moveRange( Gamedata::getInstance().getXmlInt(name+"/moveRange") ),
  startX( Gamedata::getInstance().getXmlInt(name+"/startLoc/x")+rand()%100*5),
  startY( Gamedata::getInstance().getXmlInt(name+"/startLoc/y")),
  currentHealth(Gamedata::getInstance().getXmlInt(name+"/fullHealth")),
  isDangerous(false),
  alreadyHit(false)
  { }

pinkGear::pinkGear(const string& n, const Vector2f& pos, const Vector2f& vel, const int order):
  Drawable(n, pos, vel,order, Gamedata::getInstance().getXmlInt(n + "/width"), Gamedata::getInstance().getXmlInt(n + "/height")), 
  frame( FrameFactory::getInstance().getFrame(n) ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  moveRange ( 100 ),
  startX (0),
  startY (0),
  currentHealth(Gamedata::getInstance().getXmlInt(n+"/fullHealth")),
  isDangerous(false),
  alreadyHit(false)
{ }

pinkGear::pinkGear(const string& n, const Vector2f& pos, const Vector2f& vel,
               const Frame* frm,const int order):
  Drawable(n, pos, vel, order, Gamedata::getInstance().getXmlInt(n + "/width"),
           Gamedata::getInstance().getXmlInt(n + "/height")), 
  frame( frm ),
  frameWidth(frame->getWidth()),
  frameHeight(frame->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  moveRange ( 100 ),
  startX (0),
  startY (0),
  currentHealth(Gamedata::getInstance().getXmlInt(n+"/fullHealth")),
  isDangerous(false),
  alreadyHit(false)
{ }

pinkGear::pinkGear(const pinkGear& s) :
  Drawable(s), 
  frame(s.frame),
  frameWidth(s.getFrame()->getWidth()),
  frameHeight(s.getFrame()->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  moveRange ( s.moveRange ),
  startX ( s.startX ),
  startY ( s.startY ),
  currentHealth( s.currentHealth),
  isDangerous(s.currentHealth),
  alreadyHit(false)
{}

pinkGear::pinkGear(const pinkGear& s, const std::string& name) :
  Drawable(s), 
  frame(FrameFactory::getInstance().getFrame(name)),
  frameWidth(s.getFrame()->getWidth()),
  frameHeight(s.getFrame()->getHeight()),
  worldWidth(Gamedata::getInstance().getXmlInt("world/width")),
  worldHeight(Gamedata::getInstance().getXmlInt("world/height")),
  moveRange ( s.moveRange ),
  startX ( s.startX ),
  startY ( s.startY ),
  currentHealth( s.currentHealth),
  isDangerous(s.isDangerous),
  alreadyHit(false)
{}


pinkGear& pinkGear::operator=(const pinkGear& rhs) {
  Drawable::operator=( rhs );
  frame = rhs.frame;
  frameWidth = rhs.frameWidth;
  frameHeight = rhs.frameHeight;
  worldWidth = rhs.worldWidth;
  worldHeight = rhs.worldHeight;
  moveRange = rhs.moveRange;
  startX = rhs.startX;
  startY = rhs.startY;
  currentHealth = rhs.currentHealth;
  isDangerous = rhs.isDangerous;
  alreadyHit = rhs.alreadyHit;
  return *this;
}

void pinkGear::draw() const { 
  Uint32 x = static_cast<Uint32>(X());
  Uint32 y = static_cast<Uint32>(Y());
  frame->draw(x, y); 
}

int pinkGear::getDistance(const pinkGear *obj) const { 
  return hypot(X()-obj->X(), Y()-obj->Y());
}

void pinkGear::update(Uint32 ticks) { 
  
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);

  if(getIsMoved())
  {
    //if(velocityY() == 0) velocityY(300);
    //if( abs( Y() - startY ) > moveRange ) velocityY(-velocityY());
      if(isDangerous){
        if ( Y() < 0 ) {
          velocityY( abs( velocityY() ) );
        }
        if ( Y() > worldHeight-frameHeight ){
          velocityY( -abs( velocityY() ) );
        }

        if ( X() < 0 ){
          velocityX( abs( velocityX() ) );
        }
        if ( X() > worldWidth-frameWidth ){
          velocityX( -abs( velocityX() ) );
        }
        //std::cout<<"dangerous"<<std::endl; 
      }
      else{
        CircleMovement(100.0, 500.0);
      }
  }
  else{
      velocityY(0);
      startY = Y();
      velocityX(0);
  }
}

void pinkGear::update(Uint32 ticks, Drawable *BrotherpinkGear) {   
  Vector2f incr = getVelocity() * static_cast<float>(ticks) * 0.001;
  setPosition(getPosition() + incr);
  BrotherpinkGear->setFrameNumber(0);
}

void pinkGear::setPlayerPos(const Vector2f& p){
  if( hypot(X()-p[0], Y()-p[1]) < 100 ){
    //std::cout<<"run!"<<std::endl;
    //velocityY(0);
    velocityX(velocityX()+20);
    velocityX( -1 * velocityX() );
    velocityY( -1 * velocityY() );

    setIsMoved(true);
  }
}

void pinkGear::CircleMovement(float fixRadius, float radius){// keep the pink gear move round
  //std::cout<<"circle"<<std::endl;
  float theta;
   if ( Y() > startY && X() > startX) {//4 area
    theta = atan((Y()-startY)/(X()-startX));

    X(fixRadius * cos(theta) + startX );  Y(fixRadius * sin(theta) + startY );
    velocityX(  1*sin(theta)*radius );
    velocityY( -1*cos(theta)*radius );
  }
  if ( Y() > startY && X() < startX) {//3 area
    theta = atan((Y() - startY)/(startX - X()));
    X(-1*fixRadius * cos(theta) + startX);  Y(fixRadius * sin(theta) + startY);
    velocityX( 1*sin(theta)*radius );
    velocityY( 1*cos(theta)*radius );
  }
  if ( Y() < startY && X() < startX) {//2 area
    theta = atan((startY - Y())/(startX - X()));
   X(-1*fixRadius * cos(theta) + startX);    Y(-1*fixRadius * sin(theta) + startY);
    velocityX( -1*sin(theta)*radius );
    velocityY(  1*cos(theta)*radius );
  }
  if ( Y() < startY && X() > startX) {//1 area
    theta = atan((startY - Y())/(X() - startX));
    X(fixRadius * cos(theta) + startX);  Y(-1*fixRadius * sin(theta) + startY);
    velocityX( -1*sin(theta)*radius );
    velocityY( -1*cos(theta)*radius );
  }
  
}

