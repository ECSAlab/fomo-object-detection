# Design and implementation of a compact RISC-V based Machine Learning accelerator on Low End FPGA
This repo is a placeholder for the paper submitted in [ETCEI 2023](https://conference.hetia.org) Conference including also the instructions to deploy a model and run inference on the implemented RISC-V cpu.

# Citation
If you use this work in academic research, please cite it using the following BibTeX:
> {TBD} 
```
```

![](https://img.shields.io/github/last-commit/ECSAlab/fomo-object-detection?style=plastic)

# Steps to reproduce our work
## Step 1: Install CFU-Playground 
Follow the steps described in the official [setup guide](https://cfu-playground.readthedocs.io/en/latest/setup-guide.html) to install CFU-Playgorund according to your configuration preferences.
## Step 2: Install project
Copy fomo folder from this repository under [CFU-Playground]/proj/ where [CFU-Playground] is the root installation folder from step 1.
## Step 3: Build project
```shell=
cd [CFU-Playground]/proj/fomo 
make PLATFORM=sim //run simulation or 
make prog && make load //run on Digilent Arty A7 board
```
