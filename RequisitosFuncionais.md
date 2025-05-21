### O que são requisitos funcionais? ###

Dando uma breve explicação, requisitos funcionais são todos aqueles requisitos e características que um projeto deve ter, de modo a atender todos os requisitos estipulados.

### Requisitos Funcionais do Projeto ###

- Capacidade de Operação em tempo real;
- Capacidade de Leitura de sensores de temperatura via I2C;
- Capacidade de controle de aquecedores via GPIO ou PWM, utilizando como método controle PID ou ON/OFF por histerese;
- Capacidade de controle de um mixer por GPIO ou PWM que deve ser ativado quando a diferença de temperatura entre um resitor e outro é superior a 1°C;
- O sistema deve ter uma curva padrão de temperatura e tempos de operação para uma operação Plug and Play;
- Deve ser possível adicionar novas curvas além da curva padrão;
- O sistema deve realizar uma comunicação serial com um PC (UART), por onde novas curvas podem ser inseridas;
- Deve ser capaz de realizar detecção de erros e tratamento de erros;
- Ser capaz de alertar o usuário em caso de problemas de segurança;


### Requisitos Adicionais do Projeto ###

- Display LCD capaz de indicar o andamento do processo de produção;
- Buzzer para alertar o usuário quando a operação for concluída, ou em caso de erro;
