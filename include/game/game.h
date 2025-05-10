#ifndef GAME_H
#define GAME_H

#include <H2DE/H2DE.h>
#include "utils.h"
#include "data.h"
#include "plate.h"
class Data;
class Plate;

class Game {
private:
    H2DE_Engine* engine = nullptr;
    Plate* plate = nullptr;

    Data* data = nullptr;
    Team turn = TEAM_WHITE;

    Piece* selectedPiece = nullptr;
    H2DE_LevelPos selectedPieceOffset = { 0.0f, 0.0f };

    void initEngine();
    void initData();
    void createPlate();

    void handleEvents(SDL_Event event);
    void onMouseButtonDown(SDL_Event event);
    void onMouseButtonUp(SDL_Event event);
    void onMouseMotion(SDL_Event event);
    void onKeyDown(SDL_Event event);

    void restart();

    static inline H2DE_LevelPos getFlooredPos(const H2DE_LevelPos& pos) { return { std::floor(pos.x), std::floor(pos.y) }; }
    const H2DE_LevelPos convertToLevelPos(const H2DE_AbsPos& pos) const;

public:
    Game();
    ~Game();

    void run() const;

    void nextTurn();

    inline H2DE_Engine* getEngine() const { return engine; }
    inline const Data* getData() const { return data; }
    inline const Team& getTurn() const  { return turn; }
};

#endif
