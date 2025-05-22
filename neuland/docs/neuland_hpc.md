# NeuLAND Program with HPC Clusters {#NEULANDHPC}

HPC clusters consist of an enormous number of computation cores (CPUs), that can be utilized as a whole during the execution of computing heavy programs. To put it in a simple way, HPC clusters can be thought of as a large number of "computers" connecting with each other and sharing the computation payload through the connections. Each "computer" is called a _node_ and each node possesses a certain number of processors.

Simulations related to the NeuLAND detectors usually takes a significantly amount of times due to the heavy computations of the particle interactions from the Geant4 simulation framework. They could take days or even weeks if more than a million events need to be simulated. The solution to reduce the computation time is to run the simulation in the HPC clusters such that each core runs only a fraction of the total events independently and simultaneously. For example, if the simulation is run with 20 nodes and each node uses 50 cores, the real simulation time would be 1000 ( = 20 x 50 ) times shorter.

[TOC]

## How to do

Here are the steps to run the NeuLAND cli application in HPC clusters:

1. **Login to a submit node:**

   ```shell
   ssh username@virgo.hpc.gsi.de
   ```

   See [this subsection below](#SubmitNode) about how to get access to the HPC submit node.

2. **Download NeuLAND Apptainer image in any folder _under_ `/lustre`:**

   Before pulling the image, please make sure Sylabs has been added to the remote if it has not been done before:

   ```shell
   apptainer remote add --no-login SylabsCloud cloud.sycloud.io
   ```

   Then pull the image to your current folder:

   ```shell
   apptainer pull -F neuland library://yanzhaow/r3bdev/neuland:latest
   ```

   This could take few minutes if you have a slow internet. See [this section below](#NeulandApptainer) for more details about Apptainer images.

3. **Create a submit script:**
   The file should contain the following content:

   ```bash
   #!/bin/bash
   #SBATCH --nodes=[number of nodes]
   #SBATCH --ntasks-per-node=[number of cores per node]
   #SBATCH --account=[slurm account name]
   #SBATCH --job-name=[job name]
   #SBATCH --output=[STDOUT output text file name]
   #SBATCH --chdir=[path to working directory]

   srun [path to image dir]/neuland sim -c neuland_sim_config.json
   ```

   The location of the submit script as well as all the paths specified inside the script must all under the folder `/lustre`. See [this section below](#SubmitScript) for the explanations of each option.

4. **Submit the task:**

   ```bash
   sbatch -p [partition] submit_script.txt
   ```

   The `[partition]` option could be either `debug`, `main`, `long` or `new`.

5. **Check the status of the running task:**

   ```bash
   squeue --me
   ```

## NeuLAND program as an Apptainer image {#NeulandApptainer}

> [!caution]
> The usage of NeuLAND Apptainer image requires the software `apptainer` already installed in the server. If not, please contact and ask the IT department to install the software.

An Apptainer image could be thought of as a bundle which contains the program and everything that program needs, such as the operating system, compilers and third party libraries. The operating system used in the NeuLAND Apptainer image is [Fedora 41](https://hub.docker.com/_/fedora) with `gcc14` as the main C++ compiler. Here is the list of version information of the contained compilers and libraries:

- `gcc`: 14.2
- `FairSoft`: jan24p4
- `FairRoot`: `dev` branch
- `ucesb`: `dev` branch

The `dev` branch from the above list contains the latest commit up to the time when the container was built.

### How the image is built

The build processes of the NeuLAND Apptainer image can be summarized in the following steps:

1. Build the docker image `yanzhaowang/r3bdev:fedora41`, containing the compiler and `FairSoft`, using the [Fedora 41 base image](https://hub.docker.com/_/fedora/tags). The build script (i.e. `Dockerfile`) can be found [here](https://github.com/YanzhaoW/Dockerfiles/blob/master/r3bdev/fedora41/Dockerfile).
2. Build the docker image `yanzhaowang/r3bdev:r3broot`, which contains the `dev` version of `FairRoot` and the `edwin_dev` version of `R3BRoot`, using the previous `r3bdev:fedora41` as the base image. Its build script can be found in this [webpage](https://github.com/YanzhaoW/R3BRoot/blob/edwin_dev/util/container/Dockerfile).
3. Build the Apptainer image `yanzhaow/r3bdev/neuland:latest`, which specifies the execution script of the image, using the previous docker image `r3bdev:r3broot` as the base image. The Apptainer build script can be found from the file, [neuland.def](https://github.com/YanzhaoW/R3BRoot/blob/edwin_dev/util/container/neuland.def).

The step 2 and 3 are _automatically done_ by the [this CI/CD workflow](https://github.com/YanzhaoW/R3BRoot/blob/edwin_dev/.github/workflows/container_deploy.yml) whenever a new commit is pushed to `edwin_dev` branch. Both the docker images, `r3bdev:fedora` and `r3bdev:r3broot`, can be found in [this dockerhub repo](https://hub.docker.com/r/yanzhaowang/r3bdev/tags) and the Apptainer image can be found in [this sylabs repo](https://cloud.sylabs.io/library/yanzhaow/r3bdev/neuland).

### Testing the validity of the image

To test whether the image still works, first download the image if not yet done:

```shell
apptainer pull -F neuland library://yanzhaow/r3bdev/neuland:latest
```

then run a simple simulation like:

```shell
./neuland sim
```

## HPC submit node and Slurm {#SubmitNode}

HPC clusters have some special nodes that are only used to submit the tasks from users. These nodes are called "submit nodes". Please visit [this website](https://hpc.gsi.de/virgo/user-guide/access/submit-nodes.html) to check all the available nodes in GSI. All the actions, such as submitting a task and querying the status are done with a software called _Slurm_. Available commands from the Slurm can be found in the [official Slurm documentation](https://slurm.schedmd.com/documentation.html).

### Registration

To get access to the submit node, users have to complete the registration using this [link](https://git.gsi.de/hpc/cluster/access-request/-/issues/new?issuable_template=slurm_user_request&issue%5Bconfidential%5D=true&issue%5Btitle%5D=Request+User+Access) (GSI Web account is acquired). During the registration, please provide the following information:

- **Linux group**: land
- **Collaboration/Experiment/Department**: r3b
- **Slurm account name**: r3b
- **Slurm account coordinator**: Spokesperson's name

### Login to a submit node

> [!NOTE]
> All available HPC submit nodes, such as `virgo.hpc.gsi.de` are behind the GSI network firewall and can only be accessed from other servers (as a jump server, e.g. `lx-pool.gsi.de`) in the GSI network.

To login with a jump server:

```shell
ssh -J username@lx-pool.gsi.de username@virgo.hpc.gsi.de
```

A simpler way to login to submit node is to add the following configuration to `~/.ssh/config` file (please create one if not existed):

```text
Host gsigate
    HostName lx-pool.gsi.de
    User username

Host gsihpc
    HostName virgo.hpc.gsi.de
    User username
    ProxyJump gsigate
    ForwardAgent no
```

Then, login can be simply done with:

```shell
ssh gsihpc
```

### Submit script {#SubmitScript}

The submit script specifies the configuration information needed to run a task, such as the number of nodes or account information and the execution command that launches the program. Each configuration must starts with `#SBATCH`, followed by an option and its value:

```text
  #!/bin/bash
  #SBATCH --nodes=[number]
  #SBATCH --ntasks-per-node=[number]
  #SBATCH --account=[string]
  #SBATCH --job-name=[string]
  #SBATCH --output=[string]
  #SBATCH --chdir=[string]
```

The meanings of these options are:

- `--nodes`: The number of nodes for the task.
- `--ntasks-per-node`: The number of cores used in the each node.
- `--account`: The Slurm account used in the registration.
- `--job-name`: The name of your task (job).
- `--output`: The name the output file, which contains all `STDOUT` prints from the program.
- `--chdir`: The path to the _working directory_ when the task is run in HPC clusters.

All other Slurm options can be found in its [official documentation website](https://slurm.schedmd.com/sbatch.html).

After the specification of options, user needs to specify the execution command to run the program:

```bash
  srun [path to image dir]/neuland sim -c neuland_sim_config.json
```

Again, all the files and folders mentioned above must be under `/lustre` as it's the only file partition that is mounted to the GSI Cluster nodes.

> [!important]
> To run the NeuLAND program simultaneously and independently in each core, `enable-mpi` from the [general JSON configuration](#GeneralJsonConfig) must be true.

### Job status

The command

```bash
squeue --me
```

returns the information of the running jobs belonging to you. The `ST` and `NODELIST(REASON)` columns indicate the status of the job and the "reason" why job is at the corresponding status.

The status of job can be one of:

| Status     | code | Explanation                                                           |
| :--------- | :--: | :-------------------------------------------------------------------- |
| COMPLETED  | `CD` | The job has completed successfully.                                   |
| COMPLETING | `CG` | The job is finishing but some processes are still active.             |
| FAILED     | `F`  | The job terminated with a non-zero exit code and failed to execute.   |
| PENDING    | `PD` | The job is waiting for resource allocation. It will eventually run.   |
| PREEMPTED  | `PR` | The job was terminated because of preemption by another job.          |
| RUNNING    | `R`  | The job currently is allocated to a node and is running.              |
| SUSPENDED  | `S`  | A running job has been stopped with its cores released to other jobs. |
| STOPPED    | `ST` | A running job has been stopped with its cores retained.               |

Job reason code could be one of:

| Reason Code               | Explanation                                                                                     |
| :------------------------ | :---------------------------------------------------------------------------------------------- |
| `Priority`                | One or more higher priority jobs is in queue for running. Your job will eventually run.         |
| `Dependency`              | This job is waiting for a dependent job to complete and will run afterward.                     |
| `Resources`               | The job is waiting for resources to become available and will eventually run.                   |
| `InvalidAccount`          | The job’s account is invalid. Cancel the job and rerun with the correct account.                |
| `InvalidQoS`              | The job’s QoS is invalid. Cancel the job and rerun with the correct account.                    |
| `QOSGrpCpuLimit`          | All CPUs assigned to your job’s specified QoS are in use; the job will run eventually.          |
| `QOSGrpMaxJobsLimit`      | Maximum number of jobs for your job’s QoS have been met; the job will run eventually.           |
| `QOSGrpNodeLimit`         | All nodes assigned to your job’s specified QoS are in use; the job will run eventually.         |
| `PartitionCpuLimit`       | All CPUs assigned to your job’s specified partition are in use; the job will run eventually.    |
| `PartitionMaxJobsLimit`   | Maximum number of jobs for your job’s partition have been met; the job will run eventually.     |
| `PartitionNodeLimit`      | All nodes assigned to your job’s specified partition are in use; the job will run eventually.   |
| `AssociationCpuLimit`     | All CPUs assigned to your job’s specified association are in use; the job will run eventually.  |
| `AssociationMaxJobsLimit` | Maximum number of jobs for your job’s association have been met; the job will run eventually.   |
| `AssociationNodeLimit`    | All nodes assigned to your job’s specified association are in use; the job will run eventually. |

_note: Tables above are copied from [this website](https://curc.readthedocs.io/en/latest/running-jobs/squeue-status-codes.html)._

## Example

TO be added ...
