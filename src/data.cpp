#include "data.h"

// INIT
Data::Data() {
    initStrTeams();
    initTeamMinY();
    initTeamMaxY();

    initStrPieces();
    initPiecesMoveMovements();
    initPiecesOverrideMovements();
}
 
void Data::initStrTeams() {
    strTeams[TEAM_WHITE] = "white";
    strTeams[TEAM_BLACK] = "black";
}

void Data::initTeamMinY() {
    teamsMinY[TEAM_WHITE] = plateSize - 1.0f;
    teamsMinY[TEAM_BLACK] = 0.0f;
}

void Data::initTeamMaxY() {
    teamsMaxY[TEAM_WHITE] = teamsMinY[TEAM_BLACK];
    teamsMaxY[TEAM_BLACK] = teamsMinY[TEAM_WHITE];
}

void Data::initStrPieces() {
    strPieces[PIECE_TYPE_KING] = "king";
    strPieces[PIECE_TYPE_QUEEN] = "queen";
    strPieces[PIECE_TYPE_BISHOP] = "bishop";
    strPieces[PIECE_TYPE_KNIGHT] = "knight";
    strPieces[PIECE_TYPE_ROOK] = "rook";
    strPieces[PIECE_TYPE_PAWN] = "pawn";
}

void Data::initPiecesMoveMovements() {
    piecesMoveMovements[PIECE_TYPE_KING] = {
        { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f },
        { -1.0f, 0.0f }, { 1.0f, 0.0f },
        { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f },
    };

    addDiagonalsToMovements(piecesMoveMovements[PIECE_TYPE_QUEEN]);
    addLinesToMovements(piecesMoveMovements[PIECE_TYPE_QUEEN]);

    addDiagonalsToMovements(piecesMoveMovements[PIECE_TYPE_BISHOP]);

    piecesMoveMovements[PIECE_TYPE_KNIGHT] = {
        { -1.0f, -2.0f }, { 1.0f, -2.0f },
        { -1.0f, 2.0f }, { 1.0f, 2.0f },
        { -2.0f, -1.0f }, { 2.0f, -1.0f },
        { -2.0f, 1.0f }, { 2.0f, 1.0f },
    };

    addLinesToMovements(piecesMoveMovements[PIECE_TYPE_ROOK]);

    piecesMoveMovements[PIECE_TYPE_PAWN] = {
        { 0.0f, 1.0f },
    };
}

void Data::initPiecesOverrideMovements() {
    piecesOverrideMovements[PIECE_TYPE_KING] = piecesMoveMovements[PIECE_TYPE_KING];
    piecesOverrideMovements[PIECE_TYPE_QUEEN] = piecesMoveMovements[PIECE_TYPE_QUEEN];
    piecesOverrideMovements[PIECE_TYPE_BISHOP] = piecesMoveMovements[PIECE_TYPE_BISHOP];
    piecesOverrideMovements[PIECE_TYPE_ROOK] = piecesMoveMovements[PIECE_TYPE_ROOK];
    piecesOverrideMovements[PIECE_TYPE_KNIGHT] = piecesMoveMovements[PIECE_TYPE_KNIGHT];
    piecesOverrideMovements[PIECE_TYPE_PAWN] = {
        { -1.0f, 1.0f }, { 1.0f, 1.0f },
    }; 
}

void Data::addDiagonalsToMovements(std::vector<H2DE_LevelPos>& movements) {
    for (int i = -plateSize + 1; i < plateSize; i++) {
        if (i != 0) {
            const float fi = static_cast<float>(i);
            movements.push_back({ fi, fi });
            movements.push_back({ -fi, fi });
        }
    }
}

void Data::addLinesToMovements(std::vector<H2DE_LevelPos>& movements) {
    for (int i = -plateSize + 1; i < plateSize; i++) {
        if (i != 0) {
            const float fi = static_cast<float>(i);
            movements.push_back({ 0.0f, fi });
            movements.push_back({ fi, 0.0f });
        }
    }
}

// CLEANUP
Data::~Data() {

}

// GETTER
bool Data::isPositionInPlate(const H2DE_LevelPos& pos) const {
    static float max = plateSize - 1.0f;

    bool x = (pos.x >= 0.0f && pos.x <= max);
    bool y = (pos.y >= 0.0f && pos.y <= max);

    return x && y;
}

const H2DE_LevelPos Data::getMoveOffset(const H2DE_LevelPos& offset, Team team) {
    return (team == TEAM_BLACK) ? offset : H2DE_LevelPos{ offset.x, -offset.y };
}

// maps
template<typename Piece_T>
const Piece_T Data::getPieceData(const std::unordered_map<PieceType, Piece_T>& data, PieceType type) {
    auto it = data.find(type);

    if (it == data.end()) {
        throw std::runtime_error("Can't find piece type");
    }

    return it->second;
}

const std::string Data::getStrPiece(PieceType type) const {
    return Data::getPieceData(strPieces, type);
}

const std::vector<H2DE_LevelPos> Data::getPossiblePieceMovements(const Plate* plate, const Piece* piece) const {
    std::vector<H2DE_LevelPos> res = {};

    const H2DE_LevelPos& pos = piece->getPos();
    const PieceType& type = piece->getType();
    const Team team = piece->getTeam();

    const std::vector<H2DE_LevelPos> moveMovements = Data::getPieceData(piecesMoveMovements, type);
    const std::vector<H2DE_LevelPos> overrideMovements = Data::getPieceData(piecesOverrideMovements, type);

    std::copy_if(moveMovements.begin(), moveMovements.end(), std::back_inserter(res), [this, plate, pos, team](H2DE_LevelPos offset) {
        offset = Data::getMoveOffset(offset, team);
        const H2DE_LevelPos destPos = pos + offset;

        Piece* otherPiece = plate->getPiece(destPos);
        if (otherPiece != nullptr) {
            return false;
        }

        return isPositionInPlate(destPos);
    });

    std::copy_if(overrideMovements.begin(), overrideMovements.end(), std::back_inserter(res), [plate, pos, team](H2DE_LevelPos offset) {
        offset = Data::getMoveOffset(offset, team);
        const H2DE_LevelPos destPos = pos + offset;

        Piece* otherPiece = plate->getPiece(destPos);
        if (otherPiece != nullptr) {
            bool otherPieceIsFromOtherPlayer = (otherPiece->getTeam() != team);
            return otherPieceIsFromOtherPlayer;
        }

        return false;
    });

    for (H2DE_LevelPos& offset : res) {
        offset = Data::getMoveOffset(offset, team);
        offset += pos;
    }

    return res;
}

template<typename Team_T>
const Team_T Data::getTeamData(const std::unordered_map<Team, Team_T>& data, Team team) {
    auto it = data.find(team);

    if (it == data.end()) {
        throw std::runtime_error("Can't find team");
    }

    return it->second;
}

const std::string Data::getStrTeam(Team team) const {
    return Data::getTeamData(strTeams, team);
}

const float Data::getTeamMinY(Team team) const {
    return Data::getTeamData(teamsMinY, team);
}

const float Data::getTeamMaxY(Team team) const {
    return Data::getTeamData(teamsMaxY, team);
}
