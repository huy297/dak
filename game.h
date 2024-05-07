#ifndef _GAME__H
#define _GAME__H

#include <SDL.h>
#include<SDL_mixer.h>
#include <list>
#include <iostream>
#include<bits/stdc++.h>
#include "structs.h"
#include "graphics.h"
#include <cstdlib>
#include "texting.h"

void calcSlope(int x1, int y1, int x2, int y2, float *dx, float *dy);
void blitRect(SDL_Renderer *renderer, SDL_Texture *texture, SDL_Rect *src, int x, int y);

void setRect(SDL_Rect *rect, int x, int y, int w, int h)
{
    rect->x = x;
    rect->y = y;
    rect->w = w;
    rect->h = h;
}

bool inRect(SDL_Rect *rect, int x, int y)
{
    return x >= rect->x && y >= rect->y && x <= (rect->x+rect->w) && y <= (rect->y+rect->h);
}

struct Game {
    enum StateGame
    {
        Menu = 0, Playing = 1, GameOver = 2,
    };
    Entity *player;
    std::list<Entity*> fighters;
    std::list<Entity*> bullets;
    std::list<Debris*> debris;
    int gameState;

    int enemySpawnTimer;
    int stageResetTimer;
    int score;
    Star stars[MAX_STARS];

    int backgroundX;

    SDL_Texture *bulletTexture, *enemyTexture, *playerTexture, *alienBulletTexture, *backgroundTexture, *starTexture, *scoreTexture;
    SDL_Texture *exitTexture;
    SDL_Rect *endGame;
    TTF_Font* cFont = NULL;
    TTF_Font* arialFont = NULL;
    Mix_Chunk *pressChunk;

    Title *gameOver, *leaveGame, *Yes, *No;
    Title *gameName, *playGame, *exitGame;

    int upKey = SDL_SCANCODE_UP;
    int downKey = SDL_SCANCODE_DOWN;
    int leftKey = SDL_SCANCODE_LEFT;
    int rightKey = SDL_SCANCODE_RIGHT;
    int sKey = SDL_SCANCODE_S;

    void initPlayer(Graphics *graphics) {
        player = new Entity();
        player->x = 100;
        player->y = 100;
        player->dx = 0;
        player->dy = 0;
        player->health = 1;
        player->reload = 0;
        player->side = SIDE_PLAYER;
        player->texture = playerTexture;
        SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
        fighters.push_back(player);
    }

    void initStarfield() {
        for (int i = 0; i < MAX_STARS; i++) {
            stars[i].x = rand() % SCREEN_WIDTH;
            stars[i].y = rand() % SCREEN_HEIGHT;
            stars[i].speed = 1 + rand() % STAR_SPEED;
        }
    }

    void initTitle(Graphics *graphics){
        gameOver = new Title();
        gameOver->texture = graphics->loadTexture("assets/gameovernew.png");
        setRect(gameOver->rect,0,0,300,80);
        setRect(gameOver->dest,270,150,300,80);

        leaveGame = new Title();
        leaveGame->texture = graphics->loadTexture("assets/LeaveGame.png");
        setRect(leaveGame->rect,0,0,200,48);
        setRect(leaveGame->dest,320,250,200,48);

        Yes = new Title();
        Yes->texture = graphics->loadTexture("assets/yes.png");
        setRect(Yes->rect,0,0,90,30);
        setRect(Yes->dest,320,350,90,30);

        No = new Title();
        No->texture = graphics->loadTexture("assets/no.png");
        setRect(No->rect,0,0,90,30);
        setRect(No->dest,440,350,90,30);

        gameName = new Title();
        gameName->texture = graphics->loadTexture("assets/TopGun.png");
        setRect(gameName->rect,0,0,550,150);
        setRect(gameName->dest,140,100,550,150);

        playGame = new Title();
        playGame->texture = graphics->loadTexture("assets/play.png");
        setRect(playGame->rect,0,0,200,50);
        setRect(playGame->dest,330,300,200,50);

        exitGame = new Title();
        exitGame->texture = graphics->loadTexture("assets/exit.png");
        setRect(exitGame->rect,0,0,200,50);
        setRect(exitGame->dest,330,400,200,50);
    }

