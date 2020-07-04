
#ifndef _LIBCPP_SEGMENT_SET
#define _LIBCPP_SEGMENT_SET

#include <algorithm>
#include <list>

namespace std
{

template <typename T>
struct segment
{
   segment()
   : begin(0), end(1), value(0)
   {}

   ~segment() = default;

   template <typename U>
   segment(size_t begin, size_t end, const U& value)
   : begin(begin), end(end), value(value)
   {
      this->end = max(1ul, end);
   }

   template <typename U>
   segment(size_t begin, size_t end, U&& value)
   : begin(begin), end(end), value(value)
   {
      this->end = max(1ul, end);
   }

   template <typename U>
   segment(const segment<U>& other)
   : begin(other.begin), end(other.end), value(other.value)
   {}

   template <typename U>
   segment(segment<U>&& other)
   : begin(other.begin), end(other.end), value(other.value)
   {}

   template <typename U>
   segment<T> operator=(const segment<U>& other)
   {
      return segment<T>(other.begin, other.end, other.value);
   }

   template <typename U>
   segment<T> operator=(segment<U>&& other)
   {
      return segment<T>(other.begin, other.end, forward(other.value));
   }

   size_t begin;
   size_t end;
   T value;
};

template <typename T>
ostream& operator<<(ostream& os, const segment<T>& seg)
{
   os << '(' << seg.begin << ',' << seg.end << ',' << seg.value << ')';
   return os;
}

template <typename T>
class segment_set
{
public:
   class iterator {};
   class const_iterator {};

   T find_max()
   {
       T maxval = 0;
       for_each(segments.begin(), segments.end(), [&maxval](const segment<T>& seg){
           maxval = max(maxval, seg.value);
       });
       return maxval;
   }

   [[nodiscard]] inline bool empty() const noexcept
   {
      return segments.empty();
   }

   inline size_t size() const noexcept
   {
      return segments.size();
   }

   //void clear() noexcept;

   //iterator insert(size_t pos, const T& value);
   //iterator insert(size_t pos, T&& value);
   void insert(size_t begin, size_t end, const T& value);
   //iterator insert(size_t begin, size_t end, T&& value);

   T at(size_t pos);
   //const T& at(size_t pos) const;

   //T& operator[](size_t pos);
   //const T& operator[](size_t pos) const;

   //template <typename U>
   //segment_set& operator+=(const segment<U>& segment);
   //segment_set& operator+=(segment&& segment);

private:

   // segments is a list of segment<T> where the segments never overlap
   // and are sorted on their beginning
   list< segment<T> > segments;

   template <typename U>
   friend ostream& operator<<(ostream& os, const segment_set<U>& sset);

};

template <typename T>
ostream& operator<<(ostream& os, const segment_set<T>& sset)
{
   for (const auto& seg : sset.segments)
   {
      os << seg << endl;
   }
   return os;
}

template <typename T>
void
segment_set<T>::insert(size_t begin, size_t end, const T& value)
{
   constexpr auto op = [](const T& t1, const T& t2) -> T {
      return t1 + t2;
   };

   if (segments.empty())
   {
      segments.push_back( segment<T>(begin, end, value) );
      return;
   }
   
   list< segment<T> > new_segments;

   size_t internal_begin = begin;

   auto itr = segments.begin();
   auto itr_end = segments.end();
   while (itr != itr_end && end > itr->begin)
   {
      if (internal_begin >= itr->end)
      {
         ++itr;
         continue;
      }

      if (internal_begin < itr->begin)
      {
         new_segments.push_back( segment<T>(internal_begin, itr->begin, value) );
         internal_begin = itr->begin;
      }

      bool erase_this_itr = false;
      size_t break1 = (internal_begin > itr->begin ? internal_begin : 0);
      size_t break2 = (end < itr->end ? end : 0);

      if (break1 > 0)
      {
         new_segments.push_back( segment<T>(itr->begin, internal_begin, itr->value) );
         erase_this_itr = true;
      }

      if (break2 > 0)
      {
         new_segments.push_back( segment<T>(internal_begin, end, op(itr->value, value)) );
         new_segments.push_back( segment<T>(end, itr->end, itr->value) );
         erase_this_itr = true;
      }
      else if (break1 > 0)
      {
         new_segments.push_back( segment<T>(internal_begin, itr->end, op(itr->value, value)) );
      }

      internal_begin = itr->end;

      if (erase_this_itr)
      {
         segments.erase(itr++);
      }
      else
      {
         itr->value = op(itr->value, value);
         ++itr;
      }
   }

   if (internal_begin < end)
   {
      new_segments.push_back( segment<T>(internal_begin, end, value) );
   }

   segments.splice(segments.end(), new_segments);
}

template <typename T>
T
segment_set<T>::at(size_t pos)
{
   for (const auto& seg : segments)
   {
      if (pos >= seg.begin && pos < seg.end)
      {
         return seg.value;
      }
   }
   throw out_of_range("No segment at pos " + to_string(pos));
}

/*
template <typename T> template <typename U>
segment_set<T>&
segment_set<T>::operator+=(const segment<U>& segment)
{

}
*/

} // namespace std

#endif
