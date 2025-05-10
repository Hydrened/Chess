#include "plate.h"

// INIT
Plate::Plate(Game* g) : game(g) {
    create();
    createHoverEffect();
    createPieces();
}

void Plate::create() {
    static H2DE_Engine* engine = game->getEngine();
    static const Data* data = game->getData();
    static const float& plateSize = data->getPlateSize();
    static const H2DE_ColorRGB& tileColor1 = data->getTileColor1();
    static const H2DE_ColorRGB& tileColor2 = data->getTileColor2();

    for (int i = 0; i < plateSize; i++) {
        for (int o = 0; o < plateSize; o++) {

            const H2DE_ColorRGB color = (i % 2 == 0)
                ? (o % 2 == 0)
                    ? tileColor1 : tileColor2
                : (o % 2 == 0)
                    ? tileColor2 : tileColor1;

            H2DE_ObjectData od = H2DE_ObjectData();
            od.rect = { static_cast<float>(o), static_cast<float>(i), 1.0f, 1.0f };
            od.index = 0;

            H2DE_SurfaceData sd = H2DE_SurfaceData();
            sd.textureName = "white.png";
            sd.rect = { 0.0f, 0.0f, 1.0f, 1.0f };
            sd.color = color;
            sd.scaleMode = H2DE_SCALE_MODE_NEAREST;

            H2DE_BasicObjectData bod = H2DE_BasicObjectData();
            bod.surfaces["main"] = H2DE_CreateTexture(engine, sd, H2DE_TextureData());

            H2DE_CreateBasicObject(engine, od, bod);
        }
    }
} 

void Plate::createHoverEffect() {
    static H2DE_Engine* engine = game->getEngine();

    H2DE_ObjectData od = H2DE_ObjectData();
    od.rect = { 0.0f, 0.0f, 1.0f, 1.0f };
    od.index = 2;

    H2DE_SurfaceData sd = H2DE_SurfaceData();
    sd.textureName = "hover-effect.png";
    sd.rect = { 0.0f, 0.0f, 1.0f, 1.0f };

    H2DE_BasicObjectData bod = H2DE_BasicObjectData();
    bod.surfaces["main"] = H2DE_CreateTexture(engine, sd, H2DE_TextureData());

    hoverEffect = H2DE_CreateBasicObject(engine, od, bod);

    H2DE_HideObject(hoverEffect);
}

void Plate::createPieces() {
    createPawns();
    createRooks();
    createKnights();
    createBishops();
    createQueens();
    createKings();
}

void Plate::createPiecePattern(float x, float offsetY, PieceType type) {
    static const float& plateSize = game->getData()->getPlateSize();

    Piece* black = new Piece(game, this, type, TEAM_BLACK, { x, offsetY });
    Piece* white = new Piece(game, this, type, TEAM_WHITE, { x, plateSize - 1 - offsetY });

    pieces.push_back(black);
    pieces.push_back(white);
}

void Plate::createPawns() {
    static const float& plateSize = game->getData()->getPlateSize();

    for (int i = 0; i < plateSize; i++) {
        const float x = i;
        createPiecePattern(x, 1.0f, PIECE_TYPE_PAWN);
    }
}

void Plate::createRooks() {
    static const float& plateSize = game->getData()->getPlateSize();

    for (int i = 0; i < 2; i++) {
        const float x = i * (plateSize - 1);
        createPiecePattern(x, 0.0f, PIECE_TYPE_ROOK);
    }
}

void Plate::createKnights() {
    static const float& plateSize = game->getData()->getPlateSize();

    for (int i = 0; i < 2; i++) {
        const float x = i * (plateSize - 3) + 1;
        createPiecePattern(x, 0.0f, PIECE_TYPE_KNIGHT);
    }
}

void Plate::createBishops() {
    static const float& plateSize = game->getData()->getPlateSize();

    for (int i = 0; i < 2; i++) {
        const float x = i * (plateSize - 5) + 2;
        createPiecePattern(x, 0.0f, PIECE_TYPE_BISHOP);
    }
}

void Plate::createQueens() {
    createPiecePattern(3.0f, 0.0f, PIECE_TYPE_QUEEN);
}

void Plate::createKings() {
    createPiecePattern(4.0f, 0.0f, PIECE_TYPE_KING);
}

// CLEANUP
Plate::~Plate() {
    destroyHoverEffect();
    destroyPieces();
}

void Plate::destroyHoverEffect() {
    static H2DE_Engine* engine = game->getEngine();
    H2DE_DestroyObject(engine, hoverEffect);
}

void Plate::destroyPieces() {
    for (Piece* piece : pieces) {
        delete piece;
        piece = nullptr;
    }

    pieces.clear();
}

// ACTIONS
void Plate::deletePiece(Piece* piece) {
    pieces.erase(std::remove(pieces.begin(), pieces.end(), piece), pieces.end());
    delete piece;
}

void Plate::hoverTile(const H2DE_LevelPos& pos) const {
    H2DE_SetObjectPos(hoverEffect, pos);
    H2DE_ShowObject(hoverEffect);
}

void Plate::blurTile() const {
    H2DE_HideObject(hoverEffect);
}

// GETTER
Piece* Plate::getPiece(const H2DE_LevelPos& pos) const {
    for (Piece* piece : pieces) {
        if (piece->getPos() == pos) {
            return piece;
        }
    }

    return nullptr;
}
