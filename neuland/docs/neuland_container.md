# NeuLAND Program With Containers {#neuland_container}

Most of low/middle level data analysis relating to NeuLAND detector are done in the program R3BRoot, which requires many third-party libraries and tool-kits, such as FairSoft, FairRoot and UCESB etc. These software, on the other hand, requires newer versions of compilers and even operating systems. Therefore, to compile the R3BRoot oneself is an insurmountable task and may take quite lot of unnecessary times. To solve this problem and relief some burdens of high level data analysts, several containers containing the pre-built R3BRoot program are available. One only has to download the container and run the program with the data in your local machine or servers. Since containers doesn't care about your local operating system, you could use containers to run R3BRoot in Linux, MacOS and even Windows.

[TOC]

## Run R3BRoot in containers

### In GSI servers

If you have access to GSI Linux server, there are some powerful server nodes, namely `lustre.hpc.gsi.de`, with the data storage cluster mounted in `/lustre` folder.

1. To login, simply do (you have to be inside GSI network first)

   ```bash
   ssh username@lustre.hpc.gsi.de
   ```

2. Go to your personal data folder under `/lustre`. If you don't have it, please contact your colleagues to create one for you.

   ```bash
   cd /lustre/r3b/username
   ```

3. Download and build the Apptainer container in one of the folders, e.g. `containers`

   ```bash
   cd containers

   apptainer build -F r3broot docker://yanzhaowang/r3bdev:r3broot
   ```

   Depending on the downloading speed, it may take several minutes. You may use other name instead of `r3broot` after the `-F` flag.

4. Once downloaded, you could treat the download file `r3broot` as an executable, similar to `neuland` %CLI (see @ref neuland_exe):

   ```bash
   ./r3broot neuland sim
   ```

   or if you want to run a %ROOT macro file, you could use `macro` option:

   ```bash
   ./r3broot macro your_macro.C
   ```
   or enter the shell environment of the container:

   ```bash
   apptainer shell ./neuland
   source /opt/R3BRoot/build/config.sh
   root -l -q your_macro.C
   ```

   As a high level data analyst, you most likely need to run jupyterlab in a conda environment (make sure you have conda environment already setup):

   ```bash
   apptainer shell ./neuland

   conda activate r3b
   jupyter lab --port 10000
   ```

**Further remarks:**