    void resetStage(Graphics *graphics) {
        fighters.clear();
        bullets.clear();
        debris.clear();

        initPlayer(graphics);
        initStarfield();

        enemySpawnTimer = 0;
        backgroundX = 0;
        score = 0;
        stageResetTimer = FPS * 3;
    }


    void init(Graphics *graphics) {
        srand(time(NULL));

        playerTexture = graphics->loadTexture("assets/tau1.png");
        bulletTexture = graphics->loadTexture("assets/bullet.png");
        enemyTexture = graphics->loadTexture("assets/enemyCraft.png");
        alienBulletTexture = graphics->loadTexture("assets/bulletEgg.png");
        backgroundTexture = graphics->loadTexture("assets/background.jpg");
        cFont = graphics->loadFont("assets/comicate.ttf",24);
        arialFont = graphics->loadFont("assets/arial.ttf",24);
        scoreTexture = graphics->loadText("Score: 0", cFont, setColor(1));
        initTitle(graphics);
        pressChunk = graphics->loadSound("assets/press.wav");
        score = 0;
        gameState = 0;
        resetStage(graphics);
    }



    void fireBullet(){
        Entity *bullet = new Entity();
        bullets.push_back(bullet);

        bullet->x = player->x;
        bullet->y = player->y;
        bullet->dx = BULLET_SPEED;
        bullet->health = 1;
        bullet->texture = bulletTexture;
        bullet->side = SIDE_PLAYER;
        SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

        bullet->y += (player->h / 2) - (bullet->h / 2);

        player->reload = PLAYER_RELOAD;
    }

    void doPlayer(int *keyboard){
        if (player == NULL) return;
        player->dx = player->dy = 0;

        if (player->reload > 0) {
            player->reload--;
        }

        if (keyboard[upKey]){
            player->dy = -PLAYER_SPEED;
        }
        if (keyboard[downKey]){
            player->dy = PLAYER_SPEED;
        }

        if (keyboard[leftKey]){
            player->dx = -PLAYER_SPEED;
        }

        if (keyboard[rightKey]){
            player->dx = PLAYER_SPEED;
        }

        if (keyboard[sKey] && player->reload == 0){
            fireBullet();
        }
    }

    void fireAlienBullet(Entity *enemy){
        Entity *bullet = new Entity();
        bullets.push_back(bullet);

        bullet->x = enemy->x;
        bullet->y = enemy->y;
        bullet->health = 1;
        bullet->texture = alienBulletTexture;
        bullet->side = SIDE_ALIEN;
        SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

        bullet->x += (enemy->w / 2) - (bullet->w / 2);
        bullet->y += (enemy->h / 2) - (bullet->h / 2);

        calcSlope(player->x + (player->w / 2), player->y + (player->h / 2), enemy->x, enemy->y, &bullet->dx, &bullet->dy);

        bullet->dx *= ALIEN_BULLET_SPEED;
        bullet->dy *= ALIEN_BULLET_SPEED;

        enemy->reload = (rand() % FPS * 3);
    }

    void doEnemies(){
        for (auto enemy : fighters){
            if (enemy != player && player != NULL && --enemy->reload <= 0){
                fireAlienBullet(enemy);
            }
        }
    }

    bool bulletHitFighter(Entity *bullet, Graphics* graphics){
        for (auto fighter : fighters){
            if (bullet->side != fighter->side && (bullet->collision(fighter) || fighter->collision(bullet))){
                Mix_Chunk *gJump = graphics->loadSound("assets/hit.wav");
                graphics->play(gJump);
                SDL_Delay(100);
                bullet->health = 0;
                fighter->health = 0;
                if (gJump != nullptr) Mix_FreeChunk( gJump);
                return true;
            }
        }
        return false;
    }

