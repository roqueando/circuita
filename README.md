# Heximula - Simulador de Microcontrolador

Um simulador de microcontrolador que executa firmware real em um emulador QEMU com interface CLI para monitoramento de pinos e registradores.

## Descrição do Projeto

O Heximula é um simulador que permite carregar e executar firmware de microcontrolador em um ambiente emulado usando QEMU. O sistema fornece uma interface CLI para monitorar o estado dos pinos e registradores em tempo real.

### Funcionalidades Principais

- **Upload de Firmware**: Carregamento de arquivos binários de firmware via CLI
- **Emulação QEMU**: Execução do firmware em um ambiente emulado
- **Monitoramento de Pinos**: Interface para visualizar estado de pinos (alto/baixo)
- **Monitoramento de Registradores**: Acompanhamento de valores de registradores
- **Interface CLI**: Comandos interativos para controle do simulador

## Módulos do Sistema

### • Módulo CLI (Command Line Interface)
**Arquivo**: `src/cli/cli.c`
**Descrição**: Gerencia a interface de linha de comando do simulador
**Funções Principais**:
- `cli_init()`: Inicializa a interface CLI
- `cli_parse_args()`: Processa argumentos da linha de comando
- `cli_handle_upload()`: Gerencia upload de firmware
- `cli_interactive_mode()`: Modo interativo para comandos

### • Módulo QEMU Interface
**Arquivo**: `src/qemu/qemu_interface.c`
**Descrição**: Interface para comunicação com o emulador QEMU
**Funções Principais**:
- `qemu_start()`: Inicia o processo QEMU
- `qemu_stop()`: Para o emulador QEMU
- `qemu_send_command()`: Envia comandos para QEMU
- `qemu_read_response()`: Lê respostas do QEMU

### • Módulo Microcontroller Core
**Arquivo**: `src/core/microcontroller.c`
**Descrição**: Simula o núcleo do microcontrolador
**Funções Principais**:
- `mcu_init()`: Inicializa o microcontrolador
- `mcu_load_firmware()`: Carrega firmware na memória
- `mcu_step()`: Executa um ciclo de instrução
- `mcu_get_pin_state()`: Obtém estado de um pino

### • Módulo Pin Manager
**Arquivo**: `src/core/pin_manager.c`
**Descrição**: Gerencia o estado dos pinos I/O
**Funções Principais**:
- `pin_init()`: Inicializa o sistema de pinos
- `pin_set_state()`: Define estado de um pino
- `pin_get_state()`: Obtém estado atual de um pino
- `pin_monitor()`: Monitora mudanças de estado

### • Módulo de Testes
**Arquivo**: `test/test_basic.c`
**Descrição**: Testes unitários para os módulos
**Funções Principais**:
- `test_pin_manager()`: Testa gerenciador de pinos
- `test_microcontroller()`: Testa microcontrolador
- `test_cli()`: Testa interface CLI

## Como Usar

### Compilação
```bash
mkdir build && cd build
cmake ..
ninja  # ou make, dependendo do gerador
```

### Execução
```bash
# Ajuda
./heximula --help

# Versão
./heximula --version

# Modo interativo
./heximula --interactive

# Upload de firmware (quando implementado)
./heximula --firmware path/to/firmware.bin

# Com tipo específico de microcontrolador
./heximula --mcu atmega328p --interactive
```

### Comandos Interativos
- `pin <numero>`: Mostra estado do pino especificado
- `reg <nome>`: Mostra valor do registrador
- `step`: Executa um ciclo de instrução
- `run`: Executa continuamente
- `stop`: Para a execução
- `quit`: Sai do simulador
- `help`: Mostra ajuda

### Exemplo de Uso
```bash
# Inicia o simulador em modo interativo
./heximula --interactive

# No prompt interativo:
heximula> help
heximula> pin 13
heximula> quit
```

## Estrutura de Arquivos

```
heximula/
├── src/
│   ├── main.c                 # Ponto de entrada
│   ├── cli/                   # Interface de linha de comando
│   │   └── cli.c
│   ├── qemu/                  # Interface QEMU
│   │   └── qemu_interface.c
│   └── core/                  # Núcleo do microcontrolador
│       ├── microcontroller.c
│       └── pin_manager.c
├── include/                   # Headers
│   ├── cli.h
│   ├── qemu_interface.h
│   ├── microcontroller.h
│   └── pin_manager.h
├── test/                      # Testes unitários
│   ├── test_basic.c
│   └── CMakeLists.txt
├── examples/                  # Exemplos de uso
│   ├── simple_firmware.c
│   └── simple_firmware.bin
├── heximula.cfg              # Arquivo de configuração
├── CMakeLists.txt            # Configuração do build
└── README.md                 # Esta documentação
```

## Dependências

- **QEMU**: Emulador de sistema (opcional para funcionalidade completa)
- **libconfig**: Leitura de arquivos de configuração
- **CMake**: Sistema de build
- **C Standard Library**: Biblioteca padrão C

## Status do Desenvolvimento

- [x] Estrutura básica do projeto
- [x] Sistema de build com CMake
- [x] Interface CLI básica
- [x] Módulo de gerenciamento de pinos
- [x] Módulo de microcontrolador básico
- [x] Testes unitários
- [x] Documentação básica
- [ ] Integração completa com QEMU
- [ ] Simulação real de instruções
- [ ] Carregamento de firmware real
- [ ] Interface gráfica (futuro)

## Testes

Para executar os testes:
```bash
cd build
./test/test_basic
```

Os testes verificam:
- Funcionamento do gerenciador de pinos
- Inicialização do microcontrolador
- Interface CLI
- Operações básicas de memória e registradores

## Configuração

O arquivo `heximula.cfg` permite configurar:
- Tipo de microcontrolador
- Configurações do QEMU
- Mapeamento de pinos
- Configurações de debug

## Próximos Passos

1. **Integração com QEMU**: Implementar comunicação real com o emulador
2. **Simulação de Instruções**: Decodificar e executar instruções reais
3. **Carregamento de Firmware**: Suporte para formatos binários reais
4. **Interface Gráfica**: Desenvolver interface visual para monitoramento
5. **Suporte a Mais MCUs**: Adicionar suporte para outros microcontroladores

## Contribuição

Para contribuir com o projeto:
1. Fork o repositório
2. Crie uma branch para sua feature
3. Implemente suas mudanças
4. Adicione testes
5. Envie um pull request

## Licença

Este projeto está licenciado sob a licença MIT. Veja o arquivo `LICENSE` para mais detalhes.
