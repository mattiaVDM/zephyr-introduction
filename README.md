# Zephyr introduction

This repo contains Zephyr study material for begginners, based on DigiKey's [Youtube course](https://github.com/mattiaVDM/zephyr-introduction).

In addition to the course material, this repo is meant to simplify the development on Linux devices, removing the need of using a Docker container to run Zephyr.

The code has been tested on the latest Zephyr release as of now (v4.0.0).

Some documentation has also been added, with information from the course and from sources cited in the course. See the [docs](docs/) folder for additional reference material.

## Getting started

To get started, install Zephyr following the official [Getting started guide](https://docs.zephyrproject.org/4.0.0/develop/getting_started/index.html).

### Build a zephyr project from anywhere

I had problems building Zephyr projects outside of the Zephyr workspace, but it can be done by following these steps:

1. Activate the Zephyr virtual environment:

   ```bash
   source ~/path/to/your/zephyr/venv/bin/activate
   ```

2. Source the Zephyr environment script:

   ```bash
   source ~/path/to/your/zephyr/zephyr-env.sh
   ```

3. Set the `ZEPHYR_SDK_INSTALL_DIR` environment variable to point to your Zephyr SDK installation:

    ```bash
    export ZEPHYR_SDK_INSTALL_DIR=path/to/your/zephyr/sdk
    ```

To simplify this process, you can create a `.envrc` file in the root of this repository with the above commands, and then use [direnv](https://direnv.net/) to automatically load these environment variables when you enter the directory.

An example `.envrc` file is provided as `.envrc.example`. Make sure to update the paths to match your setup.
