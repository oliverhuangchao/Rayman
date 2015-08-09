#include <SDL.h>
#include "menu.h"
#include "clock.h"
#include <vector>
class MenuManager {
public:
  MenuManager ();
  void play();

private:
  bool env;
  SDL_Surface *screen;
  const Clock& clock;

  SDL_Color backColor;
  Menu menu;
  int numberOfSprites;
  
  std::vector<int> Records;

  void drawBackground() const;
  MenuManager(const MenuManager&);
  MenuManager& operator=(const MenuManager&);
  void getNumberOfSprites();
  void getRecords();
  void showResultPage(int);
  void getHelpPage(); 
};
