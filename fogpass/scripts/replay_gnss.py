#!/usr/bin/env python3
"""Replay recorded GNSS samples over a socket."""

import socket
import time

SAMPLES = [
    "48.2082,16.3738,180",
    "48.2090,16.3745,182",
]

HOST = "127.0.0.1"
PORT = 5005

with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
    for sample in SAMPLES:
        sock.sendto(sample.encode(), (HOST, PORT))
        time.sleep(1)
