# Design and implementation of a compact RISC-V based Machine Learning accelerator on Low End FPGA
This repo is a placeholder for the paper submitted in [ETCEI 2023](https://conference.hetia.org/emerging-tech-conference-edge-intelligence-2023/#publications) Conference including also the instructions to deploy a model and run inference on the implemented RISC-V cpu.

# Citation
If you use this work in academic research, please cite it using the following BibTeX:
> Galetakis, M.; Kalapothas, S.; Flamis, G.; Plessas, F.; Kitsos, P. Design and implementation of a compact RISC-V based Machine Learning accelerator on Low End FPGA. ETCEI 2023. https://doi.org/10.63438/GSRA5108
```
@inproceedings{Galetakis2026-jz,
  title = {Design and implementation of a compact {RISC-V} based Machine Learning accelerator on Low End FPGA},
  booktitle = {Proceedings \& Highlights - Emerging Tech Conference Edge Intelligence 2023},
  author = {Galetakis, Manolis and Kalapothas, Stavros and Flamis, Georgios and Kitsos, Paris and Plessas, Fotis},
  url = {https://conference.hetia.org/publications/design-and-implementation-of-a-compact-risc-v-based-machine-learning-accelerator-on-low-end-fpga/},
  publisher = {Hellenic Emerging Technology Industry Association},
  pages = {158-165},
  month = oct,
  year = {2023},
  conference = {Emerging Tech Conference Edge Intelligence 2023},
  doi = {10.63438/GSRA5108}
}
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
