#ifndef STATECHARTCALLBACK_H
#define STATECHARTCALLBACK_H

// Itemis Create File
#include "Statechart.h"
#include "sc_types.h"

// Arduino IDE
#include <Arduino.h>

// ESP32 Time Interrupt para o LEDC
//#include <ESP32TimerInterrupt.h>

// Keypad
#include <Keypad.h>

// Display (Display OLED 0.96'' ; 128x64 ; i2C ; Azul-Amarelo)
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// FreeRTOS
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

// Display Definitions
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Endereços i2c
#define I2C_DISPLAY_ADDRESS 0x3C
#define I2C_SENSOR_ADDRESS 0x08

// PWM
#define PWM_CHANNEL     0       // Canal LEDC (0 a 15)
#define PWM_FREQUENCY   1000    // 1 kHz
#define PWM_RESOLUTION  8       // 8 bits (0-255)
#define PWM_PIN         2       // Pino D23 (pode usar outros GPIOs também)

// Keypad Definitions
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}};
  byte rowPins[ROWS] = {18, 14, 27, 33};      
  byte colsPins[COLS] = {32, 19, 26};         

// Declara que existe um objeto chamado statechart em outro arquivo
extern Statechart statechart;

// Declara que existe um display chamado 'display' em outro arquivo
extern Adafruit_SSD1306 display;

// Aqui para baixo serão desenvolvidos os Callbacks das funções do Itemis.
class StatechartCallback : public Statechart::OperationCallback {
public:

    //variaveis globais de processo
    int temperature1;
    int temperature2;
    int temperature3;
    unsigned long duration1;
    unsigned long duration2;
    unsigned long duration3;

    //Variavel global para sensor de temperatura
    float actual_temperature = 0.0;

    //Variáveis globais para o teclado.
    Keypad *keypad = nullptr;
    String keypadBuffer = "";


    // ============= FUNÇÕES DE INICIALIZAÇÃO DOS COMPONENTES ========================

    // [UART] -> Função que inicia a UART a partir de um dado Baudrate.
    void ESP_initUart(sc::integer rate) override {

      Serial.begin(rate);
      delay(1000); //Para estabilizar
      Serial.println("[UART] Uart Iniciado");
      Serial.print("[UART] Baudrate: ");
      Serial.println(rate);
      Serial.println("[UART] Digite um comando para teste: ");
      std::string received = ""; //cria um buffer vazio que receberá o digito.
      while(true){
          if(Serial.available()){
              char c = Serial.read();
              if (c == '\n') {  //se aparecer um enter (pula linha), ele sai do laço.
                  break;
              }
            
              if (c != '\r') {
                  received += c; //caso tenha um digito, acrescenta ao buffer 'received'.
              }
          }
          delay(10);
      }
      Serial.print("[UART] ");
      Serial.print("Mensagem Digitada: ");
      Serial.println(received.c_str()); //garante o formato string com o método '.c_str()'
      Serial.println("[UART] Iniciado com Sucesso! ");
    }

    // [BUZZER] -> função que inicia e testa o Buzzer. Recebe o pino do mesmo como parâmetro direto do Itemis Create.
    void ESP_buzzerInit(sc::integer pin) override {
      // Inicialização
      Serial.print("[BUZZER]");
      Serial.println("Inicializando Buzzer...");
      pinMode(pin, OUTPUT);                         // Aloca o Pino.
      digitalWrite(pin, HIGH);                      // Define como HIGH para desligar (buzzer ativo)

      // Etapa de Testes
      Serial.print("[BUZZER]");
      Serial.println("Testando Buzzer...");
      digitalWrite(pin, LOW);                       // Em seguida é posto em LOW para apitar e testar o mesmo.
      delay(2000);
      digitalWrite(pin, HIGH);                      // High para desliga-lo.

      // Conclusão
      Serial.print("[BUZZER]");
      Serial.println("Teste Completo");
      Serial.print("[BUZZER]");
      Serial.println("Buzzer Iniciado");
    }

