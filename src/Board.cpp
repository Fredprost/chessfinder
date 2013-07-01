/*
 * Matfinder, a program to help chess engines to find mat
 *
 * Copyright© 2013 Philippe Virouleau
 *
 * You can contact me at firstname.lastname@imag.fr
 * (Replace "firstname" and "lastname" with my actual names)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <sstream>
#include "Board.h"
#include "Utils.h"

namespace Board {

    const string to_string(Board::Side theSide)
    {
        switch (theSide) {
            case WHITE:
                return "White";
            case BLACK:
                return "Black";
            default:
                return "undefined";
        }
    }

    const char to_char(File theFile)
    {
        return 'a' + theFile;
    }

    const char to_char(Rank theRank)
    {
        return '0' + theRank;
    }

    Square::Square(File file, Rank rank) :
        file_(file), rank_(rank)
    {
        if (rank_ < 1 || rank_ > 8)
            Utils::handleError("Invalid rank when creating square");
    }

    Square::Square(File file, Rank rank, Piece *piece) :
        file_(file), rank_(rank)
    {
        //Can be null
        piece_ = piece;
        Square(file, rank);
    }

    Square::~Square()
    {
        Utils::output("Deleting square " + to_string() + "\n", 5);
        if (piece_)
            delete piece_;
    }

    //Should only be called by Piece::moveTo
    void Square::changePiece(Piece *newPiece)
    {
        if (newPiece == piece_)
            return;
        Utils::output("Change to :\n", 4);
        if (newPiece)
            Utils::output(newPiece->to_string(), 4);
        else
            Utils::output("null", 4);
        Utils::output("\n", 4);

        if (newPiece && piece_)
            Utils::handleError("There is a already a piece on our square : "
                    + to_string() + " when moving " + newPiece->to_string()
                    + " on " + piece_->to_string());
        //handle by the chessboard
            //piece_->moveTo(NULL);
            
        //we moved a piece from the square
        piece_ = newPiece;

        //Do not newPiece.moveTo : a piece move to a square, not the opposite
    }

    Piece *Square::getPiece()
    {
        return piece_;
    }
    const File Square::getFile()
    {
        return file_;
    }

    const Rank Square::getRank()
    {
        return rank_;
    }

    const string Square::to_string()
    {
        string str;
        str += to_char(file_);
        str += to_char(rank_);
        return str;
    }

    const char Piece::to_char(Kind k)
    {
        switch (k) {
            case KNIGHT:
                return 'N';
            case BISHOP:
                return 'B';
            case ROOK:
                return 'R';
            case QUEEN:
                return 'Q';
            case KING:
                return 'K';
            case PAWN:
                return 'P';
            default:
                return ' ';
        }

    }

    const char Piece::to_uci(Kind k)
    {
        char c = to_char(k);
        c = c - 'A' + 'a';
        return c;
    }

    const string Piece::to_string()
    {
        string ret;
        switch (getKind()) {
            case PAWN:
                ret = "Pawn";
                break;
            case KNIGHT:
                ret = "Knight";
                break;
            case BISHOP:
                ret = "Bishop";
                break;
            case ROOK:
                ret = "Rook";
                break;
            case QUEEN:
                ret = "Queen";
                break;
            case KING:
                ret = "King";
                break;
            default:
                ret = "undefined";
                break;
        }
        ret += "(" + Board::to_string(color_) + ")";
        if (square_)
            ret += "[" + square_->to_string() + "]";
        return ret;
    }

    void Piece::prettyPrint(ostringstream &oss)
    {
        if (getColor() == Side::BLACK)
            oss << Utils::RED;
        oss << this->to_char();
        if (getColor() == Side::BLACK)
            oss << Utils::RESET;
    }

    const char Piece::to_char()
    {
        return to_char(getKind());
    }

    const char Piece::to_pgn()
    {
        if (getKind() == PAWN)
            return '\0';
        else
            return to_char();
    }

    Piece::Piece(Kind kind, Side color) :
        kind_(kind), color_(color)
    {
    }

    Piece::Piece(Kind kind, Side color, Square *square) :
        kind_(kind), color_(color)
    {
        if (square)
            moveTo(square);
    }

    Piece::~Piece()
    {
        Utils::output("Deleting piece\n", 5);
    }

    const Side Piece::getColor()
    {
        return color_;
    }

    const Piece::Kind Piece::getKind()
    {
        return (promoted_ == KING)?kind_:promoted_;
    }

    //Move the piece to square, even if illegal move
    void Piece::moveTo(Square *newSquare)
    {
        if (square_ == newSquare)
            return;
        if (square_)
            square_->changePiece(NULL);
        square_ = newSquare;
        if (square_)
            square_->changePiece(this);
        else
            Utils::output(string("Dropped : ") + to_string() + "\n", 4);
            //deletedPieces.push_back(this);//drop
    }

    void Piece::promoteTo(Kind kind)
    {
        if (promoted_ == KING)
            promoted_ = kind;
        else
            Utils::handleError("Piece is already promoted");
    }

    void Piece::unpromote()
    {
        promoted_ = KING;
    }

    Side getSideFromString(string sidestr)
    {
        if (sidestr == "w" || sidestr == "white")
            return WHITE;
        else if (sidestr == "b" || sidestr == "black")
            return BLACK;
        else
            Utils::handleError("Error parsing playfor side");
        return WHITE;
    }

    bool checkMove(string mv)
    {
        if ( mv.size() < 4 || mv.size() > 5
                || mv[0] > 'h' || mv[0] < 'a'
                || mv[1] > '8' || mv[1] < '1'
                || mv[2] > 'h' || mv[2] < 'a'
                || mv[3] > '8' || mv[3] < '1')
            return false;
        else if (mv.size() == 5 && (
                    mv[4] != 'q' || mv[4] != 'n'
                    || mv[4] != 'b' || mv[4] != 'r'
                    ))
            return false;
        else
            return true;
    }

}



