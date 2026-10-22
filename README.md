# ps5-discord-rpc
Discord RPC for jailbroken PS5 consoles.

## Screenshots
| Burnout Paradise Remastered                                                                                   | Marvel's Wolverine                                                                 |
|---------------------------------------------------------------------------------------------------------------|------------------------------------------------------------------------------------|
| ![Discord status when Burnout Paradise Remastered is running](.github/assets/burnout-paradise-remastered.png) | ![Discord status when Marvel's Wolverine is running](.github/assets/wolverine.png) |

## Usage

### Server
Download it from the [releases](https://github.com/azurejelly/ps5-discord-rpc/releases) page, then load it on the PS5 by using elfldr or whatever else you have. By default the server will run on port `7270`, however, you might change this from the configuration file at `/data/ps5-discord-rpc/config.ini`:

```ini
[config]
version=1

[server]
port=7270
```

This file is created when the server first starts.

### Client
Since this project was made specifically for my own personal use, I will only be covering how to do this on Linux/macOS. Feel free to open a pull request to improve this section. To get started, clone the repo:
```shell
$ git clone https://github.com/azurejelly/ps5-discord-rpc
$ cd ps5-discord-rpc/client/
```

Ensure you have Python 3.9 or newer:
```shell
$ python3
Python 3.14.8 (main, Oct  1 2026, 00:00:00) [GCC 16.2.1 20260819 (Red Hat 16.2.1-2)] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>> exit()
```

Create a virtual environment:
```shell
$ python3 -m venv .venv
$ source .venv/bin/activate
```

Install dependencies:
```shell
$ pip install -r requirements.txt
```

Copy `.env.example` to `.env`, set the fields to something that makes sense and, finally, run the client:
```shell
$ python3 main.py
```

## Building
Clone the repo and go into the `server/` folder:
```sh
$ git clone https://github.com/azurejelly/ps5-discord-rpc
$ cd ps5-discord-rpc/server/
```

Assuming you already have all dependencies installed:
```sh
$ export PS5_PAYLOAD_SDK=/opt/ps5-payload-sdk
$ make
```

You will find the .elf file inside the `build/` folder.
