#include "log.h"

#include <iostream>
#include <string>
#include <gtest/gtest.h>

#include "math/vec2.h"

void typesafe_sprintf_detail(size_t, std::string&) {}

template<>
void LOG(std::string f) {
	std::cout << f << std::endl;
}

void CALL_SHELL(std::string s) {
	system(s.c_str());
}

TEST(TypesafeSprintf, TypesafeSprintfSeveralTests) {
	EXPECT_EQ("1,2,3:4", typesafe_sprintf("%x,%x,%x:%x", 1, 2, 3, 4));
	EXPECT_EQ("abc,2,3:def", typesafe_sprintf("%x,%x,%x:%x", "abc", 2, 3, "def"));
	EXPECT_EQ("abc,2.55,3.14:def", typesafe_sprintf("%x,%x,%x:%x", "abc", 2.55, 3.14f, "def"));

	vec2 test(123, 412);

	EXPECT_EQ("Vector is equal to: (123,412)", typesafe_sprintf("Vector is equal to: %x", test));

	int errid = 1282;
	std::string location = "augs::window::glwindow::create";

	EXPECT_EQ("OpenGL error 1282 in augs::window::glwindow::create", typesafe_sprintf("OpenGL error %x in %x", errid, location));
}