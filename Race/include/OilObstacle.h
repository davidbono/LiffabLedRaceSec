#pragma once

#include <IObstacle.h>

class OilObstacle : public IObstacle
{
protected:
    word _pressDelay;

public:
    word getPressDelay() const { return _pressDelay; }
    void setPressDelay(word delay) { _pressDelay = delay; }
    OilObstacle(word start, word end, uint32_t color);
    void Update(Player *player) override;
    void Draw(Adafruit_NeoPixel *led) override;
};
