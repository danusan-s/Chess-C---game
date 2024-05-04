#include <iostream>
#include <string>

enum class Type
{
    None,
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

enum class Color
{
    None,
    Black,
    White
};

class Piece
{
private:
    Type type;
    Color color;

public:
    Piece(Type t = Type::None, Color c = Color::None) : type(t), color(c) {}

    Type getType() const
    {
        return type;
    }

    Color getColor() const
    {
        return color;
    }
};

class Chessboard
{
private:
    static const int SIZE = 8;
    Piece board[SIZE][SIZE];

    int blackKingRow;
    int blackKingCol;
    int whiteKingRow;
    int whiteKingCol;

public:
    Chessboard()
    {
        blackKingRow = 0;
        blackKingCol = 4;
        whiteKingRow = 7;
        whiteKingCol = 4;

        for (int i = 0; i < SIZE; ++i)
        {
            board[1][i] = Piece(Type::Pawn, Color::Black);
            board[6][i] = Piece(Type::Pawn, Color::White);
            for (int j = 2; j < 6; ++j)
            {
                board[j][i] = Piece();
            }
        }
        board[0][0] = Piece(Type::Rook, Color::Black);
        board[0][7] = Piece(Type::Rook, Color::Black);
        board[7][0] = Piece(Type::Rook, Color::White);
        board[7][7] = Piece(Type::Rook, Color::White);

        board[0][1] = Piece(Type::Knight, Color::Black);
        board[0][6] = Piece(Type::Knight, Color::Black);
        board[7][1] = Piece(Type::Knight, Color::White);
        board[7][6] = Piece(Type::Knight, Color::White);

        board[0][2] = Piece(Type::Bishop, Color::Black);
        board[0][5] = Piece(Type::Bishop, Color::Black);
        board[7][2] = Piece(Type::Bishop, Color::White);
        board[7][5] = Piece(Type::Bishop, Color::White);

        board[0][3] = Piece(Type::Queen, Color::Black);
        board[7][3] = Piece(Type::Queen, Color::White);

        board[0][4] = Piece(Type::King, Color::Black);
        board[7][4] = Piece(Type::King, Color::White);
    }
    void printBoard()
    {
        for (int i = 0; i < SIZE; ++i)
        {
            for (int j = 0; j < SIZE; ++j)
            {
                Piece piece = board[i][j];
                switch (piece.getType())
                {
                case Type::None:
                    std::cout << ".";
                    break;
                case Type::Pawn:
                    std::cout << (piece.getColor() == Color::White ? "♙" : "♟");
                    break;
                case Type::Knight:
                    std::cout << (piece.getColor() == Color::White ? "♘" : "♞");
                    break;
                case Type::Bishop:
                    std::cout << (piece.getColor() == Color::White ? "♗" : "♝");
                    break;
                case Type::Rook:
                    std::cout << (piece.getColor() == Color::White ? "♖" : "♜");
                    break;
                case Type::Queen:
                    std::cout << (piece.getColor() == Color::White ? "♕" : "♛");
                    break;
                case Type::King:
                    std::cout << (piece.getColor() == Color::White ? "♔" : "♚"); 
                    break;
                }
                std::cout << " ";
            }
            std::cout << std::endl;
        }
    }
    bool movePiece(const std::string &move,bool black)
    {
        // Validate input length
        if (move.length() != 4)
        {
            std::cout << "Invalid move format. Please use chess notation (e.g., 'e2e4')." << std::endl;
            return false;
        }

        // Extract source and destination squares
        int sourceRow = 8 - (move[1] - '0');
        int sourceCol = move[0] - 'a';
        int destRow = 8 - (move[3] - '0');
        int destCol = move[2] - 'a';

        // Check if source and destination squares are within the board
        if (sourceRow < 0 || sourceRow >= SIZE || sourceCol < 0 || sourceCol >= SIZE ||
            destRow < 0 || destRow >= SIZE || destCol < 0 || destCol >= SIZE)
        {
            std::cout << "Invalid move. Source or destination square is out of bounds." << std::endl;
            return false;
        }

        // Check if there is a piece at the source square
        if (board[sourceRow][sourceCol].getType() == Type::None)
        {
            std::cout << "Invalid move. There is no piece at the source square." << std::endl;
            return false;
        }

        if ((black && (board[sourceRow][sourceCol].getColor()== Color::White)) || (!black && (board[sourceRow][sourceCol].getColor()== Color::Black))){
            std::cout << "Invalid move. This piece is not yours." << std::endl;
            return false;
        }

        // Check if the move is valid for the selected piece (not implemented yet)
        // You need to implement this logic based on the rules of chess

        // If the move is valid, update the board

        if (isValidMove(sourceRow,sourceCol,destRow,destCol,black)){
            Piece temp = board[destRow][destCol];
            board[destRow][destCol]=board[sourceRow][sourceCol];
            board[sourceRow][sourceCol] = Piece();
            if (isKingInCheck(black)){
                board[sourceRow][sourceCol] = board[destRow][destCol];
                board[destRow][destCol]= temp;
                std::cout << "That move puts your king in check" << std::endl;
                return false;
            }
            if (board[destRow][destCol].getType()==Type::King){
                if (black){
                    blackKingRow=destRow;
                    blackKingCol=destCol;
                }
                else{
                    whiteKingRow=destRow;
                    whiteKingCol=destCol;
                }
            }
            if (isKingInCheck(!black)){
                std::cout << "Check" << std::endl;
                //check for checkmate, implementation later.
            }
            return true;
        }
        else{
            std::cout << "That move is invalid" << std::endl;
        }
        return false;
    }

