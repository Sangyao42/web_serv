#include "./ConfigurationT.hpp"

#include <cstdlib>

#include "constants.hpp"
#include "Configuration.hpp"
#include "Configuration/Directive/Socket.hpp"
#include "Configuration/Directive/Block/Main.hpp"
#include "Configuration/Directive/Block/Http.hpp"
#include "Configuration/Directive/Block/Server.hpp"
#include "Configuration/Directive/Block/Location.hpp"
#include "Configuration/Directive/Block/Events.hpp"
#include "Configuration/Directive/Simple.hpp"
#include "Configuration/Directive/Simple/Listen.hpp"
#include "Configuration/Directive/Simple/ServerName.hpp"
#include "Configuration/Directive/Simple/Cgi.hpp"
#include "Configuration/Directive/Simple/Return.hpp"

TEST(TestConfiguration0, all_server_sockets)
{
  Configuration config(8);
  EXPECT_DEATH({
    std::vector<const directive::Socket *> sockets = config.all_server_sockets();
  }, "Assertion.*");
}

TEST(TestConfiguration0, query_no_main_block)
{
  Configuration config(8);

  ConfigurationQueryResult result = config.query(3, "hi.com", "/omg/what.html");
  ASSERT_TRUE(result.is_empty());
}

TEST(TestConfiguration0, query_empty)
{
  Configuration config(8);

  config.set_main_block(new directive::MainBlock());
  ConfigurationQueryResult result = config.query(3, "hi.com", "/omg/what.html");
  ASSERT_TRUE(result.is_empty());
}

////////////////////////////////////////////////
////////////   Tests for Config 1   ////////////
////////////////////////////////////////////////

TEST_F(TestConfiguration1, query_wrong_socket)
{
  const ConfigurationQueryResult result = config_.query(2, "hi.com", "/omg/what.html");
  ASSERT_TRUE(result.is_empty());
}

TEST_F(TestConfiguration1, query_server_block)
{
  ASSERT_EQ(config_.query_server_block(3, "hi.com"), &*main_block_->http().servers().first->second);
}

TEST_F(TestConfiguration1, query)
{
  const ConfigurationQueryResult result = config_.query(3, "hi.com", "/omg/what.html");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/omg/");
}

TEST_F(TestConfiguration1, query_use_first_server_if_no_server_name)
{
  const ConfigurationQueryResult result = config_.query(3, "", "/omg/what.html");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/omg/");
}

TEST_F(TestConfiguration1, query_use_first_server_if_wrong_server_name)
{
  const ConfigurationQueryResult result = config_.query(3, "non-existing.com", "/omg/what.html");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/omg/");
}

TEST_F(TestConfiguration1, query_correct_location_property)
{
  const ConfigurationQueryResult result = config_.query(3, "non-existing.com", "/develop/what.html");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/develop");
  ASSERT_EQ(result.location_property->server_block, &*main_block_->http().servers().first->second);
  ASSERT_EQ(result.location_property->match_path, "/develop");
  ASSERT_EQ(result.location_property->allowed_methods, constants::kDefaultAllowedMethods);
  ASSERT_EQ(result.location_property->client_max_body_size, constants::kDefaultClientMaxBodySize); // 1M
  {
    const directive::Return*  return_ = result.location_property->redirect;
    ASSERT_TRUE(return_ != NULL);
    ASSERT_EQ(return_->get(), 301);
    ASSERT_EQ(return_->get_path(), "http://www.develop.com");
  }
  ASSERT_EQ(result.location_property->cgis.size(), static_cast<size_t>(0));
  ASSERT_EQ(result.location_property->indexes.size(), static_cast<size_t>(1));
  ASSERT_EQ(result.location_property->indexes[0], &constants::kDefaultIndex);
  ASSERT_EQ(result.location_property->root, constants::kDefaultRoot);
  ASSERT_EQ(result.location_property->autoindex, true);
  ASSERT_EQ(result.location_property->mime_types, &constants::kDefaultMimeTypes);
  ASSERT_EQ(result.location_property->error_pages.size(), static_cast<size_t>(0));
  ASSERT_EQ(result.location_property->access_log, "/var/logs/access.log");
  ASSERT_EQ(result.location_property->error_log, "/var/logs/error.log");
}

TEST_F(TestConfiguration1, query_correct_location_property2)
{
  const ConfigurationQueryResult result = config_.query(3, "non-existing.com", "/omg/");
  ASSERT_FALSE(result.is_empty());

  ASSERT_EQ(result.location_block->type(), Directive::kDirectiveLocation);
  ASSERT_EQ(result.location_block->match(), "/omg/");
  ASSERT_EQ(result.location_property->server_block, &*main_block_->http().servers().first->second);
  ASSERT_EQ(result.location_property->match_path, "/omg/");
  ASSERT_EQ(result.location_property->allowed_methods, constants::kDefaultAllowedMethods);
  ASSERT_EQ(result.location_property->client_max_body_size, constants::kDefaultClientMaxBodySize); // 1M
  ASSERT_TRUE(result.location_property->redirect == NULL);
  {
    std::vector<const directive::Cgi*>*  cgis = &result.location_property->cgis;
    ASSERT_EQ(cgis->size(), static_cast<size_t>(1));
    ASSERT_EQ((*cgis)[0]->get(), "js");
    ASSERT_EQ((*cgis)[0]->extension(), "js");
    ASSERT_EQ((*cgis)[0]->cgi_path(), "/opt/homebrew/bin/node");
  }
  {
    std::vector<const directive::Index*>*  indexes = &result.location_property->indexes;
    ASSERT_EQ(indexes->size(), static_cast<size_t>(3));
    ASSERT_EQ((*indexes)[0]->get(), "index.html");
    ASSERT_EQ((*indexes)[1]->get(), "index.htm");
    ASSERT_EQ((*indexes)[2]->get(), "default.html");
  }
  ASSERT_EQ(result.location_property->root, "/var/www/omg");
  ASSERT_EQ(result.location_property->autoindex, constants::kDefaultAutoindex);
  ASSERT_EQ(result.location_property->mime_types, &constants::kDefaultMimeTypes);
  ASSERT_EQ(result.location_property->error_pages.size(), static_cast<size_t>(0));
  ASSERT_EQ(result.location_property->access_log, "/var/logs/access.log");
  ASSERT_EQ(result.location_property->error_log, "/var/logs/error.log");
}

///////////////////////////////////////////////
////////////   Test for Config 2   ////////////
///////////////////////////////////////////////

TEST_F(TestConfiguration2, query_wrong_socket)
{
  const ConfigurationQueryResult result = config_.query(2, "hi.com", "/omg/what.html");
  ASSERT_TRUE(result.is_empty());
}

TEST_F(TestConfiguration2, query_server_block)
{
  ASSERT_EQ(config_.query_server_block(3, "hi.com"), &*main_block_->http().servers().first->second);
}
