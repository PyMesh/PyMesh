% SELFINTERSECT Given a triangle mesh (V,F) compute a new mesh (VV,FF) which is the same as
% (V,F) except that any self-intersecting triangles in (V,F) have been
% subdivided (new vertices and face created) so that the self-intersection
% contour lies exactly on edges in (VV,FF). New vertices will appear in
% original faces or on original edges. New vertices on edges are "merged" only
% across original faces sharing that edge. This means that if the input
% triangle mesh is a closed manifold the output will be too.
% 
% [VV,FF,IF] = selfintersect(V,F,'ParameterName',ParameterValue, ...)
%
% Inputs:
%   V  #V by 3 list of vertex positions
%   F  #F by 3 list of triangle indices into V
%   Optional:
%     'DetectOnly'  followed by bool. Whether to only detect intersecting pairs
%       (sets IF but not VV or FF)
% Outputs:
%   VV  #VV by 3 list of vertex positions
%   FF  #FF by 3 list of triangle indices into V
%   IF  #intersecting face pairs by 2  list of intersecting face pairs,
%     indexing F
%

% See selfintersect.h, selfintersect.cpp for mex implementation
