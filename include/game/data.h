#ifndef DATA_H
#define DATA_H

#include "game.h"
class Plate;
class Piece;

class Data {
private:
    const int plateSize = 8;
    const H2DE_ColorRGB tileColor1 = { 235, 236, 208, 255 };
    const H2DE_ColorRGB tileColor2 = { 115, 149, 82, 255 };
    const H2DE_ColorRGB tileHoverColor = { 255, 149, 82, 255 };
    const int moveDuration = 100;

    std::unordered_map<Team, std::string> strTeams = {};
    std::unordered_map<Team, float> teamsMinY = {};
    std::unordered_map<Team, float> teamsMaxY = {};

    std::unordered_map<PieceType, std::string> strPieces = {};
    std::unordered_map<PieceType, std::vector<H2DE_LevelPos>> piecesMoveMovements = {};
    std::unordered_map<PieceType, std::vector<H2DE_LevelPos>> piecesOverrideMovements = {};

    void initStrTeams();
    void initTeamMinY();
    void initTeamMaxY();

    void initStrPieces();
    void initPiecesMoveMovements();
    void initPiecesOverrideMovements();

    template<typename Team_T>
    static const Team_T getTeamData(const std::unordered_map<Team, Team_T>& data, Team team);

    template<typename Piece_T>
    static const Piece_T getPieceData(const std::unordered_map<PieceType, Piece_T>& data, PieceType type);

    void addDiagonalsToMovements(std::vector<H2DE_LevelPos>& movements);
    void addLinesToMovements(std::vector<H2DE_LevelPos>& movements);

    bool isPositionInPlate(const H2DE_LevelPos& pos) const;

public:
    Data();
    ~Data();

    static const H2DE_LevelPos getMoveOffset(const H2DE_LevelPos& offset, Team team);
    
    inline const int& getPlateSize() const { return plateSize; }
    inline const H2DE_ColorRGB& getTileColor1() const { return tileColor1; }
    inline const H2DE_ColorRGB& getTileColor2() const { return tileColor2; }
    inline const H2DE_ColorRGB& getTileHoverColor() const { return tileHoverColor; }
    inline const int& getMoveDuration() const { return moveDuration; }

    const std::string getStrPiece(PieceType type) const;
    const std::vector<H2DE_LevelPos> getPossiblePieceMovements(const Plate* plate, const Piece* piece) const;
    
    const std::string getStrTeam(Team team) const;
    const float getTeamMinY(Team team) const;
    const float getTeamMaxY(Team team) const;
};

#endif
