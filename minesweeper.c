//flags:
typedef enum {false, true} bool;

bool DEBUG = false; //enable this to get debug messages, useful when developing, but please flip it back around when done

//This is free and unencumbered software released into the public domain.
//
//Anyone is free to copy, modify, publish, use, compile, sell, or
//distribute this software, either in source code form or as a compiled
//binary, for any purpose, commercial or non-commercial, and by any
//means.
//
//In jurisdictions that recognize copyright laws, the author or authors
//of this software dedicate any and all copyright interest in the
//software to the public domain. We make this dedication for the benefit
//of the public at large and to the detriment of our heirs and
//successors. We intend this dedication to be an overt act of
//relinquishment in perpetuity of all present and future rights to this
//software under copyright law.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
//EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
//MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
//IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
//OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
//ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
//OTHER DEALINGS IN THE SOFTWARE.
//
//You may or may not include me in the credits for any other release or fork,
//it's your choice but it would be nice if you did
//
//Made by KoNicks
//
char board[10][10];
//
//quick explanation:
//so let x be the number displayed on the tile if visible or 100 if the square is a mine
//inside the array, if the value for that square is equal to X that means the square is visible
//keep in mind X cannot possibly be bigger than 8
//if the value is 10 + X (so if the tens digit is one) it is a hidden tile, to display it we simply substract 10 from it and display that
//from this we can deduct that if at any given time there is a tile with a value bigger than 99 and smaller than 109, a mine has been exposed and the game ends (a hidden non-activated mine being 10 + X, so 110, the number may vary due to any incrementations caused by surrounding mines, but it's never gonna be bigger than 118)
//again, since X may never be bigger than 8, a value will never overflow from shown to hidden or in any other category since 8 is smaller than 10 and we look for the tens place
//in the same manner, 20 + X means the tile is hidden and has a flag on it
//and 30 + X means the tile is hidden and has a question flag on it
//this system is not very efficient and the win detection is going to be complicated af (comment from future me: it wasnt lmao)
//but it was my original idea and copying someone's method would destroy the fun really;
//also it allows for efficient use of memory and consistent tile rules
//it also allows us to make a zero-tile dig recursion with extreme ease (just check for places in the neighbouring squares where the value in the array is 10, '10' meaning hidden (10) + zero (0), so hidden zero)
#include <stdio.h>
#include <time.h>
void boardInit(){
	//fills board with zeros
	if(DEBUG) printf("Filling board with 0 (10)s\n"); 
	for(int i = 0; i < 10; i++)
		for(int j = 0; j < 10; j++)
			board[i][j] = 10;
	
	return;
}
bool drawBoard(char color){
	//ASCII color codes, this is how we make stuff colorful on unix terminals
	char reset[] = "\e[0m";
	char tile[] = "\e[47m\e[1;30m";
	char shown[] = "\e[40m\e[0;32m";
	char flag[] = "\e[47m\e[0;31m";
	char warn[] = "\e[47m\e[0;33m";

	if(!color){ //and this is how we rid it of all color, empty the chars containing the color codes to null bytes, so it goes monochrome, this is made so it can be played on windows terminals, as they dont support ASCII color codes
		if(DEBUG) printf("\nColorless mode %d\n", color);
		for(int i=0; i < sizeof(reset)/sizeof(char); i++) reset[i] = '\0';
		for(int i=0; i < sizeof(tile)/sizeof(char); i++) tile[i] = '\0';
		for(int i=0; i < sizeof(shown)/sizeof(char); i++) shown[i] = '\0';
		for(int i=0; i < sizeof(flag)/sizeof(char); i++) flag[i] = '\0';
		for(int i=0; i < sizeof(warn)/sizeof(char); i++) warn[i] = '\0';
	}
	printf("       0   1   2   3   4   5   6   7   8   9\n");
	printf("       |   |   |   |   |   |   |   |   |   |\n");
	printf("     +---+---+---+---+---+---+---+---+---+---+\n");
	char squaresDug = 0; //this is our win detection right here, if by the end of the board printing all but 10 squares are dug (the 10 being the mines) the game is won
	for(int i = 0; i < 10; i++){
		printf("%d -> ", i);
		for(int j = 0; j < 10; j++){
			if(board[i][j] % 100 / 10 == 1) //is a hidden square
				printf("| %s#%s ", tile, reset);
			
			if(board[i][j] % 100 / 10 == 0){ //is a shown square
				if(board[i][j] > 99){ //this is our loss detection, if it's a shown square (see line above) and it's bigger than 99 than it's a mine, a mined mine to be more precise, so a loss
					printf("\nGame Over!\n");
					return true;
				}else{
					if(board[i][j] != 0) printf("| %s%d%s ", shown, board[i][j], reset); //this is a shown square, we're gonna display it's value
					else printf("| %s %s ", shown, reset); //this is a shown zero, so we're gonna show it as an empty square
					squaresDug++; //and this is our win detection, if all 90 non-mine squares are shown (have been dug) we end the game, this just counts up each time it encounters a shown square
				}
			}
			if(board[i][j] % 100 / 10 == 2) //is a flagged square
				printf("| %s!%s ", flag, reset);

			if(board[i][j] % 100 / 10 == 3) //is a questioned(is this a word?) square
				printf("| %s?%s ", warn, reset);
		}
		printf("|\n     +---+---+---+---+---+---+---+---+---+---+\n");
	}
	if(squaresDug == 90){
		printf("\n\nCongrats! You won!\n");
		return true;
	}
	return false;
}
bool areEqual(char str1[], char str2[]){
	for(int i = 0; str1[i] == str2[i]; i++)
		if(str1[i] == '\0')
			return true;

	return false;
}
void recurseZero(char x, char y){
	for(char i = x - 1; i < x + 2; i++){
		for(char j = y - 1; j < y + 2; j++){
			if(i > -1 && i < 10 && j > -1 && j < 10){ //check if out of bounds
				if(board[i][j] == 10 && (i != x || j != y)) //check if tile is hidden zero (see comment chunk at the top) and if it isn't itself to prevent recursion
					recurseZero(i, j);

				while(board[i][j] % 100 / 10 != 0) //until tile is shown
					board[i][j] -= 10; //substract 10
				
				//this is for any non-zero tiles around the perimiter, they need to be cleared too
			}
		}
	}
	board[x][y] = 0;
	return;

}
void digTile(char ux, char uy){
	while(board[ux][uy] % 100 / 10 != 0) //see comment chunck at the top, if tens place isnt 0, make it, thats how you dig a tile
			board[ux][uy] -= 10;
	
	if (board[ux][uy] == 0) recurseZero(ux, uy); //collapse the zero cave if a zero is encountered
	return;
}
void flagTile(char ux, char uy){
	while(board[ux][uy] % 100 / 10 != 2){ //same principle as dig
			if(board[ux][uy] % 100 / 10 > 2) board[ux][uy] -= 10;
			else board[ux][uy] += 10;
	}
	return;
}
void warnTile(char ux, char uy){
	while(board[ux][uy] % 100 / 10 != 3){ //see above
			if(board[ux][uy] % 100 / 10 > 3) board[ux][uy] -= 10;
			else board[ux][uy] += 10;
	}
	return;
}

