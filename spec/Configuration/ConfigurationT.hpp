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
public:
  TestConfiguration1()
    : main_block_(NULL), config_(8) {}
  ~TestConfiguration1() {}
protected:
  directive::MainBlock* main_block_;
  Configuration         config_;
  
  void SetUp() override
  {
    main_block_ = new directive::MainBlock();
    Config1(*main_block_);
    config_.set_main_block(main_block_);

    std::vector<const directive::Socket *> sockets = config_.all_server_sockets();
    ASSERT_EQ(sockets.size(), static_cast<size_t>(1));
    ASSERT_EQ(*sockets[0], directive::Socket("80"));
    config_.register_server_socket(3, *sockets[0]);
  }
};
