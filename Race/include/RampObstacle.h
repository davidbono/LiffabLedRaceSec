#pragma once

#include <IObstacle.h>

class Car;

class RampObstacle : public IObstacle
{
public:
    enum RampStyle
    {
        RAMP_HILL,
        RAMP_HOLE,
        RAMP_UP,
        RAMP_DOWN,
        RAMP_STYLE_END
    };

protected:
    RampStyle _style;
    byte _height;

public:
    RampObstacle(word start, word end, byte height, uint32_t color, RampStyle style);
    void Update(Player *player) override;
    void Draw(Adafruit_NeoPixel *led) override;
    byte getHeight() const { return _height; }
    void setHeight(byte height) { _height = height; }
    RampStyle getStyle() const { return _style; }
    void setStyle(RampStyle style) { _style = style; }

    static const char *getStyleName(RampStyle style) { return _StyleNames[style]; }

private:
    void DrawUp(word left, word right, Adafruit_NeoPixel *led);
    void DrawDown(word left, word right, Adafruit_NeoPixel *led);
    float MoveUp(word left, word right, float position);
    float MoveDown(word left, word right, float position);

    static uint32_t ColorMultiply(uint32_t color, float mul);
    static const char *_StyleNames[];
};
