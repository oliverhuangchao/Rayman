#ifndef pinkGear__H
#define pinkGear__H
#include <string>
#include "drawable.h"
//#include "chunk.h"
#include <list>

class pinkGear : public Drawable {
public:
  pinkGear(const std::string&,const int);
  pinkGear(const std::string&, const Vector2f& pos, const Vector2f& vel, const int);
  pinkGear(const std::string&, const Vector2f& pos, const Vector2f& vel, const Frame*,const int);
  pinkGear(const pinkGear& s);
  pinkGear(const pinkGear& s, const std::string&);
  virtual ~pinkGear();
  pinkGear& operator=(const pinkGear&);

  virtual const Frame* getFrame() const { return frame; }
  virtual void draw() const;
  virtual void update(Uint32 ticks);
  virtual void update(Uint32 ticks, Drawable *BrotherpinkGear);// have to achieve because it is a virtual function

  int getMoveRange() const {return moveRange;}
  void setMoveRange(int x){moveRange = x;}
  int getDistance(const pinkGear*) const;

  bool getAlreadyHit() const {return alreadyHit;}
  void setAlreadyHit(bool x){alreadyHit = x;}

  int  getCurrentHealth() const {return currentHealth;}
  void setCurrentHealth(int x){currentHealth = x;}


  bool getIsDangerous() const {return isDangerous;}
  void setIsDangerous(bool x){isDangerous = x;}


  void setPlayerPos(const Vector2f& p); 
  void CircleMovement(float, float);
private:
  const Frame * frame;
  int frameWidth;
  int frameHeight;
  int worldWidth;
  int worldHeight;
  int moveRange;
  int startX;
  int startY;
  int currentHealth;//current health for each pinkGear
  bool isDangerous;//whether the pinkGear is dangerous or not
  bool alreadyHit;//whether the pinkGear is hit or not
  // the explosion part of this sprite
  //std::list<Chunk> chunks; // An ExplodingSprite is a list of sprite chunks
  //std::list<Chunk> freeList; // When a chunk gets out of range it goes here
};
#endif
