#ifndef WORLD__H
#define WORLD__H

#include <string>
#include "ioManager.h"
#include "frame.h"
#include "viewport.h"
#include "vector2f.h"

class World {
public:
  World(const std::string& name, int fact);
  // ~World() { ; } // The frame will be deleted by the FrameFactory
  void update();
  void draw() const;
  unsigned getWidth() const   { return worldWidth; }
  unsigned getHeight() const   { return worldHeight; }

private:
  const IOManager& io;
  Frame* const frame;
  int factor;
  unsigned frameWidth;
  unsigned worldWidth;
  unsigned worldHeight;
  float viewX;
  float viewY;
  const Viewport & view;
  World(const World&);
  World& operator=(const World&);
};
#endif
