# NeuLAND Neural Network based Neutron reconstruction (WiP)

Using neural networks to help with the neutron event reconstruction!


## Requirements:
- FairSoft properly build against python 3.5

	Some tinkering might be required to get ROOT & python3 to properly compile and link together:
	- If python is/should be installed via pyenv: `env PYTHON_CONFIGURE_OPTS="--enable-shared" PYTHON_CFLAGS=-fPIC pyenv install 3.5.2`
	- In FairSoft: tools/rootconfig.sh: `PYTHONBUILD="-Dpython=ON -DPYTHON_EXECUTABLE=~/.pyenv/versions/3.5.2/bin/python -DPYTHON_INCLUDE_DIR=~/.pyenv/versions/3.5.2/include/python3.5m -DPYTHON_LIBRARY=~/.pyenv/versions/3.5.2/lib/libpython3.5m.so"`

	Note: Most scripts should work with other python versions as well. YMMV.
- ZeroMQ python bindings (`pip install zmq`) (C++ version is included in FairSoft)
- Google protobuf python bindings (`pip install protobuf`) (C++ version is included in FairSoft)
- Google tensorflow 0.12+ (see [Tensorflow pip installation](https://www.tensorflow.org/get_started/os_setup#pip_installation))


## Available scripts:

- Step 1: Simulation

- Step 2: Digitizing

- Step 3: Convert

	Convert ROOT events to a stream of protobufs, for faster I/O during neural network training. WiP

- train-nmult-bars.py

	Train a very basic neural network to classify the neutron multiplicity [0-6] of events. Inputs are energy, time left, and time right of digis. Trained network parameters are saved to model.ckpt

- recoserver-nmult-bars.py

	Host a minimal zmq server for the trained NN. Accepts protobuf events and returns the neutron multiplicity.

- recoclient-nmult-test.py

	Sends some requests to a server and print the response

- recoserver-nmult-dummy.py

	Hosts a minimal zmq sever. Accepts everything and returns 0.
