#ifndef LEDController_H
#define LEDController_H
#ifndef __INC_FASTSPI_LED2_H
    #include <FastLED.h>
#endif

enum AnimationTypes {
    ANIMATION_RAINBOW = 0,
    ANIMATION_RAINBOW_CYCLE,
    ANIMATION_RAINBOW_MIDDLE,
    ANIMATION_RAINBOW_MIDDLE_CYCLE,
    ANIMATION_SOLID_TRAIL,
    ANIMATION_SOLID_TRAIL_DOUBLE
};

struct AnimationSettings {
    uint8_t rainbowStep;
    uint8_t countTrailStep;
    uint8_t delayTrail;
};

struct PowerSettings {
    uint8_t volt;
    uint8_t miliamps;
};

struct LEDControllerSettings {
    uint8_t ledPIN;
    uint8_t brightness;
    uint32_t delay;
    AnimationSettings animationSettings;
    PowerSettings powerSettings;
};

template <int numberOfLEDs, int LEDPin>
class LEDController {
    private:
        LEDControllerSettings config;

        CRGB color;
        CHSV rainbowColor;

        CRGBArray<numberOfLEDs> leds;
        AnimationTypes selectedAnimation;

        LEDController(LEDControllerSettings config): config(config) {
            color = CRGB(255, 0, 0);
            FastLED.setMaxPowerInVoltsAndMilliamps(config.powerSettings.volt, config.powerSettings.miliamps);
            FastLED.setBrightness(config.brightness);
            FastLED.addLeds<WS2812B, LEDPin, GRB>(leds, numberOfLEDs);
        };

    public:
        static LEDController& getInstance(LEDControllerSettings config) {
            static LEDController instance(config);
            return instance;
        }

        void setBrightness(uint8_t brightness) {
            config.brightness = brightness;
            FastLED.setBrightness(config.brightness);
        }

        uint8_t getBrightness(uint8_t brightness) {
            return config.brightness;
        }

        void setColor(uint8_t r, uint8_t g, uint8_t b) {
            color = CRGB(r, g, b);
        }

        CRGB getColor() {
            return color;
        }

        void setRainbowColor(uint8_t hue) {
            rainbowColor = CHSV(hue, 255, 255);
        }

        CHSV getRainbowColor() {
            return rainbowColor;
        }

        void changeAnimation(AnimationTypes selectAnimation) {
            this->selectedAnimation = selectAnimation;
        }

        AnimationTypes getAnimation() {
            return selectedAnimation;
        }

        void update(uint8_t fillCount) {
            FastLED.clear();
            switch (selectedAnimation) {
                case ANIMATION_RAINBOW:
                    this->rainbowAnimation(fillCount);
                    break;
                case ANIMATION_RAINBOW_CYCLE:
                    this->rainbowCycleAnimation(fillCount);
                    break;
                case ANIMATION_RAINBOW_MIDDLE:
                    this->rainbowAnimationMiddle(fillCount);
                    break;
                case ANIMATION_RAINBOW_MIDDLE_CYCLE:
                    this->rainbowCycleAnimationMiddle(fillCount);
                    break;
                case ANIMATION_SOLID_TRAIL:
                    this->solidTrailAnimation(fillCount);
                    break;
                case ANIMATION_SOLID_TRAIL_DOUBLE:
                    this->solidTrailAnimationDouble(fillCount);
            }
            if (config.delay > 0) {
                FastLED.delay(config.delay);
            }
            FastLED.show();
        }

        ~LEDController() {}
    private:
        void rainbowAnimation(uint8_t fillCount) {
            if(fillCount == 0) {
                fill_solid(leds, numberOfLEDs, CRGB::Black);
                return;
            }
            fill_rainbow(leds, fillCount, rgb2hsv_approximate(color).hue, config.animationSettings.rainbowStep);
        }

        void rainbowCycleAnimation(uint8_t fillCount) {
            if(fillCount == 0) {
                fill_solid(leds, numberOfLEDs, CRGB::Black);
                return;
            }
            fill_rainbow(leds, fillCount, rainbowColor.hue, config.animationSettings.rainbowStep);
        }

