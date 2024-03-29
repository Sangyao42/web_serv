#ifndef NOTHING_HPP
# define NOTHING_HPP

class Nothing
{
  public:
    Nothing();
    Nothing(const Nothing& other);
    ~Nothing();
    Nothing& operator=(const Nothing&);

    bool  operator==(const Nothing&) const;

    template <typename T>
    bool  operator==(const T&) const;
};

template <typename T>
bool  Nothing::operator==(const T&) const
{
  return false;
}

#endif
