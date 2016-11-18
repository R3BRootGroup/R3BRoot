from __future__ import print_function
import zmq
import stream

#  Socket to talk to server
print("Connecting to Recoserver on tcp://localhost:55555")
context = zmq.Context()
socket = context.socket(zmq.REQ)
socket.connect("tcp://localhost:55555")


print("Reading train data ...")
istream = stream.open("output/train.gam", "rb")

for i, data in enumerate(istream):
    print("Sending request %s …" % data)
    socket.send(data)

    #  Get the reply.
    message = int((socket.recv()))
    print("Received reply: %i" % message)

    if i > 10:
        break

istream.close()
