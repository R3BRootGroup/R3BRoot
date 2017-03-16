from __future__ import print_function
import zmq

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:55555")

print("RecoServer ready on tcp://*:55555")
while True:
    message = socket.recv()
    socket.send_string("0")
    print('.', end="", flush=True)
