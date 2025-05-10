#include "game.h"

// INIT
Game::Game() {
    initData();
    initEngine();
    createPlate();
}

void Game::initEngine() {
    H2DE_EngineData engineData = H2DE_EngineData();
    engineData.window.fps = 60;
    engineData.window.title = "Chess";
    engineData.window.size = { 960, 960 };
    engineData.camera.width = data->getPlateSize();
    engine = H2DE_CreateEngine(engineData);

    H2DE_SetHandleEventsCall(engine, [this](SDL_Event event) {
        handleEvents(event);
    });

    H2DE_SetWindowIcon(engine, "window-icon.png");
    H2DE_LoadAssets(engine, "assets");
}

void Game::initData() {
    data = new Data();
}

void Game::createPlate() {
    plate = new Plate(this);
}

// CLEANUP
Game::~Game() {
    H2DE_DestroyEngine(engine);
}

// RUN
void Game::run() const {
    H2DE_RunEngine(engine);
}

// EVENTS
void Game::handleEvents(SDL_Event event) {
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN: onMouseButtonDown(event); break;
        case SDL_MOUSEBUTTONUP: onMouseButtonUp(event); break;
        case SDL_MOUSEMOTION: onMouseMotion(event); break;
        case SDL_KEYDOWN: onKeyDown(event); break;
        default: break;
    }
} 

void Game::onMouseButtonDown(SDL_Event event) {
    if (event.button.button != SDL_BUTTON_LEFT) {
        return;
    }

    if (selectedPiece != nullptr) {
        return;
    }

    const H2DE_LevelPos pos = convertToLevelPos({ event.button.x, event.button.y });
    const H2DE_LevelPos flooredPos = Game::getFlooredPos(pos);

    Piece* piece = plate->getPiece(flooredPos);

    if (piece == nullptr) {
        return;
    }

    selectedPieceOffset = pos - flooredPos;
    selectedPiece = piece;
    H2DE_SetObjectIndex(selectedPiece->getObject(), 3);
}

void Game::onMouseButtonUp(SDL_Event event) {
    if (event.button.button != SDL_BUTTON_LEFT) {
        return;
    }

    if (selectedPiece == nullptr) {
        return;
    }

    if (event.button.button == SDL_BUTTON_LEFT) {
        const H2DE_LevelPos pos = convertToLevelPos({ event.button.x, event.button.y });
        selectedPiece->moveTo(Game::getFlooredPos(pos));

        H2DE_SetObjectIndex(selectedPiece->getObject(), 1);
        selectedPiece = nullptr;
        selectedPieceOffset = { 0.0f, 0.0f };

        plate->blurTile();
    }
}

void Game::onMouseMotion(SDL_Event event) {
    if (selectedPiece == nullptr) {
        return;
    }

    const H2DE_LevelPos pos = convertToLevelPos({ event.button.x, event.button.y });
    const H2DE_LevelPos objectPos = pos - selectedPieceOffset;

    H2DE_SetObjectPos(selectedPiece->getObject(), objectPos);

    const H2DE_LevelPos flooredPos = Game::getFlooredPos(pos);
    plate->hoverTile(flooredPos);
}

void Game::onKeyDown(SDL_Event event) {
    switch (event.key.keysym.sym) {
        case SDLK_r: restart(); break;
        case SDLK_LEFT: break;
        case SDLK_RIGHT: break;
        default: break;
    }
}

// ACTIONS
void Game::nextTurn() {
    turn = (turn == TEAM_WHITE) ? TEAM_BLACK : TEAM_WHITE;
}

void Game::restart() {
    delete plate;
    plate = new Plate(this);
}

// GETTER
const H2DE_LevelPos Game::convertToLevelPos(const H2DE_AbsPos& pos) const {
    static float cellSize = std::round(H2DE_GetWindowSize(engine).x / data->getPlateSize());
    return { pos.x / cellSize, pos.y / cellSize };
}