    // [I2C] -> Função que inicia o I2C, e testa para ver se encontra algo em algum endereço. Recebe os pinos SDA e SCL como parâmetro do Itemis.
    void ESP_i2CInit(sc::integer SDA_pin, sc::integer SCL_pin) override {
      Serial.print("[i2C] ");
      Serial.println("Inicializando i2C...");

      Wire.begin(SDA_pin, SCL_pin);                     // Inicia o Wire com os devidos pinos.
      Wire.setClock(100000);                            // Define Clock em 100kHz

      Serial.print("[i2C] ");
      Serial.println("Clock definido em 100 kHz");


      Serial.print("[i2C] ");
      Serial.println("Inicializado com Sucesso!");

      // [I2C SCAN] -> parte que faz o scan para tentar achar um endereço válido.
      Serial.println("[i2C] Escaneando endereço I2C...");
      byte error, address;                                // cria as variáveis error e address.
      for (address = 1; address < 127; address++) {       // varia address em todas as possibilidades.
        Wire.beginTransmission(address);                  // Tenta começar uma transmissão no address.
        error = Wire.endTransmission();                   // Checa se há erro.
        if (error == 0) {                                 // Se não houver erro entra e printa o endereço.
          Serial.print("[i2C] Dispositivo I2C encontrado no endereço 0x");
          Serial.println(address, HEX);
        }
      }
      Serial.print("[i2C] ");
      Serial.println("Scanner finalizado.");
    }
    
    // [DISPLAY] -> Função que inicia o Display e define alguns parâmetros.
    void ESP_displayInit() override{

      Serial.print("[DISPLAY] ");
      Serial.println("Inicializando Display...");
      
      if(!display.begin(SSD1306_SWITCHCAPVCC, I2C_DISPLAY_ADDRESS)){
        Serial.println("[DISPLAY] Erro ao iniciar o Display! ");
        while(true); //Loop infinito para travar a execução.
      }

      display.clearDisplay(); //Limpa a tela
      display.setTextColor(SSD1306_WHITE); //Define cor do texto para BRANCO
      display.setTextSize(1); //Define tamanho do texto para 1
      display.setCursor(0,0); //Coloca o ponto de referência no ponto inicial
      display.println("Iniciado com sucesso. "); //Manda mensagem para a tela.
      display.display(); //Comando para atualizar o Display
      Serial.println("[DISPLAY] Display iniciado com Sucesso!");
      delay(5000);
      
    }

    // [MIXER] -> Inicialização e testes do Mixer (que no protótipo está representado por um LED Verde)
    void ESP_mixerInit(sc::integer pin) override {
      // Inicialização
      Serial.print("[MIXER] ");
      Serial.println("Inicializando Mixer...");
      pinMode(pin, OUTPUT);

      // Etapa de Testes
      Serial.print("[MIXER] ");
      Serial.println("Testando Mixer...");
      digitalWrite(pin, HIGH);
      delay(1000);
      digitalWrite(pin, LOW);
      delay(1000);
      digitalWrite(pin, HIGH);
      delay(1000);
      digitalWrite(pin, LOW);
      delay(1000);
      digitalWrite(pin, HIGH);
      delay(1000);
      digitalWrite(pin, LOW);
      delay(1000);

      // Conclusão
      Serial.print("[MIXER]");
      Serial.println("Teste Completo");
      Serial.print("[MIXER]");
      Serial.println("Mixer Iniciado");
    }


    // [KEYPAD] -> Inicialização do Keypad e definição dos parâmetros.
    void ESP_keypadInit() override{
      std::string initBuffer = "";        //buffer vazio para receber a tecla pressionada.
      Serial.print("[KEYPAD] ");
      Serial.println("Iniciando Teclado");

      // makeKeymap faz o mapeamento baseado nos pinos referentes as colunas e linhas
      // Observar 'Keypad Definitions' na parte superior deste arquivo.
      if(keypad == nullptr){
        keypad = new Keypad(makeKeymap(keys), rowPins, colsPins, ROWS, COLS); 
      }

      // Etapa de Testes
      Serial.print("[KEYPAD] ");
      Serial.println("Testando Teclado...");
      Serial.print("[KEYPAD] ");
      Serial.println("Digite uma tecla para continuar...");
      
      char c = waitForKey();  // waitForKey() é uma função implementada mais para baixo neste arquivo.
                              // basicamente espera por uma tecla pressionada e retorna ela na função.
      Serial.print("[KEYPAD] Tecla pressionada: ");
      Serial.println(c);

      Serial.print("[KEYPAD] ");
      Serial.println("Teclado iniciado com Sucesso!");
    } 

