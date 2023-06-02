/*
 * Copyright 2022 The CFU-Playground Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cfu.h"

#ifndef _CUSTOMCFU_H
#define _CUSTOMCFU_H

#define cfu_dummy(a,b)        cfu_op0(0b0011000, a, b)

#define cfu_acc_init(a,b)     cfu_op0(0b0000001, a, b)
#define cfu_quant_init(a,b)   cfu_op0(0b0000010, a, b)
#define cfu_acc(a,b)          cfu_op0(0b0000011, a, b)
#define cfu_acc_simd(a,b)     cfu_op0(0b0001011, a, b)
#define cfu_read(a)           cfu_op0(0b0000100, a, 0)


#endif
