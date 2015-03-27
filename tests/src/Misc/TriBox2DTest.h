/* This file is part of PyMesh. Copyright (c) 2015 by Qingnan Zhou */
#pragma once
#include <Misc/TriBox2D.h>

class TriBox2DTest : public ::testing::Test {
};

TEST_F(TriBox2DTest, simple) {
    //   .          
    //   : .        
    //   :   .      
    // +-:-+   .    
    // | :......:.  
    // +---+        
    const Float center[2] = {-0.0, 0.0};
    const Float size[2] = {0.25, 0.25};
    const Float tri[3][2] = {
        {0.0, 0.0},
        {1.0, 0.0},
        {0.0, 1.0}
    };
    ASSERT_EQ(1, TriBox2D::triBoxOverlap(center, size, tri));
}

TEST_F(TriBox2DTest, simple2) {
    //       .          
    //       : .        
    //       :   .      
    // +---+ :     .    
    // |   | :......:.  
    // +---+            
    const Float center[2] = {-1.0, 0.0};
    const Float size[2] = {0.25, 0.25};
    const Float tri[3][2] = {
        {0.0, 0.0},
        {1.0, 0.0},
        {0.0, 1.0}
    };
    ASSERT_EQ(0, TriBox2D::triBoxOverlap(center, size, tri));
}

TEST_F(TriBox2DTest, simple3) {
    //       +---+      
    // .     |   | 
    // : .   +---+     
    // :   .          
    // :     .    
    // :......:.  
    //            
    const Float center[2] = {1.0, 1.0};
    const Float size[2] = {0.25, 0.25};
    const Float tri[3][2] = {
        {0.0, 0.0},
        {1.0, 0.0},
        {0.0, 1.0}
    };
    ASSERT_EQ(0, TriBox2D::triBoxOverlap(center, size, tri));
}

TEST_F(TriBox2DTest, simple4) {
    // .           
    // : .             
    // :   .          
    // :     .---+    
    // :......:. |
    //       +---+
    const Float center[2] = {1.0, 0.0};
    const Float size[2] = {0.25, 0.25};
    const Float tri[3][2] = {
        {0.0, 0.0},
        {1.0, 0.0},
        {0.0, 1.0}
    };
    ASSERT_EQ(1, TriBox2D::triBoxOverlap(center, size, tri));
}

TEST_F(TriBox2DTest, simple5) {
    // .           
    // : .---+         
    // : | . |        
    // : +---.        
    // :......:.  
    //            
    const Float center[2] = {0.5, 0.5};
    const Float size[2] = {0.25, 0.25};
    const Float tri[3][2] = {
        {0.0, 0.0},
        {1.0, 0.0},
        {0.0, 1.0}
    };
    ASSERT_EQ(1, TriBox2D::triBoxOverlap(center, size, tri));
}

TEST_F(TriBox2DTest, negative_orientation) {
    //       +---+      
    // .     |   | 
    // : .   +---+     
    // :   .          
    // :     .    
    // :......:.  
    //            
    const Float center[2] = {1.0, 1.0};
    const Float size[2] = {0.25, 0.25};
    const Float tri[3][2] = {
        {1.0, 0.0},
        {0.0, 0.0},
        {0.0, 1.0}
    };
    ASSERT_EQ(0, TriBox2D::triBoxOverlap(center, size, tri));
}

TEST_F(TriBox2DTest, degenerate_box) {
    //                  
    // .      (*)  
    // : .             
    // :   .          
    // :     .    
    // :......:.  
    //            
    const Float center[2] = {1.0, 1.0};
    const Float size[2] = {0.0, 0.0};
    const Float tri[3][2] = {
        {1.0, 0.0},
        {0.0, 0.0},
        {0.0, 1.0}
    };
    ASSERT_EQ(0, TriBox2D::triBoxOverlap(center, size, tri));
}

TEST_F(TriBox2DTest, degenerate_box_2) {
    // .           
    // : .             
    // :  (*)         
    // :     .        
    // :......:.  
    //            
    const Float center[2] = {0.5, 0.5};
    const Float size[2] = {0.0, 0.0};
    const Float tri[3][2] = {
        {0.0, 0.0},
        {1.0, 0.0},
        {0.0, 1.0}
    };
    ASSERT_EQ(1, TriBox2D::triBoxOverlap(center, size, tri));
}

TEST_F(TriBox2DTest, degenerate_triangle) {
    //             
    //   +---+             
    //   |   |        
    //   +---+        
    //(.)          
    //            
    const Float center[2] = {0.5, 0.5};
    const Float size[2] = {0.25, 0.25};
    const Float tri[3][2] = {
        {0.0, 0.0},
        {0.0, 0.0},
        {0.0, 0.0}
    };
    ASSERT_EQ(0, TriBox2D::triBoxOverlap(center, size, tri));
}

TEST_F(TriBox2DTest, degenerate_triangle_2) {
    //             
    //   +---+             
    //   | . |        
    //   .---+        
    // .           
    //            
    const Float center[2] = {0.5, 0.5};
    const Float size[2] = {0.25, 0.25};
    const Float tri[3][2] = {
        {0.0, 0.0},
        {0.0, 0.0},
        {0.5, 0.5}
    };
    ASSERT_EQ(1, TriBox2D::triBoxOverlap(center, size, tri));
}
