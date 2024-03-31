#pragma once

#include <gtest/gtest.h>

#include "Configuration.hpp"
#include "Configuration/Directive/Block/Main.hpp"

typedef void  (*ConstructionFunc)(directive::MainBlock&);

void  DoNothing(directive::MainBlock&);
void  Config1(directive::MainBlock& config);
void  Config2(directive::MainBlock& config);
void  Config3(directive::MainBlock& config);

class TestConfiguration1 : public ::testing::Test
{
protected:
  directive::MainBlock* main_block_;
  
  void SetUp() override
  {
    main_block_ = new directive::MainBlock();
    Config1(*main_block_);
  }
};
