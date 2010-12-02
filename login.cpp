/**
 * Main and only file for LOGIN.
 * LOGIN is a Game of Life, specifically NIkhil's Game Of Life
 *
 * LOGIN (C) 2007 Nikhil Marathe (http://22bits.exofire.net), GNU GPLv2
*/

#include<iostream>
#include<cmath>
#include<cstdlib>
#include<sstream>

#include<SDL/SDL.h>

class Login
{
    static const int WIDTH=160;
    static const int HEIGHT=160;
    static const int SIZE_OF_CELL=10;//each cell 6px

    
    bool cells[HEIGHT][WIDTH];
    int neighbours[HEIGHT][WIDTH];

    bool programRunning;
    bool gameRunning;
    int delay;

    SDL_Surface *screen;

    int aliveColor;//set in init()
    int deadColor;//set in init()

    int generationCount;
    std::ostringstream stream;
    
public:
    Login(int delay) : delay(delay) {  };
    void run() { init();programRunning=true;gameLoop();};
    
private:
    void init();
    void gameLoop();
    void updateGame();
    void countNeighbours(int, int);
    void updateCell(int, int);
    void toggleCell(int, int);
    void colourCell(int, int);
    void clear();
    void randomFill();
};

/**
 * toggleCell takes x and y coordinates and maps them to the cell coordinates and toggles the cell value.
 * Used to activate/deactivate cells by the user
*/
void Login::toggleCell(int x, int y)
{
    int row = x/SIZE_OF_CELL;
    int col = y/SIZE_OF_CELL;
    cells[row][col] = !cells[row][col];
    colourCell(row, col);
}

/**
 * Takes a cell position, translates it to screen coordinates and fills a rectangle of the cell size with alive or dead colour
*/
void Login::colourCell(int x, int y)
{
    int cellPixelX = x * SIZE_OF_CELL;
    int cellPixelY = y * SIZE_OF_CELL;
    int color = ( cells[x][y] ? aliveColor:deadColor);
    SDL_Rect r;
    r.x = cellPixelX;
    r.y = cellPixelY;
    r.w = r.h = SIZE_OF_CELL;
    SDL_FillRect(screen, &r, color);
}

/**
 * Clear the game. Set all cells to dead
*/
void Login::clear()
{
    //set all cells to dead
    for(int i = 0; i < HEIGHT; i++)
        for(int j = 0; j < WIDTH; j++)
            cells[i][j] = false;
    generationCount = 0;
}
/**
 * Initialises the SDL system
 * Clears the cells
 * fills the screen white
*/
void Login::init()
{
    if(SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout<<"Can;t initialize SDL"<<SDL_GetError()<<std::endl;
        exit(1);
    }
    atexit(SDL_Quit);

    screen = SDL_SetVideoMode(WIDTH*SIZE_OF_CELL, HEIGHT*SIZE_OF_CELL, 32, SDL_HWSURFACE|SDL_DOUBLEBUF);

    if (screen == NULL) {
        std::cout<<"Unable to set video mode: "<<SDL_GetError()<<std::endl;
        exit(1);
    }

    SDL_WM_SetCaption("Login", NULL);

    //fill the screen white
    SDL_Rect r;
    r.x = r.y = 0;
    r.w = screen->w;
    r.h = screen->h;
    SDL_FillRect(screen, &r, SDL_MapRGB(screen->format, 255, 255, 255));

    //set cell colours
    aliveColor = SDL_MapRGB(screen->format, 255, 0, 0);
    deadColor = SDL_MapRGB(screen->format, 255, 255, 255);
    clear();

}

/**
 * Begin running the game
*/
void Login::gameLoop()
{
    SDL_Event event;
    while(programRunning)
    {
        while(SDL_PollEvent(&event))
        {
            if (SDL_QUIT == event.type) programRunning = false;
            if (SDL_KEYDOWN == event.type && SDLK_ESCAPE == event.key.keysym.sym) programRunning = false;

            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                if(!gameRunning) // don't mess while game is running
                {
                    toggleCell(event.button.x, event.button.y);
                } 
            }

            if(event.type == SDL_KEYDOWN) {
                switch(event.key.keysym.sym) {
                    case SDLK_s:
                        gameRunning = !gameRunning;
                        break;
                    case SDLK_c:
                        if (!gameRunning) {
                            clear();
                            updateGame();
                        }
                        break;
                    case SDLK_r:
                        if(!gameRunning) {
                            clear();
                            randomFill();
                            updateGame();
                        }
                        break;
                    case SDLK_KP_MINUS:
                        delay -= (delay <= 10? 0 : 10);
                        std::cout<<delay<<std::endl;
                        break;
                    case SDLK_KP_PLUS:
                        delay += 10;
                        std::cout<<delay<<std::endl;
                        break;

                    default:break;
                }
            }
        }

        if(gameRunning)
            updateGame();
        SDL_Flip(screen);
        SDL_Delay(delay);
    }
}

/**
 * First counts the neighbours of each cell. Then updates each cell
*/
void Login::updateGame()
{
    ++generationCount;
    for(int row = 0; row < HEIGHT; ++row)
        for(int col = 0; col < WIDTH;++col)
            countNeighbours(row, col);
    for(int row = 0; row < HEIGHT; ++row)
        for(int col = 0; col < WIDTH;++col) {
            updateCell(row, col);
            colourCell(row, col);
        }
    std::ostringstream stream;
    stream << generationCount;
    SDL_WM_SetCaption(std::string("Login - Generation " + stream.str()).c_str(), NULL);
}

/**
 * Uses a step by step individual counting method.
 * The grid is considered continuous so values wrap
*/
void Login::countNeighbours(int row, int col)
{
    int count = 0;//current alive cells in the neighbourhood count
    //top left
    if(cells[(row == 0 ? HEIGHT-1 : row-1)][(col == 0 ? WIDTH-1 : col-1)])
        ++count;
    //top
    if(cells[(row == 0 ? HEIGHT-1 : row-1)][col])
        ++count;
    //top right
    if(cells[(row == 0 ? HEIGHT-1 : row-1)][(col == WIDTH-1 ? 0 : col+1)])
        ++count;
    //left
    if(cells[row][(col == 0 ? WIDTH-1 : col-1)])
        ++count;
    //right
    if(cells[row][(col == WIDTH-1 ? 0 : col+1)])
        ++count;
    //bottom-left
    if(cells[(row == HEIGHT-1 ? 0 : row+1)][(col == 0 ? WIDTH-1 : col-1)])
        ++count;
    //bottom
    if(cells[(row == HEIGHT-1 ? 0 : row+1)][col])
        ++count;
    //bottom right
    if(cells[(row == HEIGHT-1 ? 0 : row + 1)][(col == WIDTH-1 ? 0 : col + 1)])
        ++count;
    neighbours[row][col] = count;
}

/**
 * if a cell has 3 neighbours make it alive
 * if less or more kill it
*/
void Login::updateCell(int row, int col)
{

    if(neighbours[row][col] < 2 || neighbours[row][col] > 3) 
        cells[row][col] = false;
    else if(neighbours[row][col] == 3)
        cells[row][col] = true;
}

/**
 * Randomly fills the grid
*/
void Login::randomFill()
{
    for(int i = 0; i < WIDTH;++i)
        for(int j = 0; j < HEIGHT;++j)
            //std::cout<<"Random number:"<<(int)rand() % 2<<std::endl;
            cells[i][j] = (((int)rand() % 2) ? false:true);
}


int main(int argc, const char * argv[])
{
    int delay = 50;
    if(argc == 2)
        delay = atoi(argv[1]);

    (new Login(delay))->run();
    return 0;
}
