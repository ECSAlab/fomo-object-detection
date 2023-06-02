#!/bin/env python
# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# This variable lists symbols to define to the C preprocessor
export DEFINES := PLACEHOLDER

# Uncomment this line to use software defined CFU
# DEFINES += CFU_SOFTWARE_DEFINED

# Uncomment to include pdti8 in built binary
#DEFINES += INCLUDE_MODEL_PDTI8
#DEFINES += INCLUDE_MODEL_MNV2

DEFINES += INCLUDE_MODEL_FOMO
# This variable lists symbols to define to the C preprocessor
#DEFINES += TF_LITE_SHOW_MEMORY_USE
DEFINES += HIDE_PROGRESS_DOTS
#DEFINES += SHOW_CONV_PARAMS
#DEFINES += SHOW_DEPTHWISECONV_PARAMS
#DEFINES += SHOW_CONV_PERF

# CONV_2D
# Uncommnet one line to use software acceleration 
# or two line to use software && hardware acceleration 
DEFINES += ACCEL_CONV
DEFINES += ACCEL_CONV_HW


# DEPTHWISE_CONV_2D
# Uncommnet one line to use software acceleration 
# or two line to use software && hardware acceleration 
DEFINES += ACCEL_DEPTHWISECONV
DEFINES += ACCEL_DEPTHWISECONV_HW

#DEFINES += PERF_POWER
# Uncomment this line to skip debug code (large effect on performance)
DEFINES += NDEBUG

#export UART_SPEED=115200
#export TTY=/dev/ttyUSB1
# Uncomment this line to skip individual profiling output (has minor effect on performance).
#DEFINES += NPROFILE



export EXTRA_LITEX_ARGS+='--cpu-variant=full+cfu'

include ../proj.mk
