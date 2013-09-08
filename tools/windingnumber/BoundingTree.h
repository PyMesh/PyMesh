#ifndef BOUNDINGTREE_H
#define BOUNDINGTREE_H
#include <list>
#include <Eigen/Dense>
enum WindingNumberMethod
{
  EXACT_WINDING_NUMBER_METHOD = 0,
  APPROX_SIMPLE_WINDING_NUMBER_METHOD = 1,
  NUM_WINDING_NUMBER_METHODS = 2
};

// Templates:
//   Point  type for points in space, e.g. Eigen::Vector3d
template <typename Point>
class BoundingTree
{
  public:
    // Method to use (see enum above)
    static WindingNumberMethod method;
    static double min_max_w;
  protected:
    const BoundingTree * parent;
    std::list<BoundingTree * > children;
    //// List of boundary edges (recall edges are vertices in 2d)
    //const Eigen::MatrixXi boundary;
    // Base mesh vertices
    Eigen::MatrixXd & V;
    // Base mesh vertices with duplicates removed
    Eigen::MatrixXd SV;
    // Facets in this bounding volume
    Eigen::MatrixXi F;
    // Tesselated boundary curve
    Eigen::MatrixXi cap;
  public:
    // For root
    BoundingTree(
      const Eigen::MatrixXd & V,
      const Eigen::MatrixXi & F);
    // For chilluns 
    BoundingTree(
      const BoundingTree<Point> & parent,
      const Eigen::MatrixXi & F);
    virtual ~BoundingTree();
  public:
    const Eigen::MatrixXd & getV() const;
    const Eigen::MatrixXi & getF() const;
    const Eigen::MatrixXi & getcap() const;
    // Grow the Tree recursively
    virtual void grow();
    // Determine whether a given point is inside the bounding 
    //
    // Inputs:
    //   p  query point 
    // Returns true if the point p is inside this bounding volume
    virtual bool inside(const Point & p) const;
    // Compute the (partial) winding number of a given point p
    // According to method
    //  
    // Inputs:
    //   p  query point 
    // Returns winding number 
    double winding_number(const Point & p) const;
    // Same as above, but always computes winding number using exact method
    // (sum over every facet)
    double winding_number_all(const Point & p) const;
    // Same as above, but always computes using sum over tesslated boundary
    double winding_number_boundary(const Point & p) const;
    //// Same as winding_number above, but if max_simple_abs_winding_number is
    //// less than some threshold min_max_w just return 0 (colloquially the "fast
    //// multipole method)
    ////
    ////
    //// Inputs:
    ////   p  query point 
    ////   min_max_w  minimum max simple w to be processed
    //// Returns approximate winding number
    //double winding_number_approx_simple(
    //  const Point & p, 
    //  const double min_max_w);
    // Print contents of Tree
    //
    // Optional input:
    //   tab  tab to show depth
    void print(const char * tab="");
    // Determine max absolute winding number
    //
    // Inputs:
    //   p  query point 
    // Returns max winding number of 
    virtual double max_abs_winding_number(const Point & p) const; 
    // Same as above, but stronger assumptions on (V,F). Assumes (V,F) is a
    // simple polyhedron
    virtual double max_simple_abs_winding_number(const Point & p) const;

};
#endif

