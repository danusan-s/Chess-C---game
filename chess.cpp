#include <iostream>
#include <string>
#include <vector>

// Enum to classify the type of pieces
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

// Enum to classify the color of pieces
enum class Color
{
    None,
    Black,
    White
};

// Class to identify each piece
class Piece
{
private:
    Type type;
    Color color;
    bool hasMoved;

public:
    Piece(Type t = Type::None, Color c = Color::None) : type(t), color(c), hasMoved(false) {}

    // Returns the type of piece
    Type getType() const
    {
        return type;
    }

    // Returns the color of piece
    Color getColor() const
    {
        return color;
    }

    // Returns whether the piece has been moved before
    bool hasPieceMoved(){
        return hasMoved;
    }

    // Sets bool hasMoved to true
    void setMoved(){
        hasMoved=true;
    }
};

// Class to hold the main chessboard and run all operations
class Chessboard
{
private:
    static const int SIZE = 8;
    Piece board[SIZE][SIZE];

    // Variables to keep track of the king's postions
    int blackKingRow;
    int blackKingCol;
    int whiteKingRow;
    int whiteKingCol;

    // Stores previous move (useful for enpassant)
    std::string prevMove;

public:
    //Initializes the chess board
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

    // Prints out the current state of the board using unicode characters to represent pieces
    void printBoard()
    {
        for (int i = 0; i < SIZE; ++i)
        {
            std::cout << 8-i << " ";
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
        std::cout << "# a b c d e f g h"<<std::endl;
    }
    /**
     * @brief Designed to count the number of possibilites and stuff to compare with perft 
     *        but unforunately also counts duplicates so it doesnt work.
     * @param depth How many more moves to compute
     * @param black Indicates if it is black's turn
     * @param count No. of possibilites
     * @param checks No. of checks
     * @param captures No. of captures
     * @param checkmates No. of checkmates
     */
    void countPossibilites(int depth,bool black,int& count,int& checks,int& captures,int& checkmates){
        if (depth==0){
            return;
        }
        for (int i=0;i<SIZE;++i){
            for (int j=0;j<SIZE;++j){
                // Ignore pieces that we can't move in the current turn
                if (black && board[i][j].getColor()!=Color::Black) continue;
                if (!black && board[i][j].getColor()!=Color::White) continue;

                // Iterate through all the squares to find a destination
                for (int k=0;k<SIZE;++k){
                    for (int l=0;l<SIZE;++l){
                        
                        // Check if the piece can move to the destination
                        if (isValidMove(i,j,k,l)){
                            
                            // Update the board
                            Piece temp = board[k][l];
                            board[k][l] = board[i][j];
                            board[i][j] = Piece();
                            // If the king is moved, update king position
                            if (board[k][l].getType()==Type::King) updateKingPosn(k,l);

                            // Check if the move puts the king in check
                            if (isKingInCheck(black)){
                                board[i][j] = board[k][l]; 
                                board[k][l] = temp;
                                if (board[i][j].getType()==Type::King) updateKingPosn(i,j);
                                continue;
                            }
                            if (temp.getType()!=Type::None) captures++;
                            if (isKingInCheck(!black)){
                                if (isKingInCheckmate(!black)){
                                    checkmates++;
                                    board[i][j] = board[k][l]; 
                                    board[k][l] = temp;
                                    if (board[i][j].getType()==Type::King) updateKingPosn(i,j);
                                    continue;
                                }
                                checks++;
                            }
                            count++;
                            countPossibilites(depth-1,!black,count,checks,captures,checkmates);
                            // If king is not safe, revert and try next iteration.
                            board[i][j] = board[k][l]; 
                            board[k][l] = temp;
                            if (board[i][j].getType()==Type::King) updateKingPosn(i,j);
                        }
                    }
                }
            }
        }
    }

    std::string predictBestMove(bool black,int depth){
        return "";
    }

    /**
     * @brief Checks whether the user input is a valid move
     * 
     * @param move contains the user's input
     * @param black indicates if it is black's turn
     * @return true when the input is valid
     * @return false when the input is invalid
     */
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

        // Check if the piece at the source square is the correct colour based on turn
        if ((black && (board[sourceRow][sourceCol].getColor()== Color::White)) || (!black && (board[sourceRow][sourceCol].getColor()== Color::Black))){
            std::cout << "Invalid move. This piece is not yours." << std::endl;
            return false;
        }
        
        if (isValidMove(sourceRow,sourceCol,destRow,destCol)){

            // If the move is valid, update the board
            Piece temp = board[destRow][destCol];
            board[destRow][destCol]=board[sourceRow][sourceCol];
            board[sourceRow][sourceCol] = Piece();

            // If the king moved, update king position
            if (board[destRow][destCol].getType()==Type::King){
                updateKingPosn(destRow,destCol);  
            }

            // If the king is in check after the move, it is invalid and we should revert back
            if (isKingInCheck(black)){
                board[sourceRow][sourceCol] = board[destRow][destCol];
                board[destRow][destCol]= temp;
                if (board[sourceRow][sourceCol].getType()==Type::King){
                    updateKingPosn(sourceRow,sourceCol);  
                }
                std::cout << "That move puts your king in check" << std::endl;

                return false;
            }

            // If pawn reaches either end of ranks, it can promote to another piece
            if (board[destRow][destCol].getType()==Type::Pawn && (destRow==0||destRow==7)){
                std::cout<<"What would you like to promote to ? (Q,R,N,B)"<<std::endl;
                Color pawnColor = board[destRow][destCol].getColor();
                bool done = false;
                while (!done){
                    char input;
                    std::cin>>input;

                    // The user selects the piece they want to promote to;
                    switch(input){
                        case 'Q':
                            board[destRow][destCol]=Piece(Type::Queen,pawnColor);
                            done=true;
                            break;
                        case 'R':
                            board[destRow][destCol]=Piece(Type::Rook,pawnColor);
                            done=true;
                            break;
                        case 'N':
                            board[destRow][destCol]=Piece(Type::Knight,pawnColor);
                            done=true;
                            break;
                        case 'B':
                            board[destRow][destCol]=Piece(Type::Bishop,pawnColor);
                            done=true;
                            break;
                        default:
                            std::cout<<"Invalid Promotion"<<std::endl;
                    }
                }
            }

            // We just moved a piece to destRow, destCol so we set is as moved
            board[destRow][destCol].setMoved();

            // Store the move into prevMove
            prevMove=move;

            // The move is valid and didn't run into any obstructions
            return true;
        }
        else{
            std::cout << "That move is invalid" << std::endl;
        }
        return false;
    }

