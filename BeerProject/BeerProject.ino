#include "sc_types.h"
#include "Statechart.h"
#include "StatechartCallback.h"


// Objetos Globais para o statechart
Statechart statechart;
StatechartCallback callback;

// Objeto Global do Display
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Filas (Na versão final eu nem uso a fila).
QueueHandle_t xSensorQueue; // Fila para leitura do Sensor

// Protótipo task
void sensorTask(void *parameter); // Protótipo para Task de Leitura do Sensor.


void setup() {
  // Vinculando callback ao statechart.
  statechart.setOperationCallback(&callback);

  // Cria Fila do Sensor (Na versão final eu nem uso a fila).
  xSensorQueue = xQueueCreate(1, 4); //1 vaga de float = 4 bytes.

  // Checa se a Fila foi criada
  if(xSensorQueue == NULL){
    Serial.println("[ERROR] Falha na criação da Fila.");
    while(1){} //Loop infinito em caso de erro.
  }

  //Cria a Task
  xTaskCreate(sensorTask, "sensorTask", 2048, NULL, 1, NULL);  

  //Entra na máquina de estados.
  statechart.enter();
}

void loop() {
  //statechart.triggerWithoutEvent();
  //delay(10);
  vTaskDelay(10 / portTICK_PERIOD_MS);
}


// [SENSOR TASK] -> Lê a temperatura do sensor e sobrescreve na 'Actual_Temperature' definida na seção dos Callbacks.
void sensorTask(void *pvParameters){
  (void)pvParameters;

  float TempValue = -1000.0;

  while (true) {
    // Solicita 4 bytes (float) do dispositivo
    Wire.requestFrom(I2C_SENSOR_ADDRESS, 4);
    
    // Caso a conexão esteja disponível
    if (Wire.available()) {

      byte i2c_data[4]; //cria buffer de 4 bytes (float) temporária.

      for (int i = 0; i < 4; i++) {
        i2c_data[i] = Wire.read();
      }

      // Converte os 4 bytes em float e anexa em 'TempValue'.
      memcpy(&TempValue, i2c_data, 4);

      callback.SendActualTemperature(TempValue); //Puxa função que altera o valor de ActualTemperature.

    } else {

      Serial.println("Erro: bytes insuficientes lidos do I2C");
    }

    vTaskDelay(1000 / portTICK_PERIOD_MS); // Espera 1s
  }
}
