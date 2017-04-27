#ifndef DYLOCXX__TEST__TEST_BASE_H__INCLUDED
#define DYLOCXX__TEST__TEST_BASE_H__INCLUDED

#include <gtest/gtest.h>


namespace dyloc {
namespace test {

class TestBase : public ::testing::Test {

 protected:

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }
};

} // namespace test
} // namespace dyloc

#endif // DYLOCXX__TEST__TEST_BASE_H__INCLUDED
