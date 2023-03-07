#include "game.h"
#include <stdlib.h>

const int BIRD_X = 100;   // x-pos from the left of the screen
const int BIRD_SIZE = 30; // sidelength of a square bird

// barrier constants
const int PRERENDER_BUFFER_SIZE = 20; // prerender at most 20 barriers
const int MIN_BAR_OFFSET = 250;       // offset from the last barrier
const int MAX_BAR_OFFSET = 600;
const int MIN_BAR_GAP = 20; // percentage of screen height
const int MAX_BAR_GAP = 30;
const int MIN_BOT_H = 20; // percentage of screen height
const int MAX_BOT_H = 60;
const int BARRIER_WIDTH = 75;
#define BARRIER_SPEED 5

// physics constants
#define BIRD_CLIMB_RATE -16
#define BIRD_MAX_FALL_RATE 6
#define GRAVITY 1

void calculateBirdPosition(Game *game)
{
    int newY = game->birdYVal + game->birdYVelocity;
    if (newY < 0)
    {
        game->birdYVal = 0;
    }
    else if (newY + BIRD_SIZE > game->screenHeight)
    {
        game->birdYVal = game->screenHeight - BIRD_SIZE;
    }
    else
    {

        game->birdYVal = newY;
    }
}
void shiftQueue(Game *game)
{
    // set the X value of the 2nd element
    game->queue[1]->xVal = game->queue[0]->xVal + game->queue[1]->offsetFromLast + BARRIER_WIDTH;
    game->queue[1]->offsetFromLast = 0;

    // free head of queue
    free(*(game->queue));
    for (int i = 0; i < PRERENDER_BUFFER_SIZE - 1; i++)
    {
        game->queue[i] = game->queue[i + 1];
    }
}
// generate barrier and set game->queue[pos] to the pointer of this barrier
void generateBarrier(Game *game, int pos)
{
    int xOffset = rand() % (MAX_BAR_OFFSET - MIN_BAR_OFFSET + 1) + MIN_BAR_OFFSET;
    int gap = rand() % (MAX_BAR_GAP - MIN_BAR_GAP + 1) + MAX_BAR_GAP;
    gap = (float)gap / 100 * game->screenHeight;
    int botHeight = rand() % (MAX_BOT_H - MIN_BOT_H + 1) + MIN_BOT_H;
    botHeight = (float)botHeight / 100 * game->screenHeight;

    Barrier *barrier = malloc(sizeof(Barrier));
    barrier->gap = gap;
    barrier->botHeight = botHeight;
    barrier->xVal = 0;
    barrier->offsetFromLast = xOffset;

    game->queue[pos] = barrier;
}
// Returns 0 for no collision, 1 for collision
int detectCollision(Game *game)
{
    for (int i = 0; i < PRERENDER_BUFFER_SIZE; i++)
    {
        Barrier b = *(game->queue[i]);
        if (b.xVal + BARRIER_WIDTH < BIRD_X || BIRD_X + BIRD_SIZE < b.xVal)
        {
            continue;
        }

        // check for y-value
        if (game->birdYVal <= game->screenHeight - (b.botHeight + b.gap) || game->birdYVal + BIRD_SIZE >= game->screenHeight - b.botHeight)
        {
            // collision
            return 1;
        }
    }
    return 0;
}

void cycle(Game *game)
{
    int newVelocity = game->birdYVelocity + GRAVITY;
    game->birdYVelocity = newVelocity > BIRD_MAX_FALL_RATE ? BIRD_MAX_FALL_RATE : newVelocity; // apply gravity

    // calculate next bird position
    calculateBirdPosition(game);

    // shift barrier x value
    game->queue[0]->xVal -= BARRIER_SPEED;

    // dequeue and generate new barrier if barrier is offscreen
    if (game->queue[0]->xVal + BARRIER_WIDTH <= 0)
    {
        printf("run %d\n", game->queue[0]->xVal + BARRIER_WIDTH);
        shiftQueue(game);
        generateBarrier(game, PRERENDER_BUFFER_SIZE - 1);
    }
    if (detectCollision(game))
    {
        game->gameRunning = 0;
    }

    game->draw = 1;
}

Game *
createGame(int screenWidth, int screenHeight)
{
    Game *game = malloc(sizeof(Game));
    game->difficulty = 1;
    game->queue = malloc(sizeof(Barrier *) * PRERENDER_BUFFER_SIZE);
    game->birdYVal = (screenHeight - BIRD_SIZE) / 2;
    game->birdYVelocity = 0;
    game->draw = 0;
    game->gameRunning = 1;
    game->screenWidth = screenWidth;
    game->screenHeight = screenHeight;

    // generate initial barriers
    for (int i = 0; i < PRERENDER_BUFFER_SIZE; i++)
    {
        generateBarrier(game, i);
    }

    // first barrier is offset from the right side of the screen
    game->queue[0]->xVal = screenWidth;
    game->queue[0]->offsetFromLast = 0;
    return game;
}
void processInput(Game *game)
{
    game->birdYVelocity = BIRD_CLIMB_RATE;
}

void destroyGame(Game *game)
{
    for (int i = 0; i < PRERENDER_BUFFER_SIZE; i++)
    {
        free(game->queue[i]);
    }
    free(game->queue);
    free(game);
}