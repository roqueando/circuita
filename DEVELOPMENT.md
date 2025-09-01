# Heximula - Guia de Desenvolvimento

Este documento contém informações técnicas para desenvolvedores que desejam contribuir ou entender o funcionamento interno do Heximula.

## Arquitetura do Sistema

### Estrutura de Módulos

O Heximula é organizado em módulos independentes que se comunicam através de interfaces bem definidas:

```
┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐
│       CLI       │    │   Microcontroller│    │   QEMU Interface│
│                 │    │                 │    │                 │
│ - Argument      │    │ - Memory        │    │ - Process       │
│   parsing       │    │ - Registers     │    │   management    │
│ - Interactive   │    │ - Execution     │    │ - Monitor       │
│   commands      │    │ - Pin manager   │    │   communication │
└─────────────────┘    └─────────────────┘    └─────────────────┘
         │                       │                       │
         └───────────────────────┼───────────────────────┘
                                 │
                    ┌─────────────────┐
                    │   Pin Manager   │
                    │                 │
                    │ - Pin states    │
                    │ - Monitoring    │
                    │ - Register      │
                    │   mapping       │
                    └─────────────────┘
```

### Fluxo de Execução

1. **Inicialização**: CLI parseia argumentos e inicializa módulos
2. **Configuração**: Carrega configurações do microcontrolador
3. **Setup**: Configura pinos e mapeamentos de registradores
4. **Execução**: Inicia QEMU (se firmware fornecido) ou modo interativo
5. **Monitoramento**: Monitora mudanças de estado dos pinos
6. **Finalização**: Limpa recursos e encerra processos

## Módulos Principais

### CLI Module (`src/cli/`)

**Responsabilidades**:
- Parse de argumentos da linha de comando
- Interface interativa com comandos
- Coordenação entre módulos

**Principais estruturas**:
```c
typedef struct {
    char* firmware_path;
    bool interactive_mode;
    bool verbose;
    char* config_file;
    int mcu_type;
} cli_args_t;

typedef struct {
    char* command;
    char* description;
    int (*handler)(char** args, int argc);
} cli_command_t;
```

**Comandos implementados**:
- `pin <numero>`: Consulta estado do pino
- `reg <nome>`: Consulta valor do registrador
- `step`: Executa um ciclo de instrução
- `run`: Execução contínua
- `stop`: Para execução
- `quit`: Sai do simulador
- `help`: Mostra ajuda

### Microcontroller Module (`src/core/`)

**Responsabilidades**:
- Simulação do núcleo do microcontrolador
- Gerenciamento de memória e registradores
- Execução de instruções
- Interface com pinos

**Principais estruturas**:
```c
typedef struct {
    mcu_config_t config;
    mcu_state_t state;
    uint32_t program_counter;
    uint32_t* registers;
    uint8_t* memory;
    uint32_t memory_size;
    pin_manager_t pin_manager;
    qemu_instance_t qemu;
    bool firmware_loaded;
    char* firmware_path;
} microcontroller_t;
```

**Estados do microcontrolador**:
- `MCU_STATE_RESET`: Estado inicial
- `MCU_STATE_RUNNING`: Executando
- `MCU_STATE_HALTED`: Parado
- `MCU_STATE_ERROR`: Erro

### Pin Manager Module (`src/core/`)

**Responsabilidades**:
- Gerenciamento de estado dos pinos I/O
- Mapeamento de pinos para registradores
- Monitoramento de mudanças de estado
- Interface com o microcontrolador

**Principais estruturas**:
```c
typedef struct {
    int pin_number;
    pin_state_t state;
    pin_direction_t direction;
    bool is_monitored;
    char* name;
    uint32_t register_address;
    int bit_position;
} pin_t;

typedef struct {
    pin_t* pins;
    int pin_count;
    int max_pins;
    bool initialized;
} pin_manager_t;
```

**Estados de pino**:
- `PIN_LOW`: Nível baixo
- `PIN_HIGH`: Nível alto
- `PIN_FLOATING`: Flutuante
- `PIN_PULL_UP`: Pull-up ativo
- `PIN_PULL_DOWN`: Pull-down ativo

### QEMU Interface Module (`src/qemu/`)

**Responsabilidades**:
- Comunicação com o emulador QEMU
- Gerenciamento de processo QEMU
- Interface com monitor QEMU
- Debug e breakpoints

