#pragma once
#include "BoundingTree.h"

enum SplitMethod
{
  CENTER_ON_LONGEST_AXIS = 0,
  MEDIAN_ON_LONGEST_AXIS = 1,
  NUM_SPLIT_METHODS = 3
};

template <typename Point>
class AABB : public BoundingTree<Point>
{
  protected:
    Point min_corner;
    Point max_corner;
    double total_positive_area;
  public: 
    SplitMethod split_method;
  public:
    AABB(
      const Eigen::MatrixXd & V,
      const Eigen::MatrixXi & F);
    AABB(
      const BoundingTree<Point> & parent,
      const Eigen::MatrixXi & F);
    // Initialize some things
    void init();
    bool inside(const Point & p) const;
    virtual void grow();
    // Compute min and max corners
    void compute_min_max_corners();
    double max_abs_winding_number(const Point & p) const;
    double max_simple_abs_winding_number(const Point & p) const;
};
