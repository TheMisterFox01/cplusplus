#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include <sstream>
using namespace sf;



class GameObject {

public:
    Vector2f position;

};

class RectCollider : public GameObject {
public:

    RectangleShape shape;
    Vector2f size;
    Vector2f GetNearPoint(Vector2f objectPosition) {
        Vector2f point;
        if (position.x + size.x >= objectPosition.x && position.x <= objectPosition.x) {
            point.x = objectPosition.x;
        }
        else if(position.x >= objectPosition.x){
            point.x = position.x;
        } else {
            point.x = position.x + size.x;
        }
        if (position.y + size.y >= objectPosition.y && position.y <= objectPosition.y) {
            point.y = objectPosition.y;
        }
        else if (position.y >= objectPosition.y) {
            point.y = position.y;
        }
        else {
            point.y = position.y + size.y;
        }
        return point;
    }

    void Change(Vector2f positionN, Vector2f sizeN) {
        position = positionN;
        size = sizeN;
        SetChanges();
    }

    void SetChanges() {
        shape.setPosition(position);
        shape.setSize(size);
    }

};

class Player : public RectCollider {

public:

    bool IsMoovingA = false;
    bool IsMoovingD = false;
    float speed = 5;
    int score = 0;

    void Move() {
        if (IsMoovingA) {
            position.x -= speed;
        }
        else if (IsMoovingD) {
            position.x += speed;
        }
        shape.setPosition(position);
    }

    void Borders(int boardersX) {
        if (position.x + speed + shape.getSize().x >= boardersX && IsMoovingD) {
            IsMoovingD = false;
        }
        else if (position.x - speed <= 0 && IsMoovingA) {
            IsMoovingA = false;
        }
    }

    void ChangeSpeed(float speedN) {

        speed = speedN;

    }
};


class Block : public RectCollider {

public:


    int hp = 1;
    bool IsAlive = true;
    bool IsBonus = false;
    bool IsImmortal = false;

    void Damaged() {
        if (!IsImmortal) {
            hp -= 1;
        }
        if (hp == 0) {
            IsAlive = false;
            shape.setSize(Vector2f(0, 0));
        }
    }

    void BonusBlock(bool bonus = false, bool immortal = false) {
        IsBonus = bonus;
        IsImmortal = immortal;
    }

};

class Ball : public GameObject {

public:

    CircleShape shape;
    
    Vector2f speed;


    void Move() {
        position.x += speed.x;
        position.y += speed.y;
        shape.setPosition(position);
    }

    void CollisionsPlayer(Player player) {

       
        if (Distance(player.GetNearPoint(position)).y <= 1 && Distance(player.GetNearPoint(position)).x <= 1) {
             speed.y *= -1;
        }
        

    }
    
    bool CollisionBlock(Block *block) {
       

        if (Distance(block->GetNearPoint(position)).y<= 1&& Distance(block->GetNearPoint(position)).x <= 1 && block->GetNearPoint(Vector2f(position.x+ shape.getRadius(),position.y+ shape.getRadius())).x == position.x+shape.getRadius()) {
            speed.y *= -1;
            block->Damaged();
            return true;
        }
        else if (Distance(block->GetNearPoint(position)).y  <= 1 && Distance(block->GetNearPoint(position)).x<= 1) {
            speed.x *= -1;
            block->Damaged();
            return true;
        }
        return false;
    }

    void Borders(int boardersX, int boardersY,Player *player) {
        if (position.x + speed.x + shape.getRadius() >= boardersX || position.x + speed.x + shape.getRadius() <= 0) {
            speed.x *= -1;
        }
        if (position.y + speed.y + shape.getRadius() >= boardersY) {
            position = Vector2f((float)boardersX / 2, (float)boardersY / 2);
            shape.setPosition(position);
            ChangeSpeed(Vector2f(4, -4));
            player->score -= 100;
        }
        else if(position.y + speed.y + shape.getRadius() <= 0 ){
            speed.y *= -1;
        }
    }

    void ChangeSpeed(Vector2f speedN) {

        speed = speedN;

    }

    void ChangeShape(Vector2f positionN, float radius) {
        position = positionN;
        shape.setPosition(position);
        shape.setRadius(radius);
    }

    Vector2f Distance(Vector2f point) {
        Vector2f vector;
        vector.x = abs(point.x - position.x);
        vector.y = abs(point.y - position.y);
        if (point.y > position.y) {
            vector.y -= shape.getRadius()*2;
        }
        if (point.x > position.x) {
            vector.x -= shape.getRadius()*2;
        }
        return vector;
    }

};



class Bonus : public GameObject {

public:
    CircleShape shape;

    void MoveDown() {

        position.y += 1;
        shape.setPosition(position);

    }