**Principais estruturas**:
```c
typedef struct {
    char* qemu_path;
    char* machine_type;
    char* cpu_type;
    int memory_size;
    char* kernel_path;
    bool enable_monitor;
    int monitor_port;
} qemu_config_t;

typedef struct {
    int pid;
    int monitor_fd;
    bool is_running;
    qemu_config_t config;
} qemu_instance_t;
```

## Convenções de Código

### Nomenclatura

- **Funções**: `snake_case` (ex: `pin_set_state`)
- **Variáveis**: `snake_case` (ex: `pin_number`)
- **Constantes**: `UPPER_SNAKE_CASE` (ex: `PIN_HIGH`)
- **Tipos**: `snake_case_t` (ex: `pin_state_t`)
- **Estruturas**: `snake_case_t` (ex: `microcontroller_t`)

### Tratamento de Erros

- Funções retornam `0` para sucesso, `-1` para erro
- Mensagens de erro são impressas com `fprintf(stderr, ...)`
- Validação de parâmetros no início das funções
- Limpeza de recursos em caso de erro

### Gerenciamento de Memória

- Sempre verificar retorno de `malloc/calloc`
- Usar `free()` para liberar memória alocada
- Funções de cleanup devem ser idempotentes
- Preferir alocação estática quando possível

### Documentação

- Comentários explicam o "porquê", não o "o quê"
- Headers documentam interface pública
- README.md para usuários finais
- DEVELOPMENT.md para desenvolvedores

## Testes

### Estrutura de Testes

Os testes estão organizados em:
- `test/test_basic.c`: Testes unitários básicos
- `test/CMakeLists.txt`: Configuração de build dos testes

### Executando Testes

```bash
cd build
./test/test_basic
```

### Adicionando Novos Testes

1. Crie função de teste: `int test_nome_do_teste(void)`
2. Adicione ao `main()` do arquivo de teste
3. Atualize contadores de teste
4. Execute e verifique resultados

## Build System

### CMake Configuration

- `CMakeLists.txt`: Configuração principal
- `src/CMakeLists.txt`: Módulos do código fonte
- `test/CMakeLists.txt`: Configuração de testes
- `packages/`: Dependências externas

### Dependências

- **libconfig**: Gerenciamento de configurações
- **QEMU**: Emulação (opcional)
- **CMake**: Sistema de build

### Compilação

```bash
mkdir build && cd build
cmake ..
ninja  # ou make
```

## Debugging

### Logs e Debug

- Use `printf` para debug temporário
- Modo verbose com flag `--verbose`
- Logs estruturados para produção

### Valgrind

Para detectar vazamentos de memória:
```bash
valgrind --leak-check=full ./heximula --interactive
```

### GDB

Para debug com GDB:
```bash
gdb ./heximula
(gdb) run --interactive
```

## Próximos Passos de Desenvolvimento

### Prioridade Alta

1. **Integração QEMU**: Implementar comunicação real
2. **Simulação de Instruções**: Decodificar instruções AVR/ARM
3. **Carregamento de Firmware**: Suporte para ELF/bin

### Prioridade Média

1. **Interface Gráfica**: Desenvolver GUI
2. **Mais MCUs**: Suporte para PIC, MSP430
3. **Periféricos**: Timer, UART, SPI, I2C

### Prioridade Baixa

1. **Plugins**: Sistema de plugins
2. **Scripting**: Interface de scripting
3. **Documentação**: Documentação avançada

## Contribuição

### Processo de Contribuição

1. Fork o repositório
2. Crie branch para feature: `git checkout -b feature/nome`
3. Implemente mudanças
4. Adicione testes
5. Commit com mensagem descritiva
6. Push e crie Pull Request

### Checklist de Pull Request

- [ ] Código compila sem warnings
- [ ] Testes passam
- [ ] Documentação atualizada
- [ ] Não há vazamentos de memória
- [ ] Código segue convenções
- [ ] Funcionalidade testada

### Padrões de Commit

```
tipo(escopo): descrição curta

Corpo detalhado se necessário

Fixes #123
```

Tipos:
- `feat`: Nova funcionalidade
- `fix`: Correção de bug
- `docs`: Documentação
- `style`: Formatação
- `refactor`: Refatoração
- `test`: Testes
- `chore`: Manutenção

## Contato

Para dúvidas técnicas ou sugestões:
- Abra uma issue no GitHub
- Discuta no fórum do projeto
- Entre em contato com os mantenedores
