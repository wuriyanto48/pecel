# Pecel

[![Pecel CI](https://github.com/wuriyanto48/pecel/actions/workflows/ci.yml/badge.svg)](https://github.com/wuriyanto48/pecel/actions/workflows/ci.yml)

An Experimental `Distributed Key Value Store` written in `C`

### Building

Requirements:

- `gcc` compiler https://gcc.gnu.org/
- `cmake` https://cmake.org/download/

Build from source:

```shell
$ make build
```

### Running

The build results will be in the `bin` folder
```shell
$ ./bin/pecel pecel.conf 
```

### Connect to `pecel server` with `nc`

Login to server with `ATH` command, for example your server password is `12345`.
```shell
$ nc 127.0.0.1 6009
$ CONNECTED*
$ ATH 12345
$ OK*
```

Set and Get value
```shell
$ nc 127.0.0.1 6009
$ CONNECTED*
$ SET 1 wury
$ OK*
$ GET 1
$ wury
```

### TODO
- more validation and error handling