    bool bulletHitBullet(std::list<Entity*> bullets){
        bool collisionDetected = false;
        for (auto it = bullets.begin(); it != bullets.end(); ++it) {
            Entity* bulletA = *it;
            for (auto jt = std::next(it); jt != bullets.end(); ++jt) {
                     Entity* bulletB = *jt;

                if (bulletA->side != bulletB->side && (bulletA->collision(bulletB) || bulletB->collision(bulletA))){
                    bulletA->health = 0;
                    bulletB->health = 0;
                    collisionDetected = true;
                }
            }
        }
              return collisionDetected;
    }

     void doBullets(Graphics* graphics) {
        auto ins = bullets.begin();
        while (ins != bullets.end()) {
            auto temp = ins++;
            Entity *b = *temp;
            b->x += b->dx;
            b->y += b->dy;

            if (bulletHitFighter(b, graphics)) {
                delete b;
                score += (player->health > 0);
                bullets.erase(temp);
            }
            else if (b->x < -b->w || b->y < -b->h || b->x > SCREEN_WIDTH || b->y > SCREEN_HEIGHT){
                delete b;
                bullets.erase(temp);
            }
        }

            if (bulletHitBullet(bullets)) {
                 bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
                [](Entity* b) { return b->health == 0; }), bullets.end());
            }
    }

    void doFighters() {
        auto ins = fighters.begin();

        while (ins != fighters.end()) {
            auto temp = ins++;
            Entity *b = *temp;
            b->x += b->dx;
            b->y += b->dy;
            if (b != player && b->x < -b->w){
                b->health = 0;
            }
            if (b->health == 0){
                if (b == player){
                    player = NULL;
                }
                addDebris(b);
                delete b;
                fighters.erase(temp);
            }
        }
    }

    void spawnEnemies() {
        Entity *enemy = new Entity();
        if (--enemySpawnTimer <= 0) {
            enemy->x = SCREEN_WIDTH;
            enemy->y = rand() % SCREEN_HEIGHT;
            enemy->dy = 0;
            enemy->health = 1;
            enemy->texture = enemyTexture;
            enemy->dx = -(2 + rand() % 3);
            enemy->side = SIDE_ALIEN;
            enemySpawnTimer = 30 + rand() % 60;
            SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);
            fighters.push_back(enemy);
        }
    }

    void clipPlayer(){
        if (player == NULL) return;
        if (player->x < 0){
            player->x = 0;
        }

        if (player->y < 0){
            player->y = 0;
        }

        if (player->x > SCREEN_WIDTH / 2){
            player->x = SCREEN_WIDTH / 2;
        }

        if (player->y > SCREEN_HEIGHT - player->h){
            player->y = SCREEN_HEIGHT - player->h;
        }
    }

    void doStarFields() {
        for (int i = 0; i < MAX_STARS; i++){
            stars[i].x -= stars[i].speed;

            if (stars[i].x < 0){
                stars[i].x = SCREEN_WIDTH + stars[i].x;
            }
        }
    }

    void doDebris() {
        auto ins = debris.begin();
        while (ins != debris.end()){
            auto temp = ins++;
            Debris *b = *temp;

            b->x += b->dx;
            b->y += b->dy;

            b->dy += 0.5;

            if (--b->life <= 0){
                delete b;
                debris.erase(temp);
            }
        }
    }

    void addDebris(Entity *enemy) {
        int x, y, w, h;

        w = enemy->w / 2;
        h = enemy->h / 2;

        for (y = 0; y <= h; y += h){
            for (x = 0; x <= w; x += w){
                Debris *d = new Debris();
                debris.push_back(d);

                d->x = enemy->x + enemy->w / 2;
                d->y = enemy->y + enemy->h / 2;
                d->dx = (rand() % 5) - (rand() % 5);
                d->dy = -(5 + rand() % 12);
                d->life = FPS ;
                d->texture = enemy->texture;

                d->rect.x = x;
                d->rect.y = y;
                d->rect.w = w;
                d->rect.h = h;
            }
        }
    }

    void logic(Graphics *graphics, int *keyboard, int mouseX, int mouseY) {
        if (gameState == Menu)
        {
            if (inRect(playGame->dest,mouseX,mouseY))
            {
                graphics->play(pressChunk);
                resetStage(graphics);
                gameState = Playing;
            }
            else if (inRect(exitGame->dest,mouseX,mouseY))
            {
                graphics->play(pressChunk);
                exit(0);
            }
        }
        else if (gameState == Playing)
        {

            doStarFields();
            doPlayer(keyboard);
            doFighters();
            doEnemies();
            doBullets(graphics);
            spawnEnemies();
            doDebris();
            clipPlayer();
            if (player == NULL && --stageResetTimer <= 0){
                gameState = GameOver;
            }
        }
        else if (gameState == GameOver)
        {
            if (inRect(No->dest,mouseX,mouseY))
            {
                graphics->play(pressChunk);
                gameState = Menu;
            }
            else if (inRect(Yes->dest,mouseX,mouseY))
            {
                graphics->play(pressChunk);
                exit(0);
            }
        }

    }


    void drawFighters(Graphics *graphics) {
        for (auto fighter : fighters){
            graphics->renderTexture(fighter->texture, fighter->x, fighter->y);
        }
    }

    void drawBullets(Graphics *graphics) {
        for (auto bullet : bullets){
            graphics->renderTexture(bullet->texture, bullet->x, bullet->y);
        }
    }

    void drawBackground(Graphics *graphics) {
        SDL_Rect dest;
        int x;

        for (x = backgroundX; x < SCREEN_WIDTH; x += SCREEN_WIDTH){
            dest.x = x;
            dest.y = 0;
            dest.w = SCREEN_WIDTH;
            dest.h = SCREEN_HEIGHT;
            SDL_RenderCopy(graphics->renderer, backgroundTexture, NULL, &dest);
        }
    }

    void drawStarfield(Graphics *graphics) {
        for (int i = 0; i < MAX_STARS; i++) {
            int c = 32 * stars[i].speed;
            SDL_SetRenderDrawColor(graphics->renderer, c, c, c, 255);
            SDL_RenderDrawLine(graphics->renderer, stars[i].x, stars[i].y, stars[i].x + 3, stars[i].y);
        }
    }

    void drawDebris(Graphics *graphics) {
        for (auto d : debris) {
            blitRect(graphics->renderer, d->texture, &d->rect, d->x, d->y);
        }
    }

    void drawScore(Graphics *graphics) {
        std::string newText = "Score: " + std::to_string(score);
        graphics->updateText(scoreTexture,newText.c_str(),cFont,setColor(1));
        graphics->renderText(scoreTexture,350,0,50,50);
    }

    void drawMenu(Graphics *graphics) {
        blitRect(graphics->renderer,gameName->texture,gameName->rect,140,100);
        blitRect(graphics->renderer,playGame->texture,playGame->rect,330,300);
        blitRect(graphics->renderer,exitGame->texture,exitGame->rect,330,400);
    }

    void drawGameOver(Graphics *graphics) {
        blitRect(graphics->renderer,gameOver->texture,gameOver->rect,270,150);
        blitRect(graphics->renderer,leaveGame->texture,leaveGame->rect,320,250);
        blitRect(graphics->renderer,Yes->texture,Yes->rect,320,350);
        blitRect(graphics->renderer,No->texture,No->rect,440,350);
    }

    void draw(Graphics *graphics) {
        drawBackground(graphics);
        if (gameState == Playing)
        {
            drawStarfield(graphics);
            drawFighters(graphics);
            drawDebris(graphics);
            drawBullets(graphics);
            drawScore(graphics);
        }
        else if (gameState == GameOver)
        {
            drawGameOver(graphics);
            drawScore(graphics);
        }
        else if (gameState == Menu)
        {
            drawMenu(graphics);
        }
    }


};

#endif
