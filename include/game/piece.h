#ifndef PIECE_H
#define PIECE_H

#include "game.h"
class Game;
class Plate;

class Piece {
private:
    Game* game;
    Plate* plate;
    PieceType type;
    Team team;
    H2DE_LevelPos pos;

    bool didntMove = true;
    bool hasCastled = false;

    H2DE_BasicObject* object = nullptr;

    void createObject();
    
    void playSound(bool willPromote, bool willOverride, bool willCastle) const;
    void castle(const H2DE_LevelPos& destPos);

    void checkPromote(std::vector<H2DE_LevelPos>& possibleMoves) const;
    void checkCastling(std::vector<H2DE_LevelPos>& possibleMoves) const;
    void checkWay(std::vector<H2DE_LevelPos>& possibleMoves) const;

    bool isMoveLegal(const H2DE_LevelPos& destPos) const;
    bool isGoingToPromote(const H2DE_LevelPos& destPos);
    bool isGoingToOverride(const H2DE_LevelPos& destPos) const;
    bool isToGoingToCastle(const H2DE_LevelPos& destPos) const;
    const std::vector<H2DE_LevelPos> getAllPossibleMoves() const;

    void setPos(const H2DE_LevelPos& destPos, bool willCastle);

public:
    Piece(Game* game, Plate* plate, PieceType type, Team team, const H2DE_LevelPos& pos);
    ~Piece();

    void moveTo(const H2DE_LevelPos& pos);

    inline H2DE_BasicObject* getObject() const { return object; }
    inline H2DE_LevelPos getPos() const { return pos; }
    inline const PieceType& getType() const { return type; }
    inline const Team& getTeam() const { return team; }
};

#endif
