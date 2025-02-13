/**
 * Copyright (c) Facebook, Inc. and its affiliates.
 * All rights reserved.
 *
 * This source code is licensed under the MIT-style license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include <gtest/gtest.h>

#include "flashlight/fl/contrib/modules/modules.h"
#include "flashlight/fl/flashlight.h"
#include "flashlight/lib/common/System.h"
#include "flashlight/pkg/runtime/plugin/ModulePlugin.h"

using namespace fl;

namespace {

std::string pluginDir = "";

} // namespace

TEST(ModulePluginTest, ModulePlugin) {
  const std::string libfile =
      fl::lib::pathsConcat(pluginDir, "test_module_plugin.so");

  const int ninput = 80;
  const int noutput = 10;
  const int batchsize = 4;

  // Note: the following works only if the plugin is *not* statically
  // linked against AF/FL.
  //
  // auto model = fl::pkg::runtime::ModulePlugin(libfile).arch(ninput, noutput);
  //
  // If AF/FL are linked, then there will be some issue at deallocation of
  // the plugin.  For that reason, we stick to the following conservative
  // way in this test (plugin destroyed after model).
  fl::pkg::runtime::ModulePlugin plugin(libfile);
  auto model = plugin.arch(ninput, noutput);
  auto input = fl::randn({ninput, batchsize}, fl::dtype::f32);
  auto output = model->forward({noGrad(input)}).front();
  ASSERT_EQ(output.shape(), Shape({noutput, batchsize}));
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  fl::init();

  // Resolve directory for arch
#ifdef PLUGINDIR
  pluginDir = PLUGINDIR;
#endif

  return RUN_ALL_TESTS();
}
