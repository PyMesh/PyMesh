#include "BoundingTree.h"
#include "AABB.h"
#include "exterior_edges.h"
#include "winding_number.h"

#include <igl/read.h>
#include <igl/readNODE.h>
#include <igl/pathinfo.h>
#include <igl/get_seconds_hires.h>
#include <igl/upsample.h>
#include <igl/STR.h>
#include <igl/writeOFF.h>
#include <igl/writeDMAT.h>
#ifdef MEX
#  define IGL_REDRUM_NOOP
#endif
#include <igl/REDRUM.h>
#ifdef MEX
#  include <igl/matlab/mexStream.h>
#endif

// Helpful script for testing and timing:
// find ~/Dropbox/models/ -name \*.off -exec ./winding_number_bounding_tree \
// {} \; 2>/dev/null 1>batch-results.txt

#include <iostream>

// Testing and timing code (may safely ignore)
void test(
  const std::string filename, 
  Eigen::MatrixXd & V, 
  Eigen::MatrixXi & F)
{
  using namespace std;
  using namespace Eigen;
  using namespace igl;
  for(int u = 0;u<10;u++)
  {
    cout<<"'"<<filename<<"' ..."<<endl;

    // Initialize hierarchy
    AABB<Vector3d> hier(V,F);
    //hier.split_method = CENTER_ON_LONGEST_AXIS;
    hier.split_method = MEDIAN_ON_LONGEST_AXIS;
    // Build hierarchy
    hier.grow();
    //hier.print();

#define NUM_RUNS 100

#define SQRT_NUM_RUNS int(sqrt(double(NUM_RUNS)))

    //{
    //  double s = get_seconds_hires();
    //  for(int i = 0;i<SQRT_NUM_RUNS;i++)
    //  {
    //    // Initialize hierarchy
    //    AABB<Vector3d> h(V,F);
    //    // Build hierarchy
    //    h.grow();
    //  }
    //  cout<<"grow: "<<(get_seconds_hires()-s)/SQRT_NUM_RUNS<<endl;
    //}

    //Vector3d p = V.colwise().mean();
    ////Vector3d p;
    ////p<<1.1,1.1,1.1;
    //cout<< "winding_number_all("<<p.transpose()<<"): "<<hier.winding_number_all(p)<<endl;
    //cout<< "winding_number_boundary("<<p.transpose()<<"): "<<hier.winding_number_boundary(p)<<endl;
    //cout<< "winding_number("<<p.transpose()<<"): "<<hier.winding_number(p)<<endl;
    //cout<< "winding_number_approx_simple("<<p.transpose()<<"): "<<hier.winding_number_approx_simple(p,0)<<endl;

    double max_diff = 0;
    for(int i = 0;i<NUM_RUNS;i++)
    {
      Vector3d p = Vector3d(V.colwise().mean()).array() +
        Vector3d::Random().array() * 0.5 *
        Vector3d(V.colwise().maxCoeff() - V.colwise().minCoeff()).array();
      //double diff = fabs(hier.winding_number_all(p)- hier.winding_number(p));
      //cout<<"exact: "<<hier.winding_number_all(p)<<endl;
      //cout<<"approx: "<<hier.winding_number_approx_simple(p,12)<<endl;
      double all = hier.winding_number_all(p);
      //BoundingTree<Vector3d>::method = APPROX_SIMPLE_WINDING_NUMBER_METHOD;
      //BoundingTree<Vector3d>::min_max_w = 0.5;
      //double simple = hier.winding_number(p);
      BoundingTree<Vector3d>::method = EXACT_WINDING_NUMBER_METHOD;
      double exact = hier.winding_number(p);
      //double diff = fabs(all-simple);
      double diff = fabs(all-exact);
      max_diff = (diff>max_diff?diff:max_diff);
    }
    //cout<<"max_diff: "<<max_diff<<endl;

    double t = 0, t_all = 0, t_approx_simple = 0;
    for(int j = 0;j<SQRT_NUM_RUNS;j++)
    {
      Vector3d p = Vector3d(V.colwise().mean()).array() +
        Vector3d::Random().array() * 0.5 *
        Vector3d(V.colwise().maxCoeff() - V.colwise().minCoeff()).array();

      double s;
      s = get_seconds_hires();
      for(int i = 0;i<SQRT_NUM_RUNS;i++)
      {
        hier.winding_number_all(p);
      }
      t_all+=get_seconds_hires()-s;

      BoundingTree<Vector3d>::method = EXACT_WINDING_NUMBER_METHOD;
      s = get_seconds_hires();
      for(int i = 0;i<SQRT_NUM_RUNS;i++)
      {
        hier.winding_number(p);
      }
      t+=get_seconds_hires()-s;
      BoundingTree<Vector3d>::method = EXACT_WINDING_NUMBER_METHOD;

      //BoundingTree<Vector3d>::method = APPROX_SIMPLE_WINDING_NUMBER_METHOD;
      //BoundingTree<Vector3d>::min_max_w = 0.5;
      //s = get_seconds_hires();
      //for(int i = 0;i<SQRT_NUM_RUNS;i++)
      //{
      //  hier.winding_number(p);
      //}
      //t_approx_simple+=get_seconds_hires()-s;
      //BoundingTree<Vector3d>::method = EXACT_WINDING_NUMBER_METHOD;

    }
    //cout<<"winding_number_all: "<<scientific<<t_all/double(SQRT_NUM_RUNS*SQRT_NUM_RUNS)<<"s"<<endl;
    //cout<<"winding_number: "<<scientific<<t/double(SQRT_NUM_RUNS*SQRT_NUM_RUNS)<<"s"<<endl;
    //cout<<"winding_number_all/winding_number: "<<t_all/double(SQRT_NUM_RUNS*SQRT_NUM_RUNS)/(t/double(SQRT_NUM_RUNS*SQRT_NUM_RUNS))<<endl;
    //cout<<"winding_number_approx_simple: "<<t_approx_simple/double(SQRT_NUM_RUNS*SQRT_NUM_RUNS)<<"s"<<endl;
    cout<<scientific<<V.rows()<<" "<<F.rows()<<" "<<
      max_diff<<" "<<
      t_all/double(SQRT_NUM_RUNS*SQRT_NUM_RUNS)<<" "<<
      t/double(SQRT_NUM_RUNS*SQRT_NUM_RUNS)<<"; ..."<<endl;

    upsample(V,F);
  }

}

