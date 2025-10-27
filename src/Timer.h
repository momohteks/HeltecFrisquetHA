// Timer.h
#pragma once
#include <heltec.h>

class Timer {
public:
    explicit Timer(uint32_t interval) : _interval(interval) {}

    void start() { start(millis()); }
    void start(uint32_t startTick)
    {
        _lastTick = startTick;
        _started = true;
    }
    void stop()
    {
        _started = false;
        _lastTick = 0;
    }
    void restart() { start(); }

    // Renvoie true une fois par intervalle écoulé (style "ticker")
    // Catch-up: si plusieurs intervalles sont passés, renvoie true et avance d’un intervalle
    bool tick()
    {
        if (!_started || _interval == 0)
            return false;
        uint32_t now = millis();
        if ((uint32_t)(now - _lastTick) >= _interval)
        {
            // éviter la dérive : avancer par pas d'intervalle
            //_lastTick += _interval;

            // Si vraiment trop de retard (ex: sleep long) on peut aussi recoller à now :
             _lastTick = now;

            return true;
        }
        return false;
    }

    // Si tu veux savoir combien de ms se sont écoulées depuis le dernier tick/start
    uint32_t elapsed() const
    {
        if (!_started)
            return 0;
        return (uint32_t)(millis() - _lastTick);
    }

    // Temps restant avant le prochain tick (0 si déjà atteint)
    uint32_t remaining() const
    {
        if (!_started || _interval == 0)
            return 0;
        uint32_t e = elapsed();
        return (e >= _interval) ? 0 : (_interval - e);
    }

    void setInterval(uint32_t interval) { _interval = interval; }
    uint32_t interval() const { return _interval; }
    uint32_t lastTick() const { return _lastTick; }
    bool isStarted() const { return _started; }

private:
    uint32_t _interval = 0;
    uint32_t _lastTick = 0;
    bool _started = false;
};
