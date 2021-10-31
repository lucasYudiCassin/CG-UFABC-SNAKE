# Snake

Esse é um jogo criado para a disciplina de computação gráfica da UFABC - 2021.Q2

Jogo baseado no clássico snake e no [slither.io](http://slither.io/) 

**[Clique aqui para jogar!](https://lucasyudicassin.github.io/snakeCG/)**

Desenvolvido pelos alunos:
 - Fernando Kenji - RA: 11201722146
 - Lucas Yudi - RA: 11201812210

# Dependência

Foi utilizada a biblioteca [ABCg](https://github.com/hbatagelo/abcg)

Desenvolvida por Harlen Batagelo


# O jogo
## Objetivo

Comer 30 comidas sem encostar nos obstáculos
## Controles

Utilize as setas para direita  `→`  e esquerda  `←` , ou as teclas `a` e `d` para controlar o movimento da cobra.
## Tela
Na tela do jogo, os losangos vermelhos rodando, são as `comidas`

Os `obstáculos` são os polígonos azuis, eles aparecem aleatoriamente de tempo em tempo

No canto superior esquerdo está o `placar` e o `objetivo`

A cobra muda de cor toda vez que o jogo é reiniciado

# Técnicas
O código esta dividido em classes, e cada uma fica responsável por uma parte do jogo. 

A cobra é um vetor de pontos, que possui o atributo da posição e da cor. 

Para desenhar a comida e o obstáculo é verificado se nada irá se sobrepor. 

A colisão é verificada a partir da distância entre os centros dos objetos (cobra, comida e obstáculo) considerando a escala.

Os controles modificam a rotação da cobra, e o movimento se da adicionando um novo ponto considerando o tempo passado, a velocidade e a rotação. Caso uma comida tenha sido pega, nenhum outro ponto é excluído, caso o contrário, o último ponto é excluído.
