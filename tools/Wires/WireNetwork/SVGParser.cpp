#include "SVGParser.h"

#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"

#include <iostream>
#include <Core/Exception.h>

using namespace PyMesh;

namespace SVGParserHelper {

/**
 * The function implements Roger Willcocks' error bound on piecewise linear
 * approximation of cubic Bezier curvers.  See [1] for more detail.
 *
 * [1] Fischer, Kaspar. "Piecewise linear approximation of bezier curves."
 * HTTP://HCKLBRRFNN. WORDPRESS.
 * COM/2012/08/20/PIECEWISE-LINEAR-APPROXIMATION-OF-BEZIER-CURVES/. 2000.
 * http://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.86.162&rep=rep1&type=pdf
 */
Float compute_cubic_bezier_error_bound(
        const Vector2F& p0,
        const Vector2F& p1,
        const Vector2F& p2,
        const Vector2F& p3) {
    const Vector2F u = 3*p1 - 2*p0 - p3;
    const Vector2F v = 3*p2 - p0 - 2*p3;
    return 1.0/16.0 * (
            std::max(u[0]*u[0], v[0]*v[0]) +
            std::max(u[1]*u[1], v[1]*v[1]));
}

}

void SVGParser::parse(const std::string& filename, bool keep_bbox) {
    NSVGimage* image = nsvgParseFromFile(filename.c_str(), "px", 96);
    if (!image) {
        throw IOError("Error: " + filename + " does not exist.");
    }
    const auto width = image->width;
    const auto height = image->height;

    if (keep_bbox) { add_bbox(width, height); }

    for (auto shape = image->shapes; shape != NULL; shape=shape->next) {
        for (auto path = shape->paths; path != NULL; path=path->next) {
            const int v_first = m_vertices.size();
            for (size_t i=0; i<path->npts-1; i+=3) {
                float* p = &path->pts[i*2];
                // SVG's coordinate system is left handed.
                const Vector2F p0{p[0], -p[1]};
                const Vector2F p1{p[2], -p[3]};
                const Vector2F p2{p[4], -p[5]};
                const Vector2F p3{p[6], -p[7]};
                assert(p0.array().isFinite().all());
                assert(p3.array().isFinite().all());
                if (!std::isfinite(p1[0]) || !std::isfinite(p1[1]) ||
                    !std::isfinite(p2[0]) || !std::isfinite(p2[1])) {
                    add_line_segment(p0, p3, i!=0);
                } else {
                    const auto tol = (
                            (p1-p0).norm() +
                            (p2-p1).norm() +
                            (p3-p2).norm()) * 1e-3;
                    add_bezier_curve(p0, p1, p2, p3, tol, 0, i!=0);
                }
            }
            const int v_last = m_vertices.size()-1;
            if (path->closed) {
                m_edges.emplace_back(v_last, v_first);
            }
        }
    }
    nsvgDelete(image);
}


void SVGParser::add_bbox(Float width, Float height) {
    const int vid = m_vertices.size();
    m_vertices.emplace_back(0, 0);
    m_vertices.emplace_back(width, 0);
    m_vertices.emplace_back(width, -height);
    m_vertices.emplace_back(0, -height);
    m_edges.emplace_back(vid+1, vid  );
    m_edges.emplace_back(vid+2, vid+1);
    m_edges.emplace_back(vid+3, vid+2);
    m_edges.emplace_back(vid  , vid+3);
}

void SVGParser::add_bezier_curve(
        const Vector2F& p0,
        const Vector2F& p1,
        const Vector2F& p2,
        const Vector2F& p3, Float tol, int level, bool start_with_previous) {
    if (!p0.array().isFinite().all() || !p1.array().isFinite().all() ||
            !p2.array().isFinite().all() || !p3.array().isFinite().all()) {
        std::cerr << "Nan detected in cubic Bezier control points: " << std::endl;
        std::cerr << "Level " << level << std::endl;
        std::cerr << "  p0: " << p0[0] << ", " << p0[1] << std::endl;
        std::cerr << "  p1: " << p1[0] << ", " << p1[1] << std::endl;
        std::cerr << "  p2: " << p2[0] << ", " << p2[1] << std::endl;
        std::cerr << "  p3: " << p3[0] << ", " << p3[1] << std::endl;
        return;
    }
    const Vector2F p01   = 0.5 * (p0 + p1);
    const Vector2F p12   = 0.5 * (p1 + p2);
    const Vector2F p23   = 0.5 * (p2 + p3);
    const Vector2F p012  = 0.5 * (p01 + p12);
    const Vector2F p123  = 0.5 * (p12 + p23);
    const Vector2F p0123 = 0.5 * (p012 + p123);

    const Float diff =
        SVGParserHelper::compute_cubic_bezier_error_bound(p0, p1, p2, p3);
    assert(std::isfinite(diff));
    if (diff < tol || level >= 6) {
        const int vid = m_vertices.size();

        if (start_with_previous) {
            assert(vid > 0);
            m_vertices.push_back(p0123);
            m_vertices.push_back(p3);
            m_edges.emplace_back(vid-1, vid);
            m_edges.emplace_back(vid, vid+1);
        } else {
            m_vertices.push_back(p0);
            m_vertices.push_back(p0123);
            m_vertices.push_back(p3);

            m_edges.emplace_back(vid, vid+1);
            m_edges.emplace_back(vid+1, vid+2);
        }
    } else {
        add_bezier_curve(p0, p01, p012, p0123, tol, level+1, start_with_previous);
        add_bezier_curve(p0123, p123, p23, p3, tol, level+1, true);
    }
}

void SVGParser::add_line_segment(const Vector2F& p0, const Vector2F& p1,
        bool start_with_previous) {
    const int vid = m_vertices.size();
    if (start_with_previous) {
        m_vertices.push_back(p1);
        m_edges.emplace_back(vid-1, vid);
    } else {
        m_vertices.push_back(p0);
        m_vertices.push_back(p1);
        m_edges.emplace_back(vid, vid+1);
    }
}

void SVGParser::export_vertices(Float* buffer) const {
    size_t i=0;
    for (const auto& v : m_vertices) {
        buffer[i*2] = v[0];
        buffer[i*2+1] = v[1];
        i++;
    }
}

void SVGParser::export_edges(int* buffer) const {
    size_t i=0;
    for (const auto& e : m_edges) {
        buffer[i*2] = e[0];
        buffer[i*2+1] = e[1];
        i++;
    }
}