- Please regularly check the update from the tag `r3broot` of [this image repository website](https://hub.docker.com/r/yanzhaowang/r3bdev). If there is a new version (look at the "last pushed"), please repeat the first step.

- The R3BRoot version pre-built in the container may not be the latest `dev` branch. If you need latest features, please open [a new issue](https://github.com/YanzhaoW/R3BRoot/issues) in the Github website.

- All the new files and changes you have created inside the _Apptainer_ container will also be available outside the container. They will also not be deleted if the whole container is deleted. (Be careful! This is not the case for Docker containers!)

### In your laptop or PC

You could [install the Apptainer software](https://apptainer.org/docs/admin/main/installation.html) in your PC and do the same things as in GSI servers. But it's highly recommended to use Docker containers as they are more reliable and have more features. The following steps show how to run the R3BRoot program in docker container:

1. Install Docker Engine from your system package manager (e.g. `dnf`, `apt`) according to [this website](https://docs.docker.com/engine/install/). But if you are using Windows or MacOS, the best solution is to install [Docker Desktop](https://docs.docker.com/desktop/setup/install/mac-install/).

2. If your operating system supports CLI (such as MacOS and Linux), pull the latest R3BRoot image from the [dockerhub](https://hub.docker.com/r/yanzhaowang/r3bdev/tags). To create the container, one way is to use `docker run`:

   If your computer is using x86_64 architecture (Most of Windows PCs), you could do:

   ```bash
   docker run -it -v .:/root/test:rw --name r3b yanzhaowang/r3bdev:r3broot
   ```

   If your computer is using ARM architecture (MacBook with Apple Silicon), you should use:

   ```bash
   docker run -it -v .:/root/test:rw --name r3b yanzhaowang/r3bdev:r3broot-arm
   ```

   Further remarks about running a container:

   - You can use whatever name you want instead of "r3b".
   - If you want to go back to your local system, you could either do `exit` or press `Ctrl-p` and `Ctrl-q`. The former stops the container and the latter still keeps the container running. To go back to the running container, simply do `docker attach r3b`. To start and run the stopped container, do `docker start r3b`.
   - The option behind the flag `-v` specifies the shared (mounted) folders between your local(host) machine and the container. The first path, e.g. current folder `.`, is in your local machine. The second path is the folder in the container. The third option `rw` represents read and write. You could use `ro` for read-only.
   - If you need to share other directories after the container is created, you have to delete the container and recreate it. **Deleting a container will wipe out all the data inside except those in the shared folders.**

3. Run the program:

   ```bash
   neuland sim
   ```

   or

   ```bash
   root -l -q your_macro.C
   ```

> [!important]
> If you are mounting a folder from MacOS, please make sure the folder is **case-sensitive**. Be default, files and folders used in MacOS are **case-insensitive**. To create a case-sensitive folder in MacOS, please check [this blog](https://brianboyko.medium.com/a-case-sensitive-src-folder-for-mac-programmers-176cc82a3830).


## Developing R3BRoot in the container

Developing the R3BRoot program can also be done inside the container. Again there are two containers for x86_64 and ARM machines, which include almost all compilers, software and third party libraries needed to compile R3BRoot.

> [!note]
> upexp is not installed in the container as it's badly designed and completely chaotic. If you need to use experimental data, use those already mapped data in .root files.

### In GSI servers

Choose any GSI servers (e.g. `lustre.hpc.gsi.de`) which have `apptainer` available. Then create a Apptainer container with:

```bash
apptainer build --fix-perms fedora.sif docker://yanzhaowang/r3bdev:fedora-latest
```

in a folder under `/lustre`

Then enter the shell environment with:

```bash
apptainer shell --bind /lustre,/u fedora.sif
```

if you don't want to do run this command every time, you could put it in your `.bashrc` file. If you would like to run the container as a `sudo`, use the `--fakeroot` option a-case-sensitive-src-folder-for-mac-programmers-176cc82a3830

```bash
apptainer shell --fakeroot --bind /lustre,/u --overlay some_dir.overlay fedora.sif
```

The `fakeroot` option allows a user to install any package from the system package manager, like `apt` or `dnf`. But be aware the `fakeroot` option makes the container slow. For more details, please refer to this [gist](https://gist.github.com/YanzhaoW/7b8fc17bf4da854bd47f1f0757e8ff56).

### In your laptop or PC

To create the container, another easier way is to use `docker compose` and ssh into the container. So instead of writing out a long command to specify mounting, binding and port forwarding, a single command would do the trick:

```bash
docker compose up -d
```

But before running this command, several things should be done first:

1. Create a `docker-compose.yaml` file in any folder (the filename should not be changed) which contains:

   ```yaml
   services:
     r3broot:
       image: yanzhaowang/r3bdev:fedora-latest-arm
       hostname: r3b-docker
       volumes:
         - ${HOME}/.zshenv:/home/admin/.zshenv:ro # Optional. Load your local zsh envs
         - ${HOME}/.config/zsh:/home/admin/.config/zsh:ro # Optional. Load your local zsh config
         - ${HOME}/.config/tmux:/home/admin/.config/tmux:ro # Optional. Load your local tmux config
         - ${HOME}/Docker/r3bdev/r3b:/home/admin/:rw # Required. Home folder to host data
         - ${HOME}/Docker/r3bdev/system_ssh:/etc/ssh:rw # Required.
       ports:
         - "3100:22" # You can change 3100 to other port number as you like
       command:
         - /bin/bash
         - -c
         - |
           useradd -m admin
           echo "admin:admin" | chpasswd
           usermod -s /usr/bin/zsh admin
           /usr/sbin/sshd -D
   ```

   _Further remarks:_

   - Here you specifies the folder mounting under the `volumes` section.
   - The port number `3100` can be changed to other port number larger than 1024. This number will be used for ssh login.
   - The non-root user "admin" also has the password "admin". If you want a difference name for the non-root account, please change it both in the `command` and the `volumes` section.

2. Create the mounted local folders (if not existed):

   ```bash
   mkdir -p ${HOME}/Docker/r3bdev/r3b ${HOME}/Docker/r3bdev/system_ssh
   ```

   Again you could use other folders as well.

3. Run the `docker compose up -d` in the folder that contains the `docker-compose.yaml` file.

4. SSH into the container with:

   ```bash
   ssh admin@localhost -p 3100
   ```

   The initial password for the login is set in the `docker-compose.yaml` file. You could change it by using the command `chpasswd` after the login. But the best way is to send your local ssh key to the container such that password is not needed for the login. You could also login with root account with the password "root".

   If you don't want to always remember the port number, add the following config in `~/.ssh/config` file (create it if not existed):

   ```text
   Host r3b
   HostName localhost
   User admin
   Port 3100
   ForwardX11 yes
   ```

   then just do

   ```bash
   ssh r3b
   ```

### Conda environment

It's helpful to develop the program in a conda environment, where the python version is matched well with the %ROOT version. The `r3broot` image already contains some convenient command for this situation.

To initialize the conda environment and download all the packages, do

```bash
./r3broot init-conda
```

This command will install mini-conda folder `~/miniconda3` in your home folder and create an conda environment named `r3bdev`. This step need to be done only once.

Next, to open a jupyter lab server, simply do:

```
./r3broot jupyterlab --port ${ANY_PORT_NUMBER}
```


## Creation of containers (for developers)

### Developing environment

### R3BRoot
