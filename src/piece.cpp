#include "piece.h"

// INIT
Piece::Piece(Game* g, Plate* p, PieceType t, Team te, const H2DE_LevelPos& po) : game(g), plate(p), type(t), team(te), pos(po) {
    createObject();
}

void Piece::createObject() {
    static H2DE_Engine* engine = game->getEngine();
    static const Data* data = game->getData();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.rect.addPos(pos);
    od.index = 1;

    const std::string textureName = data->getStrTeam(team) + '-' + data->getStrPiece(type) + ".png";

    H2DE_SurfaceData sd = H2DE_SurfaceData();
    sd.textureName = textureName;
    sd.rect = { 0.0f, 0.0f, 1.0f, 1.0f };
    sd.scaleMode = H2DE_SCALE_MODE_BEST;

    H2DE_BasicObjectData bod = H2DE_BasicObjectData();
    bod.surfaces["main"] = H2DE_CreateTexture(engine, sd, H2DE_TextureData());

    object = H2DE_CreateBasicObject(engine, od, bod);
}

// CLEANUP
Piece::~Piece() {
    static H2DE_Engine* engine = game->getEngine();
    H2DE_DestroyObject(engine, object);
}

// ACTIONS
void Piece::moveTo(const H2DE_LevelPos& destPos) {
    static const Data* data = game->getData();
    static const int& moveDuration = data->getMoveDuration();

    if (!isMoveLegal(destPos) || game->getTurn() != team) {
        H2DE_SetObjectPos(object, pos, moveDuration, H2DE_EASING_LINEAR, true);
        return;
    }

    bool willPromote = isGoingToPromote(destPos);
    bool willOverride = isGoingToOverride(destPos);
    bool willCastle = isToGoingToCastle(destPos);

    playSound(willPromote, willOverride, willCastle);
    setPos(destPos, willCastle);
}

void Piece::playSound(bool willPromote, bool willOverride, bool willCastle) const {
    static H2DE_Engine* engine = game->getEngine();

    const std::string soundToPlay = (willCastle) ? "castling.mp3" : (willPromote) ? "promote.mp3" : (willOverride) ? "override.mp3" : "move.mp3";
    H2DE_PlaySfx(engine, soundToPlay, 0, true);
}

void Piece::castle(const H2DE_LevelPos& destPos) {
    static const Data* data = game->getData();
    static const float& maxX = data->getPlateSize();

    const float& minY = data->getTeamMinY(team);
    bool right = (destPos.x - pos.x == 2.0f);

    if (right) {
        Piece* rightRook = plate->getPiece({ maxX - 1.0f, minY });

        if (rightRook) {
            rightRook->moveTo({ destPos.x - 1.0f, minY });
        }

    } else {
        Piece* leftRook = plate->getPiece({ 0.0f, minY });
        
        if (leftRook) {
            leftRook->moveTo({ destPos.x + 1.0f, minY });
        }
    }

    hasCastled = true;
}

void Piece::checkPromote(std::vector<H2DE_LevelPos>& possibleMoves) const {
    if (type == PIECE_TYPE_PAWN && didntMove) {
        const H2DE_LevelPos supMove = Data::getMoveOffset({ 0.0f, 2.0f }, team) + pos;
        possibleMoves.push_back(supMove);
    }
}

void Piece::checkCastling(std::vector<H2DE_LevelPos>& possibleMoves) const {
    static const Data* data = game->getData();
    static const int& plateSize = data->getPlateSize();

    if (type == PIECE_TYPE_KING && !hasCastled && didntMove) {
        const Piece* possibleRightRook = plate->getPiece({ plateSize - 1.0f, pos.y });

        if (possibleRightRook != nullptr) {
            bool isRook = (possibleRightRook->getType() == PIECE_TYPE_ROOK);
            
            if (isRook && possibleRightRook->didntMove) {

                const H2DE_LevelPos supMove = H2DE_LevelPos{ 2.0f, 0.0f } + pos;
                possibleMoves.push_back(supMove);
            }
        }

        const Piece* possibleLeftRook = plate->getPiece({ 0.0f, pos.y });
        const Piece* possibleObstacle = plate->getPiece({ 1.0f, pos.y });

        if (possibleLeftRook != nullptr) {
            bool isRook = (possibleLeftRook->getType() == PIECE_TYPE_ROOK);
            bool noObstacle = (possibleObstacle == nullptr);

            if (isRook && possibleRightRook->didntMove && noObstacle) {

                const H2DE_LevelPos supMove = H2DE_LevelPos{ -2.0f, 0.0f } + pos;
                possibleMoves.push_back(supMove);
            }
        }
    }
}

