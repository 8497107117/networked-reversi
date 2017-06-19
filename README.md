# Two-Player Networked Reversi(Othello) Game
2017 NCTU Unix programming HW4 - two player networked reversi game

## Compile and Run

- `make clean`
- `make`
- `./reversi -s <PORT>`
    - server will wait for only one client to connect
- `./reversi -c <IP>:<PORT> ` 
    - prefer `127.0.0.1` than `localhost`
    
## Features

- show message of whose turn
- show the current score of each player
- show help message
- show message of winner
- can play multiple times by reset

## How to play ?

- Move
    - Up: `k` or `key up`
    - Down: `j` or `key down`
    - LEFT: `h` or `key left`
    - RIGHT: `l` or `key right`
- Put a piece
    - `space` or `Enter`
- Reset game
    - `r` or `R`
- Quit
    - `q` or `Q`
