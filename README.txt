Projeto RC 2022/2023 - Grupo 37

Compilar:

O projeto pode ser compilado executando `make` neste diretório.
Como este é um projeto em C++, pode demorar um pouco, dependendo da máquina.
Este projeto utiliza C++17.

Uma vez compilado, dois binários, `user` e `AS`, serão colocados neste diretório.

Executar o cliente:

As opções disponíveis para o executável `user` podem ser visualizadas executando:

./user -h

Uma vez que o cliente está em execução, ele solicitará ao usuário um comando.
A lista de comandos é mostrada na inicialização e pode ser mostrada novamente a qualquer momento digitando `help` ou `?` no prompt.

Todos os comandos funcionam conforme a especificação, com destaque para o comando `show_asset`,
que permite cancelar um download em andamento.

O estado não é salvo entre sessões.
O cliente tenta sair do dar logout antes de sair, mesmo ao receber um sinal SIGINT ou SIGTERM.

Constantes disponíveis:

Essas constantes, definidas em `src/common/constants.hpp`, podem ser alteradas para teste,
conforme solicitado pelos professores:

- `UDP_TIMEOUT_SECONDS`: A quantidade de segundos entre desistir de receber uma resposta UDP.
- `UDP_RESEND_TRIES`: A quantidade de vezes para tentar enviar um pacote UDP antes de desistir.
- `TCP_READ_TIMEOUT_SECONDS`: O tempo limite de leitura para conexões TCP. Se o servidor conectado não escrever dentro deste período, o cliente fecha a conexão.
- `TCP_WRITE_TIMEOUT_SECONDS`: O tempo limite de gravação para conexões TCP. Se o servidor conectado não confirmar dentro deste período, o cliente fecha a conexão.

Executar o servidor:

As opções disponíveis para o executável `AS` podem ser visualizadas executando:

./AS -h

TODO Adicionamos uma opção extra, `-r`, que habilita a seleção aleatória de palavras.
TODO Por padrão, as palavras são selecionadas sequencialmente, conforme solicitado pelos professores.

O servidor persiste dados entre sessões na pasta `ASDIR`, então, durante os testes, pode fazer sentido excluir a pasta após cada teste.
Os ficheiros podem ser abertos como qualquer ficheiro para analisar o estado atual do servidor.

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