    /**
     * @brief Updates the private variables that store the king's position (note: does not move the king)
     * 
     * @param row New row position of the king
     * @param col New column position of the king
     */
    void updateKingPosn(int row,int col){

        // The king has to be in the position to be updated to retrieve the color
        // i.e. updateKingPosn after updating the board
        if (board[row][col].getColor()==Color::Black){
            blackKingRow=row;
            blackKingCol=col;
        }
        else{
            whiteKingRow=row;
            whiteKingCol=col;
        }
    }

    /**
     * @brief Determines whether a piece can reach from source to destination in one move
     * 
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the piece can reach the destination in one move
     * @return false if the piece cannot reach the destination in one move
     */
    bool isValidMove(int sourceRow,int sourceCol,int destRow,int destCol){

        // Can't move to the same square as source
        if (sourceRow==destRow && sourceCol==destCol) return false;

        // Handling move checking based on piece type
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

    /**
     * @brief Checks whether the king is in check
     * 
     * @param black Indicates whether to look at Black or White's king
     * @return true if the king is in check
     * @return false if the king is safe
     */
    bool isKingInCheck(bool black){

        // Iterate through all pieces and see if they can attack the king
        // Same color pieces will be returned false by isValidMove
        if (black){
            for (int i=0;i<SIZE;++i){
                for (int j=0;j<SIZE;++j){
                    if (board[i][j].getColor()!=Color::White) continue;
                    if (isValidMove(i,j,blackKingRow,blackKingCol)){
                        return true;
                    }
                }
            }
        }
        else{
            for (int i=0;i<SIZE;++i){
                for (int j=0;j<SIZE;++j){
                    if (board[i][j].getColor()!=Color::Black) continue;
                    if (isValidMove(i,j,whiteKingRow,whiteKingCol)){
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /**
     * @brief Checks whether the king is in checkmate
     * 
     * @param black Indicates whether to look at Black or White's king
     * @return true if the king is in checkmate
     * @return false if the king can be saved using a move
     */
    bool isKingInCheckmate(bool black){

        // Iterate through all the squares to find source
        for (int i=0;i<SIZE;++i){
            for (int j=0;j<SIZE;++j){

                // Ignore pieces that we can't move in the current turn
                if (black && board[i][j].getColor()!=Color::Black) continue;
                if (!black && board[i][j].getColor()!=Color::White) continue;

                // Iterate through all the squares to find a destination
                for (int k=0;k<SIZE;++k){
                    for (int l=0;l<SIZE;++l){
                        
                        // Check if the piece can move to the destination
                        if (isValidMove(i,j,k,l)){
                            
                            // Update the board
                            Piece temp = board[k][l];
                            board[k][l] = board[i][j];
                            board[i][j] = Piece();
                            // If the king is moved, update king position
                            if (board[k][l].getType()==Type::King) updateKingPosn(k,l);

                            // Check if the move makes the king safe
                            if (!isKingInCheck(black)){
                                // Revert position as we do not want the player to know the safe move
                                board[i][j] = board[k][l]; 
                                board[k][l] = temp;
                                if (board[i][j].getType()==Type::King) updateKingPosn(i,j);
                                return false;
                            }

                            // If king is not safe, revert and try next iteration.
                            board[i][j] = board[k][l]; 
                            board[k][l] = temp;
                            if (board[i][j].getType()==Type::King) updateKingPosn(i,j);
                        }
                    }
                }
            }
        }
        // None of the moves put the king out of check
        return true;
    }

    /**
     * @brief Checks whether the move is valid for a pawn piece (Includes double move, En passant)
     * 
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the pawn move is valid
     * @return false if the pawn move is invalid
     */
    bool isValidPawnMove(int sourceRow,int sourceCol,int destRow,int destCol){

        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();

        // Can't attack own piece
        if (destPiece.getColor()==playerColor) return false;

        int direction = (playerColor == Color::White) ? -1:1;

        // Same column movement (Cannot capture)
        if (sourceCol==destCol && destPiece.getType()==Type::None){
            
            // Single forward movement
            if (destRow==sourceRow+direction) return true;

            // Double movement allowed if it is the pawn's first move
            if (destRow == sourceRow + 2 * direction && !sourcePiece.hasPieceMoved()) return true;

            return false;
        }

        // Diagonal capture movement
        if ((destCol==sourceCol+1 || destCol==sourceCol-1) && destRow==sourceRow+direction){

            // If enemy piece in diagonal square, then we can capture
            if ((destPiece.getType()!=Type::None)) return true;

            // Special case: En passant
            if (board[sourceRow][destCol].getType()==Type::Pawn && board[sourceRow][destCol].getColor()!=playerColor){
                if(prevMove[1]==prevMove[3]-2*direction && prevMove[0]==prevMove[2] && prevMove[0]-'a'==destCol){
                    board[sourceRow][destCol]=Piece();
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * @brief Checks whether the move is valid for a knight piece
     * 
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the knight move is valid
     * @return false if the knight move is invalid
     */
    bool isValidKnightMove(int sourceRow,int sourceCol,int destRow,int destCol){

        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();

        // Can't attack it's own piece
        if (destPiece.getColor()==playerColor) return false;

        // L shaped move
        if ((abs(destRow-sourceRow)==2 && abs(destCol-sourceCol)==1) || (abs(destRow-sourceRow)==1 && abs(destCol-sourceCol)==2)){
            return true;
        }

        return false;
    }

    /**
     * @brief Checks whether the move is valid for a Bishop piece
     * 
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the bishop move is valid
     * @return false if the bishop move is invalid
     */
    bool isValidBishopMove(int sourceRow,int sourceCol,int destRow,int destCol){

        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();

        int rowDiff= destRow-sourceRow;
        int colDiff = destCol-sourceCol;

        // Can't attack own piece
        if (destPiece.getColor()==playerColor) return false;

        // Destination is not in the same diagonal as source
        if (abs(rowDiff)!=abs(colDiff)) return false;

        int stepRow = (rowDiff > 0) ? 1 : -1; // Determine step direction for row
        int stepCol = (colDiff > 0) ? 1 : -1; // Determine step direction for column
        int checkRow = sourceRow + stepRow; 
        int checkCol = sourceCol + stepCol;

        //Check for any obstructions in the diagonal
        while (checkRow != destRow && checkCol != destCol) {
            if (board[checkRow][checkCol].getType() != Type::None) // Move is blocked by a piece
                return false; 
            checkRow += stepRow;
            checkCol += stepCol;
        }
        return true;
    }

    /**
     * @brief Checks whether the move is valid for a rook piece
     * 
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the rook move is valid
     * @return false if the rook move is invalid 
     */
    bool isValidRookMove(int sourceRow,int sourceCol,int destRow,int destCol){

        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();

        // Can't attack own piece
        if (destPiece.getColor()==playerColor) return false;

        // Vertical movement, same column
        if (destCol==sourceCol){
            int direction = destRow>sourceRow ? 1:-1; // Determine direction of movement
            int checkRow = sourceRow+direction;

            // Check for obstructions
            while (checkRow!=destRow){
                if (board[checkRow][sourceCol].getType()!=Type::None) return false; // Move is blocked by a piece
                checkRow += direction;
            }
            return true;
        }

        // Horizontal movement, same row
        if (destRow==sourceRow){
            int direction = destCol>sourceCol ? 1:-1; // Determine direction of movement
            int checkCol = sourceCol+direction;

            // Check for obstructions
            while (checkCol!=destCol){
                if (board[sourceRow][checkCol].getType()!=Type::None) return false; // Move is blocked by a piece
                checkCol += direction;
            }
            return true;
        }

        // Movement is neither horizontal/vertical
        return false;
    }

    /**
     * @brief Checks whether the move is valid for a Queen piece (Combination of rook,bishop)
     * 
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the queen move is valid
     * @return false if the queen move is invalid
     */
    bool isValidQueenMove(int sourceRow,int sourceCol,int destRow,int destCol){
        // Check if the destination can be reached either by diagonal or straight move
        return (isValidBishopMove(sourceRow,sourceCol,destRow,destCol) || isValidRookMove(sourceRow,sourceCol,destRow,destCol));
    }

    /**
     * @brief Checks whether the move is valid for a King piece (Includes castling)
     * 
     * @param sourceRow Row of the source
     * @param sourceCol Column of the source
     * @param destRow Row of the destination
     * @param destCol Column of the destination
     * @return true if the king move is valid
     * @return false if the king move is invalid or castling under checks
     */
    bool isValidKingMove(int sourceRow,int sourceCol,int destRow,int destCol){

        Piece sourcePiece = board[sourceRow][sourceCol];
        Piece destPiece = board[destRow][destCol];
        Color playerColor = sourcePiece.getColor();

        // Can't attack own piece
        if (destPiece.getColor()==playerColor) return false;

        int rowDiff= destRow-sourceRow;
        int colDiff = destCol-sourceCol;

        //Castling time
        if (!sourcePiece.hasPieceMoved() && rowDiff==0){ //since piece hasnt moved,it should be in its own home row

            // King side castling
            if (destCol==2){

                // Squares between castle is blocked
                if (board[sourceRow][3].getType()!=Type::None || board[sourceRow][2].getType()!=Type::None || board[sourceRow][1].getType()!=Type::None) return false;
                
                // Can't castle while in check
                if (isKingInCheck(playerColor==Color::Black)) return false;

                // Can only castle if the king side rook is untouched
                if (board[sourceRow][0].getType()==Type::Rook && !board[sourceRow][0].hasPieceMoved()){

                    // Update one square towards castle
                    Piece rook = board[sourceRow][0];
                    board[sourceRow][3]= sourcePiece;
                    board[sourceRow][4]= Piece();
                    updateKingPosn(sourceRow,3);

                    // Check if king is in check, if yes revert and castling is not possible
                    if (isKingInCheck(playerColor==Color::Black)){
                        board[sourceRow][4]=sourcePiece;
                        board[sourceRow][3]=Piece();
                        updateKingPosn(sourceRow,4);
                        return false;
                    }

                    // Move to the castle square
                    board[sourceRow][2]= sourcePiece;
                    board[sourceRow][3]= Piece();
                    updateKingPosn(sourceRow,2);

                    // Check if king is in check, if yes revert and castling is not possible
                    if (isKingInCheck(playerColor==Color::Black)){
                        board[sourceRow][4]=sourcePiece;
                        board[sourceRow][2]=Piece();
                        updateKingPosn(sourceRow,4);
                        return false;
                    }

                    // This reverting part im not sure if we need, revert since it will castled by the move function anyways if valid
                    board[sourceRow][4]=sourcePiece;
                    board[sourceRow][2]=Piece();
                    updateKingPosn(sourceRow,4);

                    // Move the rook
                    rook.setMoved();
                    board[sourceRow][3]= rook;
                    board[sourceRow][0]= Piece();
                    return true;
                }
            }

            //Queen side castling
            else if (destCol==6){

                // Squares between castle is blocked
                if (board[sourceRow][5].getType()!=Type::None || board[sourceRow][6].getType()!=Type::None) return false;

                // Can't castle while in check
                if (isKingInCheck(playerColor==Color::Black)) return false;

                // Can only castle if the queen side rook is untouched
                if (board[sourceRow][7].getType()==Type::Rook && !board[sourceRow][7].hasPieceMoved()){

                    // Update one square towards castle
                    Piece rook = board[sourceRow][7];
                    board[sourceRow][5]= sourcePiece;
                    board[sourceRow][4]= Piece();
                    updateKingPosn(sourceRow,5);

                    // Check if king is in check, if yes revert and castling is not possible
                    if (isKingInCheck(playerColor==Color::Black)){
                        board[sourceRow][4]=sourcePiece;
                        board[sourceRow][5]=Piece();
                        updateKingPosn(sourceRow,4);
                        return false;
                    }

                    // Move to castle square
                    board[sourceRow][6]= sourcePiece;
                    board[sourceRow][5]= Piece();
                    updateKingPosn(sourceRow,6);

                    // Check if king is in check, if yes revert and castling is not possible
                    if (isKingInCheck(playerColor==Color::Black)){
                        board[sourceRow][4]=sourcePiece;
                        board[sourceRow][6]=Piece();
                        updateKingPosn(sourceRow,4);
                        return false;
                    }

                    // This reverting part im not sure if we need, revert since it will castled by the move function anyways if valid
                    board[sourceRow][4]=sourcePiece;
                    board[sourceRow][6]=Piece();
                    updateKingPosn(sourceRow,4);

                    // Move the rook
                    rook.setMoved();
                    board[sourceRow][5]= rook;
                    board[sourceRow][7]= Piece();
                    return true;
                }
            }
        }

        // If the move is not within 1 sqaure of the king, it is invalid
        if (abs(rowDiff)>1 || abs(colDiff)>1) return false;

        return true;
    }
};

/**
 * @brief Prints out the logs
 * 
 * @param moveLog Array containing the logs
 */
void printLogs(std::vector<std::string>& moveLog){
    std::cout<< "Move Log:" << std::endl;
    for (int i=0;i<moveLog.size();++i){
        std::cout<<moveLog[i]<<std::endl;
    }
}

int main()
{
    // Create the main board
    Chessboard game;

    // Print initial board
    game.printBoard();

    // Create an array to store moves
    std::vector<std::string> moveLog;

    // Boolean to keep track of moves
    bool black=false;

    int prev=0;
    int temp=0;

    for (int i=1;i<6;++i){
        int count=0;
        int checks=0;
        int captures=0;
        int checkmates=0;
        game.countPossibilites(i,black,count,checks,captures,checkmates);
        temp=count;
        count-=prev;
        prev=temp;
        std::cout <<"Depth: "<< i << " Possibilities: "<< count<<" Captures: "<<captures<<" Checks: " <<checks<<" Checkmates: "<<checkmates<<std::endl;
    }

    while (true){
        // Get input from user
        std::string input;
        std::cout << "Enter your move:" << std::endl;
        std::cin >> input;

        // If input is end, print out log and exit
        if (input=="end"){
            printLogs(moveLog);
            break;
        }

        // If valid move, add to log, print new board and change turns
        if (game.movePiece(input,black)){
            moveLog.push_back(input);
            black=!black;
            game.printBoard();

            // Check if king is in check
            if (game.isKingInCheck(black)){
                // If king is in checkmate, print out log and exit
                if(game.isKingInCheckmate(black)){
                    std::cout<<"Checkmate! "<<(black?"White":"Black")<<" wins!"<<std::endl;
                    std::cout<<"Do you wish to see the logs ? (y/n)"<<std::endl;
                    char choice;
                    std::cin >> choice;
                    if (choice=='y') printLogs(moveLog);
                    break;
                }
                std::cout << "Check" << std::endl;
            }
        }
    }
}