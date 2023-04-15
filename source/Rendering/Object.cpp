/**
 * @file Object.cpp
 * @brief Define the interaction code for an object.
 * @date April 14, 2023
 * @copyright Copyright (c) 2023
 */

#include <CityBuilder/Rendering/Object.h>
USING_NS_CITY_BUILDER

Object::Object(Resource<Mesh> mesh, Resource<Material> material)
  : position(0.0), rotation(0, 0, 0, 1), scale(1.0),
    mesh(mesh), material(material) { }



void Object::draw() const {
  // Set the model matrix
  Real4x4 matrix;
  bx::mtxTranslate(matrix, position.x, position.y, position.z);
  Real4x4 rotationMatrix;
  bx::mtxFromQuaternion(rotationMatrix, rotation);
  matrix *= rotationMatrix;
  bx::mtxScale(matrix, scale.x, scale.y, scale.z);
  bgfx::setTransform(matrix);
  
  // Submit the mesh
  mesh->draw(material);
}
