# Projeto RC-Auction - Plataforma de Leilões

Este projeto, desenvolvido por Leonardo Alves (103706) e Gonçalo Rodrigues (90256), trata-se de uma aplicação para criação e gestão de leilões através de conexões UDP e TCP.
Funcionalidades

O projeto está completamente implementado, permitindo a execução de todos os comandos requeridos, tais como:

    Utilizadores: login, logout, unregister, exit
    Leilões: open, close, show_asset, show_record, my_auctions, bid, my_bids, list
    (Consulte o enunciado para informações detalhadas sobre a utilização dos comandos).

-Utilização do Makefile

O projeto inclui um makefile que oferece os seguintes comandos:

    make all: Cria todos os executáveis e faz o link, gerando os executáveis finais AS e user.
    make clean: Elimina os executáveis.
    make server_clean: Limpa as pastas do USER e AS.

-Observações

O servidor pode ser terminado apenas através do comando CTRL+C. Ele armazena permanentemente todas as informações, incluindo criação, fecho e listagem de leilões e utilizadores em diretórios pré-criados denominados USERS e AUCTIONS.
Esta plataforma proporciona uma gestão completa de leilões e utilizadores, com comandos claros para a sua operação.

