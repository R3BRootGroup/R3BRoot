#ifndef FILTERABLE
#define FILTERABLE

#include <functional>
#include <vector>

template <typename T>
class Filterable
{
  public:
    using Filter = std::function<bool(const T)>;

  private:
    std::vector<Filter> filters;

  public:
    Filterable() = default;
    Filterable(const std::vector<Filter>& vf)
        : filters(vf)
    {
    }
    inline void Add(const Filter f) { filters.push_back(f); }
    inline bool IsValid(const T t) const
    {
        for (const auto& filter : filters)
        {
            if (filter(t) == false)
            {
                return false;
            }
        }
        return true;
    }
};

#endif // FILTERABLE
