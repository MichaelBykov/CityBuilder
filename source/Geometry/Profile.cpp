/**
 * @file Profile.cpp
 * @brief Implement the profile mesh.
 * @date April 12, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Geometry/Profile.h>
USING_NS_CITY_BUILDER

ProfileMesh::ProfileMesh(const List<ProfilePoint> &points) {
  for (intptr_t i = 0; i < points.count(); i++) {
    const ProfilePoint &point = points[i];
    switch (point.type) {
      case ProfilePoint::Type::move:
        // Add a single vertex
        vertices.append({ point.position, point.normal0, point.uv0 });
        if (i + 1 < points.count()) {
          if (i == 0 ||
            points[i].type != ProfilePoint::Type::move ||
            i > 0 && points[i - 1].type == ProfilePoint::Type::move)
            // Connect with the next vertex
            triangles.append(vertices.count() - 1).append(vertices.count());
        }
        break;
      
      case ProfilePoint::Type::disjoint:
        // Add two vertices
        vertices.append({ point.position, point.normal0, point.uv0 });
        vertices.append({ point.position, point.normal1, point.uv0 });
        
        // Connect with the next vertex
        triangles.append(vertices.count() - 1).append(vertices.count());
        break;
      
      case ProfilePoint::Type::connected:
        // Add a single vertex
        vertices.append({ point.position, point.normal0, point.uv0 });
        
        // Connect with the next vertex
        triangles.append(vertices.count() - 1).append(vertices.count());
        break;
    }
  }
  
  // Normalize all the vertices
  for (Vertex &vertex : vertices)
    vertex.normal = vertex.normal.normalized();
  
  // Find the dimensions
  for (Vertex &vertex : vertices) {
    if (vertex.position.x > dimensions.x)
      dimensions.x = vertex.position.x;
    if (vertex.position.y > dimensions.y)
      dimensions.y = vertex.position.y;
  }
}
