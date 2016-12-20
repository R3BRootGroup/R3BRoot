from __future__ import print_function
import tensorflow as tf
import numpy as np
import stream
import itertools
import zmq
from timeit import default_timer as timer

import neuland_pb2 as neuland

context = zmq.Context()
socket = context.socket(zmq.REP)
socket.bind("tcp://*:55555")


def chunks(l, n):
    """Yield successive n-sized chunks from l."""
    for i in range(0, len(l), n):
        yield l[i:i + n]

def dense_to_one_hot(labels_dense, num_classes=10):
    """Convert class labels from scalars to one-hot vectors."""
    num_labels = labels_dense.shape[0]
    index_offset = numpy.arange(num_labels) * num_classes
    labels_one_hot = numpy.zeros((num_labels, num_classes))
    labels_one_hot.flat[index_offset + labels_dense.ravel()] = 1
    return labels_one_hot


# Parameters
learning_rate = 0.001
batch_size = 50
n_hidden_1 = 9000 # 1st layer num features
n_hidden_2 = 256 # 2nd layer num features
n_input = 60*50*3 # MNIST data input (img shape: 28*28)
n_classes = 6 # MNIST total classes (0-9 digits)

# tf Graph input
x = tf.placeholder(tf.float32, [None, 60*50, 3])
y = tf.placeholder(tf.float32, [None, n_classes])
keep_prob = tf.placeholder(tf.float32)



# Create model
def multilayer_perceptron(_X, _weights, _biases, _keep_prob):
    _X = tf.reshape(_X, [-1, n_input])
    layer_1 = tf.nn.relu(tf.add(tf.matmul(_X, _weights['h1']), _biases['b1'])) #Hidden layer with RELU activation
    dropout_layer = tf.nn.dropout(layer_1, _keep_prob)
    layer_2 = tf.nn.relu(tf.add(tf.matmul(layer_1, _weights['h2']), _biases['b2'])) #Hidden layer with RELU activation
    return tf.matmul(layer_2, _weights['out']) + _biases['out']

# Store layers weight & bias
weights = {
    'h1': tf.Variable(tf.random_normal([n_input, n_hidden_1])),
    'h2': tf.Variable(tf.random_normal([n_hidden_1, n_hidden_2])),
    'out': tf.Variable(tf.random_normal([n_hidden_2, n_classes]))
}
biases = {
    'b1': tf.Variable(tf.random_normal([n_hidden_1])),
    'b2': tf.Variable(tf.random_normal([n_hidden_2])),
    'out': tf.Variable(tf.random_normal([n_classes]))
}

# Construct model
pred = multilayer_perceptron(x, weights, biases, keep_prob)
prediction = tf.argmax(pred,1)

# Filler structures
feature_filler = np.ndarray(
    shape=(1, 60*50, 3), dtype=np.float32)
label_filler = np.ndarray(
    shape=(1, n_classes), dtype=np.float32)


sess = tf.Session()

print("Loading, please wait ...")
#new_saver = tf.train.import_meta_graph('model.ckpt.meta')
#new_saver.restore(sess, tf.train.latest_checkpoint('./'))

saver = tf.train.Saver()
ckpt = tf.train.get_checkpoint_state('./')
saver.restore(sess, ckpt.model_checkpoint_path)
print("RecoServer ready on tcp://*:55555")

i = 0
event = neuland.Event()
start = timer()
while True:
    #  Wait for next request from client
    message = socket.recv()
    #print("Received request: %s" % message)
    feature_filler.fill(0.)
    label_filler.fill(0.)
    event.ParseFromString(message)
    label_filler[0][len(event.neutrons)] = 1
    for digi in event.digis:
        feature_filler[0][digi.id-1] = [digi.e, digi.tl, digi.tr]

    predictions_val = sess.run(prediction, feed_dict={x: feature_filler, y: label_filler, keep_prob: 1})
    #print(labels_val)
    #print(predictions_val)
    #print("---")
    #print(predictions_val[0])
    #  Send reply back to client
    socket.send_string(str(predictions_val[0]))

    i += 1
    if i % 100 == 0:
        print("%f Events/s" % (100./(timer()-start)))
        start = timer()
        #print('.', end="", flush=True)