    // [HEATER] -> Inicialização do Aquecedor por PWM.
    void ESP_heaterInit() override{
      //PWM_CHANNEL     0       // Canal LEDC (0 a 15)
      //PWM_FREQUENCY   1000    // 1 kHz
      //PWM_RESOLUTION  8       // 8 bits (0-255)
      //PWM_PIN         2       // Pino D2 (pode usar outros GPIOs também)

      //Inicializa o Canal
      ledcSetup(PWM_CHANNEL, PWM_FREQUENCY, PWM_RESOLUTION);  // Definições de Canal com frequência e Resolução

      //Aloca o canal em um pino
      ledcAttachPin(PWM_PIN, PWM_CHANNEL);                    // Liga canal ao pino

      //Seta o valor para zero (desligado)
      ledcWrite(PWM_CHANNEL, 0); // Duty cycle de 50% (128/255)
    }

    // [CONFIGURAÇÃO]  -> Não é necessariamente uma inicialização, mas ela define e restaura se necessário as curvas padrões do projeto.
    void ESP_setDefaults() override {
      temperature1 = 60;
      temperature2 = 65;
      temperature3 = 70;
      duration1 = 20000;
      duration2 = 20000;
      duration3 = 20000;
    }

    // ============= FIM DAS INICIALIZAÇÕES =========================


    // ============= FUNÇÕES DE SUPORTE DA UART =====================


    // [UART] -> Função utilizada para printar o estado atual no serial
    void ESP_printState(std::string state) override {

      Serial.print("[ESTADO ATUAL] ");
      Serial.println(state.c_str());  // ← std::string precisa de .c_str()
    }

    // [UART] -> Função que printa no serial para mostrar que há mudança de estado
    void ESP_stateChange() override {

      Serial.println("=== Mudança de Estado ===");
    }

    // [UART] -> Função que printa as configurações no Serial.
    void ESP_printConfig() override {
      Serial.print("[CONFIGURACAO] ");
      Serial.print("Temperatura 1: ");
      Serial.print(temperature1);
      Serial.println(" °C");

      Serial.print("[CONFIGURACAO] ");
      Serial.print("Duracao 1: ");
      Serial.print(duration1);
      Serial.println(" ms");

      Serial.print("[CONFIGURACAO] ");
      Serial.print("Temperatura 2: ");
      Serial.print(temperature2);
      Serial.println(" °C");

      Serial.print("[CONFIGURACAO] ");
      Serial.print("Duracao 2: ");
      Serial.print(duration2);
      Serial.println(" ms");

      Serial.print("[CONFIGURACAO] ");
      Serial.print("Temperatura 3: ");
      Serial.print(temperature3);
      Serial.println(" °C");

      Serial.print("[CONFIGURACAO] ");
      Serial.print("Duracao 3: ");
      Serial.print(duration3);
      Serial.println(" ms");
    }

    // ============= FUNÇÕES DE SUPORTE Do BUZZER =====================

    // [BUZZER] -> Apito de quando a operação é concluída.
    void ESP_buzzerComplete(sc::integer pin) override{

      digitalWrite(pin, LOW);
      delay(1000);
      digitalWrite(pin, HIGH);
      delay(1000);
      digitalWrite(pin, LOW);
      delay(1000);
      digitalWrite(pin, HIGH);
      delay(1000);
      digitalWrite(pin, LOW);
      delay(1000);
      digitalWrite(pin, HIGH);
      delay(1000);
    }

    // ============= FUNÇÕES DE SUPORTE DO MIXER =====================
    
    // [MIXER] -> Liga o Mixer e printa um aviso no serial.
    void ESP_mixerON(sc::integer pin) override {
      Serial.print("[MIXER] ");
      Serial.println(" Ligando o Mixer");
      digitalWrite(pin, HIGH);
    }

    // [MIXER] -> Desliga o Mixer e printa um aviso no serial.
    void ESP_mixerOFF(sc::integer pin) override {
      Serial.print("[MIXER] ");
      Serial.println(" desligando o Mixer");
      digitalWrite(pin, LOW);
    }

    // ============= FUNÇÕES DE SUPORTE DO KEYPAD =====================

    // [KEYPAD] -> Espera por uma tecla e retorna ela na função.
    char waitForKey() {
      char key = NO_KEY;         // cria uma char remetendo-se ao 'vazio' de tecla
      while (key == NO_KEY) {    // loop enquanto não há uma tecla pressionada
        key = keypad->getKey();  // ← função da biblioteca que pega a tecla pressionada e anexa em key
        delay(10);
      }
      return key;                // Retorna key.
    } 

    //Funções não utilizadas
    void printKeypadInput(){}

    // ============= FUNÇÕES DE SUPORTE DO HEATER =====================

