extern const int PRERENDER_BUFFER_SIZE;
extern const int BARRIER_WIDTH;
extern const int BIRD_SIZE;
extern const int BIRD_X;
typedef struct
{
    int xVal;
    int gap;
    int offsetFromLast;
    int botHeight;
} Barrier;

typedef struct
{
    int difficulty;
    int birdYVal;
    int birdYVelocity;
    Barrier **queue;
    int draw;
    int screenWidth;
    int screenHeight;
    char gameRunning;

} Game;

void cycle(Game *game);
void processInput(Game *game);
Game *createGame(int screenWidth, int screenHeight);
void destroyGame(Game *game);
