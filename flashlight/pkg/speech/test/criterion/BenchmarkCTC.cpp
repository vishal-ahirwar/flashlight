/*
 * Copyright (c) Facebook, Inc. and its affiliates.
 *
 * This source code is licensed under the MIT-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "flashlight/fl/flashlight.h"

#include <array>
#include <iomanip>
#include <iostream>

#include "flashlight/fl/common/Timer.h"
#include "flashlight/fl/tensor/Index.h"
#include "flashlight/pkg/speech/criterion/criterion.h"

using namespace fl;
using namespace fl::pkg::speech;

int main() {
  fl::setDevice(1);
  fl::init();

  auto ctc = ConnectionistTemporalClassificationCriterion();

  int N = 30, T = 487, L = 34, B = 10;

  auto input = Variable(fl::log(fl::rand({N, T, B})), true);

  auto t = fl::abs(fl::rand({L, B}, fl::dtype::s32)).astype(fl::dtype::s32) %
      (N - 2);

  for (int i = 0; i < B; ++i) {
    int r = rand() % (L / 2);
    t(fl::range(L / 2 + r, fl::end), i) = -1;
  }

  Variable target(t, false);
  int ntimes = 50;
  Variable b = ctc.forward({input, target}).front();
  Variable gradoutput = Variable(fl::rand(b.shape()) * 2 - 2, false);
  for (int i = 0; i < 5; ++i) {
    b = ctc.forward({input, target}).front();
    b.backward();
  }
  fl::sync();
  auto s = fl::Timer::start();
  for (int i = 0; i < ntimes; ++i) {
    b = ctc.forward({input, target}).front();
    b.backward(gradoutput);
  }
  fl::sync();
  auto e = fl::Timer::stop(s);
  std::cout << "Total time (fwd+bwd pass) " << std::setprecision(5)
            << e * 1000.0 / ntimes << " msec" << std::endl;
  return 0;
}
