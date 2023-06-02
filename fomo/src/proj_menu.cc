/*
 * Copyright 2021 The CFU-Playground Authors
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

#include "proj_menu.h"

#include <stdio.h>
#include <stdlib.h>

#include "customcfu.h"
#include "menu.h"
#include "perf.h"

namespace {

// Template Fn
void do_hello_world(void) { puts("Hello, World!!!\n"); }
void do_test(void) 
{ 
  puts("testreq\n"); 
}

struct Menu MENU = {
    "Project Menu",
    "project",
    {
        MENU_ITEM('h', "Say Hello", do_hello_world),
        MENU_ITEM('t', "Test", do_test),
        MENU_END,
    },
};
};  // anonymous namespace

extern "C" void do_proj_menu() { menu_run(&MENU); }