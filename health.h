#include "vector2f.h"
#include "ioManager.h"
#include "aaline.h"

class Health {
public:
  Health(const std::string&, SDL_Surface*);
  Health(SDL_Surface*, int sx, int sy, int tl, int cl, 
         float t, int inc, Uint32 c, float sp);
  void draw() const;
  void update(int);//Uint32
  void reset() { currentLength = totalLength; }
private:
  SDL_Surface* screen;
  Vector2f start;
  int totalLength;
  int currentLength;
  int step;//how many step for each health bar
  int thick;
  int increments;
  float interval;
  int deltaTime;
  const Uint32 RED;
  const Uint32 GRAY;
  const Uint32 BLACK;
  const Uint32 color;
  void drawBox() const;
  Health(const Health&);
  Health& operator=(const Health&);
};
