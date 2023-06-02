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

#include "models/fomo/fomo.h"

#include <stdio.h>

#include "menu.h"
#include "models/fomo/person.h"
#include "models/fomo/noperson.h"
#include "models/fomo/person2.h"
#include "models/fomo/MobileNetV2_FOMO.h"

#include "tensorflow/lite/c/common.h"

#include "tflite.h"

extern "C" {
#include "fb_util.h"
};

#define NUM_GOLDEN 2
#define INPUT_WIDTH         96
#define INPUT_LABELS        1
#define OBJECT_THRESHOLD    (float)0.5
#define OBJECT_COUNT        10
#define CATEGORIES          "person"

struct test_data {
  const unsigned char* data;
  int32_t expected;
};

struct test_data test_data_all[] = {
    {noperson, 0}, 
    {person, 1}, 
    #ifdef INCLUDE_MODEL_FOMO
    {person2, 2},
    #endif
};
typedef struct result {
    size_t x;
    size_t y;
    size_t width;
    size_t height;
    float confidence;
    //float confidence;
    const char *label;
    //int label;
} result_t;
typedef struct results {
    size_t results;
    result_t result[OBJECT_COUNT]; 
} results_t;

// Initialize everything once
// deallocate tensors when done
static void fomo_init(void) 
{
  tflite_load_model(MobileNetV2_FOMO, MobileNetV2_FOMO_len);
}

/*
 * Original Code taken from ei SDK 
 * Checks whether a new section overlaps with a result,
 * and if so, will **update the result**
 */
static bool ei_result_check_overlap(result_t *c, size_t x, size_t y, size_t width, size_t height, float confidence) 
{
    bool is_overlapping = !(c->x + c->width < x || c->y + c->height < y || c->x > x + width || c->y > y + height);
    if (!is_overlapping) return false;

    // if we overlap, but the x of the new box is lower than the x of the current box
    if (x < c->x) {
        // update x to match new box and make width larger (by the diff between the boxes)
        c->x = x;
        c->width += c->x - x;
    }
    // if we overlap, but the y of the new box is lower than the y of the current box
    if (y < c->y) {
        // update y to match new box and make height larger (by the diff between the boxes)
        c->y = y;
        c->height += c->y - y;
    }
    // if we overlap, and x+width of the new box is higher than the x+width of the current box
    if (x + width > c->x + c->width) {
        // just make the box wider
        c->width += (x + width) - (c->x + c->width);
    }
    // if we overlap, and y+height of the new box is higher than the y+height of the current box
    if (y + height > c->y + c->height) {
        // just make the box higher
        c->height += (y + height) - (c->y + c->height);
    }
    // if the new box has higher confidence, then override confidence of the whole box
    if (confidence > c->confidence) {
        c->confidence = confidence;
    }
    return true;
}
static void parse_outpout(const TfLiteTensor *output,results_t *results) 
{                                                                            

    int out_width_factor = INPUT_WIDTH / (int)output->dims->data[1];   // = 8 

    for (size_t y = 0; y < (size_t)output->dims->data[1]; y++) 
    {
     
        for (size_t x = 0; x < (size_t)output->dims->data[2]; x++) 
        {
            size_t loc = ((y * (size_t)output->dims->data[2]) + x) * (INPUT_LABELS + 1);
            //Calulate Cubes
            for (size_t ix = 1; ix < INPUT_LABELS + 1; ix++) 
            {
                int8_t v = output->data.int8[loc+ix];
                float vf = static_cast<float>(v - output->params.zero_point) * output->params.scale;
                if (vf < OBJECT_THRESHOLD)
                    continue;//}

                bool has_overlapping = false;
                int width = 1;
                int height = 1;

                for (size_t i = 0; i <  results->results;i++) 
                {
                    result_t* r = &results->result[i];
                    // not result for same class? continue
                    if (strcmp(r->label, CATEGORIES) != 0)
                      continue;
         
                    if (ei_result_check_overlap(r, x, y, width, height, vf)) 
                    {
                        has_overlapping = true;
                        break;
                    }
                }

                if (!has_overlapping) 
                {
                    result_t* r = &results->result[results->results] ;
                    r->x = x;
                    r->y = y;
                    r->width = 1;
                    r->height = 1;
                    r->confidence = vf;
                    r->label = CATEGORIES;
                    results->results++;
                }
            }          
        }
    }
    // Tranform Cubes to FOMO
    for(size_t i=0; i<results->results;i++)
    {
        result_t* r = &results->result[i];
        r->x = r->x * out_width_factor;
        r->y = r->y * out_width_factor;
        r->width = r->width * out_width_factor;
        r->height = r->height * out_width_factor ;
    }


    return;

}

