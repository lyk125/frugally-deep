// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "fdeep/layers/pooling_2d_layer.hpp"

namespace fdeep { namespace internal
{

class average_pooling_2d_layer : public pooling_2d_layer
{
public:
    explicit average_pooling_2d_layer(const std::string& name,
        const shape2& pool_size, const shape2& strides, padding p,
        bool padding_valid_uses_offset, bool padding_same_uses_offset) :
        pooling_2d_layer(name, pool_size, strides, p,
            padding_valid_uses_offset, padding_same_uses_offset)
    {
    }
protected:
    tensor3 pool(const tensor3& in_unpadded) const override
    {
        const float_type invalid = std::numeric_limits<float_type>::lowest();
        const auto input_data = preprocess_convolution(
            pool_size_, strides_, padding_, use_offset(), in_unpadded,
            invalid);

        const std::size_t strides_y = strides_.height_;
        const std::size_t strides_x = strides_.width_;
        const std::size_t offset_y = input_data.offset_y_;
        const std::size_t offset_x = input_data.offset_x_;
        const std::size_t out_height = input_data.out_height_;
        const std::size_t out_width = input_data.out_width_;
        const tensor3& in = input_data.in_padded_;

        tensor3 out(
            shape3(in_unpadded.shape().depth_, out_height, out_width), 0);

        for (std::size_t z = 0; z < out.shape().depth_; ++z)
        {
            for (std::size_t y = 0; y < out.shape().height_; ++y)
            {
                for (std::size_t x = 0; x < out.shape().width_; ++x)
                {
                    float_type val = 0;
                    std::size_t divisor = 0;
                    for (std::size_t yf = 0; yf < pool_size_.height_; ++yf)
                    {
                        for (std::size_t xf = 0; xf < pool_size_.width_; ++xf)
                        {
                            const auto current = in.get(z,
                                offset_y + strides_y * y + yf,
                                offset_x + strides_x * x + xf);
                            if (current != invalid)
                            {
                                val += current;
                                divisor += 1;
                            }
                        }
                    }
                    out.set(z, y, x, val / static_cast<float_type>(divisor));
                }
            }
        }
        return out;
    }
};

} } // namespace fdeep, namespace internal
