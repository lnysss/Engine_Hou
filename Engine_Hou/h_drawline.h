//
// Created by hyx on 2024/12/24.
//

#ifndef ENGINE_HOU_CLION_H_DRAWLINE_H
#define ENGINE_HOU_CLION_H_DRAWLINE_H

#include "h_math.h"

namespace Line2D{

    class Bresenham {
    public:
        Bresenham(const Vec2 &p1, const Vec2 &p2) : p1_(p1), p2_(p2) {
            dx_ = 2 * abs(p1.x - p2.x);
            dy_ = 2 * abs(p1.y - p2.y);
            dirx_ = p1.x < p2.x ? 1 : p1.x == p2.x ? 0 : -1;
            diry_ = p1.y < p2.y ? 1 : p1.y == p2.y ? 0 : -1;
            err_ = dx_ >= dy_ ? -dx_ / 2 : -dy_ / 2;
        }

        inline const Vec2 &CurPoint() const { return p1_; }

        inline bool IsFinished() const { return p1_ == p2_; }

        void Step() {
            if (!IsFinished()) {
                if (dx_ >= dy_) {
                    p1_.x += dirx_;
                    err_ += dy_;
                    if (err_ >= 0) {
                        p1_.y += diry_;
                        err_ -= dx_;
                    }
                } else {
                    p1_.y += diry_;
                    err_ += dx_;
                    if (err_ >= 0) {
                        p1_.x += dirx_;
                        err_ -= dy_;
                    }
                }
            }
        }

    private:
        Vec2 p1_;
        Vec2 p2_;
        int dx_;
        int dy_;
        int dirx_;
        int diry_;
        int err_;
    };


}


#endif //ENGINE_HOU_CLION_H_DRAWLINE_H
