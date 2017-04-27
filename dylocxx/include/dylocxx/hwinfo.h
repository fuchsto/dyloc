#ifndef DYLOCXX__HWINFO_H__INCLUDED
#define DYLOCXX__HWINFO_H__INCLUDED


namespace dyloc {

class hwinfo {

 private:
  dyloc_hwinfo_t _hw;

 public:
  hwinfo();
  ~hwinfo();

  void collect();
};

} // namespace dyloc

#endif // DYLOCXX__HWINFO_H__INCLUDED
