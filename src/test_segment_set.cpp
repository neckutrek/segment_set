#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

#include <iostream>
#include <type_traits>
#include <fstream>
#include <vector>
#include <sstream>
#include <iterator>

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
   SECTION("single-segment")
   {
      segment_set<int> sset;
      REQUIRE(sset.size() == 0);
      REQUIRE(sset.empty());

      sset.insert(4, 10, 5);

      bool caught = false;
      try { sset.at(3); }
      catch (out_of_range& e) { caught = true; }
      REQUIRE(caught == true);

      caught = false;
      try { sset.at(10); }
      catch (out_of_range& e) { caught = true; }
      REQUIRE(caught == true);

      for (int i=4; i<10; ++i)
      {
         REQUIRE(sset.at(i) == 5);
      }
   }

   SECTION("two-segments-1")
   {
      segment_set<int> sset;
      sset.insert(3, 6, 5);
      REQUIRE(sset.size() == 1);
      sset.insert(6, 9, 6);
      REQUIRE(sset.size() == 2);
      for (int i=3; i<6; ++i) REQUIRE(sset.at(i) == 5);
      for (int i=6; i<9; ++i) REQUIRE(sset.at(i) == 6);
   }

   SECTION("two-segments-2")
   {
      segment_set<int> sset;
      sset.insert(3, 6, 5);
      REQUIRE(sset.size() == 1);
      sset.insert(2, 7, 6);
      REQUIRE(sset.size() == 3);

      REQUIRE(sset.at(2) == 6);
      for (int i=3; i<6; ++i) REQUIRE(sset.at(i) == 11);
      REQUIRE(sset.at(6) == 6);
   }

   SECTION("two-segments-3")
   {
      segment_set<int> sset;
      sset.insert(3, 6, 5);
      REQUIRE(sset.size() == 1);
      sset.insert(4, 5, 6);
      REQUIRE(sset.size() == 3);

      REQUIRE(sset.at(3) == 5);
      REQUIRE(sset.at(4) == 11);
      REQUIRE(sset.at(5) == 5);
   }

   SECTION("two-segments-4")
   {
      segment_set<int> sset;
      sset.insert(3, 6, 5);
      REQUIRE(sset.size() == 1);
      sset.insert(5, 8, 6);
      REQUIRE(sset.size() == 3);

      REQUIRE(sset.at(3) == 5);
      REQUIRE(sset.at(4) == 5);
      REQUIRE(sset.at(5) == 11);
      REQUIRE(sset.at(6) == 6);
      REQUIRE(sset.at(7) == 6);
   }

   SECTION("two-segments-5")
   {
      segment_set<int> sset;
      sset.insert(3, 6, 5);
      REQUIRE(sset.size() == 1);
      sset.insert(2, 5, 6);
      REQUIRE(sset.size() == 3);

      REQUIRE(sset.at(2) == 6);
      REQUIRE(sset.at(3) == 11);
      REQUIRE(sset.at(4) == 11);
      REQUIRE(sset.at(5) == 5);
   }

   SECTION("three-segments-1")
   {
      segment_set<int> sset;
      sset.insert(3, 6, 5);
      sset.insert(8, 10, 6);
      sset.insert(6, 9, 7);
      REQUIRE(sset.size() == 4);
      REQUIRE(sset.at(3) == 5);
      REQUIRE(sset.at(4) == 5);
      REQUIRE(sset.at(5) == 5);
      REQUIRE(sset.at(6) == 7);
      REQUIRE(sset.at(7) == 7);
      REQUIRE(sset.at(8) == 13);
      REQUIRE(sset.at(9) == 6);
   }

   SECTION("three-segments-unordered")
   {
      segment_set<int> sset;
      sset.insert(8, 10, 5);
      sset.insert(5, 7, 6);
      sset.insert(1, 4, 7);
      ostringstream oss;
      oss << sset;
      REQUIRE(oss.str().compare(R"(1,4,7)
(5,7,6)
(8,10,5)") == 0);
   }
}

TEST_CASE("rnd")
{
   segment_set<long> sset;

   ifstream ifs("../data/input.txt");
   string line = "";
   while (getline(ifs, line))
   {
      vector<int> input;
      istringstream iss(line);
      for_each(
         istream_iterator<string>{iss},
         istream_iterator<string>{},
         [&input](const string& str){ input.push_back(stoi(str)); }
      );

      sset.insert(input[0], input[1], input[2]);
   }
   ifs.close();

   ofstream ofs("../data/output.txt");
   ofs << sset << endl;
   ofs.close();

   long max = sset.find_max();
   REQUIRE(max == 8628l);
}