#ifdef MEX
#else
int main(int argc, char * argv[])
{
#endif
  using namespace std;
  using namespace Eigen;
  using namespace igl;
  MatrixXd V;
  MatrixXi F;

  if(argc <= 1)
  {
    cerr<<"Usage:"<<endl<<
      "  winding_number [path to .off/.obj mesh] [path to .node points]"<<endl;
    cerr<<"or (for timing)"<<endl<<
      "  winding_number [path to .off/.obj mesh]"<<endl;
    return 1;
  }

  string filename(argv[1]);
  if(!read(filename,V,F))
  {
    cerr<<REDRUM("Reading "<<filename<<" failed.")<<endl;
    return false;
  }
  cerr<<GREENGIN("Read "<<filename<<" successfully.")<<endl;

  if(F.cols() != 3)
  {
    // Standalone exectuable only operates on triangle meshes
    cerr<<REDRUM("F.cols() ("<<F.cols()<<" != 3")<<endl;
    return false;
  }

  // Determine whether running a test or just plain w.n. computation
  switch(argc)
  {
    case 2:
    {
      test(filename,V,F);
      break;
    }
    case 3:
    default:
    {
      // Read .node file
      string nodename(argv[2]);
      string prefix;
      MatrixXd O;
      MatrixXi I;
      if(!readNODE(nodename,O,I))
      {
        cerr<<REDRUM("Reading "<<nodename<<" failed.")<<endl;
        return false;
      }
      cerr<<GREENGIN("Read "<<nodename<<" successfully.")<<endl;
      {
        // dirname, basename, extension and filename
        string dirname,b,ext;
        pathinfo(nodename,dirname,b,ext,prefix);
        prefix = dirname + "/" + prefix;
      }
      // Prepare eigen versions of input
      // Initialize hierarchy
      AABB<Vector3d> hier(V,F);
      //hier.split_method = CENTER_ON_LONGEST_AXIS;
      hier.split_method = MEDIAN_ON_LONGEST_AXIS;
      // Build hierarchy
      hier.grow();
      // Be sure correct method is chosen
      BoundingTree<Vector3d>::method = EXACT_WINDING_NUMBER_METHOD;
      // loop over origins
      VectorXd W(O.rows());
      const int no = O.rows();
#   pragma omp parallel for if (no>WINDING_NUMBER_OMP_MIN_VALUE)
      for(int o = 0;o<no;o++)
      {
        Vector3d p = O.row(o);
        W(o) = hier.winding_number(p);
      }
      cout<<"writing winding numbers to "<<(prefix+".dmat")<<endl;
      writeDMAT(prefix+".dmat",W);
      break;
    }
  }
  return 0;
}