    // [HEATER] -> Liga o Heater na potência máxima.
    void heat(){
      ledcWrite(PWM_CHANNEL, 255); // Duty cycle de 100% (255/255)
    }

    // [HEATER] -> desliga o Heater.
    void cold(){
      ledcWrite(PWM_CHANNEL, 0); // Duty cycle de 0% (0/255)
    }

    // ============= FUNÇÕES DE SUPORTE DO DISPLAY =====================
    
    // [DISPLAY] -> Plota no display o que deve aparecer no estado 'IDLE'.
    void ESP_displayIdle() override {
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("STATE IDLE");
      display.println("config atual via UART");
      display.setCursor(0,24);
      display.println("1 - Configuracoes");
      display.println("2 - Procedimento");
      display.println("3 - Restart System");
      display.display();
    }

    // [DISPLAY] -> Plota no display o que deve aparecer no estado 'Procedure'.
    void ESP_displayProcedure() override {

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("STATE OPERATION");
      display.println("ETAPA [0/3]");
      display.setCursor(0,16);
      display.print("temperatura1: ");
      display.print(temperature1);
      display.println(" °C");
      display.print("duration1: ");
      display.print(duration1);
      display.println(" ms");
      display.print("temperatura2: ");
      display.print(temperature2);
      display.println(" °C");
      display.print("duration2: ");
      display.print(duration2);
      display.println(" ms");
      display.print("temperatura3: ");
      display.print(temperature3);
      display.println(" °C");
      display.print("duration3: ");
      display.print(duration3);
      display.println(" ms");
      display.display();
      delay(5000);
    }

    // [DISPLAY] -> Plota no display o que deve aparecer no estado 'Procedure 1'.
    void ESP_displayProcedure1() override {

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("STATE OPERATION");
      display.println("ETAPA [1/3]");
      display.setCursor(0,24);
      display.print("temperatura1: ");
      display.print(temperature1);
      display.println(" °C");
      display.print("duration1: ");
      display.print(duration1);
      display.println(" ms");
      display.display();
      delay(5000);
    }

    // [DISPLAY] -> Plota no display o que deve aparecer no estado 'Procedure 2'.
    void ESP_displayProcedure2() override {

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("STATE OPERATION");
      display.println("ETAPA [2/3]");
      display.setCursor(0,24);
      display.print("temperatura2: ");
      display.print(temperature2);
      display.println(" °C");
      display.print("duration2: ");
      display.print(duration2);
      display.println(" ms");
      display.display();
      delay(5000);
    }

    // [DISPLAY] -> Plota no display o que deve aparecer no estado 'Procedure 3'.
    void ESP_displayProcedure3() override {

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("STATE OPERATION");
      display.println("ETAPA [3/3]");
      display.setCursor(0,24);
      display.print("temperatura3: ");
      display.print(temperature3);
      display.println(" °C");
      display.print("duration3: ");
      display.print(duration3);
      display.println(" ms");
      display.display();
      delay(5000);
    }


    // [DISPLAY] -> Plota no display o que deve aparecer no estado 'Complete'.
    void ESP_displayComplete() override {

      Serial.print("[UART] ");
      Serial.println(" OPERAÇÃO COMPLETA ");

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("STATE COMPLETE");
      display.println("Operacao concluida");
      display.setCursor(0,24);
      display.println("1 - Reset");
      display.println("2 - Restart system");
      display.display();
    }

    // [DISPLAY] -> Plota no display o que deve aparecer no estado 'Config' (Após definir as configurações.)
    void ESP_displayConfig() override {

      display.clearDisplay();
      display.setCursor(0,0);
      display.println("STATE CONFIG");
      display.println("config atual via UART");
      display.setCursor(0,24);
      display.println("1 - Save Config");
      display.println("2 - Restart Config");
      display.println("3 - Restart System");
      display.display();
    }


    // ============= FUNÇÕES DE CONFIGURAÇÃO =====================


