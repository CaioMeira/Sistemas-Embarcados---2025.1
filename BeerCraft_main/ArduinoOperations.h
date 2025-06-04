// === ArduinoOperations.h ===


// ==== ARQUIVO DE CALLBACKS PARA O PROJETO ==== //


#ifndef ARDUINO_OPERATIONS_H
#define ARDUINO_OPERATIONS_H

#include "C:\Users\CaioN\OneDrive\Documentos\Arduino\Simple Blink\simpleBlink_main\src-gen\Statechart.h"

class ArduinoOperations : public Statechart::OperationCallback {
public:
    void ESP_digitalWrite(sc_integer pin, sc_integer value) override {
        ::digitalWrite(pin, value);
    }

    void ESP_UartStart(sc_integer rate) override {
        ::Serial.begin(rate);;
    }

    void ESP_UartRead() override {
        ::
    }
    
/*
    void ESPpinMode(sc_integer pin, sc_integer mode) override {
        ::pinMode(pin, mode);
    }
*/
};

#endif // ARDUINO_OPERATIONS_H