#ifndef REPROJECTPOINTSONMESH_HPP
#define REPROJECTPOINTSONMESH_HPP

#include "libslic3r/Point.hpp"
#include "SupportPoint.hpp"
#include "Hollowing.hpp"
#include "EigenMesh3D.hpp"
#include "libslic3r/Model.hpp"

#include <tbb/parallel_for.h>

namespace Slic3r { namespace sla {

template<class Pt> Vec3d pos(const Pt &p) { return p.pos.template cast<double>(); }
template<class Pt> void pos(Pt &p, const Vec3d &pp) { p.pos = pp.cast<float>(); }

template<class PointType>
void reproject_support_points(const EigenMesh3D &mesh, std::vector<PointType> &pts)
{
    tbb::parallel_for(size_t(0), pts.size(), [&mesh, &pts](size_t idx) {
        int junk;
        Vec3d new_pos;
        mesh.squared_distance(pos(pts[idx]), junk, new_pos);
        pos(pts[idx], new_pos);
    });
}

inline void reproject_points_and_holes(ModelObject *object)
{
    bool has_sppoints = !object->sla_support_points.empty();

    // Disabling reprojection of holes as they have a significant offset away
    // from the model body which tolerates minor geometrical changes.
    //
    // TODO: uncomment and ensure the right offset of the hole points if
    // reprojection would still be necessary.
    // bool has_holes    = !object->sla_drain_holes.empty();

    if (!object || (/*!has_holes &&*/ !has_sppoints)) return;

    TriangleMesh rmsh = object->raw_mesh();
    rmsh.require_shared_vertices();
    EigenMesh3D emesh{rmsh};

    if (has_sppoints)
        reproject_support_points(emesh, object->sla_support_points);

//    if (has_holes)
//        reproject_support_points(emesh, object->sla_drain_holes);
}

}}
#endif // REPROJECTPOINTSONMESH_HPP