void command(){
	printf("\nSelect an action:\n[dxy -> dig square]\n[fxy -> flag square]\n[wxy -> put a question flag on the square]\n:: ");
	char arg[5];
	scanf("%s", arg);
	char ux, uy;
	//convert the chars into chars
	ux = arg[2] - '0';
	uy = arg[1] - '0';
	if(arg[0] == 'd') digTile(ux, uy);
	if(arg[0] == 'f') flagTile(ux, uy);
	if(arg[0] == 'w') warnTile(ux, uy);
	return;
}
bool plantMine(char x, char y){
	//this plants a mine at xy then increases the value of each surrounding square by 1, so we're basically reverse-solving the game
	if(DEBUG) printf("Planting mine at %d %d\n", x, y);
	if(board[x][y] > 99){ //if a mine is already there, try another spot
		srand(time(NULL) + rand());
		x = (char)(rand() % 10);
		y = (char)(rand() % 10);
		if(DEBUG) printf("Planting mine failed, retrying at %d %d\n", x, y);
		plantMine(x, y);
		return true;
	}
	for(char i = x - 1; i < x + 2; i++){
		for(char j = y - 1; j < y + 2; j++){
			if(i > -1 && i < 10 && j > -1 && j < 10)
				board[i][j]++; //for all surrounding squares, increase X by one, X being the numbers of mines in it's perimeter or 100 if mine
		}
	}
	board[x][y] = 110;
	return false;
}
int main(int argc, char *argv[]){
	srand(time(NULL)); //generate random integer
	boardInit();
	char x, y; //allocate 2 bytes for the x and y variable, these will be used when calling the mine-planting func
	bool color = 1;

	//get flags
	char option;
	while((option = getopt(argc, argv, ":c")) != -1){
		if(option == 'c') color = 0;
		if(DEBUG) printf("\nRegistered flag %c\n", option);
	}

	for(int i = 0; i < 10; i++){ //plant 10 mines at random
		x = (char)(rand() % 10);
		y = (char)(rand() % 10);
		plantMine(x, y);
	}
	bool game = false;
	while(1){
		game = drawBoard(color);
		if(game != 0) break;
		command();
	}
	return 0;
}