    // [CONFIGURAÇÃO / DISPLAY / UART] -> Passo a passo da configuração de uma nova curva, com utilização do display e da UART.
    void ESP_displayInitialConfig() override {

      // Começa printando no display que o usuário tem que efetuar as alteraçoes pela UART.
      display.clearDisplay();
      display.setCursor(0,0);
      display.println("STATE CONFIG");
      display.println("config atual via UART");
      display.setCursor(0,24);
      display.println("Altere os valores pela UART");
      display.display();

      // Plota as intruções na UART.
      Serial.println("[CONFIGURACAO] Iniciando Configuração");
      Serial.print("[CONFIGURACAO] Temperatura 1 atual: ");
      Serial.println(temperature1);
      Serial.println("[CONFIGURACAO] Digite a Temperatura 1 desejada:");

      // Cria um variável received para receber informações da UART.
      std::string received = "";

      // Loop para receber a informação da UART.
      while(true){
          if(Serial.available()){
              char c = Serial.read();
              if (c == '\n') {
                  break;
              }
            
              if (c != '\r') {
                  received += c;
              }
          }
          delay(10);
      }

      // Plota na UART a alteração efetuada.
      Serial.print("[CONFIGURACAO] ");
      Serial.print("temperatura 1 digitada: ");
      Serial.println(received.c_str());

      // Anexa o valor digitado na variável em questão
      temperature1 = std::stoi(received); 

      
      // A PARTIR DAQUI SÓ REPETE PARA TODAS AS VARIÁVEIS (TEMPERATURAS E DURAÇÕES)
      //====================================================================
      
      // TEMPERATURA 2

      Serial.print("[CONFIGURACAO] Temperatura 2 atual: ");
      Serial.println(temperature2);
      Serial.println("[CONFIGURACAO] Digite a Temperatura 2 desejada:");
      received = "";
      while(true){
          if(Serial.available()){
              char c = Serial.read();
              if (c == '\n') {
                  break;
              }
            
              if (c != '\r') {
                  received += c;
              }
          }
          delay(10);
      }
      Serial.print("[CONFIGURACAO] ");
      Serial.print("temperatura 1 digitada: ");
      Serial.println(received.c_str());

      temperature2 = std::stoi(received);

      //====================================================================

      // TEMPERATURA 3

      Serial.print("[CONFIGURACAO] Temperatura 3 atual: ");
      Serial.println(temperature3);
      Serial.println("[CONFIGURACAO] Digite a Temperatura 3 desejada:");
      received = "";
      while(true){
          if(Serial.available()){
              char c = Serial.read();
              if (c == '\n') {
                  break;
              }
            
              if (c != '\r') {
                  received += c;
              }
          }
          delay(10);
      }
      Serial.print("[CONFIGURACAO] ");
      Serial.print("temperatura 3 digitada: ");
      Serial.println(received.c_str());

      temperature3 = std::stoi(received);

      //====================================================================

      // DURAÇÃO 1

      Serial.print("[CONFIGURACAO] Duration 1 atual: ");
      Serial.println(duration1);
      Serial.println("[CONFIGURACAO] Digite a Duration 1 desejada:");
      received = "";
      while(true){
          if(Serial.available()){
              char c = Serial.read();
              if (c == '\n') {
                  break;
              }
            
              if (c != '\r') {
                  received += c;
              }
          }
          delay(10);
      }
      Serial.print("[CONFIGURACAO] ");
      Serial.print("duration 1 digitada: ");
      Serial.println(received.c_str());

      duration1 = std::stoi(received);

      //====================================================================

      // DURAÇÃO 2

      Serial.print("[CONFIGURACAO] Duration 2 atual: ");
      Serial.println(duration2);
      Serial.println("[CONFIGURACAO] Digite a Duration 2 desejada:");
      received = "";
      while(true){
          if(Serial.available()){
              char c = Serial.read();
              if (c == '\n') {
                  break;
              }
            
              if (c != '\r') {
                  received += c;
              }
          }
          delay(10);
      }
      Serial.print("[CONFIGURACAO] ");
      Serial.print("duration 2 digitada: ");
      Serial.println(received.c_str());

      duration2 = std::stoi(received);

      //====================================================================

      // DURAÇÃO 3

      Serial.print("[CONFIGURACAO] Duration 3 atual: ");
      Serial.println(duration3);
      Serial.println("[CONFIGURACAO] Digite a Duration 3 desejada:");
      received = "";
      while(true){
          if(Serial.available()){
              char c = Serial.read();
              if (c == '\n') {
                  break;
              }
            
              if (c != '\r') {
                  received += c;
              }
          }
          delay(10);
      }
      Serial.print("[CONFIGURACAO] ");
      Serial.print("duration 3 digitada: ");
      Serial.println(received.c_str());

      duration3 = std::stoi(received);
    }

    // ============= FUNÇÕES DE PROCEDIMENTO =====================