// Run classification, after input has been loaded
static int32_t fomo_classify() 
{
  printf("Running fomo\n");
  tflite_classify();

  // Process the inference results.
  results_t results; 
  memset(&results,0,sizeof(results_t));
  const TfLiteTensor* output  = tflite_get_output_tensor();
  parse_outpout(output,&results); 

  for(size_t i=0; i<results.results;i++)
  {
    result_t* r = &results.result[i];
    printf("%s (%d.%.6d) [x: %d y: %d width: %d height: %d]\n",r->label,(int)r->confidence,(int)((r->confidence-(int)r->confidence)*1000000) , r->x, r->y,r->width, r->height);
  }

  return (int32_t)results.results;
}

static void do_classify_zeros() 
{
  tflite_set_input_zeros();
  int32_t result = fomo_classify();
  printf("Result is %ld\n", result);
}

static void do_classify_0() {
  tflite_set_input_unsigned(test_data_all[0].data);
  
  int32_t result = fomo_classify();
  printf("Result is %ld\n", result);

#ifdef CSR_VIDEO_FRAMEBUFFER_BASE
  char msg_buff[256] = { 0 };

  snprintf(msg_buff, sizeof(msg_buff), "Result is %ld", result);
  fb_clear();
  fb_draw_string(0,  10, 0x007FFF00, "Run test 0");
  fb_draw_buffer(0,  50, 160, 160, (const uint8_t *)test_datas[0].data, 3);
  fb_draw_string(0, 220, 0x007FFF00, (const char *)msg_buff);
  flush_cpu_dcache();
  flush_l2_cache();
#endif
}

static void do_classify_1() 
{
  tflite_set_input(test_data_all[1].data);
  
  int32_t result = fomo_classify();
  printf("Result is %ld\n", result);

#ifdef CSR_VIDEO_FRAMEBUFFER_BASE
  char msg_buff[256] = { 0 };

  snprintf(msg_buff, sizeof(msg_buff), "Result is %ld", result);
  fb_clear();
  fb_draw_string(0,  10, 0x007FFF00, "Run test 1");
  fb_draw_buffer(0,  50, 160, 160, (const uint8_t *)test_datas[1].data, 3);
  fb_draw_string(0, 220, 0x007FFF00, (const char *)msg_buff);
  flush_cpu_dcache();
  flush_l2_cache();
#endif
}
static void do_classify_2() 
{
  tflite_set_input(test_data_all[2].data);
  int32_t result = fomo_classify();
  printf("Result is %ld\n", result);

#ifdef CSR_VIDEO_FRAMEBUFFER_BASE
  char msg_buff[256] = { 0 };

  snprintf(msg_buff, sizeof(msg_buff), "Result is %ld", result);
  fb_clear();
  fb_draw_string(0,  10, 0x007FFF00, "Run test 1");
  fb_draw_buffer(0,  50, 160, 160, (const uint8_t *)test_datas[1].data, 3);
  fb_draw_string(0, 220, 0x007FFF00, (const char *)msg_buff);
  flush_cpu_dcache();
  flush_l2_cache();
#endif
}

#ifdef PERF_POWER
void test_init(void)
{
 tflite_load_model(MobileNetV2_FOMO, MobileNetV2_FOMO_len);
}
void test_classify(const unsigned char* data)
{
  static int imgs = 0;
  tflite_set_input(data);
  imgs++;
  printf("%d: ",imgs);
  tflite_classify();

  results_t results; 

  memset(&results,0,sizeof(results_t));
  const TfLiteTensor* output  = tflite_get_output_tensor();
  parse_outpout(output,&results); 
  #if 1
  for(size_t i=0; i<results.results;i++)
  {
    result_t* r = &results.result[i];
    printf("%s (%d.%.6d) [x: %d y: %d width: %d height: %d]\n",r->label,(int)r->confidence,(int)((r->confidence-(int)r->confidence)*1000000) , r->x, r->y,r->width, r->height);

  }
  #endif
}
#endif

static struct Menu MENU = {
    "Tests for fomo model",
    "fomo",
    {
        MENU_ITEM('0', "Run test no person", do_classify_0),
        MENU_ITEM('1', "Run test person", do_classify_1),
        MENU_ITEM('2', "Run test person(s)", do_classify_2),
        MENU_ITEM('z', "Run with zeros input", do_classify_zeros),
        MENU_END,
    },
};

// For integration into menu system
void fomo_menu() 
{
  fomo_init();

#ifdef CSR_VIDEO_FRAMEBUFFER_BASE
  fb_init();
  flush_cpu_dcache();
  flush_l2_cache();
#endif

  menu_run(&MENU);
}
