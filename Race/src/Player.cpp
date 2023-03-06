#include "Player.h"

#include "Car.h"
#include "Controller.h"

#include "DynamicPointerArray.h"
#include "IObstacle.h"

#include "Coroutines/LedLightingCoroutine.h"

Player::Player(uint32_t carColor, byte controllerPin, int16_t lightingPin, char *name) : _car(new Car(carColor)),
                                                                                        _controller(new Controller(controllerPin)),
                                                                                        _controllerLED(nullptr),
                                                                                        _lightingPin(lightingPin),
                                                                                        _currentObstacle(0)
{

    setName(name);
    if (lightingPin > 0)
    {
        _controllerLED = new Adafruit_NeoPixel(MAXLEDPLAYER, (int16_t)lightingPin, NEO_GRB + NEO_KHZ800);
        _controllerLED->begin();
        _controllerLED->fill(carColor);
        _controllerLED->show();
    }
}

Player::~Player()
{
    //Pointers to car and controller are not destroyed automatically
    //to allow the lazy copy operator to work. This makes things much faster
    //in our current use case.
}

void Player::Destroy()
{
    //Player should be destroyed explicitly
    //In our case, this is only needed when loading data from EEPROM
    delete (_car);
    _car = nullptr;
    delete (_controller);
    _controller = nullptr;
    if (_controllerLED != nullptr)
    {
        delete (_controllerLED);
        _controllerLED = nullptr;
    }
}

void Player::Reset()
{
    _car->Reset();
    _controller->Reset();
    _currentObstacle = 0;
}

uint32_t Player::getColor() const
{
    return _car->getColor();
}

void Player::setName(char *name)
{
    if (strlen(name) >= MAX_NAME_LENGTH)
    {
        name[MAX_NAME_LENGTH - 1] = '\0';
    }
    strcpy(_name, name);
}

void Player::setColor(uint32_t color)
{
    _car->setColor(color);
    if( _controllerLED != nullptr )
    {
        _controllerLED->fill(color);
        _controllerLED->show();
    }
}

void Player::Update(DynamicPointerArray<IObstacle *> &obstacles)
{
    if (_controller->isPressed() && !_controller->alreadyPressed())
    {
        _car->accelerate(ACEL);
    }

    if (_currentObstacle < obstacles.Count())
    {
        obstacles[_currentObstacle]->Update(this);
        if (_car->getCurrentDistance() >= obstacles[_currentObstacle]->getEnd())
        {
            ++_currentObstacle;
        }
    }

    _car->Update();
    _controller->Update();

    if (_car->isStartingNewLoop())
    {
        _currentObstacle = 0;
    }
}

bool Player::operator<(const Player &other) const
{
    if (_car->getTotalDistance() == other._car->getTotalDistance())
    {
        return (_controller->getPin() < other._controller->getPin());
    }
    //We want the player with the greatest distance to be first
    return _car->getTotalDistance() > other._car->getTotalDistance();
}

bool Player::operator==(const Player &other) const
{
    return _car == other._car;
}

bool Player::operator!=(const Player &other) const
{
    return !(*this == other);
}