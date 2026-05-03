#include <string.h>
#include <unity.h>

#define private public
#include "domain/UiStatus.h"
#undef private

namespace {

void test_setters_truncate_long_text_and_keep_null_termination() {
  UiStatus status;
  const char* longSsid = "abcdefghijklmnopqrstuvwxyz0123456789";
  const char* longIp = "192.168.100.200-extra";

  status.setSsid(longSsid);
  status.setIp(longIp);

  TEST_ASSERT_EQUAL_UINT(UiStatus::SSID_CAPACITY - 1, strlen(status.ssid));
  TEST_ASSERT_EQUAL_CHAR('\0', status.ssid[UiStatus::SSID_CAPACITY - 1]);
  TEST_ASSERT_EQUAL_UINT(UiStatus::IP_CAPACITY - 1, strlen(status.ip));
  TEST_ASSERT_EQUAL_CHAR('\0', status.ip[UiStatus::IP_CAPACITY - 1]);
}

void test_setters_accept_null_and_clear_target() {
  UiStatus status;
  status.setSsid("SimHub");
  status.setIp("192.168.0.10");

  status.setSsid(nullptr);
  status.setIp(nullptr);

  TEST_ASSERT_EQUAL_STRING("", status.ssid);
  TEST_ASSERT_EQUAL_STRING("", status.ip);
}

void test_copy_text_ignores_null_target_and_zero_capacity() {
  char buffer[8] = "keep";

  UiStatus::copyText(nullptr, sizeof(buffer), "ignored");
  UiStatus::copyText(buffer, 0, "ignored");

  TEST_ASSERT_EQUAL_STRING("keep", buffer);
}

}  // namespace

void setUp() {}
void tearDown() {}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;
  UNITY_BEGIN();
  RUN_TEST(test_setters_truncate_long_text_and_keep_null_termination);
  RUN_TEST(test_setters_accept_null_and_clear_target);
  RUN_TEST(test_copy_text_ignores_null_target_and_zero_capacity);
  return UNITY_END();
}
