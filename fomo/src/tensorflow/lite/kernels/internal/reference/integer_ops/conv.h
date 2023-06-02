/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/
#ifndef TENSORFLOW_LITE_KERNELS_INTERNAL_REFERENCE_INTEGER_OPS_CONV_H_
#define TENSORFLOW_LITE_KERNELS_INTERNAL_REFERENCE_INTEGER_OPS_CONV_H_

#include <algorithm>
#include <stdio.h>
#include "customcfu.h"
#include "perf.h"
#include "tensorflow/lite/kernels/internal/common.h"
#include "tensorflow/lite/kernels/internal/portable_tensor_utils.h"

namespace tflite {
namespace reference_integer_ops {

inline void ConvPerChannelSW(
    const ConvParams& params, const int32_t* output_multiplier,
    const int32_t* output_shift, const RuntimeShape& input_shape,
    const int8_t* input_data, const RuntimeShape& filter_shape,
    const int8_t* filter_data, const RuntimeShape& bias_shape,
    const int32_t* bias_data, const RuntimeShape& output_shape,
    int8_t* output_data){

    // Get parameters.
    const int32_t input_offset = params.input_offset;  // r = s(q - Z)
    const int32_t output_offset = params.output_offset;

    // Set min and max value of the output.
    const int32_t output_activation_min = params.quantized_activation_min;
    const int32_t output_activation_max = params.quantized_activation_max;

    // Consistency check.
    TFLITE_DCHECK_LE(output_activation_min, output_activation_max);
    TFLITE_DCHECK_EQ(input_shape.DimensionsCount(), 4);
    TFLITE_DCHECK_EQ(filter_shape.DimensionsCount(), 4);
    TFLITE_DCHECK_EQ(output_shape.DimensionsCount(), 4);
    
    const int input_depth = MatchingDim(input_shape, 3, filter_shape, 3);
    const int output_depth = MatchingDim(filter_shape, 0, output_shape, 3);

    if (bias_data) {
    TFLITE_DCHECK_EQ(bias_shape.FlatSize(), output_depth);
    }

    // Check dimensions of the tensors.
    const int filter_height = filter_shape.Dims(1);
    const int filter_width = filter_shape.Dims(2);
    const int output_height = output_shape.Dims(1);
    const int output_width = output_shape.Dims(2);
 
    if ( input_depth % 8 == 0 )
    {
        for (int h = 0; h < output_height; ++h)  
        {
            for (int w = 0; w < output_width; ++w) 
            {
                for (int d = 0; d < output_depth; ++d) 
                {
                    int32_t acc = 0;
                   
                    #ifdef SHOW_CONV_PERF 
                    perf_enable_counter(0); 
                    #endif
                    for (int in_channel = 0; in_channel < input_depth; in_channel += 8) 
                    {
                        int32_t input_val  = input_data[Offset(input_shape, 0, h, w,in_channel)]; 
                        int32_t filter_val = filter_data[Offset(filter_shape, d, 0, 0, in_channel)];
                        acc += filter_val * (input_val + input_offset);
                        
                        input_val  = input_data[Offset(input_shape, 0, h, w,in_channel + 1)];
                        filter_val = filter_data[Offset(filter_shape, d, 0, 0, in_channel + 1)];
                        acc += filter_val * (input_val + input_offset);
                        input_val  = input_data[Offset(input_shape, 0, h, w,in_channel + 2)];
                        filter_val = filter_data[Offset(filter_shape, d, 0, 0, in_channel + 2)];
                        acc += filter_val * (input_val + input_offset);
                        input_val  = input_data[Offset(input_shape, 0, h, w,in_channel + 3)];
                        filter_val = filter_data[Offset(filter_shape, d, 0, 0, in_channel + 3)];
                        acc += filter_val * (input_val + input_offset);
                        input_val  = input_data[Offset(input_shape, 0, h, w,in_channel + 4)];
                        filter_val = filter_data[Offset(filter_shape, d, 0, 0, in_channel + 4)];
                        acc += filter_val * (input_val + input_offset);
                        input_val  = input_data[Offset(input_shape, 0, h, w,in_channel + 5)];
                        filter_val = filter_data[Offset(filter_shape, d, 0, 0, in_channel + 5)];
                        acc += filter_val * (input_val + input_offset);
                        input_val  = input_data[Offset(input_shape, 0, h, w,in_channel + 6)];
                        filter_val = filter_data[Offset(filter_shape, d, 0, 0, in_channel + 6)];
                        acc += filter_val * (input_val + input_offset);
                        input_val  = input_data[Offset(input_shape, 0, h, w,in_channel + 7)];
                        filter_val = filter_data[Offset(filter_shape, d, 0, 0, in_channel + 7)];
                        acc += filter_val * (input_val + input_offset);
                    }
                    #ifdef SHOW_CONV_PERF 
                    perf_disable_counter(0); 
                    #endif
                    if (bias_data) {
                        acc += bias_data[d];
                    }
                    acc = MultiplyByQuantizedMultiplier(acc, output_multiplier[d], output_shift[d]);
                    acc += output_offset;
                    acc = std::max(acc, output_activation_min);
                    acc = std::min(acc, output_activation_max);

                    output_data[Offset(output_shape, 0, h, w, d)] = static_cast<int8_t>(acc);
                }
            }
        }
    }
    else
    {
        for (int h = 0; h < output_height; ++h)  
        {
            const int in_y_origin = h * 2;
            for (int w = 0; w < output_width; ++w) 
            {
                const int in_x_origin =  w * 2;
                for (int d = 0; d < output_depth; ++d) 
                {
                    int32_t acc = 0;
                    for (int filter_y = 0; filter_y < filter_height; ++filter_y) 
                    {
                        const int in_y = in_y_origin +  filter_y;
                        for (int filter_x = 0; filter_x < filter_width; ++filter_x) 
                        {
                            const int in_x = in_x_origin + filter_x;
                    
                            #ifdef SHOW_CONV_PERF 
                            perf_enable_counter(1); 
                            #endif
                            int32_t input_val  = input_data[Offset(input_shape, 0, in_y, in_x,0 )]; 
                            int32_t filter_val = filter_data[Offset(filter_shape, d, filter_y, filter_x, 0)];
                            acc += filter_val * (input_val + input_offset);
                            #ifdef SHOW_CONV_PERF 
                            perf_disable_counter(1); 
                            #endif
                        }
                    }

                    if (bias_data) {
                        acc += bias_data[d];
                    }
                    acc = MultiplyByQuantizedMultiplier( acc, output_multiplier[d], output_shift[d]);
                    acc += output_offset;
                    acc = std::max(acc, output_activation_min);
                    acc = std::min(acc, output_activation_max);
                    output_data[Offset(output_shape, 0, h, w, d)] = static_cast<int8_t>(acc);
                }
            }
        }
    } 

return;
}

inline void ConvPerChannelHW(
    const ConvParams& params, const int32_t* output_multiplier,
    const int32_t* output_shift, const RuntimeShape& input_shape,
    const int8_t* input_data, const RuntimeShape& filter_shape,
    const int8_t* filter_data, const RuntimeShape& bias_shape,
    const int32_t* bias_data, const RuntimeShape& output_shape,
    int8_t* output_data){

    // Get parameters.
    const int32_t input_offset = params.input_offset;  // r = s(q - Z)
    const int32_t output_offset = params.output_offset;
    const int input_depth = MatchingDim(input_shape, 3, filter_shape, 3);
    const int output_depth = MatchingDim(filter_shape, 0, output_shape, 3);

    // Check dimensions of the tensors.
    const int filter_height = filter_shape.Dims(1);
    const int output_height = output_shape.Dims(1);
    const int output_width = output_shape.Dims(2);
    

    if ( input_depth % 8 == 0 )
    {
        for (int h = 0; h < output_height; ++h)  
        {
          for (int w = 0; w < output_width; ++w) 
          {
            for (int d = 0; d < output_depth; ++d) 
            {
              cfu_acc_init(input_offset,bias_data[d]);
              cfu_quant_init(output_multiplier[d], -output_shift[d]);
              
              uint32_t* in  = (uint32_t*)(input_data + Offset(input_shape, 0, h , w, 0)); 
              uint32_t* fil = (uint32_t*)(filter_data + Offset(filter_shape, d,0, 0, 0));
            
              for (int in_channel = 0; in_channel < input_depth; in_channel += 8) 
              {
                  cfu_acc_simd(*in++,*fil++);
                  cfu_acc_simd(*in++,*fil++);

              }
              output_data[Offset(output_shape, 0, h, w, d)] = static_cast<int8_t>(cfu_read(output_offset));
            }
          }
        }
    }
    else
    {
        for (int h = 0; h < output_height; ++h)  
        {
            for (int w = 0; w < output_width; ++w) 
            {
                for (int d = 0; d < output_depth; ++d) 
                {
                    cfu_acc_init(input_offset,bias_data[d]);
                    cfu_quant_init(output_multiplier[d], -output_shift[d]);

                    for (int f = 0; f < filter_height; ++f) 
                    {
                        uint8_t* in  = (uint8_t*)(input_data + Offset(input_shape, 0,  h * 2 + f, w * 2,0 ));
                        uint8_t* fil = (uint8_t*)(filter_data + Offset(filter_shape, d, f, 0, 0));
                        cfu_acc(*in++,*fil++);
                        cfu_acc(*in++,*fil++);
                        cfu_acc(*in,*fil);
                    }
                    output_data[Offset(output_shape, 0, h, w, d)] = static_cast<int8_t>(cfu_read(output_offset));

                }
            }
        }
    } 

return;
}    


// Fixed-point per-channel-quantization convolution reference kernel.
inline void ConvPerChannel(
    const ConvParams& params, const int32_t* output_multiplier,
    const int32_t* output_shift, const RuntimeShape& input_shape,
    const int8_t* input_data, const RuntimeShape& filter_shape,
    const int8_t* filter_data, const RuntimeShape& bias_shape,
    const int32_t* bias_data, const RuntimeShape& output_shape,
    int8_t* output_data) {

  #ifdef SHOW_CONV_PARAMS
  // Format is:
  // "padding_type", "padding_width", "padding_height", "padding_width_offset",
  // "padding_height_offset", "stride_width", "stride_height",
  // "dilation_width_factor", "dilation_height_factor", "input_offset",
  // "weights_offset", "output_offset", "output_multiplier", "output_shift",
  // "quantized_activation_min", "quantized_activation_max",
  // "input_batches", "input_height", "input_width", "input_depth",
  // "filter_output_depth", "filter_height", "filter_width", "filter_input_depth",
  // "output_batches", "output_height", "output_width", "output_depth",
  print_conv_params(params, input_shape, filter_shape, output_shape);
  #endif  
  #ifdef ACCEL_CONV
    #ifdef ACCEL_CONV_HW
      ConvPerChannelHW(params, output_multiplier, output_shift, input_shape,
                      input_data, filter_shape, filter_data, bias_shape,
                      bias_data, output_shape, output_data);
    #else
      ConvPerChannelSW(params, output_multiplier, output_shift, input_shape,
                      input_data, filter_shape, filter_data, bias_shape,
                      bias_data, output_shape, output_data);
    #endif 
  #else //Original ConvPerChannel code    
  // Get parameters.
  const int32_t input_offset = params.input_offset;  // r = s(q - Z)
  const int stride_width = params.stride_width;
  const int stride_height = params.stride_height;
  const int dilation_width_factor = params.dilation_width_factor;
  const int dilation_height_factor = params.dilation_height_factor;
  const int pad_width = params.padding_values.width;
  const int pad_height = params.padding_values.height;
  const int32_t output_offset = params.output_offset;

  // Set min and max value of the output.
  const int32_t output_activation_min = params.quantized_activation_min;
  const int32_t output_activation_max = params.quantized_activation_max;

  // Consistency check.
  TFLITE_DCHECK_LE(output_activation_min, output_activation_max);
  TFLITE_DCHECK_EQ(input_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(filter_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(output_shape.DimensionsCount(), 4);
  const int batches = MatchingDim(input_shape, 0, output_shape, 0);
  const int input_depth = input_shape.Dims(3);
  const int output_depth = MatchingDim(filter_shape, 0, output_shape, 3);
  if (bias_data) {
    TFLITE_DCHECK_EQ(bias_shape.FlatSize(), output_depth);
  }

  // Check dimensions of the tensors.
  const int input_height = input_shape.Dims(1);
  const int input_width = input_shape.Dims(2);
  const int filter_height = filter_shape.Dims(1);
  const int filter_width = filter_shape.Dims(2);
  const int filter_input_depth = filter_shape.Dims(3);
  const int groups = input_depth / filter_input_depth;
  TFLITE_DCHECK_EQ(input_depth % filter_input_depth, 0);
  const int filters_per_group = output_depth / groups;
  const int output_height = output_shape.Dims(1);
  const int output_width = output_shape.Dims(2);
  for (int batch = 0; batch < batches; ++batch) {
    for (int out_y = 0; out_y < output_height; ++out_y) {
      const int in_y_origin = (out_y * stride_height) - pad_height;
      for (int out_x = 0; out_x < output_width; ++out_x) {
        const int in_x_origin = (out_x * stride_width) - pad_width;
        for (int d = 0; d < output_depth; ++d) {
          auto group = d / filters_per_group;
          int32_t acc = 0;
          for (int filter_y = 0; filter_y < filter_height; ++filter_y) {
            const int in_y = in_y_origin + dilation_height_factor * filter_y;
            for (int filter_x = 0; filter_x < filter_width; ++filter_x) {
              const int in_x = in_x_origin + dilation_width_factor * filter_x;

              // Zero padding by omitting the areas outside the image.
              const bool is_point_inside_image =
                  (in_x >= 0) && (in_x < input_width) && (in_y >= 0) &&
                  (in_y < input_height);

              if (!is_point_inside_image) {
                continue;
              }

              for (int in_channel = 0; in_channel < filter_input_depth;
                   ++in_channel) {
                int32_t input_val =
                    input_data[Offset(input_shape, batch, in_y, in_x,
                                      in_channel + group * filter_input_depth)];
                int32_t filter_val = filter_data[Offset(
                    filter_shape, d, filter_y, filter_x, in_channel)];
                // Accumulate with 32 bits accumulator.
                // In the nudging process during model quantization, we force
                // real value of 0.0 be represented by a quantized value. This
                // guarantees that the input_offset is a int8_t, even though
                // it is represented using int32_t. int32_t += int8_t *
                // (int8_t - int8_t) so the highest value we can get from each
                // accumulation is [-127, 127] * ([-128, 127] -
                // [-128, 127]), which is [-32512, 32512]. log2(32512)
                // = 14.98, which means we can accumulate at least 2^16
                // multiplications without overflow. The accumulator is
                // applied to a filter so the accumulation logic will hold as
                // long as the filter size (filter_y * filter_x * in_channel)
                // does not exceed 2^16, which is the case in all the models
                // we have seen so far.
                // TODO(b/174275578): Add a check to make sure the
                // accumulator depth is smaller than 2^16.
                acc += filter_val * (input_val + input_offset);
              }
            }
          }

          if (bias_data) {
            acc += bias_data[d];
          }
          acc = MultiplyByQuantizedMultiplier(
              acc, output_multiplier[d], output_shift[d]);
          acc += output_offset;
          acc = std::max(acc, output_activation_min);
          acc = std::min(acc, output_activation_max);
          output_data[Offset(output_shape, batch, out_y, out_x, d)] =
              static_cast<int8_t>(acc);
        }
      }
    }
  }
  #endif
}

inline void ConvPerChannelWithPackedInt4Weights(
    const ConvParams& params, const int32_t* output_multiplier,
    const int32_t* output_shift, const RuntimeShape& input_shape,
    const int8_t* input_data, const RuntimeShape& filter_shape,
    const int8_t* filter_input, int8_t* unpacked_filter_data,
    const RuntimeShape& bias_shape, const int32_t* bias_data,
    const RuntimeShape& output_shape, int8_t* output_data) {
  TFLITE_DCHECK(unpacked_filter_data != nullptr);
  tflite::tensor_utils::UnpackDenseInt4IntoInt8(
      filter_input, filter_shape.FlatSize(), unpacked_filter_data);
  ConvPerChannel(params, output_multiplier, output_shift, input_shape,
                 input_data, filter_shape, unpacked_filter_data, bias_shape,
                 bias_data, output_shape, output_data);
}

// Fixed-point per-channel-quantization convolution reference kernel.
// 16-bit data and 8-bit filter
template <typename AccumScalar>
inline void ConvPerChannel(
    const ConvParams& params, const int32_t* output_multiplier,
    const int32_t* output_shift, const RuntimeShape& input_shape,
    const int16_t* input_data, const RuntimeShape& filter_shape,
    const int8_t* filter_data, const RuntimeShape& bias_shape,
    const AccumScalar* bias_data, const RuntimeShape& output_shape,
    int16_t* output_data) {
  // Get parameters.
  const int stride_width = params.stride_width;
  const int stride_height = params.stride_height;
  const int dilation_width_factor = params.dilation_width_factor;
  const int dilation_height_factor = params.dilation_height_factor;
  const int pad_width = params.padding_values.width;
  const int pad_height = params.padding_values.height;

  // Set min and max value of the output.
  const int32_t output_activation_min = params.quantized_activation_min;
  const int32_t output_activation_max = params.quantized_activation_max;

  // Consistency check.
  TFLITE_DCHECK_LE(output_activation_min, output_activation_max);
  TFLITE_DCHECK_EQ(input_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(filter_shape.DimensionsCount(), 4);
  TFLITE_DCHECK_EQ(output_shape.DimensionsCount(), 4);
  const int batches = MatchingDim(input_shape, 0, output_shape, 0);
  const int input_depth = input_shape.Dims(3);
  const int output_depth = MatchingDim(filter_shape, 0, output_shape, 3);
  if (bias_data) {
    TFLITE_DCHECK_EQ(bias_shape.FlatSize(), output_depth);
  }

  // Check dimensions of the tensors.
  const int input_height = input_shape.Dims(1);
  const int input_width = input_shape.Dims(2);
  const int filter_height = filter_shape.Dims(1);
  const int filter_width = filter_shape.Dims(2);
  const int filter_input_depth = filter_shape.Dims(3);
  const int groups = input_depth / filter_input_depth;
  TFLITE_DCHECK_EQ(input_depth % filter_input_depth, 0);
  const int filters_per_group = output_depth / groups;
  const int output_height = output_shape.Dims(1);
  const int output_width = output_shape.Dims(2);
  for (int batch = 0; batch < batches; ++batch) {
    for (int out_y = 0; out_y < output_height; ++out_y) {
      const int in_y_origin = (out_y * stride_height) - pad_height;
      for (int out_x = 0; out_x < output_width; ++out_x) {
        const int in_x_origin = (out_x * stride_width) - pad_width;
        for (int d = 0; d < output_depth; ++d) {
          auto group = d / filters_per_group;
          AccumScalar acc = 0;
          for (int filter_y = 0; filter_y < filter_height; ++filter_y) {
            const int in_y = in_y_origin + dilation_height_factor * filter_y;
            for (int filter_x = 0; filter_x < filter_width; ++filter_x) {
              const int in_x = in_x_origin + dilation_width_factor * filter_x;

              // Zero padding by omitting the areas outside the image.
              const bool is_point_inside_image =
                  (in_x >= 0) && (in_x < input_width) && (in_y >= 0) &&
                  (in_y < input_height);

              if (!is_point_inside_image) {
                continue;
              }

              for (int in_channel = 0; in_channel < filter_input_depth;
                   ++in_channel) {
                int32_t input_val =
                    input_data[Offset(input_shape, batch, in_y, in_x,
                                      in_channel + group * filter_input_depth)];
                int32_t filter_val = filter_data[Offset(
                    filter_shape, d, filter_y, filter_x, in_channel)];
                // Accumulate with 64 bits accumulator.
                // int64_t += int8_t * int16_t so the highest value we can
                // get from each accumulation is [-127, 127] * ([-32768,
                // 32767] -
                // [-32768, 32767]), which is [-8322945, 8322945].
                // log2(8322945) = 22.99.
                acc += filter_val * input_val;
              }
            }
          }
          if (bias_data) {
            acc += bias_data[d];
          }
          int32_t scaled_acc = MultiplyByQuantizedMultiplier(
              acc, output_multiplier[d], output_shift[d]);
          scaled_acc = std::max(scaled_acc, output_activation_min);
          scaled_acc = std::min(scaled_acc, output_activation_max);
          output_data[Offset(output_shape, batch, out_y, out_x, d)] =
              static_cast<int16_t>(scaled_acc);
        }
      }
    }
  }
}

}  // namespace reference_integer_ops
}  // namespace tflite

#endif  // TENSORFLOW_LITE_KERNELS_INTERNAL_REFERENCE_INTEGER_OPS_CONV_H_