    // [PROCEDURE] -> Função que define o funcionamento de cada etapa de produção. recebe como parâmetro em qual procedimento está.
    void ESP_procedure(sc::integer procedure) override {

      // Cria as variáveis de procedimento
      int target_temperature;                   // temperatura alvo
      unsigned long target_duration;            // duração alvo
      unsigned long start_time = millis();      // anexa o tempo inicial na função start_time

      // Identifica em que procedimento está, e anexa na temperatura alvo e duração alvo.
      if(procedure == 1){
        target_temperature = temperature1;
        target_duration = duration1;
      }
      else if(procedure == 2){
        target_temperature = temperature2;
        target_duration = duration2;
      }
      else if(procedure == 3){
        target_temperature = temperature3;
        target_duration = duration3;
      }

      // cria variáveis para histerese baseada na temperatura alvo.
      float maxTemp = target_temperature + 2.0;
      float minTemp = target_temperature - 2.0;

      // durante o periodo de duração proposto verifica se a temperatura está dentro dos parametros de histerese e aciona as funções de ligar/desligar o heater.
      while(millis() - start_time < target_duration){
        if(actual_temperature < minTemp){
          Serial.println("[HEATER] Aquecedores na máxima potência");
          heat();
        }
        else if(actual_temperature > maxTemp){
          Serial.println("[HEATER] Aquecedores desligados");
          cold();
        }
        else{}
      }
    }

    // ============= FUNÇÕES DE MUDANÇA DE ESTADO =====================  

    // [STATE CHANGE] -> Função que parte do estado IDLE para mudar para um outro a partir da escolha do usuário pelo keypad.
    void ESP_idleNextState() override {
      Serial.print("[UART] ");
      Serial.println("Tome sua decisão pelo teclado");
      char c = waitForKey();  // Bloqueia até tecla
      Serial.print("[KEYPAD] Tecla pressionada: ");
      Serial.println(c);
      if(c == '1'){
        statechart.raiseStart_Config(); // Vai para o CONFIG
      }
      else if(c == '2'){
        statechart.raiseStart_Operation(); // Vai para o OPERATION
      }
      else if(c == '3'){
        statechart.raiseTotal_Reset(); // System Reset.
        display.clearDisplay(); // Limpa display
        display.display();  // Printa Display
      }
      else{
        Serial.println("[ERROR] Tecla diferente pressionada.... Resetando o Sistema");
        statechart.raiseTotal_Reset(); // System Reset
        display.clearDisplay(); //Limpa Display
        display.display();  // Printa Display
      }
    }

    // [STATE CHANGE] -> Função que parte do estado CONFIG para mudar para um outro a partir da escolha do usuário pelo keypad.
    void ESP_configNextState() override {

      Serial.print("[UART] ");
      Serial.println("Tome sua decisão pelo teclado");
      char c = waitForKey();  // Bloqueia até tecla
      Serial.print("[KEYPAD] Tecla pressionada: ");
      Serial.println(c);
      if(c == '1'){
        statechart.raiseSave_Config(); // Vai pro IDLE
      }
      else if(c == '2'){
        statechart.raiseRestart_Config(); // Reinicia o Config
      }
      else if(c == '3'){
        statechart.raiseTotal_Reset();  // System Reset
        display.clearDisplay(); // Limpa Display
        display.display();  // Printa Display
      }
      else{
        Serial.println("[ERROR] Tecla diferente pressionada.... Resetando o Sistema");
        statechart.raiseTotal_Reset();  // System Reset
        display.clearDisplay(); // Limpa Display
        display.display();  // Printa Display
      }
    }

    // [STATE CHANGE] -> Função que parte do estado COMPLETE para mudar para um outro a partir da escolha do usuário pelo keypad.
    void ESP_completeNextState() override {

      Serial.print("[UART] ");
      Serial.println("Tome sua decisão pelo teclado");
      char c = waitForKey();  // Bloqueia até tecla
      Serial.print("[KEYPAD] Tecla pressionada: ");
      Serial.println(c);
      if(c == '1'){
        statechart.raiseReset();  //Vai pro IDLE
      }
      else if(c == '2'){
        statechart.raiseTotal_Reset();  // System Reset
        display.clearDisplay();
        display.display();
      }
      else{
        Serial.println("[ERROR] Tecla diferente pressionada.... Resetando o Sistema");
        statechart.raiseTotal_Reset();  // System Reset
        display.clearDisplay();
        display.display();
      }
    }


    // ============= FUNÇÃO DE CAPTURA DE DADOS COM SENSOR =====================

    void SendActualTemperature(float newTemp){
      actual_temperature = newTemp;
    }

    

    


    


};

#endif // STATECHARTCALLBACK_H