    bool isValidMove(int sourceRow,int sourceCol,int destRow,int destCol,bool black){
        switch(board[sourceRow][sourceCol].getType()){
            case Type::Pawn:
                return (isValidPawnMove(sourceRow,sourceCol,destRow,destCol));
            case Type::Knight:
                return (isValidKnightMove(sourceRow,sourceCol,destRow,destCol));
            case Type::Bishop:
                return (isValidBishopMove(sourceRow,sourceCol,destRow,destCol));
            case Type::Rook:
                return (isValidRookMove(sourceRow,sourceCol,destRow,destCol));
            case Type::Queen:
                return (isValidQueenMove(sourceRow,sourceCol,destRow,destCol));
            case Type::King:
                return (isValidKingMove(sourceRow,sourceCol,destRow,destCol));
            default:
                break;
        }
        return false;
    }

    bool isKingInCheck(bool black){
        if (black){
            for (int i=0;i<SIZE;++i){
                for (int j=0;j<SIZE;++j){
                    if (isValidMove(i,j,blackKingRow,blackKingCol,!black)) return true;
                }
            }
        }
        else{
            for (int i=0;i<SIZE;++i){
                for (int j=0;j<SIZE;++j){
                    if (isValidMove(i,j,whiteKingRow,whiteKingCol,!black)) return true;
                }
            }
        }
        return false;
    }

    bool isValidPawnMove(int sourceRow,int sourceCol,int destRow,int destCol){
        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();
        if (destPiece.getColor()==playerColor) return false;
        int direction = (playerColor == Color::White) ? -1:1;
        if (sourceCol==destCol && destPiece.getType()==Type::None){
            if (destRow==sourceRow+direction) return true;
            if (destRow == sourceRow + 2 * direction && sourceRow == ((playerColor == Color::White) ? 6 : 1)) return true;
            return false;
        }
        if ((destCol==sourceCol+1 || destCol==sourceCol-1) && destRow==sourceRow+direction){
            if ((destPiece.getType()!=Type::None)) return true;
        }
        return false;
    }

    bool isValidKnightMove(int sourceRow,int sourceCol,int destRow,int destCol){
        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();
        if (destPiece.getColor()==playerColor) return false;
        if ((abs(destRow-sourceRow)==2 && abs(destCol-sourceCol)==1) || (abs(destRow-sourceRow)==1 && abs(destCol-sourceCol)==2)){
            return true;
        }
        return false;
    }

    bool isValidBishopMove(int sourceRow,int sourceCol,int destRow,int destCol){
        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();
        int rowDiff= destRow-sourceRow;
        int colDiff = destCol-sourceCol;

        if (destPiece.getColor()==playerColor) return false;

        if (abs(rowDiff)!=abs(colDiff)) return false;

        int stepRow = (rowDiff > 0) ? 1 : -1; // Determine step direction for row
        int stepCol = (colDiff > 0) ? 1 : -1; // Determine step direction for column
        int checkRow = sourceRow + stepRow; 
        int checkCol = sourceCol + stepCol;

        while (checkRow != destRow && checkCol != destCol) {
            if (board[checkRow][checkCol].getType() != Type::None) // Move is blocked by a piece
                return false; 
            checkRow += stepRow;
            checkCol += stepCol;
        }
        return true;
    }

    bool isValidRookMove(int sourceRow,int sourceCol,int destRow,int destCol){
        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();

        if (destPiece.getColor()==playerColor) return false;

        if (destCol==sourceCol){
            int direction = destRow>sourceRow ? 1:-1;
            int checkRow = sourceRow+direction;
            while (checkRow!=destRow){
                if (board[checkRow][sourceCol].getType()!=Type::None) return false;
                checkRow += direction;
            }
            return true;
        }
        if (destRow==sourceRow){
            int direction = destCol>sourceCol ? 1:-1;
            int checkCol = sourceCol+direction;
            while (checkCol!=destCol){
                if (board[sourceRow][checkCol].getType()!=Type::None) return false;
                checkCol += direction;
            }
            return true;
        }

        return false;
    }

    bool isValidQueenMove(int sourceRow,int sourceCol,int destRow,int destCol){
        return (isValidBishopMove(sourceRow,sourceCol,destRow,destCol) || isValidRookMove(sourceRow,sourceCol,destRow,destCol));
    }

    bool isValidKingMove(int sourceRow,int sourceCol,int destRow,int destCol){
        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();

        if (destPiece.getColor()==playerColor) return false;

        int rowDiff= destRow-sourceRow;
        int colDiff = destCol-sourceCol;

        if (abs(rowDiff)>1 || abs(colDiff)>1) return false;

        return true;
    }
};

int main()
{
    Chessboard game;
    game.printBoard();
    int move=0;
    bool black=false;
    while (true){
        std::string input;
        std::cout << "Enter your move:" << std::endl;
        std::cin >> input;
        if (input=="end") break;
        if (game.movePiece(input,black)){
            ++move;
            black=!black;
            game.printBoard();
        }
    }
}