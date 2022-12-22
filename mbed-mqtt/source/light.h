#include <stdio.h>

#include "PinNames.h"
#include "mbed.h"

#define PERIOD 0.01f

enum class light_state {
    EMERGENCY = 0,
    SERVER = 1,
    PROXIMITY = 2,
    OFF = 3,
};

class Light {
   public:
    int from_server[3] = {0, 0, 0};

    Light() {
        led_r = new PwmOut(PA_7);
        led_g = new PwmOut(PA_2);
        led_b = new PwmOut(PA_15);

        led_r->period(PERIOD);
        led_g->period(PERIOD);
        led_b->period(PERIOD);
    }

    void set_server_color(char *payload) {
        int r, g, b;
        sscanf(payload, "%d,%d,%d", &r, &g, &b);
        from_server[0] = r;
        from_server[1] = g;
        from_server[2] = b;
    }

    void set_color(light_state state) {
        switch (state) {
            case light_state::EMERGENCY:
                led_r->pulsewidth(PERIOD * (1 - RED[0] / 100.0f));
                led_g->pulsewidth(PERIOD * (1 - RED[1] / 100.0f));
                led_b->pulsewidth(PERIOD * (1 - RED[2] / 100.0f));
                break;
            case light_state::SERVER:
                led_r->pulsewidth(PERIOD * (1 - from_server[0] / 100.0f));
                led_g->pulsewidth(PERIOD * (1 - from_server[1] / 100.0f));
                led_b->pulsewidth(PERIOD * (1 - from_server[2] / 100.0f));
                break;
            case light_state::PROXIMITY:
                led_r->pulsewidth(PERIOD * (1 - WHITE[0] / 100.0f));
                led_g->pulsewidth(PERIOD * (1 - WHITE[1] / 100.0f));
                led_b->pulsewidth(PERIOD * (1 - WHITE[2] / 100.0f));
                break;
            case light_state::OFF:
                led_r->pulsewidth(PERIOD);
                led_g->pulsewidth(PERIOD);
                led_b->pulsewidth(PERIOD);
                break;
        }
    }

    bool server_is_on() {
        return from_server[0] != 0 || from_server[1] != 0 || from_server[2] != 0;
    }

   private:
    PwmOut *led_r;
    PwmOut *led_g;
    PwmOut *led_b;

    const int WHITE[3] = {50, 100, 100};
    const int RED[3] = {100, 0, 0};
};