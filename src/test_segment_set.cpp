#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <iostream>
#include <type_traits>

#include "segment_set.h"

using namespace std;

TEST_CASE("segment-construction")
{
   // default constructor
   segment<int> seg1;
   REQUIRE(seg1.begin == 0);
   REQUIRE(seg1.end == 1);
   REQUIRE(seg1.value == 0);

   // constructor with arguments
   segment<int> seg2(1, 10, 5.6);
   REQUIRE(seg2.begin == 1);
   REQUIRE(seg2.end == 10);
   REQUIRE(seg2.value == 5);

   segment<int> seg22 = {1, 10, 5.7};
   REQUIRE(seg22.begin == 1);
   REQUIRE(seg22.end == 10);
   REQUIRE(seg22.value == 5);

   // copy-constructor with T != U
   segment<double> seg3(seg2);
   REQUIRE(seg3.begin == seg2.begin);
   REQUIRE(seg3.end == seg2.end);
   REQUIRE(seg3.value == seg2.value);
   REQUIRE(!is_same<decltype(seg3.value), decltype(seg2.value)>::value);

   // move copy-constructor with T != U
   segment<double> seg4(segment<int>(2, 12, 4.2));
   REQUIRE(seg4.begin == 2);
   REQUIRE(seg4.end == 12);
   REQUIRE(seg4.value == 4.0); // conversion via int

   // assignment operator with T != U
   segment<int> seg5 = seg3;
   REQUIRE(seg5.begin == seg3.begin);
   REQUIRE(seg5.end == seg3.end);
   REQUIRE(seg5.value == seg3.value);
   REQUIRE(!is_same<decltype(seg5.value), decltype(seg3.value)>::value);

   // move assignment operator with T != U
   segment<int> seg6 = segment<double>(1, 10, 5.5);
   REQUIRE(seg6.begin == 1);
   REQUIRE(seg6.end == 10);
   REQUIRE(seg6.value == 5);
}

TEST_CASE("segment_set-construction")
{
   REQUIRE(true);
}

TEST_CASE("segment_set-insert-and-at")
{
   segment_set<double> sset;
   sset.insert(1, 10, 5);
   sset.insert(5, 12, 6);

   SECTION("throw-at-0")
   {
      bool caught = false;
      try { sset.at(0); }
      catch (out_of_range& e) { caught = true; }
      REQUIRE(caught == true);
   }

   SECTION("at-border-positions")
   {
      REQUIRE(sset.at(1) == 5);
      REQUIRE(sset.at(4) == 5);
      REQUIRE(sset.at(5) == 11);
      REQUIRE(sset.at(9) == 11);
      REQUIRE(sset.at(10) == 6);
      REQUIRE(sset.at(11) == 6);
   }

   SECTION("throw-at-12")
   {
      bool caught = false;
      try { sset.at(12); }
      catch (out_of_range& e) { caught = true; }
      REQUIRE(caught == true);
   }
}