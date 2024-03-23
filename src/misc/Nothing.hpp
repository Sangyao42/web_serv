#ifndef NOTHING_HPP
# define NOTHING_HPP

class Nothing
{
  public:
    Nothing();
    Nothing(const Nothing& other);
    ~Nothing();
    Nothing& operator=(const Nothing& other);

    bool  operator==(const Nothing& other) const;

    template <typename T>
    bool  operator==(const T& other) const;
};

template <typename T>
bool  Nothing::operator==(const T& other) const
{
  (void) other;
  return false;
}

#endif