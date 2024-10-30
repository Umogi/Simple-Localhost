# Simple HTTP Server with PHP Support
This repository contains a simple C-based HTTP server for Windows that serves basic HTML and PHP content. When the server receives a request for `index.php`, it executes the PHP file locally and returns the output as HTML. For any other request, it returns a 404 error page.

## Features
- Basic HTTP Server: Listens on port `8080` and accepts client connections.
- PHP Execution: Processes and returns the output of `index.php` using a local PHP interpreter.
- 404 Error Handling: Responds with a 404 page for unsupported requests.

## Requirements
- Windows OS: This server relies on Windows-specific libraries.
- C Compiler: Compatible with MSVC, MinGW, or similar compilers.
- PHP Interpreter: Must be installed and accessible via the php command.

## Setup

1. Clone the repository:
```
git clone https://github.com/Umogi/Simple-Localhost.git
cd Simple-Localhost
```

2. Compile the server: Use a C compiler like MSVC or MinGW. Example command with GCC:

### Windows
```
gcc local_host_win -o server.c -lws2_32
```

### Linux
```
gcc local_host_win -o server.c
```

3. Run the server:

```
./simple_http_server
```

&nbsp;&nbsp;&nbsp;&nbsp;The server will start listening on `http://localhost:8080`.

## Usage
1. Place your PHP script: The server expects a file named `index.php` in the same directory as the executable.

2. Access the server: Open a browser and navigate to `http://localhost:8080/index.php` to see the PHP output.

3. Unsupported URLs: If you request any URL other than `/index.php`, the server will respond with a `404 Not Found` error.

## How It Works
1. Socket Setup: Initializes and binds a TCP socket on port 8080.

2. Request Handling:

    - Checks if the client requests index.php.
    - If so, it uses popen to execute the PHP script and captures its output.
    - If not, it sends a 404 error page.
 
3. Response Formatting: Builds an HTTP response with headers and content, then sends it to the client.