    void ChangeShape(Vector2f positionN,float radius) {
        position = positionN;
        shape.setRadius(radius);
        shape.setPosition(positionN);
    }

    bool CollisionsPlayer(Player player) {


        if (Distance(player.GetNearPoint(position)).y <= 1 && Distance(player.GetNearPoint(position)).x <= 1) {
            return true;
        }
        return false;

    }

    Vector2f Distance(Vector2f point) {
        Vector2f vector;
        vector.x = abs(point.x - position.x);
        vector.y = abs(point.y - position.y);
        if (point.y > position.y) {
            vector.y -= shape.getRadius() * 2;
        }
        if (point.x > position.x) {
            vector.x -= shape.getRadius() * 2;
        }
        return vector;
    }

};




void Game() {

    int screenX = 600;
    int screenY = 800;
    int activeBonus = 0;
    int activeBalls = 1;

    RenderWindow window(VideoMode(screenX, screenY), "Arcanoid");
    Clock clock;
    Block blocks[60];
    Text text;
    Bonus bonus[10];
    Ball ball[10];
    Player player;
    Font font;
    std::stringstream ss;

    if (!font.loadFromFile("Letters for Learners.ttf"))
    {
        // error...
    }
    text.setFont(font);
    text.setPosition(Vector2f(screenX-100, screenY-50));
    text.setCharacterSize(24);
    text.setFillColor(sf::Color::Red);

    player.Change(Vector2f((float)screenX / 2, 600), Vector2f(100, 10));

    ball[0].ChangeShape(Vector2f((float)screenX / 2, 300),10);
    ball[0].ChangeSpeed(Vector2f(4, 4));
    for (int j = 0; j < 6; j++) {
        for (int i = 0; i < 10; i++) {
            blocks[i+j*10].Change(Vector2f(((float)screenX / 10) * i,0+j*20), Vector2f((float)screenX / 10, 20));
            if (std::rand() % 8 == 1) {
                blocks[i+j*10].BonusBlock(true);
            }
            else if (std::rand() % 18 == 1) {
                blocks[i + j * 10].BonusBlock(false,true);
            }
        }
    }

    while (window.isOpen())
    {
        Event event;
        window.setKeyRepeatEnabled(false);
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            else if (event.type == Event::KeyPressed)
            {
                if (event.key.code == Keyboard::A)
                {
                    player.IsMoovingA = true;
                }
                else if (event.key.code == Keyboard::D)
                {
                    player.IsMoovingD = true;
                }
            }
            else if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::A)
                {
                    player.IsMoovingA = false;
                }
                else if (event.key.code == Keyboard::D)
                {
                    player.IsMoovingD = false;
                }
            }

        }

        Time timeElapsed = clock.getElapsedTime();
        if (timeElapsed.asSeconds() >= 0.01) {

            
            if (player.IsMoovingA || player.IsMoovingD) {
                player.Borders(screenX);
                player.Move();              
            }
            for (int j = 0; j < activeBalls; j++) {
                ball[j].CollisionsPlayer(player);
                for (int i = 0; i < 60; i++) {
                    if (blocks[i].IsAlive)
                        if (ball[j].CollisionBlock(&blocks[i])) {
                            if (!blocks[i].IsAlive)
                            {
                                player.score += 10;

                                if (blocks[i].IsBonus) {
                                    activeBonus++;
                                    bonus[activeBonus - 1].ChangeShape(blocks[i].position, 10);
                                }
                            }
                            break;
                        }
                }
                ball[j].Borders(screenX, screenY, &player);
                ball[j].Move();
            }
            for (int i = 0; i < activeBonus; i++) {
                if (bonus[i].CollisionsPlayer(player)) {
                    if (rand() % 2 == 1) {
                        player.Change(player.position, Vector2f(player.size.x + 10, player.size.y));
                    }
                    else {
                        activeBalls++;
                        ball[activeBalls-1].ChangeShape(Vector2f((float)screenX / 2, 300), 10);
                        ball[activeBalls-1].ChangeSpeed(Vector2f(4, 4));
                    }
                    activeBonus--;
                    break;
                }
                bonus[i].MoveDown();
            }
            clock.restart();
        }


        window.clear();
        for (int i = 0; i < activeBalls; i++) {
            window.draw(ball[i].shape);
        }
        window.draw(player.shape);
        for (int i = 0; i < 60; i++) {
            window.draw(blocks[i].shape);
        }
        for (int i = 0; i < activeBonus; i++) {
            window.draw(bonus[i].shape);
        }
        ss << "Score : " << player.score;
        text.setString(ss.str().c_str());
        ss.str("");
        window.draw(text);
        window.display();
    }
}


int main()
{
    Game();

    return 0;
}