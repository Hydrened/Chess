#ifndef PLATE_H
#define PLATE_H

#include "game.h"
#include "piece.h"
class Game;
class Piece;

class Plate {
private:
    Game* game;

    H2DE_BasicObject* hoverEffect = nullptr;
    std::vector<Piece*> pieces = {};

    void createHoverEffect();
    void create();
    void createPieces();
    void createPiecePattern(float x, float offsetY, PieceType type);
    void createPawns();
    void createRooks();
    void createKnights();
    void createBishops();
    void createQueens();
    void createKings();

    void destroyHoverEffect();
    void destroyPieces();

public:
    Plate(Game* game);
    ~Plate();

    void deletePiece(Piece* piece);
    void hoverTile(const H2DE_LevelPos& pos) const;
    void blurTile() const;

    Piece* getPiece(const H2DE_LevelPos& pos) const;
};

#endif
