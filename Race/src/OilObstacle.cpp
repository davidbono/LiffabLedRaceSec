#include "OilObstacle.h"

#include <Adafruit_NeoPixel.h>

#include "Player.h"
#include "Controller.h"
#include "Car.h"
#include "Defines.h"

OilObstacle::OilObstacle(word start, word end, uint32_t color) : IObstacle(start, end, color),
                                                                 _pressDelay(OIL_PRESS_DELAY_DEFAULT)
{
    _type = IObstacle::ObstacleType::OBSTACLE_OIL;
}

void OilObstacle::Update(Player *player)
{
    float carPos = player->car().getCurrentDistance();
    if (carPos >= _start && carPos <= _end)
    {
        if (player->controller().isPressed() && !player->controller().alreadyPressed())
        {
            player->mutableCar().setSpeed(0);

            if ((millis() - player->controller().getPressedTime()) > _pressDelay)
            {
                player->mutableCar().increaseDistance(1);
            }
        }
    }
}

void OilObstacle::Draw(Adafruit_NeoPixel *led)
{
    for (word i = _start; i <= _end; ++i)
    {
        led->setPixelColor(i, _color);
    };
}