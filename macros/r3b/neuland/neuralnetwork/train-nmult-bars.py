from __future__ import print_function
import tensorflow as tf
import numpy as np
import stream
import neuland_pb2 as neuland
import itertools


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
n_input = 12*2*50*3
n_classes = 6

# tf Graph input
x = tf.placeholder(tf.float32, [None, n_input//3, 3])
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

# Define loss and optimizer
cost = tf.reduce_mean(tf.nn.softmax_cross_entropy_with_logits(pred, y)) # Softmax loss
optimizer = tf.train.AdamOptimizer(learning_rate=learning_rate).minimize(cost) # Adam Optimizer

# Calculate accuracy
correct_prediction = tf.equal(tf.argmax(pred, 1), tf.argmax(y, 1))
accuracy = tf.reduce_mean(tf.cast(correct_prediction, "float"))

# Filler structures
feature_filler = np.ndarray(
    shape=(batch_size, n_input//3, 3), dtype=np.float32)
label_filler = np.ndarray(
    shape=(batch_size, n_classes), dtype=np.float32)

# Add ops to save and restore all the variables.
saver = tf.train.Saver()

# Initializing the variables
sess = tf.Session()
sess.run(tf.global_variables_initializer())


# Read Data
print("Reading train data ...")
istream = stream.open("output/train.gam", "rb")
dataarray = []
for data in istream:
    dataarray.append(data)
istream.close()
print("... read %d events" % len(dataarray))
np.random.shuffle(dataarray)
np.set_printoptions(linewidth=200)


# Train
event = neuland.Event()
for iteration in range(1,20,1):
    for n, batch in enumerate(chunks(dataarray, batch_size)):
        feature_filler.fill(0.)
        label_filler.fill(0.)
        for m, data in enumerate(batch):
            event.ParseFromString(data)
            label_filler[m][len(event.neutrons)] = 1
            for digi in event.digis:
                feature_filler[m][digi.id-1] = [digi.e, digi.tl, digi.tr]

        sess.run([optimizer], feed_dict={x: feature_filler, y: label_filler, keep_prob: 0.7})
        # labels_val, predictions_val = sess.run([tf.argmax(y,1), tf.argmax(pred,1)], feed_dict={x: feature_filler, y: label_filler, keep_prob: 0.7})
        #print(labels_val)
        #print(predictions_val)
        #print("---")

        if (n % 100 == 0): # % 100
            # Calculate batch accuracy
            acc, loss = sess.run([accuracy, cost], feed_dict={x: feature_filler, y: label_filler, keep_prob: 0.7})
            # Calculate batch loss
            print("i " + str(iteration) + " - nb " + str(n*batch_size) + ", Minibatch Loss= " + \
                "{:.6f}".format(loss) + ", Training Accuracy= " + \
                "{:.5f}".format(acc))

        if (n % 500 == 0):
            print("Saving ...")
            save_path = saver.save(sess, "model.ckpt")
            print("Saved Training to %s" % save_path)

print("Saving ...")
save_path = saver.save(sess, "model.ckpt")
print("Saved Training to %s" % save_path)