        void rainbowAnimationMiddle(uint8_t fillCount) {
            if(fillCount == 0) {
                fill_solid(leds, numberOfLEDs, CRGB::Black);
                return;
            }
            if (numberOfLEDs % 2 == 0) {
                if (fillCount % 2 != 0) { fillCount++; }
                fill_rainbow(&leds[numberOfLEDs / 2 - 1], (fillCount / 2 + 1), rgb2hsv_approximate(color).hue, config.animationSettings.rainbowStep);

                for (int i = numberOfLEDs / 2 - 1; i >= 0; i--) {
                    if(i == 0) {
                        leds[i] = leds[numberOfLEDs - 1];
                        break;
                    }

                    leds[i] = leds[numberOfLEDs - i - 1];
                }
            }else {
                if (fillCount % 2 != 0) { fillCount++; }
                fill_rainbow(&leds[numberOfLEDs / 2 - 1], (fillCount / 2 + 1), rgb2hsv_approximate(color).hue, config.animationSettings.rainbowStep);

                for (int i = ceil(numberOfLEDs / 2 - 1); i >= 0; i--) {
                    if (i == 0) {
                        leds[i] = leds[numberOfLEDs - 1];
                        break;
                    }
                    leds[i] = leds[numberOfLEDs - i - 1];
                }
            }
        }

        void rainbowCycleAnimationMiddle(uint8_t fillCount) {
            if(fillCount == 0) {
                fill_solid(leds, numberOfLEDs, CRGB::Black);
                return;
            }
            if (numberOfLEDs % 2 == 0) {
                if (fillCount % 2 != 0) { fillCount++; }
                fill_rainbow(&leds[numberOfLEDs / 2 - 1], (fillCount / 2 + 1), rainbowColor.hue, config.animationSettings.rainbowStep);

                for (int i = numberOfLEDs / 2 - 1; i >= 0; i--) {
                    if(i == 0) {
                        leds[i] = leds[numberOfLEDs - 1];
                        break;
                    }

                    leds[i] = leds[numberOfLEDs - i - 1];
                }
            }else {
                if (fillCount % 2 != 0) { fillCount++; }
                fill_rainbow(&leds[numberOfLEDs / 2 - 1], (fillCount / 2 + 1), rainbowColor.hue, config.animationSettings.rainbowStep);

                for (int i = ceil(numberOfLEDs / 2 - 1); i >= 0; i--) {
                    if (i == 0) {
                        leds[i] = leds[numberOfLEDs - 1];
                        break;
                    }
                    leds[i] = leds[numberOfLEDs - i - 1];
                }
            }
        }

        void solidTrailAnimation(uint8_t fillCount) {
            static uint8_t currentStep = 0;
            static uint8_t currentPeak = 0;

            if (fillCount > currentPeak) {
                currentPeak = fillCount;
            }

            if(currentStep % config.animationSettings.countTrailStep == 0 && fillCount < currentPeak) {
                if(currentPeak > 0) { currentPeak--; }
                currentStep = 0;
            }
            fill_solid(leds, numberOfLEDs, CRGB::Black);
            fill_solid(leds, fillCount, color);

            for(int i = 0; i < 1; i++) {
                if (currentPeak >= 1 && currentPeak < numberOfLEDs) {
                    leds[currentPeak - i].setRGB(255, 255, 255);
                }
            }

            currentStep++;
            if (config.animationSettings.delayTrail > 0) {
                FastLED.delay(config.animationSettings.delayTrail);
            }
        }

        void solidTrailAnimationDouble(uint8_t fillCount) {
            static uint8_t currentStep = 0;
            static uint8_t currentPeak = 0;

            if (fillCount > currentPeak) {
                currentPeak = fillCount;
            }

            if(currentStep % config.animationSettings.countTrailStep == 0 && fillCount < currentPeak) {
                if(currentPeak > 0) { currentPeak--; }
                currentStep = 0;
            }
            fill_solid(leds, numberOfLEDs, CRGB::Black);
            fill_solid(leds, fillCount, color);

            for(int i = 0; i < 2; i++) {
                if (currentPeak >= 1 && currentPeak < numberOfLEDs) {
                    leds[currentPeak - i].setRGB(255, 255, 255);
                }
            }

            currentStep++;
            if (config.animationSettings.delayTrail > 0) {
                FastLED.delay(config.animationSettings.delayTrail);
            }
        }
};

#endif