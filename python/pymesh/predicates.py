import PyPredicates

"""
This module wraps the exact predicates Jonathan Richard Shewchuk.
"""

# The init function would be called when predicates module is imported.
PyPredicates.exactinit();

def orient_2D(p1, p2, p3):
    """ Determine the orientation 2D points p1, p2, p3

    Args:
        p1,p2,p3: 2D points.

    Returns:
        positive if (p1, p2, p3) is in counterclockwise order.
        negative if (p1, p2, p3) is in clockwise order.
        0.0 if they are collinear.
    """
    return PyPredicates.orient2d(p1, p2, p3);

def orient_3D(p1, p2, p3, p4):
    """ Determine the orientation 3D points p1, p2, p3, p4.

    Args:
        p1,p2,p3,p4: 3D points.

    Returns:
        positive if p4 is below the plane formed by (p1, p2, p3).
        negative if p4 is above the plane formed by (p1, p2, p3).
        0.0 if they are coplanar.
    """
    return PyPredicates.orient3d(p1, p2, p3, p4);

def in_circle(p1, p2, p3, p4):
    """ Determine if p4 is in the circle formed by p1, p2, p3.

    Args:
        p1,p2,p3,p4: 2D points.  ``orient_2D(p1, p2, p3)`` must be postive,
            otherwise the result will be flipped.

    Returns:
        positive p4 is inside of the circle.
        negative p4 is outside of the circle.
        0.0 if they are cocircular.
    """
    return PyPredicates.incircle(p1, p2, p3, p4);

def in_sphere(p1, p2, p3, p4, p5):
    """ Determine if p5 is in the sphere formed by p1, p2, p3, p4.

    Args:
        p1,p2,p3,p4,p5: 3D points.  ``orient_3D(p1, p2, p3, p4)`` must be
            positive, otherwise the result will be flipped.

    Returns:
        positive p5 is inside of the sphere.
        negative p5 is outside of the sphere.
        0.0 if they are cospherical.
    """
    return PyPredicates.insphere(p1, p2, p3, p4, p5);
