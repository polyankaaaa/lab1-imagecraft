#include "filter.h"

class SharpenFilter : public MatrixFilter {
public:
    SharpenFilter()
        : MatrixFilter({
              {0.0, -1.0, 0.0},
              {-1.0, 5.0, -1.0},
              {0.0, -1.0, 0.0},
          }) {}
};