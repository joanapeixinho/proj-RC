Projeto RC 2022/2023 - Grupo 37

Compilar:

O projeto pode ser compilado executando `make` neste diretório.
Como este é um projeto em C++, pode demorar um pouco, dependendo da máquina.
Este projeto utiliza C++17.

Uma vez compilado, dois binários, `player` e `GS`, serão colocados neste diretório.

Executar o jogador:

As opções disponíveis para o executável `player` podem ser visualizadas executando:

./player -h

Uma vez que o jogador está em execução, ele solicitará ao usuário um comando.
A lista de comandos é mostrada na inicialização e pode ser mostrada novamente a qualquer momento digitando `help` ou `?` no prompt.

Além dos comandos solicitados, implementamos o comando `kill PLID`,
que nos permite encerrar um jogo de um jogador específico, principalmente para fins de depuração.
Isso envia a mensagem de protocolo `QUT`.

Todos os comandos funcionam conforme a especificação, com destaque para o comando `hint`,
que permite cancelar um download em andamento.

O estado não é salvo entre sessões.
O jogador tenta sair do jogo atual antes de sair, mesmo ao receber um sinal SIGINT ou SIGTERM.

Constantes disponíveis:

Essas constantes, definidas em `src/common/constants.hpp`, podem ser alteradas para teste,
conforme solicitado pelos professores:

- `UDP_TIMEOUT_SECONDS`: A quantidade de segundos entre desistir de receber uma resposta UDP.
- `UDP_RESEND_TRIES`: A quantidade de vezes para tentar enviar um pacote UDP antes de desistir.
- `TCP_READ_TIMEOUT_SECONDS`: O tempo limite de leitura para conexões TCP. Se o servidor conectado não escrever dentro deste período, o cliente fecha a conexão.
- `TCP_WRITE_TIMEOUT_SECONDS`: O tempo limite de gravação para conexões TCP. Se o servidor conectado não confirmar dentro deste período, o cliente fecha a conexão.

Executar o servidor:

As opções disponíveis para o executável `GS` podem ser visualizadas executando:

./GS -h

Adicionamos uma opção extra, `-r`, que habilita a seleção aleatória de palavras.
Por padrão, as palavras são selecionadas sequencialmente, conforme solicitado pelos professores.

O servidor persiste dados entre sessões na pasta `.gamedata`, então, durante os testes, pode fazer sentido excluir a pasta após cada teste.
Os arquivos armazenados nesta pasta estão em formato binário e podem ser inspecionados com um programa como `hexdump`.

O servidor é extremamente resistente a todos os tipos de entrada incorreta e erros, tentando se recuperar deles quando possível.
Quando isso não é possível, ele sai graciosamente.

O servidor também manipula o sinal SIGINT (CTRL + C), aguardando as conexões TCP existentes terminarem. O usuário pode pressionar CTRL + C novamente para sair do servidor de forma forçada.

Decidimos usar threads para concorrência. Por padrão, o servidor suporta até 50 conexões TCP simultâneas, mas isso pode ser ajustado pela variável `TCP_WORKER_POOL_SIZE` em `src/common/constants.hpp`.
Usamos mutexes para sincronizar o acesso a variáveis compartilhadas.

Constantes disponíveis:

Essas constantes, definidas em `src/common/constants.hpp`, podem ser alteradas para teste:

- `TCP_WORKER_POOL_SIZE`: O número de threads que manipulam conexões TCP, ou seja, o número máximo de conexões TCP simultâneas suportadas.
- `TCP_READ_TIMEOUT_SECONDS`: O tempo limite de leitura para conexões TCP. Se o cliente conectado não escrever dentro deste período, o servidor fecha a conexão.
- `TCP_WRITE_TIMEOUT_SECONDS`: O tempo limite de gravação para conexões TCP. Se o cliente conectado não confirmar dentro deste período, o servidor fecha a conexão.
