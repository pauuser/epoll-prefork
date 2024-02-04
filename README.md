# Static server in C using pre-fork and epoll

This is a simple static server implemented in C using the pre-fork model for handling multiple connections and epoll for event-driven I/O.

## Features
- Pre-fork model for handling multiple connections efficiently
- Epoll for multiplexing
- Serve static files to clients
- GET and HEAD requests
- Available extensions: .html, .css, .js, .png, .jpg, .jpeg, .swf, .gif
- Large files are also supported
- Self-written logger is used

## Installation
1. Clone the repository: git clone https://github.com/pauuser/epoll-prefork.git
2. Go to `src` folder and build the project: `make`
3. Run the server: ./server

## Documentation
The documentation for this project can be found in the [doc](./doc/) directory. It includes information about the design, implementation, and usage of the static server. Documentation is written in Russian following [ГОСТ 7.32-2017](https://cs.msu.ru/sites/cmc/files/docs/2021-11gost_7.32-2017.pdf) and [ГОСТ 7.0.100-2018](https://www.rsl.ru/photo/!_ORS/5-PROFESSIONALAM/7_sibid/%D0%93%D0%9E%D0%A1%D0%A2_%D0%A0_7_0_100_2018_1204.pdf) standards using LaTeX.

PDF version can be found [here](./doc/rpz/TeX/main.pdf).