#!/usr/bin/env python3

# Description: Simple python script to serve the emscripten build directory over https.

import http.server, ssl, os, sys

if len(sys.argv) != 2:
    print("Usage: emserver.py <build_dir>")
    sys.exit(1)

print(f"Serving from {sys.argv[1]}")
os.chdir(sys.argv[1])

server_address = ('0.0.0.0', 4443)
httpd = http.server.HTTPServer(server_address, http.server.SimpleHTTPRequestHandler)
httpd.socket = ssl.wrap_socket(httpd.socket,
                               server_side=True,
                               certfile='localhost.pem',
                               ssl_version=ssl.PROTOCOL_TLS)
httpd.serve_forever()