void Piece::checkWay(std::vector<H2DE_LevelPos>& possibleMoves) const {
    if (type == PIECE_TYPE_KNIGHT) {
        return;
    }

    possibleMoves.erase(std::remove_if(possibleMoves.begin(), possibleMoves.end(), [this](const H2DE_LevelPos& targetPos) {
        float dx = targetPos.x - pos.x;
        float dy = targetPos.y - pos.y;

        float stepX = (dx == 0.0f) ? 0.0f : dx / std::abs(dx);
        float stepY = (dy == 0.0f) ? 0.0f : dy / std::abs(dy);

        H2DE_LevelPos current = pos;

        while (true) {
            current.x += stepX;
            current.y += stepY;

            if (current == targetPos) {
                break;
            }

            if (plate->getPiece(current) != nullptr) {
                return true;
            }
        }

        Piece* destPiece = plate->getPiece(targetPos);
        if (destPiece != nullptr && destPiece->getTeam() == team) {
            return true;
        }

        return false;

    }), possibleMoves.end());
}

// GETTER
bool Piece::isMoveLegal(const H2DE_LevelPos& destPos) const {
    const std::vector<H2DE_LevelPos> possibleMoves = getAllPossibleMoves();

    for (const H2DE_LevelPos& possiblePos : possibleMoves) {
        if (possiblePos == destPos) {
            return true;
        }
    }

    return false;
}

bool Piece::isGoingToPromote(const H2DE_LevelPos& destPos) {
    static const Data* data = game->getData();

    bool isPawn = (type == PIECE_TYPE_PAWN);
    bool isAtMaxY = (destPos.y == data->getTeamMaxY(team));

    bool res = (isPawn && isAtMaxY);

    if (res) {
        type = PIECE_TYPE_QUEEN;

        const std::string newTextureName = data->getStrTeam(team) + '-' + data->getStrPiece(type) + ".png";
        H2DE_SetSurfaceTextureName(H2DE_GetBasicObjectSurface(object, "main"), newTextureName);
    }

    return res;
}

bool Piece::isGoingToOverride(const H2DE_LevelPos& destPos) const {
    Piece* pieceToOverride = plate->getPiece(destPos);

    bool res = pieceToOverride != nullptr;

    if (res) {
        plate->deletePiece(pieceToOverride);
    }

    return res;
}

bool Piece::isToGoingToCastle(const H2DE_LevelPos& destPos) const {
    if (type != PIECE_TYPE_KING) {
        return false;
    }

    if (!didntMove) {
        return false;
    }

    const float dx = std::abs(destPos.x - pos.x);
    return (dx == 2.0f);
}

const std::vector<H2DE_LevelPos> Piece::getAllPossibleMoves() const {
    static const Data* data = game->getData();

    std::vector<H2DE_LevelPos> res = data->getPossiblePieceMovements(plate, this);

    checkPromote(res);
    checkCastling(res);
    checkWay(res);

    return res;
}

// SETTER
void Piece::setPos(const H2DE_LevelPos& destPos, bool willCastle) {
    static const int& moveDuration = game->getData()->getMoveDuration();

    if (willCastle) {
        castle(destPos);
    }

    H2DE_SetObjectPos(object, destPos, moveDuration, H2DE_EASING_LINEAR, true);
    pos = destPos;
    didntMove = false;

    game->nextTurn();
}
